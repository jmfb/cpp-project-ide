////////////////////////////////////////////////////////////////////////////////
// Filename:    NewClassDialog.cpp
// Description: This file implements all NewClassDialog member funcitons.
//
// Created:     2012-08-26 19:14:30
// Author:      Jacob Buysse
////////////////////////////////////////////////////////////////////////////////
#include "pch.h"
#include "NewClassDialog.h"

bool NewClassDialog::OnInitDialog(LPARAM param)
{
	Center();
	SetDlgItemChecked(IDC_OPTION_BIG4_NORMAL);
	SetDlgItemChecked(IDC_CHECK_BIG4_DEFAULT);
	SetDlgItemChecked(IDC_OPTION_FILES_SOURCE);
	SetDlgItemChecked(IDC_CHECK_UNIT_TEST);
	return true;
}

void NewClassDialog::OnCommand(WORD code, WORD id, HWND hwnd)
{
	std::ostringstream problems;
	switch(id)
	{
	case IDOK:
		className = STRING::trim(GetDlgItemText(IDC_EDIT_NAME));
		classNamespace = STRING::trim(GetDlgItemText(IDC_EDIT_NAMESPACE));
		folder = STRING::trim(GetDlgItemText(IDC_EDIT_DIRECTORY));
		if (IsDlgItemChecked(IDC_OPTION_BIG4_NORMAL))
			bigFourOption = BigFourOption::Normal;
		else if (IsDlgItemChecked(IDC_OPTION_BIG4_NOCOPY))
			bigFourOption = BigFourOption::NoCopy;
		else
			problems << "Please enter a Big Four option." << std::endl;
		useDefaultBigFour = IsDlgItemChecked(IDC_CHECK_BIG4_DEFAULT);
		if (IsDlgItemChecked(IDC_OPTION_FILES_SOURCE))
			filesOption = FilesOption::Source;
		else if (IsDlgItemChecked(IDC_OPTION_FILES_INLINE))
			filesOption = FilesOption::Inline;
		else if (IsDlgItemChecked(IDC_OPTION_FILES_HEADER))
			filesOption = FilesOption::Header;
		else
			problems << "Please enter a Files option." << std::endl;
		if (className.empty())
			problems << "Please enter a valid Class Name." << std::endl;
		unitTest = IsDlgItemChecked(IDC_CHECK_UNIT_TEST);
		if (!problems.str().empty())
		{
			MsgBox(STRING::replace(problems.str(), "\n", "\r\n"), "Invalid Data", MB_OK|MB_ICONEXCLAMATION);
			return;
		}

		//TODO: check if the files already exist and prompt for overwrite
		End(id);
		break;

	case IDCANCEL:
		End(id);
		break;
	}
}

const std::string& NewClassDialog::GetClassName() const
{
	return className;
}

const std::string& NewClassDialog::GetClassNamespace() const
{
	return classNamespace;
}

const std::string& NewClassDialog::GetFolder() const
{
	return folder;
}

bool NewClassDialog::GetUseDefaultBigFour() const
{
	return useDefaultBigFour;
}

NewClassDialog::BigFourOption NewClassDialog::GetBigFourOption() const
{
	return bigFourOption;
}

NewClassDialog::FilesOption NewClassDialog::GetFilesOption() const
{
	return filesOption;
}

bool NewClassDialog::GetUnitTest() const
{
	return unitTest;
}

