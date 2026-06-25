#include "dataTextCtrl.h"
#include <string>

DataTextCtrl::DataTextCtrl(wxWindow* parent, const wxSize& size, ColorsMenu* colorMenu) : JdcTextCtrl(parent, size)
{
	colorsMenu = colorMenu;

	Bind(wxEVT_STC_UPDATEUI, &DataTextCtrl::OnUpdateDataUI, this);

	AddRightClickOption("Go to address", 'G', 0, [&](wxCommandEvent&) {
		wxTextEntryDialog dlg(this, "", "Go to address");
		if (dlg.ShowModal() == wxID_OK)
		{
			wxString txt = dlg.GetValue();
			unsigned long long address = 0;
			if (txt.ToULongLong(&address, 16))
			{
				if (address < imageBase) 
				{
					wxMessageBox("Address is smaller than the image base", "Failed to find address");
					return;
				}
				else if(address >= imageBase + numOfFileBytes)
				{
					wxMessageBox("Address is larger than the max address", "Failed to find address");
					return;
				}
				
				CenterLine((address - imageBase) / bytesPerLine);
				return;
			}

			wxMessageBox("Not valid hex number", "Failed to find address");
		}
	});

	for (int i = 0; i < NUM_OF_DATA_TEXT_CTRL_TYPES; i++) 
	{
		AddRightClickOption("Set display type to " + wxString(dataTypeStrs[i]), 0, 0, [&](wxCommandEvent&) {
			selectedType = (enum DataTextCtrlTypes)i;
			ResetTextCtrl();
		});
	}

	AddRightClickOption("Toggle hex display", 0, 0, [&](wxCommandEvent&) {
		isHex = !isHex;
		ResetTextCtrl();
	});
}

void DataTextCtrl::Initialize(unsigned long long baseOfImage, struct FileSection* fileSections, int amountOfSections, unsigned char* bytes, unsigned long long numOfBytes)
{
	imageBase = baseOfImage;
	sections = fileSections;
	numOfSections = amountOfSections;
	fileBytes = bytes;
	numOfFileBytes = numOfBytes;

	numOfLines = numOfFileBytes / bytesPerLine;
	if (numOfFileBytes % bytesPerLine != 0) 
	{
		numOfLines++;
	}

	ResetTextCtrl();
}

void DataTextCtrl::ResetTextCtrl()
{
	int ogLine = GetCurrentLine();
	
	ClearText();

	wxString newLines = "";
	for (int i = 0; i < numOfLines; i++)
	{
		newLines += "\n";
	}

	SetReadOnly(false);
	SetText(newLines);
	SetReadOnly(true);

	CenterLine(ogLine);

	UpdateTextCtrl();
}

void DataTextCtrl::ClearData()
{
	ClearText();

	fileBytes = nullptr;
	sections = nullptr;
	numOfSections = 0;
	imageBase = 0;
}

void DataTextCtrl::OnUpdateDataUI(wxStyledTextEvent& e)
{
	UpdateTextCtrl();
	OnUpdateUI(e);
}

void DataTextCtrl::UpdateTextCtrl()
{
	if (!fileBytes || numOfFileBytes == 0)
	{
		return;
	}

	int firstLine = GetFirstVisibleLine();
	int lastLine = firstLine + LinesOnScreen();
	if (GetLineLength(firstLine) != 0 && GetLineLength(lastLine) != 0)
	{
		return;
	}

	firstLine -= 100;
	if (firstLine < 0)
	{
		firstLine = 0;
	}

	lastLine += 100;
	if (lastLine > numOfLines)
	{
		lastLine = numOfLines;
	}

	SetReadOnly(false);
	Freeze();

	int typeSize = typeSizes[selectedType];

	char lineBuffer[512] = { 0 };
	for (unsigned long long i = firstLine * bytesPerLine; i < lastLine * bytesPerLine; i += bytesPerLine)
	{
		int lineLen = GetLineLength(i / bytesPerLine);
		if (lineLen != 0)
		{
			continue;
		}
		
		unsigned long long address = imageBase + i;
		struct FileSection* section = 0;
		for (int j = 0; j < numOfSections; j++) 
		{
			if (address >= sections[j].virtualAddress + imageBase && address < sections[j].virtualAddress + sections[j].size + imageBase) 
			{
				section = &sections[j];
				break;
			}
		}

		if (section) 
		{
			sprintf(lineBuffer, "0x%llX%s\t", address, section->name.buffer);
		}
		else 
		{
			sprintf(lineBuffer, "0x%llX\t", address);
		}

		for (unsigned int j = 0; j < bytesPerLine; j += typeSize)
		{
			if (i + j >= numOfFileBytes) 
			{
				break;
			}
			else if (i + j + typeSize > numOfFileBytes)
			{
				selectedType = ONE_BYTE_INT_TYPE;
				typeSize = 1;
			}
			
			switch (selectedType)
			{
			case ONE_BYTE_INT_TYPE:
			{
				if (isHex)
				{
					sprintf(lineBuffer + strlen(lineBuffer), "0x%02X ", fileBytes[i + j]);
				}
				else
				{
					sprintf(lineBuffer + strlen(lineBuffer), "%d ", fileBytes[i + j]);
				}
				break;
			}
			case TWO_BYTE_INT_TYPE:
			{
				if (isHex)
				{
					sprintf(lineBuffer + strlen(lineBuffer), "0x%04X ", *(unsigned short*)(fileBytes + i + j));
				}
				else
				{
					sprintf(lineBuffer + strlen(lineBuffer), "%d ", *(unsigned short*)(fileBytes + i + j));
				}
				break;
			}
			case FOUR_BYTE_INT_TYPE:
			{
				if (isHex)
				{
					sprintf(lineBuffer + strlen(lineBuffer), "0x%08X ", *(unsigned int*)(fileBytes + i + j));
				}
				else
				{
					sprintf(lineBuffer + strlen(lineBuffer), "%d ", *(unsigned int*)(fileBytes + i + j));
				}
				break;
			}
			case EIGHT_BYTE_INT_TYPE:
			{
				if (isHex)
				{
					sprintf(lineBuffer + strlen(lineBuffer), "0x%016llX ", *(unsigned long long*)(fileBytes + i + j));
				}
				else
				{
					sprintf(lineBuffer + strlen(lineBuffer), "%lld ", *(unsigned long long*)(fileBytes + i + j));
				}
				break;
			}
			case FLOAT_TYPE:
			{
				sprintf(lineBuffer + strlen(lineBuffer), "%0.8g ", *(float*)(fileBytes + i + j));
				break;
			}
			case DOUBLE_TYPE:
			{
				sprintf(lineBuffer + strlen(lineBuffer), "%0.16g ", *(double*)(fileBytes + i + j));
				break;
			}
			case ASCII_CHAR_TYPE:
			{
				char c = *(char*)(fileBytes + i + j);
				if(c > 31 && c < 127)
				{
					sprintf(lineBuffer + strlen(lineBuffer), "'%c' ", c);
				}
				else
				{
					sprintf(lineBuffer + strlen(lineBuffer), "0x%02X ", fileBytes[i + j]);			
				}
				break;
			}
			}
		}

		InsertText(PositionFromLine(i / bytesPerLine), lineBuffer);
	}

	ApplyDataHighlighting();
	Thaw();
	SetReadOnly(true);
}

void DataTextCtrl::ApplyDataHighlighting()
{
	for (int i = 0; i < NUM_OF_DATA_COLORS; i++)
	{
		StyleSetForeground(i, colorsMenu->dataColors[i]);
	}

	int firstLine = GetFirstVisibleLine();
	int lastLine = firstLine + LinesOnScreen();

	firstLine -= 99;
	if (firstLine < 0)
	{
		firstLine = 0;
	}

	lastLine += 99;
	if (lastLine > numOfLines)
	{
		lastLine = numOfLines;
	}

	int lineStart = PositionFromLine(firstLine) + 1;
	int end = PositionFromLine(lastLine);
	wxString dataText = GetValue();
	while (lineStart < end)
	{
		int dataStart = dataText.find("\t", lineStart);
		int lineEnd = dataText.find("\n", dataStart);
		if (dataStart != wxNOT_FOUND && lineEnd != wxNOT_FOUND)
		{
			StartStyling(dataStart);
			SetStyling(lineEnd - dataStart + 1, VALUE_DATA_COLOR);

			lineStart = lineEnd + 1;
		}
		else
		{
			break;
		}
	}
}

void DataTextCtrl::HighlightInstruction(unsigned long long address, int numOfBytes)
{
	IndicatorClearRange(0, GetTextLength());
	SetIndicatorCurrent(0);

	if (address >= imageBase && address < imageBase + numOfFileBytes)
	{
		int row = (address - imageBase) / bytesPerLine;
		int rowStart = PositionFromLine(row);
		int dataStart = FindText(rowStart, rowStart + 50, "\t") + 1;

		if (selectedType == ONE_BYTE_INT_TYPE)
		{
			int remainder = (address - imageBase) % bytesPerLine;
			int start = dataStart + (remainder * 5);
			if (numOfBytes + remainder > bytesPerLine) 
			{
				IndicatorFillRange(start, (5 * (bytesPerLine - remainder)) - 1);

				rowStart = PositionFromLine(row + 1);
				dataStart = FindText(rowStart, rowStart + 50, "\t") + 1; 
				IndicatorFillRange(dataStart, (5 * (numOfBytes - (bytesPerLine - remainder))) - 1);
			}
			else 
			{
				IndicatorFillRange(start, (5 * numOfBytes) - 1);
			}
		}
		else 
		{
			int dataEnd = FindText(rowStart, rowStart + 50, "\n");
			IndicatorFillRange(rowStart, dataEnd - rowStart - 1);
		}

		GotoLine(row);
	}
}