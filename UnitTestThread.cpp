////////////////////////////////////////////////////////////////////////////////
// Filename:    UnitTestThread.cpp
// Description: This file implements all UnitTestThread member functions.
//
// Created:     2012-09-13 00:45:24
// Author:      Jacob Buysse
////////////////////////////////////////////////////////////////////////////////
#include "UnitTestThread.h"
#include "Process2.h"

UnitTestThread::UnitTestThread()
	: done(false)
{
}

void UnitTestThread::SetTestData(
	unsigned long testIndex,
	const std::string& command,
	const std::string& workingDirectory,
	TestResultsTarget* target)
{
	this->testIndex = testIndex;
	this->command = command;
	this->workingDirectory = workingDirectory;
	this->target = target;
	Start();
}

bool UnitTestThread::IsDone()
{
	return done;
}

void UnitTestThread::Run()
{
	try
	{
		target->TestRunning(testIndex);

		Process process;
		process.Start(command, workingDirectory);
		process.SoftWaitForExit();

		auto result = process.ReadOutputPipe();
		if (result.find("Success") == 0)
			target->TestPassed(testIndex);
		else
		{
			auto pos = result.find("Failed: ");
			if (pos == 0)
				result.erase(0, 8);
			target->TestFailed(testIndex, result);
		}
	}
	catch (const std::exception& error)
	{
		target->TestFailed(testIndex, error.what());
	}
	catch (const ERR::CError& error)
	{
		target->TestFailed(testIndex, error.Format());
	}
	done = true;
}


