////////////////////////////////////////////////////////////////////////////////
// Filename:    FileLocation.cpp
// Description: This file implements all FileLocation member functions.
//
// Created:     2012-09-03 18:32:52
// Author:      Jacob Buysse
////////////////////////////////////////////////////////////////////////////////
#include "FileLocation.h"
#include <CRL/StringUtility.h>

FileLocation::FileLocation(const std::string& text)
{
	//2> FileLocation.h:14:21: error: 'string' in namespace 'std' does not name a type
	auto prefix = text.find("> ");
	if (prefix == std::string::npos)
		return;
	auto firstColon = text.find(':', prefix + 2);
	if (firstColon == std::string::npos)
		return;
	auto secondColon = text.find(':', firstColon + 1);
	if (secondColon == std::string::npos)
		return;
	auto thirdColon = text.find(':', secondColon + 1);
	if (thirdColon == std::string::npos)
		return;
	fileName = text.substr(prefix + 2, firstColon - prefix - 2);
	line = STRING::from_string<unsigned long>(text.substr(firstColon + 1, secondColon - firstColon - 1));
	column = STRING::from_string<unsigned long>(text.substr(secondColon + 1, thirdColon - secondColon - 1));
}

bool FileLocation::IsValid() const
{
	return !fileName.empty();
}

const std::string& FileLocation::GetFileName() const
{
	return fileName;
}

unsigned long FileLocation::GetLine() const
{
	return line;
}

unsigned long FileLocation::GetColumn() const
{
	return column;
}

