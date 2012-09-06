////////////////////////////////////////////////////////////////////////////////
// Filename:    DocumentUtility.cpp
// Description: This file implements all DocumentUtility member functions.
//
// Created:     2012-08-21 21:29:26
// Author:      Jacob Buysse
//                                                              Modification Log
// When       Who               Description
// ---------- ----------------- ------------------------------------------------
////////////////////////////////////////////////////////////////////////////////
#include "pch.h"
#include "DocumentUtility.h"

std::string DocumentUtility::GenerateFileHeader(const std::string& fileName, const std::string& description)
{
	std::ostringstream out;
	out << std::string(80, '/') << std::endl
		<< "// Filename:    " << FSYS::GetFileName(fileName) << std::endl
		<< "// Description: " << description << std::endl
		<< "//" << std::endl
		<< "// Created:     " << TIME::Format(TIME::GetDateTime(), "{yyyy}-{MM}-{dd} {HH}:{mm}:{ss}") << std::endl
		<< "// Author:      Jacob Buysse" << std::endl
		<< std::string(80, '/') << std::endl;
	return out.str();
}

std::string DocumentUtility::GenerateOneTimeInclude()
{
	std::ostringstream out;
	out << "#pragma once" << std::endl;
	return out.str();
}

std::string DocumentUtility::GenerateClassHeader(
	const std::string& fileName,
	const std::string& className,
	const std::string& classNamespace,
	NewClassDialog::BigFourOption bigFourOption,
	bool useDefaultBigFour,
	NewClassDialog::FilesOption filesOption,
	bool unitTest)
{
	std::ostringstream out;

	//Generate the file documentation header and the include guard statement
	auto description = "This file declares the " + className + " class.";
	if (filesOption == NewClassDialog::FilesOption::Header)
		description = "This file inlines the entire " + className + " class.";
	out << GenerateFileHeader(fileName, description) << GenerateOneTimeInclude() << std::endl;

	//Start the optional namespace (store indent so we know to indent the class that follows)
	std::string indent;
	if (!classNamespace.empty())
	{
		indent = "\t";
		out << "namespace " << classNamespace << std::endl
			<< "{" << std::endl;
	}

	//Start the class declaration
	out << indent << "class " << className << std::endl
		<< indent << "{" << std::endl
		<< indent << "public:" << std::endl;

	//Generate the default constructor
	out << indent << "\t" << className << "()";
	if (useDefaultBigFour)
		out << " = default;" << std::endl;
	else if (filesOption == NewClassDialog::FilesOption::Header)
		out << std::endl
			<< indent << "\t{" << std::endl
			<< indent << "\t}" << std::endl
			<< std::endl;
	else
		out << ";" << std::endl;

	//Generate the copy constructor
	out << indent << "\t" << className << "(const " << className << "& rhs)";
	if (bigFourOption == NewClassDialog::BigFourOption::NoCopy)
	{
		out << " = delete;" << std::endl;
		if (!useDefaultBigFour && filesOption == NewClassDialog::FilesOption::Header)
			out << std::endl;
	}
	else if (useDefaultBigFour)
		out << " = default;" << std::endl;
	else if (filesOption == NewClassDialog::FilesOption::Header)
		out << std::endl
			<< indent << "\t{" << std::endl
			<< indent << "\t\toperator=(rhs);" << std::endl
			<< indent << "\t}" << std::endl
			<< std::endl;
	else
		out << ";" << std::endl;

	//Generate the destructor
	out << indent << "\t~" << className << "()";
	if (useDefaultBigFour)
		out << " = default;" << std::endl
			<< std::endl;
	else if (filesOption == NewClassDialog::FilesOption::Header)
		out << std::endl
			<< indent << "\t{" << std::endl
			<< indent << "\t}" << std::endl
			<< std::endl;
	else
		out << ";" << std::endl
			<< std::endl;

	//Generate the copy-assignment operator
	out << indent << "\t" << className << "& operator=(const " << className << "& rhs)";
	if (bigFourOption == NewClassDialog::BigFourOption::NoCopy)
		out << " = delete;" << std::endl
			<< std::endl;
	else if (useDefaultBigFour)
		out << " = default;" << std::endl
			<< std::endl;
	else if (filesOption == NewClassDialog::FilesOption::Header)
		out << std::endl
			<< indent << "\t{" << std::endl
			<< indent << "\t\tif (this != &rhs)" << std::endl
			<< indent << "\t\t{" << std::endl
			<< indent << "\t\t\t//TODO:" << std::endl
			<< indent << "\t\t}" << std::endl
			<< indent << "\t\treturn *this;" << std::endl
			<< indent << "\t}" << std::endl
			<< std::endl;
	else
		out << ";" << std::endl
			<< std::endl;

	//Close the class
	if (unitTest)
	{
		out << indent << "private:" << std::endl
			<< indent << "\tfriend class " << className << "Test;" << std::endl;
	}
	out << indent << "};" << std::endl;

	//Close the optional namespace
	if (!classNamespace.empty())
		out << "}" << std::endl;
	out << std::endl;

	//Include the optional inline file at the very end (outside the namespace)
	if (filesOption == NewClassDialog::FilesOption::Inline)
	{
		out << "#include \"" << className << ".inl\"" << std::endl
			<< std::endl;
	}

	return out.str();
}

std::string DocumentUtility::GenerateClassSource(
	const std::string& fileName,
	const std::string& className,
	const std::string& classNamespace,
	NewClassDialog::BigFourOption bigFourOption,
	bool useDefaultBigFour,
	NewClassDialog::FilesOption filesOption)
{
	std::ostringstream out;

	//Generate the file documentation header (and the include guard statement for inline files)
	auto description = "This file implements all " + className + " member functions.";
	out << GenerateFileHeader(fileName, description);
	std::string prefix;
	if (filesOption == NewClassDialog::FilesOption::Inline)
	{
		prefix = "inline ";
		out << GenerateOneTimeInclude();
	}
	else
	{
		out << "#include \"" << className << ".h\"" << std::endl;
	}
	out << std::endl;

	//Generate the optional namespace opening (store indent)
	std::string indent;
	if (!classNamespace.empty())
	{
		indent = "\t";
		out << "namespace " << classNamespace << std::endl
			<< "{" << std::endl;
	}

	//Only generate function bodies when not using the default implementation.
	if (!useDefaultBigFour)
	{
		//Generate the default constructor
		out << indent << prefix << className << "::" << className << "()" << std::endl
			<< indent << "{" << std::endl
			<< indent << "}" << std::endl
			<< std::endl;

		//Only generate the copy constructor if allowing copy
		if (bigFourOption != NewClassDialog::BigFourOption::NoCopy)
			out << indent << prefix << className << "::" << className << "(const " << className << "& rhs)" << std::endl
				<< indent << "{" << std::endl
				<< indent << "\toperator=(rhs);" << std::endl
				<< indent << "}" << std::endl
				<< std::endl;

		//Generate the destructor
		out << indent << prefix << className << "::~" << className << "()" << std::endl
			<< indent << "{" << std::endl
			<< indent << "}" << std::endl
			<< std::endl;

		//Only generate the copy-assignment operator if allowing copy
		if (bigFourOption != NewClassDialog::BigFourOption::NoCopy)
			out << indent << prefix << className << "& " << className << "::operator=(const " << className << "& rhs)" << std::endl
				<< indent << "{" << std::endl
				<< indent << "\tif (this != &rhs)" << std::endl
				<< indent << "\t{" << std::endl
				<< indent << "\t\t//TODO:" << std::endl
				<< indent << "\t}" << std::endl
				<< indent << "\treturn *this;" << std::endl
				<< indent << "}" << std::endl
				<< std::endl;
	}

	//Close the optional namespace
	if (!classNamespace.empty())
		out << "}" << std::endl;
	out << std::endl;

	return out.str();
}

std::string DocumentUtility::GenerateClassUnitTest(
	const std::string& fileName,
	const std::string& className,
	const std::string& classNamespace)
{
	std::ostringstream out;

	//Generate the file documentation header
	auto description = "This file defines all " + className + " unit tests.";
	out << GenerateFileHeader(fileName, description);
	out << "#include \"" << className << ".h\"" << std::endl
		<< "#include <UnitTest/UnitTest.h>" << std::endl
		<< "using UnitTest::Assert;" << std::endl
		<< std::endl;

	std::string indent;
	if (!classNamespace.empty())
	{
		out << "namespace " << classNamespace << std::endl
			<< "{" << std::endl;
		indent = "\t";
	}

	out << indent << "TEST_CLASS(" << className << "Test)" << std::endl
		<< indent << "{" << std::endl
		<< indent << "public:" << std::endl
		<< indent << "\t" << className << "Test()" << std::endl
		<< indent << "\t{" << std::endl
		<< indent << "\t}" << std::endl
		<< std::endl
		<< indent << "};" << std::endl;

	if (!classNamespace.empty())
		out << "}" << std::endl;
	out << std::endl;
	return out.str();
}

