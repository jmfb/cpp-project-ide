////////////////////////////////////////////////////////////////////////////////
// Filename:    UnitTestThread.h
// Description: This file declares the UnitTestThread class.
//
// Created:     2012-09-13 00:45:24
// Author:      Jacob Buysse
////////////////////////////////////////////////////////////////////////////////
#pragma once
#include "BaseThread.h"
#include "TestResultsTarget.h"
#include <string>
#include <atomic>
#include <memory>

class UnitTestThread : public BaseThread
{
public:
	UnitTestThread();
	UnitTestThread(const UnitTestThread& rhs) = delete;
	~UnitTestThread() = default;

	UnitTestThread& operator=(const UnitTestThread& rhs) = delete;

	void SetTestData(
		unsigned long testIndex,
		const std::string& command,
		const std::string& workingDirectory,
		TestResultsTarget* target);
	bool IsDone();

	void Run() final;

private:
	std::string command;
	std::string workingDirectory;
	unsigned long testIndex = 0;
	TestResultsTarget* target = nullptr;
	std::atomic<bool> done;
};

typedef std::shared_ptr<UnitTestThread> UnitTestThreadPtr;

