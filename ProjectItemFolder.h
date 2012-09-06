////////////////////////////////////////////////////////////////////////////////
// Filename:    ProjectItemFolder.h
// Description: This file declares the ProjectItemFolder class.
//
// Created:     2012-08-09 22:39:25
// Author:      Jacob Buysse
//                                                              Modification Log
// When       Who               Description
// ---------- ----------------- ------------------------------------------------
////////////////////////////////////////////////////////////////////////////////
#pragma once
#include "ProjectItem.h"
#include <vector>
#include <rapidxml-1.13/rapidxml.hpp>

class ProjectItemFolder : public ProjectItem
{
public:
	ProjectItemFolder(const std::string& name = "");
	ProjectItemFolder(const ProjectItemFolder& rhs) = delete;
	virtual ~ProjectItemFolder();

	ProjectItemFolder& operator=(const ProjectItemFolder& rhs) = delete;

	ProjectItemType GetType() const override;
	const std::string& GetName() const override;
	void SetName(const std::string& value) override;

	void Visit(ProjectItemVisitor* visitor) override;

	unsigned long GetChildrenCount() const;
	void AddChild(ProjectItemPtr projectItem);
	void InsertChild(unsigned long index, ProjectItemPtr projectItem);
	void RemoveChild(unsigned long index);
	ProjectItemPtr RemoveChild(ProjectItem* child);
	ProjectItemPtr GetChild(unsigned long index) const;
	void Clear();

	void SaveChildren(rapidxml::xml_document<char>& document, rapidxml::xml_node<char>* parent) const;

private:
	std::string name;
	std::vector<ProjectItemPtr> children;
};

