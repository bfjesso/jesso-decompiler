#include "dataViewerMenu.h"

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

	dataListBox = new wxListBox(this, wxID_ANY, wxPoint(0, 0), wxSize(500, 500));
	dataListBox->SetOwnBackgroundColour(foregroundColor);
	dataListBox->SetOwnForegroundColour(textColor);

	// ---------------------

	row1Sizer = new wxBoxSizer(wxHORIZONTAL);
	row2Sizer = new wxBoxSizer(wxHORIZONTAL);
	vSizer = new wxBoxSizer(wxVERTICAL);

	row1Sizer->Add(dataTypeChoice, 0, wxALL, 10);
	row1Sizer->Add(hexCheckBox, 0, wxBOTTOM | wxRIGHT, 10);

	row2Sizer->Add(dataListBox, 0, wxLEFT | wxBOTTOM | wxRIGHT, 10);

	vSizer->Add(row1Sizer, 0, wxEXPAND);
	vSizer->Add(row2Sizer, 0, wxEXPAND);

	SetSizer(vSizer);
}

void DataViewer::UpdateDataList(wxCommandEvent& e) 
{
	LoadData();
}

void DataViewer::LoadData()
{
	if (!bytes)
	{
		wxMessageBox("No data section bytes", "Can't load data");
		return;
	}

	dataListBox->Clear();

	int baseIndex = 0;
	for (int j = 0; j < numOfDataSections; j++) 
	{
		switch (dataTypeChoice->GetSelection())
		{
		case 0:
		{
			for (int i = 0; i < dataSections[j].size; i++)
			{
				uintptr_t address = imageBase + dataSections[j].virtualAddress + i;

				char addressStr[20];
				sprintf(addressStr, "%s %llX", dataSections[j].name, address);

				char dataStr[50];
				if (hexCheckBox->IsChecked())
				{
					sprintf(dataStr, "0x%X", bytes[i + baseIndex]);
				}
				else
				{
					sprintf(dataStr, "%d", bytes[i + baseIndex]);
				}

				dataListBox->AppendString(wxString(addressStr) + "\t" + wxString(dataStr));
			}
			break;
		}
		case 1:
		{
			for (int i = 0; i < dataSections[j].size; i += 2)
			{
				uintptr_t address = imageBase + dataSections[j].virtualAddress + i;

				char addressStr[20];
				sprintf(addressStr, "%s %llX", dataSections[j].name, address);

				char dataStr[50];
				if (hexCheckBox->IsChecked())
				{
					sprintf(dataStr, "0x%X", *(short*)(bytes + baseIndex + i));
				}
				else
				{
					sprintf(dataStr, "%d", *(short*)(bytes + baseIndex + i));
				}

				dataListBox->AppendString(wxString(addressStr) + "\t" + wxString(dataStr));
			}
			break;
		}
		case 2:
		{
			for (int i = 0; i < dataSections[j].size; i += 4)
			{
				uintptr_t address = imageBase + dataSections[j].virtualAddress + i;

				char addressStr[20];
				sprintf(addressStr, "%s %llX", dataSections[j].name, address);

				char dataStr[50];
				if (hexCheckBox->IsChecked())
				{
					sprintf(dataStr, "0x%X", *(int*)(bytes + baseIndex + i));
				}
				else
				{
					sprintf(dataStr, "%d", *(int*)(bytes + baseIndex + i));
				}

				dataListBox->AppendString(wxString(addressStr) + "\t" + wxString(dataStr));
			}
			break;
		}
		case 3:
		{
			for (int i = 0; i < dataSections[j].size; i += 8)
			{
				uintptr_t address = imageBase + dataSections[j].virtualAddress + i;

				char addressStr[20];
				sprintf(addressStr, "%s %llX", dataSections[j].name, address);

				char dataStr[50];
				if (hexCheckBox->IsChecked())
				{
					sprintf(dataStr, "0x%llX", *(long long*)(bytes + baseIndex + i));
				}
				else
				{
					sprintf(dataStr, "%lld", *(long long*)(bytes + baseIndex + i));
				}

				dataListBox->AppendString(wxString(addressStr) + "\t" + wxString(dataStr));
			}
			break;
		}
		case 4:
		{
			for (int i = 0; i < dataSections[j].size; i += 4)
			{
				uintptr_t address = imageBase + dataSections[j].virtualAddress + i;

				char addressStr[20];
				sprintf(addressStr, "%s %llX", dataSections[j].name, address);

				dataListBox->AppendString(wxString(addressStr) + "\t" + std::to_string(*(float*)(bytes + baseIndex + i)));
			}
			break;
		}
		case 5:
		{
			for (int i = 0; i < dataSections[j].size; i += 8)
			{
				uintptr_t address = imageBase + dataSections[j].virtualAddress + i;

				char addressStr[20];
				sprintf(addressStr, "%s %llX", dataSections[j].name, address);

				dataListBox->AppendString(wxString(addressStr) + "\t" + std::to_string(*(double*)(bytes + baseIndex + i)));
			}
			break;
		}
		}

		baseIndex += dataSections[j].size;
	}
}

void DataViewer::OpenMenu(wxPoint position, uintptr_t imageBas, FileSection* dataSecs, int numOfDataSecs, unsigned char* dataBytes)
{
	position.x += 10;
	position.y += 10;
	SetPosition(position);
	Show();
	Raise();

	imageBase = imageBas;
	dataSections = dataSecs;
	numOfDataSections = numOfDataSecs;
	bytes = dataBytes;

	LoadData();
}

void DataViewer::CloseMenu(wxCloseEvent& e) // stops this frame from being destroyed and the data being lost
{
	Hide();
}
