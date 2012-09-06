////////////////////////////////////////////////////////////////////////////////
// Filename:    NewFolderDialog.h
// Description: This file declares the NewFolderDialog class.
//
// Created:     2012-08-25 21:15:40
// Author:      Jacob Buysse
////////////////////////////////////////////////////////////////////////////////
#pragma once
#include <string>
#include "resource.h"

class NewFolderDialog : public WIN::CDialogImpl<NewFolderDialog>
{
public:
	NewFolderDialog() = default;
	NewFolderDialog(const NewFolderDialog& rhs) = delete;
	~NewFolderDialog() = default;

	NewFolderDialog& operator=(const NewFolderDialog& rhs) = delete;

	enum { IDD = IDD_NEW_FOLDER };

	bool OnInitDialog(LPARAM param) override;
	void OnCommand(WORD code, WORD id, HWND hwnd) override;

	const std::string& GetName() const;

private:
	std::string name;
};

