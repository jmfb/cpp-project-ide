////////////////////////////////////////////////////////////////////////////////
// Filename:    Settings.cpp
// Description: This file implements all Settings member functions.
//
// Created:     2012-09-07 10:02:35
// Author:      Jacob Buysse
////////////////////////////////////////////////////////////////////////////////
#include "Settings.h"
#include <CRL/StringUtility.h>
#include <iterator>

constexpr auto systemIncludeDirectoriesName = "SystemIncludeDirectories";
constexpr auto systemIncludeDirectoriesDefault =
	R"(c:\program files\mingw\x86_64-w64-mingw32\include;)"
	R"(c:\program files\mingw\lib\gcc\x86_64-w64-mingw32\4.7.0\include;)"
	R"(c:\program files\mingw\lib\gcc\x86_64-w64-mingw32\4.7.0\include\c++;)"
	R"(c:\program files\mingw\lib\gcc\x86_64-w64-mingw32\4.7.0\include\c++\x86_64-w64-mingw32)";

std::vector<std::string> Settings::GetSystemIncludeDirectories()
{
	auto value = GetString(systemIncludeDirectoriesName, systemIncludeDirectoriesDefault);
	std::vector<std::string> result;
	STRING::split(value, ";", std::back_inserter(result));
	return result;
}

void Settings::SetSystemIncludeDirectories(const std::vector<std::string>& value)
{
	SetString(systemIncludeDirectoriesName, STRING::join(value, ";"));
}

