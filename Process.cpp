////////////////////////////////////////////////////////////////////////////////
// Filename:    Process.cpp
// Description: This file implements all Process member functions.
//
// Created:     2012-09-12 23:58:49
// Author:      Jacob Buysse
////////////////////////////////////////////////////////////////////////////////
#include "pch.h"
#include "Process2.h"
#include <thread>

void Process::Shell(const std::string& command, const std::string& workingDirectory, unsigned long createFlags, bool waitForExit)
{
	STARTUPINFO startupInfo = {0};
	std::memset(&startupInfo, 0, sizeof(startupInfo));
	startupInfo.cb = sizeof(startupInfo);

	PROCESS_INFORMATION processInfo = {0};
	std::memset(&processInfo, 0, sizeof(processInfo));

	//We need to make a copy of the string for the command line (non-const)
	STRING::CStringPtr commandCopy(new char[command.size() + 1]);
	std::strcpy(commandCopy.Get(), command.c_str());

	//Create the process (do not show the associated console)
	auto result = ::CreateProcess(
		nullptr,
		commandCopy.Get(),
		nullptr,
		nullptr,
		FALSE,
		createFlags,
		nullptr,
		workingDirectory.c_str(),
		&startupInfo,
		&processInfo);
	ERR::CheckWindowsError(!result, "Process::Shell", "CreateProcess");

	//Attach resultant process and thread handles to scoped containers.
	WIN::CHandle processThread(processInfo.hThread);
	WIN::CHandle process(processInfo.hProcess);

	//Wait for the process to complete
	if (waitForExit)
		::WaitForSingleObject(processInfo.hProcess, INFINITE);
}

void Process::Start(const std::string& command, const std::string& workingDirectory)
{
	constexpr auto trace = __PRETTY_FUNCTION__;

	//Create a security attributes specifying handles will be inherited
	SECURITY_ATTRIBUTES securityAttributes = {0};
	securityAttributes.nLength = sizeof(securityAttributes);
	securityAttributes.lpSecurityDescriptor = nullptr;
	securityAttributes.bInheritHandle = TRUE;

	//Create read and write pipes for input, output, and error communication channels with process.
	WIN::CreatePipe(inputPipes[0], inputPipes[1], &securityAttributes);
	WIN::CreatePipe(outputPipes[0], outputPipes[1], &securityAttributes);
	WIN::CreatePipe(errorPipes[0], errorPipes[1], &securityAttributes);
	::SetHandleInformation(inputPipes[1].Get(), HANDLE_FLAG_INHERIT, 0);
	::SetHandleInformation(outputPipes[0].Get(), HANDLE_FLAG_INHERIT, 0);
	::SetHandleInformation(errorPipes[0].Get(), HANDLE_FLAG_INHERIT, 0);

	//Create the startup info that contains the pipes to use (these will be the write pipes
	//used by the newly created process for output and error and the read pipe for input).
	STARTUPINFO startupInfo = {0};
	std::memset(&startupInfo, 0, sizeof(startupInfo));
	startupInfo.cb = sizeof(startupInfo);
	startupInfo.dwFlags = STARTF_USESTDHANDLES;
	startupInfo.hStdInput = inputPipes[0].Get();
	startupInfo.hStdOutput = outputPipes[1].Get();
	startupInfo.hStdError = errorPipes[1].Get();

	//Declare the process information (handles) that will be set from call to create
	std::memset(&processInfo, 0, sizeof(processInfo));

	//We need to make a copy of the string for the command line (non-const)
	STRING::CStringPtr commandCopy(new char[command.size() + 1]);
	std::strcpy(commandCopy.Get(), command.c_str());

	//Create the process (do not show the associated console)
	auto result = ::CreateProcess(
		nullptr,
		commandCopy.Get(),
		nullptr,
		nullptr,
		TRUE,
		CREATE_NO_WINDOW,
		nullptr,
		workingDirectory.c_str(),
		&startupInfo,
		&processInfo);
	ERR::CheckWindowsError(!result, trace, "CreateProcess");

	//Attach resultant process and thread handles to scoped containers.
	processThread.Attach(processInfo.hThread);
	process.Attach(processInfo.hProcess);
}

bool Process::IsDone()
{
	constexpr auto trace = __PRETTY_FUNCTION__;
	auto waitResult = ::WaitForSingleObject(processInfo.hProcess, 100);
	ERR::CheckWindowsError(waitResult == WAIT_FAILED, trace, "WaitForSingleObject");
	return waitResult == WAIT_OBJECT_0;
}

void Process::WaitForExit(unsigned long timeout)
{
	constexpr auto trace = __PRETTY_FUNCTION__;
	auto waitResult = ::WaitForSingleObject(processInfo.hProcess, timeout);
	ERR::CheckWindowsError(waitResult == WAIT_FAILED, trace, "WaitForSingleObject");
	if (waitResult == WAIT_OBJECT_0)
		Close();
	else
		Terminate();
}

void Process::SoftWaitForExit()
{
	while (!IsDone())
	{
		ReadSomeError();
		ReadSomeOutput();
		std::this_thread::yield();
	}
	Close();
}

void Process::Terminate()
{
	constexpr auto trace = __PRETTY_FUNCTION__;
	auto result = ::TerminateProcess(processInfo.hProcess, 0);
	ERR::CheckWindowsError(!result, trace, "TerminateProcess");
	auto waitResult = ::WaitForSingleObject(processInfo.hProcess, INFINITE);
	ERR::CheckWindowsError(waitResult == WAIT_FAILED, trace, "WaitForSingleObject[stopping]");
	Close();
}

void Process::Close()
{
	processThread.Release();
	process.Release();
}

std::string Process::ReadOutputPipe()
{
	ReadSomeOutput();
	return outputData.str();
}

std::string Process::ReadErrorPipe()
{
	ReadSomeError();
	return errorData.str();
}

void Process::ReadSomeOutput()
{
	outputData << outputPipes[0].ReadString();
}

void Process::ReadSomeError()
{
	errorData << errorPipes[0].ReadString();
}

