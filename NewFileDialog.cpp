////////////////////////////////////////////////////////////////////////////////
// Filename:    NewFileDialog.cpp
// Description: This file implements all NewFileDialog member functions.
//
// Created:     2012-08-25 22:31:29
// Author:      Jacob Buysse
////////////////////////////////////////////////////////////////////////////////
#include "pch.h"
#include "NewFileDialog.h"

bool NewFileDialog::OnInitDialog(LPARAM param)
{
	Center();
	return true;
}

void NewFileDialog::OnCommand(WORD code, WORD id, HWND hwnd)
{
	switch(id)
	{
	case IDOK:
		name = STRING::trim(GetDlgItemText(IDC_EDIT_NAME));
		directory = STRING::trim(GetDlgItemText(IDC_EDIT_DIRECTORY));
		if (name.empty())
			MsgBox("Please enter a file name.", "Missing Information", MB_OK|MB_ICONEXCLAMATION);
		else
		{
			auto extension = FSYS::GetFileExt(name);
			if (extension.empty())
			{
				auto result = MsgBox(
					"Are you sure you want to create a file without an extension?",
					"Confirm File Name",
					MB_YESNOCANCEL|MB_ICONQUESTION);
				if (result == IDCANCEL)
					End(IDCANCEL);
				else if (result == IDNO)
					return;
				else
					End(IDOK);
			}
			else
			{
				End(IDOK);
			}
		}
		break;

	case IDCANCEL:
		End(id);
		break;
	}
}

const std::string& NewFileDialog::GetName() const
{
	return name;
}

const std::string& NewFileDialog::GetDirectory() const
{
	return directory;
}

