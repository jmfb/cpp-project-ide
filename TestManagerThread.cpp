////////////////////////////////////////////////////////////////////////////////
// Filename:    TestManagerThread.cpp
// Description: This file implements all TestManagerThread member functions.
//
// Created:     2012-09-13 00:43:46
// Author:      Jacob Buysse
////////////////////////////////////////////////////////////////////////////////
#include "TestManagerThread.h"

void TestManagerThread::AddUnitTest(unsigned long testIndex, const std::string& command)
{
	pending.push_back({ testIndex, command });
}

void TestManagerThread::RunTests(const std::string& workingDirectory, TestResultsTarget* target)
{
	this->workingDirectory = workingDirectory;
	this->target = target;
	Start();
}

bool TestManagerThread::IsDone()
{
	return done;
}

void TestManagerThread::Run()
{
	try
	{
		while (!pending.empty())
		{
			while (workers.size() < std::thread::hardware_concurrency() && !pending.empty())
			{
				auto worker = std::make_shared<UnitTestThread>();
				const auto& test = pending.front();
				worker->SetTestData(test.testIndex, test.command, workingDirectory, target);
				workers.push_back(worker);
				pending.pop_front();
			}
			TidyWorkers();
			std::this_thread::yield();
		}

		while (!workers.empty())
		{
			TidyWorkers();
			std::this_thread::yield();
		}
	}
	catch (...)
	{
		//todo: report exceptions from test manager thread
	}
	done = true;
}

void TestManagerThread::TidyWorkers()
{
	for (auto iter = workers.begin(); iter != workers.end(); )
	{
		if ((*iter)->IsDone())
			iter = workers.erase(iter);
		else
			++iter;
	}
}

