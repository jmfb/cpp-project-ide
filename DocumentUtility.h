////////////////////////////////////////////////////////////////////////////////
// Filename:    DocumentUtility.h
// Description: This file declares all DocumentUtility member functions.
//
// Created:     2012-08-21 21:17:48
// Author:      Jacob Buysse
//                                                              Modification Log
// When       Who               Description
// ---------- ----------------- ------------------------------------------------
////////////////////////////////////////////////////////////////////////////////
#pragma once
#include "NewClassDialog.h"

class DocumentUtility
{
public:
	static std::string GenerateFileHeader(const std::string& fileName, const std::string& description);
	static std::string GenerateOneTimeInclude();
	static std::string GenerateClassHeader(
		const std::string& fileName,
		const std::string& className,
		const std::string& classNamespace,
		NewClassDialog::BigFourOption bigFourOption,
		bool useDefaultBigFour,
		NewClassDialog::FilesOption filesOption,
		bool unitTest);
	static std::string GenerateClassSource(
		const std::string& fileName,
		const std::string& className,
		const std::string& classNamespace,
		NewClassDialog::BigFourOption bigFourOption,
		bool useDefaultBigFour,
		NewClassDialog::FilesOption filesOption);
	static std::string GenerateClassUnitTest(
		const std::string& fileName,
		const std::string& className,
		const std::string& classNamespace);
};

