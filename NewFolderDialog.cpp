////////////////////////////////////////////////////////////////////////////////
// Filename:    NewFolderDialog.cpp
// Description: This file implements all NewFolderDialog member functions.
//
// Created:     2012-08-25 21:18:22
// Author:      Jacob Buysse
////////////////////////////////////////////////////////////////////////////////
#include "pch.h"
#include "NewFolderDialog.h"

bool NewFolderDialog::OnInitDialog(LPARAM param)
{
	Center();
	return true;
}

void NewFolderDialog::OnCommand(WORD code, WORD id, HWND hwnd)
{
	switch(id)
	{
	case IDOK:
		name = STRING::trim(GetDlgItemText(IDC_EDIT_NAME));
		if (name.empty())
			MsgBox("Please enter a folder name.", "Missing Information", MB_OK|MB_ICONEXCLAMATION);
		else
			End(IDOK);
		break;

	case IDCANCEL:
		End(id);
		break;
	}
}

const std::string& NewFolderDialog::GetName() const
{
	return name;
}

