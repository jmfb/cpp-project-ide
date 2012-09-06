////////////////////////////////////////////////////////////////////////////////
// Filename:    DocumentColor.h
// Description: This file declares the DocumentColor class contains all color
//              constants.
//
// Created:     2012-08-12 18:35:32
// Author:      Jacob Buysse
//                                                              Modification Log
// When       Who               Description
// ---------- ----------------- ------------------------------------------------
////////////////////////////////////////////////////////////////////////////////
#pragma once

class DocumentColor
{
public:
	enum
	{
		background = RGB(0, 0, 0),
		text = RGB(0, 255, 255),

		marginBackground = RGB(64, 64, 64),
		marginText = RGB(128, 128, 128),
		marginBookmark = RGB(0, 0, 255),

		selectionBackground = RGB(0, 255, 255),
		selectionText = RGB(0, 0, 0),

		currentLineBackground = RGB(64, 64, 64),

		comment = RGB(0, 255, 0),
		string = RGB(192, 192, 192),
		punctuation = RGB(255, 255, 255),
		number = RGB(192, 192, 192),
		instructionKeyword = RGB(255, 0, 0),
		typeKeyword = RGB(255, 128, 128),
		preprocessor = RGB(255, 0, 255)
	};

	static void DrawLine(
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
		bool isEmptyVerticalSelection);

private:
	enum class State
	{
		initial,
		none,
		string,
		stringEscape,
		stringEnd,
		character,
		characterEscape,
		characterEnd,
		singleLineComment,
		instructionKeyword,
		typeKeyword,
		identifier,
		preprocessor,
		preprocessorKeyword,
		includeString,
		includeStringEnd,
		number,
		punctuation
	};

	static COLORREF GetWhitespaceColor(COLORREF color);
	static std::string ScanWord(const std::string& line, unsigned long index);
	static bool IsInstructionKeyword(const std::string& word);
	static bool IsTypeKeyword(const std::string& word);
	static void TransitionState(
		const std::string& text,
		unsigned long index,
		State& state,
		bool& isIf,
		bool& isInclude,
		bool& isPragma);
	static COLORREF GetStateTextColor(State state);
	static void DrawTab(
		HDC dc,
		unsigned long& column,
		COLORREF color,
		COLORREF backColor,
		int top,
		int bottom,
		int left,
		int charWidth,
		unsigned long firstVisibleColumn);
	static void DrawSpace(
		HDC dc,
		unsigned long& column,
		COLORREF color,
		COLORREF backColor,
		int top,
		int bottom,
		int left,
		int charWidth,
		unsigned long firstVisibleColumn);
	static void DrawCharacter(
		HDC dc,
		char c,
		unsigned long& column,
		COLORREF color,
		COLORREF backColor,
		int top,
		int bottom,
		int left,
		int charWidth,
		unsigned long firstVisibleColumn);
};

