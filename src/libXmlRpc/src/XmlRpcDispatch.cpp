
/* changed by eQ-3 Entwicklung GmbH 2006 */


#include "XmlRpcDispatch.h"
#include "XmlRpcSource.h"
#include "XmlRpcUtil.h"

#include <errno.h>
#include <math.h>
#include <sys/timeb.h>
#include <time.h>

#if defined(_WINDOWS)
# include <winsock2.h>

# define USE_FTIME
# if defined(_MSC_VER)
#  define timeb _timeb
#  define ftime _ftime
# endif
#else
# include <sys/time.h>
# include <poll.h>
#endif  // _WINDOWS

#include <map>
#include <vector>

#include <asm/bitsperlong.h>

using namespace XmlRpc;


XmlRpcDispatch::XmlRpcDispatch()
{
  _doClear = false;
  _inWork = false;
}


XmlRpcDispatch::~XmlRpcDispatch()
{
}

// Monitor this source for the specified events and call its event handler
// when the event occurs
void
XmlRpcDispatch::addSource(XmlRpcSource* source, unsigned mask)
{
  //avoid double addition
  for (SourceList::iterator it=_sources.begin(); it!=_sources.end(); ++it)
    if (it->getSource() == source)
    {
      it->getMask()=mask;
      return;
    }

  _sources.push_back(MonitoredSource(source, mask));
}

// Stop monitoring this source. Does not close the source.
void
XmlRpcDispatch::removeSource(XmlRpcSource* source)
{
  for (SourceList::iterator it=_sources.begin(); it!=_sources.end(); ++it)
    if (it->getSource() == source)
    {
      _sources.erase(it);
      break;
    }
}


// Modify the types of events to watch for on this source
void 
XmlRpcDispatch::setSourceEvents(XmlRpcSource* source, unsigned eventMask)
{
  for (SourceList::iterator it=_sources.begin(); it!=_sources.end(); ++it)
    if (it->getSource() == source)
    {
      it->getMask() = eventMask;
      break;
    }
}



// Watch current set of sources and process events
bool
XmlRpcDispatch::work(int32_t msTime)
{
	_doClear = false;
	_inWork = true;
	
	
	// Construct the list of descriptors we are interested in. poll() instead of select(): an fd_set only
	// holds descriptors below FD_SETSIZE (1024), and FD_SET() with a larger one writes past the set.
	std::vector<struct pollfd> fds;
	std::map<int, size_t> fdIndex;     // descriptor -> its entry in fds
	bool dataPending = false;
	SourceList::iterator it;
	for (it=_sources.begin(); it!=_sources.end(); ++it) {
		int fd = it->getSource()->getfd();
		unsigned mask=it->getMask()|it->getSource()->getExtraMask();
		if ( ! mask) continue;
		short events = 0;
		if (mask & ReadableEvent) events |= POLLIN;
		if (mask & WritableEvent) events |= POLLOUT;
		if (mask & Exception)     events |= POLLPRI;
		if (mask & DataPending)	  dataPending = true;
		std::map<int, size_t>::iterator idx = fdIndex.find(fd);
		if (idx == fdIndex.end()) {
			struct pollfd pfd;
			pfd.fd = fd;
			pfd.events = events;
			pfd.revents = 0;
			fdIndex[fd] = fds.size();
			fds.push_back(pfd);
		} else {
			fds[idx->second].events |= events;
		}
	}
	
	// Check for events. At least one source with data to be processed at once: just poll the others.
	int timeout = (msTime <= 0) ? -1 : (dataPending ? 0 : msTime);
	int nEvents = poll(fds.empty() ? NULL : &fds[0], fds.size(), timeout);
	
	if (nEvents < 0)
	{
		// A signal (e.g. SIGTERM, whose handler ends the daemon's main loop)
		// interrupts poll(); that is no error worth logging.
		if (errno != EINTR)
			XmlRpcUtil::error("Error in XmlRpcDispatch::work: error in poll (%d).", nEvents);
		_inWork = false;
		return false;
	}

	// Process events
	for (it=_sources.begin(); it != _sources.end(); )
	{
		SourceList::iterator thisIt = it++;
		XmlRpcSource* src = thisIt->getSource();
		std::map<int, size_t>::iterator idx = fdIndex.find(src->getfd());
		unsigned newMask = (unsigned) -1;
		if (idx != fdIndex.end()) {  // a source added while handling events is polled next time
			const struct pollfd& pfd = fds[idx->second];
			short ready = pfd.revents;
			// as with select(), a hang-up or an error makes the descriptor readable and writable,
			// so the source sees it on its next read or write
			if (ready & (POLLHUP | POLLERR | POLLNVAL))
				ready |= pfd.events & (POLLIN | POLLOUT);
			// If you select on multiple event types this could be ambiguous
			unsigned mask=src->getExtraMask();
			if ( mask & DataPending )
			{
				newMask&=src->handleEvent(DataPending);
			}else{
				if (ready & POLLIN){
					newMask&=src->handleEvent(ReadableEvent);
				}
			}
			if (ready & POLLOUT)
				newMask &= src->handleEvent(WritableEvent);
			if (ready & POLLPRI)
				newMask &= src->handleEvent(Exception);
			
			if ( ! newMask) {
				XmlRpcUtil::log(5, "XmlRpcDispatch::work: erasing src %p", src);
				_sources.erase(thisIt);  // Stop monitoring this one
				if ( ! src->getKeepOpen())
					src->close();
			} else if (newMask != (unsigned) -1) {
				thisIt->getMask() = newMask;
			}
		}
	}
	
	// Check whether to clear all sources
	if (_doClear)
	{
		SourceList closeList = _sources;
		_sources.clear();
		for (SourceList::iterator it=closeList.begin(); it!=closeList.end(); ++it) {
			XmlRpcSource *src = it->getSource();
			src->close();
		}
		
		_doClear = false;
	}
	
	_inWork = false;

    return nEvents > 0;
}


// Exit from work routine. Presumably this will be called from
// one of the source event handlers.
void
XmlRpcDispatch::exit()
{
//  _endTime = 0.0;   // Return from work asap
}

// Clear all sources from the monitored sources list
void
XmlRpcDispatch::clear()
{
  if (_inWork)
    _doClear = true;  // Finish reporting current events before clearing
  else
  {
    SourceList closeList = _sources;
    _sources.clear();
    for (SourceList::iterator it=closeList.begin(); it!=closeList.end(); ++it)
      it->getSource()->close();
  }
}

#define NSEC_PER_SEC      1000000000L
#define NSEC_PER_MILLISEC 1000000L
#define MSEC_PER_SEC      1000L

uint64_t
XmlRpcDispatch::getTime()
{
#ifdef USE_FTIME
  struct timeb	tbuff;

  ftime(&tbuff);
  return static_cast<uint64_t>((tbuff.time * MSEC_PER_SEC) + tbuff.millitm + (tbuff.timezone * 60000));
#else
  struct timespec now;
  clock_gettime(CLOCK_REALTIME, &now);
  return now.tv_sec * MSEC_PER_SEC + now.tv_nsec / NSEC_PER_MILLISEC;
#endif /* USE_FTIME */
}
