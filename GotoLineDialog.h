////////////////////////////////////////////////////////////////////////////////
// Filename:    GotoLineDialog.h
// Description: This file declares the GotoLineDialog class.
//
// Created:     2012-09-06 21:24:02
// Author:      Jacob Buysse
////////////////////////////////////////////////////////////////////////////////
#pragma once
#include "resource.h"
#include <CRL/CDialogImpl.h>

class GotoLineDialog : public WIN::CDialogImpl<GotoLineDialog>
{
public:
	GotoLineDialog() = default;
	GotoLineDialog(const GotoLineDialog& rhs) = delete;
	~GotoLineDialog() = default;

	GotoLineDialog& operator=(const GotoLineDialog& rhs) = delete;

	enum { IDD = IDD_GOTO_LINE };

	bool OnInitDialog(LPARAM lparam) override;
	void OnCommand(WORD code, WORD id, HWND hwnd) override;

	void SetLineCount(unsigned long value);
	unsigned long GetLine() const;
	
private:
	unsigned long lineCount;
	unsigned long line;
};

