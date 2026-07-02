#include "functionsTextCtrl.h"
#include "mainGui.h"
#include "functionInfoMenu.h"

#include "../decompiler/functions.h"
#include "../decompiler/decompiler.h"

FunctionsTextCtrl::FunctionsTextCtrl(MainGui* parent, wxString name) : JdcTextCtrl(parent, name)
{
	mainGui = parent;
	EnableLineNumbers();

	Bind(wxEVT_CONTEXT_MENU, &FunctionsTextCtrl::FunctionsRightClickOptions, this);
	Bind(wxEVT_CHAR_HOOK, &FunctionsTextCtrl::OnFunctionsKeyDown, this);

	ShowAllFunctions();
}

void FunctionsTextCtrl::ShowFindAddressDialog()
{
	wxTextEntryDialog dlg(this, "", "Find address");
	if (dlg.ShowModal() == wxID_OK)
	{
		wxString txt = dlg.GetValue();
		unsigned long long address = 0;
		if (txt.ToULongLong(&address, 16))
		{
			int index = findFunctionByAddressInclusive(&mainGui->decompParams, address);
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

	const int ID_DECOMPILE = 100;
	const int ID_VIEW_INFO = 101;
	const int ID_FIND_ADDRESS = 102;

	int selectedLine = GetCurrentLine();
	if (selectedLine >= 0 && selectedLine < mainGui->decompParams.numOfFunctions) 
	{
		menu.Append(ID_DECOMPILE, "Decompile");
		menu.Bind(wxEVT_MENU, [&](wxCommandEvent&) {
			if (mainGui->decompilationTextCtrls.size() == 0)
			{
				mainGui->AddDecompilationTextCtrl();
				mainGui->decompilationTextCtrls[0]->DecompileFunction(selectedLine);
			}
			else if (mainGui->decompilationTextCtrls.size() == 1)
			{
				mainGui->decompilationTextCtrls[0]->DecompileFunction(selectedLine);
			}
			else
			{
				wxArrayString windowCaptions;
				for (int i = 0; i < mainGui->decompilationTextCtrls.size(); i++)
				{
					windowCaptions.push_back(mainGui->decompilationTextCtrls[i]->GetName());
				}
				wxSingleChoiceDialog choiceDialog(this, "", "Choose a window", windowCaptions);
				if (choiceDialog.ShowModal() != wxID_CANCEL)
				{
					mainGui->decompilationTextCtrls[choiceDialog.GetSelection()]->DecompileFunction(selectedLine);
				}
			}
		}, ID_DECOMPILE);

		menu.Append(ID_VIEW_INFO, "View info");
		menu.Bind(wxEVT_MENU, [&](wxCommandEvent&) {
			new FunctionInfoMenu(this, GetPosition(), mainGui->decompParams.instructions, &mainGui->decompParams.functions[selectedLine]);
		}, ID_VIEW_INFO);
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
	int key = e.GetKeyCode();
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

void FunctionsTextCtrl::ShowAllFunctions()
{
	ClearText();
	SetReadOnly(false);
	Freeze();

	int entryFunctionIndex = findFunctionByAddress(&mainGui->decompParams, mainGui->entryPoint + mainGui->imageBase);
	wxString functionsStr = "";
	struct JdcStr functionHeaderBuffer = initializeJdcStr();
	for (int i = 0; i < mainGui->decompParams.numOfFunctions; i++)
	{
		struct Function* function = &mainGui->decompParams.functions[i];
		if (!generateFunctionHeader(function, &functionHeaderBuffer))
		{
			return;
		}

		sprintfJdc(&functionHeaderBuffer, 1, "; // address: 0x%llX; num of instructions: %d", mainGui->decompParams.instructions[function->firstInstructionIndex].address, function->lastInstructionIndex - function->firstInstructionIndex + 1);
		functionsStr += wxString(functionHeaderBuffer.buffer);

		if (i == entryFunctionIndex) 
		{
			functionsStr += "; entry point";
		}

		if (i != mainGui->decompParams.numOfFunctions - 1)
		{
			functionsStr += "\n";
		}
	}

	SetText(functionsStr);
	ApplyFunctionsHighlighting();
	Thaw();
	SetReadOnly(true);

	HighlightLine(entryFunctionIndex, YELLOW_INDICATOR, 1);
}

void FunctionsTextCtrl::ApplyFunctionsHighlighting()
{
	for (int i = 0; i < NUM_OF_DECOMP_COLORS; i++)
	{
		StyleSetForeground(i, mainGui->colorsMenu->decompColors[i]);
	}

	wxString text = GetValue();

	StartStyling(0);
	SetStyling(text.length(), OPERATOR_DECOMP_COLOR);

	int lineStart = 0;
	while (lineStart < text.length())
	{
		int argsStartPos = text.find("(", lineStart);
		int functionNamePos = text.rfind(" ", argsStartPos);
		int callingConventionPos = text.rfind(" ", functionNamePos - 1);
		int argsEndPos = text.find(")", lineStart);

		if (argsStartPos != wxNOT_FOUND && functionNamePos != wxNOT_FOUND && argsEndPos != wxNOT_FOUND)
		{
			int numOfPtrs = 0;
			while (text[callingConventionPos - numOfPtrs - 1] == '*')
			{
				numOfPtrs++;
			}

			StartStyling(lineStart);
			SetStyling(callingConventionPos - lineStart - numOfPtrs, PRIMITIVE_DECOMP_COLOR);

			StartStyling(callingConventionPos);
			SetStyling(functionNamePos - callingConventionPos, PRIMITIVE_DECOMP_COLOR);

			StartStyling(functionNamePos);
			SetStyling(argsStartPos - functionNamePos, FUNCTION_DECOMP_COLOR);

			if (argsStartPos + 1 != argsEndPos)
			{
				int argTypePos = argsStartPos + 1;
				int argEndPos = text.find(",", argsStartPos);
				while (argEndPos != wxNOT_FOUND && argEndPos < argsEndPos)
				{
					int argNamePos = text.rfind(" ", argEndPos);

					numOfPtrs = 0;
					while (text[argNamePos - numOfPtrs - 1] == '*')
					{
						numOfPtrs++;
					}

					StartStyling(argTypePos);
					SetStyling(argNamePos - argTypePos - numOfPtrs, PRIMITIVE_DECOMP_COLOR);

					StartStyling(argNamePos);
					SetStyling(argEndPos - argNamePos, ARGUMENT_DECOMP_COLOR);

					argTypePos = argEndPos + 2;
					argEndPos = text.find(",", argTypePos);
				}

				// last argument does not end with a comma
				int lastArgNamePos = text.rfind(" ", argsEndPos);
				StartStyling(lastArgNamePos);
				SetStyling(argsEndPos - lastArgNamePos, ARGUMENT_DECOMP_COLOR);

				numOfPtrs = 0;
				while (text[lastArgNamePos - numOfPtrs - 1] == '*')
				{
					numOfPtrs++;
				}

				StartStyling(argTypePos);
				SetStyling(lastArgNamePos - argTypePos - numOfPtrs, PRIMITIVE_DECOMP_COLOR);
			}

			int commentStartPos = text.find(";", lineStart) + 1;
			StartStyling(commentStartPos);

			lineStart = text.find("\n", commentStartPos);
			if (lineStart == wxNOT_FOUND)
			{
				SetStyling(text.length() - commentStartPos, COMMENT_DECOMP_COLOR);
				break;
			}

			SetStyling(lineStart - commentStartPos + 1, COMMENT_DECOMP_COLOR);
		}
		else
		{
			break;
		}
	}
}