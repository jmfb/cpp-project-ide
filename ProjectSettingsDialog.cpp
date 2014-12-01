////////////////////////////////////////////////////////////////////////////////
// Filename:    ProjectSettingsDialog.cpp
// Description: This file implements all ProjectSettingsDialog member functions.
//
// Created:     2012-08-30 21:38:10
// Author:      Jacob Buysse
////////////////////////////////////////////////////////////////////////////////
#include "pch.h"
#include "ProjectSettingsDialog.h"

bool ProjectSettingsDialog::OnInitDialog(LPARAM param)
{
	Center();
	SetDlgItemText(IDC_EDIT_NAME, project->GetName());
	SetDlgItemText(IDC_EDIT_DIRECTORY, project->GetOutputFolder());
	SetDlgItemText(IDC_EDIT_FILENAME, project->GetOutputFileName());

	comboStandard = GetDlgItem(IDC_COMBO_STANDARD);
	comboStandard.AddString("c++98");
	comboStandard.AddString("c++11");
	comboStandard.SelectString(project->GetStandard());

	comboSubsystem = GetDlgItem(IDC_COMBO_SUBSYSTEM);
	comboSubsystem.AddString("windows");
	comboSubsystem.AddString("console");
	comboSubsystem.SelectString(project->GetSubsystem());

	comboWarnings = GetDlgItem(IDC_COMBO_WARNINGS);
	comboWarnings.AddString("");
	comboWarnings.AddString("all");
	comboWarnings.AddString("pedantic");
	comboWarnings.AddString("extra");
	comboWarnings.SelectString(project->GetWarnings());

	if (project->GetWarningsAsErrors())
		SetDlgItemChecked(IDC_CHECK_WARNINGS_AS_ERRORS);

	comboOptimization = GetDlgItem(IDC_COMBO_OPTIMIZATION);
	comboOptimization.AddString("0");
	comboOptimization.AddString("1");
	comboOptimization.AddString("2");
	comboOptimization.AddString("3");
	comboOptimization.AddString("4");
	comboOptimization.AddString("s");
	comboOptimization.AddString("fast");
	comboOptimization.SelectString(project->GetOptimizationLevel());

	comboTarget = GetDlgItem(IDC_COMBO_TARGET);
	comboTarget.AddString("EXE");
	comboTarget.AddString("DLL");
	comboTarget.SelectString(project->GetTarget());

	comboArchitecture = GetDlgItem(IDC_COMBO_ARCHITECTURE);
	comboArchitecture.AddString("32-bit");
	comboArchitecture.AddString("64-bit");
	comboArchitecture.SelectString(project->GetArchitecture());

	if (project->GetDebugInfo())
		SetDlgItemChecked(IDC_CHECK_DEBUG);

	if (project->GetMultithreaded())
		SetDlgItemChecked(IDC_CHECK_MULTITHREADED);

	std::ostringstream includeDirectories;
	for (const auto& includeDirectory: project->GetIncludeDirectories())
		includeDirectories << includeDirectory << std::endl;
	SetDlgItemText(IDC_EDIT_INCLUDE_DIRECTORIES, STRING::replace(includeDirectories.str(), "\n", "\r\n"));

	std::ostringstream libraries;
	for (const auto& library: project->GetLibraries())
		libraries << library << std::endl;
	SetDlgItemText(IDC_EDIT_LIBRARIES, STRING::replace(libraries.str(), "\n", "\r\n"));

	std::ostringstream projectReferences;
	for (const auto& projectReference : project->GetProjectReferences())
		projectReferences << projectReference << std::endl;
	SetDlgItemText(IDC_EDIT_PROJECT_REFERENCES, STRING::replace(projectReferences.str(), "\n", "\r\n"));

	return true;
}

void ProjectSettingsDialog::OnCommand(WORD code, WORD id, HWND hwnd)
{
	switch(id)
	{
	case IDOK:
		{
			std::ostringstream problems;
			auto name = STRING::trim(GetDlgItemText(IDC_EDIT_NAME));
			if (name.empty())
				problems << "Please enter a valid project name." << std::endl;
			auto outputFolder = STRING::trim(GetDlgItemText(IDC_EDIT_DIRECTORY));
			if (outputFolder.empty())
				problems << "Please enter a valid output folder." << std::endl;
			auto outputFileName = STRING::trim(GetDlgItemText(IDC_EDIT_FILENAME));
			if (outputFileName.empty())
				problems << "Please enter a valid output file name." << std::endl;
			if (comboStandard.GetCurSel() == CB_ERR)
				problems << "Please select a valid standard." << std::endl;
			if (comboSubsystem.GetCurSel() == CB_ERR)
				problems << "Please select a valid subsystem." << std::endl;
			if (comboWarnings.GetCurSel() == CB_ERR)
				problems << "Please select a valid warnings level." << std::endl;
			if (comboOptimization.GetCurSel() == CB_ERR)
				problems << "Please select a valid optimization level." << std::endl;
			if (comboTarget.GetCurSel() == CB_ERR)
				problems << "Please select a valid target." << std::endl;
			if (comboArchitecture.GetCurSel() == CB_ERR)
				problems << "Please select a valid architecture." << std::endl;

			auto problemsText = problems.str();
			if (problemsText.empty())
			{
				project->SetName(name);
				project->SetOutputFolder(outputFolder);
				project->SetOutputFileName(outputFileName);
				project->SetStandard(comboStandard.GetText());
				project->SetSubsystem(comboSubsystem.GetText());
				project->SetWarnings(comboWarnings.GetText());
				project->SetWarningsAsErrors(IsDlgItemChecked(IDC_CHECK_WARNINGS_AS_ERRORS));
				project->SetOptimizationLevel(comboOptimization.GetText());
				project->SetTarget(comboTarget.GetText());
				project->SetArchitecture(comboArchitecture.GetText());
				project->SetDebugInfo(IsDlgItemChecked(IDC_CHECK_DEBUG));
				project->SetMultithreaded(IsDlgItemChecked(IDC_CHECK_MULTITHREADED));

				std::list<std::string> includeDirectories;
				std::istringstream inIncludeDirectories(GetDlgItemText(IDC_EDIT_INCLUDE_DIRECTORIES));
				std::string includeDirectory;
				while (std::getline(inIncludeDirectories, includeDirectory))
				{
					includeDirectory = STRING::trim(includeDirectory);
					if (!includeDirectory.empty())
						includeDirectories.push_back(includeDirectory);
				}
				project->SetIncludeDirectories(includeDirectories);

				std::list<std::string> libraries;
				std::istringstream inLibraries(GetDlgItemText(IDC_EDIT_LIBRARIES));
				std::string library;
				while (std::getline(inLibraries, library))
				{
					library = STRING::trim(library);
					if (!library.empty())
						libraries.push_back(library);
				}
				project->SetLibraries(libraries);

				std::vector<std::string> projectReferences;
				std::istringstream inProjectReferences(GetDlgItemText(IDC_EDIT_PROJECT_REFERENCES));
				std::string projectReference;
				while (std::getline(inProjectReferences, projectReference))
				{
					projectReference = STRING::trim(projectReference);
					if (!projectReference.empty())
						projectReferences.push_back(projectReference);
				}
				project->SetProjectReferences(projectReferences);

				End(id);
			}
			else
			{
				MsgBox(STRING::replace(problemsText, "\n", "\r\n"), "Invalid Settings", MB_OK|MB_ICONEXCLAMATION);
			}
		}
		break;

	case IDCANCEL:
		End(id);
		break;
	}
}

void ProjectSettingsDialog::SetProject(Project* project)
{
	this->project = project;
}

