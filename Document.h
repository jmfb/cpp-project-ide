////////////////////////////////////////////////////////////////////////////////
// Filename:    Document.h
// Description: This file declares the Document class.  The Document class
//              represents the model for the file contents of a project item
//              that is open in the document window and potentially being viewed
//              by a document view.
//
// Created:     2012-08-12 10:18:08
// Author:      Jacob Buysse
//                                                              Modification Log
// When       Who               Description
// ---------- ----------------- ------------------------------------------------
////////////////////////////////////////////////////////////////////////////////
#pragma once
#include <string>
#include <vector>
#include <stack>
#include "DocumentSelection.h"
#include "DocumentPosition.h"
#include "DocumentAction.h"
#include "DocumentEvents.h"
#include "DocumentOperations.h"
#include "OutputTarget.h"

class Document : public DocumentOperations
{
public:
	Document() = default;
	Document(const Document& rhs) = default;
	~Document() = default;

	Document& operator=(const Document& rhs) = default;

	void Open(const std::string& fileName, const std::string& relativeFileName);
	const std::string& GetFileName() const;
	void SetFileName(const std::string& value);
	bool IsDirty() const;
	void Save();
	void SetEvents(DocumentEvents* events);
	SIZE GetSize() const;
	unsigned long GetMaxWidth() const;
	unsigned long GetLineCount() const;
	const std::string& GetLine(unsigned long index) const;
	unsigned long GetColumnWidth(unsigned long index) const;
	DocumentPosition HitTest(const DocumentPosition& position) const;
	bool HasSelectedText() const;

	const DocumentSelection& GetSelection() const final;
	std::string GetSelectedText() const final;
	void AtomicDeleteSelection() final;
	void AtomicInsertText(const std::string& value) final;

	//undo/redo functions
	bool CanUndo() const;
	void Undo();
	bool CanRedo() const;
	void Redo();

	//editing functions
	void InsertText(const std::string& value);
	void PerformTab(bool shift);
	void PerformDelete();
	void PerformBackspace();
	void InsertNewLine();
	void Tabify();
	void InsertFileHeader();
	void InsertOneTimeInclude();

	//selection functions
	void SelectPosition(const DocumentPosition& position, bool extend, bool isVertical) final;
	void SelectAll();
	void SelectPreviousCharacter(bool extend, bool isVertical);
	void SelectNextCharacter(bool extend, bool isVertical);
	void SelectPreviousWord(bool extend, bool isVertical, bool skipWhitespace);
	void SelectNextWord(bool extend, bool isVertical, bool skipWhitespace);
	void SelectPreviousLine(bool extend, bool isVertical);
	void SelectNextLine(bool extend, bool isVertical);
	void SelectLine(long delta, bool extend, bool isVertical);
	void SelectStartOfLine(bool extend, bool isVertical);
	void SelectEndOfLine(bool extend, bool isVertical);
	void SelectStartOfFile(bool extend, bool isVertical);
	void SelectEndOfFile(bool extend, bool isVertical);

	//bookmark functions
	void ToggleBookmark();
	void NextBookmark();
	void PreviousBookmark();
	bool IsLineBookmarked(unsigned long index) const;
	
	void FindTextInDocument(const std::string& text, OutputTarget* outputTarget);

private:
	static unsigned long CalculateColumnWidth(const std::string& line);
	static unsigned long CalculateIndexFromColumn(const std::string& line, unsigned long column);
	static unsigned long CalculateNextColumn(char c, unsigned long columnWidth);
	static unsigned long CalculateIndentOfLine(const std::string& value);
	static std::string CreateIndent(unsigned long columnWidth);
	static std::string FindFirstWordOfLine(const std::string& value);

	void RecordAction(const DocumentAction& action);
	void RaiseEvents();

private:
	std::string fileName;
	std::string relativeFileName;
	std::vector<std::string> lines;
	std::stack<DocumentAction> undoBuffer;
	std::stack<DocumentAction> redoBuffer;
	std::set<unsigned long> bookmarks;
	DocumentSelection selection;
	DocumentEvents* events = nullptr;
};

