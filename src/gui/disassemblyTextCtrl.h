#pragma once
#include "jdcTextCtrl.h"

class MainGui;
class DecompilationTextCtrl;
class FunctionsTextCtrl;
class DataTextCtrl;

class DisassemblyTextCtrl : public JdcTextCtrl
{
public:
	DisassemblyTextCtrl(MainGui* parent, wxString name);

	MainGui* mainGui = nullptr;

	DecompilationTextCtrl* decompilationTextCtrl = nullptr;
	FunctionsTextCtrl* functionsTextCtrl = nullptr;
	DataTextCtrl* dataTextCtrl = nullptr;

	void Initialize(unsigned long long errorAddress);

	void ShowGoToAddressDialog();

	void DisassemblyRightClickOptions(wxContextMenuEvent& e);

	void OnDisassemblyKeyDown(wxKeyEvent& e);

	void OnUpdateDisassemblyUI(wxStyledTextEvent& e);

	void UpdateTextCtrl();

	void ApplyAsmHighlighting();

};