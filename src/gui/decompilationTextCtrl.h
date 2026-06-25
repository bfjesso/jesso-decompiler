#pragma once
#include "jdcTextCtrl.h"
#include "colorsMenu.h"

#include "../decompiler/decompilationStructs.h"

class DisassemblyTextCtrl;

class DecompilationTextCtrl : public JdcTextCtrl
{
public:
	DecompilationTextCtrl(wxWindow* parent, const wxSize& size, struct DecompilationParameters* decompParams, ColorsMenu* colorMenu, wxStaticText* statusText);

	ColorsMenu* colorsMenu = nullptr;
	DisassemblyTextCtrl* disassemblyTextCtrl = nullptr;

	struct DecompilationParameters* params = nullptr;
	int currentDecompiledFunc = -1;

	unsigned char showAssociatedInstructions = 1;

	void SetAssociatedDisassemblyTextCtrl(DisassemblyTextCtrl* window);

	void OnUpdateDecompilationUI(wxStyledTextEvent& e);

	void DecompileFunction(int functionIndex);

	void ApplyDecompilationHighlighting();
};