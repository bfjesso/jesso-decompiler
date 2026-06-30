#include "jdcTextCtrl.h"

JdcTextCtrl::JdcTextCtrl(wxWindow* parent, wxString name) : wxStyledTextCtrl(parent, wxID_ANY)
{
	SetName(name);
	SetMinSize(wxSize(100, 100));

	Bind(wxEVT_CONTEXT_MENU, &JdcTextCtrl::RightClickOptions, this);
	Bind(wxEVT_CHAR_HOOK, &JdcTextCtrl::OnKeyDown, this);
	Bind(wxEVT_STC_UPDATEUI, &JdcTextCtrl::OnUpdateUI, this);

	SetReadOnly(true);
	SetMarginWidth(1, 0);
	StyleSetFont(wxSTC_STYLE_DEFAULT, codeFont);
	StyleSetForeground(wxSTC_STYLE_DEFAULT, darkerTextColor);
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

void JdcTextCtrl::AddLine(wxString txt)
{
	SetReadOnly(false);
	AppendText(txt + "\n");
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
	for (int i = 0; i < NUM_OF_INDICATORS; i++) 
	{
		SetIndicatorCurrent(i);
		IndicatorClearRange(0, GetTextLength());
	}
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

	//if (statusStaticText) 
	//{
	//	statusStaticText->SetLabelText("Status: Finding '" + text + "' (" + std::to_string(CountNumOfResults(text, pos, flags) + 1) + "/" + std::to_string(totalFindResults) + ")");
	//}
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

	//if (statusStaticText) 
	//{
	//	statusStaticText->SetLabelText("Status: idle");
	//}
}

char JdcTextCtrl::IsCharDigit(char c)
{
	return (c >= '0' && c <= '9') || (c >= 'A' && c <= 'F');
}

void JdcTextCtrl::AddDefaultRightClickOptions(wxMenu* menu)
{
	const int ID_COPY = 0;
	const int ID_SELECT_ALL = 1;
	const int ID_CONVERT_TO_UNSIGNED_DEC = 2;
	const int ID_CONVERT_TO_SIGNED_DEC = 3;
	const int ID_CONVERT_TO_UNSIGNED_HEX = 4;
	const int ID_CONVERT_TO_SIGNED_HEX = 5;
	const int ID_FIND = 6;
	const int ID_HIGHLIGHT_SELECTED_INSTRUCTIONS = 7;

	long start;
	long end;
	GetSelection(&start, &end);
	wxString selection = "";

	if (start != end)
	{
		wxString text = GetValue();
		selection = text.substr(start, end - start);

		menu->Append(ID_COPY, "Copy");
		menu->Bind(wxEVT_MENU, [&](wxCommandEvent&) { CopyToClipboard(selection); }, ID_COPY);

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
			else if (start < 2 || (text[start - 2] != '0' && text[start - 1] != 'x' && text[start - 1] != '-'))
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
				menu->Append(ID_CONVERT_TO_UNSIGNED_DEC, "Convert to unsigned decimal");
				menu->Bind(wxEVT_MENU, [&](wxCommandEvent&) {
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
				menu->Append(ID_CONVERT_TO_SIGNED_DEC, "Convert to signed decimal");
				menu->Bind(wxEVT_MENU, [&](wxCommandEvent&) {
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
				menu->Append(ID_CONVERT_TO_UNSIGNED_HEX, "Convert to unsigned hexadecimal");
				menu->Bind(wxEVT_MENU, [&](wxCommandEvent&) {
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
				menu->Append(ID_CONVERT_TO_SIGNED_HEX, "Convert to signed hexadecimal");
				menu->Bind(wxEVT_MENU, [&](wxCommandEvent&) {
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


	menu->Append(ID_SELECT_ALL, "Select all");
	menu->Bind(wxEVT_MENU, [&](wxCommandEvent&) {
		SetSelection(0, GetLastPosition());
		SetFocus();
	}, ID_SELECT_ALL);

	menu->Append(ID_FIND, "Find");
	menu->Bind(wxEVT_MENU, [&](wxCommandEvent&) {
		ShowFindDialog();
	}, ID_FIND);

	menu->AppendCheckItem(ID_HIGHLIGHT_SELECTED_INSTRUCTIONS, "Highlight selected lines");
	menu->Check(ID_HIGHLIGHT_SELECTED_INSTRUCTIONS, highlightSelectedLines);
	menu->Bind(wxEVT_MENU, [&](wxCommandEvent& e) {
		highlightSelectedLines = e.IsChecked();
		ClearIndicators();
	}, ID_HIGHLIGHT_SELECTED_INSTRUCTIONS);
}

void JdcTextCtrl::RightClickOptions(wxContextMenuEvent& e)
{
	wxMenu menu;
	AddDefaultRightClickOptions(&menu);
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
	}

	e.Skip();
}

void JdcTextCtrl::OnUpdateUI(wxStyledTextEvent& e)
{
	if (!HasFocus())
	{
		return;
	}

	ClearIndicators();

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
	
	if (highlightSelectedLines)
	{
		HighlightLine(GetCurrentLine(), YELLOW_INDICATOR, 0);
	}

	// highlighting other instances of selected text
	wxString selection = GetSelectedText();
	if (selection != "")
	{
		SetIndicatorCurrent(GRAY_INDICATOR);

		int selectionLen = selection.Length();
		int firstVisibleLine = GetFirstVisibleLine();
		int numOfVisibleLines = LinesOnScreen();

		int minPos = 0;
		if(firstVisibleLine > 0)
		{
			minPos = PositionFromLine(firstVisibleLine - 1);
		}
		
		int maxPos = GetTextLength();
		if (firstVisibleLine + numOfVisibleLines < GetNumberOfLines() + 1)
		{
			maxPos = PositionFromLine(firstVisibleLine + numOfVisibleLines + 1);
		}

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