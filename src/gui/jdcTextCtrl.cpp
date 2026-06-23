#include "jdcTextCtrl.h"

#include "../decompiler/intrinsics.h"

JdcTextCtrl::JdcTextCtrl(wxWindow* parent, const wxSize& size, wxStaticText* statusText) : wxStyledTextCtrl(parent, wxID_ANY, wxPoint(0, 0), size)
{
	statusStaticText = statusText;
	
	Bind(wxEVT_CONTEXT_MENU, [&](wxContextMenuEvent& e) -> void { RightClickOptions(e); });
	Bind(wxEVT_CHAR_HOOK, &JdcTextCtrl::OnKeyDown, this);
	Bind(wxEVT_STC_UPDATEUI, &JdcTextCtrl::OnUpdateUI, this);

	SetReadOnly(true);
	SetMarginWidth(1, 0);
	StyleSetFont(wxSTC_STYLE_DEFAULT, codeFont);
	StyleSetBackground(wxSTC_STYLE_DEFAULT, gridColor);
	StyleClearAll();
	SetSelBackground(true, wxSystemSettings::GetColour(wxSYS_COLOUR_HIGHLIGHT));
	SetSelForeground(true, wxSystemSettings::GetColour(wxSYS_COLOUR_HIGHLIGHTTEXT));
	SetCaretForeground(textColor);
	SetCaretWidth(2);
	SetWrapMode(wxSTC_WRAP_NONE);
	SetScrollWidthTracking(true);
	SetScrollWidth(1);
	SetViewWhiteSpace(wxSTC_WS_INVISIBLE);

	Bind(wxEVT_SET_CURSOR, [](wxSetCursorEvent& e) {
		e.SetCursor(wxCursor(wxCURSOR_ARROW));
	});

	IndicatorSetStyle(PURPLE_INDICATOR, wxSTC_INDIC_ROUNDBOX);
	IndicatorSetForeground(PURPLE_INDICATOR, wxColour(255, 0, 255));
	IndicatorSetAlpha(PURPLE_INDICATOR, 40);

	IndicatorSetStyle(GRAY_INDICATOR, wxSTC_INDIC_ROUNDBOX);
	IndicatorSetForeground(GRAY_INDICATOR, wxColour(200, 200, 200));
	IndicatorSetAlpha(GRAY_INDICATOR, 40);

	IndicatorSetStyle(YELLOW_INDICATOR, wxSTC_INDIC_ROUNDBOX);
	IndicatorSetForeground(YELLOW_INDICATOR, wxColour(255, 200, 0));
	IndicatorSetAlpha(YELLOW_INDICATOR, 40);

	IndicatorSetStyle(RED_INDICATOR, wxSTC_INDIC_ROUNDBOX);
	IndicatorSetForeground(RED_INDICATOR, wxColour(255, 0, 0));
	IndicatorSetAlpha(RED_INDICATOR, 40);

	StyleSetForeground(wxSTC_STYLE_BRACELIGHT, textColor);
	StyleSetBackground(wxSTC_STYLE_BRACELIGHT, foregroundColor);
	StyleSetForeground(wxSTC_STYLE_BRACEBAD, wxColour(200, 80, 80));

	SetTabWidth(4);
	SetIndent(4);
}

void JdcTextCtrl::EnableLineNumbers()
{
	SetMarginType(0, wxSTC_MARGIN_NUMBER);
	SetMarginWidth(0, TextWidth(wxSTC_STYLE_LINENUMBER, "99999"));
	StyleSetForeground(wxSTC_STYLE_LINENUMBER, darkerTextColor);
	StyleSetBackground(wxSTC_STYLE_LINENUMBER, gridColor);
	SetMarginBackground(1, backgroundColor);
	SetIndentationGuides(wxSTC_IV_LOOKBOTH);
	StyleSetForeground(wxSTC_STYLE_INDENTGUIDE, darkerTextColor);
	SetEdgeMode(wxSTC_EDGE_LINE);
	SetEdgeColumn(0);
	SetEdgeColour(darkerTextColor);
}

void JdcTextCtrl::ClearText()
{
	SetReadOnly(false);
	SetText("");
	SetReadOnly(true);
}

void JdcTextCtrl::CenterLine(int line)
{
	GotoLine(line);
	LineScroll(0, line - GetFirstVisibleLine() - (LinesOnScreen() / 2));
}

void JdcTextCtrl::HighlightLine(int line, enum IndicatorColor color, unsigned char gotoLine)
{
	if (gotoLine)
	{
		CenterLine(line);
	}

	SetIndicatorCurrent(color);
	int start = PositionFromLine(line);
	int len = GetLineLength(line);
	IndicatorFillRange(start, len);
}

void JdcTextCtrl::ClearIndicators()
{
	IndicatorClearRange(0, GetTextLength());
}

void JdcTextCtrl::ShowFindDialog()
{
	lastFindText = "";

	if (!findDialog)
	{
		findDialog = new wxFindReplaceDialog(this, &findData, "Find");
		findDialog->Bind(wxEVT_FIND, &JdcTextCtrl::OnFindDialog, this);
		findDialog->Bind(wxEVT_FIND_NEXT, &JdcTextCtrl::OnFindDialog, this);
		findDialog->Bind(wxEVT_FIND_CLOSE, &JdcTextCtrl::OnFindDialogClose, this);
	}

	findDialog->Show();
	findDialog->Raise();
}


void JdcTextCtrl::OnFindDialog(wxFindDialogEvent& e)
{
	wxString text = e.GetFindString();
	if (text.IsEmpty())
	{
		return;
	}

	int flags = 0;
	if (e.GetFlags() & wxFR_MATCHCASE)
	{
		flags |= wxSTC_FIND_MATCHCASE;
	}
	if (e.GetFlags() & wxFR_WHOLEWORD)
	{
		flags |= wxSTC_FIND_WHOLEWORD;
	}

	if (!text.IsSameAs(lastFindText))
	{
		totalFindResults = CountNumOfResults(text, GetLength(), flags);

		if (totalFindResults == 0)
		{
			wxMessageBox("Text not found", "Failed to find text");
			return;
		}

		lastFindText = text;
	}

	long selStart = GetSelectionStart();
	long selEnd = GetSelectionEnd();
	unsigned char forward = (e.GetFlags() & wxFR_DOWN) != 0;

	int start = forward ? selEnd : 0;
	int end = forward ? GetLength() : selStart;

	int pos = FindInRange(text, start, end, flags, forward);
	if (pos == -1)
	{
		int wrapStart = forward ? 0 : selEnd;
		int wrapEnd = forward ? selStart : GetLength();
		pos = FindInRange(text, wrapStart, wrapEnd, flags, forward);
	}

	GotoPos(pos);
	SetSelection(pos, pos + text.size());

	if (statusStaticText) 
	{
		statusStaticText->SetLabelText("Status: Finding '" + text + "' (" + std::to_string(CountNumOfResults(text, pos, flags) + 1) + "/" + std::to_string(totalFindResults) + ")");
	}
}

int JdcTextCtrl::FindInRange(const wxString& text, int start, int end, int flags, unsigned char forward)
{
	if (forward)
	{
		return FindText(start, end, text, flags);
	}

	int lastPos = -1;
	int pos = FindText(start, end, text, flags);
	while (pos != -1)
	{
		lastPos = pos;
		pos = FindText(pos + 1, end, text, flags);
	}

	return lastPos;
}

int JdcTextCtrl::CountNumOfResults(const wxString& text, int end, int flags)
{
	int result = 0;

	int start = 0;
	int pos = FindText(start, end, text, flags);
	while (pos != -1)
	{
		result++;
		start = pos + 1;
		pos = FindText(pos + 1, end, text, flags);
	}

	return result;
}

void JdcTextCtrl::OnFindDialogClose(wxFindDialogEvent& e)
{
	if (findDialog)
	{
		findDialog->Destroy();
		findDialog = nullptr;
	}

	if (statusStaticText) 
	{
		statusStaticText->SetLabelText("Status: idle");
	}
}

char JdcTextCtrl::IsCharDigit(char c)
{
	return (c >= '0' && c <= '9') || (c >= 'A' && c <= 'F');
}

void JdcTextCtrl::AddRightClickOption(wxString name, char commandKey, unsigned char* check, const std::function<void(wxCommandEvent&)>& function)
{
	struct RightClickOption option = { name, commandKey, check, function };
	additionalRightClickOptions.push_back(option);
}

void JdcTextCtrl::RightClickOptions(wxContextMenuEvent& e)
{
	wxMenu menu;

	const int ID_COPY = 100;
	const int ID_SELECT_ALL = 101;
	const int ID_CONVERT_TO_UNSIGNED_DEC = 102;
	const int ID_CONVERT_TO_SIGNED_DEC = 103;
	const int ID_CONVERT_TO_UNSIGNED_HEX = 104;
	const int ID_CONVERT_TO_SIGNED_HEX = 105;
	const int ID_FIND = 106;
	const int ID_GO_TO_ADDR = 107;
	const int ID_SHOW_ASSOCIATED = 108;
	const int ID_SHOW_BYTES = 109;
	const int ID_HIGHLIGHT_SELECTED_INSTRUCTIONS = 110;

	long start;
	long end;
	GetSelection(&start, &end);
	wxString selection = "";

	if (start != end)
	{
		wxString text = GetValue();
		selection = text.substr(start, end - start);

		menu.Append(ID_COPY, "Copy");
		menu.Bind(wxEVT_MENU, [&](wxCommandEvent&) { CopyToClipboard(selection); }, ID_COPY);

		int numColor = GetStyleAt(start);
		if ((start == 0 || (!IsCharDigit(text[start - 1]) && text[start - 1] != '-')) && !IsCharDigit(text[end]))
		{
			long long num = 0;
			unsigned char isHex = 0;
			unsigned char isDec = 0;
			unsigned char isSigned = 0; // its possible for both isSigned and isUnsigned to be 1
			unsigned char isUnsigned = 0;

			if (selection.substr(0, 2) == "0x" || selection.substr(0, 3) == "-0x")
			{
				isSigned = selection.ToLongLong(&num, 16);
				isUnsigned = selection.ToULongLong((unsigned long long*)(&num), 16);
				isHex = isSigned || isUnsigned;
			}
			else if(start < 2 || (text[start - 2] != '0' && text[start - 1] != 'x' && text[start - 1] != '-'))
			{
				isSigned = selection.ToLongLong(&num, 10);
				isUnsigned = selection.ToULongLong((unsigned long long*)(&num), 10);
				isDec = isSigned || isUnsigned;
			}

			if (text[start] == '-')
			{
				isUnsigned = 0;
			}

			if (isHex || (!isUnsigned && isDec))
			{
				menu.Append(ID_CONVERT_TO_UNSIGNED_DEC, "Convert to unsigned decimal");
				menu.Bind(wxEVT_MENU, [&](wxCommandEvent&) {
					SetReadOnly(false);
					wxString numStr = std::to_string((unsigned long long)num);
					Replace(start, end, numStr);
					StartStyling(start);
					SetStyling(strlen(numStr), numColor);
					SetReadOnly(true);
				}, ID_CONVERT_TO_UNSIGNED_DEC);
			}

			if (isHex || (!isSigned && isDec))
			{
				menu.Append(ID_CONVERT_TO_SIGNED_DEC, "Convert to signed decimal");
				menu.Bind(wxEVT_MENU, [&](wxCommandEvent&) {
					SetReadOnly(false);
					wxString numStr = std::to_string(num);
					Replace(start, end, numStr);
					StartStyling(start);
					SetStyling(strlen(numStr), numColor);
					SetReadOnly(true);
				}, ID_CONVERT_TO_SIGNED_DEC);
			}

			if (isDec || (!isUnsigned && isHex))
			{
				menu.Append(ID_CONVERT_TO_UNSIGNED_HEX, "Convert to unsigned hexadecimal");
				menu.Bind(wxEVT_MENU, [&](wxCommandEvent&) {
					SetReadOnly(false);
					char numStr[50] = { 0 };
					sprintf(numStr, "0x%llX", (unsigned long long)num);
					Replace(start, end, numStr);
					StartStyling(start);
					SetStyling(strlen(numStr), numColor);
					SetReadOnly(true);
				}, ID_CONVERT_TO_UNSIGNED_HEX);
			}

			if (isDec || (!isSigned && isHex))
			{
				menu.Append(ID_CONVERT_TO_SIGNED_HEX, "Convert to signed hexadecimal");
				menu.Bind(wxEVT_MENU, [&](wxCommandEvent&) {
					SetReadOnly(false);
					char numStr[50] = { 0 };
					if (num < 0)
					{
						sprintf(numStr, "-0x%llX", -num);
					}
					else
					{
						sprintf(numStr, "0x%llX", num);
					}

					Replace(start, end, numStr);
					StartStyling(start);
					SetStyling(strlen(numStr), numColor);
					SetReadOnly(true);
				}, ID_CONVERT_TO_SIGNED_HEX);
			}
		}
	}


	menu.Append(ID_SELECT_ALL, "Select all");
	menu.Bind(wxEVT_MENU, [&](wxCommandEvent&) {
		SetSelection(0, GetLastPosition());
		SetFocus();
		}, ID_SELECT_ALL);

	menu.Append(ID_FIND, "Find");
	menu.Bind(wxEVT_MENU, [&](wxCommandEvent&) {
		ShowFindDialog();
		}, ID_FIND);

	menu.AppendCheckItem(ID_HIGHLIGHT_SELECTED_INSTRUCTIONS, "Highlight selected lines");
	menu.Check(ID_HIGHLIGHT_SELECTED_INSTRUCTIONS, highlightSelectedLines);
	menu.Bind(wxEVT_MENU, [&](wxCommandEvent& e) {
		highlightSelectedLines = e.IsChecked();
		ClearIndicators();
		}, ID_HIGHLIGHT_SELECTED_INSTRUCTIONS);

	for (int i = 0; i < additionalRightClickOptions.size() && i < 100; i++)
	{
		if (additionalRightClickOptions[i].check != 0)
		{
			menu.AppendCheckItem(i, additionalRightClickOptions[i].name);
			menu.Check(i, *additionalRightClickOptions[i].check);
			menu.Bind(wxEVT_MENU, additionalRightClickOptions[i].function, i);
		}
		else
		{
			menu.Append(i, additionalRightClickOptions[i].name);
			menu.Bind(wxEVT_MENU, additionalRightClickOptions[i].function, i);
		}
	}

	PopupMenu(&menu, ScreenToClient(e.GetPosition()));
}

void JdcTextCtrl::OnKeyDown(wxKeyEvent& e)
{
	int key = e.GetKeyCode();
	if ((e.GetModifiers() & wxMOD_CONTROL) != 0 && key != 0)
	{
		if (key == 'F')
		{
			ShowFindDialog();
		}

		for (int i = 0; i < additionalRightClickOptions.size(); i++)
		{
			if (key == additionalRightClickOptions[i].commandKey) 
			{
				wxCommandEvent tmp;
				additionalRightClickOptions[i].function(tmp);
			}
		}
	}

	e.Skip();
}

void JdcTextCtrl::SetAdditionalOnUpdateUI(const std::function<void()>& function)
{
	additionalOnUpdateUI = function;
}

void JdcTextCtrl::OnUpdateUI(wxStyledTextEvent& e)
{
	if (!HasFocus())
	{
		return;
	}

	ClearIndicators();
	SetIndicatorCurrent(YELLOW_INDICATOR); // used later to check if the line is already highlighted

	if (additionalOnUpdateUI) 
	{
		additionalOnUpdateUI();
	}

	// brace highlighting
	int pos = GetCurrentPos();
	int bracePos1 = -1;
	int bracePos2 = -1;
	int ch = GetCharAt(pos);
	if (ch == '{' || ch == '}' || ch == '(' || ch == ')' || ch == '[' || ch == ']')
	{
		bracePos1 = pos;
	}

	if (bracePos1 != -1)
	{
		bracePos2 = BraceMatch(bracePos1);
	}

	if (bracePos1 != -1 && bracePos2 != -1)
	{
		BraceHighlight(bracePos1, bracePos2);
	}
	else if (bracePos1 != -1)
	{
		BraceBadLight(bracePos1);
	}
	else
	{
		BraceHighlight(-1, -1);
	}
	
	if (highlightSelectedLines && GetIndicatorCurrent() == YELLOW_INDICATOR)
	{
		HighlightLine(GetCurrentLine(), YELLOW_INDICATOR, 0);
	}

	// highlighting other instances of selected text
	wxString selection = GetSelectedText();
	if (selection != "")
	{
		SetIndicatorCurrent(GRAY_INDICATOR);

		int selectionLen = selection.Length();

		int firstLine = GetFirstVisibleLine();
		int minPos = PositionFromLine(firstLine);
		int maxPos = PositionFromLine(firstLine + LinesOnScreen());
		while (1)
		{
			int index = FindText(minPos, maxPos, selection);
			if (index == -1)
			{
				break;
			}

			IndicatorFillRange(index, selectionLen);
			minPos = index + selectionLen;
		}
	}
}

void JdcTextCtrl::ApplyDecompilationHighlighting(struct DecompilationParameters* params, wxColour* decompColors)
{
	if (!params || !params->currentFunc)
	{
		return;
	}

	highlightingType = DECOMPILATION_HIGHLIGHTING;
	for (int i = 0; i < NUM_OF_DECOMP_COLORS; i++)
	{
		StyleSetForeground(i, decompColors[i]);
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

void JdcTextCtrl::ApplyFunctionsHighlighting(wxColour* decompColors)
{
	highlightingType = DECOMPILATION_HIGHLIGHTING;
	for (int i = 0; i < NUM_OF_DECOMP_COLORS; i++)
	{
		StyleSetForeground(i, decompColors[i]);
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

void JdcTextCtrl::ApplyDataHighlighting(wxColour* dataColors)
{
	highlightingType = DATA_HIGHLIGHTING;
	for (int i = 0; i < NUM_OF_DATA_COLORS; i++)
	{
		StyleSetForeground(i, dataColors[i]);
	}

	StartStyling(0);

	wxString dataText = GetValue();
	SetStyling(dataText.length(), ADDRESS_DATA_COLOR);

	int start = 0;
	while (start < dataText.length())
	{
		int pos = dataText.find("\t", start);
		int end = dataText.find("\n", pos);
		if (pos != wxNOT_FOUND && end != wxNOT_FOUND)
		{
			StartStyling(pos);
			SetStyling(end - pos + 1, VALUE_DATA_COLOR);

			start = end + 1;
		}
		else
		{
			break;
		}
	}
}

void JdcTextCtrl::ColorAllStrs(wxString text, const char* str, DecompilationColor color, unsigned char forceColor)
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