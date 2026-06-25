#include "disassemblyTextCtrl.h"
#include "decompilationTextCtrl.h"
#include "functionsTextCtrl.h"
#include "dataTextCtrl.h"

#include "../disassembler/disassembler.h"
#include "../decompiler/functions.h"
#include "../decompiler/decompilationUtils.h"

DisassemblyTextCtrl::DisassemblyTextCtrl(wxWindow* parent, const wxSize& size, struct DecompilationParameters* decompParams, ColorsMenu* colorMenu, wxStaticText* statusText) : JdcTextCtrl(parent, size, statusText)
{
	params = decompParams;
	colorsMenu = colorMenu;

	Bind(wxEVT_STC_UPDATEUI, &DisassemblyTextCtrl::OnUpdateDisassemblyUI, this);

	AddRightClickOption("Go to address", 'G', 0, [&](wxCommandEvent&) {
		wxTextEntryDialog dlg(this, "", "Go to address");
		if (dlg.ShowModal() == wxID_OK)
		{
			if (!params || params->numOfInstructions == 0) 
			{
				wxMessageBox("No instructions loaded", "Failed to find address");
				return;
			}
			
			wxString txt = dlg.GetValue();
			unsigned long long address = 0;
			if (txt.ToULongLong(&address, 16))
			{
				int index = findInstructionByAddress(params->instructions, 0, params->numOfInstructions - 1, address);
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

void DisassemblyTextCtrl::SetAssociatedDecompilationTextCtrl(DecompilationTextCtrl* window)
{
	decompilationTextCtrl = window;
}

void DisassemblyTextCtrl::SetAssociatedFunctionsTextCtrl(FunctionsTextCtrl* window)
{
	functionsTextCtrl = window;
}

void DisassemblyTextCtrl::SetAssociatedDataTextCtrl(DataTextCtrl* window)
{
	dataTextCtrl = window;
}

void DisassemblyTextCtrl::Initialize(unsigned long long pointOfEntry, unsigned long long errorAddress)
{
	entryPoint = pointOfEntry;

	ClearText();

	wxString newLines = "";
	for (int i = 0; i < params->numOfInstructions; i++)
	{
		newLines += "\n";
	}

	SetReadOnly(false);
	SetText(newLines);
	SetReadOnly(true);

	if (errorAddress != 0)
	{
		int errorIndex = findInstructionByAddress(params->instructions, 0, params->numOfInstructions - 1, errorAddress);
		CenterLine(errorIndex);
		UpdateTextCtrl();
		HighlightLine(errorIndex, RED_INDICATOR, 0);
	}
	else
	{
		int entryPointIndex = findInstructionByAddress(params->instructions, 0, params->numOfInstructions - 1, entryPoint + params->imageBase);
		CenterLine(entryPointIndex);
		UpdateTextCtrl();
		HighlightLine(entryPointIndex, YELLOW_INDICATOR, 0);
	}
}

void DisassemblyTextCtrl::ClearData()
{
	entryPoint = 0;
	ClearText();
}

void DisassemblyTextCtrl::OnUpdateDisassemblyUI(wxStyledTextEvent& e)
{
	UpdateTextCtrl();

	if (params && decompilationTextCtrl && HasFocus())
	{
		decompilationTextCtrl->ClearIndicators();
		int instructionIndex = GetCurrentLine();
		if (decompilationTextCtrl->currentDecompiledFunc != -1 && showAssociatedDecompiledLines &&
			instructionIndex >= params->functions[decompilationTextCtrl->currentDecompiledFunc].firstInstructionIndex && instructionIndex <= params->functions[decompilationTextCtrl->currentDecompiledFunc].lastInstructionIndex)
		{
			for (int i = 0; i < params->functions[decompilationTextCtrl->currentDecompiledFunc].numOfLines; i++)
			{
				struct AssociatedInstructions* a = &params->functions[decompilationTextCtrl->currentDecompiledFunc].associatedInstructions[i];
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

		if (showAssociatedFunctions && functionsTextCtrl && instructionIndex < params->numOfInstructions)
		{
			functionsTextCtrl->ClearIndicators();
			int funcIndex = findFunctionByAddressInclusive(params, 0, params->numOfFunctions - 1, params->instructions[instructionIndex].address);
			if (funcIndex != -1)
			{
				functionsTextCtrl->HighlightLine(funcIndex, PURPLE_INDICATOR, 1);
				HighlightLine(instructionIndex, PURPLE_INDICATOR, 0);
			}
		}

		if (dataTextCtrl && dataTextCtrl->IsShown() && showBytesInDataViewer && params->numOfInstructions > 0)
		{
			dataTextCtrl->HighlightInstruction(params->instructions[instructionIndex].address, params->instructions[instructionIndex].numOfBytes);
			HighlightLine(instructionIndex, PURPLE_INDICATOR, 0);
		}
	}

	OnUpdateUI(e);
}

void DisassemblyTextCtrl::UpdateTextCtrl()
{
	if (!params || params->numOfInstructions == 0)
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
	if (lastLine > params->numOfInstructions)
	{
		lastLine = params->numOfInstructions;
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
		
		for (int j = sectionIndex + 1; j < params->numOfSections; j++)
		{
			if (params->sections[j].type == CODE_FST && params->instructions[i].address >= params->sections[j].virtualAddress + params->imageBase)
			{
				sectionIndex = j;
				break;
			}
		}

		char addressStr[20] = { 0 };
		sprintf(addressStr, "0x%llX", params->instructions[i].address);
		wxString addressInfoStr = wxString(addressStr) + wxString(params->sections[sectionIndex].name.buffer) + "\t";

		wxString asmStr = "";
		if (instructionToStr(&params->instructions[i], &instructionStrBuffer))
		{
			asmStr = wxString(instructionStrBuffer.buffer);
		}

		if (params->instructions[i].isInvalid)
		{
			asmStr += " ; invalid instruction";
		}
		else if (params->instructions[i].opcode == NO_MNEMONIC)
		{
			asmStr += " ; unrecognized opcode";
		}
		else if (params->instructions[i].address == entryPoint + params->imageBase)
		{
			asmStr += " ; entry point";
		}

		unsigned long long dst = getJmpDst(&params->instructions[0], i, i - 0x1000);
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
	highlightingType = DISASSEMBLY_HIGHLIGHTING;
	for (int i = 0; i < NUM_OF_DISASSEMBLY_COLORS; i++)
	{
		StyleSetForeground(i, colorsMenu->disassemblyColors[i]);
	}

	int firstLine = GetFirstVisibleLine();
	int lastLine = firstLine + LinesOnScreen();

	firstLine -= 99;
	if (firstLine < 0)
	{
		firstLine = 0;
	}

	lastLine += 99;
	if (lastLine > params->numOfInstructions)
	{
		lastLine = params->numOfInstructions;
	}

	int pos = PositionFromLine(firstLine) + 1;
	wxString disassemblyText = GetValue();
	for (int i = firstLine; i < lastLine; i++)
	{
		struct DisassembledInstruction* instruction = &(params->instructions[i]);

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