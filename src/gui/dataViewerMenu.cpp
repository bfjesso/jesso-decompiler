#include "dataViewerMenu.h"
#include "colorsMenu.h"

wxBEGIN_EVENT_TABLE(DataViewer, wxFrame)
EVT_CLOSE(DataViewer::CloseMenu)
EVT_CHOICE(DataTypeChoiceID, DataViewer::UpdateDataList)
EVT_CHOICE(SectionChoiceID, DataViewer::UpdateDataList)
EVT_CHECKBOX(HexCheckBoxID, DataViewer::UpdateDataList)
wxEND_EVENT_TABLE()

DataViewer::DataViewer() : wxFrame(nullptr, MainWindowID, "Data Viewer", wxPoint(50, 50), wxSize(900, 600))
{
	SetOwnBackgroundColour(backgroundColor);

	dataTypeChoice = new wxChoice(this, DataTypeChoiceID, wxPoint(0, 0), wxSize(120, 50), wxArrayString(6, dataTypeStrs));
	dataTypeChoice->SetSelection(0);
	dataTypeChoice->SetOwnBackgroundColour(foregroundColor);
	dataTypeChoice->SetOwnForegroundColour(textColor);

	sectionChoice = new wxChoice(this, SectionChoiceID, wxPoint(0, 0), wxSize(120, 50));
	sectionChoice->SetOwnBackgroundColour(foregroundColor);
	sectionChoice->SetOwnForegroundColour(textColor);

	hexCheckBox = new wxCheckBox(this, HexCheckBoxID, "Hexadecimal");
	hexCheckBox->SetOwnForegroundColour(textColor);
	hexCheckBox->SetValue(true);

	dataTextCtrl = new wxStyledTextCtrl(this, wxID_ANY, wxPoint(0, 0), wxSize(900, 500));
	SetUpStyledTextCtrl(dataTextCtrl);

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
	const int bytesPerLine = 16;
	unsigned char isHex = hexCheckBox->IsChecked();

	int baseIndex = 0;
	for (int i = 0; i < sectionSelection; i++) 
	{
		baseIndex += sections[i].size;
	}

	wxString dataText = "";
	dataText.reserve(sections[sectionSelection].size * 6);

	char sprintfBuffer[100] = { 0 };

	for (int i = 0; i < sections[sectionSelection].size; i += bytesPerLine)
	{
		unsigned long long address = imageBase + sections[sectionSelection].virtualAddress + i;
		sprintf(sprintfBuffer, "%llX", address);
		dataText += wxString(sprintfBuffer) + wxString(sections[sectionSelection].name.buffer) + "\t";

		for (int j = 0; j < bytesPerLine / typeSize; j++)
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
					sprintf(sprintfBuffer, "0x%02X", fileBytes[i + j + baseIndex]);
				}
				else
				{
					sprintf(sprintfBuffer, "%d", fileBytes[i + j + baseIndex]);
				}
				break;
			}
			case 1: // 2-byte int
			{
				if (isHex)
				{
					sprintf(sprintfBuffer, "0x%04X", *(short*)(fileBytes + i + j + baseIndex));
				}
				else
				{
					sprintf(sprintfBuffer, "%d", *(short*)(fileBytes + i + j + baseIndex));
				}
				break;
			}
			case 2: // 4-byte int
			{
				if (isHex)
				{
					sprintf(sprintfBuffer, "0x%08X", *(int*)(fileBytes + i + j + baseIndex));
				}
				else
				{
					sprintf(sprintfBuffer, "%d", *(int*)(fileBytes + i + j + baseIndex));
				}
				break;
			}
			case 3: // 8-byte int
			{
				if (isHex)
				{
					sprintf(sprintfBuffer, "0x%016llX", *(long long*)(fileBytes + i + j + baseIndex));
				}
				else
				{
					sprintf(sprintfBuffer, "%lld", *(long long*)(fileBytes + i + j + baseIndex));
				}
				break;
			}
			case 4: // float
			{
				sprintf(sprintfBuffer, "%0.8g", *(float*)(fileBytes + i + j + baseIndex));
				break;
			}
			case 5: // double
			{
				sprintf(sprintfBuffer, "%0.16g", *(double*)(fileBytes + i + j + baseIndex));
				break;
			}
			}

			dataText += wxString(sprintfBuffer) + " ";
		}

		dataText += "\n";
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
