#pragma once
#include "guiUtils.h"
#include <wx/stc/stc.h>
#include <wx/fdrepdlg.h>

#include "../disassembler/disassemblyStructs.h"
#include "../decompiler/decompilationStructs.h"

enum JdcTextCtrlType 
{
	DISASSEMBLY_CTRL_TYPE,
	DECOMPILATION_CTRL_TYPE
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
	JdcTextCtrl(wxWindow* parent, const wxSize& size, enum JdcTextCtrlType type);

	enum JdcTextCtrlType ctrlType;
	std::function<void()> additionalOnUpdateUI;
	std::vector<struct RightClickOption> additionalRightClickOptions;

	wxFindReplaceData findData;
	wxFindReplaceDialog* findDialog = nullptr;
	wxString lastFindText = "";
	int totalFindResults = 0;

	unsigned char highlightSelectedLines = 1;

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

	void ApplySyntaxHighlighting(struct DecompilationParameters* params);

	void ApplyAsmHighlighting(struct DisassembledInstruction* instructions, int numOfInstructions);

	void ColorAllStrs(wxString text, const char* str, DecompilationColor color, unsigned char forceColor);
};