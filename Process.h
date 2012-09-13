////////////////////////////////////////////////////////////////////////////////
// Filename:    Process.h
// Description: This file declares the Process class.
//
// Created:     2012-09-12 23:58:49
// Author:      Jacob Buysse
////////////////////////////////////////////////////////////////////////////////
#pragma once
#include <string>
#include <CRL/WinUtility.h>

class Process
{
public:
	Process() = default;
	Process(const Process& rhs) = delete;
	~Process() = default;

	Process& operator=(const Process& rhs) = delete;

	static void Shell(const std::string& command, const std::string& workingDirectory, unsigned long createFlags, bool waitForExit);

	void Start(const std::string& command, const std::string& workingDirectory);
	bool IsDone();
	void WaitForExit(unsigned long timeout);
	void Terminate();
	void Close();
	std::string ReadOutputPipe();
	std::string ReadErrorPipe();

private:
	friend class ProcessTest;
	WIN::CPipe inputPipes[2], outputPipes[2], errorPipes[2];
	PROCESS_INFORMATION processInfo = {0};
	WIN::CHandle processThread;
	WIN::CHandle process;
};

