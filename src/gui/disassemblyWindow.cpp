#include "disassemblyWindow.h"

#include "../disassembler/disassembler.h"
#include "../decompiler/decompilationUtils.h"

DisassemblyWindow::DisassemblyWindow(wxWindow* parent, const wxSize& size, ColorsMenu* colorMenu, wxStaticText* statusText) : JdcTextCtrl(parent, size, statusText)
{
	colorsMenu = colorMenu;

	Bind(wxEVT_STC_UPDATEUI, &DisassemblyWindow::OnUpdateDisassemblyUI, this);

	AddRightClickOption("Go to address", 'G', 0, [&](wxCommandEvent&) {
		wxTextEntryDialog dlg(this, "", "Go to address");
		if (dlg.ShowModal() == wxID_OK)
		{
			if (!instructions || numOfInstructions == 0) 
			{
				wxMessageBox("No instructions loaded", "Failed to find address");
				return;
			}
			
			wxString txt = dlg.GetValue();
			unsigned long long address = 0;
			if (txt.ToULongLong(&address, 16))
			{
				int index = findInstructionByAddress(instructions, 0, numOfInstructions - 1, address);
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
}

void DisassemblyWindow::Initialize(struct DisassembledInstruction* instructionsPtr, int amountOfInstructions, struct FileSection* sectionsPtr, int amountOfSections, unsigned long long baseOfImage, unsigned long long pointOfEntry, unsigned long long errorAddress)
{
	instructions = instructionsPtr;
	numOfInstructions = amountOfInstructions;
	sections = sectionsPtr;
	numOfSections = amountOfSections;
	imageBase = baseOfImage;
	entryPoint = pointOfEntry;

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
		int errorIndex = findInstructionByAddress(instructions, 0, numOfInstructions - 1, errorAddress);
		CenterLine(errorIndex);
		UpdateTextCtrl();
		HighlightLine(errorIndex, RED_INDICATOR, 0);
	}
	else
	{
		int entryPointIndex = findInstructionByAddress(instructions, 0, numOfInstructions - 1, entryPoint + imageBase);
		CenterLine(entryPointIndex);
		UpdateTextCtrl();
		HighlightLine(entryPointIndex, YELLOW_INDICATOR, 0);
	}
}

void DisassemblyWindow::ClearData()
{
	instructions = 0;
	numOfInstructions = 0;
	sections = 0;
	numOfSections = 0;
	imageBase = 0;
	entryPoint = 0;

	ClearText();
}

void DisassemblyWindow::OnUpdateDisassemblyUI(wxStyledTextEvent& e)
{
	UpdateTextCtrl();
	OnUpdateUI(e);
}

void DisassemblyWindow::UpdateTextCtrl()
{
	if (!instructions || numOfInstructions == 0 || !sections || numOfSections == 0)
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
		for (int j = sectionIndex + 1; j < numOfSections; j++)
		{
			if (sections[j].type == CODE_FST && instructions[i].address >= sections[j].virtualAddress + imageBase)
			{
				sectionIndex = j;
				break;
			}
		}

		char addressStr[20] = { 0 };
		sprintf(addressStr, "0x%llX", instructions[i].address);
		wxString addressInfoStr = wxString(addressStr) + wxString(sections[sectionIndex].name.buffer) + "\t";

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
		else if (instructions[i].address == entryPoint + imageBase)
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

void DisassemblyWindow::ApplyAsmHighlighting()
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