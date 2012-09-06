////////////////////////////////////////////////////////////////////////////////
// Filename:    CompileThread.h
// Description: This file declares the CompileThread class.
//
// Created:     2012-08-26 22:58:09
// Author:      Jacob Buysse
////////////////////////////////////////////////////////////////////////////////
#pragma once
#include "BaseThread.h"
#include "CompileThreadEvents.h"
#include "FileCompileSettings.h"
#include <atomic>
#include <string>
#include <memory>

class CompileThread : public BaseThread
{
public:
	CompileThread();
	CompileThread(const CompileThread& rhs) = delete;
	virtual ~CompileThread() = default;

	CompileThread& operator=(const CompileThread& rhs) = delete;

	void Compile(
		CompileThreadEvents* events,
		unsigned long id,
		const FileCompileSettings& settings,
		const std::string& workingDirectory);
	void Link(
		CompileThreadEvents* events,
		unsigned long id,
		const Project* project,
		const std::string& objects,
		bool unitTest);
	bool IsDone() const;

	void Run() override;

private:
	void PrepareForLink() const;
	std::string GetLinkingCommand() const;
	std::string GetTargetFile() const;

private:
	std::atomic<bool> done;
	bool linking = false;
	unsigned long id = 0;
	FileCompileSettings settings;
	std::string workingDirectory;
	CompileThreadEvents* events = nullptr;
	const Project* project = nullptr;
	std::string objects;
	bool unitTest = false;
};

typedef std::shared_ptr<CompileThread> CompileThreadPtr;

