////////////////////////////////////////////////////////////////////////////////
// Filename:    DocumentView.cpp
// Description: This file implements all DocumentView member functions.
//
// Created:     2012-08-12 11:10:22
// Author:      Jacob Buysse
//                                                              Modification Log
// When       Who               Description
// ---------- ----------------- ------------------------------------------------
////////////////////////////////////////////////////////////////////////////////
#include "pch.h"
#include "DocumentView.h"
#include "DocumentColor.h"
#include "resource.h"
#include <cstring>

const int bookmarkWidth = 20;

DocumentView::DocumentView()
{
	SetDocument(nullptr);
}

void DocumentView::SetupClass(WNDCLASSEX& cls)
{
	cls.lpszClassName = "DocumentView";
	cls.style = CS_DBLCLKS | CS_HREDRAW | CS_VREDRAW;
	cls.hCursor = ::LoadCursor(nullptr, IDC_IBEAM);
}

bool DocumentView::OnCreate(CREATESTRUCT* cs)
{
	background.Create(DocumentColor::background);
	currentLine.Create(DocumentColor::currentLineBackground);
	margin.Create(DocumentColor::marginBackground);
	bookmarkBrush.Create(DocumentColor::marginBookmark);

	auto dc = ::GetDC(GetHWND());
	font.Create("Courier New", WIN::CFont::CalcHeight(dc, 10));
	::SelectObject(dc, font.Get());
	::GetTextMetrics(dc, &metrics);
	charSize.cx = metrics.tmAveCharWidth;
	charSize.cy = metrics.tmHeight;
	::ReleaseDC(GetHWND(), dc);

	::CreateCaret(GetHWND(), nullptr, 2, 17);
	::SetCaretPos(0, 0);
	::ShowCaret(GetHWND());

	SCROLLINFO info;
	info.cbSize = sizeof(info);
	info.fMask = SIF_ALL;
	info.nMin = 0;
	info.nMax = 0;
	info.nPage = 0;
	info.nPos = 0;
	info.nTrackPos = 0;
	::SetScrollInfo(GetHWND(), SB_VERT, &info, TRUE);
	::SetScrollInfo(GetHWND(), SB_HORZ, &info, TRUE);
	::EnableScrollBar(GetHWND(), SB_VERT, ESB_DISABLE_BOTH);
	::EnableScrollBar(GetHWND(), SB_HORZ, ESB_DISABLE_BOTH);

	return true;
}

void DocumentView::OnDestroy()
{
	::DestroyCaret();
}

void DocumentView::OnChar(char c, unsigned long flags)
{
	if (std::isprint(c))
		document->InsertText(std::string(1, c));
	else
	{
		auto shift = ::GetKeyState(VK_SHIFT) < 0;
		switch(c)
		{
		case VK_BACK:
			document->PerformBackspace();
			break;
		case VK_TAB:
			document->PerformTab(shift);
			break;
		case VK_RETURN:
			document->InsertNewLine();
			break;
		}
	}
}

void DocumentView::OnKeyDown(unsigned long key, unsigned long flags)
{
	auto shift = (::GetKeyState(VK_SHIFT) < 0);
	auto control = (::GetKeyState(VK_CONTROL) < 0);
	auto alt = (::GetKeyState(VK_MENU) < 0);

	switch(key)
	{
	case VK_LEFT:
		if (control)
			document->SelectPreviousWord(shift, alt, true);
		else
			document->SelectPreviousCharacter(shift, alt);
		break;

	case VK_RIGHT:
		if (control)
			document->SelectNextWord(shift, alt, true);
		else
			document->SelectNextCharacter(shift, alt);
		break;

	case VK_UP:
		if (control)
		{
			ScrollLine(-1);
		}
		else
			document->SelectPreviousLine(shift, alt);
		break;

	case VK_DOWN:
		if (control)
		{
			ScrollLine(1);
		}
		else
			document->SelectNextLine(shift, alt);
		break;

	case VK_PRIOR:
		PageUp(shift, alt);
		break;

	case VK_NEXT:
		PageDown(shift, alt);
		break;

	case VK_HOME:
		if (control)
			document->SelectStartOfFile(shift, alt);
		else
			document->SelectStartOfLine(shift, alt);
		break;

	case VK_END:
		if (control)
			document->SelectEndOfFile(shift, alt);
		else
			document->SelectEndOfLine(shift, alt);
		break;
	}
}

void DocumentView::OnPaint()
{
	PAINTSTRUCT ps;
	auto hdc = ::BeginPaint(GetHWND(), &ps);

	RECT clipBox = {0};
	::GetClipBox(hdc, &clipBox);
	auto compatibleDc = ::CreateCompatibleDC(hdc);
	::LPtoDP(hdc, reinterpret_cast<POINT*>(&clipBox), 2);
	auto bitmap = ::CreateCompatibleBitmap(hdc, clipBox.right - clipBox.left, clipBox.bottom - clipBox.top);
	auto oldBitmap = ::SelectObject(compatibleDc, bitmap);
	::DPtoLP(hdc, reinterpret_cast<POINT*>(&clipBox), 2);
	::SetWindowOrgEx(compatibleDc, clipBox.left, clipBox.top, nullptr);

	auto client = GetClientRect();
	auto textRect = client;
	auto marginRect = client;
	marginRect.right = marginRect.left + marginWidth;
	textRect.left = marginRect.right;

	::FillRect(compatibleDc, &marginRect, margin);
	::FillRect(compatibleDc, &textRect, background);

	::SelectObject(compatibleDc, font.Get());

	DrawDocument(compatibleDc);

	::BitBlt(
		hdc,
		clipBox.left,
		clipBox.top,
		clipBox.right - clipBox.left,
		clipBox.bottom - clipBox.top,
		compatibleDc,
		clipBox.left,
		clipBox.top,
		SRCCOPY);
	::SelectObject(compatibleDc, oldBitmap);
	::DeleteObject(bitmap);
	::DeleteObject(compatibleDc);
	::EndPaint(GetHWND(), &ps);
}

void DocumentView::OnSize(unsigned long flags, unsigned short w, unsigned short h)
{
	UpdateScrollStatus();
}

void DocumentView::OnScroll(bool vertical, unsigned short code, unsigned short thumb, HWND from)
{
	int scrollBar = vertical ? SB_VERT : SB_HORZ;

	SCROLLINFO info = {0};
	info.cbSize = sizeof(info);
	info.fMask = SIF_ALL;
	::GetScrollInfo(GetHWND(), scrollBar, &info);

	info.fMask = SIF_POS;
	switch(code)
	{
	case SB_BOTTOM:
		info.nPos = info.nMax - info.nPage + 1;
		break;
	case SB_LINEDOWN:
		info.nPos = MATH::Min(static_cast<int>(info.nMax - info.nPage + 1), info.nPos + 1);
		break;
	case SB_LINEUP:
		info.nPos = MATH::Max(0, info.nPos - 1);
		break;
	case SB_PAGEDOWN:
		info.nPos = MATH::Min(info.nMax - info.nPage + 1, info.nPos + info.nPage);
		break;
	case SB_PAGEUP:
		info.nPos = MATH::Max(0, static_cast<int>(info.nPos - info.nPage));
		break;
	case SB_TOP:
		info.nPos = 0;
		break;
	case SB_THUMBPOSITION:
	case SB_THUMBTRACK:
		info.nPos = info.nTrackPos;
		break;
	case SB_ENDSCROLL:
		return;
	}

	::SetScrollInfo(GetHWND(), scrollBar, &info, TRUE);
	Invalidate();
	UpdateCaret();
}

void DocumentView::OnMouseWheel(short delta, unsigned short flags, short x, short y)
{
	SCROLLINFO info;
	info.cbSize = sizeof(info);
	info.fMask = SIF_ALL;
	::GetScrollInfo(GetHWND(), SB_VERT, &info);

	info.fMask = SIF_POS;
	int offset = info.nPos - static_cast<short>(delta / WHEEL_DELTA);
	info.nPos = MATH::Max(0, MATH::Min(info.nMax - static_cast<int>(info.nPage) + 1, offset));

	::SetScrollInfo(GetHWND(), SB_VERT, &info, TRUE);
	Invalidate();
	UpdateCaret();
}

void DocumentView::OnLButtonDown(unsigned long flags, short x, short y)
{
	::SetCapture(GetHWND());
	auto position = GetDocumentPositionFromPoint(x, y);
	selectingText = true;
	auto shift = MATH::TestFlag(flags, MK_SHIFT);
	auto alt = ::GetKeyState(VK_MENU) < 0;
	document->SelectPosition(position, shift, alt);
	if (MATH::TestFlag(flags, MK_CONTROL))
	{
		document->SelectPreviousWord(false, false, false);
		document->SelectNextWord(true, false, false);
	}
}

void DocumentView::OnLButtonUp(unsigned long flags, short x, short y)
{
	selectingText = false;
	::ReleaseCapture();
}

void DocumentView::OnLButtonDblClk(unsigned long flags, short x, short y)
{
	auto position = GetDocumentPositionFromPoint(x, y);
	document->SelectPosition(position, false, false);
	document->SelectPreviousWord(false, false, false);
	document->SelectNextWord(true, false, false);
}

void DocumentView::OnRButtonUp(unsigned long flags, short x, short y)
{
}

void DocumentView::OnMouseMove(unsigned long flags, short x, short y)
{
	if (selectingText)
	{
		auto position = GetDocumentPositionFromPoint(x, y);
		auto alt = ::GetKeyState(VK_MENU) < 0;
		document->SelectPosition(position, true, alt);
	}
}

void DocumentView::OnDropFiles(HDROP drop)
{
}

LRESULT DocumentView::OnMouseActivate(HWND parent, unsigned short hit, unsigned short msg)
{
	SetFocus();
	return MA_ACTIVATE;
}

void DocumentView::OnSetFocus(HWND prev)
{
	::CreateCaret(GetHWND(), nullptr, 2, 17);
	UpdateCaret();
	::ShowCaret(GetHWND());
}

void DocumentView::OnKillFocus(HWND next)
{
	::DestroyCaret();
}

void DocumentView::OnCommand(WORD code, WORD id, HWND hwnd)
{
	switch(id)
	{
	case ID_SELECT_NEXT_CHARACTER_VERTICAL:
		document->SelectNextCharacter(false, true);
		break;
	case ID_SELECT_NEXT_CHARACTER_VERTICAL_EXTEND:
		document->SelectNextCharacter(true, true);
		break;
	case ID_SELECT_PREVIOUS_CHARACTER_VERTICAL:
		document->SelectPreviousCharacter(false, true);
		break;
	case ID_SELECT_PREVIOUS_CHARACTER_VERTICAL_EXTEND:
		document->SelectPreviousCharacter(true, true);
		break;
	case ID_SELECT_NEXT_WORD_VERTICAL:
		document->SelectNextWord(false, true, true);
		break;
	case ID_SELECT_NEXT_WORD_VERTICAL_EXTEND:
		document->SelectNextWord(true, true, true);
		break;
	case ID_SELECT_PREVIOUS_WORD_VERTICAL:
		document->SelectPreviousWord(false, true, true);
		break;
	case ID_SELECT_PREVIOUS_WORD_VERTICAL_EXTEND:
		document->SelectPreviousWord(true, true, true);
		break;
	case ID_SELECT_NEXT_LINE_VERTICAL:
		document->SelectNextLine(false, true);
		break;
	case ID_SELECT_NEXT_LINE_VERTICAL_EXTEND:
		document->SelectNextLine(true, true);
		break;
	case ID_SELECT_PREVIOUS_LINE_VERTICAL:
		document->SelectPreviousLine(false, true);
		break;
	case ID_SELECT_PREVIOUS_LINE_VERTICAL_EXTEND:
		document->SelectPreviousLine(true, true);
		break;
	case ID_SELECT_PAGEUP_VERTICAL:
		PageUp(false, true);
		break;
	case ID_SELECT_PAGEUP_VERTICAL_EXTEND:
		PageUp(true, true);
		break;
	case ID_SELECT_PAGEDOWN_VERTICAL:
		PageDown(false, true);
		break;
	case ID_SELECT_PAGEDOWN_VERTICAL_EXTEND:
		PageDown(true, true);
		break;
	case ID_SELECT_END_OF_LINE_VERTICAL:
		document->SelectEndOfLine(false, true);
		break;
	case ID_SELECT_END_OF_LINE_VERTICAL_EXTEND:
		document->SelectEndOfLine(true, true);
		break;
	case ID_SELECT_BEGINNING_OF_LINE_VERTICAL:
		document->SelectStartOfLine(false, true);
		break;
	case ID_SELECT_BEGINNING_OF_LINE_VERTICAL_EXTEND:
		document->SelectStartOfLine(true, true);
		break;
	case ID_SELECT_END_OF_FILE_VERTICAL:
		document->SelectEndOfFile(false, true);
		break;
	case ID_SELECT_END_OF_FILE_VERTICAL_EXTEND:
		document->SelectEndOfFile(true, true);
		break;
	case ID_SELECT_BEGINNING_OF_FILE_VERTICAL:
		document->SelectStartOfFile(false, true);
		break;
	case ID_SELECT_BEGINNING_OF_FILE_VERTICAL_EXTEND:
		document->SelectStartOfFile(true, true);
		break;
	case ID_SELECT_ALL:
		document->SelectAll();
		break;
	case ID_EDIT_COPY:
		if (document->HasSelectedText())
			CopyToClipboard(document->GetSelectedText());
		break;
	case ID_EDIT_CUT:
		if (document->HasSelectedText())
		{
			CopyToClipboard(document->GetSelectedText());
			document->PerformDelete();
		}
		break;
	case ID_EDIT_PASTE:
		document->InsertText(GetClipboardText());
		break;
	case ID_EDIT_UNDO:
		document->Undo();
		break;
	case ID_EDIT_REDO:
		document->Redo();
		break;
	case ID_EDIT_TABIFY:
		document->Tabify();
		break;
	case ID_EDIT_INSERT_FILE_HEADER:
		document->InsertFileHeader();
		break;
	case ID_EDIT_INSERT_ONE_TIME_INCLUDE:
		document->InsertOneTimeInclude();
		break;
	case ID_SELECT_SET_FOCUS:
		SetFocus();
		break;
	case ID_EDIT_DELETE:
		document->PerformDelete();
		break;
	case ID_EDIT_TOGGLE_BOOKMARK:
		OnToggleBookmark();
		break;
	case ID_EDIT_NEXT_BOOKMARK:
		OnNextBookmark();
		break;
	case ID_EDIT_PREVIOUS_BOOKMARK:
		OnPreviousBookmark();
		break;
	}
}

POINT DocumentView::GetScrollPos()
{
	POINT pt;
	pt.x = ::GetScrollPos(GetHWND(), SB_HORZ);
	pt.y = ::GetScrollPos(GetHWND(), SB_VERT);
	return pt;
}

void DocumentView::SetScrollPos(POINT pt)
{
	::SetScrollPos(GetHWND(), SB_HORZ, pt.x, TRUE);
	::SetScrollPos(GetHWND(), SB_VERT, pt.y, TRUE);
}

void DocumentView::UpdateScrollStatus()
{
	auto size = GetClientSize();
	UpdateScrollBarStatus(SB_HORZ, size.cx, documentSize.cx);
	size = GetClientSize();
	UpdateScrollBarStatus(SB_VERT, size.cy, documentSize.cy);
	Invalidate();
	UpdateCaret();
}

void DocumentView::UpdateScrollBarStatus(int scrollBar, int visible, int total)
{
	SCROLLINFO info = {0};
	info.cbSize = sizeof(info);
	info.fMask = SIF_ALL;
	if (visible < total)
	{
		::EnableScrollBar(GetHWND(), scrollBar, ESB_ENABLE_BOTH);
		::GetScrollInfo(GetHWND(), scrollBar, &info);
		info.nMin = 0;
		info.nMax = total - 1;
		info.nPage = visible;
		info.nPos = MATH::Min(info.nPos, total - visible);
		info.nTrackPos = 0;
		::SetScrollInfo(GetHWND(), scrollBar, &info, TRUE);
	}
	else
	{
		info.nMin = 0;
		info.nMax = 0;
		info.nPage = 0;
		info.nPos = 0;
		info.nTrackPos = 0;
		::SetScrollInfo(GetHWND(), scrollBar, &info, TRUE);
		::EnableScrollBar(GetHWND(), scrollBar, ESB_DISABLE_BOTH);
	}
}

void DocumentView::UpdateCaret()
{
	auto scrollPosition = GetScrollPos();
	auto selection = document->GetSelection();
	auto column = selection.GetEndColumn() - scrollPosition.x;
	auto line = selection.GetEndLine() - scrollPosition.y;
	::SetCaretPos(marginWidth + column * charSize.cx, line * charSize.cy);
}

SIZE DocumentView::GetClientSize()
{
	auto client = GetClientRect();
	return { (client.right - client.left - marginWidth) / charSize.cx + 1, (client.bottom - client.top) / charSize.cy + 1 };
}

RECT DocumentView::GetViewRect()
{
	auto scrollPosition = GetScrollPos();
	auto size = GetClientSize();
	RECT view = {0};
	view.top = scrollPosition.y;
	view.left = scrollPosition.x;
	view.bottom = scrollPosition.y + size.cy;
	view.right = scrollPosition.x + size.cx;
	return view;
}

void DocumentView::DrawDocument(HDC dc)
{
	auto client = GetClientRect();
	auto view = GetViewRect();
	unsigned long firstVisibleLine = view.top;
	unsigned long lastVisibleLine = MATH::Min(
		static_cast<unsigned long>(view.bottom),
		document->GetLineCount());

	auto selection = document->GetSelection();
	auto isTextSelected =
		selection.GetStartLine() != selection.GetEndLine() ||
		selection.GetStartColumn() != selection.GetEndColumn();
	auto isEmptyVerticalSelection =
		selection.IsVertical() &&
		selection.GetStartLine() != selection.GetEndLine() &&
		selection.GetStartColumn() == selection.GetEndColumn();

	//Normalize the selection range
	auto firstSelectedLine = selection.GetStartLine();
	auto firstSelectedColumn = selection.GetStartColumn();
	auto lastSelectedLine = selection.GetEndLine();
	auto lastSelectedColumn = selection.GetEndColumn();
	if (lastSelectedLine < firstSelectedLine ||
		(lastSelectedLine == firstSelectedLine && lastSelectedColumn < firstSelectedColumn))
	{
		std::swap(firstSelectedLine, lastSelectedLine);
		std::swap(firstSelectedColumn, lastSelectedColumn);
	}
	if (selection.IsVertical() && lastSelectedColumn < firstSelectedColumn)
		std::swap(firstSelectedColumn, lastSelectedColumn);

	int lineTop = client.top;
	for (unsigned long index = firstVisibleLine; index < lastVisibleLine; ++index)
	{
		auto lineRect = client;
		lineRect.top = lineTop;
		lineRect.bottom = lineRect.top + charSize.cy;
		lineTop += charSize.cy;

		std::ostringstream out;
		out << std::setw(marginLineNumberWidth) << std::setfill(' ') << (index + 1);
		::SetBkColor(dc, DocumentColor::marginBackground);
		::SetTextColor(dc, DocumentColor::marginText);
		::DrawText(dc, out.str().c_str(), marginLineNumberWidth, &lineRect, DT_LEFT|DT_SINGLELINE|DT_NOPREFIX);

		if (document->IsLineBookmarked(index))
		{
			const auto bookmarkBorder = 3;
			auto bookmarkRect = lineRect;
			bookmarkRect.left += marginWidth - bookmarkWidth + bookmarkBorder;
			bookmarkRect.right = bookmarkRect.left + bookmarkWidth - 2 * bookmarkBorder;
			bookmarkRect.top += bookmarkBorder;
			bookmarkRect.bottom -= bookmarkBorder;
			::FillRect(dc, &bookmarkRect, bookmarkBrush);
		}

		lineRect.left += marginWidth;
		auto isCurrentLine = selection.GetEndLine() == index;
		if (isCurrentLine)
			::FillRect(dc, &lineRect, currentLine);

		const auto& text = document->GetLine(index);

		unsigned long selectedColumnStart = 0;
		unsigned long selectedColumnEnd = 0;
		if (isTextSelected && MATH::Between(firstSelectedLine, lastSelectedLine, index))
		{
			selectedColumnStart = (index == firstSelectedLine || selection.IsVertical()) ? firstSelectedColumn : 0;
			selectedColumnEnd = (index == lastSelectedLine || selection.IsVertical()) ? lastSelectedColumn : document->GetColumnWidth(index);
		}

		DocumentColor::DrawLine(
			dc,
			text,
			lineRect.top,
			lineRect.bottom,
			lineRect.left,
			charSize.cx,
			view.left,
			view.right,
			selectedColumnStart,
			selectedColumnEnd,
			isCurrentLine,
			isEmptyVerticalSelection && MATH::Between(firstSelectedLine, lastSelectedLine, index));
	}
}

void DocumentView::EnsureCaretVisible()
{
	auto scrollPosition = GetScrollPos();
	auto view = GetViewRect();
	auto caret = document->GetSelection().GetEnd().ToPoint();

	if (!::PtInRect(&view, caret))
	{
		if (caret.x < view.left)
		{
			scrollPosition.x -= (view.left - caret.x);
		}
		else if (caret.x >= view.right)
		{
			scrollPosition.x += (caret.x - view.right + 1);
		}

		if (caret.y < view.top)
		{
			scrollPosition.y -= (view.top - caret.y);
		}
		else if (caret.y >= view.bottom)
		{
			scrollPosition.y += (caret.y - view.bottom + 1);
		}

		SetScrollPos(scrollPosition);
		Invalidate();
	}
}

void DocumentView::PageUp(bool extend, bool isVertical)
{
	auto size = GetClientSize();

	SCROLLINFO info = {0};
	info.cbSize = sizeof(info);
	info.fMask = SIF_ALL;
	::GetScrollInfo(GetHWND(), SB_VERT, &info);

	if (info.nPage != 0)
	{
		info.fMask = SIF_POS;
		info.nPos = MATH::Bound(0, static_cast<int>(info.nMax - info.nPage + 1), static_cast<int>(info.nPos - size.cy + 1));
		::SetScrollInfo(GetHWND(), SB_VERT, &info, TRUE);
		Invalidate();
	}

	document->SelectLine(-size.cy + 1, extend, isVertical);
}

void DocumentView::PageDown(bool extend, bool isVertical)
{
	auto size = GetClientSize();

	SCROLLINFO info = {0};
	info.cbSize = sizeof(info);
	info.fMask = SIF_ALL;
	::GetScrollInfo(GetHWND(), SB_VERT, &info);

	if (info.nPage != 0)
	{
		info.fMask = SIF_POS;
		info.nPos = MATH::Bound(0, static_cast<int>(info.nMax - info.nPage + 1), static_cast<int>(info.nPos + size.cy - 1));
		::SetScrollInfo(GetHWND(), SB_VERT, &info, TRUE);
		Invalidate();
	}

	document->SelectLine(size.cy - 1, extend, isVertical);
}

void DocumentView::ScrollLine(long cy)
{
	SCROLLINFO info = {0};
	info.cbSize = sizeof(info);
	info.fMask = SIF_ALL;
	::GetScrollInfo(GetHWND(), SB_VERT, &info);

	if (info.nPage != 0)
	{
		info.fMask = SIF_POS;
		info.nPos = MATH::Bound(0, static_cast<int>(info.nMax - info.nPage + 1), info.nPos + cy);
		::SetScrollInfo(GetHWND(), SB_VERT, &info, TRUE);
		Invalidate();
		UpdateCaret();
	}
}

void DocumentView::CopyToClipboard(const std::string& value)
{
	auto text = STRING::replace(value, "\n", "\r\n");

	auto size = text.size() + 1;
	auto global = ::GlobalAlloc(GMEM_MOVEABLE, size);
	auto p = ::GlobalLock(global);
	std::memcpy(p, text.c_str(), size);
	::GlobalUnlock(global);

	if (::OpenClipboard(GetHWND()))
	{
		::EmptyClipboard();
		::SetClipboardData(CF_TEXT, global);
		::CloseClipboard();
	}
	else
	{
		::GlobalFree(global);
	}
}

std::string DocumentView::GetClipboardText()
{
	std::string retval;
	if (::OpenClipboard(GetHWND()))
	{
		auto global = ::GetClipboardData(CF_TEXT);
		if (global != nullptr)
		{
			auto str = reinterpret_cast<const char*>(::GlobalLock(global));
			if (str != nullptr)
			{
				retval = str;
				::GlobalUnlock(global);
			}
		}

		::CloseClipboard();
	}

	return retval;
}

DocumentPosition DocumentView::GetDocumentPositionFromPoint(int x, int y)
{
	auto scrollPosition = GetScrollPos();
	auto line = MATH::Max(0, y / charSize.cy + scrollPosition.y);
	auto column = MATH::Max(0, (x - marginWidth) / charSize.cx + scrollPosition.x);
	return document->HitTest(DocumentPosition(line, column));
}

std::pair<std::string, bool> DocumentView::GetFileNameAtCursor()
{
	std::pair<std::string, bool> result("", false);
	auto selection = document->GetSelection();
	if (selection.GetStartLine() == selection.GetEndLine())
	{
		auto line = STRING::trim(document->GetLine(selection.GetStartLine()));
		if (line.find("#include") == 0 && line.size() > 12)
		{
			auto start = line.find_first_of("<\"");
			if (start != std::string::npos)
			{
				auto end = line.rfind(line[start] == '<' ? '>' : '"');
				if (end != std::string::npos && end > start)
				{
					result.first = line.substr(start + 1, end - start - 1);
					result.second = line[start] == '<';
				}
			}
		}
	}
	return result;
}

void DocumentView::SetCursorPosition(unsigned long line, unsigned long column)
{
	//TODO: calculate column from argument which is character index in line
	document->SelectPosition({ line - 1, 0 }, false, false);
}

void DocumentView::SetOutputTarget(OutputTarget* outputTarget)
{
	this->outputTarget = outputTarget;
}

unsigned long DocumentView::GetDocumentLineCount()
{
	return document->GetLineCount();
}

std::string DocumentView::GetDocumentFileName()
{
	return document->GetFileName();
}

void DocumentView::OnToggleBookmark()
{
	document->ToggleBookmark();
}

void DocumentView::OnNextBookmark()
{
	document->NextBookmark();
}

void DocumentView::OnPreviousBookmark()
{
	document->PreviousBookmark();
}

void DocumentView::SetDocument(Document* document)
{
	static Document nullDocument;
	if (this->document)
		this->document->SetEvents(nullptr);
	this->document = document ? document : &nullDocument;
	this->document->SetEvents(this);

	if (IsWindow())
	{
		Invalidate();
		OnDocumentSizeChanged();
		UpdateCaret();
	}
}

void DocumentView::OnDocumentSizeChanged()
{
	auto size = document->GetSize();
	if (size.cx != documentSize.cx || size.cy != documentSize.cy)
	{
		documentSize = size;

		marginWidth = bookmarkWidth;
		marginLineNumberWidth = 0;
		for (auto value = documentSize.cy; value > 0; value /= 10)
			++marginLineNumberWidth;
		marginWidth += marginLineNumberWidth * charSize.cx;

		UpdateScrollStatus();
	}
}

void DocumentView::OnDocumentEditRegion(const DocumentPosition& start, const DocumentPosition& end)
{
	//TODO: Could make this function more efficient by determining the actual region to invalidate.
	//      The double buffering of the paint routine hides most of the slowness.
	//      If typing appears too slow then this function (and perhaps the function raising the event)
	//      could be changed to update just the previously selected line, the currently selected line,
	//      and only the portion of other lines that have had the selection changed.
	Invalidate();
}

void DocumentView::OnDocumentSelectionChanged()
{
	EnsureCaretVisible();
	UpdateCaret();
	//TODO: update status bar position
}

void DocumentView::FindTextInDocument(const std::string& text)
{
	document->FindTextInDocument(text, outputTarget);
}

