#include "mainGui.h"
#include <wx/numdlg.h>
#include "bytesDisassemblerWindow.h"
#include "sectionsGrid.h"
#include "stringsTextCtrl.h"
#include "importsGrid.h"
#include "fileHeadersWindow.h"
#include "calculatorWindow.h"
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

wxDEFINE_EVENT(EVT_WORKER_LOG, wxThreadEvent);
wxDEFINE_EVENT(EVT_WORKER_LOG_PROGRESS, wxThreadEvent);
wxDEFINE_EVENT(EVT_ANALYSIS_DONE, wxThreadEvent);
wxDEFINE_EVENT(EVT_DISASSEMBLY_DONE, wxThreadEvent);

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

	Bind(EVT_WORKER_LOG, [this](wxThreadEvent& e) {
		enum WorkerThread::LogParams logParam = (enum WorkerThread::LogParams)(e.GetInt());
		switch (logParam)
		{
		case WorkerThread::LogParams::LOG_PARAM_ERROR:
			logTextCtrl->Log(e.GetString(), true);
			break;
		case WorkerThread::LogParams::LOG_PARAM_ADD_NEW_LINE:
			logTextCtrl->Log(e.GetString());
			break;
		case WorkerThread::LogParams::LOG_PARAM_NONE:
			logTextCtrl->Log(e.GetString(), false, false);
			break;
		}
		});
	Bind(EVT_WORKER_LOG_PROGRESS, [this](wxThreadEvent& e) {
		logTextCtrl->LogProgress(e.GetPayload<uint64_t>(), e.GetInt() == 1);
		});

	Bind(EVT_DISASSEMBLY_DONE, [this](wxThreadEvent&) {
		isDisassembling = false;
		for (int32_t i = 0; i < disassemblyTextCtrls.size(); i++)
		{
			disassemblyTextCtrls[i]->Initialize(disassembledInstructions.data(), disassembledInstructions.size(), -1, errorAddress);
		}

		if (functions.size() == 0)
		{
			int32_t answer = wxMessageBox("Do you want to analyze the file?", "Analyze file", wxYES_NO, this);
			if (answer == wxYES)
			{
				AnalyzeFile();
			}
		}
		});

	Bind(EVT_ANALYSIS_DONE, [this](wxThreadEvent&) {
		isAnalyzing = false;
		for (int32_t i = 0; i < functionsTextCtrls.size(); i++)
		{
			functionsTextCtrls[i]->ShowAllFunctions(-1);
		}
		});

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

	workerThread = new WorkerThread(this, WorkerThread::DISASSEMBLE_WORKER_THREAD, false);
	if (workerThread->Run() != wxTHREAD_NO_ERROR)
	{
		delete workerThread;
		wxMessageBox("Error starting disassembly thread", "Can't disassemble");
		return;
	}

	isDisassembling = true;
}

void MainGui::AnalyzeFile()
{
	if (currentFilePath == "")
	{
		wxMessageBox("No file opened", "Can't analyze");
		return;
	}
	if (isDisassembling)
	{
		wxMessageBox("File has not finished being disassembled", "Can't analyze");
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

	int32_t getSymbols = wxNO;
	if (fileFormat != UNKNOWN_FF)
	{
		getSymbols = wxMessageBox("Do you want to look for function name symbols? This could take some time.", "Get function name symbols", wxYES_NO, this);
	}

	workerThread = new WorkerThread(this, WorkerThread::ANALYZE_WORKER_THREAD, getSymbols == wxYES);
	if (workerThread->Run() != wxTHREAD_NO_ERROR)
	{
		delete workerThread;
		wxMessageBox("Error starting analysis thread", "Can't analyze");
		return;
	}

	isAnalyzing = true;
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

void MainGui::CloseApp(wxCloseEvent& e)
{
	if (workerThread)
	{
		workerThread->Stop();
		delete workerThread;
		workerThread = nullptr;
	}

	settingsWindow->SavePreferences();
	auiManager.UnInit();
	colorsMenu->Destroy();
	Destroy();
}