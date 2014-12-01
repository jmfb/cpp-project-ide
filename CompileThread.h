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
	void PrepareForLink();
	void ValidateProjectReferences();
	void ValidateProjectReference(const std::string& projectReference);
	void DeleteTargetFile() const;
	std::string GetLinkingCommand() const;
	std::string GetTargetFile() const;

	template <typename ValueType>
	void ValidateProjectSetting(
		const Project& reference,
		ValueType (Project::*setting)() const,
		const std::string& settingName) const
	{
		auto thisValue = (project->*setting)();
		auto referenceValue = (reference.*setting)();
		if (thisValue != referenceValue)
		{
			std::ostringstream out;
			out << "Project reference " << settingName << " mismatch." << std::endl
				<< project->GetName() << " = " << thisValue << std::endl
				<< reference.GetName() << " = " << referenceValue << std::endl;
			throw std::runtime_error{ out.str() };
		}
	}

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
	std::vector<std::string> referencedLibraries;
};

typedef std::shared_ptr<CompileThread> CompileThreadPtr;

