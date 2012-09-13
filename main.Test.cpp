////////////////////////////////////////////////////////////////////////////////
// Filename:    main.Test.cpp
// Description: This file defines the unit test entry point.
//
// Created:     2012-09-03 20:39:56
// Author:      Jacob Buysse
////////////////////////////////////////////////////////////////////////////////
#include <iostream>
#include <UnitTest/UnitTest.h>

int main(int argc, char** argv)
{
	UnitTest::TestRunner::RunTestsFromCommandLine(argc, argv);
	return 0;
}

