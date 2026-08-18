#include "disassemblyTextCtrl.h"
#include "mainGui.h"

#include "../disassembler/disassembler.h"
#include "../decompiler/functions.h"
#include "../decompiler/decompilationUtils.h"

DisassemblyTextCtrl::DisassemblyTextCtrl(wxWindow* parent, MainGui* mainGuiRef, wxString name, struct DisassembledInstruction* disassembledInstructions, int amountOfInstructions) : JdcTextCtrl(parent, mainGuiRef, name)
{
	Bind(wxEVT_CONTEXT_MENU, &DisassemblyTextCtrl::DisassemblyRightClickOptions, this);
	Bind(wxEVT_CHAR_HOOK, &DisassemblyTextCtrl::OnDisassemblyKeyDown, this);
	Bind(wxEVT_STC_UPDATEUI, &DisassemblyTextCtrl::OnUpdateDisassemblyUI, this);

	Initialize(disassembledInstructions, amountOfInstructions, 0);
}

void DisassemblyTextCtrl::ClearData()
{
	instructions = 0;
	numOfInstructions = 0;

	ClearText();
}

void DisassemblyTextCtrl::Initialize(struct DisassembledInstruction* disassembledInstructions, int amountOfInstructions, unsigned long long errorAddress)
{
	instructions = disassembledInstructions;
	numOfInstructions = amountOfInstructions;

	if (!instructions || numOfInstructions == 0) 
	{
		return;
	}

	ClearText();

	wxString newLines = "";
	for (int i = 0; i < numOfInstructions; i++)
	{
		newLines += "\n";
	}

	SetReadOnly(false);
	SetText(newLines);
	SetReadOnly(true);

	if (errorAddress != 0)
	{
		int errorIndex = findInstructionByAddress(instructions, numOfInstructions, errorAddress);
		CenterLine(errorIndex);
		UpdateTextCtrl();
		HighlightLine(errorIndex, RED_INDICATOR, 0);
	}
	else
	{
		int entryPointIndex = findInstructionByAddress(instructions, numOfInstructions, mainGui->entryPoint + mainGui->decompParams.imageBase);
		if (entryPointIndex != -1)
		{
			CenterLine(entryPointIndex);
			UpdateTextCtrl();
			HighlightLine(entryPointIndex, YELLOW_INDICATOR, 0);
		}
		else
		{
			CenterLine(0);
			UpdateTextCtrl();
			HighlightLine(0, YELLOW_INDICATOR, 0);
		}
	}
}

void DisassemblyTextCtrl::ShowGoToAddressDialog() 
{
	wxTextEntryDialog dlg(this, "", "Go to address");
	if (dlg.ShowModal() == wxID_OK)
	{
		if (numOfInstructions == 0)
		{
			wxMessageBox("No instructions loaded", "Failed to find address");
			return;
		}

		wxString txt = dlg.GetValue();
		unsigned long long address = 0;
		if (txt.ToULongLong(&address, 16))
		{
			int index = findInstructionByAddressInclusive(instructions, numOfInstructions, address);
			if (index == -1)
			{
				wxMessageBox("Address not found", "Failed to find address");
				return;
			}

			CenterLine(index);

			if (address != instructions[index].address) 
			{
				wxMessageBox("Exact address not found", "Failed to find address");
			}
			
			return;
		}

		wxMessageBox("Not valid hex number", "Failed to find address");
	}
}

void DisassemblyTextCtrl::HighlightLine(int line, enum IndicatorColor color, unsigned char gotoLine)
{
	if (gotoLine) 
	{
		CenterLine(line);
		UpdateTextCtrl();
	}

	return JdcTextCtrl::HighlightLine(line, color, 0);
}

void DisassemblyTextCtrl::DisassemblyRightClickOptions(wxContextMenuEvent& e)
{
	wxMenu menu;

	AddDefaultRightClickOptions(&menu);

	const int ID_GO_TO_ADDRESS = 100;
	const int ID_SET_ASSOCIATED_DECOMPILATION = 101;
	const int ID_UNASSOCIATE_DECOMPILATION = 102;
	const int ID_SET_ASSOCIATED_FUNCTIONS = 103;
	const int ID_UNASSOCIATE_FUNCTIONS = 104;
	const int ID_SET_ASSOCIATED_DATA = 105;
	const int ID_UNASSOCIATE_DATA = 106;
	const int ID_SHOW_UNHANDLED_OPCODES = 107;

	menu.Append(ID_GO_TO_ADDRESS, "Go to address");
	menu.Bind(wxEVT_MENU, [&](wxCommandEvent&) {
		ShowGoToAddressDialog();
	}, ID_GO_TO_ADDRESS);

	menu.Append(ID_SET_ASSOCIATED_DECOMPILATION, "Set associated decompilation");
	menu.Bind(wxEVT_MENU, [&](wxCommandEvent&) {
		wxArrayString windowCaptions;
		for (int i = 0; i < mainGui->decompilationTextCtrls.size(); i++)
		{
			windowCaptions.push_back(mainGui->decompilationTextCtrls[i]->GetName());
		}
		windowCaptions.push_back("New window");
		wxSingleChoiceDialog choiceDialog(this, "", "Choose a window", windowCaptions);
		if (choiceDialog.ShowModal() != wxID_CANCEL)
		{
			int selection = choiceDialog.GetSelection();
			if (selection == mainGui->decompilationTextCtrls.size())
			{
				decompilationTextCtrl = mainGui->AddDecompilationTextCtrl();
			}
			else
			{
				decompilationTextCtrl = mainGui->decompilationTextCtrls[selection];
			}
		}
	}, ID_SET_ASSOCIATED_DECOMPILATION);

	if (decompilationTextCtrl)
	{
		menu.Append(ID_UNASSOCIATE_DECOMPILATION, "Unassociate " + decompilationTextCtrl->GetName());
		menu.Bind(wxEVT_MENU, [&](wxCommandEvent&) {
			decompilationTextCtrl->ClearIndicators();
			decompilationTextCtrl = nullptr;
		}, ID_UNASSOCIATE_DECOMPILATION);
	}

	menu.Append(ID_SET_ASSOCIATED_FUNCTIONS, "Set associated functions");
	menu.Bind(wxEVT_MENU, [&](wxCommandEvent&) {
		wxArrayString windowCaptions;
		for (int i = 0; i < mainGui->functionsTextCtrls.size(); i++)
		{
			windowCaptions.push_back(mainGui->functionsTextCtrls[i]->GetName());
		}
		windowCaptions.push_back("New window");
		wxSingleChoiceDialog choiceDialog(this, "", "Choose a window", windowCaptions);
		if (choiceDialog.ShowModal() != wxID_CANCEL)
		{
			int selection = choiceDialog.GetSelection();
			if (selection == mainGui->functionsTextCtrls.size())
			{
				functionsTextCtrl = mainGui->AddFunctionsTextCtrl();
			}
			else
			{
				functionsTextCtrl = mainGui->functionsTextCtrls[selection];
			}
		}
	}, ID_SET_ASSOCIATED_FUNCTIONS);

	if (functionsTextCtrl)
	{
		menu.Append(ID_UNASSOCIATE_FUNCTIONS, "Unassociate " + functionsTextCtrl->GetName());
		menu.Bind(wxEVT_MENU, [&](wxCommandEvent&) {
			functionsTextCtrl->ClearIndicators();
			functionsTextCtrl = nullptr;
		}, ID_UNASSOCIATE_FUNCTIONS);
	}

	menu.Append(ID_SET_ASSOCIATED_DATA, "Set associated data");
	menu.Bind(wxEVT_MENU, [&](wxCommandEvent&) {
		wxArrayString windowCaptions;
		for (int i = 0; i < mainGui->dataTextCtrls.size(); i++)
		{
			windowCaptions.push_back(mainGui->dataTextCtrls[i]->GetName());
		}
		windowCaptions.push_back("New window");
		wxSingleChoiceDialog choiceDialog(this, "", "Choose a window", windowCaptions);
		if (choiceDialog.ShowModal() != wxID_CANCEL)
		{
			int selection = choiceDialog.GetSelection();
			if (selection == mainGui->dataTextCtrls.size())
			{
				dataTextCtrl = mainGui->AddDataTextCtrl();
			}
			else
			{
				dataTextCtrl = mainGui->dataTextCtrls[selection];
			}
		}
	}, ID_SET_ASSOCIATED_DATA);

	if (dataTextCtrl)
	{
		menu.Append(ID_UNASSOCIATE_DATA, "Unassociate " + dataTextCtrl->GetName());
		menu.Bind(wxEVT_MENU, [&](wxCommandEvent&) {
			dataTextCtrl->ClearIndicators();
			dataTextCtrl = nullptr;
		}, ID_UNASSOCIATE_DATA);
	}

	menu.Append(ID_SHOW_UNHANDLED_OPCODES, "Show all opcodes not handled in the decopliler");
	menu.Bind(wxEVT_MENU, [&](wxCommandEvent&) {
		mainGui->OpenLog(wxAUI_DOCK_NONE);
		struct JdcStr errorBuffer = initializeJdcStr();
		for (int i = 0; i < numOfInstructions; i++) 
		{
			if (!isOpcodeImplementedInDecompiler(instructions[i].opcode)) 
			{
				sprintfJdc(&errorBuffer, 0, "%s at 0x%llX is not handled in the decompiler.", mnemonicStrs[instructions[i].opcode], instructions[i].address);
				mainGui->logTextCtrl->Log(errorBuffer.buffer, 1);
			}
		}
		freeJdcStr(&errorBuffer);
	}, ID_SHOW_UNHANDLED_OPCODES);

	PopupMenu(&menu, ScreenToClient(e.GetPosition()));
}

void DisassemblyTextCtrl::OnDisassemblyKeyDown(wxKeyEvent& e)
{
	int key = e.GetKeyCode();
	if ((e.GetModifiers() & wxMOD_CONTROL) != 0 && key != 0)
	{
		if (key == 'G')
		{
			ShowGoToAddressDialog();
		}
	}

	OnKeyDown(e);
	e.Skip();
}

void DisassemblyTextCtrl::OnUpdateDisassemblyUI(wxStyledTextEvent& e)
{
	UpdateTextCtrl();

	if (!HasFocus())
	{
		return;
	}

	ClearIndicators();

	int instructionIndex = GetCurrentLine();
	unsigned char isLineHighlighted = 0;

	if (decompilationTextCtrl)
	{
		decompilationTextCtrl->ClearIndicators();
		if (decompilationTextCtrl->currentDecompiledFunc != -1 &&
			instructionIndex >= mainGui->decompParams.functions[decompilationTextCtrl->currentDecompiledFunc].firstInstructionIndex && instructionIndex <= mainGui->decompParams.functions[decompilationTextCtrl->currentDecompiledFunc].lastInstructionIndex)
		{
			for (int i = 0; i < mainGui->decompParams.functions[decompilationTextCtrl->currentDecompiledFunc].numOfLines; i++)
			{
				struct AssociatedInstructions* a = &mainGui->decompParams.functions[decompilationTextCtrl->currentDecompiledFunc].associatedInstructions[i];
				for (int j = 0; j < a->numOfIndexes; j++)
				{
					if (a->indexes[j] == instructionIndex)
					{
						decompilationTextCtrl->HighlightLine(i, PURPLE_INDICATOR, 1);
						break;
					}
				}
			}

			ClearIndicators();
			HighlightLine(instructionIndex, PURPLE_INDICATOR, 0);
			isLineHighlighted = 1;
		}
	}

	if (functionsTextCtrl && instructionIndex < numOfInstructions)
	{
		functionsTextCtrl->ClearIndicators();
		int funcIndex = findFunctionByAddressInclusive(&mainGui->decompParams, instructions[instructionIndex].address);
		if (funcIndex != -1)
		{
			functionsTextCtrl->HighlightLine(funcIndex, PURPLE_INDICATOR, 1);
			ClearIndicators();
			HighlightLine(instructionIndex, PURPLE_INDICATOR, 0);
			isLineHighlighted = 1;
		}
	}

	if (dataTextCtrl && numOfInstructions > 0)
	{
		unsigned long long address = instructions[instructionIndex].address;
		FileSection* section = 0;
		unsigned long long fileOffset = rvaToFileOffset(mainGui->decompParams.sections, mainGui->decompParams.numOfSections, address - mainGui->decompParams.imageBase, &section);
		if (section)
		{
			dataTextCtrl->HighlightBytes(fileOffset, instructions[instructionIndex].numOfBytes, PURPLE_INDICATOR);
			ClearIndicators();
			HighlightLine(instructionIndex, PURPLE_INDICATOR, 0);
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

void DisassemblyTextCtrl::UpdateTextCtrl()
{
	if (!instructions || numOfInstructions == 0)
	{
		return;
	}

	int firstLine = GetFirstVisibleLine();
	int lastLine = firstLine + LinesOnScreen();
	if (GetLineLength(firstLine) != 0 && GetLineLength(lastLine) != 0)
	{
		return;
	}

	firstLine -= 100;
	if (firstLine < 0)
	{
		firstLine = 0;
	}

	lastLine += 100;
	if (lastLine > numOfInstructions)
	{
		lastLine = numOfInstructions;
	}

	SetReadOnly(false);
	Freeze();

	int sectionIndex = -1;
	struct JdcStr instructionStrBuffer = initializeJdcStr();
	for (int i = firstLine; i < lastLine; i++)
	{
		int lineLen = GetLineLength(i);
		if (lineLen != 0)
		{
			continue;
		}
		
		for (int j = sectionIndex + 1; j < mainGui->decompParams.numOfSections; j++)
		{
			if (mainGui->decompParams.sections[j].type == CODE_FST && instructions[i].address >= mainGui->decompParams.sections[j].rva + mainGui->decompParams.imageBase)
			{
				sectionIndex = j;
				break;
			}
		}

		char addressStr[20] = { 0 };
		sprintf(addressStr, "0x%llX", instructions[i].address);
		wxString addressInfoStr = wxString(addressStr) + wxString(mainGui->decompParams.sections[sectionIndex].name.buffer) + "\t";

		wxString asmStr = "";
		if (instructionToStr(&instructions[i], &instructionStrBuffer))
		{
			asmStr = wxString(instructionStrBuffer.buffer);
		}

		if (instructions[i].isInvalid)
		{
			asmStr += " ; invalid instruction";
		}
		else if (instructions[i].opcode == NO_MNEMONIC)
		{
			asmStr += " ; unrecognized opcode";
		}
		else if (instructions[i].address == mainGui->entryPoint + mainGui->decompParams.imageBase)
		{
			asmStr += " ; entry point";
		}

		unsigned long long dst = getJmpDst(&instructions[0], i, i - 0x1000);
		if (dst != 0)
		{
			char dstStr[20] = { 0 };
			sprintf(dstStr, "0x%llX", dst);
			asmStr += " ; dst: " + wxString(dstStr);
		}

		InsertText(PositionFromLine(i), addressInfoStr + asmStr);
	}

	freeJdcStr(&instructionStrBuffer);

	ApplyAsmHighlighting();

	Thaw();
	SetReadOnly(true);
}

void DisassemblyTextCtrl::ApplyAsmHighlighting()
{
	for (int i = 0; i < NUM_OF_DISASSEMBLY_COLORS; i++)
	{
		StyleSetForeground(i, mainGui->colorsMenu->disassemblyColors[i]);
	}

	int firstLine = GetFirstVisibleLine();
	int lastLine = firstLine + LinesOnScreen();

	firstLine -= 99;
	if (firstLine < 0)
	{
		firstLine = 0;
	}

	lastLine += 99;
	if (lastLine > numOfInstructions)
	{
		lastLine = numOfInstructions;
	}

	int pos = PositionFromLine(firstLine) + 1;
	wxString disassemblyText = GetValue();
	for (int i = firstLine; i < lastLine; i++)
	{
		struct DisassembledInstruction* instruction = &(instructions[i]);

		int tabPos = disassemblyText.find('\t', pos);
		wxString addressInfoStr = disassemblyText.substr(pos, tabPos - pos);
		wxString asmStr = disassemblyText.substr(tabPos + 1, disassemblyText.find('\n', tabPos) - (tabPos + 1));

		StartStyling(pos);
		SetStyling(addressInfoStr.length(), ADDRESS_ASM_COLOR);

		pos += addressInfoStr.length() + 1;

		StartStyling(pos);
		SetStyling(asmStr.length(), OPERATOR_ASM_COLOR);

		StartStyling(pos);

		int opcodeLen = strlen(mnemonicStrs[instruction->opcode]) + 1;
		if (instruction->group1Prefix != NO_PREFIX)
		{
			opcodeLen += strlen(getGroup1PrefixStr(instruction)) + 1;
		}
		SetStyling(opcodeLen, OPCODE_ASM_COLOR);

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
				SetStyling(regStr.length(), REGISTER_ASM_COLOR);
				regStart = loc + regStr.length();
			}
			else if (instruction->operands[i].type == MEM_ADDRESS)
			{
				if (instruction->operands[i].memoryAddress.reg != NO_REG)
				{
					wxString regStr = wxString(registerStrs[instruction->operands[i].memoryAddress.reg]);
					int loc = asmStr.find(regStr, regStart);
					StartStyling(pos + loc);
					SetStyling(regStr.length(), REGISTER_ASM_COLOR);
					regStart = loc + regStr.length();
				}

				if (instruction->operands[i].memoryAddress.regDisplacement != NO_REG)
				{
					wxString regStr = wxString(registerStrs[instruction->operands[i].memoryAddress.regDisplacement]);
					int loc = asmStr.find(regStr, regStart);
					StartStyling(pos + loc);
					SetStyling(regStr.length(), REGISTER_ASM_COLOR);
					regStart = loc + regStr.length();
				}

				if (instruction->operands[i].memoryAddress.segment != NO_SEGMENT)
				{
					wxString segStr = wxString(segmentStrs[instruction->operands[i].memoryAddress.segment]) + ":";
					int loc = asmStr.find(segStr, segStart);
					StartStyling(pos + loc);
					SetStyling(segStr.length() - 1, SEGMENT_ASM_COLOR);
					segStart = loc + segStr.length();
				}

				int ptrSize = instruction->operands[i].memoryAddress.ptrSize;
				if (ptrSize != 0)
				{
					wxString sizeStr = wxString(getPtrSizeStr(ptrSize));
					int loc = asmStr.find(sizeStr, ptrSizeStart);
					StartStyling(pos + loc);
					SetStyling(sizeStr.length(), PTR_SIZE_ASM_COLOR);
					ptrSizeStart = loc + sizeStr.length();
				}
			}
			else if (instruction->operands[i].type == SEGMENT)
			{
				wxString segStr = wxString(segmentStrs[instruction->operands[i].segment]);
				int loc = asmStr.find(segStr, segStart);
				StartStyling(pos + loc);
				SetStyling(segStr.length(), SEGMENT_ASM_COLOR);
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
				SetStyling(end - num, NUMBER_ASM_COLOR);

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
			SetStyling(asmStr.length() - commentStart, COMMENT_ASM_COLOR);
		}

		pos += asmStr.size();
	}
}