////////////////////////////////////////////////////////////////////////////////
// Filename:    NewFileDialog.h
// Description: This file declares the NewFileDialog class.
//
// Created:     2012-08-25 22:29:42
// Author:      Jacob Buysse
////////////////////////////////////////////////////////////////////////////////
#pragma once
#include <string>
#include "resource.h"

class NewFileDialog : public WIN::CDialogImpl<NewFileDialog>
{
public:
	NewFileDialog() = default;
	NewFileDialog(const NewFileDialog& rhs) = delete;
	~NewFileDialog() = default;

	NewFileDialog& operator=(const NewFileDialog& rhs) = default;

	enum { IDD = IDD_NEW_FILE };

	bool OnInitDialog(LPARAM param) override;
	void OnCommand(WORD code, WORD id, HWND hwnd) override;

	const std::string& GetName() const;
	const std::string& GetDirectory() const;

private:
	std::string name;
	std::string directory;
};

