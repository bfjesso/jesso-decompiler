#pragma once
#include "jdcTextCtrl.h"
#include "colorsMenu.h"

class DecompilationTextCtrl;
class FunctionsTextCtrl;
class DataTextCtrl;

class DisassemblyTextCtrl : public JdcTextCtrl
{
public:
	DisassemblyTextCtrl(wxWindow* parent, struct DecompilationParameters* decompParams, ColorsMenu* colorMenu, const std::function<DecompilationTextCtrl* ()>& getDecompTextCtrl, const std::function<FunctionsTextCtrl* ()>& getFuncsTextCtrl, const std::function<DataTextCtrl* ()>& getDatTextCtrl);

	ColorsMenu* colorsMenu = nullptr;

	std::function<DecompilationTextCtrl* ()> getDecompilationTextCtrl;
	std::function<FunctionsTextCtrl* ()> getFunctionsTextCtrl;
	std::function<DataTextCtrl* ()> getDataTextCtrl;

	DecompilationTextCtrl* decompilationTextCtrl = nullptr;
	FunctionsTextCtrl* functionsTextCtrl = nullptr;
	DataTextCtrl* dataTextCtrl = nullptr;
	
	struct DecompilationParameters* params = nullptr;
	unsigned long long entryPoint = 0;

	unsigned char showAssociatedDecompiledLines = 1;
	unsigned char showAssociatedFunctions = 1;
	unsigned char showBytesInDataViewer = 1;

	void Initialize(unsigned long long entryPoint, unsigned long long errorAddress);

	void ClearData();

	void OnUpdateDisassemblyUI(wxStyledTextEvent& e);

	void UpdateTextCtrl();

	void ApplyAsmHighlighting();

};