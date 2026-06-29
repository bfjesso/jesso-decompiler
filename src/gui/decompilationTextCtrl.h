#pragma once
#include "jdcTextCtrl.h"

class MainGui;
class DisassemblyTextCtrl;

class DecompilationTextCtrl : public JdcTextCtrl
{
public:
	DecompilationTextCtrl(MainGui* parent, wxString name);

	MainGui* mainGui = nullptr;

	DisassemblyTextCtrl* disassemblyTextCtrl = nullptr;

	int currentDecompiledFunc = -1;

	unsigned char showAssociatedInstructions = 1;

	void OnUpdateDecompilationUI(wxStyledTextEvent& e);

	void DecompileFunction(int functionIndex);

	void ApplyDecompilationHighlighting();
};