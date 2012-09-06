////////////////////////////////////////////////////////////////////////////////
// Filename:    FileLocation.h
// Description: This file declares the FileLocation class.
//
// Created:     2012-09-03 18:32:52
// Author:      Jacob Buysse
////////////////////////////////////////////////////////////////////////////////
#pragma once

#include <string>

class FileLocation
{
public:
	FileLocation() = default;
	FileLocation(const std::string& text);
	FileLocation(const FileLocation& rhs) = default;
	~FileLocation() = default;

	FileLocation& operator=(const FileLocation& rhs) = default;

	bool IsValid() const;

	const std::string& GetFileName() const;
	unsigned long GetLine() const;
	unsigned long GetColumn() const;

public:
	std::string fileName;
	unsigned long line = 0;
	unsigned long column = 0;
};

