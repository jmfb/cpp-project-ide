////////////////////////////////////////////////////////////////////////////////
// Filename:    Project.cpp
// Description: This file implements all Project member functions.
//
// Created:     2012-08-05 20:50:59
// Author:      Jacob Buysse
//                                                              Modification Log
// When       Who               Description
// ---------- ----------------- ------------------------------------------------
////////////////////////////////////////////////////////////////////////////////
#include "pch.h"
#include "Project.h"
#include <fstream>
#include <iterator>
#include <cstring>
#include <rapidxml-1.13/rapidxml.hpp>
#include <rapidxml-1.13/rapidxml_print.hpp>
#include <rapidxml-1.13/rapidxml_utils.hpp>

const auto defaultName = "Project1";
const auto defaultStandard = "c++11";
const auto defaultSubsystem = "windows";
const auto defaultWarnings = "all";
const auto defaultWarningsAsErrors = true;
const auto defaultOptimizationLevel = "4";
const auto defaultDebugInfo = false;
const auto defaultMultithreaded = true;
const auto defaultOutputFolder = "output";
const auto defaultOutputFileName = "{ProjectName}.exe";
const auto xmlBoolTrue = "True";
const auto xmlBoolFalse = "False";

Project::Project()
	: isOpen(false), isDirty(false), warningsAsErrors(false), debugInfo(false), multithreaded(false)
{
}

Project::~Project()
{
}

void Project::New(const std::string& directory, const std::string& name)
{
	isOpen = true;
	isDirty = true;
	auto fullDirectory = FSYS::FormatPath(directory, name);
	fileName = FSYS::FormatPath(fullDirectory, name + ".cpp-project");
	rootFolder.SetName(name);
	standard = defaultStandard;
	subsystem = defaultSubsystem;
	warnings = defaultWarnings;
	warningsAsErrors = defaultWarningsAsErrors;
	optimizationLevel = defaultOptimizationLevel;
	debugInfo = defaultDebugInfo;
	multithreaded = defaultMultithreaded;
	outputFolder = defaultOutputFolder;
	outputFileName = defaultOutputFileName;
	includeDirectories.clear();
	libraries.clear();
}

void Project::Open(const std::string& fileName)
{
	isOpen = true;
	isDirty = false;
	this->fileName = fileName;

	//Open the file and parse the xml
	rapidxml::file<char> file(fileName.c_str());
	rapidxml::xml_document<char> document;
	document.parse<0>(file.data());

	auto project = document.first_node("Project");

	auto nameAttribute = project ? project->first_attribute("name") : nullptr;
	rootFolder.SetName(nameAttribute ? nameAttribute->value() : defaultName);

	auto settings = project ? project->first_node("Settings") : nullptr;

	auto loadSetting = [&](const char* nodeName, const char* defaultValue) -> const char*
	{
		auto node = settings ? settings->first_node(nodeName) : nullptr;
		return node ? node->value() : defaultValue;
	};
	auto loadBitSetting = [&](const char* nodeName, bool defaultValue) -> bool
	{
		return std::strcmp(loadSetting(nodeName, defaultValue ? xmlBoolTrue : xmlBoolFalse), xmlBoolTrue) == 0;
	};

	standard = loadSetting("Standard", defaultStandard);
	subsystem = loadSetting("Subsystem", defaultSubsystem);
	warnings = loadSetting("Warnings", defaultWarnings);
	warningsAsErrors = loadBitSetting("WarningsAsErrors", defaultWarningsAsErrors);
	optimizationLevel = loadSetting("OptimizationLevel", defaultOptimizationLevel);
	debugInfo = loadBitSetting("DebugInfo", defaultDebugInfo);
	multithreaded = loadBitSetting("Multithreaded", defaultMultithreaded);
	outputFolder = loadSetting("OutputFolder", defaultOutputFolder);
	outputFileName = loadSetting("OutputFileName", defaultOutputFileName);

	auto includeDirectoriesNode = settings ? settings->first_node("IncludeDirectories") : nullptr;
	includeDirectories.clear();
	if (includeDirectoriesNode)
		for (auto node = includeDirectoriesNode->first_node("IncludeDirectory"); node; node = node->next_sibling("IncludeDirectory"))
			includeDirectories.push_back(node->value());

	auto librariesNode = settings ? settings->first_node("Libraries") : nullptr;
	libraries.clear();
	if (librariesNode)
		for (auto node = librariesNode->first_node("Library"); node; node = node->next_sibling("Library"))
			libraries.push_back(node->value());

	auto files = project->first_node("Files");
	rootFolder.Clear();
	if (files)
		LoadFiles(&rootFolder, files);
}

void Project::LoadFiles(ProjectItemFolder* folder, rapidxml::xml_node<char>* files)
{
	for (auto item = files->first_node(); item; item = item->next_sibling())
	{
		if (std::strcmp(item->name(), "File") == 0)
		{
			folder->AddChild(ProjectItemPtr(new ProjectItemFile(item->value())));
		}
		else if (std::strcmp(item->name(), "Folder") == 0)
		{
			auto nameAttribute = item->first_attribute("name");
			auto childFolder = new ProjectItemFolder(nameAttribute ? nameAttribute->value() : "");
			folder->AddChild(ProjectItemPtr(childFolder));
			LoadFiles(childFolder, item);
		}
	}
}

void Project::Save(const std::string& fileName)
{
	isOpen = true;
	isDirty = false;

	this->fileName = fileName;

	auto directory = FSYS::GetFilePath(fileName);
	if (!FSYS::PathExists(directory))
		FSYS::CreatePath(directory);

	rapidxml::xml_document<char> document;
	auto project = document.allocate_node(rapidxml::node_element, "Project");
	document.append_node(project);
	project->append_attribute(document.allocate_attribute("name", rootFolder.GetName().c_str()));

	auto settings = document.allocate_node(rapidxml::node_element, "Settings");
	project->append_node(settings);
	auto saveSetting = [&](const char* nodeName, const std::string& value) -> void
	{
		settings->append_node(document.allocate_node(rapidxml::node_element, nodeName, document.allocate_string(value.c_str())));
	};
	auto saveBitSetting = [&](const char* nodeName, const bool value) -> void
	{
		saveSetting(nodeName, value ? xmlBoolTrue : xmlBoolFalse);
	};
	saveSetting("Standard", standard);
	saveSetting("Subsystem", subsystem);
	saveSetting("Warnings", warnings);
	saveBitSetting("WarningsAsErrors", warningsAsErrors);
	saveSetting("OptimizationLevel", optimizationLevel);
	saveBitSetting("DebugInfo", debugInfo);
	saveBitSetting("Multithreaded", multithreaded);
	saveSetting("OutputFolder", outputFolder);
	saveSetting("OutputFileName", outputFileName);

	auto includeDirectoriesNode = document.allocate_node(rapidxml::node_element, "IncludeDirectories");
	settings->append_node(includeDirectoriesNode);
	for (const auto& includeDirectory: includeDirectories)
		includeDirectoriesNode->append_node(document.allocate_node(rapidxml::node_element, "IncludeDirectory", includeDirectory.c_str()));

	auto librariesNode = document.allocate_node(rapidxml::node_element, "Libraries");
	settings->append_node(librariesNode);
	for (const auto& library: libraries)
		librariesNode->append_node(document.allocate_node(rapidxml::node_element, "Library", library.c_str()));

	auto files = document.allocate_node(rapidxml::node_element, "Files");
	project->append_node(files);
	rootFolder.SaveChildren(document, files);

	std::ofstream out(fileName.c_str());
	rapidxml::print(std::ostream_iterator<char>(out), document, 0);
}

void Project::Close()
{
	isOpen = false;
	isDirty = false;
}

bool Project::IsOpen() const
{
	return isOpen;
}

bool Project::IsDirty() const
{
	return isDirty;
}

const std::string& Project::GetFileName() const
{
	return fileName;
}

const std::string& Project::GetName() const
{
	return rootFolder.GetName();
}

const std::string& Project::GetStandard() const
{
	return standard;
}

const std::string& Project::GetSubsystem() const
{
	return subsystem;
}

const std::string& Project::GetWarnings() const
{
	return warnings;
}

bool Project::GetWarningsAsErrors() const
{
	return warningsAsErrors;
}

const std::string& Project::GetOptimizationLevel() const
{
	return optimizationLevel;
}

bool Project::GetDebugInfo() const
{
	return debugInfo;
}

bool Project::GetMultithreaded() const
{
	return multithreaded;
}

const std::string& Project::GetOutputFolder() const
{
	return outputFolder;
}

const std::string& Project::GetOutputFileName() const
{
	return outputFileName;
}

const std::list<std::string>& Project::GetIncludeDirectories() const
{
	return includeDirectories;
}

const std::list<std::string>& Project::GetLibraries() const
{
	return libraries;
}

void Project::SetDirty()
{
	isDirty = true;
}

void Project::SetName(const std::string& value)
{
	isDirty = true;
	rootFolder.SetName(value);
}

void Project::SetStandard(const std::string& value)
{
	isDirty = true;
	standard = value;
}

void Project::SetSubsystem(const std::string& value)
{
	isDirty = true;
	subsystem = value;
}

void Project::SetWarnings(const std::string& value)
{
	isDirty = true;
	warnings = value;
}

void Project::SetWarningsAsErrors(bool value)
{
	isDirty = true;
	warningsAsErrors = value;
}

void Project::SetOptimizationLevel(const std::string& value)
{
	isDirty = true;
	optimizationLevel = value;
}

void Project::SetDebugInfo(const bool value)
{
	isDirty = true;
	debugInfo = value;
}

void Project::SetMultithreaded(const bool value)
{
	isDirty = true;
	multithreaded = value;
}

void Project::SetOutputFolder(const std::string& value)
{
	isDirty = true;
	outputFolder = value;
}

void Project::SetOutputFileName(const std::string& value)
{
	isDirty = true;
	outputFileName = value;
}

void Project::SetIncludeDirectories(const std::list<std::string>& value)
{
	isDirty = true;
	includeDirectories = value;
}

void Project::SetLibraries(const std::list<std::string>& value)
{
	isDirty = true;
	libraries = value;
}

std::string Project::GetTargetFile() const
{
	return FSYS::FormatPath(FSYS::FormatPath(FSYS::GetFilePath(fileName), outputFolder), GetExecutableFile());
}

std::string Project::GetTargetUnitTestFile() const
{
	return FSYS::FormatPath(FSYS::FormatPath(FSYS::GetFilePath(fileName), outputFolder), GetUnitTestFile());
}

std::string Project::GetExecutableFile() const
{
	return STRING::replace(outputFileName, "{ProjectName}", GetName());
}

std::string Project::GetUnitTestFile() const
{
	auto file = GetExecutableFile();
	file.insert(file.rfind('.'), ".Test");
	return file;
}

std::string Project::GetRelativeFileName(const std::string& fileName) const
{
	auto directory = FSYS::GetFilePath(this->fileName);
	if (fileName.find(directory) == 0 && (directory.size() + 1) < fileName.size())
		return fileName.substr(directory.size() + 1);
	return fileName;
}

ProjectItemFolder& Project::GetRootFolder()
{
	return rootFolder;
}

const ProjectItemFolder& Project::GetRootFolder() const
{
	return rootFolder;
}

