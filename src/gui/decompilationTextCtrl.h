#pragma once
#include "jdcTextCtrl.h"
#include "colorsMenu.h"
#include "disassemblyTextCtrl.h"

#include "../decompiler/decompilationStructs.h"

class DecompilationTextCtrl : public JdcTextCtrl
{
public:
	DecompilationTextCtrl(wxWindow* parent, const wxSize& size, ColorsMenu* colorMenu, wxStaticText* statusText);

	ColorsMenu* colorsMenu = nullptr;
	DisassemblyTextCtrl* disassemblyTextCtrl = nullptr;

	int currentDecompiledFunc = -1;

	void SetAssociatedDisassemblyTextCtrl(DisassemblyTextCtrl* window);

	void DecompileFunction(struct DecompilationParameters* params, int functionIndex);

	void ApplyDecompilationHighlighting(struct DecompilationParameters* params);
};