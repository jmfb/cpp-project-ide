////////////////////////////////////////////////////////////////////////////////
// Filename:    DocumentAction.h
// Description: This file declares the DocumentAction class.  This class
//              represents an action to perform or undo (since it was previously
//              done) to a document.
//
// Created:     2012-08-12 10:37:29
// Author:      Jacob Buysse
//                                                              Modification Log
// When       Who               Description
// ---------- ----------------- ------------------------------------------------
////////////////////////////////////////////////////////////////////////////////
#pragma once
#include "DocumentSelection.h"
#include "DocumentOperations.h"

class DocumentAction
{
public:
	DocumentAction() = default;
	DocumentAction(const DocumentAction& rhs) = default;
	~DocumentAction() = default;

	DocumentAction& operator=(const DocumentAction& rhs) = default;

	void Redo(DocumentOperations* target) const;
	void Undo(DocumentOperations* target) const;

	void SetOriginalSelection(const DocumentSelection& value);
	void SetSelectionBeforeDelete(const DocumentSelection& value);
	void SetTextDeleted(const std::string& value);
	void SetSelectionBeforeInsert(const DocumentSelection& value);
	void SetTextInserted(const std::string& value);
	void SetSelectionAfterInsert(const DocumentSelection& value);
	void SetFinalSelection(const DocumentSelection& value);

private:
	DocumentSelection originalSelection;
	DocumentSelection selectionBeforeDelete;
	std::string textDeleted;
	DocumentSelection selectionBeforeInsert;
	std::string textInserted;
	DocumentSelection selectionAfterInsert;
	DocumentSelection finalSelection;
};

