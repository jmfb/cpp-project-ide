////////////////////////////////////////////////////////////////////////////////
// Filename:    TestResultsTarget.h
// Description: This file declares the TestResultsTarget interface.
//
// Created:     2012-09-13 00:46:17
// Author:      Jacob Buysse
////////////////////////////////////////////////////////////////////////////////
#pragma once
#include <string>

class TestResultsTarget
{
public:
	virtual void TestRunning(unsigned long index) = 0;
	virtual void TestPassed(unsigned long index) = 0;
	virtual void TestFailed(unsigned long index, const std::string& description) = 0;
};

