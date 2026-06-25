#pragma once
#include "jdcTextCtrl.h"
#include "colorsMenu.h"

class DecompilationTextCtrl;
class FunctionsTextCtrl;
class DataTextCtrl;

class DisassemblyTextCtrl : public JdcTextCtrl
{
public:
	DisassemblyTextCtrl(wxWindow* parent, const wxSize& size, struct DecompilationParameters* decompParams, ColorsMenu* colorMenu);

	ColorsMenu* colorsMenu = nullptr;

	DecompilationTextCtrl* decompilationTextCtrl = nullptr;
	FunctionsTextCtrl* functionsTextCtrl = nullptr;
	DataTextCtrl* dataTextCtrl = nullptr;
	
	struct DecompilationParameters* params = nullptr;
	unsigned long long entryPoint = 0;

	unsigned char showAssociatedDecompiledLines = 1;
	unsigned char showAssociatedFunctions = 1;
	unsigned char showBytesInDataViewer = 1;

	void SetAssociatedDecompilationTextCtrl(DecompilationTextCtrl* window);

	void SetAssociatedFunctionsTextCtrl(FunctionsTextCtrl* window);

	void SetAssociatedDataTextCtrl(DataTextCtrl* window);

	void Initialize(unsigned long long entryPoint, unsigned long long errorAddress);

	void ClearData();

	void OnUpdateDisassemblyUI(wxStyledTextEvent& e);

	void UpdateTextCtrl();

	void ApplyAsmHighlighting();

};