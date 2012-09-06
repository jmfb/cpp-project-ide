////////////////////////////////////////////////////////////////////////////////
// Filename:    NewProjectDialog.cpp
// Description: This file implements all NewProjectDialog member functions.
//
// Created:     2012-08-05 23:29:20
// Author:      Jacob Buysse
//                                                              Modification Log
// When       Who               Description
// ---------- ----------------- ------------------------------------------------
////////////////////////////////////////////////////////////////////////////////
#include "pch.h"
#include "NewProjectDialog.h"

NewProjectDialog::NewProjectDialog()
{
}

NewProjectDialog::~NewProjectDialog()
{
}

bool NewProjectDialog::OnInitDialog(LPARAM param)
{
	Center();
	SetDlgItemText(IDC_EDIT_DIRECTORY, "c:\\save\\code");
	GetDlgItem(IDC_EDIT_NAME).SetFocus();
	return false;
}

void NewProjectDialog::OnCommand(WORD code, WORD id, HWND hwnd)
{
	switch(id)
	{
	case IDOK:
		directory = STRING::trim(GetDlgItemText(IDC_EDIT_DIRECTORY));
		name = STRING::trim(GetDlgItemText(IDC_EDIT_NAME));
		if (directory.empty())
			MsgBox("Please enter a directory.", "Missing Information", MB_OK|MB_ICONEXCLAMATION);
		else if (name.empty())
			MsgBox("Please enter a project name.", "Missing Information", MB_OK|MB_ICONEXCLAMATION);
		else
			End(IDOK);
		break;

	case IDCANCEL:
		End(id);
		break;
	}
}

const std::string& NewProjectDialog::GetDirectory() const
{
	return directory;
}

const std::string& NewProjectDialog::GetName() const
{
	return name;
}

