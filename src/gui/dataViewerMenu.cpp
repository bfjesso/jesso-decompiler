#include "dataViewerMenu.h"
#include "colorsMenu.h"
#include <string>

wxBEGIN_EVENT_TABLE(DataViewer, wxFrame)
EVT_CLOSE(DataViewer::CloseMenu)
EVT_CHOICE(DataTypeChoiceID, DataViewer::UpdateDataList)
EVT_CHOICE(SectionChoiceID, DataViewer::UpdateDataList)
EVT_CHECKBOX(HexCheckBoxID, DataViewer::UpdateDataList)
wxEND_EVENT_TABLE()

DataViewer::DataViewer() : wxFrame(nullptr, MainWindowID, "Data Viewer", wxPoint(50, 50), wxSize(500, 600))
{
	SetOwnBackgroundColour(backgroundColor);

	dataTypeChoice = new wxChoice(this, DataTypeChoiceID, wxPoint(0, 0), wxSize(120, 50), wxArrayString(numOfDataTypes, dataTypeStrs));
	dataTypeChoice->SetSelection(0);
	dataTypeChoice->SetOwnBackgroundColour(foregroundColor);
	dataTypeChoice->SetOwnForegroundColour(textColor);

	sectionChoice = new wxChoice(this, SectionChoiceID, wxPoint(0, 0), wxSize(120, 50));
	sectionChoice->SetOwnBackgroundColour(foregroundColor);
	sectionChoice->SetOwnForegroundColour(textColor);

	hexCheckBox = new wxCheckBox(this, HexCheckBoxID, "Hexadecimal");
	hexCheckBox->SetOwnForegroundColour(textColor);
	hexCheckBox->SetValue(true);

	dataTextCtrl = new wxStyledTextCtrl(this, wxID_ANY, wxPoint(0, 0), wxSize(500, 500));
	SetUpStyledTextCtrl(dataTextCtrl);
	dataTextCtrl->Bind(wxEVT_CONTEXT_MENU, [&](wxContextMenuEvent& e) -> void { StyledTextCtrlRightClickOptions(e); });

	row1Sizer = new wxBoxSizer(wxHORIZONTAL);
	row2Sizer = new wxBoxSizer(wxHORIZONTAL);
	vSizer = new wxBoxSizer(wxVERTICAL);

	row1Sizer->Add(dataTypeChoice, 0, wxLEFT | wxTOP | wxRIGHT, 10);
	row1Sizer->Add(sectionChoice, 0, wxTOP | wxRIGHT, 10);
	row1Sizer->Add(hexCheckBox, 0, wxTOP, 10);

	row2Sizer->Add(dataTextCtrl, 1, wxLEFT | wxBOTTOM | wxRIGHT | wxEXPAND, 10);

	vSizer->Add(row1Sizer, 0, wxEXPAND);
	vSizer->Add(row2Sizer, 1, wxEXPAND);

	SetSizerAndFit(vSizer);
}

void DataViewer::UpdateDataList(wxCommandEvent& e) 
{
	LoadData();
}

void DataViewer::LoadData()
{
	if (!fileBytes)
	{
		wxMessageBox("No file bytes", "Can't load data");
		return;
	}

	ClearStyledTextCtrl(dataTextCtrl);

	dataTextCtrl->SetReadOnly(false);
	dataTextCtrl->Freeze();

	int typeSelection = dataTypeChoice->GetSelection();
	int typeSize = typeSizes[typeSelection];
	int sectionSelection = sectionChoice->GetSelection();
	unsigned char isHex = hexCheckBox->IsChecked();

	int baseIndex = 0;
	for (int i = 0; i < sectionSelection; i++) 
	{
		baseIndex += sections[i].size;
	}

	std::string dataText;
	dataText.reserve(sections[sectionSelection].size * 6);

	char lineBuffer[512] = { 0 };
	for (int i = 0; i < sections[sectionSelection].size; i += bytesPerLine)
	{
		unsigned long long address = imageBase + sections[sectionSelection].virtualAddress + i;
		sprintf(lineBuffer, "%llX%s\t", address, sections[sectionSelection].name.buffer);

		for (int j = 0; j < bytesPerLine; j += typeSize)
		{
			if (i + j >= sections[sectionSelection].size) 
			{
				break;
			}
			
			switch (typeSelection)
			{
			case 0: // 1-byte int
			{
				if (isHex)
				{
					sprintf(lineBuffer + strlen(lineBuffer), "0x%02X ", fileBytes[i + j + baseIndex]);
				}
				else
				{
					sprintf(lineBuffer + strlen(lineBuffer), "%d ", fileBytes[i + j + baseIndex]);
				}
				break;
			}
			case 1: // 2-byte int
			{
				if (isHex)
				{
					sprintf(lineBuffer + strlen(lineBuffer), "0x%04X ", *(unsigned short*)(fileBytes + i + j + baseIndex));
				}
				else
				{
					sprintf(lineBuffer + strlen(lineBuffer), "%d ", *(unsigned short*)(fileBytes + i + j + baseIndex));
				}
				break;
			}
			case 2: // 4-byte int
			{
				if (isHex)
				{
					sprintf(lineBuffer + strlen(lineBuffer), "0x%08X ", *(unsigned int*)(fileBytes + i + j + baseIndex));
				}
				else
				{
					sprintf(lineBuffer + strlen(lineBuffer), "%d ", *(unsigned int*)(fileBytes + i + j + baseIndex));
				}
				break;
			}
			case 3: // 8-byte int
			{
				if (isHex)
				{
					sprintf(lineBuffer + strlen(lineBuffer), "0x%016llX ", *(unsigned long long*)(fileBytes + i + j + baseIndex));
				}
				else
				{
					sprintf(lineBuffer + strlen(lineBuffer), "%lld ", *(unsigned long long*)(fileBytes + i + j + baseIndex));
				}
				break;
			}
			case 4: // float
			{
				sprintf(lineBuffer + strlen(lineBuffer), "%0.8g ", *(float*)(fileBytes + i + j + baseIndex));
				break;
			}
			case 5: // double
			{
				sprintf(lineBuffer + strlen(lineBuffer), "%0.16g ", *(double*)(fileBytes + i + j + baseIndex));
				break;
			}
			case 6: // ASCII character
			{
				char c = *(char*)(fileBytes + i + j + baseIndex);
				if(c > 31 && c < 127)
				{
					sprintf(lineBuffer + strlen(lineBuffer), "'%c' ", c);
				}
				else
				{
					sprintf(lineBuffer + strlen(lineBuffer), "0x%02X ", fileBytes[i + j + baseIndex]);			
				}
				break;
			}
			}
		}

		dataText += lineBuffer;
		dataText += '\n';
	}

	dataTextCtrl->SetText(dataText);

	dataTextCtrl->StartStyling(0);
	dataTextCtrl->SetStyling(dataText.length(), ColorsMenu::DisassemblyColor::ADDRESS_COLOR);

	int start = 0;
	while (start < dataText.length())
	{
		int pos = dataText.find("\t", start);
		int end = dataText.find("\n", pos);
		if (pos != wxNOT_FOUND && end != wxNOT_FOUND)
		{
			dataTextCtrl->StartStyling(pos);
			dataTextCtrl->SetStyling(end - pos + 1, ColorsMenu::DisassemblyColor::CONSTANT_COLOR);

			start = end + 1;
		}
		else
		{
			break;
		}
	}

	dataTextCtrl->Thaw();
	dataTextCtrl->SetReadOnly(true);
}

void DataViewer::StyledTextCtrlRightClickOptions(wxContextMenuEvent& e)
{
	wxMenu menu;

	const int ID_COPY = 100;
	const int ID_SELECT_ALL = 101;
	const int ID_FIND = 102;
	const int ID_GO_TO_ADDR = 103;

	wxStyledTextCtrl* ctrl = (wxStyledTextCtrl*)(e.GetEventObject());

	long start;
	long end;
	ctrl->GetSelection(&start, &end);
	wxString selection = "";

	if (start != end)
	{
		wxString text = ctrl->GetValue();
		selection = text.substr(start, end - start);

		menu.Append(ID_COPY, "Copy");
		menu.Bind(wxEVT_MENU, [&](wxCommandEvent&) { CopyToClipboard(selection); }, ID_COPY);
	}


	menu.Append(ID_SELECT_ALL, "Select all");
	menu.Bind(wxEVT_MENU, [&](wxCommandEvent&) {
		ctrl->SetSelection(0, ctrl->GetLastPosition());
		ctrl->SetFocus();
	}, ID_SELECT_ALL);

	menu.Append(ID_FIND, "Find");
	menu.Bind(wxEVT_MENU, [&](wxCommandEvent&) {
		wxTextEntryDialog dlg(this, "", "Find text");
		if (dlg.ShowModal() == wxID_OK)
		{
			wxString txt = dlg.GetValue();
			if (!txt.IsEmpty())
			{
				int pos = ctrl->FindText(0, ctrl->GetLength(), txt);
				if (pos == -1)
				{
					wxMessageBox("Text not found", "Failed to find text");
				}
				else
				{
					ctrl->GotoPos(pos);
					ctrl->SetSelection(pos, pos + txt.size());
				}
			}
		}
	}, ID_FIND);

	menu.Append(ID_GO_TO_ADDR, "Go to address");
	menu.Bind(wxEVT_MENU, [&](wxCommandEvent&) {
		wxTextEntryDialog dlg(this, "", "Address");
		if (dlg.ShowModal() == wxID_OK)
		{
			wxString txt = dlg.GetValue();
			unsigned long long address = 0;
			if (txt.ToULongLong(&address, 16))
			{
				FileSection* section = &sections[sectionChoice->GetSelection()];
				unsigned long long sectionStart = section->virtualAddress + imageBase;
				if (address >= sectionStart && address < sectionStart + section->size)
				{
					int row = (address - sectionStart) / bytesPerLine;
					dataTextCtrl->GotoLine(row);
					int pos = dataTextCtrl->PositionFromLine(row);
					dataTextCtrl->SetSelection(pos, pos + dataTextCtrl->GetLineLength(row));
				}
				else 
				{
					wxMessageBox("Address not in currently selected section", "Failed to find address");
				}
			}
			else
			{
				wxMessageBox("Not valid hex number", "Failed to find address");
			}
		}
	}, ID_GO_TO_ADDR);

	PopupMenu(&menu, ScreenToClient(e.GetPosition()));
}

void DataViewer::ClearData() 
{
	ClearStyledTextCtrl(dataTextCtrl);

	fileBytes = nullptr;
	sections = nullptr;
	numOfSections = 0;
	imageBase = 0;
}

void DataViewer::OpenMenu(wxPoint position, uintptr_t imageBas, FileSection* secs, int numOfSecs, unsigned char* bytes)
{
	position.x += 10;
	position.y += 10;
	SetPosition(position);
	Show();
	Raise();

	if (!fileBytes) 
	{
		imageBase = imageBas;
		sections = secs;
		numOfSections = numOfSecs;
		fileBytes = bytes;

		sectionChoice->Clear();
		for (int i = 0; i < numOfSections; i++) 
		{
			sectionChoice->Append(sections[i].name.buffer);
		}
		sectionChoice->SetSelection(0);

		LoadData();
	}
}

void DataViewer::CloseMenu(wxCloseEvent& e) // stops this frame from being destroyed and the data being lost
{
	Hide();
}
