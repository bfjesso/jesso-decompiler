#include "decompilationTextCtrl.h"
#include "mainGui.h"

#include "../decompiler/decompiler.h"
#include "../decompiler/decompilationUtils.h"
#include "../decompiler/intrinsics.h"

DecompilationTextCtrl::DecompilationTextCtrl(wxWindow* parent, MainGui* mainGuiRef, wxString name) : JdcTextCtrl(parent, mainGuiRef, name)
{
	EnableLineNumbers();

	Bind(wxEVT_CONTEXT_MENU, &DecompilationTextCtrl::DecompilationRightClickOptions, this);
	Bind(wxEVT_STC_UPDATEUI, &DecompilationTextCtrl::OnUpdateDecompilationUI, this);
}

void DecompilationTextCtrl::DecompilationRightClickOptions(wxContextMenuEvent& e)
{
	wxMenu menu;

	const int32_t ID_DECOMPILE = 100;
	const int32_t ID_RENAME = 101;
	const int32_t ID_FIND_CODE_REFERENCES = 102;
	const int32_t ID_SET_ASSOCIATED_DISASSEMBLY = 103;
	const int32_t ID_UNASSOCIATE_DISASSEMBLY = 104;

	int32_t pos = GetCurrentPos();
	int32_t start = WordStartPosition(pos, true);
	int32_t end = WordEndPosition(pos, true);
	wxString word = GetTextRange(start, end);
	if (word != "")
	{
		bool foundName = false;

		int32_t numOfFunctions = mainGui->functions.size();
		for (int32_t i = 0; i < numOfFunctions; i++)
		{
			struct Function* func = &mainGui->decompParams.functions[i];
			if(strcmp(func->name.buffer, word.c_str()) == 0)
			{
				if (i != currentDecompiledFunc) 
				{
					menu.Append(ID_DECOMPILE, "Decompile");
					menu.Bind(wxEVT_MENU, [&](wxCommandEvent&) {
						DecompilationTextCtrl* newDecompTextCtrl = mainGui->AddDecompilationTextCtrl();
						newDecompTextCtrl->DecompileFunction(i);
						newDecompTextCtrl->disassemblyTextCtrl = disassemblyTextCtrl;
					}, ID_DECOMPILE);
				}

				menu.Append(ID_RENAME, "Rename");
				menu.Bind(wxEVT_MENU, [&](wxCommandEvent&) {
					ShowRenameDialog(i, &func->name);
				}, ID_RENAME);

				menu.Append(ID_FIND_CODE_REFERENCES, "Find code references to function");
				menu.Bind(wxEVT_MENU, [&](wxCommandEvent&) {
					uint64_t functionAddress = mainGui->decompParams.instructions[func->firstInstructionIndex].address;
					mainGui->AddCodeReferencesWindow()->FindCodeReferences(functionAddress, 1);
				}, ID_FIND_CODE_REFERENCES);
				
				foundName = true;
				break;
			}
		}

		if (currentDecompiledFunc != -1 && !foundName)
		{
			struct Function* func = &mainGui->decompParams.functions[currentDecompiledFunc];

			for (int32_t i = 0; i < func->numOfRegVars && !foundName; i++)
			{
				if (strcmp(func->regVars[i].name.buffer, word.c_str()) == 0)
				{
					menu.Append(ID_RENAME, "Rename");
					menu.Bind(wxEVT_MENU, [&](wxCommandEvent&) {
						ShowRenameDialog(currentDecompiledFunc, &func->regVars[i].name);
					}, ID_RENAME);

					foundName = true;
					break;
				}
			}

			for (int32_t i = 0; i < func->numOfStackVars && !foundName; i++)
			{
				if (strcmp(func->stackVars[i].name.buffer, word.c_str()) == 0)
				{
					menu.Append(ID_RENAME, "Rename");
					menu.Bind(wxEVT_MENU, [&](wxCommandEvent&) {
						ShowRenameDialog(currentDecompiledFunc, &func->stackVars[i].name);
					}, ID_RENAME);

					foundName = true;
					break;
				}
			}

			for (int32_t i = 0; i < func->numOfReturnedVars && !foundName; i++)
			{
				if (strcmp(func->returnedVars[i].name.buffer, word.c_str()) == 0)
				{
					menu.Append(ID_RENAME, "Rename");
					menu.Bind(wxEVT_MENU, [&](wxCommandEvent&) {
						ShowRenameDialog(currentDecompiledFunc, &func->returnedVars[i].name);
					}, ID_RENAME);

					foundName = true;
					break;
				}
			}
		}
	}

	AddDefaultRightClickOptions(&menu);

	menu.Append(ID_SET_ASSOCIATED_DISASSEMBLY, "Set associated disassembly");
	menu.Bind(wxEVT_MENU, [&](wxCommandEvent&) {
		wxArrayString windowCaptions;
		for (int32_t i = 0; i < mainGui->disassemblyTextCtrls.size(); i++)
		{
			windowCaptions.push_back(mainGui->disassemblyTextCtrls[i]->GetName());
		}
		windowCaptions.push_back("New window");
		wxSingleChoiceDialog choiceDialog(this, "", "Choose a window", windowCaptions);
		if (choiceDialog.ShowModal() != wxID_CANCEL)
		{
			int32_t selection = choiceDialog.GetSelection();
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
	
	bool isLineHighlighted = false;

	if (disassemblyTextCtrl && HasFocus())
	{
		disassemblyTextCtrl->ClearIndicators();
		int32_t selectedLine = GetCurrentLine();
		if (currentDecompiledFunc != -1 && selectedLine < mainGui->decompParams.functions[currentDecompiledFunc].associatedInstructionsBufferLen)
		{
			struct AssociatedInstructions* a = &mainGui->decompParams.functions[currentDecompiledFunc].associatedInstructions[selectedLine];

			for (int32_t i = 0; i < a->numOfIndexes; i++)
			{
				disassemblyTextCtrl->HighlightLine(a->indexes[i], PURPLE_INDICATOR, 1);
			}

			ClearIndicators();
			HighlightLine(selectedLine, PURPLE_INDICATOR, 0);
			isLineHighlighted = true;
		}
	}

	if (highlightSelectedLines && !isLineHighlighted)
	{
		HighlightLine(GetCurrentLine(), YELLOW_INDICATOR, 0);
	}

	HighlightSelectedBraces();
	HighlightSelectionInstances();
}

void DecompilationTextCtrl::DecompileFunction(int32_t functionIndex)
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
	int32_t errorInstructionIndex = 0;
	if (!decompileFunction(&mainGui->decompParams, &decompilationResult, &statusMessage, &errorInstructionIndex))
	{
		if (disassemblyTextCtrl)
		{
			disassemblyTextCtrl->HighlightLine(errorInstructionIndex, RED_INDICATOR, 1);
		}

		mainGui->logTextCtrl->Log(statusMessage.buffer, 1);
		wxMessageBox(statusMessage.buffer, "Can't decompile");
		freeJdcStr(&statusMessage);

		int32_t showOutput = wxMessageBox("Do you still want to see the mangled output?", "Show output", wxYES_NO, this);
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

	for (int32_t i = 0; i < NUM_OF_DECOMP_COLORS; i++)
	{
		StyleSetForeground(i, mainGui->colorsMenu->decompColors[i]);
	}

	wxString text = GetValue();

	StartStyling(0);
	SetStyling(text.length(), OPERATOR_DECOMP_COLOR);

	// stack vars
	for (int32_t i = 0; i < mainGui->decompParams.currentFunc->numOfStackVars; i++)
	{
		struct StackVariable* stackVar = &mainGui->decompParams.currentFunc->stackVars[i];
		ColorAllStrs(text, stackVar->name.buffer, stackVar->isArgument ? ARGUMENT_DECOMP_COLOR : LOCAL_VAR_DECOMP_COLOR, true);
	}

	// reg vars
	for (int32_t i = 0; i < mainGui->decompParams.currentFunc->numOfRegVars; i++)
	{
		struct RegisterVariable* regVar = &mainGui->decompParams.currentFunc->regVars[i];
		ColorAllStrs(text, regVar->name.buffer, regVar->isArgument ? ARGUMENT_DECOMP_COLOR : LOCAL_VAR_DECOMP_COLOR, true);
	}

	// returned vars
	for (int32_t i = 0; i < mainGui->decompParams.currentFunc->numOfReturnedVars; i++)
	{
		ColorAllStrs(text, mainGui->decompParams.currentFunc->returnedVars[i].name.buffer, LOCAL_VAR_DECOMP_COLOR, true);
	}

	// imports
	for (int32_t i = 0; i < mainGui->decompParams.numOfImports; i++)
	{
		ColorAllStrs(text, mainGui->decompParams.imports[i].name.buffer, IMPORT_DECOMP_COLOR, false);
	}

	// intrinsic functions
	for (int32_t i = 0; i < NUM_OF_RETURNING_INTRINSICS; i++)
	{
		ColorAllStrs(text, returningIntrinsics[i].name, INTRINSIC_DECOMP_COLOR, false);
	}
	for (int32_t i = 0; i < NUM_OF_VOID_INTRINSICS; i++)
	{
		if (voidIntrinsics[i].opcode == DATA) 
		{
			ColorAllStrs(text, voidIntrinsics[i].name, ERROR_DECOMP_COLOR, false);
		}
		else 
		{
			ColorAllStrs(text, voidIntrinsics[i].name, INTRINSIC_DECOMP_COLOR, false);
		}
	}

	// calling conventions
	for (int32_t i = 0; i < NUM_OF_CALLING_CONVENTIONS; i++)
	{
		ColorAllStrs(text, callingConventionStrs[i], PRIMITIVE_DECOMP_COLOR, false);
	}

	// stdint
	for (int32_t i = INT8_TYPE; i <= UINT64_TYPE; i++)
	{
		ColorAllStrs(text, primitiveTypeToStr((enum PrimitiveType)i, true), USER_TYPE_DECOMP_COLOR, false);
	}

	// primitive data types
	for (int32_t i = 0; i < NUM_OF_PRIMITIVE_TYPES; i++)
	{
		ColorAllStrs(text, primitiveTypeToStr((enum PrimitiveType)i, false), PRIMITIVE_DECOMP_COLOR, false);
	}
	ColorAllStrs(text, "sizeof", PRIMITIVE_DECOMP_COLOR, false);

	// keywords
	for (int32_t i = 0; i < NUM_OF_KEYWORDS; i++)
	{
		ColorAllStrs(text, keywordStrs[i], KEYWORD_DECOMP_COLOR, false);
	}

	// strings
	int32_t start = 0;
	while (start < text.length())
	{
		int32_t pos = text.find("\"", start);
		int32_t end = text.find("\"", pos + 1);
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
		int32_t pos = text.find("label_", start);
		int32_t end = text.find("\n", pos + 1);

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

	// functions
	for (int32_t i = 0; i < mainGui->decompParams.numOfFunctions; i++)
	{
		ColorAllStrs(text, mainGui->decompParams.functions[i].name.buffer, FUNCTION_DECOMP_COLOR, false);
	}

	// this is for when :: is part of a function name
	ColorAllStrs(text, ":", OPERATOR_DECOMP_COLOR, true);

	// comments
	start = 0;
	while (start < text.length())
	{
		int32_t pos = text.find("//", start);
		int32_t end = text.find("\n", pos + 1);
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

	// hex numbers (these need to be forced due to conflicts with some reg names)
	start = 0;
	while (start < text.length())
	{
		int32_t num = text.find("0x", start);
		if (num != wxNOT_FOUND)
		{
			int32_t end = text.length();
			for (int32_t i = num + 2; i < end; i++)
			{
				if ((text[i] < '0' || text[i] > '9') && (text[i] < 'A' || text[i] > 'F'))
				{
					end = i;
					break;
				}
			}

			StartStyling(num);
			SetStyling(end - num, NUMBER_DECOMP_COLOR);

			start = end + 1;
		}
		else
		{
			break;
		}
	}

	// regs/segs that arent variables/arguments
	for (int32_t i = 0; i < NUM_OF_REGISTERS; i++)
	{
		ColorAllStrs(text, registerStrs[i], ERROR_DECOMP_COLOR, false);
	}
	for (int32_t i = 0; i < NUM_OF_SEGMENTS; i++)
	{
		ColorAllStrs(text, segmentStrs[i], ERROR_DECOMP_COLOR, false);
	}
	ColorAllStrs(text, "ERROR", ERROR_DECOMP_COLOR, false);
	ColorAllStrs(text, "jumpTo", ERROR_DECOMP_COLOR, false);

	// decimal numbers
	const char* numberChars[10] = { "0", "1", "2", "3", "4", "5", "6", "7", "8", "9" };
	for (int32_t i = 0; i < 10; i++)
	{
		ColorAllStrs(text, numberChars[i], NUMBER_DECOMP_COLOR, false);
	}
}

void DecompilationTextCtrl::ColorAllStrs(wxString text, const char* str, DecompilationColor color, bool forceColor)
{
	if (!str || !strcmp(str, ""))
	{
		return;
	}

	int32_t start = 0;
	int32_t pos = 0;
	while (start < text.length())
	{
		pos = text.find(str, start);
		if (pos != wxNOT_FOUND)
		{
			int32_t end = pos + strlen(str);

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