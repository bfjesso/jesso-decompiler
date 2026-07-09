#pragma once
#include "jdcTextCtrl.h"

class MainGui;
class DecompilationTextCtrl;
class FunctionsTextCtrl;
class DataTextCtrl;

class DisassemblyTextCtrl : public JdcTextCtrl
{
public:
	DisassemblyTextCtrl(wxWindow* parent, MainGui* mainGuiRef, wxString name, struct DisassembledInstruction* disassembledInstructions, int amountOfInstructions);

	MainGui* mainGui = nullptr;

	struct DisassembledInstruction* instructions;
	int numOfInstructions;

	DecompilationTextCtrl* decompilationTextCtrl = nullptr;
	FunctionsTextCtrl* functionsTextCtrl = nullptr;
	DataTextCtrl* dataTextCtrl = nullptr;

	void Initialize(struct DisassembledInstruction* disassembledInstructions, int amountOfInstructions, unsigned long long errorAddress);

	void ShowGoToAddressDialog();

	void DisassemblyRightClickOptions(wxContextMenuEvent& e);

	void OnDisassemblyKeyDown(wxKeyEvent& e);

	void OnUpdateDisassemblyUI(wxStyledTextEvent& e);

	void UpdateTextCtrl();

	void ApplyAsmHighlighting();

};