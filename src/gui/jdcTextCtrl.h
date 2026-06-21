#pragma once
#include "guiUtils.h"
#include <wx/stc/stc.h>
#include <wx/fdrepdlg.h>

#include "../disassembler/disassemblyStructs.h"
#include "../decompiler/decompilationStructs.h"

enum SyntaxHighlightingType
{
	DISASSEMBLY_HIGHLIGHTING,
	DECOMPILATION_HIGHLIGHTING,
	DATA_HIGHLIGHTING
};

enum IndicatorColor
{
	PURPLE_INDICATOR,
	GRAY_INDICATOR,
	YELLOW_INDICATOR,
	RED_INDICATOR
};

struct RightClickOption 
{
	wxString name;
	char commandKey;
	unsigned char* check;
	std::function<void(wxCommandEvent&)> function;
};

class JdcTextCtrl : public wxStyledTextCtrl 
{
private:
	char IsCharDigit(char c);

public:
	JdcTextCtrl(wxWindow* parent, const wxSize& size);

	std::function<void()> additionalOnUpdateUI;
	std::vector<struct RightClickOption> additionalRightClickOptions;

	wxFindReplaceData findData;
	wxFindReplaceDialog* findDialog = nullptr;
	wxString lastFindText = "";
	int totalFindResults = 0;

	unsigned char highlightSelectedLines = 1;

	enum SyntaxHighlightingType highlightingType;

	void EnableLineNumbers();

	void ClearText();

	void CenterLine(int line);

	void HighlightLine(int line, enum IndicatorColor color, unsigned char gotoLine);

	void ClearIndicators();

	void ShowFindDialog();

	void OnFindDialog(wxFindDialogEvent& e);

	int FindInRange(const wxString& text, int start, int end, int flags, unsigned char forward);

	int CountNumOfResults(const wxString& text, int end, int flags);

	void OnFindDialogClose(wxFindDialogEvent& e);

	void AddRightClickOption(wxString name, char commandKey, unsigned char* check, const std::function<void(wxCommandEvent&)>& function);

	void RightClickOptions(wxContextMenuEvent& e);

	void OnKeyDown(wxKeyEvent& e);

	void SetAdditionalOnUpdateUI(const std::function<void()>& function);

	void OnUpdateUI(wxStyledTextEvent& e);

	void ApplySyntaxHighlighting(struct DecompilationParameters* params, wxColour* decompColors);

	void ApplyAsmHighlighting(struct DisassembledInstruction* instructions, int numOfInstructions, wxColour* disassemblyColors);

	void ApplyDataHighlighting(wxColour* dataColors);

	void ColorAllStrs(wxString text, const char* str, DecompilationColor color, unsigned char forceColor);
};