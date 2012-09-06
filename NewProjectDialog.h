////////////////////////////////////////////////////////////////////////////////
// Filename:    NewProjectDialog.h
// Description: This file declares the NewProjectDialog class.  This class is
//              the code behind the dialog that is shown when the user creates
//              a new project.
//
// Created:     2012-08-05 23:26:27
// Author:      Jacob Buysse
//                                                              Modification Log
// When       Who               Description
// ---------- ----------------- ------------------------------------------------
////////////////////////////////////////////////////////////////////////////////
#pragma once
#include "resource.h"

class NewProjectDialog : public WIN::CDialogImpl<NewProjectDialog>
{
public:
	NewProjectDialog();
	NewProjectDialog(const NewProjectDialog& rhs) = delete;
	~NewProjectDialog();

	NewProjectDialog& operator=(const NewProjectDialog& rhs) = delete;

	enum { IDD = IDD_NEW_PROJECT };

	bool OnInitDialog(LPARAM param) override;
	void OnCommand(WORD code, WORD id, HWND hwnd) override;

	const std::string& GetDirectory() const;
	const std::string& GetName() const;

private:
	std::string directory;
	std::string name;
};

