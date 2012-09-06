////////////////////////////////////////////////////////////////////////////////
// Filename:    pch.h
// Description: Pre-compiled header file.
//
// Created:     2012-08-04 10:14:54
// Author:      Jacob Buysse
//                                                              Modification Log
// When       Who               Description
// ---------- ----------------- ------------------------------------------------
////////////////////////////////////////////////////////////////////////////////
#pragma once

#define WIN32_LEAN_AND_MEAN
#define WINVER 0x0601
#define _WIN32_WINNT 0x0601
#define _WIN32_IE 0x0600
#include <windows.h>
#include <commctrl.h>
#include <commdlg.h>
#include <shellapi.h>
#include <oleidl.h>
#include <objidl.h>

#include <fstream>
#include <vector>
#include <list>
#include <map>
#include <set>
#include <algorithm>
#include <utility>

#include <CRL/WinUtility.h>
#include <CRL/MathUtility.h>
#include <CRL/TimeUtility.h>
#include <CRL/StringUtility.h>
#include <CRL/FileUtility.h>
#include <CRL/CStream.h>
#include <CRL/CSingleton.h>
#include <CRL/CRegSettings.h>

