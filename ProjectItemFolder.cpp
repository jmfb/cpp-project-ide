////////////////////////////////////////////////////////////////////////////////
// Filename:    ProjectItemFolder.cpp
// Description: This file implements all ProjectItemFolder member functions.
//
// Created:     2012-08-09 22:43:29
// Author:      Jacob Buysse
//                                                              Modification Log
// When       Who               Description
// ---------- ----------------- ------------------------------------------------
////////////////////////////////////////////////////////////////////////////////
#include "pch.h"
#include "ProjectItemFolder.h"

ProjectItemFolder::ProjectItemFolder(const std::string& name)
	: name(name)
{
}

ProjectItemFolder::~ProjectItemFolder()
{
}

ProjectItemType ProjectItemFolder::GetType() const
{
	return ProjectItemType::Folder;
}

const std::string& ProjectItemFolder::GetName() const
{
	return name;
}

void ProjectItemFolder::SetName(const std::string& value)
{
	name = value;
}

void ProjectItemFolder::Visit(ProjectItemVisitor* visitor)
{
	visitor->VisitFolder(*this);
	for (const auto& item: children)
		item->Visit(visitor);
}

unsigned long ProjectItemFolder::GetChildrenCount() const
{
	return children.size();
}

void ProjectItemFolder::AddChild(ProjectItemPtr projectItem)
{
	children.push_back(projectItem);
}

void ProjectItemFolder::InsertChild(unsigned long index, ProjectItemPtr projectItem)
{
	children.insert(children.begin() + index, projectItem);
}

void ProjectItemFolder::RemoveChild(unsigned long index)
{
	children.erase(children.begin() + index);
}

ProjectItemPtr ProjectItemFolder::RemoveChild(ProjectItem* child)
{
	ProjectItemPtr result;
	for (auto iter = children.begin(); iter != children.end(); ++iter)
	{
		if (iter->get() == child)
		{
			result = *iter;
			children.erase(iter);
			break;
		}
	}
	return result;
}

ProjectItemPtr ProjectItemFolder::GetChild(unsigned long index) const
{
	return children[index];
}

void ProjectItemFolder::Clear()
{
	children.clear();
}

void ProjectItemFolder::SaveChildren(rapidxml::xml_document<char>& document, rapidxml::xml_node<char>* parent) const
{
	for (const auto& child: children)
	{
		switch(child->GetType())
		{
		case ProjectItemType::File:
			parent->append_node(document.allocate_node(rapidxml::node_element, "File", child->GetName().c_str()));
			break;

		case ProjectItemType::Folder:
			{
				auto folder = document.allocate_node(rapidxml::node_element, "Folder");
				folder->append_attribute(document.allocate_attribute("name", child->GetName().c_str()));
				parent->append_node(folder);
				dynamic_cast<const ProjectItemFolder*>(child.get())->SaveChildren(document, folder);
			}
			break;
		}
	}
}

