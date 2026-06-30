#include "dataTextCtrl.h"
#include "../file-handler/fileHandler.h"

#include <string>

DataTextCtrl::DataTextCtrl(wxWindow* parent, wxString name, struct DecompilationParameters* decompParams, ColorsMenu* colorMenu) : JdcTextCtrl(parent, name)
{
	params = decompParams;
	colorsMenu = colorMenu;

	Bind(wxEVT_CONTEXT_MENU, &DataTextCtrl::DataRightClickOptions, this);
	Bind(wxEVT_CHAR_HOOK, &DataTextCtrl::OnDataKeyDown, this);
	Bind(wxEVT_STC_UPDATEUI, &DataTextCtrl::OnUpdateDataUI, this);
}

void DataTextCtrl::Initialize()
{
	numOfLines = params->numOfFileBytes / bytesPerLine;
	if (params->numOfFileBytes % bytesPerLine != 0) 
	{
		numOfLines++;
	}

	ResetTextCtrl();
}

void DataTextCtrl::ResetTextCtrl()
{
	int ogLine = GetCurrentLine();

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

void DataTextCtrl::ShowGoToAddressDialog()
{
	wxTextEntryDialog dlg(this, "", "Go to address");
	if (dlg.ShowModal() == wxID_OK)
	{
		wxString txt = dlg.GetValue();
		unsigned long long address = 0;
		if (txt.ToULongLong(&address, 16))
		{
			if (address < params->imageBase)
			{
				wxMessageBox("Address is smaller than the image base", "Failed to find address");
				return;
			}

			FileSection* section = 0;
			unsigned long long fileOffset = rvaToFileOffset(params->sections, params->numOfSections, address - params->imageBase, &section);
			if (!section) 
			{
				wxMessageBox("Address is not within a section", "Failed to find address");
				return;
			}

			HighlightBytes(fileOffset, 1, YELLOW_INDICATOR);
			return;
		}

		wxMessageBox("Not valid hex number", "Failed to find address");
	}
}

void DataTextCtrl::DataRightClickOptions(wxContextMenuEvent& e)
{
	wxMenu menu;

	const int ID_CHANGE_DISPLAY_TYPE = 100;
	const int ID_HEX = 101;

	menu.Append(ID_CHANGE_DISPLAY_TYPE, "Change display type");
	menu.Bind(wxEVT_MENU, [&](wxCommandEvent&) {
		wxSingleChoiceDialog choiceDialog(this, "", "Choose a type", wxArrayString(NUM_OF_DATA_TEXT_CTRL_TYPES, dataTypeStrs));
		if (choiceDialog.ShowModal() != wxID_CANCEL)
		{
			selectedType = (enum DataTextCtrlTypes)(choiceDialog.GetSelection());
			ResetTextCtrl();
		}
	}, ID_CHANGE_DISPLAY_TYPE);

	if (selectedType >= ONE_BYTE_INT_TYPE && selectedType <= EIGHT_BYTE_INT_TYPE) 
	{
		menu.AppendCheckItem(ID_HEX, "Hex");
		menu.Check(ID_HEX, isHex);
		menu.Bind(wxEVT_MENU, [&](wxCommandEvent& e) {
			isHex = e.IsChecked();
			ResetTextCtrl();
		}, ID_HEX);
	}

	AddDefaultRightClickOptions(&menu);

	PopupMenu(&menu, ScreenToClient(e.GetPosition()));
}

void DataTextCtrl::OnDataKeyDown(wxKeyEvent& e)
{
	int key = e.GetKeyCode();
	if ((e.GetModifiers() & wxMOD_CONTROL) != 0 && key != 0)
	{
		if (key == 'G')
		{
			ShowGoToAddressDialog();
		}
	}

	OnKeyDown(e);
	e.Skip();
}

void DataTextCtrl::OnUpdateDataUI(wxStyledTextEvent& e)
{
	UpdateTextCtrl();
	OnUpdateUI(e);
}

void DataTextCtrl::UpdateTextCtrl()
{
	if (params->numOfFileBytes == 0)
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
		
		struct FileSection* section = 0;
		for (int j = 0; j < params->numOfSections; j++)
		{
			if (i >= params->sections[j].fileOffset && i < params->sections[j].fileOffset + params->sections[j].physicalSize)
			{
				section = &params->sections[j];
				break;
			}
		}

		if (section) 
		{
			sprintf(lineBuffer, "0x%llX%s (0x%llX)\t", params->imageBase + section->rva + (i - section->fileOffset), section->name.buffer, i);
		}
		else 
		{
			sprintf(lineBuffer, "no section (0x%llX)\t", i);
		}

		for (unsigned int j = 0; j < bytesPerLine; j += typeSize)
		{
			if (i + j >= params->numOfFileBytes)
			{
				break;
			}
			else if (i + j + typeSize > params->numOfFileBytes)
			{
				selectedType = ONE_BYTE_INT_TYPE;
				typeSize = 1;
			}

			if (j != 0)
			{
				strcat(lineBuffer, " ");
			}
			
			switch (selectedType)
			{
			case ONE_BYTE_INT_TYPE:
			{
				if (isHex)
				{
					sprintf(lineBuffer + strlen(lineBuffer), "0x%02X", params->fileBytes[i + j]);
				}
				else
				{
					sprintf(lineBuffer + strlen(lineBuffer), "%d", params->fileBytes[i + j]);
				}
				break;
			}
			case TWO_BYTE_INT_TYPE:
			{
				if (isHex)
				{
					sprintf(lineBuffer + strlen(lineBuffer), "0x%04X", *(unsigned short*)(params->fileBytes + i + j));
				}
				else
				{
					sprintf(lineBuffer + strlen(lineBuffer), "%d", *(unsigned short*)(params->fileBytes + i + j));
				}
				break;
			}
			case FOUR_BYTE_INT_TYPE:
			{
				if (isHex)
				{
					sprintf(lineBuffer + strlen(lineBuffer), "0x%08X", *(unsigned int*)(params->fileBytes + i + j));
				}
				else
				{
					sprintf(lineBuffer + strlen(lineBuffer), "%d", *(unsigned int*)(params->fileBytes + i + j));
				}
				break;
			}
			case EIGHT_BYTE_INT_TYPE:
			{
				if (isHex)
				{
					sprintf(lineBuffer + strlen(lineBuffer), "0x%016llX", *(unsigned long long*)(params->fileBytes + i + j));
				}
				else
				{
					sprintf(lineBuffer + strlen(lineBuffer), "%lld", *(unsigned long long*)(params->fileBytes + i + j));
				}
				break;
			}
			case FLOAT_TYPE:
			{
				sprintf(lineBuffer + strlen(lineBuffer), "%0.8g", *(float*)(params->fileBytes + i + j));
				break;
			}
			case DOUBLE_TYPE:
			{
				sprintf(lineBuffer + strlen(lineBuffer), "%0.16g", *(double*)(params->fileBytes + i + j));
				break;
			}
			case ASCII_CHAR_TYPE:
			{
				char c = *(char*)(params->fileBytes + i + j);
				if(c > 31 && c < 127)
				{
					sprintf(lineBuffer + strlen(lineBuffer), "'%c'", c);
				}
				else
				{
					sprintf(lineBuffer + strlen(lineBuffer), "0x%02X", params->fileBytes[i + j]);
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

void DataTextCtrl::HighlightBytes(unsigned long long fileOffset, int numOfBytes, enum IndicatorColor color)
{
	ClearIndicators();
	SetIndicatorCurrent(color);

	if (fileOffset < params->numOfFileBytes)
	{
		int row = fileOffset / bytesPerLine;
		int rowStart = PositionFromLine(row);
		int dataStart = FindText(rowStart, rowStart + 50, "\t") + 1;

		if (selectedType == ONE_BYTE_INT_TYPE && isHex)
		{
			int remainder = fileOffset % bytesPerLine;
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
			IndicatorFillRange(rowStart, dataEnd - rowStart);
		}

		CenterLine(row);
	}
}