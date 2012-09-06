////////////////////////////////////////////////////////////////////////////////
// Filename:    Document.cpp
// Description: This file implements all Document member functions.
//
// Created:     2012-08-12 10:18:29
// Author:      Jacob Buysse
//                                                              Modification Log
// When       Who               Description
// ---------- ----------------- ------------------------------------------------
////////////////////////////////////////////////////////////////////////////////
#include "pch.h"
#include "Document.h"
#include "DocumentUtility.h"
#include <fstream>
#include <iostream>
#include <cassert>

void Document::Open(const std::string& fileName, const std::string& relativeFileName)
{
	while (!undoBuffer.empty())
		undoBuffer.pop();
	while (!redoBuffer.empty())
		redoBuffer.pop();
	selection.Clear();
	lines.clear();

	this->fileName = fileName;
	this->relativeFileName = relativeFileName;
	std::ifstream in(fileName.c_str());
	std::string line;
	while (std::getline(in, line))
		lines.push_back(STRING::rtrim(line));

	//Make sure empty files display at least one line
	if (lines.empty())
		lines.push_back("");
}

const std::string& Document::GetFileName() const
{
	return fileName;
}

void Document::SetFileName(const std::string& value)
{
	fileName = value;
}

bool Document::IsDirty() const
{
	return !undoBuffer.empty();
}

void Document::Save()
{
	std::ofstream out(fileName.c_str());
	for (const auto& line: lines)
		out << line << std::endl;
	while (!undoBuffer.empty())
		undoBuffer.pop();
}

void Document::SetEvents(DocumentEvents* events)
{
	this->events = events;
}

SIZE Document::GetSize() const
{
	return { static_cast<long>(GetMaxWidth()), static_cast<long>(GetLineCount()) };
}

unsigned long Document::GetMaxWidth() const
{
	unsigned long maxWidth = 0;
	for (const auto& line: lines)
		maxWidth = MATH::Max(maxWidth, line.size());
	return maxWidth;
}

unsigned long Document::GetLineCount() const
{
	return lines.size();
}

const std::string& Document::GetLine(unsigned long index) const
{
	return lines[index];
}

unsigned long Document::GetColumnWidth(unsigned long index) const
{
	return CalculateColumnWidth(lines[index]);
}

DocumentPosition Document::HitTest(const DocumentPosition& position) const
{
	auto line = MATH::Min(lines.size() - 1, position.GetLine());
	auto index = CalculateIndexFromColumn(lines[line], position.GetColumn());
	auto column = CalculateColumnWidth(lines[line].substr(0, index));
	return DocumentPosition(line, column);
}

bool Document::HasSelectedText() const
{
	return !selection.IsEmpty();
}

const DocumentSelection& Document::GetSelection() const
{
	return selection;
}

std::string Document::GetSelectedText() const
{
	auto start = selection.GetStart();
	auto end = selection.GetEnd();
	if (end < start)
		std::swap(start, end);

	//Single line selection
	if (start.GetLine() == end.GetLine())
	{
		//Nothing selected, return a blank string
		if (start.GetColumn() == end.GetColumn())
			return "";

		const auto& line = lines[start.GetLine()];
		auto firstIndex = CalculateIndexFromColumn(line, start.GetColumn());
		auto lastIndex = CalculateIndexFromColumn(line, end.GetColumn());
		return line.substr(firstIndex, lastIndex - firstIndex);
	}
	//Vertical text selection
	else if (selection.IsVertical())
	{
		//Empty vertical text selection, return n empty strings separated by newlines (thus n - 1 newlines)
		if (start.GetColumn() == end.GetColumn())
			return std::string(end.GetLine() - start.GetLine(), '\n');

		//Order the leftmost and rightmost column positions
		auto firstColumn = start.GetColumn();
		auto lastColumn = end.GetColumn();
		if (lastColumn < firstColumn)
			std::swap(firstColumn, lastColumn);

		//Take a newline separated string of each vertical slice of each line from top to bottom
		std::ostringstream out;
		for (auto line = start.GetLine(); line <= end.GetLine(); ++line)
		{
			if (line > start.GetLine())
				out << std::endl;
			const auto& text = lines[line];
			auto firstIndex = CalculateIndexFromColumn(text, firstColumn);
			auto lastIndex = CalculateIndexFromColumn(text, lastColumn);
			out << text.substr(firstIndex, lastIndex - firstIndex);
		}
		return out.str();
	}
	//Normal multi-line text selection
	else
	{
		std::ostringstream out;
		for (auto line = start.GetLine(); line <= end.GetLine(); ++line)
		{
			const auto& text = lines[line];
			if (line == start.GetLine())
				out << text.substr(CalculateIndexFromColumn(text, start.GetColumn()));
			else if (line != end.GetLine())
				out << std::endl << text;
			else
				out << std::endl << text.substr(0, CalculateIndexFromColumn(text, end.GetColumn()));
		}
		return out.str();
	}
}

void Document::AtomicDeleteSelection()
{
	auto start = selection.GetStart();
	auto end = selection.GetEnd();
	if (end < start)
		std::swap(start, end);

	//Single line selection
	if (start.GetLine() == end.GetLine())
	{
		//Nothing selected, nothing to delete
		if (start.GetColumn() == end.GetColumn())
			return;

		auto& line = lines[start.GetLine()];
		auto firstIndex = CalculateIndexFromColumn(line, start.GetColumn());
		auto lastIndex = CalculateIndexFromColumn(line, end.GetColumn());
		line.erase(firstIndex, lastIndex - firstIndex);
		selection.SetStart(start);
		selection.SetEnd(start);
	}
	//Vertical text selection
	else if (selection.IsVertical())
	{
		//Empty vertical text selection, nothing to delete
		if (start.GetColumn() == end.GetColumn())
			return;

		//Order the leftmost and rightmost column positions
		auto firstColumn = start.GetColumn();
		auto lastColumn = end.GetColumn();
		if (lastColumn < firstColumn)
			std::swap(firstColumn, lastColumn);

		//Remove vertical slice from each line from top to bottom
		for (auto line = start.GetLine(); line <= end.GetLine(); ++line)
		{
			auto& text = lines[line];
			auto firstIndex = CalculateIndexFromColumn(text, firstColumn);
			auto lastIndex = CalculateIndexFromColumn(text, lastColumn);
			text.erase(firstIndex, lastIndex - firstIndex);
		}
		selection.SetStartColumn(firstColumn);
		selection.SetEndColumn(firstColumn);
	}
	//Normal multi-line text selection
	else
	{
		auto& firstLine = lines[start.GetLine()];
		auto firstLineIndex = CalculateIndexFromColumn(firstLine, start.GetColumn());
		auto firstLinePrefix = firstLine.substr(0, firstLineIndex);

		auto& lastLine = lines[end.GetLine()];
		lastLine.erase(0, CalculateIndexFromColumn(lastLine, end.GetColumn()));
		lastLine.insert(lastLine.begin(), firstLinePrefix.begin(), firstLinePrefix.end());

		auto linesToDelete = end.GetLine() - start.GetLine();
		if (linesToDelete > 0)
			lines.erase(lines.begin() + start.GetLine(), lines.begin() + start.GetLine() + linesToDelete);

		selection.SetStart(start);
		selection.SetEnd(start);
	}
}

void Document::AtomicInsertText(const std::string& value)
{
	//This operation is not valid at any point if there is text currently selected
	assert(selection.IsEmpty());

	//Nothing to perform is value is empty
	if (value.empty())
		return;

	//Split the inserted text up into multiple lines
	std::vector<std::string> insertedLines;
	std::istringstream in(value);
	std::string insertedLine;
	while (std::getline(in, insertedLine))
	{
		if (!insertedLine.empty() && insertedLine.back() == '\r')
			insertedLine.pop_back();
		insertedLines.push_back(insertedLine);
	}
	if (value.back() == '\n')
		insertedLines.push_back("");

	//Vertical text selection insert
	if (selection.GetStartLine() != selection.GetEndLine() && selection.IsVertical())
	{
		auto start = selection.GetStart();
		auto end = selection.GetEnd();
		if (end < start)
			std::swap(start, end);

		//Insert this string on to each line in the vertical text selection where the selection is valid
		if (insertedLines.size() == 1)
		{
			auto maxWidth = 0ul;
			for (auto line = start.GetLine(); line <= end.GetLine(); ++line)
			{
				auto& text = lines[line];
				auto columnWidth = CalculateColumnWidth(text);
				if (start.GetColumn() > columnWidth)
					continue;
				auto index = CalculateIndexFromColumn(text, start.GetColumn());
				text.insert(text.begin() + index, insertedLines[0].begin(), insertedLines[0].end());
				maxWidth = MATH::Max(maxWidth, CalculateColumnWidth(text.substr(0, index + insertedLines[0].size())));
			}
			selection.SetStartColumn(maxWidth);
			selection.SetEndColumn(maxWidth);
		}
		//Insert each respective line onto each respective vertically selected line
		else if (insertedLines.size() == (end.GetLine() - start.GetLine() + 1))
		{
			auto maxWidth = 0ul;
			auto line = start.GetLine();
			for (const auto& insertedLine: insertedLines)
			{
				auto& text = lines[line++];
				auto columnWidth = CalculateColumnWidth(text);
				if (start.GetColumn() > columnWidth)
					continue;
				auto index = CalculateIndexFromColumn(text, start.GetColumn());
				text.insert(text.begin() + index, insertedLine.begin(), insertedLine.end());
				maxWidth = MATH::Max(maxWidth, CalculateColumnWidth(text.substr(0, index + insertedLine.size())));
			}
			selection.SetStartColumn(maxWidth);
			selection.SetEndColumn(maxWidth);
		}
		else
		{
			//error - incorrect number of lines being inserted into a vertical text selection.
			assert(false);
		}
	}
	//Normal insertion of text at a given point
	else
	{
		if (insertedLines.size() == 1)
		{
			auto& text = lines[selection.GetStartLine()];
			auto index = CalculateIndexFromColumn(text, selection.GetStartColumn());
			text.insert(index, insertedLines[0]);
			auto column = CalculateColumnWidth(text.substr(0, index + insertedLines[0].size()));
			selection.SetStartColumn(column);
			selection.SetEndColumn(column);
		}
		else if (insertedLines.size() > 1)
		{
			auto& firstLine = lines[selection.GetStartLine()];
			auto index = CalculateIndexFromColumn(firstLine, selection.GetStartColumn());
			auto suffix = firstLine.substr(index);
			firstLine.erase(firstLine.begin() + index, firstLine.end());
			firstLine.insert(firstLine.end(), insertedLines[0].begin(), insertedLines[0].end());

			auto& lastLine = insertedLines.back();
			auto column = CalculateColumnWidth(lastLine);
			lastLine.insert(lastLine.end(), suffix.begin(), suffix.end());

			lines.insert(lines.begin() + selection.GetStartLine() + 1, insertedLines.begin() + 1, insertedLines.end());

			DocumentPosition position(selection.GetStartLine() + insertedLines.size() - 1, column);
			selection.SetStart(position);
			selection.SetEnd(position);
		}
	}
}

bool Document::CanUndo() const
{
	return !undoBuffer.empty();
}

void Document::Undo()
{
	if (!undoBuffer.empty())
	{
		auto action = undoBuffer.top();
		action.Undo(this);
		redoBuffer.push(action);
		undoBuffer.pop();
		RaiseEvents();
	}
}

bool Document::CanRedo() const
{
	return !redoBuffer.empty();
}

void Document::Redo()
{
	if (!redoBuffer.empty())
	{
		auto action = redoBuffer.top();
		action.Redo(this);
		undoBuffer.push(action);
		redoBuffer.pop();
		RaiseEvents();
	}
}

void Document::InsertText(const std::string& value)
{
	if (value.empty())
		return;

	DocumentAction action;
	action.SetOriginalSelection(selection);

	auto textInserted = value;
	auto emptyNonVerticalSelectionAfterFirstLine =
		selection.IsEmpty() &&
		selection.GetStartLine() == selection.GetEndLine() &&
		!selection.IsVertical() &&
		selection.GetEndLine() > 0;

	if (value == "}" &&
		emptyNonVerticalSelectionAfterFirstLine &&
		STRING::trim(lines[selection.GetEndLine()]).empty())
	{
		//Move select from current position to the correct indentation as determined by the correct
		//indentation of the previous line ending in { without a matching line beginning with }.
		auto openBraceCount = 0l;
		auto indent = 0ul;
		for (auto line = selection.GetEndLine(); line > 0; )
		{
			--line;
			const auto& text = lines[line];
			auto lastPosition = text.find_last_not_of("\t ");
			auto firstPosition = text.find_first_not_of("\t ");
			if (lastPosition != std::string::npos && text[lastPosition] == '{')
				++openBraceCount;
			if (openBraceCount > 0)
			{
				indent = CalculateColumnWidth(text.substr(0, firstPosition));
				break;
			}
			if (firstPosition != std::string::npos && text[firstPosition] == '}')
				--openBraceCount;
		}
		if (openBraceCount > 0 && selection.GetEndColumn() != indent)
		{
			textInserted = CreateIndent(indent) + value;
			selection.SetStartColumn(0);
			selection.SetVertical(false);
		}
	}
	//Move case labels (including default) to the indentation of the previous switch statement.
	if (value == ":" &&
		emptyNonVerticalSelectionAfterFirstLine &&
		selection.GetEndColumn() == CalculateColumnWidth(lines[selection.GetEndLine()]))
	{
		auto firstWord = FindFirstWordOfLine(lines[selection.GetEndLine()]);
		std::string matchStatement;
		if (firstWord == "case" || firstWord == "default")
			matchStatement = "switch";
		else if (firstWord == "public" || firstWord == "protected" || firstWord == "private")
			matchStatement = "class";
		if (!matchStatement.empty())
		{
			auto foundStatement = false;
			auto statementIndent = 0ul;
			for (auto line = selection.GetEndLine(); line > 0; )
			{
				--line;
				if (FindFirstWordOfLine(lines[line]) == matchStatement)
				{
					foundStatement = true;
					statementIndent = CalculateIndentOfLine(lines[line]);
					break;
				}
			}
			if (foundStatement)
			{
				textInserted = CreateIndent(statementIndent) + STRING::trim(lines[selection.GetEndLine()]) + value;
				selection.SetStartColumn(0);
				selection.SetVertical(false);
			}
		}
	}

	action.SetSelectionBeforeDelete(selection);
	action.SetTextDeleted(GetSelectedText());
	AtomicDeleteSelection();
	action.SetSelectionBeforeInsert(selection);
	action.SetTextInserted(textInserted);
	AtomicInsertText(textInserted);
	action.SetSelectionAfterInsert(selection);
	action.SetFinalSelection(selection);
	RecordAction(action);
}

void Document::PerformTab(bool shift)
{
	//Decrease indentation
	if (shift)
	{
		if (!selection.IsEmpty())
		{
			//If we have a selection on a single line or a vertical selection area, then shift+tab does nothing.
			if (selection.GetStartLine() == selection.GetEndLine() || selection.IsVertical())
				return;

			//We have multiple lines selected, so set selection to encompass entire set of lines
			//and decrease indent on all lines by 1 unit.
			DocumentAction action;
			action.SetOriginalSelection(selection);

			//Calculate the selection that encompasses all of the selected lines.
			auto inverted = selection.GetEnd() < selection.GetStart();
			DocumentSelection insertionSelection;
			insertionSelection.SetStart(inverted ? selection.GetEnd() : selection.GetStart());
			insertionSelection.SetEnd(inverted ? selection.GetStart() : selection.GetEnd());
			insertionSelection.SetStartColumn(0);
			if (insertionSelection.GetEndColumn() != 0)
			{
				if (insertionSelection.GetEndLine() < (lines.size() - 1))
				{
					insertionSelection.SetEndLine(insertionSelection.GetEndLine() + 1);
					insertionSelection.SetEndColumn(0);
				}
				else
				{
					insertionSelection.SetEndColumn(CalculateColumnWidth(lines.back()));
				}
			}
			insertionSelection.SetVertical(false);

			//Select that range, store the text, and delete the lines (shift+tab cannot be
			//achieved by vertical text selection like tab can).
			selection = insertionSelection;
			action.SetSelectionBeforeDelete(selection);
			auto textDeleted = GetSelectedText();
			action.SetTextDeleted(textDeleted);
			AtomicDeleteSelection();
			action.SetSelectionBeforeInsert(selection);

			//Decrease indent on each line.  Note: this will have the side effect of inserting
			//a newline at the end of the file if the selection reaches the end and the file
			//does not already end in a newline.  Since this is a desirable yet confusing side
			//effect, it will remain.
			std::istringstream in(textDeleted);
			std::ostringstream out;
			std::string text;
			while (std::getline(in, text))
			{
				auto indentIndex = text.find_first_not_of("\t ");
				if (indentIndex == std::string::npos)
					indentIndex = text.size();
				if (indentIndex > 0)
				{
					auto indent = CalculateColumnWidth(text.substr(0, indentIndex));
					indent = (indent < 4) ? 0 : indent - 4;
					text = CreateIndent(indent) + text.substr(indentIndex);
				}
				out << text << std::endl;
			}

			//Insert the new text with decreased indent.
			auto textInserted = out.str();
			action.SetTextInserted(textInserted);
			AtomicInsertText(textInserted);
			action.SetSelectionAfterInsert(selection);

			//Restore the order of the selection.
			selection.SetStart(inverted ? insertionSelection.GetEnd() : insertionSelection.GetStart());
			selection.SetEnd(inverted ? insertionSelection.GetStart() : insertionSelection.GetEnd());
			action.SetFinalSelection(selection);

			//Undo the action if it yielded no change in the text
			if (textInserted == textDeleted)
				action.Undo(this);
			else
				RecordAction(action);
		}
		else
		{
			//Vertical text selection (even when empty) has no side effect from shift+tab.
			if (selection.GetStartLine() != selection.GetEndLine())
				return;

			//Decrease indent of single line (only when at beginning of line in the
			//whitespace region or at the end of the line with trailing whitespace).
			DocumentAction action;
			action.SetOriginalSelection(selection);

			auto text = lines[selection.GetEndLine()];
			auto index = CalculateIndexFromColumn(text, selection.GetEndColumn());
			auto columnWidth = CalculateColumnWidth(text);
			auto column = selection.GetEndColumn();

			//Delete the contents of the line
			selection.SetStartColumn(0);
			selection.SetEndColumn(columnWidth);
			selection.SetVertical(false);
			action.SetSelectionBeforeDelete(selection);
			action.SetTextDeleted(text);
			AtomicDeleteSelection();
			action.SetSelectionBeforeInsert(selection);

			//Decrease indent of the selected line
			auto startOfLine = text.find_first_not_of("\t ");
			auto endOfLine = text.find_last_not_of("\t ");
			auto cancelAction = false;
			//The entire line is already whitespace (put selection at end and decrease indent)
			if (startOfLine == std::string::npos)
			{
				column = columnWidth < 4 ? 0 : columnWidth - 4;
				text = CreateIndent(column);
			}
			//The selection is at the beginning of the line (put selection at beginning of text and decrease indent)
			else if (index <= startOfLine)
			{
				columnWidth = CalculateColumnWidth(text.substr(0, startOfLine));
				column = columnWidth < 4 ? 0 : columnWidth - 4;
				text = CreateIndent(column) + text.substr(startOfLine);
			}
			//The selection is at the end of the line and there is trailing whitespace.
			//Put the selection at the very end of the line and decrease trailing indent.
			else if (endOfLine < (text.size() - 1))
			{
				auto prefixWidth = CalculateColumnWidth(text.substr(0, endOfLine + 1));
				auto suffixWidth = columnWidth - prefixWidth + prefixWidth % 4;
				suffixWidth = suffixWidth < 4 ? 0 : suffixWidth - 4;
				text = text.substr(0, endOfLine + 1) + CreateIndent(suffixWidth);
				column = CalculateColumnWidth(text);
			}
			//Implied else is to leave the line and selection as is (note this will still end up in undo buffer)
			else
			{
				cancelAction = true;
			}

			//Insert the modified line text
			action.SetTextInserted(text);
			AtomicInsertText(text);
			action.SetSelectionAfterInsert(selection);

			//Place the cursor at the appropriate spot for the new line
			selection.SetStartColumn(column);
			selection.SetEndColumn(column);
			action.SetFinalSelection(selection);

			//Undo the action if it yielded no change
			if (cancelAction)
				action.Undo(this);
			else
				RecordAction(action);
		}
	}
	//Increase indentation
	else
	{
		if (selection.IsEmpty() || selection.GetStartLine() == selection.GetEndLine())
		{
			InsertText("\t");
		}
		else
		{
			DocumentAction action;
			action.SetOriginalSelection(selection);

			auto inverted = selection.GetEnd() < selection.GetStart();
			DocumentSelection insertionSelection;
			insertionSelection.SetStart(inverted ? selection.GetEnd() : selection.GetStart());
			insertionSelection.SetEnd(inverted ? selection.GetStart() : selection.GetEnd());
			insertionSelection.SetStartColumn(0);
			if (insertionSelection.GetEndColumn() == 0)
				insertionSelection.SetEndLine(insertionSelection.GetEndLine() - 1);
			else
				insertionSelection.SetEndColumn(0);
			insertionSelection.SetVertical(true);

			selection = insertionSelection;
			action.SetSelectionBeforeDelete(selection);
			action.SetTextDeleted(GetSelectedText());
			AtomicDeleteSelection();

			action.SetSelectionBeforeInsert(selection);
			auto textInserted = "\t";
			action.SetTextInserted(textInserted);
			AtomicInsertText(textInserted);
			action.SetSelectionAfterInsert(selection);

			if (insertionSelection.GetEndLine() < (lines.size() - 1))
				insertionSelection.SetEndLine(insertionSelection.GetEndLine() + 1);
			else
				insertionSelection.SetEndColumn(CalculateColumnWidth(lines.back()));

			selection.SetVertical(false);
			selection.SetStart(inverted ? insertionSelection.GetEnd() : insertionSelection.GetStart());
			selection.SetEnd(inverted ? insertionSelection.GetStart() : insertionSelection.GetEnd());
			action.SetFinalSelection(selection);
			RecordAction(action);
		}
	}
}

void Document::PerformDelete()
{
	//Delete the selected text
	if (HasSelectedText())
	{
		DocumentAction action;
		action.SetOriginalSelection(selection);
		action.SetSelectionBeforeDelete(selection);
		action.SetTextDeleted(GetSelectedText());
		AtomicDeleteSelection();
		action.SetSelectionBeforeInsert(selection);
		action.SetTextInserted("");
		action.SetSelectionAfterInsert(selection);
		action.SetFinalSelection(selection);
		RecordAction(action);
	}
	//Delete the next character from a vertical text selection
	else if (selection.GetStartLine() != selection.GetEndLine() && selection.IsVertical())
	{
		//Ignore delete key presses with a vertical text selection with nothing to the right
		auto maxWidth = 0ul;
		auto firstLine = selection.GetStartLine();
		auto lastLine = selection.GetEndLine();
		if (lastLine < firstLine)
			std::swap(firstLine, lastLine);
		for (auto line = firstLine; line <= lastLine; ++line)
			maxWidth = CalculateColumnWidth(lines[line]);
		if (selection.GetStartColumn() < maxWidth)
		{
			DocumentAction action;
			action.SetOriginalSelection(selection);

			//Select the next column of characters (let each line take care of itself)
			selection.SetEndColumn(selection.GetStartColumn() + 1);

			action.SetSelectionBeforeDelete(selection);
			action.SetTextDeleted(GetSelectedText());
			AtomicDeleteSelection();
			action.SetSelectionBeforeInsert(selection);
			action.SetTextInserted("");
			action.SetSelectionAfterInsert(selection);
			action.SetFinalSelection(selection);
			RecordAction(action);
		}
	}
	//Delete the next character from a normal cursor position
	else
	{
		//Ignore delete key presses at the end of the file.
		auto lastLine = lines.size() - 1;
		auto lastLineWidth = CalculateColumnWidth(lines.back());
		if (selection.GetStartLine() < lastLine || selection.GetStartColumn() < lastLineWidth)
		{
			DocumentAction action;
			action.SetOriginalSelection(selection);

			//Select the next character
			const auto& text = lines[selection.GetStartLine()];
			auto width = CalculateColumnWidth(text);
			auto index = CalculateIndexFromColumn(text, selection.GetStartColumn());
			if (width == selection.GetStartColumn())
			{
				selection.SetEndLine(selection.GetStartLine() + 1);
				selection.SetEndColumn(0);
				selection.SetVertical(false);
			}
			else
			{
				auto column = CalculateNextColumn(text[index + 1], selection.GetStartColumn());
				selection.SetEndColumn(column);
				selection.SetVertical(false);
			}

			action.SetSelectionBeforeDelete(selection);
			action.SetTextDeleted(GetSelectedText());
			AtomicDeleteSelection();
			action.SetSelectionBeforeInsert(selection);
			action.SetTextInserted("");
			action.SetSelectionAfterInsert(selection);
			action.SetFinalSelection(selection);
			RecordAction(action);
		}
	}
}

void Document::PerformBackspace()
{
	//Delete the selected text
	if (HasSelectedText())
	{
		DocumentAction action;
		action.SetOriginalSelection(selection);
		action.SetSelectionBeforeDelete(selection);
		action.SetTextDeleted(GetSelectedText());
		AtomicDeleteSelection();
		action.SetSelectionBeforeInsert(selection);
		action.SetTextInserted("");
		action.SetSelectionAfterInsert(selection);
		action.SetFinalSelection(selection);
		RecordAction(action);
	}
	//Delete the previous character from a vertical text selection
	else if (selection.GetStartLine() != selection.GetEndLine() && selection.IsVertical())
	{
		//Ignore backspace key presses with a vertical text selection with nothing to the left
		if (selection.GetStartColumn() > 0)
		{
			DocumentAction action;
			action.SetOriginalSelection(selection);

			//Select the next column of characters (let each line take care of itself)
			selection.SetEndColumn(selection.GetStartColumn() - 1);

			action.SetSelectionBeforeDelete(selection);
			action.SetTextDeleted(GetSelectedText());
			AtomicDeleteSelection();
			action.SetSelectionBeforeInsert(selection);
			action.SetTextInserted("");
			action.SetSelectionAfterInsert(selection);
			action.SetFinalSelection(selection);
			RecordAction(action);
		}
	}
	//Delete the previous character from a normal cursor position
	else
	{
		//Ignore backspace key presses at the beginning of the file.
		if (selection.GetStartLine() > 0 || selection.GetStartColumn() > 0)
		{
			DocumentAction action;
			action.SetOriginalSelection(selection);

			//Select the previous character
			if (selection.GetStartColumn() == 0)
			{
				selection.SetEndLine(selection.GetStartLine() - 1);
				selection.SetEndColumn(CalculateColumnWidth(lines[selection.GetEndLine()]));
				selection.SetVertical(false);
			}
			else
			{
				auto index = CalculateIndexFromColumn(lines[selection.GetStartLine()], selection.GetStartColumn());
				auto column = CalculateColumnWidth(lines[selection.GetStartLine()].substr(0, index - 1));
				selection.SetEndColumn(column);
				selection.SetVertical(false);
			}

			action.SetSelectionBeforeDelete(selection);
			action.SetTextDeleted(GetSelectedText());
			AtomicDeleteSelection();
			action.SetSelectionBeforeInsert(selection);
			action.SetTextInserted("");
			action.SetSelectionAfterInsert(selection);
			action.SetFinalSelection(selection);
			RecordAction(action);
		}
	}
}

void Document::InsertNewLine()
{
	DocumentAction action;
	action.SetOriginalSelection(selection);

	//Disable vertical text selection when pressing return
	if (selection.IsVertical())
	{
		selection.SetStart(selection.GetEnd());
		selection.SetVertical(false);
	}

	//Extend selection to be deleted to the last non-whitespace character before the current position on the current line.
	if (selection.GetEnd() < selection.GetStart())
	{
		auto oldEnd = selection.GetEnd();
		selection.SetEnd(selection.GetStart());
		selection.SetStart(oldEnd);
	}
	const auto& firstLine = lines[selection.GetStartLine()];
	auto index = CalculateIndexFromColumn(firstLine, selection.GetStartColumn());
	while (index > 0 && std::isspace(firstLine[index - 1]))
		--index;
	selection.SetStartColumn(CalculateColumnWidth(firstLine.substr(0, index)));

	action.SetSelectionBeforeDelete(selection);
	action.SetTextDeleted(GetSelectedText());
	AtomicDeleteSelection();

	action.SetSelectionBeforeInsert(selection);

	//Caclulate the indention of the new line based on indentation the of previous non-blank line
	auto indentation = 0ul;
	auto firstLineToSearchBackFrom = selection.GetEndLine() + 1;
	if (selection.GetEndColumn() == 0)
		--firstLineToSearchBackFrom;
	for (auto line = firstLineToSearchBackFrom; line > 0; )
	{
		--line;
		const auto& text = lines[line];
		if (STRING::trim(text).empty())
			continue;
		indentation = CalculateColumnWidth(text.substr(0, text.find_first_not_of("\t ")));
		auto firstWord = FindFirstWordOfLine(text);
		if (text[text.find_last_not_of("\t ")] == '{' ||
			firstWord == "case" ||
			firstWord == "default" ||
			firstWord == "public" ||
			firstWord == "protected" ||
			firstWord == "private")
			indentation += 4;
		break;
	}

	//Properly indent the new line
	auto textInserted = "\n" + CreateIndent(indentation);
	action.SetTextInserted(textInserted);
	AtomicInsertText(textInserted);
	action.SetSelectionAfterInsert(selection);
	action.SetFinalSelection(selection);
	RecordAction(action);
}

void Document::Tabify()
{
	DocumentAction action;
	action.SetOriginalSelection(selection);
	auto cursorPosition = selection.GetEnd();

	selection.SetStartLine(0);
	selection.SetStartColumn(0);
	selection.SetEndLine(lines.size() - 1);
	selection.SetEndColumn(CalculateColumnWidth(lines.back()));
	selection.SetVertical(false);
	action.SetSelectionBeforeDelete(selection);
	action.SetTextDeleted(GetSelectedText());

	for (auto& text: lines)
	{
		auto firstPosition = text.find_first_not_of("\t ");
		if (firstPosition == std::string::npos)
		{
			if (!text.empty())
				text = CreateIndent((CalculateColumnWidth(text) / 4) * 4);
		}
		else if (firstPosition > 0)
		{
			text = CreateIndent((CalculateColumnWidth(text.substr(0, firstPosition)) / 4) * 4) + text.substr(firstPosition);
		}
	}

	action.SetTextInserted(GetSelectedText());

	selection.SetEndLine(0);
	selection.SetEndColumn(0);
	action.SetSelectionBeforeInsert(selection);

	selection.SetEndLine(lines.size() - 1);
	selection.SetEndColumn(CalculateColumnWidth(lines.back()));
	action.SetSelectionAfterInsert(selection);

	selection.SetStart(cursorPosition);
	selection.SetEnd(cursorPosition);
	action.SetFinalSelection(selection);
	RecordAction(action);
}

void Document::InsertFileHeader()
{
	DocumentAction action;
	action.SetOriginalSelection(selection);
	selection.SetStartLine(0);
	selection.SetStartColumn(0);
	selection.SetEnd(selection.GetStart());
	selection.SetVertical(false);
	action.SetSelectionBeforeDelete(selection);
	action.SetTextDeleted("");
	action.SetSelectionBeforeInsert(selection);
	auto textInserted = DocumentUtility::GenerateFileHeader(fileName, "...");
	action.SetTextInserted(textInserted);
	AtomicInsertText(textInserted);
	action.SetSelectionAfterInsert(selection);
	action.SetFinalSelection(selection);
	RecordAction(action);
}

void Document::InsertOneTimeInclude()
{
	DocumentAction action;
	action.SetOriginalSelection(selection);
	selection.SetStartLine(0);
	selection.SetStartColumn(0);
	selection.SetEnd(selection.GetStart());
	selection.SetVertical(false);
	action.SetSelectionBeforeDelete(selection);
	action.SetTextDeleted("");
	action.SetSelectionBeforeInsert(selection);
	auto textInserted = DocumentUtility::GenerateOneTimeInclude();
	action.SetTextInserted(textInserted);
	AtomicInsertText(textInserted);
	action.SetSelectionAfterInsert(selection);
	action.SetFinalSelection(selection);
	RecordAction(action);
}

void Document::SelectPosition(const DocumentPosition& position, bool extend, bool isVertical)
{
	//Region start/end define the region that needs to be redrawn
	auto regionStart = selection.GetStart();
	auto regionEnd = selection.GetEnd();

	//Set the new end to be the given position and update the vertical text selection flag
	selection.SetEnd(position);
	selection.SetVertical(isVertical);

	if (extend)
	{
		//The redraw region only need to extend from the previous end to the new position
		regionStart = position;
	}
	else
	{
		//We are not extending, so set the start to the new position as well
		selection.SetStart(position);
	}

	//Notify the observer that the given region (ordered from start to end) needs to be redrawn
	if (regionStart < regionEnd)
		std::swap(regionStart, regionEnd);
	events->OnDocumentEditRegion(regionStart, regionEnd);

	//Notify the observer that the selection has changed.
	events->OnDocumentSelectionChanged();
}

void Document::SelectAll()
{
	SelectStartOfFile(false, false);
	SelectEndOfFile(true, false);
}

void Document::SelectPreviousCharacter(bool extend, bool isVertical)
{
	auto line = selection.GetEndLine();
	auto column = selection.GetEndColumn();

	if (column == 0)
	{
		if (line > 0)
		{
			--line;
			column = CalculateColumnWidth(lines[line]);
		}
	}
	else
	{
		auto index = CalculateIndexFromColumn(lines[line], column);
		column = CalculateColumnWidth(lines[line].substr(0, index - 1));
	}

	SelectPosition(DocumentPosition(line, column), extend, isVertical);
}

void Document::SelectNextCharacter(bool extend, bool isVertical)
{
	auto line = selection.GetEndLine();
	auto column = selection.GetEndColumn();
	auto columnWidth = CalculateColumnWidth(lines[line]);

	if (column == columnWidth)
	{
		if ((line + 1) < lines.size())
		{
			++line;
			column = 0;
		}
	}
	else
	{
		auto index = CalculateIndexFromColumn(lines[line], column);
		column = CalculateNextColumn(lines[line][index], column);
	}

	SelectPosition(DocumentPosition(line, column), extend, isVertical);
}

void Document::SelectPreviousWord(bool extend, bool isVertical, bool skipWhitespace)
{
	auto line = selection.GetEndLine();
	auto column = selection.GetEndColumn();
	auto columnWidth = CalculateColumnWidth(lines[line]);
	auto index = column == columnWidth ? lines[line].size() : CalculateIndexFromColumn(lines[line], column);

	//Skip to the end of the previous line if at the beginning of the currnet line
	if (index == 0 && skipWhitespace)
	{
		if (line > 0)
		{
			--line;
			index = lines[line].size();
		}
	}

	const auto& text = lines[line];

	//Skip backwards over whitespace
	while (skipWhitespace && index > 0 && std::isspace(text[index - 1]))
		--index;

	if (index > 0)
	{
		if (STRING::isidnum(text[index - 1]))
		{
			//Continue selection to the beginning of the identifier or number
			while (index > 0 && STRING::isidnum(text[index - 1]))
				--index;
		}
		else
		{
			//Continue selection to the beginning of the punctuation
			while (index > 0 && std::ispunct(text[index - 1]))
				--index;
		}
	}

	column = CalculateColumnWidth(text.substr(0, index));
	SelectPosition(DocumentPosition(line, column), extend, isVertical);
}

void Document::SelectNextWord(bool extend, bool isVertical, bool skipWhitespace)
{
	auto line = selection.GetEndLine();
	auto column = selection.GetEndColumn();
	auto columnWidth = CalculateColumnWidth(lines[line]);

	//Skip to the beginning of the next line if we are at the end of the current line
	if (column == columnWidth && (line + 1) < lines.size())
	{
		++line;
		column = 0;
	}

	const auto& text = lines[line];
	auto index = CalculateIndexFromColumn(text, column);
	auto advanceIndex = [&]()
	{
		column = CalculateNextColumn(text[index], column);
		++index;
	};

	if (index < text.size())
	{
		if (STRING::isidnum(text[index]))
		{
			//Continue selection to the end of the identifier or number
			while (index < text.size() && STRING::isidnum(text[index]))
				advanceIndex();
		}
		else
		{
			//Continue selection to the end of the punctuation
			while (index < text.size() && std::ispunct(text[index]))
				advanceIndex();
		}
	}

	//Skip over whitespace
	while (skipWhitespace && index < text.size() && std::isspace(text[index]))
		advanceIndex();

	SelectPosition(DocumentPosition(line, column), extend, isVertical);
}

void Document::SelectPreviousLine(bool extend, bool isVertical)
{
	SelectLine(-1, extend, isVertical);
}

void Document::SelectNextLine(bool extend, bool isVertical)
{
	SelectLine(1, extend, isVertical);
}

void Document::SelectLine(long delta, bool extend, bool isVertical)
{
	auto line = selection.GetEndLine();
	auto column = selection.GetEndColumn();
	line = MATH::Bound(0, static_cast<long>(lines.size() - 1), static_cast<long>(line) + delta);
	column = MATH::Bound(0ul, CalculateColumnWidth(lines[line]), column);
	SelectPosition(DocumentPosition(line, column), extend, isVertical);
}

void Document::SelectStartOfLine(bool extend, bool isVertical)
{
	auto line = selection.GetEndLine();
	auto column = selection.GetEndColumn();
	auto firstNonSpace = lines[line].find_first_not_of(" \t");
	auto leadingSpaceWidth = firstNonSpace == std::string::npos ? 0 : CalculateColumnWidth(lines[line].substr(0, firstNonSpace));
	if (column == leadingSpaceWidth || firstNonSpace == std::string::npos)
		column = 0;
	else
		column = leadingSpaceWidth;
	SelectPosition(DocumentPosition(line, column), extend, isVertical);
}

void Document::SelectEndOfLine(bool extend, bool isVertical)
{
	auto line = selection.GetEndLine();
	auto lastColumn = CalculateColumnWidth(lines[line]);
	SelectPosition(DocumentPosition(line, lastColumn), extend, isVertical);
}

void Document::SelectStartOfFile(bool extend, bool isVertical)
{
	DocumentPosition position(0, 0);
	SelectPosition(position, extend, isVertical);
}

void Document::SelectEndOfFile(bool extend, bool isVertical)
{
	DocumentPosition position(lines.size() - 1, CalculateColumnWidth(lines.back()));
	SelectPosition(position, extend, isVertical);
}

void Document::FindTextInDocument(const std::string& text, OutputTarget* outputTarget)
{
	if (outputTarget == nullptr || fileName.empty())
		return;

	auto count = 0ul;
	const auto& fileName = relativeFileName;
	outputTarget->Clear();

	if (text.empty())
		return;

	outputTarget->Append("Find results for '" + text + "' in '" + fileName + "'.\r\n");

	for (auto index = 0ul; index < lines.size(); ++index)
	{
		const auto& line = lines[index];
		auto iter = std::search(line.begin(), line.end(), text.begin(), text.end(), STRING::iequal_char());
		if (iter != line.end())
		{
			std::ostringstream out;
			out << "0> " << fileName << ":" << (index + 1) << ":" << ((iter - line.begin()) + 1) << ": " << line << "\r\n";
			outputTarget->Append(out.str());
			++count;
		}
	}

	outputTarget->Append(STRING::to_string(count) + " occurrence(s) found.\r\n");
}

unsigned long Document::CalculateColumnWidth(const std::string& line)
{
	auto columnWidth = 0ul;
	for (auto c: line)
		columnWidth = CalculateNextColumn(c, columnWidth);
	return columnWidth;
}

unsigned long Document::CalculateIndexFromColumn(const std::string& line, unsigned long column)
{
	auto columnWidth = 0ul;
	for (auto index = 0ul; index < line.size(); ++index)
	{
		if (columnWidth >= column)
			return index;
		columnWidth = CalculateNextColumn(line[index], columnWidth);
	}
	return line.size();
}

unsigned long Document::CalculateNextColumn(char c, unsigned long columnWidth)
{
	if (c == '\t')
		return columnWidth + 4 - (columnWidth % 4);
	return columnWidth + 1;
}

unsigned long Document::CalculateIndentOfLine(const std::string& value)
{
	auto firstPosition = value.find_first_not_of("\t ");
	if (firstPosition == std::string::npos)
		return 0;
	return CalculateColumnWidth(value.substr(0, firstPosition));
}

std::string Document::CreateIndent(unsigned long columnWidth)
{
	return std::string(columnWidth / 4, '\t') + std::string(columnWidth % 4, ' ');
}

std::string Document::FindFirstWordOfLine(const std::string& value)
{
	auto firstPosition = value.find_first_not_of("\t ");
	if (firstPosition == std::string::npos)
		return "";

	auto nextPosition = value.find_first_of("\t {}[]!@#$%^&*()\\|;:'\"<>,./?`~", firstPosition);
	return nextPosition == std::string::npos ?
		value.substr(firstPosition) :
		value.substr(firstPosition, nextPosition - firstPosition);
}

void Document::RecordAction(const DocumentAction& action)
{
	undoBuffer.push(action);
	while (!redoBuffer.empty())
		redoBuffer.pop();
	RaiseEvents();
}

void Document::RaiseEvents()
{
	events->OnDocumentSizeChanged();
	events->OnDocumentEditRegion(selection.GetStart(), selection.GetEnd());
	events->OnDocumentSelectionChanged();
}

