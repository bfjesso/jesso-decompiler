#pragma once
#include "jdcTextCtrl.h"
#include "colorsMenu.h"

class DisassemblyWindow : public JdcTextCtrl
{
public:
	DisassemblyWindow(wxWindow* parent, const wxSize& size, ColorsMenu* colorMenu);

	ColorsMenu* colorsMenu = nullptr;
	
	struct DisassembledInstruction* instructions = nullptr;
	int numOfInstructions = 0;
	struct FileSection* sections = nullptr;
	int numOfSections = 0;
	unsigned long long imageBase = 0;
	unsigned long long entryPoint = 0;

	void Initialize(struct DisassembledInstruction* instructions, int numOfInstructions, struct FileSection* sections, int numOfSections, unsigned long long imageBase, unsigned long long entryPoint, unsigned long long errorAddress);

	void ClearData();

	void OnUpdateDisassemblyUI(wxStyledTextEvent& e);

	void UpdateTextCtrl();

	void ApplyAsmHighlighting();

};