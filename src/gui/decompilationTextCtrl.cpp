#include "decompilationTextCtrl.h"
#include "mainGui.h"

#include "../decompiler/decompiler.h"
#include "../decompiler/decompilationUtils.h"
#include "../decompiler/intrinsics.h"

DecompilationTextCtrl::DecompilationTextCtrl(MainGui* parent, wxString name) : JdcTextCtrl(parent, name)
{
	mainGui = parent;
	EnableLineNumbers();

	Bind(wxEVT_CONTEXT_MENU, &DecompilationTextCtrl::DecompilationRightClickOptions, this);
	Bind(wxEVT_STC_UPDATEUI, &DecompilationTextCtrl::OnUpdateDecompilationUI, this);
}

void DecompilationTextCtrl::ShowRenameDialog(struct JdcStr* currentName)
{
	wxTextEntryDialog dlg(this, "", "Rename " + wxString(currentName->buffer));
	if (dlg.ShowModal() == wxID_OK)
	{
		wxString txt = dlg.GetValue();
		mainGui->decompParams.currentFunc = &mainGui->functions[currentDecompiledFunc];
		if (validateName(&mainGui->decompParams, txt.c_str())) 
		{
			strcpyJdc(currentName, txt.c_str());
			mainGui->RefreshVarNames();
		}
		else 
		{
			wxMessageBox("Name is invalid or already in use", "Can't rename");
		}
	}
}

void DecompilationTextCtrl::DecompilationRightClickOptions(wxContextMenuEvent& e)
{
	wxMenu menu;

	const int ID_DECOMPILE = 100;
	const int ID_RENAME = 101;
	const int ID_SET_ASSOCIATED_DISASSEMBLY = 102;
	const int ID_UNASSOCIATE_DISASSEMBLY = 103;

	int pos = GetCurrentPos();
	int start = WordStartPosition(pos, true);
	int end = WordEndPosition(pos, true);
	wxString word = GetTextRange(start, end);
	if (word != "")
	{
		unsigned char foundName = 0;

		int numOfFunctions = mainGui->functions.size();
		for (int i = 0; i < numOfFunctions; i++)
		{
			if(strcmp(mainGui->functions[i].name.buffer, word.c_str()) == 0)
			{
				if (i != currentDecompiledFunc) 
				{
					menu.Append(ID_DECOMPILE, "Decompile");
					menu.Bind(wxEVT_MENU, [&](wxCommandEvent&) {
						mainGui->AddDecompilationTextCtrl()->DecompileFunction(i);
					}, ID_DECOMPILE);
				}

				menu.Append(ID_RENAME, "Rename " + wxString(mainGui->functions[i].name.buffer));
				menu.Bind(wxEVT_MENU, [&](wxCommandEvent&) {
					ShowRenameDialog(&mainGui->functions[i].name);
				}, ID_RENAME);
				
				foundName = 1;
				break;
			}
		}

		if (currentDecompiledFunc != -1 && !foundName)
		{
			struct Function* func = &mainGui->decompParams.functions[currentDecompiledFunc];

			for (int i = 0; i < func->numOfRegArgs; i++)
			{
				if (strcmp(func->regArgs[i].name.buffer, word.c_str()) == 0)
				{
					menu.Append(ID_RENAME, "Rename " + wxString(func->regArgs[i].name.buffer));
					menu.Bind(wxEVT_MENU, [&](wxCommandEvent&) {
						ShowRenameDialog(&func->regArgs[i].name);
					}, ID_RENAME);

					foundName = 1;
					break;
				}
			}

			for (int i = 0; i < func->numOfStackArgs && !foundName; i++)
			{
				if (strcmp(func->stackArgs[i].name.buffer, word.c_str()) == 0)
				{
					menu.Append(ID_RENAME, "Rename " + wxString(func->stackArgs[i].name.buffer));
					menu.Bind(wxEVT_MENU, [&](wxCommandEvent&) {
						ShowRenameDialog(&func->stackArgs[i].name);
					}, ID_RENAME);

					foundName = 1;
					break;
				}
			}

			for (int i = 0; i < func->numOfRegVars && !foundName; i++)
			{
				if (strcmp(func->regVars[i].name.buffer, word.c_str()) == 0)
				{
					menu.Append(ID_RENAME, "Rename " + wxString(func->regVars[i].name.buffer));
					menu.Bind(wxEVT_MENU, [&](wxCommandEvent&) {
						ShowRenameDialog(&func->regVars[i].name);
					}, ID_RENAME);

					foundName = 1;
					break;
				}
			}

			for (int i = 0; i < func->numOfStackVars && !foundName; i++)
			{
				if (strcmp(func->stackVars[i].name.buffer, word.c_str()) == 0)
				{
					menu.Append(ID_RENAME, "Rename " + wxString(func->stackVars[i].name.buffer));
					menu.Bind(wxEVT_MENU, [&](wxCommandEvent&) {
						ShowRenameDialog(&func->stackVars[i].name);
					}, ID_RENAME);

					foundName = 1;
					break;
				}
			}

			for (int i = 0; i < func->numOfReturnedVars && !foundName; i++)
			{
				if (strcmp(func->returnedVars[i].name.buffer, word.c_str()) == 0)
				{
					menu.Append(ID_RENAME, "Rename " + wxString(func->returnedVars[i].name.buffer));
					menu.Bind(wxEVT_MENU, [&](wxCommandEvent&) {
						ShowRenameDialog(&func->returnedVars[i].name);
					}, ID_RENAME);

					foundName = 1;
					break;
				}
			}
		}
	}

	AddDefaultRightClickOptions(&menu);

	menu.Append(ID_SET_ASSOCIATED_DISASSEMBLY, "Set associated disassembly");
	menu.Bind(wxEVT_MENU, [&](wxCommandEvent&) {
		wxArrayString windowCaptions;
		for (int i = 0; i < mainGui->disassemblyTextCtrls.size(); i++)
		{
			windowCaptions.push_back(mainGui->disassemblyTextCtrls[i]->GetName());
		}
		windowCaptions.push_back("New window");
		wxSingleChoiceDialog choiceDialog(this, "", "Choose a window", windowCaptions);
		if (choiceDialog.ShowModal() != wxID_CANCEL)
		{
			int selection = choiceDialog.GetSelection();
			if (selection == mainGui->disassemblyTextCtrls.size())
			{
				disassemblyTextCtrl = mainGui->AddDisassemblyTextCtrl();
			}
			else
			{
				disassemblyTextCtrl = mainGui->disassemblyTextCtrls[selection];
			}
		}
	}, ID_SET_ASSOCIATED_DISASSEMBLY);

	if (disassemblyTextCtrl)
	{
		menu.Append(ID_UNASSOCIATE_DISASSEMBLY, "Unassociate " + disassemblyTextCtrl->GetName());
		menu.Bind(wxEVT_MENU, [&](wxCommandEvent&) {
			disassemblyTextCtrl->ClearIndicators();
			disassemblyTextCtrl = nullptr;
		}, ID_UNASSOCIATE_DISASSEMBLY);
	}

	PopupMenu(&menu, ScreenToClient(e.GetPosition()));
}

void DecompilationTextCtrl::OnUpdateDecompilationUI(wxStyledTextEvent& e)
{
	if (!HasFocus())
	{
		return;
	}

	ClearIndicators();
	
	unsigned char isLineHighlighted = 0;

	if (disassemblyTextCtrl && HasFocus())
	{
		disassemblyTextCtrl->ClearIndicators();
		int selectedLine = GetCurrentLine();
		if (currentDecompiledFunc != -1 && selectedLine < mainGui->decompParams.functions[currentDecompiledFunc].associatedInstructionsBufferLen)
		{
			struct AssociatedInstructions* a = &mainGui->decompParams.functions[currentDecompiledFunc].associatedInstructions[selectedLine];

			for (int i = 0; i < a->numOfIndexes; i++)
			{
				disassemblyTextCtrl->HighlightLine(a->indexes[i], PURPLE_INDICATOR, 1);
			}

			ClearIndicators();
			HighlightLine(selectedLine, PURPLE_INDICATOR, 0);
			isLineHighlighted = 1;
		}
	}

	if (highlightSelectedLines && !isLineHighlighted)
	{
		HighlightLine(GetCurrentLine(), YELLOW_INDICATOR, 0);
	}

	HighlightSelectedBraces();
	HighlightSelectionInstances();
}

void DecompilationTextCtrl::DecompileFunction(int functionIndex)
{
	if (mainGui->decompParams.numOfFileBytes == 0)
	{
		wxMessageBox("No file opened", "Can't decompile");
		return;
	}

	ClearText();
	ClearIndicators();

	if (disassemblyTextCtrl)
	{
		disassemblyTextCtrl->ClearIndicators();
	}

	mainGui->decompParams.currentFunc = &mainGui->decompParams.functions[functionIndex];

	struct JdcStr decompilationResult = initializeJdcStr();
	if (decompilationResult.bufferSize == 0)
	{
		wxMessageBox("Error allocating memory for function decompilation", "Can't decompile");
		return;
	}

	struct JdcStr statusMessage = initializeJdcStr();
	int errorInstructionIndex = 0;
	if (!decompileFunction(&mainGui->decompParams, &decompilationResult, &statusMessage, &errorInstructionIndex))
	{
		if (disassemblyTextCtrl)
		{
			disassemblyTextCtrl->HighlightLine(errorInstructionIndex, RED_INDICATOR, 1);
		}

		mainGui->logTextCtrl->Log(statusMessage.buffer, 1);
		wxMessageBox(statusMessage.buffer, "Can't decompile");
		freeJdcStr(&statusMessage);

		int showOutput = wxMessageBox("Do you still want to see the mangled output?", "Show output", wxYES_NO, this);
		if (showOutput == wxNO)
		{
			freeJdcStr(&decompilationResult);
			return;
		}
	}

	currentDecompiledFunc = functionIndex;
	SetReadOnly(false);
	SetValue(decompilationResult.buffer);
	freeJdcStr(&decompilationResult);
	ApplyDecompilationHighlighting();
	SetReadOnly(true);
}

void DecompilationTextCtrl::ApplyDecompilationHighlighting()
{
	if (!mainGui->decompParams.currentFunc)
	{
		return;
	}

	for (int i = 0; i < NUM_OF_DECOMP_COLORS; i++)
	{
		StyleSetForeground(i, mainGui->colorsMenu->decompColors[i]);
	}

	wxString text = GetValue();

	StartStyling(0);
	SetStyling(text.length(), OPERATOR_DECOMP_COLOR);

	// stack vars
	for (int i = 0; i < mainGui->decompParams.currentFunc->numOfStackVars; i++)
	{
		ColorAllStrs(text, mainGui->decompParams.currentFunc->stackVars[i].name.buffer, LOCAL_VAR_DECOMP_COLOR, 1);
	}

	// reg vars
	for (int i = 0; i < mainGui->decompParams.currentFunc->numOfRegVars; i++)
	{
		ColorAllStrs(text, mainGui->decompParams.currentFunc->regVars[i].name.buffer, LOCAL_VAR_DECOMP_COLOR, 1);
	}

	// returned vars
	for (int i = 0; i < mainGui->decompParams.currentFunc->numOfReturnedVars; i++)
	{
		ColorAllStrs(text, mainGui->decompParams.currentFunc->returnedVars[i].name.buffer, LOCAL_VAR_DECOMP_COLOR, 1);
	}

	// stack args
	for (int i = 0; i < mainGui->decompParams.currentFunc->numOfStackArgs; i++)
	{
		ColorAllStrs(text, mainGui->decompParams.currentFunc->stackArgs[i].name.buffer, ARGUMENT_DECOMP_COLOR, 1);
	}

	// reg args
	for (int i = 0; i < mainGui->decompParams.currentFunc->numOfRegArgs; i++)
	{
		ColorAllStrs(text, mainGui->decompParams.currentFunc->regArgs[i].name.buffer, ARGUMENT_DECOMP_COLOR, 1);
	}

	// imports
	for (int i = 0; i < mainGui->decompParams.numOfImports; i++)
	{
		ColorAllStrs(text, mainGui->decompParams.imports[i].name.buffer, IMPORT_DECOMP_COLOR, 0);
	}

	// intrinsic functions
	for (int i = 0; i < NUM_OF_RETURNING_INTRINSICS; i++)
	{
		ColorAllStrs(text, returningIntrinsicFuncs[i].name, INTRINSIC_DECOMP_COLOR, 0);
	}
	for (int i = 0; i < NUM_OF_VOID_INTRINSICS; i++)
	{
		if (voidIntrinsicFuncs[i].opcode == DATA) 
		{
			ColorAllStrs(text, voidIntrinsicFuncs[i].name, ERROR_DECOMP_COLOR, 0);
		}
		else 
		{
			ColorAllStrs(text, voidIntrinsicFuncs[i].name, INTRINSIC_DECOMP_COLOR, 0);
		}
	}

	// keywords
	for (int i = 0; i < NUM_OF_KEYWORDS; i++)
	{
		ColorAllStrs(text, keywordStrs[i], KEYWORD_DECOMP_COLOR, 0);
	}

	// strings
	int start = 0;
	while (start < text.length())
	{
		int pos = text.find("\"", start);
		int end = text.find("\"", pos + 1);
		if (pos != wxNOT_FOUND && end != wxNOT_FOUND)
		{
			StartStyling(pos);
			SetStyling(end - pos + 1, STRING_DECOMP_COLOR);

			start = end + 1;
		}
		else
		{
			break;
		}
	}

	// labels
	start = 0;
	while (start < text.length())
	{
		int pos = text.find("label_", start);
		int end = text.find("\n", pos + 1);

		if (pos != wxNOT_FOUND && end != wxNOT_FOUND)
		{
			StartStyling(pos);
			SetStyling(end - pos - 1, LABEL_DECOMP_COLOR);

			start = end + 1;
		}
		else
		{
			break;
		}
	}

	// regs/segs that arent variables/arguments
	for (int i = 0; i < NUM_OF_REGISTERS; i++)
	{
		ColorAllStrs(text, registerStrs[i], ERROR_DECOMP_COLOR, 0);
	}
	for (int i = 0; i < NUM_OF_SEGMENTS; i++)
	{
		ColorAllStrs(text, segmentStrs[i], ERROR_DECOMP_COLOR, 0);
	}
	ColorAllStrs(text, "ERROR", ERROR_DECOMP_COLOR, 0);
	ColorAllStrs(text, "jumpTo", ERROR_DECOMP_COLOR, 0);

	// numbers
	const char* numberChars[17] = { "0x", "0", "1", "2", "3", "4", "5", "6", "7", "8", "9", "A", "B", "C", "D", "E", "F" };
	for (int i = 0; i < 17; i++)
	{
		ColorAllStrs(text, numberChars[i], NUMBER_DECOMP_COLOR, 0);
	}

	// functions
	for (int i = 0; i < mainGui->decompParams.numOfFunctions; i++)
	{
		ColorAllStrs(text, mainGui->decompParams.functions[i].name.buffer, FUNCTION_DECOMP_COLOR, 0);
	}

	// calling conventions
	for (int i = 0; i < NUM_OF_CALLING_CONVENTIONS; i++)
	{
		ColorAllStrs(text, callingConventionStrs[i], PRIMITIVE_DECOMP_COLOR, 0);
	}

	// primitive data types
	for (int i = 0; i < NUM_OF_PRIMITIVE_TYPES; i++)
	{
		ColorAllStrs(text, primitiveTypeStrs[i], PRIMITIVE_DECOMP_COLOR, 0);
	}
	ColorAllStrs(text, "unsigned", PRIMITIVE_DECOMP_COLOR, 0);
	ColorAllStrs(text, "sizeof", PRIMITIVE_DECOMP_COLOR, 0);

	// this is for when :: is part of a function name
	ColorAllStrs(text, ":", OPERATOR_DECOMP_COLOR, 1);

	// comments
	start = 0;
	while (start < text.length())
	{
		int pos = text.find("//", start);
		int end = text.find("\n", pos + 1);
		if (pos != wxNOT_FOUND)
		{
			if (end == wxNOT_FOUND)
			{
				end = text.length() - 1;
			}

			StartStyling(pos);
			SetStyling(end - pos + 1, COMMENT_DECOMP_COLOR);

			start = end + 1;
		}
		else
		{
			break;
		}
	}
}

void DecompilationTextCtrl::ColorAllStrs(wxString text, const char* str, DecompilationColor color, unsigned char forceColor)
{
	if (!str || !strcmp(str, ""))
	{
		return;
	}

	int start = 0;
	int pos = 0;
	while (start < text.length())
	{
		pos = text.find(str, start);
		if (pos != wxNOT_FOUND)
		{
			int end = pos + strlen(str);

			if (forceColor ||
				GetStyleAt(pos) == color || // incase there are two strs that are equal except for one having more text at the end
				GetStyleAt(pos) == OPERATOR_DECOMP_COLOR) // only apply color if it hasn't been colored yet
			{
				StartStyling(pos);
				SetStyling(strlen(str), color);
			}

			start = end;
		}
		else
		{
			break;
		}
	}
}