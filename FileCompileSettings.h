////////////////////////////////////////////////////////////////////////////////
// Filename:    FileCompileSettings.h
// Description: This file declares the FileCompileSettings class.
//
// Created:     2012-08-27 23:30:29
// Author:      Jacob Buysse
////////////////////////////////////////////////////////////////////////////////
#pragma once
#include "Project.h"
#include "ProjectItem.h"
#include <string>

class FileCompileSettings
{
public:
	FileCompileSettings() = default;
	FileCompileSettings(const FileCompileSettings& rhs) = default;
	~FileCompileSettings() = default;

	FileCompileSettings& operator=(const FileCompileSettings& rhs) = default;

	void SetProjectItemFile(Project* project, ProjectItemFile* projectItem);

	bool CanCompile() const;
	bool NeedsToCompile() const;
	const std::string& GetFileName() const;
	std::string PrepareForCompile(const std::string& suffix) const;
	std::string GetCompileCommand() const;
	std::string GetOutputFile(const std::string& suffix) const;

private:
	Project* project = nullptr;
	ProjectItemFile* projectItem = nullptr;
	//g++
	//-O3
	//-Wall
	//-std=c++11
	//-c FileCompileSettings.cpp
	//-I c:\save\code
	//-o output/FileCompileSettings.o
};

