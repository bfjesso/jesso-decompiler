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
#include "../save-handler/saveHandler.h"
#include "../save-handler/loadHandler.h"

wxBEGIN_EVENT_TABLE(MainGui, wxFrame)
EVT_CLOSE(MainGui::CloseApp)
EVT_AUI_PANE_CLOSE(MainGui::OnPaneClose)
EVT_AUINOTEBOOK_PAGE_CLOSE(NotebookID, MainGui::OnPageClose)
EVT_AUINOTEBOOK_TAB_RIGHT_DOWN(NotebookID, MainGui::OnTabRightClick)
EVT_RIGHT_DOWN(MainGui::OnMouseRightClick)
wxEND_EVENT_TABLE()

MainGui::MainGui() : wxFrame(nullptr, wxID_ANY, "Jesso Decompiler x64")
{
	SetMinSize(wxSize(800, 600));

	SetOwnBackgroundColour(backgroundColor);

	colorsMenu = new ColorsMenu();
	settingsWindow = new SettingsWindow(this, this);
	settingsWindow->LoadPreferences();

	menuBar = new wxMenuBar();

	wxMenu* fileMenu = new wxMenu();
	AddMenuItem(fileMenu, OpenFileID, "Open file", [&](wxCommandEvent& ce) -> void { OpenFile(); });
	AddMenuItem(fileMenu, SaveFileID, "Save file", [&](wxCommandEvent& ce) -> void { SaveFile(); });
	AddMenuItem(fileMenu, DisassembleFileID, "Disassemble file", [&](wxCommandEvent& ce) -> void { DisassembleFile(); });
	AddMenuItem(fileMenu, AnalyzeFileID, "Analyze file", [&](wxCommandEvent& ce) -> void { AnalyzeFile(); });

	wxMenu* toolMenu = new wxMenu();
	AddMenuItem(toolMenu, OpenDisassemblyID, "Disassembly", [&](wxCommandEvent& ce) -> void { AddDisassemblyTextCtrl(); });
	AddMenuItem(toolMenu, OpenDecompilationID, "Decompilation", [&](wxCommandEvent& ce) -> void { AddDecompilationTextCtrl(); });
	AddMenuItem(toolMenu, OpenFunctionsID, "Functions", [&](wxCommandEvent& ce) -> void { AddFunctionsTextCtrl(); });
	AddMenuItem(toolMenu, OpenDataID, "Data", [&](wxCommandEvent& ce) -> void { AddDataTextCtrl(); });
	AddMenuItem(toolMenu, OpenSectionsViewerID, "File sections", [&](wxCommandEvent& ce) -> void { AddFloatingPane(new SectionsGrid(this, sections, numOfSections), "File sections"); });
	AddMenuItem(toolMenu, OpenStringsMenuID, "Strings", [&](wxCommandEvent& ce) -> void { AddFloatingPane(new StringsTextCtrl(this, this), "Strings"); });
	AddMenuItem(toolMenu, OpenImportsViewerID, "Imports", [&](wxCommandEvent& ce) -> void { AddFloatingPane(new ImportsGrid(this, imports, numOfImports, libraryNames, numOfLibraries), "Imports"); });
	AddMenuItem(toolMenu, OpenFileHeadersMenuID, "File headers", [&](wxCommandEvent& ce) -> void { AddFloatingPane(new FileHeadersWindow(this, currentFilePath, fileFormat), "File headers"); });
	AddMenuItem(toolMenu, OpenCodeReferencesWindowID, "Find code references", [&](wxCommandEvent& ce) -> void { AddCodeReferencesWindow(); });
	AddMenuItem(toolMenu, OpenCalculatorMenuID, "Calculator", [&](wxCommandEvent& ce) -> void { AddFloatingPane(new CalculatorWindow(this), "Calculator"); });
	AddMenuItem(toolMenu, OpenBytesDisassemblerID, "Bytes disassembler", [&](wxCommandEvent& ce) -> void { AddFloatingPane(new BytesDisassemblerWindow(this, settingsWindow), "Bytes disassembler"); });
	AddMenuItem(toolMenu, OpenLogID, "Log", [&](wxCommandEvent& ce) -> void { ShowWindowInAUI(wxAUI_DOCK_NONE, logTextCtrl); });

	wxMenu* windowMenu = new wxMenu();
	AddMenuItem(windowMenu, ResetWindowLayoutID, "Reset window layout", [&](wxCommandEvent& ce) -> void { ResetWindowLayout(); });

	wxMenu* optionsMenu = new wxMenu();
	AddMenuItem(optionsMenu, OpenColorsMenuID, "Colors", [&](wxCommandEvent& ce) -> void { colorsMenu->OpenMenu(GetPosition()); });
	AddMenuItem(optionsMenu, OpenSettingsID, "Settings", [&](wxCommandEvent& ce) -> void { ShowWindowInAUI(wxAUI_DOCK_NONE, settingsWindow); });

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
	auiNotebook->SetArtProvider(new JdcTabArt());

	logTextCtrl = new LogTextCtrl(this, this);

	ResetWindowLayout();
}

void MainGui::ResetWindowLayout()
{
	wxAuiPaneInfoArray& panes = auiManager.GetAllPanes();
	for (int32_t i = 0; i < panes.size(); i++)
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

	ShowWindowInAUI(wxAUI_DOCK_LEFT, logTextCtrl);
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

void MainGui::ShowWindowInAUI(int32_t direction, wxWindow* window)
{
	if (!window->IsShown())
	{
		wxAuiPaneInfo pane = wxAuiPaneInfo()
			.Name(window->GetName()
				.Lower())
			.Caption(window->GetName())
			.BestSize(500, -1)
			.MinSize(window->GetMinSize());
		if (direction == wxAUI_DOCK_NONE)
		{
			pane.Float();
		}
		else
		{
			pane.Direction(direction);
		}

		auiManager.AddPane(window, pane);
		auiManager.Update();
		window->Show();
	}
}

DisassemblyTextCtrl* MainGui::AddDisassemblyTextCtrl()
{
	highestDisassemblyTextCtrlNum++;
	DisassemblyTextCtrl* disassemblyTextCtrl = new DisassemblyTextCtrl(this, this, "Disassembly " + wxString::Format("%d", highestDisassemblyTextCtrlNum), disassembledInstructions.data(), disassembledInstructions.size());
	disassemblyTextCtrls.push_back(disassemblyTextCtrl);

	colorsMenu->AddDisassemblyTextCtrl(disassemblyTextCtrl);

	auiNotebook->AddPage(disassemblyTextCtrl, disassemblyTextCtrl->GetName());
	auiNotebook->SetSelection(auiNotebook->GetPageIndex(disassemblyTextCtrl));

	return disassemblyTextCtrls[disassemblyTextCtrls.size() - 1];
}

DecompilationTextCtrl* MainGui::AddDecompilationTextCtrl()
{
	highestDecompilationTextCtrlNum++;
	DecompilationTextCtrl* decompilationTextCtrl = new DecompilationTextCtrl(this, this, "Decompilation " + wxString::Format("%d", highestDecompilationTextCtrlNum));
	decompilationTextCtrls.push_back(decompilationTextCtrl);

	colorsMenu->AddDecompilationTextCtrl(decompilationTextCtrl);

	auiNotebook->AddPage(decompilationTextCtrl, decompilationTextCtrl->GetName());
	auiNotebook->SetSelection(auiNotebook->GetPageIndex(decompilationTextCtrl));

	return decompilationTextCtrls[decompilationTextCtrls.size() - 1];
}

FunctionsTextCtrl* MainGui::AddFunctionsTextCtrl()
{
	highestFunctionsTextCtrlNum++;
	FunctionsTextCtrl* functionsTextCtrl = new FunctionsTextCtrl(this, this, "Functions " + wxString::Format("%d", highestFunctionsTextCtrlNum));
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
	highestDataTextCtrlNum++;
	DataTextCtrl* dataTextCtrl = new DataTextCtrl(this, this, "Data " + wxString::Format("%d", highestDataTextCtrlNum));
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
	FunctionInfoWindow* functionInfoWindow = new FunctionInfoWindow(this, this, disassembledInstructions.data(), function);

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
	if (window == logTextCtrl || window == settingsWindow)
	{
		if (window == settingsWindow)
		{
			settingsWindow->CheckUnappliedChanges();
		}

		auiManager.DetachPane(window);
		auiManager.Update();
		window->Hide();
		return;
	}

	RemoveTextCtrl(window);
	auiManager.DetachPane(window);
	window->Destroy();
}

void MainGui::OnPageClose(wxAuiNotebookEvent& e)
{
	wxWindow* window = auiNotebook->GetPage(e.GetSelection());
	if (window == logTextCtrl || window == settingsWindow)
	{
		if (window == settingsWindow)
		{
			settingsWindow->CheckUnappliedChanges();
		}

		auiNotebook->RemovePage(e.GetSelection());
		window->Hide();
		return;
	}

	RemoveTextCtrl(window);
}

void MainGui::OnTabRightClick(wxAuiNotebookEvent& e)
{
	wxMenu menu;
	const int32_t ID_POP_OUT = 100;

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
	int32_t captionSize = auiManager.GetArtProvider()->GetMetric(wxAUI_DOCKART_CAPTION_SIZE);
	wxPoint mousePos = e.GetPosition();

	wxAuiPaneInfoArray& panes = auiManager.GetAllPanes();
	for (int32_t i = 0; i < panes.size(); i++)
	{
		wxAuiPaneInfo& pane = panes.Item(i);
		if (mousePos.x > pane.rect.x &&
			mousePos.y > pane.rect.y - captionSize &&
			mousePos.x < pane.rect.x + pane.rect.width &&
			mousePos.y < pane.rect.y)
		{
			wxMenu menu;
			const int32_t ID_MAKE_TAB = 100;

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
	for (int32_t i = 0; i < disassemblyTextCtrls.size(); i++)
	{
		if (disassemblyTextCtrls[i] == window)
		{
			disassemblyTextCtrls.erase(disassemblyTextCtrls.begin() + i);
			if (disassemblyTextCtrls.size() == 0)
			{
				highestDisassemblyTextCtrlNum = 0;
			}
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

	for (int32_t i = 0; i < decompilationTextCtrls.size(); i++)
	{
		if (decompilationTextCtrls[i] == window)
		{
			decompilationTextCtrls.erase(decompilationTextCtrls.begin() + i);
			if (decompilationTextCtrls.size() == 0)
			{
				highestDecompilationTextCtrlNum = 0;
			}
			break;
		}
		else if (decompilationTextCtrls[i]->disassemblyTextCtrl == window)
		{
			decompilationTextCtrls[i]->disassemblyTextCtrl = nullptr;
			break;
		}
	}

	for (int32_t i = 0; i < functionsTextCtrls.size(); i++)
	{
		if (functionsTextCtrls[i] == window)
		{
			functionsTextCtrls.erase(functionsTextCtrls.begin() + i);
			if (functionsTextCtrls.size() == 0)
			{
				highestFunctionsTextCtrlNum = 0;
			}
			return;
		}
	}

	for (int32_t i = 0; i < dataTextCtrls.size(); i++)
	{
		if (dataTextCtrls[i] == window)
		{
			dataTextCtrls.erase(dataTextCtrls.begin() + i);
			if (dataTextCtrls.size() == 0)
			{
				highestDataTextCtrlNum = 0;
			}
			return;
		}
	}
}

void MainGui::RefreshVarNames(int32_t functionIndex)
{
	for (int32_t i = 0; i < decompilationTextCtrls.size(); i++)
	{
		if (decompilationTextCtrls[i]->currentDecompiledFunc != -1)
		{
			decompilationTextCtrls[i]->DecompileFunction(decompilationTextCtrls[i]->currentDecompiledFunc);
		}
	}

	for (int32_t i = 0; i < functionsTextCtrls.size(); i++)
	{
		functionsTextCtrls[i]->UpdateFunctionHeader(functionIndex);
	}
}

void MainGui::UpdateSettings()
{
	UpdateDecompParams();

	for (int32_t i = 0; i < disassemblyTextCtrls.size(); i++)
	{
		disassemblyTextCtrls[i]->ResetDisassembly();
	}

	for (int32_t i = 0; i < decompilationTextCtrls.size(); i++)
	{
		if (decompilationTextCtrls[i]->currentDecompiledFunc != -1)
		{
			decompilationTextCtrls[i]->DecompileFunction(decompilationTextCtrls[i]->currentDecompiledFunc);
		}
	}

	for (int32_t i = 0; i < functionsTextCtrls.size(); i++)
	{
		functionsTextCtrls[i]->ResetFunctionsList();
	}
}

void MainGui::AddMenuItem(wxMenu* menu, int32_t id, const char* name, const std::function<void(wxCommandEvent&)>& function)
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
		OpenFileFromPath(openFileDialog.GetPath());
	}

	openFileDialog.Close(true);
}

void MainGui::OpenFileFromPath(wxString filePath)
{
	ClearData();
	SetTitle("Jesso Decompiler x64");

	if (!wxFileExists(filePath))
	{
		wxMessageBox("\"" + filePath + "\" does not exist", "Can't load file");
		return;
	}

	if (!filePath.empty())
	{
		bool isJdcSave = false;
		if (ERROR_JDC == isFileJdcSave(filePath.c_str().AsWChar(), &isJdcSave))
		{
			wxMessageBox("Error checking if file is a JDC save", "Can't load file");
			return;
		}

		if (isJdcSave)
		{
			uint32_t saveVersion = 0;
			if (ERROR_JDC == getJdcSaveVersion(filePath.c_str().AsWChar(), &saveVersion))
			{
				wxMessageBox("Error getting save file version", "Can't load file");
				return;
			}

			if (saveVersion != CURRENT_SAVE_FILE_VERSION)
			{
				wxMessageBox("This save file is a different version than the current", "Can't load file");
				return;
			}

			int32_t numOfFunctions = 0;
			if (ERROR_JDC == getNumOfFuncsFromJdcSaveFile(filePath.c_str().AsWChar(), &numOfFunctions))
			{
				wxMessageBox("Error getting number of functions from save", "Can't load file");
				return;
			}

			functions.resize(numOfFunctions);
			if (ERROR_JDC == getAllFunctionsFromJdcSaveFile(filePath.c_str().AsWChar(), functions.data()))
			{
				wxMessageBox("Error getting functions from save", "Can't load file");
				return;
			}

			UpdateDecompParams();

			logTextCtrl->Log("loaded analyzed functions from save at " + filePath);

			size_t filePathLen = 0;
			if (ERROR_JDC == getJdcSaveFilePathLen(filePath.c_str().AsWChar(), &filePathLen))
			{
				wxMessageBox("Error getting path length of analyzed file", "Can't load file");
				return;
			}

			wchar_t* analyzedFilePath = new wchar_t[filePathLen];
			if (ERROR_JDC == getJdcSaveFilePath(filePath.c_str().AsWChar(), analyzedFilePath, filePathLen))
			{
				wxMessageBox("Error getting path of analyzed file", "Can't load file");
				return;
			}

			filePath = wxString(analyzedFilePath);
			delete[] analyzedFilePath;
		}

		if (ERROR_JDC == getNumOfFileBytes(filePath.c_str().AsWChar(), &numOfFileBytes) || numOfFileBytes == 0)
		{
			wxMessageBox("Error getting number of bytes in file", "Can't load data");
			return;
		}

		if (ERROR_JDC == identifyFileFormat(filePath.c_str().AsWChar(), &fileFormat))
		{
			wxMessageBox("Error identifying file format", "Failed to open file");
			return;
		}

		wxString fileName = filePath.Mid(filePath.Last('\\') + 1);

		if (fileFormat != UNKNOWN_FF)
		{
			logTextCtrl->Log("opened " + fileName);
			if (ERROR_JDC == LoadKnownFile(filePath))
			{
				ClearData();
				logTextCtrl->Log("closed " + fileName);
				return;
			}
		}
		else
		{
			int32_t loadAnyway = wxMessageBox("The file is in an unknown format. Do you still want to load the file?", "Failed to open file", wxYES_NO, this);
			if (loadAnyway == wxYES)
			{
				logTextCtrl->Log("opened " + fileName);
				if (ERROR_JDC == LoadUnknownFile(filePath))
				{
					ClearData();
					logTextCtrl->Log("closed " + fileName);
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

		logTextCtrl->Log(wxString::Format("file size: %lld (0x%llX) bytes", numOfFileBytes, numOfFileBytes));
		logTextCtrl->Log("file format: " + wxString(fileFormatToStr(fileFormat)));
		logTextCtrl->Log("architecture: " + (wxString)(is64Bit ? "x86-64" : "x86"));
		logTextCtrl->Log(wxString::Format("image base: 0x%llX", imageBase));
		logTextCtrl->Log(wxString::Format("entry point: 0x%llX", entryPoint + imageBase));

		for (int32_t i = 0; i < dataTextCtrls.size(); i++)
		{
			dataTextCtrls[i]->Initialize();
		}

		if (isJdcSave)
		{
			DisassembleFile();

			for (int32_t i = 0; i < functionsTextCtrls.size(); i++)
			{
				functionsTextCtrls[i]->ShowAllFunctions(-1);
			}
		}
		else
		{
			int32_t disassembleAnswer = wxMessageBox("Do you want to disassemble the code sections?", "Disassemble code sections", wxYES_NO, this);
			if (disassembleAnswer == wxYES)
			{
				DisassembleFile();
			}
		}

	}
	else
	{
		wxMessageBox("Error opening the file", "Failed to open file");
		return;
	}
}

void MainGui::SaveFile()
{
	if (functions.size() == 0)
	{
		wxMessageBox("No file has been analyzed", "Nothing to save");
		return;
	}

	if (saveToFile(currentFilePath.c_str().AsWChar(), functions.data(), functions.size()) == ERROR_JDC)
	{
		wxMessageBox("Error saving to file", "Failed to save to file");
		return;
	}

	wxMessageBox("Analysis data has been saved to file", "Saved to file");
	return;
}

enum JdcStatus MainGui::LoadKnownFile(wxString filePath)
{
	if (ERROR_JDC == isFile64Bit(filePath.c_str().AsWChar(), fileFormat, &is64Bit))
	{
		wxMessageBox("Error determining file architecture", "Can't load file");
		return ERROR_JDC;
	}

	fileBytes = new uint8_t[numOfFileBytes];
	if (ERROR_JDC == readFileBytes(filePath.c_str().AsWChar(), fileBytes, numOfFileBytes))
	{
		wxMessageBox("Error reading bytes from file", "Can't load file");
		return ERROR_JDC;
	}

	if (ERROR_JDC == getFileImageBase(filePath.c_str().AsWChar(), fileFormat, is64Bit, &imageBase))
	{
		wxMessageBox("Error getting image base", "Can't load file");
		return ERROR_JDC;
	}

	if (ERROR_JDC == getFileEntryPoint(filePath.c_str().AsWChar(), fileFormat, is64Bit, &entryPoint))
	{
		wxMessageBox("Error getting entry point", "Can't load file");
		return ERROR_JDC;
	}

	if (ERROR_JDC == getNumOfSections(filePath.c_str().AsWChar(), fileFormat, is64Bit, &numOfSections))
	{
		wxMessageBox("Error getting number of file sections", "Can't load file");
		return ERROR_JDC;
	}

	sections = new FileSection[numOfSections];
	if (ERROR_JDC == getAllFileSectionHeaders(filePath.c_str().AsWChar(), fileFormat, is64Bit, sections, numOfSections))
	{
		wxMessageBox("Error getting all file sections", "Failed to open file");
		return ERROR_JDC;
	}

	if (ERROR_JDC == getNumOfImports(filePath.c_str().AsWChar(), fileFormat, is64Bit, &numOfImports, &numOfLibraries))
	{
		wxMessageBox("Error getting number of imports", "Failed to open file");
		return ERROR_JDC;
	}

	imports = new ImportedFunction[numOfImports];
	libraryNames = new JdcStr[numOfLibraries];
	if (ERROR_JDC == getAllImports(filePath.c_str().AsWChar(), fileFormat, is64Bit, imports, numOfImports, libraryNames, numOfLibraries))
	{
		wxMessageBox("Error getting all imports", "Failed to open file");
		return ERROR_JDC;
	}

	return SUCCESS_JDC;
}

enum JdcStatus MainGui::LoadUnknownFile(wxString filePath)
{
	fileBytes = new uint8_t[numOfFileBytes];
	if (ERROR_JDC == readFileBytes(filePath.c_str().AsWChar(), fileBytes, numOfFileBytes))
	{
		wxMessageBox("Error reading bytes from file", "Can't load data");
		return ERROR_JDC;
	}

	entryPoint = 0;
	while (1)
	{
		wxTextEntryDialog dlg(this, "", "Specify the entry point as a file offset");
		if (dlg.ShowModal() == wxID_OK)
		{
			wxString txt = dlg.GetValue();
			if (!txt.ToULongLong((wxULongLong_t*)(&entryPoint), 16))
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
			return ERROR_JDC;
		}
	}

	int32_t ask64Bit = wxMessageBox("Do you want to treat the file as 64-bit?", "Specify architecture", wxYES_NO, this);
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

	return SUCCESS_JDC;
}

bool CompareInstructions(const DisassembledInstruction& a, const DisassembledInstruction& b)
{
	return a.address < b.address;
}

bool CompareJumpTables(const JumpTable& a, const JumpTable& b)
{
	return a.jmpTableAddress < b.jmpTableAddress;
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

	UpdateDecompParams();

	// first the instructions that are definitely executed are disassembled, then the other code sections bytes or bytes inbetween instructions are disassembled
	logTextCtrl->Log("disassembling jumped to instructions...");
	logTextCtrl->Log("\tCurrent instruction: ", false, false);
	logTextCtrl->LogProgress(0, false);

	struct DisassemblerOptions options = { 0 };
	options.is64BitMode = is64Bit;
	struct DisassembledInstruction instructionBuffer;
	uint64_t errorAddress = 0;
	bool didErrorOccur = false;
	if (ERROR_JDC == DisassembleTakingJumps(entryPoint + imageBase, &instructionBuffer, &options, &errorAddress, 0))
	{
		didErrorOccur = true;
	}

	if (disassembledInstructions.size() == 0)
	{
		wxMessageBox("An error occured while disassembling", "Disassembly not fully completed");
		return;
	}

	logTextCtrl->Log("disassembling all other instructions...");
	logTextCtrl->Log("\tCurrent instruction: ", false, false);
	logTextCtrl->LogProgress(0, false);

	uint64_t firstAddress = disassembledInstructions[0].address;
	uint64_t lastAddress = disassembledInstructions[disassembledInstructions.size() - 1].address + disassembledInstructions[disassembledInstructions.size() - 1].numOfBytes;
	for (int32_t i = 0; i < numOfSections; i++)
	{
		if (sections[i].type == CODE_FST)
		{
			uint64_t sectionStart = sections[i].rva + imageBase;
			uint64_t sectionEnd = sections[i].rva + sections[i].physicalSize + imageBase;
			if (sectionEnd < firstAddress || sectionStart > lastAddress)
			{
				if (ERROR_JDC == DisassembleBetweenBounds(sectionStart, sectionEnd, &instructionBuffer, &options))
				{
					didErrorOccur = true;
				}
			}
			else
			{
				if (sectionStart < firstAddress && sectionEnd > firstAddress)
				{
					if (ERROR_JDC == DisassembleBetweenBounds(sectionStart, firstAddress, &instructionBuffer, &options))
					{
						didErrorOccur = true;
					}
				}

				if (sectionStart < lastAddress && sectionEnd > lastAddress)
				{
					if (ERROR_JDC == DisassembleBetweenBounds(lastAddress, sectionEnd, &instructionBuffer, &options))
					{
						didErrorOccur = true;
					}
				}
			}
		}
	}

	std::sort(disassembledInstructions.begin(), disassembledInstructions.end(), CompareInstructions);

	int32_t numOfInstructions = disassembledInstructions.size();
	for (int32_t i = 0; i < numOfInstructions - 1; i++)
	{
		uint64_t startVA = disassembledInstructions[i].address + disassembledInstructions[i].numOfBytes;
		uint64_t endVA = disassembledInstructions[i + 1].address;
		if (startVA != endVA)
		{
			if (ERROR_JDC == DisassembleBetweenBounds(startVA, endVA, &instructionBuffer, &options))
			{
				didErrorOccur = true;
			}
		}
	}

	std::sort(disassembledInstructions.begin(), disassembledInstructions.end(), CompareInstructions);

	if (didErrorOccur)
	{
		wxMessageBox("An error occured while disassembling", "Disassembly not fully completed");
	}

	UpdateDecompParams();

	logTextCtrl->Log("handling jump tables...");
	logTextCtrl->Log("\tCurrent instruction: ", false, false);
	logTextCtrl->LogProgress(0, false);

	if (!HandleJmpTables())
	{
		wxMessageBox("An error occured while handling jump tables", "Disassembly not fully completed");
	}

	logTextCtrl->Log("updating disassembly GUI...");

	for (int32_t i = 0; i < disassemblyTextCtrls.size(); i++)
	{
		disassemblyTextCtrls[i]->Initialize(disassembledInstructions.data(), disassembledInstructions.size(), -1, errorAddress);
	}

	logTextCtrl->Log("finished disassembling");

	if (functions.size() == 0)
	{
		int32_t answer = wxMessageBox("Do you want to analyze the file?", "Analyze file", wxYES_NO, this);
		if (answer == wxYES)
		{
			AnalyzeFile();
		}
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

	UpdateDecompParams();

	logTextCtrl->Log("finding all functions...");

	int32_t getSymbols = wxNO;
	if (fileFormat != UNKNOWN_FF)
	{
		getSymbols = wxMessageBox("Do you want to look for function name symbols? This could take some time.", "Get function name symbols", wxYES_NO, this);
	}

	FindAllFunctions(getSymbols == wxYES);

	logTextCtrl->Log("analyzing functions...");

	if (!analyzeAllFunctions(&decompParams))
	{
		wxMessageBox("Error analyzing functions", "Failed to analyze functions");
	}

	logTextCtrl->Log("updating functions GUI...");

	for (int32_t i = 0; i < functionsTextCtrls.size(); i++)
	{
		functionsTextCtrls[i]->ShowAllFunctions(-1);
	}

	logTextCtrl->Log("finished analyzing file");

	decompParams.currentFunc = 0;
}

void MainGui::UpdateDecompParams()
{
	decompParams.imports = imports;
	decompParams.numOfImports = numOfImports;

	decompParams.imageBase = imageBase;
	decompParams.sections = sections;
	decompParams.numOfSections = numOfSections;

	decompParams.fileBytes = fileBytes;
	decompParams.numOfFileBytes = numOfFileBytes;

	decompParams.is64Bit = is64Bit;
	decompParams.fileFormat = fileFormat;

	decompParams.instructions = disassembledInstructions.data();
	decompParams.numOfInstructions = disassembledInstructions.size();

	decompParams.functions = functions.data();
	decompParams.numOfFunctions = functions.size();

	decompParams.useStdInt = settingsWindow->decompilerSettingsWindow->useStdInt;

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

	for (int32_t i = 0; i < dataTextCtrls.size(); i++)
	{
		dataTextCtrls[i]->ClearText();
	}
	for (int32_t i = 0; i < disassemblyTextCtrls.size(); i++)
	{
		disassemblyTextCtrls[i]->ClearData();
	}
	for (int32_t i = 0; i < decompilationTextCtrls.size(); i++)
	{
		decompilationTextCtrls[i]->ClearText();
	}
	for (int32_t i = 0; i < functionsTextCtrls.size(); i++)
	{
		functionsTextCtrls[i]->ClearText();
	}

	int32_t numOfInstructions = disassembledInstructions.size();
	for (int32_t i = 0; i < numOfInstructions; i++)
	{
		free(disassembledInstructions[i].operands);
	}

	disassembledInstructions.clear();
	disassembledInstructions.shrink_to_fit();

	jumpTables.clear();
	jumpTables.shrink_to_fit();

	if (sections)
	{
		for (int32_t i = 0; i < numOfSections; i++)
		{
			freeJdcStr(&sections[i].name);
		}

		delete[] sections;
		sections = 0;
	}

	if (imports)
	{
		for (int32_t i = 0; i < numOfImports; i++)
		{
			freeJdcStr(&imports[i].name);
		}

		delete[] imports;
		imports = 0;
	}

	if (libraryNames)
	{
		for (int32_t i = 0; i < numOfLibraries; i++)
		{
			freeJdcStr(&libraryNames[i]);
		}

		delete[] libraryNames;
		numOfLibraries = 0;
	}

	for (int32_t i = 0; i < functions.size(); i++)
	{
		freeFunction(&functions[i]);
	}

	functions.clear();
	functions.shrink_to_fit();

	currentFilePath = "";
	fileFormat = UNKNOWN_FF;
}

enum JdcStatus MainGui::DisassembleTakingJumps(uint64_t startVA, struct DisassembledInstruction* instructionBuffer, struct DisassemblerOptions* options, uint64_t* errorAddress, int32_t callNum)
{
	struct FileSection* currentSection = 0;
	uint64_t currentFileOffset = rvaToFileOffset(sections, numOfSections, startVA - imageBase, &currentSection);
	if (!currentSection || currentSection->type != CODE_FST)
	{
		return SUCCESS_JDC;
	}
	else if (currentFileOffset >= numOfFileBytes || currentSection->fileOffset + currentSection->physicalSize > numOfFileBytes)
	{
		logTextCtrl->Log(wxString::Format("instruction jumps outside file: 0x%llX", instructionBuffer->address), true);
		return ERROR_JDC;
	}

	bool storeInstruction = true;

	uint64_t currentVirtualAddress = startVA;
	while (currentFileOffset < currentSection->fileOffset + currentSection->physicalSize)
	{
		if (findInstructionByAddress(disassembledInstructions.data(), disassembledInstructions.size(), currentVirtualAddress) != -1)
		{
			return SUCCESS_JDC;
		}

		int32_t instructionIndex = findInstructionInsertPoint(disassembledInstructions.data(), disassembledInstructions.size(), currentVirtualAddress);
		if (instructionIndex > 0 && instructionIndex < disassembledInstructions.size() &&
			currentVirtualAddress > disassembledInstructions[instructionIndex - 1].address &&
			currentVirtualAddress < disassembledInstructions[instructionIndex - 1].address + disassembledInstructions[instructionIndex - 1].numOfBytes)
		{
			logTextCtrl->Log(wxString::Format("instruction overlaps with existing instruction: 0x%llX", currentVirtualAddress), true);
			return ERROR_JDC;
		}

		if (!disassembleInstruction(&fileBytes[currentFileOffset], fileBytes + currentSection->fileOffset + currentSection->physicalSize - 1, options, instructionBuffer))
		{
			if (errorAddress) { *errorAddress = currentVirtualAddress; }
			logTextCtrl->Log(wxString::Format("bad instruction at 0x%llX", currentVirtualAddress), true);
			return ERROR_JDC;
		}

		instructionBuffer->address = currentVirtualAddress;

		currentFileOffset += instructionBuffer->numOfBytes;
		currentVirtualAddress += instructionBuffer->numOfBytes;

		if (storeInstruction) // only the instructions that are jumped to are stored because inserting into the vector takes too much time
		{
			// this needs to be sorted here because the find instruction functions use a binary search
			disassembledInstructions.insert(disassembledInstructions.begin() + instructionIndex, *instructionBuffer);
			storeInstruction = false;
		}

		logTextCtrl->LogProgress(currentVirtualAddress, true);

		decompParams.instructions = instructionBuffer;
		if (isOpcodeReturn(instructionBuffer->opcode))
		{
			return SUCCESS_JDC;
		}
		else if (isOpcodeJmp(instructionBuffer->opcode))
		{
			uint64_t jmpDst = getJmpDst(&decompParams, 0);
			if (jmpDst != 0)
			{
				struct FileSection* section = 0;
				currentFileOffset = rvaToFileOffset(sections, numOfSections, jmpDst - imageBase, &section);
				if (!section || section->type != CODE_FST)
				{
					return SUCCESS_JDC;
				}

				currentVirtualAddress = jmpDst;
				storeInstruction = true;
			}
			else
			{
				return SUCCESS_JDC;
			}
		}
		else if (isOpcodeJcc(instructionBuffer->opcode) || isOpcodeCall(instructionBuffer->opcode))
		{
			uint64_t jmpDst = getJmpDst(&decompParams, 0);
			if (jmpDst != 0)
			{
				if (ERROR_JDC == DisassembleTakingJumps(jmpDst, instructionBuffer, options, errorAddress, callNum + 1))
				{
					return ERROR_JDC;
				}
			}
		}
		else if (callNum > 2000)
		{
			return SUCCESS_JDC;
		}
	}

	return SUCCESS_JDC;
}

enum JdcStatus MainGui::DisassembleBetweenBounds(uint64_t startVA, uint64_t endVA, struct DisassembledInstruction* instructionBuffer, struct DisassemblerOptions* options)
{
	struct FileSection* currentSection = 0;
	uint64_t currentFileOffset = rvaToFileOffset(sections, numOfSections, startVA - imageBase, &currentSection);
	if (!currentSection || currentSection->type != CODE_FST)
	{
		return SUCCESS_JDC;
	}
	else if (currentFileOffset >= numOfFileBytes)
	{
		return ERROR_JDC;
	}

	uint64_t endFileOffset = rvaToFileOffset(sections, numOfSections, endVA - imageBase, 0);
	if (endFileOffset > numOfFileBytes || endFileOffset == 0)
	{
		return ERROR_JDC;
	}

	uint64_t currentVirtualAddress = startVA;
	while (currentFileOffset < endFileOffset)
	{
		if (!disassembleInstruction(&fileBytes[currentFileOffset], fileBytes + endFileOffset - 1, options, instructionBuffer) ||
			currentFileOffset + instructionBuffer->numOfBytes > endFileOffset)
		{
			int32_t numOfBytes = instructionBuffer->numOfBytes;
			if (numOfBytes == 0)
			{
				return ERROR_JDC;
			}

			memset(instructionBuffer, 0, sizeof(struct DisassembledInstruction));
			instructionBuffer->opcode = DATA;
			instructionBuffer->numOfBytes = 1;

			logTextCtrl->LogProgress(currentVirtualAddress, true);

			for (int32_t i = 0; i < numOfBytes; i++)
			{
				if (currentFileOffset >= endFileOffset)
				{
					return SUCCESS_JDC;
				}

				instructionBuffer->address = currentVirtualAddress;
				instructionBuffer->operands = (struct Operand*)calloc(1, sizeof(struct Operand));
				if (!instructionBuffer->operands)
				{
					return ERROR_JDC;
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

	return SUCCESS_JDC;
}

enum JdcStatus MainGui::HandleJmpTables()
{
	// this is setting called instructions from the RUNTIME_FUNCTION structs in .pdata
	if (fileFormat == PE_FF && is64Bit) // https://learn.microsoft.com/en-us/cpp/build/exception-handling-x64?view=msvc-170
	{
		for (int32_t i = 0; i < numOfSections; i++)
		{
			if (strcmp(sections[i].name.buffer, ".pdata") == 0)
			{
				int32_t j = 0;
				while (j < sections[i].physicalSize)
				{
					uint64_t beginAddress = imageBase + *(uint32_t*)(fileBytes + sections[i].fileOffset + j);
					if (beginAddress == 0)
					{
						break;
					}

					int32_t calledInstructionIndex = findInstructionByAddress(disassembledInstructions.data(), disassembledInstructions.size(), beginAddress);
					if (calledInstructionIndex != -1)
					{
						disassembledInstructions[calledInstructionIndex].isCalled = 1;
					}

					j += 0xC;
				}

				break;
			}
		}
	}

	int32_t numOfInstructions = disassembledInstructions.size();
	for (int32_t i = 0; i < numOfInstructions; i++)
	{
		logTextCtrl->LogProgress(disassembledInstructions[i].address, true);

		if (disassembledInstructions[i].address == imageBase + entryPoint)
		{
			disassembledInstructions[i].isCalled = 1;
		}

		struct JumpTable jumpTable = { 0 };
		if (getJumpTable(&decompParams, i, &jumpTable))
		{
			jumpTables.push_back(jumpTable);
		}
		else if (disassembledInstructions[i].opcode == CALL_NEAR)
		{
			int32_t calledInstructionIndex = findInstructionByAddress(disassembledInstructions.data(), disassembledInstructions.size(), getJmpDst(&decompParams, i));
			if (calledInstructionIndex != -1)
			{
				disassembledInstructions[calledInstructionIndex].isCalled = 1;
			}
		}
		else if (disassembledInstructions[i].opcode == JMP_NEAR)
		{
			int32_t dstIndex = findInstructionByAddress(disassembledInstructions.data(), disassembledInstructions.size(), getJmpDst(&decompParams, i));
			if (dstIndex != -1)
			{
				disassembledInstructions[dstIndex].isJmpDst = 1;
			}
		}
	}

	std::sort(jumpTables.begin(), jumpTables.end(), CompareJumpTables);

	std::vector<struct DisassembledInstruction> dataInstructions; // this buffer is used so that only one insert call is needed
	int32_t numOfJmpTables = jumpTables.size();
	for (int32_t i = 0; i < numOfJmpTables; i++)
	{
		uint64_t fileOffest = rvaToFileOffset(sections, numOfSections, jumpTables[i].jmpTableAddress - imageBase, 0);
		int32_t instructionIndex = findInstructionByAddress(disassembledInstructions.data(), disassembledInstructions.size(), jumpTables[i].jmpTableAddress);
		if (instructionIndex != -1)
		{
			int32_t lastInstructionIndex = instructionIndex;
			while (!disassembledInstructions[lastInstructionIndex].isCalled &&
				!disassembledInstructions[lastInstructionIndex].isJmpDst)
			{
				free(disassembledInstructions[lastInstructionIndex].operands);
				lastInstructionIndex++;
			}

			uint64_t lastAddress = disassembledInstructions[lastInstructionIndex].address;
			disassembledInstructions.erase(disassembledInstructions.begin() + instructionIndex, disassembledInstructions.begin() + lastInstructionIndex);

			bool isInIndirectTable = false;
			uint64_t currentAddress = jumpTables[i].jmpTableAddress;
			uint64_t initialAddress = currentAddress;
			while (currentAddress < lastAddress)
			{
				if (currentAddress == jumpTables[i].indirectTableAddress)
				{
					isInIndirectTable = true;
				}
				else if (i + 1 < numOfJmpTables && currentAddress == jumpTables[i + 1].jmpTableAddress)
				{
					i++;
					isInIndirectTable = false;
				}

				struct DisassembledInstruction instruction;
				memset(&instruction, 0, sizeof(struct DisassembledInstruction));
				instruction.opcode = DATA;
				instruction.numOfOperands = 1;
				instruction.operands = (struct Operand*)calloc(1, sizeof(struct Operand));
				if (!instruction.operands)
				{
					return ERROR_JDC;
				}
				instruction.operands[0].type = IMMEDIATE;
				instruction.operands[0].immediate.size = isInIndirectTable ? 1 : jumpTables[i].addressSize;
				instruction.numOfBytes = isInIndirectTable ? 1 : jumpTables[i].addressSize;
				instruction.address = currentAddress;

				if (isInIndirectTable)
				{
					instruction.operands[0].immediate.value = *(uint8_t*)(fileBytes + fileOffest + (currentAddress - initialAddress));
				}
				else if (jumpTables[i].addressSize == 4)
				{
					instruction.operands[0].immediate.value = *(uint32_t*)(fileBytes + fileOffest + (currentAddress - initialAddress));
				}
				else
				{
					instruction.operands[0].immediate.value = *(uint64_t*)(fileBytes + fileOffest + (currentAddress - initialAddress));
				}

				dataInstructions.push_back(instruction);
				currentAddress += isInIndirectTable ? 1 : jumpTables[i].addressSize;
			}

			disassembledInstructions.insert(disassembledInstructions.begin() + instructionIndex, dataInstructions.begin(), dataInstructions.end());
			decompParams.numOfInstructions = disassembledInstructions.size();
			dataInstructions.clear();
		}
	}

	return SUCCESS_JDC;
}

void MainGui::FindAllFunctions(bool getSymbols)
{
	logTextCtrl->Log("\tCurrent function: ", false, false);
	logTextCtrl->LogProgress(0, false);

	int32_t numOfInstructions = disassembledInstructions.size();
	int32_t instructionIndex = 0;

	struct Function currentFunction;
	memset(&currentFunction, 0, sizeof(struct Function));
	int32_t numOfFunctions = 0;
	while (instructionIndex < numOfInstructions && findNextFunction(&decompParams, &currentFunction, &instructionIndex))
	{
		currentFunction.name = initializeJdcStr();
		if (!getSymbols || !getSymbolByValue(currentFilePath.c_str().AsWChar(), fileFormat, is64Bit, disassembledInstructions[currentFunction.firstInstructionIndex].address, &currentFunction.name))
		{
			sprintfJdc(&currentFunction.name, false, "func%llX", disassembledInstructions[currentFunction.firstInstructionIndex].address - imageBase);
		}

		logTextCtrl->LogProgress(disassembledInstructions[currentFunction.firstInstructionIndex].address, true);

		functions.push_back(currentFunction);
		memset(&currentFunction, 0, sizeof(struct Function));
		numOfFunctions++;
	}

	UpdateDecompParams();
}

void MainGui::CloseApp(wxCloseEvent& e)
{
	settingsWindow->SavePreferences();
	auiManager.UnInit();
	colorsMenu->Destroy();
	Destroy();
}