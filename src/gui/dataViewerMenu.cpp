#include "dataViewerMenu.h"
#include "../pe-handler/peHandler.h"

wxBEGIN_EVENT_TABLE(DataViewer, wxFrame)
EVT_CLOSE(CloseMenu)
EVT_CHOICE(DataTypeChoiceID, UpdateDataList)
EVT_CHECKBOX(HexCheckBoxID, UpdateDataList)
wxEND_EVENT_TABLE()

DataViewer::DataViewer() : wxFrame(nullptr, MainWindowID, "Data Viewer", wxPoint(50, 50), wxSize(600, 600))
{
	SetOwnBackgroundColour(backgroundColor);

	dataTypeChoice = new wxChoice(this, DataTypeChoiceID, wxPoint(0, 0), wxSize(120, 50), wxArrayString(6, dataTypeStrs));
	dataTypeChoice->SetSelection(0);
	dataTypeChoice->SetOwnBackgroundColour(foregroundColor);
	dataTypeChoice->SetOwnForegroundColour(textColor);

	numOfbytesInputTextCtrl = new wxTextCtrl(this, wxID_ANY, "1000", wxPoint(0, 0), wxSize(100, 25));
	numOfbytesInputTextCtrl->SetOwnBackgroundColour(foregroundColor);
	numOfbytesInputTextCtrl->SetOwnForegroundColour(textColor);
	numOfbytesInputTextCtrl->SetToolTip("Number of bytes to read from the file's code section");

	hexCheckBox = new wxCheckBox(this, HexCheckBoxID, "Hexadecimal");
	hexCheckBox->SetOwnForegroundColour(textColor);

	dataListBox = new wxListBox(this, wxID_ANY, wxPoint(0, 0), wxSize(500, 500));
	dataListBox->SetOwnBackgroundColour(foregroundColor);
	dataListBox->SetOwnForegroundColour(textColor);

	// ---------------------

	row1Sizer = new wxBoxSizer(wxHORIZONTAL);
	row2Sizer = new wxBoxSizer(wxHORIZONTAL);
	row3Sizer = new wxBoxSizer(wxHORIZONTAL);
	vSizer = new wxBoxSizer(wxVERTICAL);

	row1Sizer->Add(dataTypeChoice, 0, wxALL, 10);
	row1Sizer->Add(numOfbytesInputTextCtrl, 0, wxTOP | wxBOTTOM | wxRIGHT, 10);

	row2Sizer->Add(hexCheckBox, 0, wxBOTTOM | wxRIGHT, 10);

	row3Sizer->Add(dataListBox, 0, wxLEFT | wxBOTTOM | wxRIGHT, 10);

	vSizer->Add(row1Sizer, 0, wxEXPAND);
	vSizer->Add(row2Sizer, 0, wxEXPAND);
	vSizer->Add(row3Sizer, 0, wxEXPAND);

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

	switch (dataTypeChoice->GetSelection())
	{
	case 0:
	{
		for (unsigned int i = 0; i < dataSection.size; i++)
		{
			uintptr_t address = imageBase + dataSection.virtualAddress + i;

			char addressStr[10];
			sprintf(addressStr, "%llX", address);

			char dataStr[50];
			if (hexCheckBox->IsChecked()) 
			{
				sprintf(dataStr, "%X", bytes[i]);
			}
			else 
			{
				sprintf(dataStr, "%d", bytes[i]);
			}

			dataListBox->AppendString(wxString(addressStr) + "\t" + wxString(dataStr));
		}
		break;
	}
	case 1:
	{
		for (unsigned int i = 0; i < dataSection.size; i += 2)
		{
			uintptr_t address = imageBase + dataSection.virtualAddress + i;

			char addressStr[10];
			sprintf(addressStr, "%llX", address);

			char dataStr[50];
			if (hexCheckBox->IsChecked())
			{
				sprintf(dataStr, "%X", *(short*)(bytes + i));
			}
			else
			{
				sprintf(dataStr, "%d", *(short*)(bytes + i));
			}

			dataListBox->AppendString(wxString(addressStr) + "\t" + wxString(dataStr));
		}
		break;
	}
	case 2:
	{
		for (unsigned int i = 0; i < dataSection.size; i += 4)
		{
			uintptr_t address = imageBase + dataSection.virtualAddress + i;

			char addressStr[10];
			sprintf(addressStr, "%llX", address);

			char dataStr[50];
			if (hexCheckBox->IsChecked())
			{
				sprintf(dataStr, "%X", *(int*)(bytes + i));
			}
			else
			{
				sprintf(dataStr, "%d", *(int*)(bytes + i));
			}

			dataListBox->AppendString(wxString(addressStr) + "\t" + wxString(dataStr));
		}
		break;
	}
	case 3:
	{
		for (unsigned int i = 0; i < dataSection.size; i += 8)
		{
			uintptr_t address = imageBase + dataSection.virtualAddress + i;

			char addressStr[10];
			sprintf(addressStr, "%llX", address);

			char dataStr[50];
			if (hexCheckBox->IsChecked())
			{
				sprintf(dataStr, "%llX", *(long long*)(bytes + i));
			}
			else
			{
				sprintf(dataStr, "%lld", *(long long*)(bytes + i));
			}

			dataListBox->AppendString(wxString(addressStr) + "\t" + wxString(dataStr));
		}
		break;
	}
	case 4:
	{
		for (unsigned int i = 0; i < dataSection.size; i += 4)
		{
			uintptr_t address = imageBase + dataSection.virtualAddress + i;

			char addressStr[10];
			sprintf(addressStr, "%llX", address);

			dataListBox->AppendString(wxString(addressStr) + "\t" + std::to_string(*(float*)(bytes + i)));
		}
		break;
	}
	case 5:
	{
		for (unsigned int i = 0; i < dataSection.size; i += 8)
		{
			uintptr_t address = imageBase + dataSection.virtualAddress + i;

			char addressStr[10];
			sprintf(addressStr, "%llX", address);

			dataListBox->AppendString(wxString(addressStr) + "\t" + std::to_string(*(double*)(bytes + i)));
		}
		break;
	}
	}
}

void DataViewer::OpenMenu(wxPoint position, uintptr_t imageBas, FileSection dataSec, unsigned char* dataBytes)
{
	position.x += 10;
	position.y += 10;
	SetPosition(position);
	Show();
	Raise();

	imageBase = imageBas;
	dataSection = dataSec;
	bytes = dataBytes;

	LoadData();
}

void DataViewer::CloseMenu(wxCloseEvent& e) // stops this frame from being destroyed and the data being lost
{
	Hide();
}
