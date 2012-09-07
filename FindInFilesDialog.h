////////////////////////////////////////////////////////////////////////////////
// Filename:    FindInFilesDialog.h
// Description: This file declares the FindInFilesDialog class.
//
// Created:     2012-09-06 22:28:26
// Author:      Jacob Buysse
////////////////////////////////////////////////////////////////////////////////
#pragma once
#include "resource.h"
#include <CRL/CDialogImpl.h>
#include <string>

class FindInFilesDialog : public WIN::CDialogImpl<FindInFilesDialog>
{
public:
	FindInFilesDialog() = default;
	FindInFilesDialog(const FindInFilesDialog& rhs) = delete;
	~FindInFilesDialog() = default;

	FindInFilesDialog& operator=(const FindInFilesDialog& rhs) = delete;

	enum { IDD = IDD_FIND_IN_FILES };

	bool OnInitDialog(LPARAM lparam) override;
	void OnCommand(WORD code, WORD id, HWND hwnd) override;

	const std::string& GetFindText() const;

private:
	std::string findText;
};

