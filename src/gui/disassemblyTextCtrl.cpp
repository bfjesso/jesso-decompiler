#include "disassemblyTextCtrl.h"
#include "mainGui.h"

#include "../disassembler/disassembler.h"
#include "../decompiler/functions.h"
#include "../decompiler/decompilationUtils.h"

DisassemblyTextCtrl::DisassemblyTextCtrl(MainGui* parent) : JdcTextCtrl(parent)
{
	mainGui = parent;

	Bind(wxEVT_STC_UPDATEUI, &DisassemblyTextCtrl::OnUpdateDisassemblyUI, this);

	AddRightClickOption("Go to address", 'G', 0, [&](wxCommandEvent&) {
		wxTextEntryDialog dlg(this, "", "Go to address");
		if (dlg.ShowModal() == wxID_OK)
		{
			if (mainGui->decompParams.numOfInstructions == 0) 
			{
				wxMessageBox("No instructions loaded", "Failed to find address");
				return;
			}
			
			wxString txt = dlg.GetValue();
			unsigned long long address = 0;
			if (txt.ToULongLong(&address, 16))
			{
				int index = findInstructionByAddress(mainGui->decompParams.instructions, 0, mainGui->decompParams.numOfInstructions - 1, address);
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

	AddRightClickOption("Set associated decompilation", 0, 0, [&](wxCommandEvent& e) {
		wxArrayString windowCaptions;
		for (int i = 0; i < mainGui->decompilationTextCtrls.size(); i++)
		{
			windowCaptions.push_back("Decompilation " + std::to_string(i));
		}
		wxSingleChoiceDialog choiceDialog(this, "", "Choose a window", windowCaptions);
		if (choiceDialog.ShowModal() != wxID_CANCEL)
		{
			decompilationTextCtrl =  mainGui->decompilationTextCtrls[choiceDialog.GetSelection()];
		}
	});

	AddRightClickOption("Set associated functions list", 0, 0, [&](wxCommandEvent& e) {
		wxArrayString windowCaptions;
		for (int i = 0; i < mainGui->functionsTextCtrls.size(); i++)
		{
			windowCaptions.push_back("Functions " + std::to_string(i));
		}
		wxSingleChoiceDialog choiceDialog(this, "", "Choose a window", windowCaptions);
		if (choiceDialog.ShowModal() != wxID_CANCEL)
		{
			functionsTextCtrl = mainGui->functionsTextCtrls[choiceDialog.GetSelection()];
		}
	});

	AddRightClickOption("Set associated data window", 0, 0, [&](wxCommandEvent& e) {
		wxArrayString windowCaptions;
		for (int i = 0; i < mainGui->dataTextCtrls.size(); i++)
		{
			windowCaptions.push_back("Data " + std::to_string(i));
		}
		wxSingleChoiceDialog choiceDialog(this, "", "Choose a window", windowCaptions);
		if (choiceDialog.ShowModal() != wxID_CANCEL)
		{
			dataTextCtrl = mainGui->dataTextCtrls[choiceDialog.GetSelection()];
		}
	});

	AddRightClickOption("Show associated decompiled lines", 0, &showAssociatedDecompiledLines, [&](wxCommandEvent& e) {
		showAssociatedDecompiledLines = e.IsChecked();
		ClearIndicators();
		if (decompilationTextCtrl) 
		{
			decompilationTextCtrl->ClearIndicators();
		}
	});

	AddRightClickOption("Show bytes in data viewer", 0, &showBytesInDataViewer, [&](wxCommandEvent& e) {
		showBytesInDataViewer = e.IsChecked();
		ClearIndicators();
		if (dataTextCtrl) 
		{
			dataTextCtrl->ClearIndicators();
		}
	});

	AddRightClickOption("Show associated function", 0, &showAssociatedFunctions, [&](wxCommandEvent& e) {
		showAssociatedFunctions = e.IsChecked();
		ClearIndicators();
		if (functionsTextCtrl) 
		{
			functionsTextCtrl->ClearIndicators();
		}
	});
}

void DisassemblyTextCtrl::Initialize(unsigned long long errorAddress)
{
	ClearText();

	wxString newLines = "";
	for (int i = 0; i < mainGui->decompParams.numOfInstructions; i++)
	{
		newLines += "\n";
	}

	SetReadOnly(false);
	SetText(newLines);
	SetReadOnly(true);

	if (errorAddress != 0)
	{
		int errorIndex = findInstructionByAddress(mainGui->decompParams.instructions, 0, mainGui->decompParams.numOfInstructions - 1, errorAddress);
		CenterLine(errorIndex);
		UpdateTextCtrl();
		HighlightLine(errorIndex, RED_INDICATOR, 0);
	}
	else
	{
		int entryPointIndex = findInstructionByAddress(mainGui->decompParams.instructions, 0, mainGui->decompParams.numOfInstructions - 1, mainGui->entryPoint + mainGui->decompParams.imageBase);
		CenterLine(entryPointIndex);
		UpdateTextCtrl();
		HighlightLine(entryPointIndex, YELLOW_INDICATOR, 0);
	}
}

void DisassemblyTextCtrl::OnUpdateDisassemblyUI(wxStyledTextEvent& e)
{
	UpdateTextCtrl();

	if (decompilationTextCtrl && HasFocus())
	{
		decompilationTextCtrl->ClearIndicators();
		int instructionIndex = GetCurrentLine();
		if (decompilationTextCtrl->currentDecompiledFunc != -1 && showAssociatedDecompiledLines &&
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

			HighlightLine(instructionIndex, PURPLE_INDICATOR, 0);
		}

		if (showAssociatedFunctions && functionsTextCtrl && instructionIndex < mainGui->decompParams.numOfInstructions)
		{
			functionsTextCtrl->ClearIndicators();
			int funcIndex = findFunctionByAddressInclusive(&mainGui->decompParams, 0, mainGui->decompParams.numOfFunctions - 1, mainGui->decompParams.instructions[instructionIndex].address);
			if (funcIndex != -1)
			{
				functionsTextCtrl->HighlightLine(funcIndex, PURPLE_INDICATOR, 1);
				HighlightLine(instructionIndex, PURPLE_INDICATOR, 0);
			}
		}

		if (dataTextCtrl && dataTextCtrl->IsShown() && showBytesInDataViewer && mainGui->decompParams.numOfInstructions > 0)
		{
			dataTextCtrl->HighlightInstruction(mainGui->decompParams.instructions[instructionIndex].address, mainGui->decompParams.instructions[instructionIndex].numOfBytes);
			HighlightLine(instructionIndex, PURPLE_INDICATOR, 0);
		}
	}

	OnUpdateUI(e);
}

void DisassemblyTextCtrl::UpdateTextCtrl()
{
	if (mainGui->decompParams.numOfInstructions == 0)
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
	if (lastLine > mainGui->decompParams.numOfInstructions)
	{
		lastLine = mainGui->decompParams.numOfInstructions;
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
			if (mainGui->decompParams.sections[j].type == CODE_FST && mainGui->decompParams.instructions[i].address >= mainGui->decompParams.sections[j].virtualAddress + mainGui->decompParams.imageBase)
			{
				sectionIndex = j;
				break;
			}
		}

		char addressStr[20] = { 0 };
		sprintf(addressStr, "0x%llX", mainGui->decompParams.instructions[i].address);
		wxString addressInfoStr = wxString(addressStr) + wxString(mainGui->decompParams.sections[sectionIndex].name.buffer) + "\t";

		wxString asmStr = "";
		if (instructionToStr(&mainGui->decompParams.instructions[i], &instructionStrBuffer))
		{
			asmStr = wxString(instructionStrBuffer.buffer);
		}

		if (mainGui->decompParams.instructions[i].isInvalid)
		{
			asmStr += " ; invalid instruction";
		}
		else if (mainGui->decompParams.instructions[i].opcode == NO_MNEMONIC)
		{
			asmStr += " ; unrecognized opcode";
		}
		else if (mainGui->decompParams.instructions[i].address == mainGui->entryPoint + mainGui->decompParams.imageBase)
		{
			asmStr += " ; entry point";
		}

		unsigned long long dst = getJmpDst(&mainGui->decompParams.instructions[0], i, i - 0x1000);
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
	if (lastLine > mainGui->decompParams.numOfInstructions)
	{
		lastLine = mainGui->decompParams.numOfInstructions;
	}

	int pos = PositionFromLine(firstLine) + 1;
	wxString disassemblyText = GetValue();
	for (int i = firstLine; i < lastLine; i++)
	{
		struct DisassembledInstruction* instruction = &(mainGui->decompParams.instructions[i]);

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