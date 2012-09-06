////////////////////////////////////////////////////////////////////////////////
// Filename:    ProjectWindow.h
// Description: This file declares the ProjectWindow class.  This is the left hand
//              pane of the application that contains the project explorer tree.
//
// Created:     2012-08-04 23:33:49
// Author:      Jacob Buysse
//                                                              Modification Log
// When       Who               Description
// ---------- ----------------- ------------------------------------------------
////////////////////////////////////////////////////////////////////////////////
#pragma once
#include "Project.h"
#include "ProjectEvents.h"

static const int treeId = 1001;

class ProjectWindow : public WIN::CTreeViewEvents<WIN::CWindowImpl<ProjectWindow>, treeId>
{
public:
	ProjectWindow() = default;
	ProjectWindow(const ProjectWindow& rhs) = delete;
	~ProjectWindow() = default;

	ProjectWindow& operator=(const ProjectWindow& rhs) = delete;

	static void SetupClass(WNDCLASSEX& cls);

	bool OnCreate(CREATESTRUCT* cs) override;
	void OnSize(unsigned long flag, unsigned short w, unsigned short h) override;
	void OnTreeViewDblClk(int id) override;
	void OnTreeViewReturn(int id) override;
	void OnTreeViewRClick(int id) override;
	void OnTreeViewEndLabelEdit(int id, NMTVDISPINFO* hdr) override;
	void OnTreeViewBeginDrag(int id, NMTREEVIEW* hdr) override;

	void SetEvents(ProjectEvents* value);
	void SetProject(Project& value);
	void EnsureFileIsVisible(const std::string& fileName);
	void RenameSelection();
	void CreateNewFolder(const std::string& value);
	void CreateNewFile(const std::string& value);
	void DeleteSelection();
	ProjectItemFile* GetSelectedFile();
	void UpdateProjectName();
	void Clear();

private:
	HTREEITEM AddChild(ProjectItemPtr item, HTREEITEM parent);
	void CreateProjectItem(ProjectItemPtr itemToAdd);

private:
	WIN::CTreeView tree;
	WIN::CImageList images;
	Project* project = nullptr;
	ProjectEvents* events = nullptr;
};

