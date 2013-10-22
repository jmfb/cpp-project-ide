////////////////////////////////////////////////////////////////////////////////
// Filename:    FileCompileSettings.cpp
// Description: This file implements all FileCompileSettings member functions.
//
// Created:     2012-08-27 23:30:29
// Author:      Jacob Buysse
////////////////////////////////////////////////////////////////////////////////
#include "pch.h"
#include "FileCompileSettings.h"
#include "Process2.h"

void FileCompileSettings::SetProjectItemFile(Project* project, ProjectItemFile* projectItem)
{
	this->project = project;
	this->projectItem = projectItem;
}

bool FileCompileSettings::CanCompile() const
{
	auto extension = STRING::upper(FSYS::GetFileExt(projectItem->GetName()));
	const std::set<std::string> validExtensions = {
		"CPP", "RC"
	};
	return validExtensions.find(extension) != validExtensions.end();
}

bool FileCompileSettings::IsModuleDefinitionFile() const
{
	return STRING::upper(FSYS::GetFileExt(projectItem->GetName())) == "DEF";
}

bool FileCompileSettings::NeedsToCompile() const
{
	auto extension = STRING::upper(FSYS::GetFileExt(projectItem->GetName()));
	//There is no g++ dependency utility for RC files since they use windres to compile.
	//We could optimize by scanning for include statements manually...
	if (extension == "RC")
		return true;

	//Prepare to generate the dep file, but only do so if the output file
	//already exists (because if the output file does not exist then we
	//definitely need to
	auto depFile = PrepareForCompile("dep");
	auto outputFile = depFile.substr(0, depFile.size() - 3) + "o";
	if (!FSYS::FileExists(outputFile))
		return true;

	try
	{
		std::ostringstream out;
		out << "g++ -MM -E -std=" << project->GetStandard()
			<< " -c " << STRING::replace(projectItem->GetName(), "\\", "/");
		for (const auto& includeDirectory: project->GetIncludeDirectories())
			out << " -I " << includeDirectory;
		out << " -MF " << GetOutputFile("dep");

		Process::Shell(out.str(), FSYS::GetFilePath(project->GetFileName()), CREATE_NO_WINDOW, true);
	}
	catch (...)
	{
		//If any exception occurs attempting to determine if a file needs to compile
		//then we can assume that it does need to compile.  Errors would be from the
		//process call to generate the dep file.
		return true;
	}

	//If for some reason the dep file did not get generated, assume we need to rebuild.
	if (!FSYS::FileExists(depFile))
		return true;

	//Read the dependencies from the file
	std::vector<std::string> dependencies;
	std::ifstream in(depFile.c_str());
	std::string line;
	while (std::getline(in, line))
	{
		//Remove the trailing \ if it exists.
		if (!line.empty() && line.back() == '\\')
			line.pop_back();

		//Ignore the output file name from the first line (goes to '.o:').
		auto colon = line.find(".o:");
		if (colon != std::string::npos)
			line = line.substr(colon + 3);

		//Trim leading and trailing spaces.
		line = STRING::trim(line);
		if (line.empty())
			continue;

		//Scan each file separated by spaces (note that "\ " indicates a space
		//in an individual file name).
		std::string::size_type position = 0;
		for (;;)
		{
			auto next = line.find(' ', position + 1);
			while (next != std::string::npos && line[next - 1] == '\\')
				next = line.find(' ', next + 1);
			if (next == std::string::npos)
			{
				dependencies.push_back(line.substr(position));
				break;
			}
			else
			{
				dependencies.push_back(line.substr(position, next - position));
				position = next + 1;
			}
		}
	}

	//Check the last modified date for the output file and return true if
	//any of the dependencies has been modified after that time.
	auto lastCompiled = FSYS::GetFileLastWriteTime(outputFile);
	for (auto& dependency: dependencies)
	{
		//Clean up dependecy file name (and skip files we can't find)
		dependency = STRING::replace(dependency, "\\ ", " ");
		dependency = STRING::replace(dependency, "/", "\\");
		if (dependency.find(':') != 1)
			dependency = FSYS::FormatPath(FSYS::GetFilePath(project->GetFileName()), dependency);
		if (!FSYS::FileExists(dependency))
			continue;

		auto lastUpdated = FSYS::GetFileLastWriteTime(dependency);
		if (lastUpdated > lastCompiled)
			return true;
	}

	//We checked all dependencies and none have changed - this file is up to date.
	return false;
}

const std::string& FileCompileSettings::GetFileName() const
{
	return projectItem->GetName();
}

std::string FileCompileSettings::PrepareForCompile(const std::string& suffix) const
{
	auto outputFile = FSYS::FormatPath(
		FSYS::GetFilePath(project->GetFileName()),
		STRING::replace(GetOutputFile(suffix), "/", "\\"));
	auto outputDirectory = FSYS::GetFilePath(outputFile);
	//Make sure the output directory does exist
	if (!FSYS::PathExists(outputDirectory))
	{
		try
		{
			FSYS::CreatePath(outputDirectory);
		}
		catch (...)
		{
			if (!FSYS::PathExists(outputDirectory))
				throw;
		}
	}
	//Delete any previous output file that may have existed
	if (FSYS::FileExists(outputFile))
		::DeleteFile(outputFile.c_str());
	return outputFile;
}

std::string FileCompileSettings::GetCompileCommand() const
{
	std::ostringstream out;
	auto extension = STRING::upper(FSYS::GetFileExt(projectItem->GetName()));
	auto fileName = STRING::replace(projectItem->GetName(), "\\", "/");
	if (extension == "RC")
	{
		//windres -i resource.rc -o $(OUTDIR)/resource.o
		out << "windres -i " << fileName << " -o " << GetOutputFile("o");
	}
	else
	{
		//g++ -O3 -Wall -std=c++11 -c FileCompileSettings.cpp -I c:\save\code -o output/FileCompileSettings.o
		out << "g++ -O" << project->GetOptimizationLevel();
		if (!project->GetWarnings().empty())
			out << " -W" << project->GetWarnings();
		else
			out << " -w";
		if (project->GetWarningsAsErrors())
			out << " -Werror";
		out << " -std=" << project->GetStandard();
		if (project->GetDebugInfo())
			out << " -ggdb";
		if (project->GetArchitecture() == "64-bit")
			out << " -m64";
		if (project->GetMultithreaded())
			out << " -mthreads";
		out << " -c " << fileName;
		for (const auto& includeDirectory: project->GetIncludeDirectories())
			out << " -I " << includeDirectory;
		out << " -o " << GetOutputFile("o");
	}
	return out.str();
}

std::string FileCompileSettings::GetOutputFile(const std::string& suffix) const
{
	std::ostringstream out;
	out << project->GetOutputFolder() << "/";
	auto directory = FSYS::GetFilePath(projectItem->GetName());
	if (!directory.empty())
		out << directory << "/";
	out << FSYS::GetFileTitle(projectItem->GetName()) << "." << suffix;
	return out.str();
}


