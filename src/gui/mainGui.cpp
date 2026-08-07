#include "mainGui.h"
#include <wx/numdlg.h>
#include "bytesDisassemblerWindow.h"
#include "sectionsGrid.h"
#include "stringsTextCtrl.h"
#include "importsGrid.h"
#include "fileHeadersWindow.h"
#include "calculatorWindow.h"
#include "../decompiler/decompilationUtils.h"
#include "../disassembler/mnemonics.h"
#include "../decompiler/functions.h"

wxBEGIN_EVENT_TABLE(MainGui, wxFrame)
EVT_CLOSE(MainGui::CloseApp)
EVT_AUI_PANE_CLOSE(MainGui::OnPaneClose)
EVT_AUINOTEBOOK_PAGE_CLOSE(NotebookID, MainGui::OnPageClose)
EVT_AUINOTEBOOK_TAB_RIGHT_DOWN(NotebookID, MainGui::OnTabRightClick)
EVT_RIGHT_DOWN(MainGui::OnMouseRightClick)
wxEND_EVENT_TABLE()

class ColoredTabArt final : public wxAuiDefaultTabArt
{
public:
	ColoredTabArt() {}

	wxAuiTabArt* Clone() override
	{
		return new ColoredTabArt(*this);
	}

	void DrawBackground(wxDC& dc, wxWindow* wnd, const wxRect& rect) override
	{
		dc.SetPen(*wxTRANSPARENT_PEN);
		dc.SetBrush(wxBrush(foregroundColor));
		dc.DrawRectangle(rect);
	}

	void DrawTab(wxDC& dc, wxWindow* wnd, const wxAuiNotebookPage& page, const wxRect& in_rect, int closeButtonState, wxRect* outTabRect, wxRect* outButtonRect, int* xExtent) override
	{
		int textWidth = 0;
		int textHeight = 0;
		dc.GetTextExtent(page.caption, &textWidth, &textHeight);

		const int tabWidth = textWidth + 32;
		wxRect rect = in_rect;
		rect.width = tabWidth;
		rect.Deflate(1, 2);

		const wxColour fill = page.active ? backgroundColor : foregroundColor;
		const wxColour border = fill.ChangeLightness(page.active ? 85 : 70);

		dc.SetPen(wxPen(border));
		dc.SetBrush(wxBrush(fill));
		dc.DrawRoundedRectangle(rect, 2);

		dc.SetFont(wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT));
		dc.SetTextForeground(textColor);

		dc.DrawText(page.caption, rect.x + 8, rect.y + (rect.height - dc.GetCharHeight()) / 2);

		if (outTabRect) { *outTabRect = rect; }
		if (xExtent) { *xExtent = tabWidth; }
		if (outButtonRect)
		{
			wxRect btn(rect.GetRight() - 16, rect.y + 4, 12, 12);
			*outButtonRect = btn;

			wxColour btnColor = textColor;
			if (closeButtonState == wxAUI_BUTTON_STATE_HOVER) { btnColor = wxColour(255, 255, 255); }
			else if (closeButtonState == wxAUI_BUTTON_STATE_PRESSED) { btnColor = wxColour(255, 50, 50); }

			dc.SetPen(wxPen(btnColor, 2));
			dc.DrawLine(btn.x + 1, btn.y + 1, btn.x + 9, btn.y + 9);
			dc.DrawLine(btn.x + 9, btn.y + 1, btn.x + 1, btn.y + 9);
		}
	}
};

MainGui::MainGui() : wxFrame(nullptr, wxID_ANY, "Jesso Decompiler x64")
{
	SetMinSize(wxSize(800, 600));
	Maximize();
	
	SetOwnBackgroundColour(backgroundColor);

	colorsMenu = new ColorsMenu();

	menuBar = new wxMenuBar();

	wxMenu* fileMenu = new wxMenu();
	AddMenuItem(fileMenu, OpenFileID, "Open file", [&](wxCommandEvent& ce) -> void { OpenFile(); });
	AddMenuItem(fileMenu, DisassembleFileID, "Disassemble file", [&](wxCommandEvent& ce) -> void { DisassembleFile(); });
	AddMenuItem(fileMenu, AnalyzeFileID, "Analyze file", [&](wxCommandEvent& ce) -> void { AnalyzeFile(); });

	wxMenu* toolMenu = new wxMenu();
	AddMenuItem(toolMenu, OpenDisassemblyID, "Disassembly", [&](wxCommandEvent& ce) -> void { AddDisassemblyTextCtrl(); });
	AddMenuItem(toolMenu, OpenDecompilationID, "Decompilation", [&](wxCommandEvent& ce) -> void { AddDecompilationTextCtrl(); });
	AddMenuItem(toolMenu, OpenFunctionsID, "Functions", [&](wxCommandEvent& ce) -> void { AddFunctionsTextCtrl(); });
	AddMenuItem(toolMenu, OpenDataID, "Data", [&](wxCommandEvent& ce) -> void { AddDataTextCtrl(); });
	AddMenuItem(toolMenu, OpenSectionsViewerID, "File sections", [&](wxCommandEvent& ce) -> void { AddFloatingPane(new SectionsGrid(this, sections, numOfSections), "File sections"); });
	AddMenuItem(toolMenu, OpenStringsMenuID, "Strings", [&](wxCommandEvent& ce) -> void { AddFloatingPane(new StringsTextCtrl(this, this), "Strings"); });
	AddMenuItem(toolMenu, OpenImportsViewerID, "Imports", [&](wxCommandEvent& ce) -> void { AddFloatingPane(new ImportsGrid(this, imports, numOfImports), "Imports"); });
	AddMenuItem(toolMenu, OpenFileHeadersMenuID, "File headers", [&](wxCommandEvent& ce) -> void { AddFloatingPane(new FileHeadersWindow(this, currentFilePath), "File headers"); });
	AddMenuItem(toolMenu, OpenCodeReferencesWindowID, "Find code references", [&](wxCommandEvent& ce) -> void { AddCodeReferencesWindow(); });
	AddMenuItem(toolMenu, OpenCalculatorMenuID, "Calculator", [&](wxCommandEvent& ce) -> void { AddFloatingPane(new CalculatorWindow(this), "Calculator"); });
	AddMenuItem(toolMenu, OpenBytesDisassemblerID, "Bytes disassembler", [&](wxCommandEvent& ce) -> void { AddFloatingPane(new BytesDisassemblerWindow(this), "Bytes disassembler"); });
	AddMenuItem(toolMenu, OpenLogID, "Log", [&](wxCommandEvent& ce) -> void { OpenLog(wxAUI_DOCK_NONE); });

	wxMenu* windowMenu = new wxMenu();
	AddMenuItem(windowMenu, ResetWindowLayoutID, "Reset window layout", [&](wxCommandEvent& ce) -> void { ResetWindowLayout(); });

	wxMenu* optionsMenu = new wxMenu();
	AddMenuItem(optionsMenu, OpenColorsMenuID, "Colors", [&](wxCommandEvent& ce) -> void { colorsMenu->OpenMenu(GetPosition()); });

	menuBar->Append(fileMenu, "File");
	menuBar->Append(toolMenu, "Tools");
	menuBar->Append(windowMenu, "Window");
	menuBar->Append(optionsMenu, "Options");
	this->SetMenuBar(menuBar);

	auiManager.SetManagedWindow(this);
	auiManager.GetArtProvider()->SetColor(wxAUI_DOCKART_INACTIVE_CAPTION_COLOUR, foregroundColor);
	auiManager.GetArtProvider()->SetColor(wxAUI_DOCKART_INACTIVE_CAPTION_COLOUR, foregroundColor);
	auiManager.GetArtProvider()->SetColor(wxAUI_DOCKART_INACTIVE_CAPTION_TEXT_COLOUR, textColor);
	auiManager.GetArtProvider()->SetMetric(wxAUI_DOCKART_GRADIENT_TYPE, wxAUI_GRADIENT_NONE);

	auiNotebook = new wxAuiNotebook(this, NotebookID, wxDefaultPosition, wxDefaultSize, wxAUI_NB_TOP | wxAUI_NB_TAB_SPLIT | wxAUI_NB_TAB_MOVE | wxAUI_NB_SCROLL_BUTTONS | wxAUI_NB_CLOSE_ON_ALL_TABS | wxAUI_NB_MIDDLE_CLICK_CLOSE);
	auiNotebook->SetArtProvider(new ColoredTabArt());

	logTextCtrl = new LogTextCtrl(this, this);

	ResetWindowLayout();
}

void MainGui::ResetWindowLayout()
{
	wxAuiPaneInfoArray& panes = auiManager.GetAllPanes();
	for (int i = 0; i < panes.size(); i++) 
	{
		auiManager.ClosePane(panes.Item(i));
		auiManager.Update();
	}

	auiNotebook->DeleteAllPages();

	disassemblyTextCtrls.clear();
	decompilationTextCtrls.clear();
	functionsTextCtrls.clear();
	dataTextCtrls.clear();

	auiManager.AddPane(auiNotebook, wxAuiPaneInfo()
		.Name("notebook")
		.Center()
		.CloseButton(false)
		.CaptionVisible(false)
		.MinSize(100, 100));

	OpenLog(wxAUI_DOCK_LEFT);
	AddDisassemblyTextCtrl();
	AddFunctionsTextCtrl();
	auiManager.Update();
}

void MainGui::AddFloatingPane(wxWindow* window, wxString caption)
{
	auiManager.AddPane(window, wxAuiPaneInfo()
		.Name(caption.Lower())
		.Caption(caption)
		.Float()
		.MinSize(window->GetMinSize()));
	auiManager.Update();
}

void MainGui::OpenLog(int direction)
{
	if (!logTextCtrl->IsShown())
	{
		wxAuiPaneInfo pane = wxAuiPaneInfo()
			.Name(logTextCtrl->GetName()
				.Lower())
			.Caption(logTextCtrl->GetName())
			.BestSize(500, -1)
			.MinSize(logTextCtrl->GetMinSize());
		if (direction == wxAUI_DOCK_NONE) 
		{
			pane.Float();
		}
		else 
		{
			pane.Direction(direction);
		}
		
		auiManager.AddPane(logTextCtrl, pane);
		auiManager.Update();
		logTextCtrl->Show();
	}
}

DisassemblyTextCtrl* MainGui::AddDisassemblyTextCtrl()
{
	DisassemblyTextCtrl* disassemblyTextCtrl = new DisassemblyTextCtrl(this, this, "Disassembly " + std::to_string(disassemblyTextCtrls.size() + 1), disassembledInstructions.data(), disassembledInstructions.size());
	disassemblyTextCtrls.push_back(disassemblyTextCtrl);
	
	colorsMenu->AddDisassemblyTextCtrl(disassemblyTextCtrl);
	
	auiNotebook->AddPage(disassemblyTextCtrl, disassemblyTextCtrl->GetName());
	auiNotebook->SetSelection(auiNotebook->GetPageIndex(disassemblyTextCtrl));

	return disassemblyTextCtrls[disassemblyTextCtrls.size() - 1];
}

DecompilationTextCtrl* MainGui::AddDecompilationTextCtrl()
{
	DecompilationTextCtrl* decompilationTextCtrl = new DecompilationTextCtrl(this, this, "Decompilation " + std::to_string(decompilationTextCtrls.size() + 1));
	decompilationTextCtrls.push_back(decompilationTextCtrl);

	colorsMenu->AddDecompilationTextCtrl(decompilationTextCtrl);

	auiNotebook->AddPage(decompilationTextCtrl, decompilationTextCtrl->GetName());
	auiNotebook->SetSelection(auiNotebook->GetPageIndex(decompilationTextCtrl));

	return decompilationTextCtrls[decompilationTextCtrls.size() - 1];
}

FunctionsTextCtrl* MainGui::AddFunctionsTextCtrl()
{
	FunctionsTextCtrl* functionsTextCtrl = new FunctionsTextCtrl(this, this, "Functions " + std::to_string(functionsTextCtrls.size() + 1));
	functionsTextCtrls.push_back(functionsTextCtrl);

	colorsMenu->AddDecompilationTextCtrl(functionsTextCtrl);

	auiManager.AddPane(functionsTextCtrl, wxAuiPaneInfo()
		.Name(functionsTextCtrl->GetName().Lower())
		.Caption(functionsTextCtrl->GetName())
		.Bottom()
		.BestSize(-1, 250)
		.MinSize(functionsTextCtrl->GetMinSize()));
	auiManager.Update();

	return functionsTextCtrls[functionsTextCtrls.size() - 1];
}

DataTextCtrl* MainGui::AddDataTextCtrl()
{
	DataTextCtrl* dataTextCtrl = new DataTextCtrl(this, this, "Data " + std::to_string(dataTextCtrls.size() + 1));
	dataTextCtrls.push_back(dataTextCtrl);

	colorsMenu->AddDataTextCtrl(dataTextCtrl);

	auiManager.AddPane(dataTextCtrl, wxAuiPaneInfo()
		.Name(dataTextCtrl->GetName().Lower())
		.Caption(dataTextCtrl->GetName())
		.Float()
		.MinSize(dataTextCtrl->GetMinSize()));
	auiManager.Update();

	return dataTextCtrls[dataTextCtrls.size() - 1];
}

CodeReferencesWindow* MainGui::AddCodeReferencesWindow()
{
	CodeReferencesWindow* codeReferencesWindow = new CodeReferencesWindow(this);

	auiManager.AddPane(codeReferencesWindow, wxAuiPaneInfo()
		.Name("find code references")
		.Caption("Find code references")
		.Float()
		.MinSize(codeReferencesWindow->GetMinSize()));
	auiManager.Update();

	return codeReferencesWindow;
}

FunctionInfoWindow* MainGui::AddFunctionInfoWindow(struct Function* function)
{
	FunctionInfoWindow* functionInfoWindow = new FunctionInfoWindow(this, disassembledInstructions.data(), function);

	auiManager.AddPane(functionInfoWindow, wxAuiPaneInfo()
		.Name(function->name.buffer)
		.Caption(wxString(function->name.buffer) + " information")
		.Float()
		.MinSize(functionInfoWindow->GetMinSize()));
	auiManager.Update();

	return functionInfoWindow;
}

void MainGui::OnPaneClose(wxAuiManagerEvent& e)
{
	wxWindow* window = e.GetPane()->window;
	if (window == logTextCtrl)
	{
		auiManager.DetachPane(window);
		auiManager.Update();
		logTextCtrl->Hide();
		return;
	}

	RemoveTextCtrl(window);
	auiManager.DetachPane(window);
	window->Destroy();
}

void MainGui::OnPageClose(wxAuiNotebookEvent& e)
{
	wxWindow* window = auiNotebook->GetPage(e.GetSelection());
	if (window == logTextCtrl)
	{
		auiNotebook->RemovePage(e.GetSelection());
		logTextCtrl->Hide();
		return;
	}
	
	RemoveTextCtrl(window);
}

void MainGui::OnTabRightClick(wxAuiNotebookEvent& e)
{
	wxMenu menu;
	const int ID_POP_OUT = 100;

	menu.Append(ID_POP_OUT, "Pop out");
	menu.Bind(wxEVT_MENU, [&](wxCommandEvent&) {
		wxWindow* window = auiNotebook->GetPage(e.GetSelection());
		wxString caption = auiNotebook->GetPageText(e.GetSelection());
		auiNotebook->RemovePage(e.GetSelection());
		AddFloatingPane(window, caption);
		}, ID_POP_OUT);

	PopupMenu(&menu, ScreenToClient(wxGetMousePosition()));
}

void MainGui::OnMouseRightClick(wxMouseEvent& e)
{
	int captionSize = auiManager.GetArtProvider()->GetMetric(wxAUI_DOCKART_CAPTION_SIZE);
	wxPoint mousePos = e.GetPosition();

	wxAuiPaneInfoArray& panes = auiManager.GetAllPanes();
	for (int i = 0; i < panes.size(); i++)
	{
		wxAuiPaneInfo& pane = panes.Item(i);
		if (mousePos.x > pane.rect.x &&
			mousePos.y > pane.rect.y - captionSize &&
			mousePos.x < pane.rect.x + pane.rect.width &&
			mousePos.y < pane.rect.y)
		{
			wxMenu menu;
			const int ID_MAKE_TAB = 100;

			menu.Append(ID_MAKE_TAB, "Make tab");
			menu.Bind(wxEVT_MENU, [&](wxCommandEvent&) {
				wxWindow* window = pane.window;
				wxString caption = pane.caption;
				auiManager.DetachPane(window);
				auiManager.Update();
				auiNotebook->AddPage(window, caption);
			}, ID_MAKE_TAB);

			PopupMenu(&menu, ScreenToClient(wxGetMousePosition()));
			break;
		}
	}
}

void MainGui::RemoveTextCtrl(wxWindow* window)
{
	for (int i = 0; i < disassemblyTextCtrls.size(); i++)
	{
		if (disassemblyTextCtrls[i] == window)
		{
			disassemblyTextCtrls.erase(disassemblyTextCtrls.begin() + i);
			break;
		}
		else if (disassemblyTextCtrls[i]->decompilationTextCtrl == window) 
		{
			disassemblyTextCtrls[i]->decompilationTextCtrl = nullptr;
			break;
		}
		else if (disassemblyTextCtrls[i]->functionsTextCtrl == window)
		{
			disassemblyTextCtrls[i]->functionsTextCtrl = nullptr;
			break;
		}
		else if (disassemblyTextCtrls[i]->dataTextCtrl == window)
		{
			disassemblyTextCtrls[i]->dataTextCtrl = nullptr;
			break;
		}
	}

	for (int i = 0; i < decompilationTextCtrls.size(); i++)
	{
		if (decompilationTextCtrls[i] == window)
		{
			decompilationTextCtrls.erase(decompilationTextCtrls.begin() + i);
			break;
		}
		else if (decompilationTextCtrls[i]->disassemblyTextCtrl == window)
		{
			decompilationTextCtrls[i]->disassemblyTextCtrl = nullptr;
			break;
		}
	}

	for (int i = 0; i < functionsTextCtrls.size(); i++)
	{
		if (functionsTextCtrls[i] == window)
		{
			functionsTextCtrls.erase(functionsTextCtrls.begin() + i);
			return;
		}
	}

	for (int i = 0; i < dataTextCtrls.size(); i++)
	{
		if (dataTextCtrls[i] == window)
		{
			dataTextCtrls.erase(dataTextCtrls.begin() + i);
			return;
		}
	}
}

void MainGui::RefreshVarNames(int functionIndex)
{
	for (int i = 0; i < decompilationTextCtrls.size(); i++) 
	{
		if (decompilationTextCtrls[i]->currentDecompiledFunc != -1) 
		{
			decompilationTextCtrls[i]->DecompileFunction(decompilationTextCtrls[i]->currentDecompiledFunc);
		}
	}

	for (int i = 0; i < functionsTextCtrls.size(); i++)
	{
		functionsTextCtrls[i]->UpdateFunctionHeader(functionIndex);
	}
}

void MainGui::AddMenuItem(wxMenu* menu, int id, const char* name, const std::function<void(wxCommandEvent&)>& function)
{
	wxMenuItem* menuItem = new wxMenuItem(0, id, name);

	// this does not work on linux
	//menuItem->SetBackgroundColour(foregroundColor);
	//menuItem->SetTextColour(textColor);
	
	menu->Append(menuItem);
	menu->Bind(wxEVT_MENU, function, id);
}

void MainGui::OpenFile()
{
	wxFileDialog openFileDialog(this, "Choose file", "", "", "", wxFD_FILE_MUST_EXIST);

	if (openFileDialog.ShowModal() != wxID_CANCEL)
	{
		wxString fileName = openFileDialog.GetPath().Mid(openFileDialog.GetPath().Last('\\') + 1);
		
		ClearData();
		SetTitle("Jesso Decompiler x64");

		wxString filePath = openFileDialog.GetPath();
		if (!filePath.empty())
		{
			numOfFileBytes = getNumOfFileBytes(filePath.c_str().AsWChar());
			if (numOfFileBytes == 0)
			{
				wxMessageBox("Error getting number of bytes in file", "Can't load data");
				return;
			}

			if (isFile64Bit(filePath.c_str().AsWChar(), &is64Bit))
			{
				logTextCtrl->Log("opened " + fileName, 0);
				if (!LoadKnownFile(filePath))
				{
					ClearData();
					logTextCtrl->Log("closed " + fileName, 0);
					return;
				}
			}
			else 
			{
				int loadAnyway = wxMessageBox("Error determining file architecture. Do you still want to load the file?", "Failed to open file", wxYES_NO, this);
				if (loadAnyway == wxYES)
				{
					logTextCtrl->Log("opened " + fileName, 0);
					if (!LoadUnknownFile(filePath))
					{
						ClearData();
						logTextCtrl->Log("closed " + fileName, 0);
						return;
					}
				}
				else
				{
					return;
				}
			}

			currentFilePath = filePath;
			SetTitle("Jesso Decompiler x64 - opened file " + fileName);

			char hexStr[20] = { 0 };
			sprintf(hexStr, "0x%llX", numOfFileBytes);
			logTextCtrl->Log("file size: " + wxString(hexStr) + " (" + std::to_string(numOfFileBytes) + ") bytes", 0);
			logTextCtrl->Log("architecture: " + (wxString)(is64Bit ? "x86-64" : "x86"), 0);
			logTextCtrl->LogHexNum("image base", imageBase, 0);
			logTextCtrl->LogHexNum("entry point", entryPoint + imageBase, 0);

			for (int i = 0; i < dataTextCtrls.size(); i++)
			{
				dataTextCtrls[i]->Initialize();
			}

			int disassembleAnswer = wxMessageBox("Do you want to disassemble the code sections?", "Disassemble code sections", wxYES_NO, this);
			if (disassembleAnswer == wxYES)
			{
				DisassembleFile();
			}
		}
		else
		{
			wxMessageBox("Error opening the file", "Failed to open file");
			return;
		}
	}

	openFileDialog.Close(true);
}

unsigned char MainGui::LoadKnownFile(wxString filePath)
{
	fileBytes = new unsigned char[numOfFileBytes];
	if (!readFileBytes(filePath.c_str().AsWChar(), fileBytes, numOfFileBytes))
	{
		wxMessageBox("Error reading bytes from file", "Can't load data");
		return 0;
	}
	
	imageBase = getFileImageBase(filePath.c_str().AsWChar(), is64Bit);
	entryPoint = getFileEntryPoint(filePath.c_str().AsWChar(), is64Bit);

	numOfSections = getNumOfSections(filePath.c_str().AsWChar(), is64Bit);
	sections = new FileSection[numOfSections];
	if (!getAllFileSectionHeaders(filePath.c_str().AsWChar(), is64Bit, sections, numOfSections))
	{
		wxMessageBox("Error getting all file sections", "Failed to open file");
		return 0;
	}

	numOfImports = getNumOfImports(filePath.c_str().AsWChar(), is64Bit);
	imports = new ImportedFunction[numOfImports];
	if (getAllImports(filePath.c_str().AsWChar(), is64Bit, imports, numOfImports) != numOfImports)
	{
		wxMessageBox("Error getting all imports", "Failed to open file");
		return 0;
	}

	return 1;
}

unsigned char MainGui::LoadUnknownFile(wxString filePath)
{
	fileBytes = new unsigned char[numOfFileBytes];
	if (!readFileBytes(filePath.c_str().AsWChar(), fileBytes, numOfFileBytes))
	{
		wxMessageBox("Error reading bytes from file", "Can't load data");
		return 0;
	}

	entryPoint = 0;
	while (1)
	{
		wxTextEntryDialog dlg(this, "", "Specify the entry point as a file offset");
		if (dlg.ShowModal() == wxID_OK)
		{
			wxString txt = dlg.GetValue();
			if (!txt.ToULongLong(&entryPoint, 16))
			{
				wxMessageBox("Not a valid hex number", "Failed to set entry point");
			}
			else if (entryPoint >= numOfFileBytes)
			{
				wxMessageBox("File offset is larger than the file", "Failed to set entry point");
			}
			else
			{
				break;
			}
		}
		else
		{
			return 0;
		}
	}
	
	int ask64Bit = wxMessageBox("Do you want to disassemble in 64 bit mode?", "Specify architecture", wxYES_NO, this);
	is64Bit = ask64Bit == wxYES;

	// this is just for formatting in the gui
	if (numOfFileBytes < 0x10) { imageBase = 0x10; }
	else if (numOfFileBytes < 0x1000) { imageBase = 0x1000; }
	else if (numOfFileBytes < 0x100000) { imageBase = 0x100000; }
	else if (numOfFileBytes < 0x10000000) { imageBase = 0x10000000; }
	else if (numOfFileBytes < 0x1000000000) { imageBase = 0x1000000000; }
	else if (numOfFileBytes < 0x100000000000) { imageBase = 0x100000000000; }
	else if (numOfFileBytes < 0x10000000000000) { imageBase = 0x10000000000000; }
	else { imageBase = 0x1000000000000000; }

	numOfSections = 1;
	sections = new FileSection[1];
	sections[0].name = initializeJdcStrWithVal(".dummyText");
	sections[0].type = CODE_FST;
	sections[0].isReadOnly = 1;
	sections[0].rva = 0;
	sections[0].fileOffset = 0;
	sections[0].physicalSize = numOfFileBytes;

	numOfImports = 0;

	return 1;
}

unsigned char CompareInstructions(const DisassembledInstruction& a, const DisassembledInstruction& b) 
{
	return a.address < b.address;
}

void MainGui::DisassembleFile()
{
	if (currentFilePath == "")
	{
		wxMessageBox("No file opened", "Can't disassemble");
		return;
	}
	if (disassembledInstructions.size() > 0)
	{
		wxMessageBox("File already disassembled", "Can't disassemble");
		return;
	}

	logTextCtrl->Log("disassembling...", 0);

	// first the instructions that are definitely executed are disassembled, then the other code sections bytes or bytes inbetween instructions are disassembled
	struct DisassemblerOptions options = { 0 };
	options.is64BitMode = is64Bit;
	struct DisassembledInstruction instructionBuffer;
	unsigned long long errorAddress = 0;
	unsigned char didErrorOccur = 0;
	if (!DisassembleTakingJumps(entryPoint + imageBase, &instructionBuffer, &options, &errorAddress))
	{
		didErrorOccur = 1;
	}

	unsigned long long firstAddress = disassembledInstructions[0].address;
	unsigned long long lastAddress = disassembledInstructions[disassembledInstructions.size() - 1].address + disassembledInstructions[disassembledInstructions.size() - 1].numOfBytes;
	for (int i = 0; i < numOfSections; i++)
	{
		if (sections[i].type == CODE_FST)
		{
			unsigned long long sectionStart = sections[i].rva + imageBase;
			unsigned long long sectionEnd = sections[i].rva + sections[i].physicalSize + imageBase;
			if (sectionEnd < firstAddress || sectionStart > lastAddress)
			{
				if (!DisassembleBetweenBounds(sectionStart, sectionEnd, &instructionBuffer, &options))
				{
					didErrorOccur = 1;
				}
			}
			else
			{
				if (sectionStart < firstAddress && sectionEnd > firstAddress)
				{
					if (!DisassembleBetweenBounds(sectionStart, firstAddress, &instructionBuffer, &options))
					{
						didErrorOccur = 1;
					}
				}

				if (sectionStart < lastAddress && sectionEnd > lastAddress)
				{
					if (!DisassembleBetweenBounds(lastAddress, sectionEnd, &instructionBuffer, &options))
					{
						didErrorOccur = 1;
					}
				}
			}
		}
	}

	std::sort(disassembledInstructions.begin(), disassembledInstructions.end(), CompareInstructions);

	int numOfInstructions = disassembledInstructions.size();
	for (int i = 0; i < numOfInstructions - 1; i++)
	{
		unsigned long long startVA = disassembledInstructions[i].address + disassembledInstructions[i].numOfBytes;
		unsigned long long endVA = disassembledInstructions[i + 1].address;
		if (startVA != endVA)
		{
			if (!DisassembleBetweenBounds(startVA, endVA, &instructionBuffer, &options))
			{
				didErrorOccur = 1;
			}
		}
	}

	std::sort(disassembledInstructions.begin(), disassembledInstructions.end(), CompareInstructions);

	if (didErrorOccur) 
	{
		wxMessageBox("An error occured while disassembling", "Disassembly not fully completed");
	}

	decompParams.imports = imports;
	decompParams.numOfImports = numOfImports;

	decompParams.instructions = disassembledInstructions.data();
	decompParams.numOfInstructions = disassembledInstructions.size();

	decompParams.imageBase = imageBase;
	decompParams.sections = sections;
	decompParams.numOfSections = numOfSections;

	decompParams.fileBytes = fileBytes;
	decompParams.numOfFileBytes = numOfFileBytes;

	decompParams.is64Bit = is64Bit;

	logTextCtrl->Log("updating disassembly GUI...", 0);

	for (int i = 0; i < disassemblyTextCtrls.size(); i++)
	{
		disassemblyTextCtrls[i]->Initialize(disassembledInstructions.data(), disassembledInstructions.size(), errorAddress);
	}

	logTextCtrl->Log("finished disassembling", 0);

	int answer = wxMessageBox("Do you want to analyze the file?", "Analyze file", wxYES_NO, this);
	if (answer == wxYES)
	{
		AnalyzeFile();
	}
}

void MainGui::AnalyzeFile() 
{
	if (currentFilePath == "")
	{
		wxMessageBox("No file opened", "Can't analyze");
		return;
	}
	if (disassembledInstructions.size() == 0) 
	{
		wxMessageBox("File not disassembled", "Can't analyze");
		return;
	}
	if (functions.size() > 0)
	{
		wxMessageBox("File already analyzed", "Can't analyze");
		return;
	}

	logTextCtrl->Log("finding all functions...", 0);

	int getSymbols = wxMessageBox("Do you want to look for function name symbols? This could take some time.", "Get function name symbols", wxYES_NO, this);
	FindAllFunctions(getSymbols == wxYES);

	logTextCtrl->Log("analyzing functions...", 0);

	if (!analyzeAllFunctions(&decompParams))
	{
		wxMessageBox("Error analyzing functions", "Failed to analyze functions");
	}
	
	logTextCtrl->Log("updating functions GUI...", 0);

	for (int i = 0; i < functionsTextCtrls.size(); i++)
	{
		functionsTextCtrls[i]->ShowAllFunctions(-1);
	}

	logTextCtrl->Log("finished analyzing file", 0);

	decompParams.currentFunc = 0;
}

void MainGui::ClearData()
{
	if (fileBytes)
	{
		delete[] fileBytes;
		fileBytes = 0;
	}

	memset(&decompParams, 0, sizeof(decompParams));

	for (int i = 0; i < dataTextCtrls.size(); i++) 
	{
		dataTextCtrls[i]->ClearText();
	}
	for (int i = 0; i < disassemblyTextCtrls.size(); i++)
	{
		disassemblyTextCtrls[i]->ClearData();
	}
	for (int i = 0; i < decompilationTextCtrls.size(); i++)
	{
		decompilationTextCtrls[i]->ClearText();
	}
	for (int i = 0; i < functionsTextCtrls.size(); i++)
	{
		functionsTextCtrls[i]->ClearText();
	}

	int numOfInstructions = disassembledInstructions.size();
	for (int i = 0; i < numOfInstructions; i++)
	{
		free(disassembledInstructions[i].operands);
	}

	disassembledInstructions.clear();
	disassembledInstructions.shrink_to_fit();

	for (int i = 0; i < numOfSections; i++)
	{
		freeJdcStr(&sections[i].name);
	}
	if (sections)
	{
		delete[] sections;
		sections = 0;
	}

	for (int i = 0; i < numOfImports; i++) 
	{
		freeJdcStr(&imports[i].name);
	}
	if (imports)
	{
		delete[] imports;
		imports = 0;
	}
	
	for (int i = 0; i < functions.size(); i++)
	{
		freeFunction(&functions[i]);
	}

	functions.clear();
	functions.shrink_to_fit();

	currentFilePath = "";
}

unsigned char MainGui::DisassembleTakingJumps(unsigned long long startVA, struct DisassembledInstruction* instructionBuffer, struct DisassemblerOptions* options, unsigned long long* errorAddress)
{
	struct FileSection* currentSection = 0;
	unsigned long long currentFileOffset = rvaToFileOffset(sections, numOfSections, startVA - imageBase, &currentSection);
	if (!currentSection || currentSection->type != CODE_FST)
	{
		return 1;
	}
	else if (currentFileOffset >= numOfFileBytes || currentSection->fileOffset + currentSection->physicalSize > numOfFileBytes)
	{
		logTextCtrl->LogHexNum("instruction jumps outside file", instructionBuffer->address, 1);
		return 0;
	}

	unsigned char storeInstruction = 1;

	unsigned long long currentVirtualAddress = startVA;
	while (currentFileOffset < currentSection->fileOffset + currentSection->physicalSize)
	{
		if (findInstructionByAddress(disassembledInstructions.data(), disassembledInstructions.size(), currentVirtualAddress) != -1)
		{
			return 1;
		}

		int instructionIndex = findInstructionInsertPoint(disassembledInstructions.data(), disassembledInstructions.size(), currentVirtualAddress);
		if (instructionIndex > 0 && instructionIndex < disassembledInstructions.size() &&
			currentVirtualAddress > disassembledInstructions[instructionIndex - 1].address &&
			currentVirtualAddress < disassembledInstructions[instructionIndex - 1].address + disassembledInstructions[instructionIndex - 1].numOfBytes)
		{
			logTextCtrl->LogHexNum("instruction overlaps with existing instruction", currentVirtualAddress, 1);
			return 0;
		}

		if (!disassembleInstruction(&fileBytes[currentFileOffset], fileBytes + currentSection->fileOffset + currentSection->physicalSize - 1, options, instructionBuffer))
		{
			if (errorAddress) { *errorAddress = currentVirtualAddress; }
			logTextCtrl->LogHexNum("bad instruction at", currentVirtualAddress, 1);
			return 0;
		}

		instructionBuffer->address = currentVirtualAddress;

		currentFileOffset += instructionBuffer->numOfBytes;
		currentVirtualAddress += instructionBuffer->numOfBytes;

		if (storeInstruction) // only the instructions that are jumped to are stored because inserting into the vector takes too much time
		{
			// this needs to be sorted here because the find instruction functions use a binary search
			disassembledInstructions.insert(disassembledInstructions.begin() + instructionIndex, *instructionBuffer);
			storeInstruction = 0;
		}

		unsigned long long jmpDst = 0;
		unsigned char stop = 0;
		if (checkForControlFlowJump(instructionBuffer, &jmpDst, &stop))
		{
			if (jmpDst == 0)
			{
				continue;
			}

			if (jmpDst != instructionBuffer->address)
			{
				if (stop)
				{
					struct FileSection* section = 0;
					currentFileOffset = rvaToFileOffset(sections, numOfSections, jmpDst - imageBase, &section);
					if (!section || section->type != CODE_FST)
					{
						return 1;
					}

					currentVirtualAddress = jmpDst;
					storeInstruction = 1;
				}
				else if (!DisassembleTakingJumps(jmpDst, instructionBuffer, options, errorAddress))
				{
					return 0;
				}
			}
			else if(stop)
			{
				return 1;
			}
		}
	}

	return 1;
}

unsigned char MainGui::DisassembleBetweenBounds(unsigned long long startVA, unsigned long long endVA, struct DisassembledInstruction* instructionBuffer, struct DisassemblerOptions* options)
{
	struct FileSection* currentSection = 0;
	unsigned long long currentFileOffset = rvaToFileOffset(sections, numOfSections, startVA - imageBase, &currentSection);
	if (!currentSection || currentSection->type != CODE_FST)
	{
		return 1;
	}
	else if (currentFileOffset >= numOfFileBytes)
	{
		return 0;
	}

	unsigned long long endFileOffset = rvaToFileOffset(sections, numOfSections, endVA - imageBase, 0);
	if (endFileOffset > numOfFileBytes || endFileOffset == 0)
	{
		return 0;
	}

	unsigned long long currentVirtualAddress = startVA;
	while (currentFileOffset < endFileOffset)
	{
		if (!disassembleInstruction(&fileBytes[currentFileOffset], fileBytes + endFileOffset - 1, options, instructionBuffer) || 
			currentFileOffset + instructionBuffer->numOfBytes > endFileOffset)
		{
			int numOfBytes = instructionBuffer->numOfBytes;
			if (numOfBytes == 0) 
			{
				return 0;
			}

			memset(instructionBuffer, 0, sizeof(struct DisassembledInstruction));
			instructionBuffer->opcode = DATA;
			instructionBuffer->numOfBytes = 1;

			for (int i = 0; i < numOfBytes; i++) 
			{
				if (currentFileOffset >= endFileOffset)
				{
					return 1;
				}
				
				instructionBuffer->address = currentVirtualAddress;
				instructionBuffer->operands = (struct Operand*)calloc(1, sizeof(struct Operand));
				if (!instructionBuffer->operands) 
				{
					return 0;
				}

				instructionBuffer->operands[0].immediate.value = fileBytes[currentFileOffset];
				instructionBuffer->operands[0].type = IMMEDIATE;
				instructionBuffer->operands[0].immediate.size = 1;
				instructionBuffer->numOfOperands = 1;
				
				currentFileOffset++;
				currentVirtualAddress++;
				disassembledInstructions.push_back(*instructionBuffer);
			}
		}
		else 
		{
			instructionBuffer->address = currentVirtualAddress;
			
			currentFileOffset += instructionBuffer->numOfBytes;
			currentVirtualAddress += instructionBuffer->numOfBytes;
			disassembledInstructions.push_back(*instructionBuffer);
		}
	}

	return 1;
}

void MainGui::FindAllFunctions(unsigned char getSymbols) 
{
	int numOfInstructions = disassembledInstructions.size();
	int instructionIndex = 0;

	int codeSectionIndex = 0;
	for (int i = 0; i < numOfSections; i++) 
	{
		if (sections[i].type == CODE_FST) 
		{
			codeSectionIndex = i;
			break;
		}
	}
	unsigned long long currentSectionEndAddress = imageBase + sections[codeSectionIndex].rva + sections[codeSectionIndex].physicalSize - 1;

	std::vector<unsigned long long> calledAddresses;
	calledAddresses.push_back(imageBase + entryPoint);
	for (int i = 0; i < numOfInstructions; i++) 
	{
		if (disassembledInstructions[i].opcode == CALL_NEAR) 
		{
			unsigned long long address = resolveJmpChain(&decompParams, i);
			if (findAddressInArr(calledAddresses.data(), calledAddresses.size(), address) == -1)
			{
				calledAddresses.insert(std::lower_bound(calledAddresses.begin(), calledAddresses.end(), address), address); // sorting it
			}
		}
	}

	struct Function currentFunction;
	memset(&currentFunction, 0, sizeof(struct Function));
	int numOfFunctions = 0;
	while (instructionIndex < numOfInstructions && findNextFunction(&decompParams, currentSectionEndAddress, &calledAddresses[0], calledAddresses.size(), &currentFunction, &instructionIndex))
	{
		if (disassembledInstructions[instructionIndex].address > currentSectionEndAddress)
		{
			unsigned foundNextCodeSection = 0;
			for (int i = codeSectionIndex + 1; i < numOfSections; i++)
			{
				if (sections[i].type == CODE_FST)
				{
					codeSectionIndex = i;
					foundNextCodeSection = 1;
					break;
				}
			}

			if (foundNextCodeSection)
			{
				currentSectionEndAddress = imageBase + sections[codeSectionIndex].rva + sections[codeSectionIndex].physicalSize - 1;
			}
		}

		currentFunction.name = initializeJdcStr();
		if (!getSymbols || !getSymbolByValue(currentFilePath.c_str().AsWChar(), is64Bit, disassembledInstructions[currentFunction.firstInstructionIndex].address, &currentFunction.name))
		{
			sprintfJdc(&currentFunction.name, 0, "func%llX", disassembledInstructions[currentFunction.firstInstructionIndex].address - imageBase);
		}

		functions.push_back(currentFunction);
		memset(&currentFunction, 0, sizeof(struct Function));
		numOfFunctions++;
	}

	decompParams.functions = functions.data();
	decompParams.numOfFunctions = numOfFunctions;
	decompParams.currentFunc = 0;
}

void MainGui::CloseApp(wxCloseEvent& e)
{
	auiManager.UnInit();
	colorsMenu->Destroy();
	Destroy();
}