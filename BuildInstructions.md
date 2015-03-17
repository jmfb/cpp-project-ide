# Instructions to Build cpp-project.exe #

  * Get the latest version of MinGW
    * http://sourceforge.net/projects/mingwbuilds/
    * NOTE: Use the installer and specify posix threads/seh exceptions.
  * Get the latest version of the cpp-project.exe from:
    * https://drive.google.com/open?id=0B-32hiIMW3e7d1lfdHJTLTdVN28&authuser=0
    * NOTE: You could manually use MinGW to build, but a makefile is not provided.  Use the cpp-project.cpp-project XML file to see g++ command line settings that are used.
  * Get the latest version of the UnitTest framework.
    * http://code.google.com/p/cpp-unit-test/
  * Get the latest version of the CRL framework.
    * http://code.google.com/p/cpp-code-reuse-library/
  * Get the latest version of the XML framework.
    * http://rapidxml.sourceforge.net/
    * NOTE: As of 1.13, you will need to make modifications to rapidxml\_print.hpp to get this to compile:
      * Remove the last two print overloads.  The overloads will confuse the argument deduction rules with the original templated version and will never pick the correct version.
      * Wrap all global functions in the internal namespace in a dummy class and make them all static.  The one pass rule for global functions will not properly compile with the order of declarations, but member function name look up will.
      * Update the usage of internal::print\_node to include dummy class scope.
  * Get the latest version of the code.
    * http://code.google.com/p/cpp-project-ide/
  * Open the project in cpp-project.exe.
  * Configure the include directories for the libraries being used.
    * Recommended include directory: c:\save\code\
    * Subdirectories set up as follows:
      * UnitTest -> <UnitTest/...>
      * CRL -> <CRL/...>
      * XML -> <rapidxml-1.13/...>
  * Build (F7).