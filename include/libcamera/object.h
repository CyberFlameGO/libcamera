/* SPDX-License-Identifier: LGPL-2.1-or-later */
/*
 * Copyright (C) 2019, Google Inc.
 *
 * object.h - Base object
 */
#ifndef __LIBCAMERA_OBJECT_H__
#define __LIBCAMERA_OBJECT_H__

#include <list>
#include <memory>
#include <vector>

#include <libcamera/bound_method.h>

namespace libcamera {

class Message;
template<typename... Args>
class Signal;
class SignalBase;
class Thread;

class Object
{
public:
	Object(Object *parent = nullptr);
	virtual ~Object();

	void postMessage(std::unique_ptr<Message> msg);

	template<typename T, typename R, typename... FuncArgs, typename... Args,
		 typename std::enable_if<std::is_base_of<Object, T>::value>::type * = nullptr>
	R invokeMethod(R (T::*func)(FuncArgs...), ConnectionType type,
		       Args... args)
	{
		T *obj = static_cast<T *>(this);
		auto *method = new BoundMethodMember<T, R, FuncArgs...>(obj, this, func, type);
		return method->activate(args..., true);
	}

	Thread *thread() const { return thread_; }
	void moveToThread(Thread *thread);

	Object *parent() const { return parent_; }

protected:
	virtual void message(Message *msg);

private:
	friend class SignalBase;
	friend class Thread;

	void notifyThreadMove();

	void connect(SignalBase *signal);
	void disconnect(SignalBase *signal);

	Object *parent_;
	std::vector<Object *> children_;

	Thread *thread_;
	std::list<SignalBase *> signals_;
	unsigned int pendingMessages_;
};

} /* namespace libcamera */

#endif /* __LIBCAMERA_OBJECT_H__ */
