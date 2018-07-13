#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string>
#include <set>
#include <map>
#include <strings.h>
#include <string.h>
#include <utils.h>
#include <fstream>
#include <net/if.h>
#include <fcntl.h>
#include <ifaddrs.h>
#include <sstream>
#include <errno.h>

//static const char* RESPONSE_URL =
//		"http://127.0.0.1/upnp/basic_dev.cgi?ssdp=response";

static const int REPEAT_AFTER = 1800;

#define SSDP_PORT 1900
#define SSDP_IP   "239.255.255.250"

static std::string serial;
static std::string uuid;

#if 0
static bool InterfaceByTargetAddress(unsigned long address, std::string* interface)
{
	std::ifstream file(KERNEL_ROUTES);
	char buffer[256];
	if(!file.good())return false;
	//address=ntohl(address);
	//skip the first line
	file.getline(buffer, sizeof(buffer));
	while(file.good() && !file.eof()) {
		char interface_name[64];
		unsigned long dest_address;
		unsigned long mask;
		file.getline(buffer, sizeof(buffer));
		if(sscanf(buffer, "%s %lX %*X %*X %*X %*X %*X %lX", interface_name, &dest_address, &mask)<=0)break;
		if((dest_address&mask)==(address&mask)) {
			*interface=interface_name;
			return true;
		}
	}
	return false;
}
#endif

#if 0
static int ParseURL(const std::string& url, struct sockaddr_in* sa,
		std::string* uri) {
	std::string::size_type left, right;
	std::string host;
	unsigned long port;
	left = 0;
	right = url.find("://", left);
	if (right != std::string::npos) {
//		protocol=url.substr(left, right-left);
		left = right + 3;
	}
	right = url.find_first_of("/:", left);
	host = url.substr(left, right - left);
	if (right != std::string::npos) {
		left = right;
		if (url[left] == ':') {
			left++;
			right = url.find('/', left);
			port = atoi(url.substr(left, right - left).c_str());
		} else {
			port = 80;
		}
	}
	if (right != std::string::npos) {
		left = right;
		*uri = url.substr(left);
	} else {
		*uri = "/";
	}
	sa->sin_family = AF_INET;
	sa->sin_port = htons(port);
	sa->sin_addr.s_addr = inet_addr(host.c_str());
	memset(&(sa->sin_zero), 0, 8);
	return 0;
}
#endif

#if 0
static int GetHTTP(const std::string& url, std::string* response) {
	std::string uri;
	struct sockaddr_in dest_addr;
	ParseURL(url, &dest_addr, &uri);
	static char buffer[1024];
	std::string http_request = "GET " + uri + " HTTP/1.0\x0d\x0a\x0d\x0a";
	int fd = socket(PF_INET, SOCK_STREAM, 0);
	if (fd < 0) {
		perror("socket");
		return fd;
	}
	if (connect(fd, (struct sockaddr*) &dest_addr, sizeof(struct sockaddr))
			< 0) {
		perror("connect");
		close(fd);
		return -1;
	}
	unsigned int index = 0;
	while (index < http_request.size()) {
		int count = send(fd, http_request.c_str() + index,
				http_request.size() - index, 0);
		if (count < 0) {
			perror("send");
			close(fd);
			return -1;
		}
		index += count;
	}
	std::string http_response_head;
	std::string http_response_body;
	int count;
	int content_length = -1;
	while ((count = recv(fd, buffer, sizeof(buffer), 0))) {
		buffer[count] = 0;
		if (content_length < 0) {
			http_response_head.append(buffer, count);
			std::string::size_type pos = http_response_head.find(
					"\x0d\x0a\x0d\x0a");
			if (pos != std::string::npos) {
				http_response_body = http_response_head.substr(pos + 4);
				http_response_head.erase(pos);
				pos = http_response_head.find("Content-Length:");
				if (pos != std::string::npos) {
					content_length = atoi(
							http_response_head.c_str() + pos + 15);
				} else {
					content_length = 0;
				}
			}
		} else {
			http_response_body.append(buffer, count);
		}
		if (content_length && content_length <= (int) http_response_body.size())
			break;
	}
	close(fd);
	if (content_length < 0
			|| content_length > (int) http_response_body.size()) {
		printf("Content-Length mismatch\n");
		return -1;
	}
	if (content_length) {
		*response = http_response_body.substr(0, content_length);
	} else {
		*response = http_response_body;
	}
	return 0;
}
#endif

static std::string get_header_field(const std::string& s, std::string f) {
	std::istringstream split(s);
	std::vector<std::string> tokens;
	for (std::string each; std::getline(split, each, '\n'); ) {
		std::string::size_type right = each.find('\r');
		if (right != std::string::npos)
			tokens.push_back(each.substr(0, right));
		else
			tokens.push_back(each);
	}

	// now use `tokens`
	for (std::vector <std::string>::iterator iter = tokens.begin(); iter != tokens.end(); iter++) {
		std::string line = *iter;
		if (strcasecmp(line.substr(0, f.size()).c_str(), f.c_str()) == 0) {
			std::string::size_type left = line.find_first_not_of(" \"", f.size());
			if (left == std::string::npos)
				return "";
			std::string::size_type right = line.find_last_not_of(" \"");
			if (right != std::string::npos)
				right++;
			return line.substr(left, right - left);
		}
	}
	return "";
}

static void fetch_uuid(const std::string& msg) {
	std::string usn = get_header_field(msg, "USN:");
	std::string::size_type left = msg.find("uuid:");
	std::string::size_type right = msg.find("::");
	if (left != std::string::npos)
		uuid = msg.substr(left, right - left);
}

static void send_udp(int s, struct sockaddr_in* sa, std::string response, int mx) {
	int nb_packets = 0;
	std::string::size_type start = 0;
	while (std::string::npos
			!= (start = response.find("\x0d\x0a\x0d\x0a", start))) {
		nb_packets++;
		start += 4;
	}
	std::set<int> delays;
	while (nb_packets) {
		int delay = rand() / (RAND_MAX / (mx * 1000));
		while (delays.find(delay) != delays.end()) {
			delay = rand() / (RAND_MAX / (mx * 1000));
		}
		delays.insert(delay);
		nb_packets--;
	}
	start = 0;
	std::string::size_type end = response.find("\x0d\x0a\x0d\x0a");
	if (end != std::string::npos)
		end += 4;
	std::set<int>::iterator it = delays.begin();
	unsigned long t = time_millis();
	while (end != std::string::npos) {
		std::string msg = response.substr(start, end - start);
		fetch_uuid(msg);
		start = end;
		end = response.find("\x0d\x0a\x0d\x0a", start);
		if (end != std::string::npos)
			end += 4;
		int delay = t + (*it) - time_millis();
		if (delay > 0)
			usleep(delay * 1000);
		int n;
		n = sendto(s, msg.c_str(), msg.size(), 0, (struct sockaddr *) sa,
				sizeof(struct sockaddr_in));
		if (n < 0) {
			perror("sendto");
		}
		it++;
	}
}

static std::string getIPAddress() {
  std::string ipAddress="0.0.0.0";
  struct ifaddrs *interfaces = NULL;
  struct ifaddrs *temp_addr = NULL;
  int success = 0;
  // retrieve the current interfaces - returns 0 on success
  success = getifaddrs(&interfaces);
  if (success == 0) {
    // Loop through linked list of interfaces
    temp_addr = interfaces;
    while(temp_addr != NULL) {
      // ignore non ipv4, if not up or if a loopback or not running interface
      if(temp_addr->ifa_addr != NULL && temp_addr->ifa_addr->sa_family == AF_INET &&
         (temp_addr->ifa_flags & IFF_UP) == IFF_UP &&
         (temp_addr->ifa_flags & IFF_LOOPBACK) != IFF_LOOPBACK &&
         (temp_addr->ifa_flags & IFF_POINTOPOINT) != IFF_POINTOPOINT &&
         (temp_addr->ifa_flags & IFF_RUNNING) == IFF_RUNNING) {
        ipAddress=inet_ntoa(((struct sockaddr_in*)temp_addr->ifa_addr)->sin_addr);
        break;
      }
      temp_addr = temp_addr->ifa_next;
    }

    // Free memory
    freeifaddrs(interfaces);
  }
  else
    perror("getifaddr() failed");

  return ipAddress;
}

static void sendRespose(int socke, struct sockaddr_in* sa, int mx) {
	std::string ip = getIPAddress();
	std::string resposeMsg = "HTTP/1.1 200 OK\r\n";
	resposeMsg += "CACHE-CONTROL:max-age=5000\r\n";
	resposeMsg += "EXT:\r\n";
	resposeMsg += "LOCATION:http://" + ip
			+ "/upnp/basic_dev.cgi\r\n";
	resposeMsg += "SERVER:HomeMatic\r\n";
	resposeMsg += "ST:upnp:rootdevice\r\n";
	resposeMsg += "USN:uuid:upnp-BasicDevice-1_0-" + serial
			+ "::upnp:rootdevice\r\n\r\n";
	send_udp(socke, sa, resposeMsg, mx);
}
static void sendAlive(int sock, struct sockaddr_in* sa, int mx) {
	std::string ip = getIPAddress();
	std::string aliveMsg = "NOTIFY * HTTP/1.1\r\n";
	aliveMsg += "HOST:239.255.255.250:1900\r\n";
	aliveMsg += "CACHE-CONTROL:max-age=5000\r\n";
	aliveMsg += "LOCATION:http://" + ip	+ "/upnp/basic_dev.cgi\r\n";
	aliveMsg += "NTS:ssdp:alive\r\n";
	aliveMsg += "SERVER:HomeMatic\r\n";
	aliveMsg += "NT:upnp:rootdevice\r\n";
	aliveMsg += "USN:uuid:upnp-BasicDevice-1_0-" + serial
			+ "::upnp:rootdevice\r\n\r\n";
	send_udp(sock,sa,aliveMsg,mx);
}

static std::string getSerialNum() {
	std::string serialContetn = "";
	FILE* serialFile = NULL;
	serialFile = fopen("/var/board_sgtin", "r");
	if (serialFile == NULL)
		serialFile = fopen("/var/board_serial", "r");
	if (serialFile == NULL)
		serialFile = fopen("/sys/module/plat_eq3ccu2/parameters/board_serial", "r");
	if (serialFile != NULL) {
		char *data = NULL;
		size_t dataSize = 0;
		ssize_t readDataSize = getline(&data, &dataSize, serialFile);
		if(readDataSize > 0)
			serialContetn = data;
		fclose(serialFile);
		free(data);
	}
	int pos = serialContetn.find('\n');
	if(pos != -1)
		serialContetn.erase(pos,1);
	return serialContetn;

}
int main(int argc, char ** argv) {
	char buffer[2048];

	struct sockaddr_in sa_mcast_client;
	struct sockaddr_in sa_peer;

	int onOff;
	u_char ttl = (u_char)4;
	int ssdpSock = -1;
	struct ip_mreq ssdpMcastAddr;
	struct sockaddr_storage __ss;
	struct sockaddr_in *ssdpAddr4 = (struct sockaddr_in *)&__ss;
	struct in_addr addr;

	serial = getSerialNum();
	srand(time(NULL));

	ssdpSock = socket(AF_INET, SOCK_DGRAM, 0);
	if (ssdpSock < 0) {
		perror("socket() failed");
		exit(1);
	}

	// set SO_REUSEPORT and SO_REUSEADDR to allow multiple apps to
	// use the same udp port (1900)
	onOff = 1;
	if (setsockopt(ssdpSock, SOL_SOCKET, SO_REUSEADDR, (const char*)&onOff, sizeof(onOff)) < 0) {
		perror("setsockopt(SO_REUSEADDR) failed");
		exit(1);
	}

	// SO_REUSEPORT is only supported since kernel 3.9+, so depending
	// on the toolchain we check for an error or not.
	onOff = 1;
	#ifndef SO_REUSEPORT
	#define SO_REUSEPORT	15
	setsockopt(ssdpSock, SOL_SOCKET, SO_REUSEPORT, (const char*)&onOff, sizeof(onOff));
	#else
	if (setsockopt(ssdpSock, SOL_SOCKET, SO_REUSEPORT, (const char*)&onOff, sizeof(onOff)) < 0) {
		perror("setsockopt(SO_REUSEPORT) failed");
		exit(1);
	}
	#endif

	memset(&__ss, 0, sizeof(__ss));
	ssdpAddr4->sin_family = AF_INET;
	ssdpAddr4->sin_addr.s_addr = htonl(INADDR_ANY);
	ssdpAddr4->sin_port = htons(SSDP_PORT);

	if (bind(ssdpSock, (struct sockaddr *)ssdpAddr4, sizeof(*ssdpAddr4)) == -1) {
		perror("bind failed");
		exit(1);
	}

	memset((void *)&ssdpMcastAddr, 0, sizeof(struct ip_mreq));
	ssdpMcastAddr.imr_interface.s_addr = inet_addr(getIPAddress().c_str());
	ssdpMcastAddr.imr_multiaddr.s_addr = inet_addr(SSDP_IP);
	if (setsockopt(ssdpSock, IPPROTO_IP, IP_ADD_MEMBERSHIP, (char *)&ssdpMcastAddr, sizeof(struct ip_mreq)) == -1) {
		perror("setsockopt(IP_ADD_MEMBERSHIP) failed");
		exit(1);
	}

	/* Set multicast interface. */
	memset((void *)&addr, 0, sizeof(struct in_addr));
	addr.s_addr = inet_addr(getIPAddress().c_str());
	if (setsockopt(ssdpSock, IPPROTO_IP, IP_MULTICAST_IF, (char *)&addr, sizeof addr) == -1) {
		perror("setsockopt(IP_MULTICAST_IF) failed");
		/* This is probably not a critical error, so let's continue. */
	}

	/* result is not checked becuase it will fail in WinMe and Win9x. */
	setsockopt(ssdpSock, IPPROTO_IP, IP_MULTICAST_TTL, &ttl, sizeof(ttl));
	onOff = 1;
	if (setsockopt(ssdpSock, SOL_SOCKET, SO_BROADCAST, (char *)&onOff, sizeof(onOff)) == -1) {
		perror("setsockopt(SO_BROADCAST) failed");
		exit(1);
	}

	memset(&sa_peer, 0, sizeof(struct sockaddr_in));
	sa_peer.sin_family = AF_INET;
	sa_peer.sin_port = htons(SSDP_PORT);
	sa_peer.sin_addr.s_addr = inet_addr(SSDP_IP);

	memset(&sa_mcast_client, 0, sizeof(struct sockaddr_in));
	sa_mcast_client.sin_family = AF_INET;
	sa_mcast_client.sin_port = htons(SSDP_PORT);
	sa_mcast_client.sin_addr.s_addr = inet_addr(SSDP_IP);

	sendAlive(ssdpSock, &sa_mcast_client, 5);
	time_t t = time(NULL);

	while (1) {

		fd_set inFd;
		FD_ZERO(&inFd);
		FD_SET(ssdpSock, &inFd);

		struct timeval tv;
		tv.tv_sec = REPEAT_AFTER - (time(NULL) - t);
		tv.tv_usec = 0;
		if (tv.tv_sec > REPEAT_AFTER || tv.tv_sec < 0)
			tv.tv_sec = 0;

		int ret = select(ssdpSock + 1, &inFd, NULL, NULL, &tv);
		if (ret < 0) {
			perror("select failed");

			// only exit if not EINVAL (invalid argument)
			if(errno != EINVAL)
				break;
		}

		if (ret > 0) {
			socklen_t len_r;
			len_r = sizeof(struct sockaddr_in);
			memset(buffer, 0, sizeof(buffer));
			ssize_t cnt = recvfrom(ssdpSock, buffer, sizeof(buffer) - 1, 0,
					(struct sockaddr *) &sa_peer, &len_r);
			if (cnt < 0) {
				perror("recvfrom failed");
				exit(1);
			} else if (cnt == 0) { /* end of transmission */
				break;
			}
			if(cnt < (ssize_t)sizeof(buffer))
				buffer[cnt] = '\0';

			if (strncmp(buffer, "M-SEARCH", 8) == 0) {
				std::string request = buffer;
				std::string st_header = get_header_field(request, "ST:");
				std::string man_header = get_header_field(request, "MAN:");
				if (man_header == "ssdp:discover"
						&& (st_header == "upnp:rootdevice" || st_header == uuid || st_header == "ssdp:all")) {
					int mx = atoi(get_header_field(request, "MX:").c_str());
					sendRespose(ssdpSock, &sa_peer, mx);
				}
			}
		} else {
			sendAlive(ssdpSock, &sa_mcast_client, 5);
			t = time(NULL);
		}
	}

	return 0;
}
