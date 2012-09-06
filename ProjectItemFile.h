////////////////////////////////////////////////////////////////////////////////
// Filename:    ProjectItemFile.h
// Description: This file declares the ProjectItemFile class.
//
// Created:     2012-08-09 22:33:07
// Author:      Jacob Buysse
//                                                              Modification Log
// When       Who               Description
// ---------- ----------------- ------------------------------------------------
////////////////////////////////////////////////////////////////////////////////
#pragma once
#include "ProjectItem.h"

class ProjectItemFile : public ProjectItem
{
public:
	ProjectItemFile(const std::string& name = "");
	ProjectItemFile(const ProjectItemFile& rhs) = delete;
	virtual ~ProjectItemFile();

	ProjectItemFile& operator=(const ProjectItemFile& rhs) = delete;

	ProjectItemType GetType() const override;
	const std::string& GetName() const override;
	void SetName(const std::string& value) override;

	void Visit(ProjectItemVisitor* visitor) override;

private:
	std::string name;
};

