#pragma once
#include "jdcTextCtrl.h"

class DecompilationTextCtrl;
class FunctionsTextCtrl;
class DataTextCtrl;

class DisassemblyTextCtrl : public JdcTextCtrl
{
public:
	DisassemblyTextCtrl(wxWindow* parent, MainGui* mainGuiRef, wxString name, struct DisassembledInstruction* disassembledInstructions, int32_t amountOfInstructions);

	struct DisassembledInstruction* instructions;
	int32_t numOfInstructions;

	DecompilationTextCtrl* decompilationTextCtrl = nullptr;
	FunctionsTextCtrl* functionsTextCtrl = nullptr;
	DataTextCtrl* dataTextCtrl = nullptr;

	void ClearData();

	void Initialize(struct DisassembledInstruction* disassembledInstructions, int32_t amountOfInstructions, int32_t centerInstructionIndex, uint64_t errorAddress);

	void ResetDisassembly();

	void ShowGoToAddressDialog();

	void HighlightLine(int32_t line, enum IndicatorColor color, bool gotoLine);

	void DisassemblyRightClickOptions(wxContextMenuEvent& e);

	void OnDisassemblyKeyDown(wxKeyEvent& e);

	void OnUpdateDisassemblyUI(wxStyledTextEvent& e);

	void UpdateTextCtrl();

	void ApplyAsmHighlighting();

};