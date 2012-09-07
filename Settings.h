////////////////////////////////////////////////////////////////////////////////
// Filename:    Settings.h
// Description: This file declares the Settings class.
//
// Created:     2012-09-07 10:02:35
// Author:      Jacob Buysse
////////////////////////////////////////////////////////////////////////////////
#pragma once
#include <CRL/CRegSettings.h>
#include <vector>
#include <string>

class Settings : public REG::CRegSettings
{
public:
	Settings() = default;
	Settings(const Settings& rhs) = delete;
	~Settings() = default;

	Settings& operator=(const Settings& rhs) = delete;

	std::vector<std::string> GetSystemIncludeDirectories();
	void SetSystemIncludeDirectories(const std::vector<std::string>& value);
};

