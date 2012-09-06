////////////////////////////////////////////////////////////////////////////////
// Filename:    DocumentAction.cpp
// Description: This file implements all DocumentAction member functions.
//
// Created:     2012-08-12 10:41:16
// Author:      Jacob Buysse
//                                                              Modification Log
// When       Who               Description
// ---------- ----------------- ------------------------------------------------
////////////////////////////////////////////////////////////////////////////////
#include "pch.h"
#include "DocumentAction.h"

void DocumentAction::Redo(DocumentOperations* target) const
{
	//Restore original selection before any potential text was deleted
	target->SelectPosition(selectionBeforeDelete.GetStart(), false, false);
	target->SelectPosition(selectionBeforeDelete.GetEnd(), true, selectionBeforeDelete.IsVertical());

	//Delete the selected text before we insert the new positential text
	target->AtomicDeleteSelection();

	//Restore selection before any potential insert
	target->SelectPosition(selectionBeforeInsert.GetStart(), false, false);
	target->SelectPosition(selectionBeforeInsert.GetEnd(), true, selectionBeforeInsert.IsVertical());

	//Add the potentially inserted text at this point.
	target->AtomicInsertText(textInserted);

	//Restore the final position
	target->SelectPosition(finalSelection.GetStart(), false, false);
	target->SelectPosition(finalSelection.GetEnd(), true, finalSelection.IsVertical());
}

void DocumentAction::Undo(DocumentOperations* target) const
{
	//Restore the final position extended to the selection prior to text insertion
	target->SelectPosition(selectionBeforeInsert.GetStart(), false, false);
	target->SelectPosition(selectionAfterInsert.GetEnd(), true, selectionAfterInsert.IsVertical());

	//Delete the text that was previous inserted
	target->AtomicDeleteSelection();

	//Restore the deleted text
	target->AtomicInsertText(textDeleted);

	//Restore the original selection
	target->SelectPosition(originalSelection.GetStart(), false, false);
	target->SelectPosition(originalSelection.GetEnd(), true, originalSelection.IsVertical());
}

void DocumentAction::SetOriginalSelection(const DocumentSelection& value)
{
	originalSelection = value;
}

void DocumentAction::SetSelectionBeforeDelete(const DocumentSelection& value)
{
	selectionBeforeDelete = value;
}

void DocumentAction::SetTextDeleted(const std::string& value)
{
	textDeleted = value;
}

void DocumentAction::SetSelectionBeforeInsert(const DocumentSelection& value)
{
	selectionBeforeInsert = value;
}

void DocumentAction::SetTextInserted(const std::string& value)
{
	textInserted = value;
}

void DocumentAction::SetSelectionAfterInsert(const DocumentSelection& value)
{
	selectionAfterInsert = value;
}

void DocumentAction::SetFinalSelection(const DocumentSelection& value)
{
	finalSelection = value;
}

