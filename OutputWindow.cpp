////////////////////////////////////////////////////////////////////////////////
// Filename:    OutputWindow.cpp
// Description: This file implements all OutputWindow member functions.
//
// Created:     2012-08-05 00:01:06
// Author:      Jacob Buysse
//                                                              Modification Log
// When       Who               Description
// ---------- ----------------- ------------------------------------------------
////////////////////////////////////////////////////////////////////////////////
#include "pch.h"
#include "OutputWindow.h"
#include "resource.h"

const auto backgroundColor = RGB(255, 255, 255);
const auto findWindowHeight = 20;
const auto findWindowPadding = 4;

OutputWindow::OutputWindow()
{
}

OutputWindow::~OutputWindow()
{
}

void OutputWindow::SetupClass(WNDCLASSEX& cls)
{
	cls.lpszClassName = "OutputWindow";
	cls.hbrBackground = ::GetSysColorBrush(COLOR_3DFACE);
}

bool OutputWindow::OnCreate(CREATESTRUCT* cs)
{
	findWindow.Create(
		GetHWND(),
		nullptr,
		nullptr,
		WS_CHILD|WS_VISIBLE|WS_CLIPSIBLINGS|WS_CLIPCHILDREN,
		WS_EX_CONTROLPARENT);
	
	output.Create(
		GetHWND(),
		1001,
		"",
		WS_CHILD|WS_VISIBLE|WS_CLIPCHILDREN|WS_CLIPSIBLINGS|WS_HSCROLL|WS_VSCROLL|
		ES_LEFT|ES_AUTOHSCROLL|ES_MULTILINE|ES_AUTOVSCROLL|ES_READONLY,
		WS_EX_CLIENTEDGE,
		GetClientRect());

	auto dc = ::GetDC(GetHWND());
	font.Create("Courier New", WIN::CFont::CalcHeight(dc, 8));
	::ReleaseDC(GetHWND(), dc);

	output.SetFont(font.Get());

	background.Create(backgroundColor);

	return true;
}

void OutputWindow::OnSize(unsigned long flag, unsigned short w, unsigned short h)
{
	auto client = GetClientRect();

	auto findRect = client;
	findRect.bottom = findRect.top + findWindowHeight;
	findWindow.Move(findRect);

	auto outputRect = client;
	outputRect.top = findRect.bottom + findWindowPadding;
	output.Move(outputRect);
}

HBRUSH OutputWindow::OnCtlColorStatic(HWND control, HDC dc)
{
	::SetBkColor(dc, backgroundColor);
	return background;
}

void OutputWindow::OnCommand(WORD code, WORD id, HWND hwnd)
{
	switch(id)
	{
	case ID_BUILD_MESSAGE:
		{
			std::lock_guard<std::mutex> lock(messageQueueLock);
			for (const auto& message: messageQueue)
				Append(message);
			messageQueue.clear();
		}
		break;
	}
}

void OutputWindow::Clear()
{
	output.SetText("");
}

void OutputWindow::Append(const std::string& message)
{
	output.AppendText(message);
}

void OutputWindow::ProcessBuildMessage(unsigned long id, const std::string& message)
{
	std::ostringstream out;
	out << id << "> " << STRING::rtrim(message) << "\r\n";
	{
		std::lock_guard<std::mutex> lock(messageQueueLock);
		messageQueue.push_back(out.str());
	}
	Post(WM_COMMAND, MAKEWPARAM(ID_BUILD_MESSAGE, 0));
}

FileLocation OutputWindow::GetSelectedFileLocation()
{
	auto selection = output.GetSel();
	auto selectedLine = output.LineFromChar(selection.first);
	auto selectedText = output.GetLine(selectedLine);
	return { selectedText };
}

FindInDocumentWindow& OutputWindow::GetFindInDocumentWindow()
{
	return findWindow;
}


