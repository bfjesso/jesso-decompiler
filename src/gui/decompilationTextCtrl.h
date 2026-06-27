#pragma once
#include "jdcTextCtrl.h"
#include "colorsMenu.h"

#include "../decompiler/decompilationStructs.h"

class DisassemblyTextCtrl;

class DecompilationTextCtrl : public JdcTextCtrl
{
public:
	DecompilationTextCtrl(wxWindow* parent, struct DecompilationParameters* decompParams, ColorsMenu* colorMenu, const std::function<DisassemblyTextCtrl* ()>& getDisasmTextCtrl);

	ColorsMenu* colorsMenu = nullptr;

	std::function<DisassemblyTextCtrl* ()> getDisassemblyTextCtrl;
	DisassemblyTextCtrl* disassemblyTextCtrl = nullptr;

	struct DecompilationParameters* params = nullptr;
	int currentDecompiledFunc = -1;

	unsigned char showAssociatedInstructions = 1;

	void OnUpdateDecompilationUI(wxStyledTextEvent& e);

	void DecompileFunction(int functionIndex);

	void ApplyDecompilationHighlighting();
};