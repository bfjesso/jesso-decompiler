#include "functionsTextCtrl.h"
#include "mainGui.h"

#include "../decompiler/functions.h"
#include "../decompiler/decompiler.h"

FunctionsTextCtrl::FunctionsTextCtrl(wxWindow* parent, MainGui* mainGuiRef, wxString name) : JdcTextCtrl(parent, mainGuiRef, name)
{
	EnableLineNumbers();

	Bind(wxEVT_CONTEXT_MENU, &FunctionsTextCtrl::FunctionsRightClickOptions, this);
	Bind(wxEVT_CHAR_HOOK, &FunctionsTextCtrl::OnFunctionsKeyDown, this);

	ShowAllFunctions(-1);
}

void FunctionsTextCtrl::ShowFindAddressDialog()
{
	wxTextEntryDialog dlg(this, "", "Find address");
	if (dlg.ShowModal() == wxID_OK)
	{
		wxString txt = dlg.GetValue();
		uint64_t address = 0;
		if (txt.ToULongLong((wxULongLong_t*)(&address), 16))
		{
			int32_t index = findFunctionByAddressInclusive(&mainGui->decompParams, address);
			if (index == -1)
			{
				wxMessageBox("Address not found", "Failed to find address");
				return;
			}

			CenterLine(index);
			return;
		}

		wxMessageBox("Not valid hex number", "Failed to find address");
	}
}

void FunctionsTextCtrl::FunctionsRightClickOptions(wxContextMenuEvent& e)
{
	wxMenu menu;

	const int32_t ID_DECOMPILE = 100;
	const int32_t ID_VIEW_INFO = 101;
	const int32_t ID_RENAME = 102;
	const int32_t ID_FIND_CODE_REFERENCES = 103;
	const int32_t ID_FIND_ADDRESS = 104;

	int32_t selectedLine = GetCurrentLine();
	if (selectedLine >= 0 && selectedLine < mainGui->decompParams.numOfFunctions) 
	{
		struct Function* function = &mainGui->decompParams.functions[selectedLine];

		menu.Append(ID_DECOMPILE, "Decompile");
		menu.Bind(wxEVT_MENU, [&](wxCommandEvent&) {
			if (mainGui->decompilationTextCtrls.size() == 0)
			{
				mainGui->AddDecompilationTextCtrl()->DecompileFunction(selectedLine);
			}
			else
			{
				wxArrayString windowCaptions;
				for (int32_t i = 0; i < mainGui->decompilationTextCtrls.size(); i++)
				{
					windowCaptions.push_back(mainGui->decompilationTextCtrls[i]->GetName());
				}
				windowCaptions.push_back("New window");
				wxSingleChoiceDialog choiceDialog(this, "", "Choose a window", windowCaptions);
				if (choiceDialog.ShowModal() != wxID_CANCEL)
				{
					int32_t selection = choiceDialog.GetSelection();
					if (selection == mainGui->decompilationTextCtrls.size())
					{
						mainGui->AddDecompilationTextCtrl()->DecompileFunction(selectedLine);
					}
					else
					{
						mainGui->decompilationTextCtrls[selection]->DecompileFunction(selectedLine);
					}
				}
			}
		}, ID_DECOMPILE);

		menu.Append(ID_VIEW_INFO, "View info");
		menu.Bind(wxEVT_MENU, [&](wxCommandEvent&) { mainGui->AddFunctionInfoWindow(function); }, ID_VIEW_INFO);

		int32_t pos = GetCurrentPos();
		int32_t start = WordStartPosition(pos, true);
		int32_t end = WordEndPosition(pos, true);
		wxString word = GetTextRange(start, end);
		if (function && word != "")
		{
			bool foundName = false;

			if (strcmp(function->name.buffer, word.c_str()) == 0)
			{
				menu.Append(ID_RENAME, "Rename");
				menu.Bind(wxEVT_MENU, [&](wxCommandEvent&) {
					ShowRenameDialog(selectedLine, &function->name);
				}, ID_RENAME);

				foundName = true;

				menu.Append(ID_FIND_CODE_REFERENCES, "Find code references to function");
				menu.Bind(wxEVT_MENU, [&](wxCommandEvent&) {
					ShowRenameDialog(selectedLine, &function->name);
					uint64_t functionAddress = mainGui->decompParams.instructions[function->firstInstructionIndex].address;
					mainGui->AddCodeReferencesWindow()->FindCodeReferences(functionAddress, 1);
				}, ID_FIND_CODE_REFERENCES);
			}

			for (int32_t i = 0; i < function->numOfRegVars && !foundName; i++)
			{
				struct RegisterVariable* regVar = &function->regVars[i];
				if (regVar->isArgument && strcmp(regVar->name.buffer, word.c_str()) == 0)
				{
					menu.Append(ID_RENAME, "Rename");
					menu.Bind(wxEVT_MENU, [&](wxCommandEvent&) {
						ShowRenameDialog(selectedLine, &regVar->name);
					}, ID_RENAME);

					foundName = true;
					break;
				}
			}

			for (int32_t i = 0; i < function->numOfStackVars && !foundName; i++)
			{
				struct StackVariable* stackVar = &function->stackVars[i];
				if (stackVar->isArgument && strcmp(stackVar->name.buffer, word.c_str()) == 0)
				{
					menu.Append(ID_RENAME, "Rename");
					menu.Bind(wxEVT_MENU, [&](wxCommandEvent&) {
						ShowRenameDialog(selectedLine, &stackVar->name);
					}, ID_RENAME);

					foundName = true;
					break;
				}
			}
		}
	}

	menu.Append(ID_FIND_ADDRESS, "Find function by address");
	menu.Bind(wxEVT_MENU, [&](wxCommandEvent&) {
		ShowFindAddressDialog();
	}, ID_FIND_ADDRESS);

	AddDefaultRightClickOptions(&menu);

	PopupMenu(&menu, ScreenToClient(e.GetPosition()));
}

void FunctionsTextCtrl::OnFunctionsKeyDown(wxKeyEvent& e)
{
	int32_t key = e.GetKeyCode();
	if ((e.GetModifiers() & wxMOD_CONTROL) != 0 && key != 0)
	{
		if (key == 'G')
		{
			ShowFindAddressDialog();
		}
	}

	OnKeyDown(e);
	e.Skip();
}

wxString FunctionsTextCtrl::GenerateFunctionDefinition(int32_t functionIndex, struct JdcStr* functionHeaderBuffer)
{
	struct Function* function = &mainGui->decompParams.functions[functionIndex];
	if (!generateFunctionHeader(function, mainGui->decompParams.useStdInt, functionHeaderBuffer))
	{
		return "";
	}

	sprintfJdc(functionHeaderBuffer, true, "; // address: 0x%llX; num of instructions: %d", mainGui->decompParams.instructions[function->firstInstructionIndex].address, function->lastInstructionIndex - function->firstInstructionIndex + 1);
	
	wxString result = wxString(functionHeaderBuffer->buffer);

	if (functionIndex == entryFunctionIndex)
	{
		result += "; entry point";
	}

	return result + "\n";
}

void FunctionsTextCtrl::UpdateFunctionHeader(int32_t functionIndex)
{
	if (functionIndex < 0 || functionIndex >= GetNumberOfLines()) 
	{
		return;
	}

	struct JdcStr functionHeaderBuffer = initializeJdcStr();
	wxString functionDefinition = GenerateFunctionDefinition(functionIndex, &functionHeaderBuffer);
	freeJdcStr(&functionHeaderBuffer);

	if (functionDefinition == "") 
	{
		return;
	}
	
	int32_t lineStart = PositionFromLine(functionIndex);
	int32_t lineLen = GetLineLength(functionIndex);

	SetReadOnly(false);
	Replace(lineStart, lineStart + lineLen + 1, functionDefinition);
	ApplyFunctionsHighlighting(lineStart, lineStart + functionDefinition.Length());
	SetReadOnly(true);
}

void FunctionsTextCtrl::ShowAllFunctions(int32_t highlightIndex)
{
	ClearText();
	SetReadOnly(false);
	Freeze();

	entryFunctionIndex = findFunctionByAddress(&mainGui->decompParams, mainGui->entryPoint + mainGui->imageBase);
	wxString functionsStr = "";
	struct JdcStr functionHeaderBuffer = initializeJdcStr();
	for (int32_t i = 0; i < mainGui->decompParams.numOfFunctions; i++)
	{
		wxString functionDefinition = GenerateFunctionDefinition(i, &functionHeaderBuffer);
		if (functionDefinition == "") 
		{
			freeJdcStr(&functionHeaderBuffer);
			return;
		}

		functionsStr += functionDefinition;
	}

	freeJdcStr(&functionHeaderBuffer);

	SetText(functionsStr);
	ApplyFunctionsHighlighting(0, GetTextLength());
	Thaw();
	SetReadOnly(true);

	if (highlightIndex != -1) 
	{
		HighlightLine(highlightIndex, YELLOW_INDICATOR, 1);
	}
	else 
	{
		HighlightLine(entryFunctionIndex, YELLOW_INDICATOR, 1);
	}
}

void FunctionsTextCtrl::ResetFunctionsList() 
{
	ShowAllFunctions(-1);
}

void FunctionsTextCtrl::ApplyFunctionsHighlighting(int32_t start, int32_t end)
{
	for (int32_t i = 0; i < NUM_OF_DECOMP_COLORS; i++)
	{
		StyleSetForeground(i, mainGui->colorsMenu->decompColors[i]);
	}

	wxString text = GetValue();

	StartStyling(start);

	int32_t funcIndex = 0;
	int32_t lineStart = start;
	while (lineStart < end)
	{
		struct Function* func = &mainGui->decompParams.functions[funcIndex];
		int32_t argsStartPos = text.find("(", lineStart);
		int32_t functionNamePos = text.rfind(" ", argsStartPos);
		int32_t callingConventionPos = text.rfind(" ", functionNamePos - 1);
		int32_t argsEndPos = text.find(")", lineStart);

		if (argsStartPos != wxNOT_FOUND && functionNamePos != wxNOT_FOUND && argsEndPos != wxNOT_FOUND)
		{
			int32_t numOfPtrs = 0;
			while (text[callingConventionPos - numOfPtrs - 1] == '*')
			{
				numOfPtrs++;
			}

			StartStyling(lineStart);
			SetStyling(callingConventionPos - lineStart - numOfPtrs, GetPrimitiveTypeColor(func->returnType.primitiveType, mainGui->decompParams.useStdInt));

			StartStyling(callingConventionPos);
			SetStyling(functionNamePos - callingConventionPos, PRIMITIVE_DECOMP_COLOR);

			StartStyling(functionNamePos);
			SetStyling(argsStartPos - functionNamePos, FUNCTION_DECOMP_COLOR);

			if (argsStartPos + 1 != argsEndPos)
			{
				int32_t argTypePos = argsStartPos + 1;
				int32_t argEndPos = text.find(",", argsStartPos);
				while (argEndPos != wxNOT_FOUND && argEndPos < argsEndPos)
				{
					int32_t argNamePos = text.rfind(" ", argEndPos);

					numOfPtrs = 0;
					while (text[argNamePos - numOfPtrs - 1] == '*')
					{
						numOfPtrs++;
					}

					wxString argName = text.substr(argNamePos + 1, argEndPos - argNamePos - 1);

					StartStyling(argTypePos);
					SetStyling(argNamePos - argTypePos - numOfPtrs, GetPrimitiveTypeColor(GetArgPrimitiveByName(func, argName.c_str().AsChar()), mainGui->decompParams.useStdInt));

					StartStyling(argNamePos);
					SetStyling(argEndPos - argNamePos, ARGUMENT_DECOMP_COLOR);

					argTypePos = argEndPos + 2;
					argEndPos = text.find(",", argTypePos);
				}

				// last argument does not end with a comma
				int32_t lastArgNamePos = text.rfind(" ", argsEndPos);
				StartStyling(lastArgNamePos);
				SetStyling(argsEndPos - lastArgNamePos, ARGUMENT_DECOMP_COLOR);

				numOfPtrs = 0;
				while (text[lastArgNamePos - numOfPtrs - 1] == '*')
				{
					numOfPtrs++;
				}

				wxString lastArgName = text.substr(lastArgNamePos + 1, argsEndPos - lastArgNamePos - 1);

				StartStyling(argTypePos);
				SetStyling(lastArgNamePos - argTypePos - numOfPtrs, GetPrimitiveTypeColor(GetArgPrimitiveByName(func, lastArgName.c_str().AsChar()), mainGui->decompParams.useStdInt));
			}

			int32_t commentStartPos = text.find(";", lineStart) + 1;
			StartStyling(commentStartPos);

			lineStart = text.find("\n", commentStartPos);
			if (lineStart == wxNOT_FOUND)
			{
				SetStyling(text.length() - commentStartPos, COMMENT_DECOMP_COLOR);
				break;
			}

			SetStyling(lineStart - commentStartPos + 1, COMMENT_DECOMP_COLOR);
			funcIndex++;
		}
		else
		{
			break;
		}
	}
}

enum DecompilationColor GetPrimitiveTypeColor(enum PrimitiveType primitive, bool useStdInt)
{
	if (useStdInt && primitive >= INT8_TYPE && primitive <= UINT64_TYPE)
	{
		return USER_TYPE_DECOMP_COLOR;
	}

	return PRIMITIVE_DECOMP_COLOR;
}

enum PrimitiveType GetArgPrimitiveByName(struct Function* func, const char* name) 
{
	for (int32_t i = 0; i < func->numOfRegVars; i++) 
	{
		if (func->regVars[i].isArgument && strcmp(func->regVars[i].name.buffer, name) == 0) 
		{
			return func->regVars[i].dataType.primitiveType;
		}
	}

	for (int32_t i = 0; i < func->numOfStackVars; i++)
	{
		if (func->stackVars[i].isArgument && strcmp(func->stackVars[i].name.buffer, name) == 0)
		{
			return func->stackVars[i].dataType.primitiveType;
		}
	}

	return VOID_TYPE;
}