////////////////////////////////////////////////////////////////////////////////
// Filename:    ProjectItemFile.cpp
// Description: This file implements all ProjectItemFile member functions.
//
// Created:     2012-08-09 22:36:16
// Author:      Jacob Buysse
//                                                              Modification Log
// When       Who               Description
// ---------- ----------------- ------------------------------------------------
////////////////////////////////////////////////////////////////////////////////
#include "pch.h"
#include "ProjectItemFile.h"

ProjectItemFile::ProjectItemFile(const std::string& name)
	: name(name)
{
}

ProjectItemFile::~ProjectItemFile()
{
}

ProjectItemType ProjectItemFile::GetType() const
{
	return ProjectItemType::File;
}

const std::string& ProjectItemFile::GetName() const
{
	return name;
}

void ProjectItemFile::SetName(const std::string& value)
{
	name = value;
}

void ProjectItemFile::Visit(ProjectItemVisitor* visitor)
{
	visitor->VisitFile(*this);
}

