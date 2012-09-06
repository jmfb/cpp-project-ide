////////////////////////////////////////////////////////////////////////////////
// Filename:    BaseThread.h
// Description: This file declares the BaseThread class.
//
// Created:     2012-08-26 22:54:30
// Author:      Jacob Buysse
////////////////////////////////////////////////////////////////////////////////
#pragma once
#include <thread>
#include <memory>

class BaseThread
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
	static void RunCallback(BaseThread* instance);

private:
	std::shared_ptr<std::thread> thread;
};

