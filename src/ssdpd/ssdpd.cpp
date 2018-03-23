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

static const char* RESPONSE_URL =
		"http://127.0.0.1/upnp/basic_dev.cgi?ssdp=response";

static const int REPEAT_AFTER = 1800;

#define PORT (1900)
#define UPNP_MCAST_ADDR ("239.255.255.250")

static std::string serial;
static std::string uuid;
struct in_addr getOwnIP(std::string interface);

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

static std::string get_header_field(const std::string& s, std::string f) {
	f += ":";
	std::string::size_type left = 0;
	std::string::size_type right = 0;
	while (right != std::string::npos) {
		std::string::size_type right = s.find("\x0d\x0a", left);
		if (right == left)
			return "";
		std::string line = s.substr(left, right - left);
		if (strcasecmp(line.substr(0, f.size()).c_str(), f.c_str()) == 0) {
			left = line.find_first_not_of(" \"", f.size());
			if (left == std::string::npos)
				return "";
			right = line.find_last_not_of(" \"");
			if (right != std::string::npos)
				right++;
			return line.substr(left, right - left);
		}
		left = right + 2;
	}
	return "";
}

static void fetch_uuid(const std::string& msg) {
	std::string usn = get_header_field(msg, "USN");
	std::string::size_type left = msg.find("uuid:");
	std::string::size_type right = msg.find("::");
	if (left != std::string::npos)
		uuid = msg.substr(left, right - left);
}

void send_udp(int s, struct sockaddr_in* sa, std::string response, int mx) {
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
void sendRespose(int socke, struct sockaddr_in* sa, int mx) {
	std::string ip = inet_ntoa(getOwnIP("eth0"));
	std::string resposeMsg = "HTTP/1.1 200 OK\r\n";
	resposeMsg += "CACHE-CONTROL:max-age=5000\r\n";
	resposeMsg += "EXT:\r\n";
	resposeMsg += "LOCATION:http://" + ip
			+ "/upnp/basic_dev.cgi\r\n";
	resposeMsg += "SERVER:HomeMatic\r\n";
	resposeMsg += "ST:upnp:rootdevice";
	resposeMsg += "USN:uuid:upnp-BasicDevice-1_0-" + serial
			+ "::upnp:rootdevice\r\n\r\n";
	send_udp(socke, sa, resposeMsg, mx);
}
void sendAlive(int sock, struct sockaddr_in* sa, int mx) {
	std::string ip = inet_ntoa(getOwnIP("eth0"));
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
struct in_addr getOwnIP(std::string interface) {
	struct ifreq ifa;
	struct sockaddr_in *i;
	i = (struct sockaddr_in*) &ifa.ifr_addr;
	int fd;
	if (interface.size() >= sizeof(ifa.ifr_name)) {
		i->sin_addr.s_addr = inet_addr("0.0.0.0");
		return i->sin_addr;
	}
	strcpy(ifa.ifr_name, interface.c_str());
	fd = socket(AF_INET, SOCK_DGRAM, 0);
	if (fd == -1) {
		i->sin_addr.s_addr = inet_addr("0.0.0.0");
		return i->sin_addr;
	}
	ioctl(fd, SIOCGIFADDR, &ifa);
	i = (struct sockaddr_in*) &ifa.ifr_addr;
	return i->sin_addr;
}
std::string getSerialNum() {
	char data[1024];
	std::string serialContetn;
	int serialFile = open("/sys/module/plat_eq3ccu2/parameters/board_serial",
			O_RDONLY);
	if (serialFile > 0) {
		size_t readDataSize = read(serialFile, data, 1024);
		if(readDataSize >0)
		{
			serialContetn = data;
		}
		close(serialFile);
	}
	int pos = serialContetn.find('\n');
	if(pos != -1)
	{
		serialContetn.erase(pos,1);
	}
	return serialContetn;

}
int main(int argc, char ** argv) {
	char buffer[2048];

	int mcast_server_socket;
	int cnt;
	struct sockaddr_in sa_mcast_server;
	struct sockaddr_in sa_mcast_client;
	struct sockaddr_in sa_peer;
	struct ip_mreq imr;
	serial = getSerialNum();
	srand(time(NULL));
	mcast_server_socket = socket(PF_INET, SOCK_DGRAM, 0);

	memset(&sa_mcast_server, 0, sizeof(struct sockaddr_in));
	memset(&sa_peer, 0, sizeof(struct sockaddr_in));
	memset(&sa_mcast_client, 0, sizeof(struct sockaddr_in));

	sa_mcast_server.sin_family = AF_INET;
	sa_mcast_server.sin_port = htons(PORT);
	sa_mcast_server.sin_addr.s_addr = htonl(INADDR_ANY );

	sa_peer.sin_family = AF_INET;
	sa_peer.sin_port = htons(PORT);
	sa_peer.sin_addr.s_addr = inet_addr(UPNP_MCAST_ADDR);

	sa_mcast_client.sin_family = AF_INET;
	sa_mcast_client.sin_port = htons(PORT);
	sa_mcast_client.sin_addr.s_addr = inet_addr(UPNP_MCAST_ADDR);

	if (bind(mcast_server_socket, (struct sockaddr *) &sa_mcast_server,
			sizeof(struct sockaddr_in)) < 0) {
		perror("bind");
		exit(1);
	}

	imr.imr_multiaddr.s_addr = inet_addr(UPNP_MCAST_ADDR);
	imr.imr_interface.s_addr = htonl(INADDR_ANY );

	if (setsockopt(mcast_server_socket, IPPROTO_IP, IP_ADD_MEMBERSHIP,
			(void *) &imr, sizeof(struct ip_mreq)) < 0) {
		perror("setsockopt - IP_ADD_MEMBERSHIP");
		exit(1);
	}
	sendAlive(mcast_server_socket,&sa_mcast_client,5);
	time_t t = time(NULL);

	while (1) {

		fd_set inFd, outFd, excFd;
		FD_ZERO(&inFd);
		FD_ZERO(&outFd);
		FD_ZERO(&excFd);

		int maxFd = mcast_server_socket;
		FD_SET(mcast_server_socket, &inFd);

		struct timeval tv;
		tv.tv_sec = REPEAT_AFTER - (time(NULL) - t);
		tv.tv_usec = 0;
		if (tv.tv_sec > REPEAT_AFTER)
			tv.tv_sec = 0;
		if (select(maxFd + 1, &inFd, &outFd, &excFd, &tv) > 0) {
			socklen_t len_r;
			len_r = sizeof(struct sockaddr_in);
			cnt = recvfrom(mcast_server_socket, buffer, sizeof(buffer), 0,
					(struct sockaddr *) &sa_peer, &len_r);
			if (cnt < 0) {
				perror("recvfrom");
				exit(1);
			} else if (cnt == 0) { /* end of transmission */
				break;
			}
			if (strncmp(buffer, "M-SEARCH", 8) == 0) {
				buffer[cnt] = 0;
				std::string request = buffer;
				std::string st_header = get_header_field(request, "ST");
				std::string man_header = get_header_field(request, "MAN");
				if (man_header == "ssdp:discover"
						&& (st_header == "upnp:rootdevice" || st_header == uuid)) {
					int mx = atoi(get_header_field(request, "MX").c_str());
					sendRespose(mcast_server_socket, &sa_peer, mx);
				}
			}
		} else {
			sendAlive(mcast_server_socket,&sa_mcast_client,5);
			t = time(NULL);
		}
	}
	return 0;
}

