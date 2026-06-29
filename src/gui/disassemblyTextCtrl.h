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

	unsigned char showAssociatedDecompiledLines = 1;
	unsigned char showAssociatedFunctions = 1;
	unsigned char showBytesInDataViewer = 1;

	void Initialize(unsigned long long errorAddress);

	void OnUpdateDisassemblyUI(wxStyledTextEvent& e);

	void UpdateTextCtrl();

	void ApplyAsmHighlighting();

};