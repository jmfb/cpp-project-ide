////////////////////////////////////////////////////////////////////////////////
// Filename:    TestManagerThread.h
// Description: This file declares the TestManagerThread class.
//
// Created:     2012-09-13 00:43:46
// Author:      Jacob Buysse
////////////////////////////////////////////////////////////////////////////////
#pragma once
#include "BaseThread.h"
#include "UnitTestThread.h"
#include "TestResultsTarget.h"
#include <string>
#include <memory>
#include <vector>
#include <atomic>
#include <deque>

class TestManagerThread : public BaseThread
{
public:
	TestManagerThread() = default;
	TestManagerThread(const TestManagerThread& rhs) = delete;
	~TestManagerThread() = default;

	TestManagerThread& operator=(const TestManagerThread& rhs) = delete;

	void AddUnitTest(unsigned long testIndex, const std::string& command);
	void RunTests(const std::string& workingDirectory, TestResultsTarget* target);
	bool IsDone();
		
	void Run() final;

private:
	void TidyWorkers();

private:
	struct TestData
	{
		unsigned long testIndex;
		std::string command;
	};
	TestResultsTarget* target = nullptr;
	std::string workingDirectory;
	std::deque<TestData> pending;
	std::vector<UnitTestThreadPtr> workers;
	std::atomic<bool> done;
};

typedef std::shared_ptr<TestManagerThread> TestManagerThreadPtr;

