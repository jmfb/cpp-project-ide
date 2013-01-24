////////////////////////////////////////////////////////////////////////////////
// Filename:    ProjectEvents.h
// Description: This file declares the ProjectEvents interface.
//
// Created:     2012-08-25 11:59:49
// Author:      Jacob Buysse
//                                                              Modification Log
// When       Who               Description
// ---------- ----------------- ------------------------------------------------
////////////////////////////////////////////////////////////////////////////////
#pragma once
#include <string>

class ProjectEvents
{
public:
	virtual void OnProjectOpenFile(const std::string& fileName, bool openInOther) = 0;
	virtual void OnProjectRenameFile(const std::string& oldFileName, const std::string& newFileName) = 0;
};

