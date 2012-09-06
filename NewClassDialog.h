////////////////////////////////////////////////////////////////////////////////
// Filename:    NewClassDialog.h
// Description: This file declares the NewClassDialog class.
//
// Created:     2012-08-26 19:07:28
// Author:      Jacob Buysse
////////////////////////////////////////////////////////////////////////////////
#pragma once
#include <string>
#include "resource.h"

class NewClassDialog : public WIN::CDialogImpl<NewClassDialog>
{
public:
	NewClassDialog() = default;
	NewClassDialog(const NewClassDialog& rhs) = delete;
	~NewClassDialog() = default;

	NewClassDialog& operator=(const NewClassDialog& rhs) = delete;

	enum { IDD = IDD_NEW_CLASS };

	bool OnInitDialog(LPARAM param) override;
	void OnCommand(WORD code, WORD id, HWND hwnd) override;

	enum class BigFourOption
	{
		Normal,
		NoCopy
	};

	enum class FilesOption
	{
		Source,
		Inline,
		Header
	};

	const std::string& GetClassName() const;
	const std::string& GetClassNamespace() const;
	const std::string& GetFolder() const;
	bool GetUseDefaultBigFour() const;
	BigFourOption GetBigFourOption() const;
	FilesOption GetFilesOption() const;
	bool GetUnitTest() const;

private:
	std::string className;
	std::string classNamespace;
	std::string folder;
	bool useDefaultBigFour = false;
	BigFourOption bigFourOption = BigFourOption::Normal;
	FilesOption filesOption = FilesOption::Source;
	bool unitTest = false;
};

