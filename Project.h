////////////////////////////////////////////////////////////////////////////////
// Filename:    Project.h
// Description: This file declares the Project class.  This class holds all
//              information contained within the project entity and persists
//              to/from the .cpp-project XML file.
//
// Created:     2012-08-05 20:40:59
// Author:      Jacob Buysse
//                                                              Modification Log
// When       Who               Description
// ---------- ----------------- ------------------------------------------------
////////////////////////////////////////////////////////////////////////////////
#pragma once
#include <string>
#include <list>
#include <vector>
#include <rapidxml-1.13/rapidxml.hpp>
#include "ProjectItem.h"
#include "ProjectItemFolder.h"
#include "ProjectItemFile.h"

class Project
{
public:
	Project();
	Project(const Project& rhs) = delete;
	~Project();

	Project& operator=(const Project& rhs);

	void New(const std::string& directory, const std::string& name);
	void Open(const std::string& fileName);
	void Save(const std::string& fileName);
	void Close();
	bool IsOpen() const;
	bool IsDirty() const;

	const std::string& GetFileName() const;
	const std::string& GetName() const;
	const std::string& GetStandard() const;
	const std::string& GetSubsystem() const;
	const std::string& GetWarnings() const;
	bool GetWarningsAsErrors() const;
	const std::string& GetOptimizationLevel() const;
	bool GetDebugInfo() const;
	bool GetMultithreaded() const;
	const std::string& GetOutputFolder() const;
	const std::string& GetOutputFileName() const;
	const std::list<std::string>& GetIncludeDirectories() const;
	const std::list<std::string>& GetLibraries() const;

	void SetDirty();
	void SetName(const std::string& value);
	void SetStandard(const std::string& value);
	void SetSubsystem(const std::string& value);
	void SetWarnings(const std::string& value);
	void SetWarningsAsErrors(bool value);
	void SetOptimizationLevel(const std::string& value);
	void SetDebugInfo(const bool value);
	void SetMultithreaded(const bool value);
	void SetOutputFolder(const std::string& value);
	void SetOutputFileName(const std::string& value);
	void SetIncludeDirectories(const std::list<std::string>& value);
	void SetLibraries(const std::list<std::string>& value);

	std::string GetTargetFile() const;
	std::string GetTargetUnitTestFile() const;
	std::string GetExecutableFile() const;
	std::string GetUnitTestFile() const;
	std::string GetRelativeFileName(const std::string& fileName) const;

	ProjectItemFolder& GetRootFolder();
	const ProjectItemFolder& GetRootFolder() const;

private:
	void LoadFiles(ProjectItemFolder* folder, rapidxml::xml_node<char>* files);

private:
	bool isOpen;
	bool isDirty;
	std::string fileName;
	std::string standard;
	std::string subsystem;
	std::string warnings;
	bool warningsAsErrors;
	std::string optimizationLevel;
	bool debugInfo;
	bool multithreaded;
	std::string outputFolder;
	std::string outputFileName;
	std::list<std::string> includeDirectories;
	std::list<std::string> libraries;
	ProjectItemFolder rootFolder;
};

