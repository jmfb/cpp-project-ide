////////////////////////////////////////////////////////////////////////////////
// Filename:    ProjectSettingsDialog.h
// Description: This file declares the ProjectSettingsDialog class.
//
// Created:     2012-08-30 21:38:10
// Author:      Jacob Buysse
////////////////////////////////////////////////////////////////////////////////
#pragma once
#include "resource.h"
#include "Project.h"

class ProjectSettingsDialog : public WIN::CDialogImpl<ProjectSettingsDialog>
{
public:
	ProjectSettingsDialog() = default;
	ProjectSettingsDialog(const ProjectSettingsDialog& rhs) = delete;
	~ProjectSettingsDialog() = default;

	ProjectSettingsDialog& operator=(const ProjectSettingsDialog& rhs) = delete;

	enum { IDD = IDD_PROJECT_SETTINGS };

	bool OnInitDialog(LPARAM param) override;
	void OnCommand(WORD code, WORD id, HWND hwnd) override;

	void SetProject(Project* project);

private:
	Project* project = nullptr;
	WIN::CComboBox comboStandard;
	WIN::CComboBox comboSubsystem;
	WIN::CComboBox comboWarnings;
	WIN::CComboBox comboOptimization;
	WIN::CComboBox comboTarget;
	WIN::CComboBox comboArchitecture;
};

