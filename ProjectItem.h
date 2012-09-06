////////////////////////////////////////////////////////////////////////////////
// Filename:    ProjectItem.h
// Description: This file declares the ProjectItem class.
//
// Created:     2012-08-09 22:25:56
// Author:      Jacob Buysse
//                                                              Modification Log
// When       Who               Description
// ---------- ----------------- ------------------------------------------------
////////////////////////////////////////////////////////////////////////////////
#pragma once
#include "ProjectItemType.h"
#include "ProjectItemVisitor.h"
#include <memory>
#include <string>

class ProjectItem
{
public:
	ProjectItem();
	ProjectItem(const ProjectItem& rhs) = delete;
	virtual ~ProjectItem();

	ProjectItem& operator=(const ProjectItem& rhs) = delete;

	virtual ProjectItemType GetType() const = 0;
	virtual const std::string& GetName() const = 0;
	virtual void SetName(const std::string& value) = 0;

	virtual void Visit(ProjectItemVisitor* visitor) = 0;
};

typedef std::shared_ptr<ProjectItem> ProjectItemPtr;

