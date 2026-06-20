#include "jdcTextCtrl.h"
#include "dataViewerMenu.h"

#include "../decompiler/intrinsics.h"
#include "../disassembler/disassembler.h"
#include "../decompiler/decompilationUtils.h"

JdcTextCtrl::JdcTextCtrl(wxWindow* parent, const wxSize& size, enum JdcTextCtrlType type, std::vector<DisassembledInstruction>* instructions, std::vector<Function>* functions, int* currentFunc) : wxStyledTextCtrl(parent, wxID_ANY, wxPoint(0, 0), size)
{
	ctrlType = type;
	instructionsRef = instructions;
	functionsRef = functions;
	currentDecompiledFuncRef = currentFunc;

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

	if (ctrlType == DECOMPILATION_CTRL_TYPE) 
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
		SetTabWidth(4);
		SetIndent(4);
		StyleSetForeground(wxSTC_STYLE_BRACELIGHT, textColor);
		StyleSetBackground(wxSTC_STYLE_BRACELIGHT, foregroundColor);
		StyleSetForeground(wxSTC_STYLE_BRACEBAD, wxColour(200, 80, 80));
	}
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

void JdcTextCtrl::AddAssociatedTextCtrl(JdcTextCtrl* textCtrl) 
{
	associatedTextCtrls.push_back(textCtrl);
}

void JdcTextCtrl::ShowGoToAddrDialog()
{
	wxTextEntryDialog dlg(this, "", "Go to address");
	if (dlg.ShowModal() == wxID_OK)
	{
		wxString txt = dlg.GetValue();
		unsigned long long address = 0;
		if (txt.ToULongLong(&address, 16))
		{
			if (ctrlType == DISASSEMBLY_CTRL_TYPE) 
			{
				int index = findInstructionByAddress(instructionsRef->data(), 0, instructionsRef->size() - 1, address);
				if (index == -1)
				{
					wxMessageBox("Address not found", "Failed to find address");
				}
				else
				{
					CenterLine(index);
				}
			}
			else if (ctrlType == DATA_CTRL_TYPE) 
			{
				DataViewer* dataViewer = (DataViewer*)(GetParent());
				FileSection* section = &dataViewer->sections[dataViewer->sectionChoice->GetSelection()];
				unsigned long long sectionStart = section->virtualAddress + dataViewer->imageBase;
				if (address >= sectionStart && address < sectionStart + section->size)
				{
					CenterLine((address - sectionStart) / dataViewer->bytesPerLine);
				}
				else
				{
					wxMessageBox("Address not in currently selected section", "Failed to find address");
				}
			}
		}
		else
		{
			wxMessageBox("Not valid hex number", "Failed to find address");
		}
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
	// statusStaticText->SetLabelText("Status: Finding '" + text + "' (" + std::to_string(CountNumOfResults(findCtrl, text, pos, flags) + 1) + "/" + std::to_string(totalFindResults) + ")");
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

	// statusStaticText->SetLabelText("Status: idle");
}

char JdcTextCtrl::IsCharDigit(char c)
{
	return (c >= '0' && c <= '9') || (c >= 'A' && c <= 'F');
}

void JdcTextCtrl::RightClickOptions(wxContextMenuEvent& e)
{
	wxMenu menu;

	const int ID_COPY = 100;
	const int ID_SELECT_ALL = 101;
	const int ID_CONVERT_NUMBER = 102;
	const int ID_FIND = 103;
	const int ID_GO_TO_ADDR = 104;
	const int ID_SHOW_ASSOCIATED = 105;
	const int ID_SHOW_BYTES = 106;
	const int ID_HIGHLIGHT_SELECTED_INSTRUCTIONS = 107;

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

		int numColor = ctrlType == DISASSEMBLY_CTRL_TYPE ? DisassemblyColor::CONSTANT_COLOR : DecompilationColor::NUMBER_COLOR;
		if (GetStyleAt(start) == numColor && !IsCharDigit(text[start - 1]) && !IsCharDigit(text[end]))
		{
			long long num = 0;
			unsigned long long unum = 0;
			if (selection.ToLongLong(&num, 10))
			{
				menu.Append(ID_CONVERT_NUMBER, "Convert to hexadecimal");
				menu.Bind(wxEVT_MENU, [&](wxCommandEvent&) {
					SetReadOnly(false);
					char numStr[50] = { 0 };
					sprintf(numStr, "0x%llX", num);
					Replace(start, end, numStr);
					StartStyling(start);
					SetStyling(strlen(numStr), numColor);
					SetReadOnly(true);
					}, ID_CONVERT_NUMBER);
			}
			else if (selection.ToLongLong(&num, 16))
			{
				menu.Append(ID_CONVERT_NUMBER, "Convert to decimal");
				menu.Bind(wxEVT_MENU, [&](wxCommandEvent&) {
					SetReadOnly(false);
					wxString numStr = std::to_string(num);
					Replace(start, end, numStr);
					StartStyling(start);
					SetStyling(strlen(numStr), numColor);
					SetReadOnly(true);
					}, ID_CONVERT_NUMBER);
			}
			else if (selection.ToULongLong(&unum, 10))
			{
				menu.Append(ID_CONVERT_NUMBER, "Convert to hexadecimal");
				menu.Bind(wxEVT_MENU, [&](wxCommandEvent&) {
					SetReadOnly(false);
					char numStr[50] = { 0 };
					sprintf(numStr, "0x%llX", unum);
					Replace(start, end, numStr);
					StartStyling(start);
					SetStyling(strlen(numStr), numColor);
					SetReadOnly(true);
					}, ID_CONVERT_NUMBER);
			}
			else if (selection.ToULongLong(&unum, 16))
			{
				menu.Append(ID_CONVERT_NUMBER, "Convert to decimal");
				menu.Bind(wxEVT_MENU, [&](wxCommandEvent&) {
					SetReadOnly(false);
					wxString numStr = std::to_string(unum);
					Replace(start, end, numStr);
					StartStyling(start);
					SetStyling(strlen(numStr), numColor);
					SetReadOnly(true);
					}, ID_CONVERT_NUMBER);
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

	if (ctrlType != DECOMPILATION_CTRL_TYPE) 
	{
		menu.Append(ID_GO_TO_ADDR, "Go to address");
		menu.Bind(wxEVT_MENU, [&](wxCommandEvent&) {
			ShowGoToAddrDialog();
			}, ID_GO_TO_ADDR);
	}

	if (ctrlType == DISASSEMBLY_CTRL_TYPE)
	{
		for (int i = 0; i < associatedTextCtrls.size(); i++) 
		{
			if (associatedTextCtrls[i]->ctrlType == DATA_CTRL_TYPE && associatedTextCtrls[i]->IsShown()) 
			{
				menu.AppendCheckItem(ID_SHOW_BYTES, "Show bytes in data viewer");
				menu.Check(ID_SHOW_BYTES, showBytesInDataViewer);
				menu.Bind(wxEVT_MENU, [&](wxCommandEvent& e) {
					showBytesInDataViewer = e.IsChecked();
					ClearIndicators();
					associatedTextCtrls[i]->ClearIndicators();
					}, ID_SHOW_BYTES);
			}
		}
	}

	menu.AppendCheckItem(ID_HIGHLIGHT_SELECTED_INSTRUCTIONS, "Highlight selected lines");
	menu.Check(ID_HIGHLIGHT_SELECTED_INSTRUCTIONS, highlightSelectedLines);
	menu.Bind(wxEVT_MENU, [&](wxCommandEvent& e) {
		highlightSelectedLines = e.IsChecked();
		ClearIndicators();
		}, ID_HIGHLIGHT_SELECTED_INSTRUCTIONS);

	if (ctrlType == DISASSEMBLY_CTRL_TYPE)
	{
		menu.AppendCheckItem(ID_SHOW_ASSOCIATED, "Show associated decompilation lines");
	}
	else
	{
		menu.AppendCheckItem(ID_SHOW_ASSOCIATED, "Show associated instructions");
	}

	menu.Check(ID_SHOW_ASSOCIATED, showAssociatedInstructions);
	menu.Bind(wxEVT_MENU, [&](wxCommandEvent& e) {
		showAssociatedInstructions = e.IsChecked();
		ClearIndicators();
		}, ID_SHOW_ASSOCIATED);

	PopupMenu(&menu, ScreenToClient(e.GetPosition()));
}

void JdcTextCtrl::OnKeyDown(wxKeyEvent& e)
{
	if ((e.GetModifiers() & wxMOD_CONTROL) != 0)
	{
		int key = e.GetKeyCode();
		if (key == 'F' || key == 'f')
		{
			ShowFindDialog();
		}
		if ((key == 'G' || key == 'g') && ctrlType != DECOMPILATION_CTRL_TYPE)
		{
			ShowGoToAddrDialog();
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
	int selectedLine = GetCurrentLine();

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

	// highlighting other instances of selected text
	if (ctrlType == DECOMPILATION_CTRL_TYPE) 
	{
		wxString selection = GetSelectedText();
		if (selection != "")
		{
			SetIndicatorCurrent(GRAY_INDICATOR);

			int selectionLen = selection.Length();
			int minPos = 0;
			while (1)
			{
				int index = FindText(minPos, GetTextLength(), selection);
				if (index == -1)
				{
					break;
				}

				IndicatorFillRange(index, selectionLen);
				minPos = index + selectionLen;
			}
		}
	}
	
	// associated highlighting
	unsigned char isAlreadyHighlighted = 0;
	for (int i = 0; i < associatedTextCtrls.size(); i++)
	{
		if (!associatedTextCtrls[i]) { continue; }
		associatedTextCtrls[i]->ClearIndicators();

		if (ctrlType == DISASSEMBLY_CTRL_TYPE)
		{
			if (showAssociatedInstructions && associatedTextCtrls[i]->ctrlType == DECOMPILATION_CTRL_TYPE)
			{
				if (associatedTextCtrls[i]->currentDecompiledFuncRef && *associatedTextCtrls[i]->currentDecompiledFuncRef != -1)
				{
					struct Function* function = &functionsRef->data()[*associatedTextCtrls[i]->currentDecompiledFuncRef];
					if (selectedLine >= function->firstInstructionIndex && selectedLine <= function->lastInstructionIndex)
					{
						for (int j = 0; j < function->numOfLines; j++)
						{
							struct AssociatedInstructions* a = &function->associatedInstructions[j];
							for (int k = 0; k < a->numOfIndexes; k++)
							{
								if (a->indexes[k] == selectedLine)
								{
									associatedTextCtrls[i]->HighlightLine(j, PURPLE_INDICATOR, 1);
									break;
								}
							}
						}

						HighlightLine(selectedLine, PURPLE_INDICATOR, 0);
						isAlreadyHighlighted = 1;
					}
				}
			}
			else if (showBytesInDataViewer && associatedTextCtrls[i]->ctrlType == DATA_CTRL_TYPE)
			{
				DataViewer* dataViewer = (DataViewer*)(associatedTextCtrls[i]->GetParent());
				if (dataViewer && dataViewer->IsShown()) 
				{
					dataViewer->HighlightInstruction(instructionsRef->data()[selectedLine].address, instructionsRef->data()[selectedLine].numOfBytes);
					HighlightLine(selectedLine, PURPLE_INDICATOR, 0);
					isAlreadyHighlighted = 1;
				}
			}
		}
		else if (ctrlType == DECOMPILATION_CTRL_TYPE) 
		{
			if (showAssociatedInstructions && currentDecompiledFuncRef && *currentDecompiledFuncRef != -1) 
			{
				struct Function* function = &functionsRef->data()[*currentDecompiledFuncRef];
				if (selectedLine < function->associatedInstructionsBufferLen) 
				{
					struct AssociatedInstructions* a = &function->associatedInstructions[selectedLine];

					for (int j = 0; j < a->numOfIndexes; j++)
					{
						associatedTextCtrls[i]->HighlightLine(a->indexes[j], PURPLE_INDICATOR, 1);
					}

					HighlightLine(selectedLine, PURPLE_INDICATOR, 0);
					isAlreadyHighlighted = 1;
				}
			}
		}
	}

	// highlighting selected line
	if (highlightSelectedLines && !isAlreadyHighlighted)
	{
		HighlightLine(selectedLine, YELLOW_INDICATOR, 0);
	}
}

void JdcTextCtrl::ApplySyntaxHighlighting(ImportedFunction* imports, int numOfImports)
{
	if (!currentDecompiledFuncRef || *currentDecompiledFuncRef == -1) 
	{
		return;
	}
	
	struct Function* function = &functionsRef->data()[*currentDecompiledFuncRef];
	
	wxString text = GetValue();

	StartStyling(0);
	SetStyling(text.length(), DecompilationColor::OPERATOR_COLOR);

	// stack vars
	for (int i = 0; i < function->numOfStackVars; i++)
	{
		ColorAllStrs(text, function->stackVars[i].name.buffer, DecompilationColor::LOCAL_VAR_COLOR, 1);
	}

	// reg vars
	for (int i = 0; i < function->numOfRegVars; i++)
	{
		ColorAllStrs(text, function->regVars[i].name.buffer, DecompilationColor::LOCAL_VAR_COLOR, 1);
	}

	// returned vars
	for (int i = 0; i < function->numOfReturnedVars; i++)
	{
		ColorAllStrs(text, function->returnedVars[i].name.buffer, DecompilationColor::LOCAL_VAR_COLOR, 1);
	}

	// stack args
	for (int i = 0; i < function->numOfStackArgs; i++)
	{
		ColorAllStrs(text, function->stackArgs[i].name.buffer, DecompilationColor::ARGUMENT_COLOR, 1);
	}

	// reg args
	for (int i = 0; i < function->numOfRegArgs; i++)
	{
		ColorAllStrs(text, function->regArgs[i].name.buffer, DecompilationColor::ARGUMENT_COLOR, 1);
	}

	// functions
	for (int i = 0; i < functionsRef->size(); i++)
	{
		ColorAllStrs(text, functionsRef->data()[i].name.buffer, DecompilationColor::FUNCTION_COLOR, 0);
	}

	// imports
	for (int i = 0; i < numOfImports; i++)
	{
		ColorAllStrs(text, imports[i].name.buffer, DecompilationColor::IMPORT_COLOR, 0);
	}

	// intrinsic functions
	for (int i = 0; i < NUM_OF_RETURNING_INTRINSICS; i++)
	{
		ColorAllStrs(text, returningIntrinsicFuncs[i].name, DecompilationColor::INTRINSIC_COLOR, 0);
	}
	for (int i = 0; i < NUM_OF_VOID_INTRINSICS; i++)
	{
		ColorAllStrs(text, voidIntrinsicFuncs[i].name, DecompilationColor::INTRINSIC_COLOR, 0);
	}

	// calling conventions
	for (int i = 0; i < NUM_OF_CALLING_CONVENTIONS; i++)
	{
		ColorAllStrs(text, callingConventionStrs[i], DecompilationColor::PRIMITIVE_COLOR, 0);
	}

	// primitive data types
	for (int i = 0; i < NUM_OF_PRIMITIVE_TYPES; i++)
	{
		ColorAllStrs(text, primitiveTypeStrs[i], DecompilationColor::PRIMITIVE_COLOR, 0);
	}
	ColorAllStrs(text, "unsigned", DecompilationColor::PRIMITIVE_COLOR, 0);
	ColorAllStrs(text, "sizeof", DecompilationColor::PRIMITIVE_COLOR, 0);

	// keywords
	const char* keywordStrs[11] = { "if", "else", "for", "while", "do", "break", "continue", "switch", "case", "goto", "return" };
	for (int i = 0; i < 11; i++)
	{
		ColorAllStrs(text, keywordStrs[i], DecompilationColor::KEYWORD_COLOR, 0);
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
			SetStyling(end - pos + 1, DecompilationColor::STRING_COLOR);

			start = end + 1;
		}
		else
		{
			break;
		}
	}

	// comments
	start = 0;
	while (start < text.length())
	{
		int pos = text.find("//", start);
		int end = text.find("\n", pos + 1);
		if (pos != wxNOT_FOUND && end != wxNOT_FOUND)
		{
			StartStyling(pos);
			SetStyling(end - pos + 1, DecompilationColor::COMMENT_DECOMP_COLOR);

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
			SetStyling(end - pos - 1, DecompilationColor::LABEL_COLOR);

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
		ColorAllStrs(text, registerStrs[i], DecompilationColor::ERROR_COLOR, 0);
	}
	for (int i = 0; i < NUM_OF_SEGMENTS; i++)
	{
		ColorAllStrs(text, segmentStrs[i], DecompilationColor::ERROR_COLOR, 0);
	}
	ColorAllStrs(text, "ERROR", DecompilationColor::ERROR_COLOR, 0);
	ColorAllStrs(text, "jumpTo", DecompilationColor::ERROR_COLOR, 0);

	// numbers
	const char* numberChars[17] = { "0x", "0", "1", "2", "3", "4", "5", "6", "7", "8", "9", "A", "B", "C", "D", "E", "F" };
	for (int i = 0; i < 17; i++)
	{
		ColorAllStrs(text, numberChars[i], DecompilationColor::NUMBER_COLOR, 0);
	}

	// this is for when :: is part of a function name
	ColorAllStrs(text, ":", DecompilationColor::OPERATOR_COLOR, 1);
}

void JdcTextCtrl::ApplyAsmHighlighting()
{
	int numOfInstructions = instructionsRef->size();
	int pos = 0;
	wxString disassemblyText = GetValue();
	for (int i = 0; i < numOfInstructions; i++)
	{
		struct DisassembledInstruction* instruction = &(instructionsRef->data()[i]);

		int tabPos = disassemblyText.find('\t', pos);
		wxString addressInfoStr = disassemblyText.substr(pos, tabPos - pos);
		wxString asmStr = disassemblyText.substr(tabPos + 1, disassemblyText.find('\n', tabPos) - (tabPos + 1));

		StartStyling(pos);
		SetStyling(addressInfoStr.length(), DisassemblyColor::ADDRESS_COLOR);

		pos += addressInfoStr.length() + 1;

		StartStyling(pos);
		SetStyling(asmStr.length(), DisassemblyColor::PUNCTUATION_COLOR);

		StartStyling(pos);

		int opcodeLen = strlen(mnemonicStrs[instruction->opcode]) + 1;
		if (instruction->group1Prefix != NO_PREFIX)
		{
			opcodeLen += strlen(getGroup1PrefixStr(instruction)) + 1;
		}
		SetStyling(opcodeLen, DisassemblyColor::OPCODE_COLOR);

		// operands
		int regStart = 0;
		int segStart = 0;
		int ptrSizeStart = 0;
		for (int i = 0; i < instruction->numOfOperands; i++)
		{
			if (instruction->operands[i].type == REGISTER)
			{
				wxString regStr = wxString(registerStrs[instruction->operands[i].reg]);
				int loc = asmStr.find(regStr, regStart);
				StartStyling(pos + loc);
				SetStyling(regStr.length(), DisassemblyColor::REGISTER_COLOR);
				regStart = loc + regStr.length();
			}
			else if (instruction->operands[i].type == MEM_ADDRESS)
			{
				if (instruction->operands[i].memoryAddress.reg != NO_REG)
				{
					wxString regStr = wxString(registerStrs[instruction->operands[i].memoryAddress.reg]);
					int loc = asmStr.find(regStr, regStart);
					StartStyling(pos + loc);
					SetStyling(regStr.length(), DisassemblyColor::REGISTER_COLOR);
					regStart = loc + regStr.length();
				}

				if (instruction->operands[i].memoryAddress.regDisplacement != NO_REG)
				{
					wxString regStr = wxString(registerStrs[instruction->operands[i].memoryAddress.regDisplacement]);
					int loc = asmStr.find(regStr, regStart);
					StartStyling(pos + loc);
					SetStyling(regStr.length(), DisassemblyColor::REGISTER_COLOR);
					regStart = loc + regStr.length();
				}

				if (instruction->operands[i].memoryAddress.segment != NO_SEGMENT)
				{
					wxString segStr = wxString(segmentStrs[instruction->operands[i].memoryAddress.segment]) + ":";
					int loc = asmStr.find(segStr, segStart);
					StartStyling(pos + loc);
					SetStyling(segStr.length() - 1, DisassemblyColor::SEGMENT_COLOR);
					segStart = loc + segStr.length();
				}

				int ptrSize = instruction->operands[i].memoryAddress.ptrSize;
				if (ptrSize != 0)
				{
					wxString sizeStr = wxString(getPtrSizeStr(ptrSize));
					int loc = asmStr.find(sizeStr, ptrSizeStart);
					StartStyling(pos + loc);
					SetStyling(sizeStr.length(), DisassemblyColor::PTR_SIZE_COLOR);
					ptrSizeStart = loc + sizeStr.length();
				}
			}
			else if (instruction->operands[i].type == SEGMENT)
			{
				wxString segStr = wxString(segmentStrs[instruction->operands[i].segment]);
				int loc = asmStr.find(segStr, segStart);
				StartStyling(pos + loc);
				SetStyling(segStr.length(), DisassemblyColor::SEGMENT_COLOR);
				segStart = loc + segStr.length();
			}
		}

		// numbers
		int start = 0;
		while (start < asmStr.length())
		{
			int num = asmStr.find("0x", start);
			if (num != wxNOT_FOUND)
			{
				int end = asmStr.length();
				for (int i = num + 2; i < end; i++)
				{
					if ((asmStr[i] < '0' || asmStr[i] > '9') && (asmStr[i] < 'A' || asmStr[i] > 'F'))
					{
						end = i;
						break;
					}
				}

				StartStyling(pos + num);
				SetStyling(end - num, DisassemblyColor::CONSTANT_COLOR);

				start = end + 1;
			}
			else
			{
				break;
			}
		}

		// comments
		int commentStart = asmStr.find(";", 0);
		if (commentStart != wxNOT_FOUND)
		{
			StartStyling(pos + commentStart);
			SetStyling(asmStr.length() - commentStart, DisassemblyColor::COMMENT_DIS_COLOR);
		}

		pos += asmStr.size();
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
				GetStyleAt(pos) == DecompilationColor::OPERATOR_COLOR) // only apply color if it hasn't been colored yet
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