////////////////////////////////////////////////////////////////////////////////
// Filename:    CompileThread.cpp
// Description: This file implements all CompileThread member functions.
//
// Created:     2012-08-26 22:58:09
// Author:      Jacob Buysse
////////////////////////////////////////////////////////////////////////////////
#include "pch.h"
#include "CompileThread.h"
#include <cstring>

CompileThread::CompileThread()
	: done(false)
{
}

void CompileThread::Compile(
	CompileThreadEvents* events,
	unsigned long id,
	const FileCompileSettings& settings,
	const std::string& workingDirectory)
{
	this->events = events;
	this->id = id;
	this->settings = settings;
	this->workingDirectory = workingDirectory;
	Start();
}

void CompileThread::Link(
	CompileThreadEvents* events,
	unsigned long id,
	const Project* project,
	const std::string& objects,
	bool unitTest)
{
	this->events = events;
	this->id = id;
	this->project = project;
	this->objects = objects;
	this->unitTest = unitTest;
	workingDirectory = FSYS::GetFilePath(project->GetFileName());
	linking = true;
}

bool CompileThread::IsDone() const
{
	return done;
}

void CompileThread::Run()
{
	const std::string trace = "CompileThread::Run";
	try
	{
		//Check if nothing needs to compile (done in thread instead of caller
		//because time to check dependencies is not zero - requires -MM run of
		//g++ and many file last write time accesses).
		if (!linking && !settings.NeedsToCompile())
		{
			events->ProcessMessage(id, settings.GetFileName() + " is up to date.");
			done = true;
			return;
		}

		auto command = linking ? GetLinkingCommand() : settings.GetCompileCommand();
		events->ProcessMessage(id, command);

		//Prepare for the compile
		if (linking)
			PrepareForLink();
		else
			settings.PrepareForCompile("o");

		//Create a security attributes specifying handles will be inherited
		SECURITY_ATTRIBUTES securityAttributes = {0};
		securityAttributes.nLength = sizeof(securityAttributes);
		securityAttributes.lpSecurityDescriptor = nullptr;
		securityAttributes.bInheritHandle = TRUE;

		//Create read and write pipes for input, output, and error communication channels with process.
		WIN::CPipe inputPipes[2], outputPipes[2], errorPipes[2];
		WIN::CreatePipe(inputPipes[0], inputPipes[1], &securityAttributes);
		WIN::CreatePipe(outputPipes[0], outputPipes[1], &securityAttributes);
		WIN::CreatePipe(errorPipes[0], errorPipes[1], &securityAttributes);

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
			TRUE,
			CREATE_NO_WINDOW,
			nullptr,
			workingDirectory.c_str(),
			&startupInfo,
			&processInfo);
		ERR::CheckWindowsError(!result, trace, "CreateProcess");

		//Attach resultant process and thread handles to scoped containers.
		WIN::CHandle processThread(processInfo.hThread);
		WIN::CHandle process(processInfo.hProcess);

		//Wait for the process to exit but periodically check the stopping flag.
		for (;;)
		{
			auto waitResult = ::WaitForSingleObject(processInfo.hProcess, 100);
			ERR::CheckWindowsError(waitResult == WAIT_FAILED, trace, "WaitForSingleObject");
			if (waitResult == WAIT_OBJECT_0)
			{
				processThread.Release();
				process.Release();
				break;
			}
			else if (events->IsStopping())
			{
				//The process has not stopped on its own and the user has requested the build
				//be stopped.  Terminate the process but still wait for it to complete termination
				//before exiting the loop.
				result = ::TerminateProcess(processInfo.hProcess, 0);
				ERR::CheckWindowsError(!result, trace, "TerminateProcess");
				waitResult = ::WaitForSingleObject(processInfo.hProcess, INFINITE);
				ERR::CheckWindowsError(waitResult == WAIT_FAILED, trace, "WaitForSingleObject[stopping]");
				processThread.Release();
				process.Release();
				break;
			}
			else
			{
				std::this_thread::yield();
			}
		}

		auto errorText = errorPipes[0].ReadString();
		std::istringstream errorIn(errorText);
		std::string errorLine;
		while (std::getline(errorIn, errorLine))
			events->ProcessMessage(id, errorLine);
	}
	catch (const std::exception& error)
	{
		events->ProcessMessage(id, error.what());
	}
	catch (const ERR::CError& error)
	{
		events->ProcessMessage(id, error.Format());
	}
	catch (...)
	{
		events->ProcessMessage(id, "Unhandled exception.");
	}
	done = true;
}

void CompileThread::PrepareForLink() const
{
	auto targetFile = FSYS::FormatPath(
		FSYS::GetFilePath(project->GetFileName()),
		STRING::replace(GetTargetFile(), "/", "\\"));
	if (FSYS::FileExists(targetFile))
		::DeleteFile(targetFile.c_str());
}

std::string CompileThread::GetLinkingCommand() const
{
	//g++ -O3 -Wall -std=c++11 ./output/resource.o -o ./output/cpp-project.exe
	//-mthreads -Xlinker --subsystem -Xlinker windows -lcomctl32
	std::ostringstream out;
	out << "g++ -O" << project->GetOptimizationLevel()
		<< " -W" << project->GetWarnings()
		<< " -std=" << project->GetStandard();
	if (project->GetDebugInfo())
		out << " -ggdb";
	out << " " << objects << " -o ./" << GetTargetFile();
	if (project->GetMultithreaded())
		out << " -mthreads";
	out << " -Xlinker --subsystem -Xlinker " << (unitTest ? std::string("console") : project->GetSubsystem());
	for (const auto& library: project->GetLibraries())
		out << " -l" << library;
	return out.str();
}

std::string CompileThread::GetTargetFile() const
{
	std::ostringstream out;
	out << project->GetOutputFolder() << "/" << (unitTest ? project->GetUnitTestFile() : project->GetExecutableFile());
	return out.str();
}

