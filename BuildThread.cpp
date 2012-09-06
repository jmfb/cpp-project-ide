////////////////////////////////////////////////////////////////////////////////
// Filename:    BuildThread.cpp
// Description: This file implements all BuildThread member functions.
//
// Created:     2012-08-27 00:47:18
// Author:      Jacob Buysse
////////////////////////////////////////////////////////////////////////////////
#include "pch.h"
#include "BuildThread.h"

BuildThread::BuildThread()
	: done(false)
{
}

void BuildThread::AddFileCompileSettings(const FileCompileSettings& setting)
{
	settings.push_back(setting);
}

void BuildThread::Build(
	CompileThreadEvents* events,
	unsigned long id,
	const std::string& workingDirectory)
{
	this->events = events;
	this->id = id;
	this->workingDirectory = workingDirectory;
	Start();
}

void BuildThread::MakeProjectTarget(const Project* project, const std::string& objects)
{
	this->project = project;
	this->objects = objects;
}

void BuildThread::MakeProjectUnitTest(const Project* project, const std::string& testObjects)
{
	this->project = project;
	this->testObjects = testObjects;
}

bool BuildThread::IsDone() const
{
	return done;
}

void BuildThread::Run()
{
	try
	{
		events->ProcessMessage(id, "Build started.");
		unsigned long nextWorkerId = id + 1;
		while (!settings.empty() && !events->IsStopping())
		{
			while (workers.size() < std::thread::hardware_concurrency() && !settings.empty() && !events->IsStopping())
			{
				CompileThreadPtr worker(new CompileThread());
				worker->Compile(events, nextWorkerId++, settings.front(), workingDirectory);
				workers.push_back(worker);
				settings.pop_front();
			}
			TidyWorkers();
			std::this_thread::yield();
		}

		while (!workers.empty())
		{
			TidyWorkers();
			std::this_thread::yield();
		}

		if (project != nullptr && !objects.empty() && !events->IsStopping())
		{
			CompileThread linker;
			linker.Link(events, id, project, objects, false);
			linker.Run();
		}
		if (project != nullptr && !testObjects.empty() && !events->IsStopping())
		{
			CompileThread linker;
			linker.Link(events, id, project, testObjects, true);
			linker.Run();
		}

		events->ProcessMessage(id, events->IsStopping() ? "Build canceled." : "Build Completed");
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

void BuildThread::TidyWorkers()
{
	for (auto iter = workers.begin(); iter != workers.end(); )
	{
		if ((*iter)->IsDone())
			iter = workers.erase(iter);
		else
			++iter;
	}
}

