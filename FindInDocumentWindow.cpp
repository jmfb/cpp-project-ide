////////////////////////////////////////////////////////////////////////////////
// Filename:    FindInDocumentWindow.cpp
// Description: This file implements all FindInDocumentWindow member functions.
//
// Created:     2012-09-03 20:15:32
// Author:      Jacob Buysse
////////////////////////////////////////////////////////////////////////////////
#include "FindInDocumentWindow.h"

constexpr auto editLabelTextId = 1001;

void FindInDocumentWindow::SetupClass(WNDCLASSEX& cls)
{
	cls.lpszClassName = "FindInDocumentWindow";
}

bool FindInDocumentWindow::OnCreate(CREATESTRUCT* cs)
{
	labelFindText.Attach(WIN::CWindow::Create(WC_STATIC, GetHWND(), nullptr, "Find: ", WS_CHILD | WS_VISIBLE | SS_CENTERIMAGE, 0, 0, 0, 40, 12, nullptr));
	editFindText.Create(GetHWND(), editLabelTextId, "", WS_CHILD | WS_VISIBLE | ES_LEFT | ES_AUTOHSCROLL, WS_EX_CLIENTEDGE, { 40, 0, 100, 12 });

	auto dc = ::GetDC(GetHWND());
	font.Create("Courier New", WIN::CFont::CalcHeight(dc, 8));
	::ReleaseDC(GetHWND(), dc);

	labelFindText.SetFont(font.Get());
	editFindText.SetFont(font.Get());

	return true;
}

void FindInDocumentWindow::OnSize(unsigned long flag, unsigned short w, unsigned short h)
{
	auto client = GetClientRect();
	auto labelRect = client;
	auto editRect = client;
	labelRect.right = 40;
	labelFindText.Move(labelRect);
	editRect.left = 40;
	editFindText.Move(editRect);
}

void FindInDocumentWindow::OnCommand(WORD code, WORD id, HWND hwnd)
{
	switch(id)
	{
	case editLabelTextId:
		if (code == EN_CHANGE && events != nullptr)
			events->FindTextInDocument(editFindText.GetText());
		break;
	}
}

void FindInDocumentWindow::SetEvents(FindInDocumentEvents* events)
{
	this->events = events;
}

void FindInDocumentWindow::OnEditFind()
{
	editFindText.SetFocus();
}

