#pragma once
#include "guiUtils.h"
#include "jdcTextCtrl.h"
#include "../disassembler/disassemblyStructs.h"

class MainGui;

class InstructionInfoWindow : public wxWindow
{
public:
	InstructionInfoWindow(wxWindow* parent, MainGui* mainGuiRef, DisassembledInstruction* instruction, enum InstructionSetArchitecture isa, uint8_t* bytes, uint64_t bytesOffset);

	JdcTextCtrl* infoTextCtrl = nullptr;
	wxBoxSizer* vSizer = nullptr;
};