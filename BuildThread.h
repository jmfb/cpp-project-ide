////////////////////////////////////////////////////////////////////////////////
// Filename:    BuildThread.h
// Description: This file declares the BuildThread class.
//
// Created:     2012-08-27 00:47:18
// Author:      Jacob Buysse
////////////////////////////////////////////////////////////////////////////////
#pragma once
#include "BaseThread.h"
#include "CompileThread.h"
#include "CompileThreadEvents.h"
#include "FileCompileSettings.h"
#include "Project.h"
#include <list>
#include <atomic>
#include <memory>

class BuildThread : public BaseThread
{
public:
	BuildThread();
	BuildThread(const BuildThread& rhs) = delete;
	virtual ~BuildThread() = default;

	BuildThread& operator=(const BuildThread& rhs) = delete;

	void AddFileCompileSettings(const FileCompileSettings& setting);
	void MakeProjectTarget(const Project* project, const std::string& objects);
	void MakeProjectUnitTest(const Project* project, const std::string& testObjects);
	void Build(
		CompileThreadEvents* events,
		unsigned long id,
		const std::string& workingDirectory);
	bool IsDone() const;

	void Run() override;

private:
	void TidyWorkers();

private:
	unsigned long id = 0;
	std::string workingDirectory;
	std::list<FileCompileSettings> settings;
	std::list<CompileThreadPtr> workers;
	CompileThreadEvents* events = nullptr;
	const Project* project = nullptr;
	std::string objects;
	std::string testObjects;
	std::atomic<bool> done;
};

typedef std::shared_ptr<BuildThread> BuildThreadPtr;

