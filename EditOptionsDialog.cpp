////////////////////////////////////////////////////////////////////////////////
// Filename:    EditOptionsDialog.cpp
// Description: This file implements all EditOptionsDialog member functions.
//
// Created:     2012-09-07 10:35:04
// Author:      Jacob Buysse
////////////////////////////////////////////////////////////////////////////////
#include "EditOptionsDialog.h"
#include "Settings.h"
#include <CRL/StringUtility.h>

bool EditOptionsDialog::OnInitDialog(LPARAM lparam)
{
	Center();
	Settings settings;
	SetDlgItemText(IDC_EDIT_INCLUDE_DIRECTORIES, STRING::join(settings.GetSystemIncludeDirectories(), "\r\n"));
	return true;
}

void EditOptionsDialog::OnCommand(WORD code, WORD id, HWND hwnd)
{
	switch(id)
	{
	case IDOK:
		{
			Settings settings;
			std::vector<std::string> systemIncludeDirectories;
			STRING::split(STRING::trim(GetDlgItemText(IDC_EDIT_INCLUDE_DIRECTORIES)), "\r\n", std::back_inserter(systemIncludeDirectories));
			settings.SetSystemIncludeDirectories(systemIncludeDirectories);
		}
		End(id);
		break;
	case IDCANCEL:
		End(id);
		break;
	}
}

