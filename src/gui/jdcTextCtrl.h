#pragma once
#include "guiUtils.h"
#include <wx/stc/stc.h>
#include <wx/fdrepdlg.h>

#include "../disassembler/disassemblyStructs.h"
#include "../decompiler/decompilationStructs.h"

enum JdcTextCtrlType 
{
	DISASSEMBLY_CTRL_TYPE,
	DECOMPILATION_CTRL_TYPE,
	DATA_CTRL_TYPE
};

enum IndicatorColor
{
	PURPLE_INDICATOR,
	GRAY_INDICATOR,
	YELLOW_INDICATOR,
	RED_INDICATOR
};

class JdcTextCtrl : public wxStyledTextCtrl 
{
private:
	char IsCharDigit(char c);

public:
	JdcTextCtrl(wxWindow* parent, const wxSize& size, enum JdcTextCtrlType type, std::vector<DisassembledInstruction>* instructions, std::vector<Function>* functions, int* currentFunc);

	enum JdcTextCtrlType ctrlType;

	std::vector<DisassembledInstruction>* instructionsRef = nullptr;
	std::vector<Function>* functionsRef = nullptr;
	int* currentDecompiledFuncRef = nullptr;

	std::vector<JdcTextCtrl*> associatedTextCtrls;

	wxFindReplaceData findData;
	wxFindReplaceDialog* findDialog = nullptr;
	wxString lastFindText = "";
	int totalFindResults = 0;

	unsigned char showAssociatedInstructions = 1;
	unsigned char showBytesInDataViewer = 1;
	unsigned char highlightSelectedLines = 1;

	void ClearText();

	void CenterLine(int line);

	void HighlightLine(int line, enum IndicatorColor color, unsigned char gotoLine);

	void ClearIndicators();

	void AddAssociatedTextCtrl(JdcTextCtrl* textCtrl);

	void ShowGoToAddrDialog();

	void ShowFindDialog();

	void OnFindDialog(wxFindDialogEvent& e);

	int FindInRange(const wxString& text, int start, int end, int flags, unsigned char forward);

	int CountNumOfResults(const wxString& text, int end, int flags);

	void OnFindDialogClose(wxFindDialogEvent& e);

	void RightClickOptions(wxContextMenuEvent& e);

	void OnKeyDown(wxKeyEvent& e);

	void OnUpdateUI(wxStyledTextEvent& e);

	void ApplySyntaxHighlighting(ImportedFunction* imports, int numOfImports);

	void ApplyAsmHighlighting();

	void ColorAllStrs(wxString text, const char* str, DecompilationColor color, unsigned char forceColor);
};