#include "dataViewerMenu.h"
#include "colorsMenu.h"
#include <string>

wxBEGIN_EVENT_TABLE(DataViewer, wxFrame)
EVT_CLOSE(DataViewer::CloseMenu)
EVT_CHOICE(DataTypeChoiceID, DataViewer::UpdateDataList)
EVT_CHOICE(SectionChoiceID, DataViewer::UpdateDataList)
EVT_CHECKBOX(HexCheckBoxID, DataViewer::UpdateDataList)
wxEND_EVENT_TABLE()

DataViewer::DataViewer() : wxFrame(nullptr, MainWindowID, "Data Viewer", wxPoint(50, 50), wxSize(500, 250))
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

	dataTextCtrl = new JdcTextCtrl(this, wxSize(500, 250), DATA_CTRL_TYPE, 0, 0, 0);

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

	dataTextCtrl->ClearText();

	dataTextCtrl->SetReadOnly(false);
	dataTextCtrl->Freeze();

	int typeSelection = dataTypeChoice->GetSelection();
	int typeSize = typeSizes[typeSelection];
	int sectionSelection = sectionChoice->GetSelection();
	unsigned char isHex = hexCheckBox->IsChecked();

	std::string dataText;
	dataText.reserve(sections[sectionSelection].size * 6);

	char lineBuffer[512] = { 0 };
	for (unsigned int i = 0; i < sections[sectionSelection].size; i += bytesPerLine)
	{
		unsigned long long address = imageBase + sections[sectionSelection].virtualAddress + i;
		sprintf(lineBuffer, "%llX%s\t", address, sections[sectionSelection].name.buffer);

		for (unsigned int j = 0; j < bytesPerLine; j += typeSize)
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
					sprintf(lineBuffer + strlen(lineBuffer), "0x%02X ", fileBytes[i + j + sections[sectionSelection].fileOffset]);
				}
				else
				{
					sprintf(lineBuffer + strlen(lineBuffer), "%d ", fileBytes[i + j + sections[sectionSelection].fileOffset]);
				}
				break;
			}
			case 1: // 2-byte int
			{
				if (isHex)
				{
					sprintf(lineBuffer + strlen(lineBuffer), "0x%04X ", *(unsigned short*)(fileBytes + i + j + sections[sectionSelection].fileOffset));
				}
				else
				{
					sprintf(lineBuffer + strlen(lineBuffer), "%d ", *(unsigned short*)(fileBytes + i + j + sections[sectionSelection].fileOffset));
				}
				break;
			}
			case 2: // 4-byte int
			{
				if (isHex)
				{
					sprintf(lineBuffer + strlen(lineBuffer), "0x%08X ", *(unsigned int*)(fileBytes + i + j + sections[sectionSelection].fileOffset));
				}
				else
				{
					sprintf(lineBuffer + strlen(lineBuffer), "%d ", *(unsigned int*)(fileBytes + i + j + sections[sectionSelection].fileOffset));
				}
				break;
			}
			case 3: // 8-byte int
			{
				if (isHex)
				{
					sprintf(lineBuffer + strlen(lineBuffer), "0x%016llX ", *(unsigned long long*)(fileBytes + i + j + sections[sectionSelection].fileOffset));
				}
				else
				{
					sprintf(lineBuffer + strlen(lineBuffer), "%lld ", *(unsigned long long*)(fileBytes + i + j + sections[sectionSelection].fileOffset));
				}
				break;
			}
			case 4: // float
			{
				sprintf(lineBuffer + strlen(lineBuffer), "%0.8g ", *(float*)(fileBytes + i + j + sections[sectionSelection].fileOffset));
				break;
			}
			case 5: // double
			{
				sprintf(lineBuffer + strlen(lineBuffer), "%0.16g ", *(double*)(fileBytes + i + j + sections[sectionSelection].fileOffset));
				break;
			}
			case 6: // ASCII character
			{
				char c = *(char*)(fileBytes + i + j + sections[sectionSelection].fileOffset);
				if(c > 31 && c < 127)
				{
					sprintf(lineBuffer + strlen(lineBuffer), "'%c' ", c);
				}
				else
				{
					sprintf(lineBuffer + strlen(lineBuffer), "0x%02X ", fileBytes[i + j + sections[sectionSelection].fileOffset]);			
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

void DataViewer::HighlightInstruction(unsigned long long address, int numOfBytes)
{
	dataTextCtrl->IndicatorClearRange(0, dataTextCtrl->GetTextLength());
	dataTextCtrl->SetIndicatorCurrent(0);

	FileSection* section = &sections[sectionChoice->GetSelection()];
	unsigned long long sectionStart = section->virtualAddress + imageBase;
	if (address >= sectionStart && address < sectionStart + section->size)
	{
		int row = (address - sectionStart) / bytesPerLine;
		int rowStart = dataTextCtrl->PositionFromLine(row);
		int dataStart = dataTextCtrl->FindText(rowStart, rowStart + 50, "\t") + 1;

		if (dataTypeChoice->GetSelection() == 0) // 1 byte
		{
			int remainder = (address - sectionStart) % bytesPerLine;
			int start = dataStart + (remainder * 5);
			if (numOfBytes + remainder > bytesPerLine) 
			{
				dataTextCtrl->IndicatorFillRange(start, (5 * (bytesPerLine - remainder)) - 1);

				rowStart = dataTextCtrl->PositionFromLine(row + 1);
				dataStart = dataTextCtrl->FindText(rowStart, rowStart + 50, "\t") + 1; 
				dataTextCtrl->IndicatorFillRange(dataStart, (5 * (numOfBytes - (bytesPerLine - remainder))) - 1);
			}
			else 
			{
				dataTextCtrl->IndicatorFillRange(start, (5 * numOfBytes) - 1);
			}
		}
		else 
		{
			int dataEnd = dataTextCtrl->FindText(rowStart, rowStart + 50, "\n");
			dataTextCtrl->IndicatorFillRange(rowStart, dataEnd - rowStart - 1);
		}

		dataTextCtrl->GotoLine(row);
	}
}

void DataViewer::ClearData() 
{
	dataTextCtrl->ClearText();

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
