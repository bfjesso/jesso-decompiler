#include "decompilationWindow.h"

#include "../decompiler/decompiler.h"
#include "../decompiler/intrinsics.h"

DecompilationWindow::DecompilationWindow(wxWindow* parent, const wxSize& size, ColorsMenu* colorMenu, wxStaticText* statusText) : JdcTextCtrl(parent, size, statusText)
{
	colorsMenu = colorMenu;
	EnableLineNumbers();
}

void DecompilationWindow::SetAssociatedDisassemblyWindow(DisassemblyWindow* window)
{
	disassemblyWindow = window;
}

void DecompilationWindow::DecompileFunction(struct DecompilationParameters* params, int functionIndex)
{
	if (params->numOfFileBytes == 0)
	{
		wxMessageBox("No file opened", "Can't decompile");
		return;
	}

	ClearText();

	ClearText();
	ClearIndicators();

	if (disassemblyWindow) 
	{
		disassemblyWindow->ClearIndicators();
	}

	params->currentFunc = &params->functions[functionIndex];

	struct JdcStr decompilationResult = initializeJdcStr();
	if (decompilationResult.bufferSize == 0)
	{
		wxMessageBox("Error allocating memory for function decompilation", "Can't decompile");
		return;
	}

	struct JdcStr statusMessage = initializeJdcStr();
	int errorInstructionIndex = 0;
	if (!decompileFunction(params, &decompilationResult, &statusMessage, &errorInstructionIndex))
	{
		if (disassemblyWindow) 
		{
			disassemblyWindow->HighlightLine(errorInstructionIndex, RED_INDICATOR, 1);
		}

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
	ApplyDecompilationHighlighting(params);
	SetReadOnly(true);
}

void DecompilationWindow::ApplyDecompilationHighlighting(struct DecompilationParameters* params)
{
	if (!params || !params->currentFunc)
	{
		return;
	}

	highlightingType = DECOMPILATION_HIGHLIGHTING;
	for (int i = 0; i < NUM_OF_DECOMP_COLORS; i++)
	{
		StyleSetForeground(i, colorsMenu->decompColors[i]);
	}

	wxString text = GetValue();

	StartStyling(0);
	SetStyling(text.length(), OPERATOR_DECOMP_COLOR);

	// stack vars
	for (int i = 0; i < params->currentFunc->numOfStackVars; i++)
	{
		ColorAllStrs(text, params->currentFunc->stackVars[i].name.buffer, LOCAL_VAR_DECOMP_COLOR, 1);
	}

	// reg vars
	for (int i = 0; i < params->currentFunc->numOfRegVars; i++)
	{
		ColorAllStrs(text, params->currentFunc->regVars[i].name.buffer, LOCAL_VAR_DECOMP_COLOR, 1);
	}

	// returned vars
	for (int i = 0; i < params->currentFunc->numOfReturnedVars; i++)
	{
		ColorAllStrs(text, params->currentFunc->returnedVars[i].name.buffer, LOCAL_VAR_DECOMP_COLOR, 1);
	}

	// stack args
	for (int i = 0; i < params->currentFunc->numOfStackArgs; i++)
	{
		ColorAllStrs(text, params->currentFunc->stackArgs[i].name.buffer, ARGUMENT_DECOMP_COLOR, 1);
	}

	// reg args
	for (int i = 0; i < params->currentFunc->numOfRegArgs; i++)
	{
		ColorAllStrs(text, params->currentFunc->regArgs[i].name.buffer, ARGUMENT_DECOMP_COLOR, 1);
	}

	// imports
	for (int i = 0; i < params->numOfImports; i++)
	{
		ColorAllStrs(text, params->imports[i].name.buffer, IMPORT_DECOMP_COLOR, 0);
	}

	// intrinsic functions
	for (int i = 0; i < NUM_OF_RETURNING_INTRINSICS; i++)
	{
		ColorAllStrs(text, returningIntrinsicFuncs[i].name, INTRINSIC_DECOMP_COLOR, 0);
	}
	for (int i = 0; i < NUM_OF_VOID_INTRINSICS; i++)
	{
		ColorAllStrs(text, voidIntrinsicFuncs[i].name, INTRINSIC_DECOMP_COLOR, 0);
	}

	// keywords
	const char* keywordStrs[11] = { "if", "else", "for", "while", "do", "break", "continue", "switch", "case", "goto", "return" };
	for (int i = 0; i < 11; i++)
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
	for (int i = 0; i < params->numOfFunctions; i++)
	{
		ColorAllStrs(text, params->functions[i].name.buffer, FUNCTION_DECOMP_COLOR, 0);
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