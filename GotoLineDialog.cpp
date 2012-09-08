////////////////////////////////////////////////////////////////////////////////
// Filename:    GotoLineDialog.cpp
// Description: This file implements all GotoLineDialog member functions.
//
// Created:     2012-09-06 21:24:02
// Author:      Jacob Buysse
////////////////////////////////////////////////////////////////////////////////
#include "GotoLineDialog.h"
#include <sstream>

bool GotoLineDialog::OnInitDialog(LPARAM lparam)
{
	Center();

	std::ostringstream out;
	out << "Line (1-" << lineCount << "):";
	SetDlgItemText(IDC_LABEL_LINE, out.str());

	return true;
}

void GotoLineDialog::OnCommand(WORD code, WORD id, HWND hwnd)
{
	switch(id)
	{
	case IDOK:
		{
			bool success = false;
			line = GetDlgItemInt(IDC_EDIT_LINE, &success);
			if (!success || line == 0 || line > lineCount)
				MsgBox("Please enter a valid line number.", "Invalid Line Number", MB_OK|MB_ICONEXCLAMATION);
			else
				End(id);
		}
		break;

	case IDCANCEL:
		End(id);
		break;
	}
}

void GotoLineDialog::SetLineCount(unsigned long value)
{
	lineCount = value;
}

unsigned long GotoLineDialog::GetLine() const
{
	return line;
}

