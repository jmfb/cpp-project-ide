////////////////////////////////////////////////////////////////////////////////
// Filename:    ProjectItemVisitor.h
// Description: This file declares the ProjectItemVisitor interface.
//
// Created:     2012-08-29 21:59:11
// Author:      Jacob Buysse
////////////////////////////////////////////////////////////////////////////////
#pragma once

class ProjectItemFile;
class ProjectItemFolder;

class ProjectItemVisitor
{
public:
	virtual void VisitFile(ProjectItemFile& file) = 0;
	virtual void VisitFolder(ProjectItemFolder& folder) = 0;
};

