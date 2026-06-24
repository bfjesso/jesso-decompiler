#pragma once
#include "jdcTextCtrl.h"
#include "colorsMenu.h"
#include "disassemblyWindow.h"

#include "../decompiler/decompilationStructs.h"

class DecompilationWindow : public JdcTextCtrl
{
public:
	DecompilationWindow(wxWindow* parent, const wxSize& size, ColorsMenu* colorMenu, wxStaticText* statusText);

	ColorsMenu* colorsMenu = nullptr;
	DisassemblyWindow* disassemblyWindow = nullptr;

	int currentDecompiledFunc = -1;

	void SetAssociatedDisassemblyWindow(DisassemblyWindow* window);

	void DecompileFunction(struct DecompilationParameters* params, int functionIndex);

	void ApplyDecompilationHighlighting(struct DecompilationParameters* params);
};