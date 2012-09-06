////////////////////////////////////////////////////////////////////////////////
// Filename:    BaseThread.cpp
// Description: This file implements all BaseThread member functions.
//
// Created:     2012-08-26 22:54:30
// Author:      Jacob Buysse
////////////////////////////////////////////////////////////////////////////////
#include "BaseThread.h"

BaseThread::~BaseThread()
{
	Stop();
}

void BaseThread::Start()
{
	Stop();
	thread.reset(new std::thread(&RunCallback, this));
}

void BaseThread::Stop()
{
	if (thread)
	{
		thread->join();
		thread.reset();
	}
}

void BaseThread::RunCallback(BaseThread* instance)
{
	instance->Run();
}

