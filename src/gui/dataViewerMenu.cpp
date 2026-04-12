#include "dataViewerMenu.h"
#include "colorsMenu.h"

wxBEGIN_EVENT_TABLE(DataViewer, wxFrame)
EVT_CLOSE(DataViewer::CloseMenu)
EVT_CHOICE(DataTypeChoiceID, DataViewer::UpdateDataList)
EVT_CHECKBOX(HexCheckBoxID, DataViewer::UpdateDataList)
wxEND_EVENT_TABLE()

DataViewer::DataViewer() : wxFrame(nullptr, MainWindowID, "Data Viewer", wxPoint(50, 50), wxSize(600, 600))
{
	SetOwnBackgroundColour(backgroundColor);

	dataTypeChoice = new wxChoice(this, DataTypeChoiceID, wxPoint(0, 0), wxSize(120, 50), wxArrayString(6, dataTypeStrs));
	dataTypeChoice->SetSelection(2);
	dataTypeChoice->SetOwnBackgroundColour(foregroundColor);
	dataTypeChoice->SetOwnForegroundColour(textColor);

	hexCheckBox = new wxCheckBox(this, HexCheckBoxID, "Hexadecimal");
	hexCheckBox->SetOwnForegroundColour(textColor);
	hexCheckBox->SetValue(true);

	dataTextCtrl = new wxStyledTextCtrl(this, wxID_ANY, wxPoint(0, 0), wxSize(500, 500));
	SetUpStyledTextCtrl(dataTextCtrl);

	row1Sizer = new wxBoxSizer(wxHORIZONTAL);
	row2Sizer = new wxBoxSizer(wxHORIZONTAL);
	vSizer = new wxBoxSizer(wxVERTICAL);

	row1Sizer->Add(dataTypeChoice, 0, wxLEFT | wxTOP | wxRIGHT, 10);
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

	int baseIndex = 0;
	for (int j = 0; j < numOfSections; j++) 
	{
		int i = 0;
		while (i < sections[j].size) 
		{
			uintptr_t address = imageBase + sections[j].virtualAddress + i;
			char addressStrBuffer[20];
			sprintf(addressStrBuffer, "%llX", address);
			wxString addressStr = wxString(addressStrBuffer) + wxString(sections[j].name.buffer) + "\t";

			char dataStrBuffer[50];
			switch (typeSelection)
			{
			case 0: // 1-byte int
			{
				if (hexCheckBox->IsChecked())
				{
					sprintf(dataStrBuffer, "0x%X", fileBytes[i + baseIndex]);
				}
				else
				{
					sprintf(dataStrBuffer, "%d", fileBytes[i + baseIndex]);
				}
				break;
			}
			case 1: // 2-byte int
			{
				if (hexCheckBox->IsChecked())
				{
					sprintf(dataStrBuffer, "0x%X", *(short*)(fileBytes + baseIndex + i));
				}
				else
				{
					sprintf(dataStrBuffer, "%d", *(short*)(fileBytes + baseIndex + i));
				}
				break;
			}
			case 2: // 4-byte int
			{
				if (hexCheckBox->IsChecked())
				{
					sprintf(dataStrBuffer, "0x%X", *(int*)(fileBytes + baseIndex + i));
				}
				else
				{
					sprintf(dataStrBuffer, "%d", *(int*)(fileBytes + baseIndex + i));
				}
				break;
			}
			case 3: // 8-byte int
			{
				if (hexCheckBox->IsChecked())
				{
					sprintf(dataStrBuffer, "0x%llX", *(long long*)(fileBytes + baseIndex + i));
				}
				else
				{
					sprintf(dataStrBuffer, "%lld", *(long long*)(fileBytes + baseIndex + i));
				}
				break;
			}
			case 4: // float
			{
				sprintf(dataStrBuffer, "%f", *(float*)(fileBytes + baseIndex + i));
				break;
			}
			case 5: // double
			{
				sprintf(dataStrBuffer, "%lf", *(double*)(fileBytes + baseIndex + i));
				break;
			}
			}

			wxString dataStr = wxString(dataStrBuffer) + "\n";

			int pos = dataTextCtrl->GetLength() - 1;
			dataTextCtrl->AppendText(addressStr);
			dataTextCtrl->StartStyling(pos);
			dataTextCtrl->SetStyling(addressStr.length(), ColorsMenu::DisassemblyColor::ADDRESS_COLOR);

			pos += addressStr.length() + 1;

			dataTextCtrl->AppendText(dataStr);
			dataTextCtrl->StartStyling(pos);
			dataTextCtrl->SetStyling(dataStr.length(), ColorsMenu::DisassemblyColor::CONSTANT_COLOR);

			i += typeSize;
		}

		baseIndex += sections[j].size;
	}

	dataTextCtrl->Thaw();
	dataTextCtrl->SetReadOnly(true);
}

void DataViewer::OpenMenu(wxPoint position, uintptr_t imageBas, FileSection* secs, int numOfSecs, unsigned char* bytes)
{
	position.x += 10;
	position.y += 10;
	SetPosition(position);
	Show();
	Raise();

	imageBase = imageBas;
	sections = secs;
	numOfSections = numOfSecs;
	fileBytes = bytes;

	LoadData();
}

void DataViewer::CloseMenu(wxCloseEvent& e) // stops this frame from being destroyed and the data being lost
{
	Hide();
}
