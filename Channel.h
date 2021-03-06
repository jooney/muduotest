/*
 * Channel.h
 *
 *  Created on: 2017��9��24��
 *      Author: jjz
 */

#ifndef CHANNEL_H_
#define CHANNEL_H_
#include "Timestamp.h"
#include <functional>
#include <memory>
#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>
class EventLoop;
using namespace muduo;

class Channel
{
public:
	typedef std::function<void()> EventCallback;
	typedef std::function<void(Timestamp)> ReadEventCallback;
	//typedef std::function<void()>
	Channel(EventLoop* loop, int fd);
	~Channel();

	void handleEvent(Timestamp receiveTime);
	void setReadCallback(const ReadEventCallback& cb)
	{
		_readCallback = cb;
	}
	void setWriteCallback(const EventCallback& cb)
	{
		_writeCallback = cb;
	}
	void setCloseCallback(const EventCallback& cb)
	{
		_closeCallback = cb;
	}

	void setErrorCallback(const EventCallback& cb)
	{
		_errorCallback = cb;
	}
	void tie(const boost::shared_ptr<void>&);
	int fd() const { return _fd; }
	int events() const { return _events; }
	void set_revents(int revt) { _revents = revt; } // used by pollers
	  // int revents() const { return revents_; }
	bool isNoneEvent() const { return _events == kNoneEvent; }
	void enableReading() { _events |= kReadEvent; update(); }
	void disableReading() { _events &= ~kReadEvent; update(); }
	void enableWriting() { _events |= kWriteEvent; update(); }
	void disableWriting() { _events &= ~kWriteEvent; update(); }
	void disableAll() { _events = kNoneEvent; update(); }
	bool isWriting() const { return _events & kWriteEvent; }
	bool isReading() const { return _events & kReadEvent; }
	// for Poller
	int index() { return _index; }
	void set_index(int idx) { _index = idx; }

	// for debug
	string reventsToString() const;
	string eventsToString() const;
	EventLoop* ownerLoop() { return _loop; }
	void remove();
private:
	static string eventsToString(int fd, int ev);
	void update();
	void handleEventWithGuard(Timestamp receiveTime);
	static const int kNoneEvent;
	static const int kReadEvent;
	static const int kWriteEvent;
	EventLoop* _loop;
	const int _fd;
	int _events;
	int _revents;  //it's the received event types of epoll or poll
	int _index;    //used by poller
	ReadEventCallback _readCallback;
	EventCallback  _writeCallback;
	EventCallback  _closeCallback;
	EventCallback  _errorCallback;
	bool   _eventHandling;
	bool   _addedToLoop;
	bool   _tied;
	boost::weak_ptr<void> _tie;
};



#endif /* CHANNEL_H_ */
