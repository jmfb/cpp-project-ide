////////////////////////////////////////////////////////////////////////////////
// Filename:    EditOptionsDialog.h
// Description: This file declares the EditOptionsDialog class.
//
// Created:     2012-09-07 10:35:04
// Author:      Jacob Buysse
////////////////////////////////////////////////////////////////////////////////
#pragma once
#include "resource.h"
#include <CRL/CDialogImpl.h>

class EditOptionsDialog : public WIN::CDialogImpl<EditOptionsDialog>
{
public:
	EditOptionsDialog() = default;
	EditOptionsDialog(const EditOptionsDialog& rhs) = delete;
	~EditOptionsDialog() = default;

	EditOptionsDialog& operator=(const EditOptionsDialog& rhs) = delete;

	enum { IDD = IDD_EDIT_OPTIONS };

	bool OnInitDialog(LPARAM lparam) override;
	void OnCommand(WORD code, WORD id, HWND hwnd) override;
};

