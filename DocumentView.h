////////////////////////////////////////////////////////////////////////////////
// Filename:    DocumentView.h
// Description: This file declares the DocumentView class.  This class represents
//              the view port window on top of a document.
//
// Created:     2012-08-12 11:08:18
// Author:      Jacob Buysse
//                                                              Modification Log
// When       Who               Description
// ---------- ----------------- ------------------------------------------------
////////////////////////////////////////////////////////////////////////////////
#pragma once
#include "DocumentEvents.h"
#include "Document.h"
#include "DocumentPosition.h"
#include "FindInDocumentEvents.h"
#include "OutputTarget.h"

class DocumentView :
	public WIN::CWindowImpl<DocumentView>,
	public DocumentEvents,
	public FindInDocumentEvents
{
public:
	DocumentView();
	DocumentView(const DocumentView& rhs) = delete;
	virtual ~DocumentView() = default;

	DocumentView& operator=(const DocumentView& rhs) = delete;

	static void SetupClass(WNDCLASSEX& cls);

	bool OnCreate(CREATESTRUCT* cs) override;
	void OnDestroy() override;
	void OnChar(char c, unsigned long flags) override;
	void OnKeyDown(unsigned long key, unsigned long flags) override;
	void OnPaint() override;
	void OnSize(unsigned long flags, unsigned short w, unsigned short h) override;
	void OnScroll(bool vertical, unsigned short code, unsigned short thumb, HWND from) override;
	void OnMouseWheel(short delta, unsigned short flags, short x, short y) override;
	void OnLButtonDown(unsigned long flags, short x, short y) override;
	void OnLButtonUp(unsigned long flags, short x, short y) override;
	void OnLButtonDblClk(unsigned long flags, short x, short y) override;
	void OnRButtonUp(unsigned long flags, short x, short y) override;
	void OnMouseMove(unsigned long flags, short x, short y) override;
	void OnDropFiles(HDROP drop) override;
	LRESULT OnMouseActivate(HWND parent, unsigned short hit, unsigned short msg) override;
	void OnSetFocus(HWND prev) override;
	void OnKillFocus(HWND next) override;
	void OnCommand(WORD code, WORD id, HWND hwnd) override;

	POINT GetScrollPos();
	void SetScrollPos(POINT pt);
	void UpdateScrollStatus();
	void UpdateScrollBarStatus(int scrollBar, int visible, int total);
	void UpdateCaret();
	SIZE GetClientSize();
	RECT GetViewRect();
	void DrawDocument(HDC dc);
	void EnsureCaretVisible();
	void PageUp(bool extend, bool isVertical);
	void PageDown(bool extend, bool isVertical);
	void ScrollLine(long cy);
	std::string GetClipboardText();
	void CopyToClipboard(const std::string& value);
	DocumentPosition GetDocumentPositionFromPoint(int x, int y);
	std::pair<std::string, bool> GetFileNameAtCursor();
	void SetCursorPosition(unsigned long line, unsigned long column);
	void SetOutputTarget(OutputTarget* outputTarget);
	unsigned long GetDocumentLineCount();
	std::string GetDocumentFileName();

	void SetDocument(Document* document);

	void OnDocumentSizeChanged() override;
	void OnDocumentEditRegion(const DocumentPosition& start, const DocumentPosition& end) override;
	void OnDocumentSelectionChanged() override;

	void FindTextInDocument(const std::string& text) override;

private:
	Document* document = nullptr;
	WIN::CBrush background;
	WIN::CBrush currentLine;
	WIN::CBrush margin;
	WIN::CFont font;
	TEXTMETRIC metrics = {0};
	SIZE charSize = {0};
	SIZE documentSize = {0};
	int marginWidth = 0;
	int marginLineNumberWidth = 0;
	bool selectingText = false;
	OutputTarget* outputTarget = nullptr;
};

