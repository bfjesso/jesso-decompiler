#include "instructionInfoWindow.h"
#include "mainGui.h"
#include "../disassembler/disassembler.h"
#include "../jdc-str/jdcStr.h"

InstructionInfoWindow::InstructionInfoWindow(wxWindow* parent, MainGui* mainGuiRef, DisassembledInstruction* instruction, enum InstructionSetArchitecture isa, uint8_t* bytes, uint64_t bytesOffset) : wxWindow(parent, wxID_ANY)
{
	SetOwnBackgroundColour(backgroundColor);

	infoTextCtrl = new JdcTextCtrl(this, mainGuiRef, "instruction info");
	infoTextCtrl->SetReadOnly(false);

	struct JdcStr instructionInfoStr = initializeJdcStr();
	generateInstructionInfoStr(bytes, bytesOffset, instruction, isa, &instructionInfoStr);
	infoTextCtrl->SetText(instructionInfoStr.buffer);

	infoTextCtrl->SetReadOnly(true);

	vSizer = new wxBoxSizer(wxVERTICAL);
	vSizer->Add(infoTextCtrl, 1, wxEXPAND);

	SetSizerAndFit(vSizer);
	SetMinSize(wxSize(200, 200));
}