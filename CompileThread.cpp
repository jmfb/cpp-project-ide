////////////////////////////////////////////////////////////////////////////////
// Filename:    CompileThread.cpp
// Description: This file implements all CompileThread member functions.
//
// Created:     2012-08-26 22:58:09
// Author:      Jacob Buysse
////////////////////////////////////////////////////////////////////////////////
#include "pch.h"
#include "CompileThread.h"
#include "Process2.h"
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

		Process process;
		process.Start(command, workingDirectory);

		//Wait for the process to exit but periodically check the stopping flag.
		for (;;)
		{
			if (process.IsDone())
			{
				process.Close();
				break;
			}
			else if (events->IsStopping())
			{
				//The process has not stopped on its own and the user has requested the build
				//be stopped.  Terminate the process but still wait for it to complete termination
				//before exiting the loop.
				process.Terminate();
				break;
			}
			else
			{
				process.ReadSomeOutput();
				process.ReadSomeError();
				std::this_thread::yield();
			}
		}

		auto errorText = process.ReadErrorPipe();
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
	//	g++ -O3 -Wall -std=c++11 -ggdb -shared -m64
	//	./output/resource.o ./test.def -o ./output/cpp-project.exe
	//	-mthreads -Xlinker --subsystem -Xlinker windows -lcomctl32
	std::ostringstream out;
	out << "g++ -O" << project->GetOptimizationLevel()
		<< " -W" << project->GetWarnings()
		<< " -std=" << project->GetStandard();
	if (project->GetDebugInfo())
		out << " -ggdb";
	if (project->GetTarget() == "DLL" && !unitTest)
	{
		out << " -shared";
		out << " -Wl,--out-implib,./" << project->GetOutputFolder() << "/lib" << project->GetName() << ".a";
	}
	if (project->GetArchitecture() == "64-bit")
		out << " -m64";
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

