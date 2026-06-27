#include "functionsTextCtrl.h"
#include "decompilationTextCtrl.h"
#include "functionInfoMenu.h"

#include "../decompiler/functions.h"
#include "../decompiler/decompiler.h"

FunctionsTextCtrl::FunctionsTextCtrl(wxWindow* parent, struct DecompilationParameters* decompParams, ColorsMenu* colorMenu, const std::function<DecompilationTextCtrl* ()>& getDecompTextCtrl) : JdcTextCtrl(parent)
{
	params = decompParams;
	colorsMenu = colorMenu;
	getDecompilationTextCtrl = getDecompTextCtrl;
	EnableLineNumbers();

	AddRightClickOption("Find function by address", 'G', 0, [&](wxCommandEvent&) {
		wxTextEntryDialog dlg(this, "", "Find address");
		if (dlg.ShowModal() == wxID_OK)
		{
			wxString txt = dlg.GetValue();
			unsigned long long address = 0;
			if (txt.ToULongLong(&address, 16))
			{
				int index = findFunctionByAddressInclusive(params, 0, params->numOfFunctions - 1, address);
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
	});

	AddRightClickOption("Decompile", 0, 0, [&](wxCommandEvent& e) {
		int selectedLine = GetCurrentLine();
		if (selectedLine >= 0 && selectedLine < params->numOfFunctions)
		{
			DecompilationTextCtrl* textCtrl = getDecompilationTextCtrl();
			if (textCtrl) 
			{
				textCtrl->DecompileFunction(selectedLine);
			}
		}
	});

	AddRightClickOption("View info", 0, 0, [&](wxCommandEvent& e) {
		int selectedLine = GetCurrentLine();
		if (selectedLine >= 0 && selectedLine < params->numOfFunctions)
		{
			new FunctionInfoMenu(this, GetPosition(), params->instructions, &params->functions[selectedLine]);
		}
	});

	//AddRightClickOption("Edit properties", 0, 0, [&](wxCommandEvent& e) {
	//	int selectedLine = GetCurrentLine();
	//	if (selectedLine >= 0 && selectedLine < functions.size())
	//	{
	//		new FunctionPropertiesMenu(this, GetPosition(), this, selectedLine);
	//	}
	//});
}

void FunctionsTextCtrl::ShowAllFunctions()
{
	ClearText();
	SetReadOnly(false);
	Freeze();

	wxString functionsStr = "";

	struct JdcStr functionHeaderBuffer = initializeJdcStr();
	for (int i = 0; i < params->numOfFunctions; i++)
	{
		struct Function* function = &params->functions[i];
		if (!generateFunctionHeader(function, &functionHeaderBuffer))
		{
			return;
		}

		sprintfJdc(&functionHeaderBuffer, 1, "; // address: 0x%llX; num of instructions: %d", params->instructions[function->firstInstructionIndex].address, function->lastInstructionIndex - function->firstInstructionIndex + 1);
		functionsStr += wxString(functionHeaderBuffer.buffer);

		if (i != params->numOfFunctions - 1)
		{
			functionsStr += "\n";
		}
	}

	SetText(functionsStr);
	ApplyFunctionsHighlighting();
	Thaw();
	SetReadOnly(true);
}

void FunctionsTextCtrl::ApplyFunctionsHighlighting()
{
	for (int i = 0; i < NUM_OF_DECOMP_COLORS; i++)
	{
		StyleSetForeground(i, colorsMenu->decompColors[i]);
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