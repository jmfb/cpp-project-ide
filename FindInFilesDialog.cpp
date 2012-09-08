////////////////////////////////////////////////////////////////////////////////
// Filename:    FindInFilesDialog.cpp
// Description: This file implements all FindInFilesDialog member functions.
//
// Created:     2012-09-06 22:28:26
// Author:      Jacob Buysse
////////////////////////////////////////////////////////////////////////////////
#include "FindInFilesDialog.h"

bool FindInFilesDialog::OnInitDialog(LPARAM lparam)
{
	Center();
	return true;
}

void FindInFilesDialog::OnCommand(WORD code, WORD id, HWND hwnd)
{
	switch(id)
	{
	case IDOK:
		findText = STRING::trim(GetDlgItemText(IDC_EDIT_FIND_TEXT));
		if (findText.empty())
			MsgBox("Please enter the text to find.", "Invalid Find Text", MB_OK|MB_ICONEXCLAMATION);
		else
			End(id);
		break;

	case IDCANCEL:
		End(id);
		break;
	}
}

const std::string& FindInFilesDialog::GetFindText() const
{
	return findText;
}

