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
	UnitTest::TestRunWriter writer(std::cout);
	UnitTest::TestRunner::RunTests(writer);
	std::cout << "Press enter to continue...";
	std::cin.get();
	return 0;
}

