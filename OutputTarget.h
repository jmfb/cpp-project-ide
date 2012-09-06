////////////////////////////////////////////////////////////////////////////////
// Filename:    OutputTarget.h
// Description: This file declares the OutputTarget interface.
//
// Created:     2012-09-03 23:45:44
// Author:      Jacob Buysse
////////////////////////////////////////////////////////////////////////////////
#pragma once

class OutputTarget
{
public:
	virtual void Clear() = 0;
	virtual void Append(const std::string& message) = 0;
	virtual void ProcessBuildMessage(unsigned long id, const std::string& message) = 0;
};

