////////////////////////////////////////////////////////////////////////////////
// Filename:    BaseThread.h
// Description: This file declares the BaseThread class.
//
// Created:     2012-08-26 22:54:30
// Author:      Jacob Buysse
////////////////////////////////////////////////////////////////////////////////
#pragma once
#include <thread>
#include <CRL/CThread.h>

//NOTE: This class originally used the std::thread internally to spawn the thread
//		and join it again.  However, upon heavy usage the following fault occurs:
//		libstdc++-6!execute_native_thread_routine segmentation fault.
//		This could be reproduced by running a project with 100+ unit tests over and
//		over.  Eventually the fault would occur.  Attaching gdb to inspect yielded
//		no additional information.  The MinGW GCC port of <thread> for windows is
//		most likely just not fully mature yet.  So for now I have switched it back
//		to using the CRL/CThread class (internally using _begintrheadex for proper
//		c-runtime initialization).
class BaseThread : public PTR::CThread
{
public:
	BaseThread() = default;
	BaseThread(const BaseThread& rhs) = delete;
	virtual ~BaseThread();

	BaseThread& operator=(const BaseThread& rhs) = delete;

	virtual void Run() = 0;

	void Start();
	void Stop();

private:
	unsigned int OnRun() override;
};

