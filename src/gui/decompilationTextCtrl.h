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

	void ShowRenameDialog(struct JdcStr* currentName);

	void DecompilationRightClickOptions(wxContextMenuEvent& e);

	void OnUpdateDecompilationUI(wxStyledTextEvent& e);

	void DecompileFunction(int functionIndex);

	void ApplyDecompilationHighlighting();

	void ColorAllStrs(wxString text, const char* str, DecompilationColor color, unsigned char forceColor);
};