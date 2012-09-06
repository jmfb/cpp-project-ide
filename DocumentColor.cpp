////////////////////////////////////////////////////////////////////////////////
// Filename:    DocumentColor.cpp
// Description: This file implements all DocumentColor member functions.
//
// Created:     2012-08-12 20:00:44
// Author:      Jacob Buysse
//                                                              Modification Log
// When       Who               Description
// ---------- ----------------- ------------------------------------------------
////////////////////////////////////////////////////////////////////////////////
#include "pch.h"
#include "DocumentColor.h"
#include <cctype>
#include <set>

void DocumentColor::DrawLine(
	HDC dc,
	const std::string& text,
	int top,
	int bottom,
	int left,
	int charWidth,
	unsigned long firstVisibleColumn,
	unsigned long lastVisibleColumn,
	unsigned long firstSelectedColumn,
	unsigned long lastSelectedColumn,
	bool isCurrentLine,
	bool isEmptyVerticalSelection)
{
	State state = State::initial;
	bool isIf = false;
	bool isInclude = false;
	bool isPragma = false;

	unsigned long column = 0;
	for (unsigned long index = 0; index < text.size(); ++index)
	{
		auto c = text[index];

		//Transition the state
		TransitionState(
			text,
			index,
			state,
			isIf,
			isInclude,
			isPragma);

		//Get the text colors
		auto color = GetStateTextColor(state);
		auto backColor = isCurrentLine ?
			DocumentColor::currentLineBackground :
			DocumentColor::background;

		//Updates colors if the character is selected.
		if (column >= firstSelectedColumn && column < lastSelectedColumn)
		{
			backColor = DocumentColor::selectionBackground;
			color = DocumentColor::selectionText;
		}
		else if (std::isspace(c))
		{
			//Shade the whitespace character a darker shade (only when not selected)
			color = GetWhitespaceColor(color);
		}

		//Draw the given character (note that column is advanced in each call)
		if (c == '\t')
			DrawTab(dc, column, color, backColor, top, bottom, left, charWidth, firstVisibleColumn);
		else if (c == ' ')
			DrawSpace(dc, column, color, backColor, top, bottom, left, charWidth, firstVisibleColumn);
		else
			DrawCharacter(dc, c, column, color, backColor, top, bottom, left, charWidth, firstVisibleColumn);

		//Stop parsing/drawing if we reach the end of the visible area
		if (column >= lastVisibleColumn)
			break;
	}

	if (isEmptyVerticalSelection && MATH::Between(firstVisibleColumn, lastVisibleColumn, firstSelectedColumn))
	{
		WIN::CBrush brush;
		brush.Create(DocumentColor::selectionBackground);
		RECT rect = {0};
		rect.top = top;
		rect.bottom = bottom;
		rect.left = left + (firstSelectedColumn - firstVisibleColumn) * charWidth;
		rect.right = rect.left + 2;
		::FillRect(dc, &rect, brush);
	}
}

COLORREF DocumentColor::GetWhitespaceColor(COLORREF color)
{
	unsigned long red = GetRValue(color);
	unsigned long green = GetGValue(color);
	unsigned long blue = GetBValue(color);
	red = (red * 4) / 10;
	blue = (blue * 4) / 10;
	green = (green * 4) / 10;
	return RGB(red, green, blue);
}

std::string DocumentColor::ScanWord(const std::string& line, unsigned long index)
{
	unsigned long end = index + 1;
	while (end < line.size() && (line[end] == '_' || std::isalnum(line[end])))
		++end;
	return line.substr(index, end - index);
}

bool DocumentColor::IsInstructionKeyword(const std::string& word)
{
	static const std::set<std::string> keywords = {
		"namespace", "using",
		"class", "struct", "union", "enum",
		"public", "protected", "private",
		"virtual", "override", "final",
		"typedef", "template", "typename",
		"decltype",
		"static", "friend", "inline", "extern",
		"constexpr", "__declspec", "dllexport",
		"if", "else", "for", "do", "while", "switch",
		"case", "break", "continue", "default", "return", "goto",
		"operator", "new", "delete", "typeid", "sizeof",
		"__stdcall",
		"const_cast", "static_cast", "dynamic_cast", "reinterpret_cast",
		"this", "true", "false", "nullptr",
		"try", "catch", "throw",
		"static_assert",
		"noexcept"
	};
	return keywords.find(word) != keywords.end();
}

bool DocumentColor::IsTypeKeyword(const std::string& word)
{
	static const std::set<std::string> keywords = {
		"void", "bool", "short", "long", "int",
		"signed", "unsigned",
		"float", "double",
		"const", "volatile",
		"register", "auto",
		"char", "wchar_t"
	};
	return keywords.find(word) != keywords.end();
}

void DocumentColor::TransitionState(
	const std::string& text,
	unsigned long index,
	State& state,
	bool& isIf,
	bool& isInclude,
	bool& isPragma)
{
	auto c = text[index];
	auto hasNext = (index + 1) < text.size();
	auto next = hasNext ? text[index + 1] : '\0';

	//Determine the state of the current character based on the previous state
	switch(state)
	{
	case State::initial:
		if (c == '#')
			state = State::preprocessor;
		else if (!std::isspace(c))
		{
			state = State::none;
			goto case_state_none;
		}
		break;

	case State::none:
	case_state_none:
		if (c == '"')
			state = State::string;
		else if (c == '\'')
			state = State::character;
		else if (c == '/' && hasNext && next == '/')
			state = State::singleLineComment;
		else if (c == '_' || std::isalpha(c))
		{
			auto word = ScanWord(text, index);
			if (IsInstructionKeyword(word))
				state = State::instructionKeyword;
			else if (IsTypeKeyword(word))
				state = State::typeKeyword;
			else if ((isIf && word == "defined") ||
				(isPragma && word == "once"))
				state = State::preprocessorKeyword;
			else
				state = State::identifier;
		}
		else if (isInclude && c == '<')
			state = State::includeString;
		else if (std::isdigit(c))
			state = State::number;
		else if (std::ispunct(c))
			state = State::punctuation;
		break;

	case State::punctuation:
	case State::stringEnd:
	case State::characterEnd:
		state = State::none;
		goto case_state_none;

	case State::number:
		if (!std::isalnum(c) && c != '.')
		{
			state = State::none;
			goto case_state_none;
		}
		break;

	case State::includeString:
		if (c == '>')
			state = State::includeStringEnd;
		break;

	case State::includeStringEnd:
		state = State::none;
		goto case_state_none;

	case State::string:
		if (c == '\\')
			state = State::stringEscape;
		else if (c == '"')
			state = State::stringEnd;
		break;

	case State::stringEscape:
		state = State::string;
		break;

	case State::character:
		if (c == '\\')
			state = State::characterEscape;
		else if (c == '\'')
			state = State::characterEnd;
		break;

	case State::characterEscape:
		state = State::character;
		break;

	case State::instructionKeyword:
	case State::typeKeyword:
	case State::identifier:
		if (c != '_' && !std::isalnum(c))
		{
			state = State::none;
			goto case_state_none;
		}
		break;

	case State::preprocessor:
		if (!std::isspace(c))
		{
			auto word = ScanWord(text, index);
			if (word == "if")
				isIf = true;
			else if (word == "pragma")
				isPragma = true;
			else if (word == "include")
				isInclude = true;
			state = State::preprocessorKeyword;
		}
		break;

	case State::preprocessorKeyword:
		if (!std::isalpha(c))
		{
			state = State::none;
			goto case_state_none;
		}
		break;

	case State::singleLineComment:
		break;
	}
}

COLORREF DocumentColor::GetStateTextColor(State state)
{
	switch(state)
	{
	case State::string:
	case State::stringEscape:
	case State::stringEnd:
	case State::includeString:
	case State::includeStringEnd:
	case State::character:
	case State::characterEscape:
	case State::characterEnd:
		return DocumentColor::string;
	case State::punctuation:
		return DocumentColor::punctuation;
	case State::number:
		return DocumentColor::number;
	case State::singleLineComment:
		return DocumentColor::comment;
	case State::instructionKeyword:
		return DocumentColor::instructionKeyword;
	case State::typeKeyword:
		return DocumentColor::typeKeyword;
	case State::preprocessor:
	case State::preprocessorKeyword:
		return DocumentColor::preprocessor;
	default:
		return DocumentColor::text;
	}
}

void DocumentColor::DrawTab(
	HDC dc,
	unsigned long& column,
	COLORREF color,
	COLORREF backColor,
	int top,
	int bottom,
	int left,
	int charWidth,
	unsigned long firstVisibleColumn)
{
	auto visibleTabWidth = 4 - (column % 4);
	while (visibleTabWidth > 0 && column < firstVisibleColumn)
	{
		--visibleTabWidth;
		++column;
	}
	if (visibleTabWidth > 0)
	{
		if (backColor != DocumentColor::background)
		{
			RECT rect = {0};
			rect.top = top;
			rect.bottom = bottom;
			rect.left = left + (column - firstVisibleColumn) * charWidth;
			rect.right = rect.left + visibleTabWidth * charWidth;
			WIN::CBrush brush;
			brush.Create(backColor);
			::FillRect(dc, &rect, brush);
		}

		auto x = left + (column - firstVisibleColumn) * charWidth + 1;
		auto y = top + (bottom - top) / 2 - 1;
		WIN::CPen pen;
		pen.Create(PS_SOLID, 1, color);
		auto oldPen = ::SelectObject(dc, pen.Get());
		::MoveToEx(dc, x, y, nullptr);
		::LineTo(dc, x + charWidth - 2, y);
		::LineTo(dc, x + charWidth - 6, y - 4);
		::MoveToEx(dc, x + charWidth - 2, y, nullptr);
		::LineTo(dc, x + charWidth - 6, y + 4);
		::SelectObject(dc, oldPen);
		column += visibleTabWidth;
	}
}

void DocumentColor::DrawSpace(
	HDC dc,
	unsigned long& column,
	COLORREF color,
	COLORREF backColor,
	int top,
	int bottom,
	int left,
	int charWidth,
	unsigned long firstVisibleColumn)
{
	if (column >= firstVisibleColumn)
	{
		if (backColor != DocumentColor::background)
		{
			RECT backRect = {0};
			backRect.top = top;
			backRect.bottom = bottom;
			backRect.left = left + (column - firstVisibleColumn) * charWidth;
			backRect.right = backRect.left + charWidth;
			WIN::CBrush brush;
			brush.Create(backColor);
			::FillRect(dc, &backRect, brush);
		}

		RECT rect = {0};
		rect.top = top + (bottom - top) / 2 - 1;
		rect.bottom = rect.top + 2;
		rect.left = left + ((column - firstVisibleColumn) * charWidth) + charWidth / 2 - 1;
		rect.right = rect.left + 2;
		WIN::CBrush brush;
		brush.Create(color);
		::FillRect(dc, &rect, brush);
	}
	++column;
}

void DocumentColor::DrawCharacter(
	HDC dc,
	char c,
	unsigned long& column,
	COLORREF color,
	COLORREF backColor,
	int top,
	int bottom,
	int left,
	int charWidth,
	unsigned long firstVisibleColumn)
{
	if (column >= firstVisibleColumn)
	{
		RECT rect = {0};
		rect.top = top;
		rect.bottom = bottom;
		rect.left = left + (column - firstVisibleColumn) * charWidth;
		rect.right = rect.left + charWidth;
		::SetTextColor(dc, color);
		::SetBkColor(dc, backColor);
		::DrawText(dc, &c, 1, &rect, DT_LEFT|DT_SINGLELINE|DT_NOPREFIX);
	}
	++column;
}

