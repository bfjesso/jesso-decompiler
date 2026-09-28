#include "dataTextCtrl.h"
#include "mainGui.h"
#include "../file-handler/fileHandler.h"

#include <string>

DataTextCtrl::DataTextCtrl(wxWindow* parent, MainGui* mainGuiRef, wxString name) : JdcTextCtrl(parent, mainGuiRef, name)
{
	Bind(wxEVT_CONTEXT_MENU, &DataTextCtrl::DataRightClickOptions, this);
	Bind(wxEVT_CHAR_HOOK, &DataTextCtrl::OnDataKeyDown, this);
	Bind(wxEVT_STC_UPDATEUI, &DataTextCtrl::OnUpdateDataUI, this);

	Initialize();
}

void DataTextCtrl::Initialize()
{
	numOfLines = mainGui->decompParams.numOfFileBytes / bytesPerLine;
	if (mainGui->decompParams.numOfFileBytes % bytesPerLine != 0) 
	{
		numOfLines++;
	}

	ResetTextCtrl();
}

void DataTextCtrl::ResetTextCtrl()
{
	int32_t ogLine = GetCurrentLine();

	wxString newLines = "";
	for (int32_t i = 0; i < numOfLines; i++)
	{
		newLines += "\n";
	}

	SetReadOnly(false);
	SetText(newLines);
	SetReadOnly(true);

	CenterLine(ogLine);

	UpdateTextCtrl();
}

void DataTextCtrl::ShowGoToVirtualAddressDialog()
{
	wxTextEntryDialog dlg(this, "", "Go to virtual address");
	if (dlg.ShowModal() == wxID_OK)
	{
		wxString txt = dlg.GetValue();
		uint64_t address = 0;
		if (txt.ToULongLong((wxULongLong_t*)(&address), 16))
		{
			if (address < mainGui->decompParams.imageBase)
			{
				wxMessageBox("Address is smaller than the image base", "Failed to find address");
				return;
			}

			FileSection* section = 0;
			uint64_t fileOffset = rvaToFileOffset(mainGui->decompParams.sections, mainGui->decompParams.numOfSections, address - mainGui->decompParams.imageBase, &section);
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

void DataTextCtrl::ShowGoToFileOffsetDialog()
{
	wxTextEntryDialog dlg(this, "", "Go to file offset");
	if (dlg.ShowModal() == wxID_OK)
	{
		wxString txt = dlg.GetValue();
		uint64_t fileOffset = 0;
		if (txt.ToULongLong((wxULongLong_t*)(&fileOffset), 16))
		{
			if (fileOffset >= mainGui->decompParams.numOfFileBytes)
			{
				wxMessageBox("File offset is larger than the file", "Failed to find file offset");
				return;
			}

			HighlightBytes(fileOffset, 1, YELLOW_INDICATOR);
			return;
		}

		wxMessageBox("Not valid hex number", "Failed to find file offset");
	}
}

void DataTextCtrl::DataRightClickOptions(wxContextMenuEvent& e)
{
	wxMenu menu;

	const int32_t ID_CHANGE_DISPLAY_TYPE = 100;
	const int32_t ID_HEX = 101;
	const int32_t ID_SIGNED = 102;
	const int32_t ID_GO_TO_VIRTUAL_ADDRESS = 103;
	const int32_t ID_GO_TO_FILE_OFFSET = 104;

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

		menu.AppendCheckItem(ID_SIGNED, "Signed");
		menu.Check(ID_SIGNED, isSigned);
		menu.Bind(wxEVT_MENU, [&](wxCommandEvent& e) {
			isSigned = e.IsChecked();
			ResetTextCtrl();
		}, ID_SIGNED);
	}

	menu.Append(ID_GO_TO_VIRTUAL_ADDRESS, "Go to virtual address");
	menu.Bind(wxEVT_MENU, [&](wxCommandEvent& e) {
		ShowGoToVirtualAddressDialog();
	}, ID_GO_TO_VIRTUAL_ADDRESS);

	menu.Append(ID_GO_TO_FILE_OFFSET, "Go to file offset");
	menu.Bind(wxEVT_MENU, [&](wxCommandEvent& e) {
		ShowGoToFileOffsetDialog();
	}, ID_GO_TO_FILE_OFFSET);

	AddDefaultRightClickOptions(&menu);

	PopupMenu(&menu, ScreenToClient(e.GetPosition()));
}

void DataTextCtrl::OnDataKeyDown(wxKeyEvent& e)
{
	int32_t key = e.GetKeyCode();
	if (key == 'G')
	{
		if ((e.GetModifiers() & wxMOD_CONTROL) != 0)
		{
			ShowGoToVirtualAddressDialog();
		}
		else 
		{
			ShowGoToFileOffsetDialog();
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
	if (mainGui->decompParams.numOfFileBytes == 0)
	{
		return;
	}

	int32_t firstLine = GetFirstVisibleLine();
	int32_t lastLine = firstLine + LinesOnScreen();
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

	int32_t typeSize = typeSizes[selectedType];

	char lineBuffer[512] = { 0 };
	for (uint64_t i = firstLine * bytesPerLine; i < lastLine * bytesPerLine; i += bytesPerLine)
	{
		int32_t lineLen = GetLineLength(i / bytesPerLine);
		if (lineLen != 0)
		{
			continue;
		}
		
		struct FileSection* section = 0;
		for (int32_t j = 0; j < mainGui->decompParams.numOfSections; j++)
		{
			if (i >= mainGui->decompParams.sections[j].fileOffset && i < mainGui->decompParams.sections[j].fileOffset + mainGui->decompParams.sections[j].physicalSize)
			{
				section = &mainGui->decompParams.sections[j];
				break;
			}
		}

		if (section) 
		{
			sprintf(lineBuffer, "0x%llX%s (0x%llX)\t", mainGui->decompParams.imageBase + section->rva + (i - section->fileOffset), section->name.buffer, i);
		}
		else 
		{
			sprintf(lineBuffer, "no section (0x%llX)\t", i);
		}

		for (uint32_t j = 0; j < bytesPerLine; j += typeSize)
		{
			if (i + j >= mainGui->decompParams.numOfFileBytes)
			{
				break;
			}
			else if (i + j + typeSize > mainGui->decompParams.numOfFileBytes)
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
				if (isHex && isSigned) 
				{ 
					int8_t val = (int8_t)mainGui->decompParams.fileBytes[i + j];
					if (val < 0) { sprintf(lineBuffer + strlen(lineBuffer), "-0x%02X", -val); }
					else { sprintf(lineBuffer + strlen(lineBuffer), "0x%02X", val); }
				}
				else if (isHex && !isSigned) { sprintf(lineBuffer + strlen(lineBuffer), "0x%02X", mainGui->decompParams.fileBytes[i + j]); }
				else if (!isHex && isSigned) { sprintf(lineBuffer + strlen(lineBuffer), "%d", (int8_t)mainGui->decompParams.fileBytes[i + j]); }
				else { sprintf(lineBuffer + strlen(lineBuffer), "%u", mainGui->decompParams.fileBytes[i + j]); }
				break;
			}
			case TWO_BYTE_INT_TYPE:
			{
				if (isHex && isSigned)
				{
					int16_t val = *(int16_t*)(mainGui->decompParams.fileBytes + i + j);
					if (val < 0) { sprintf(lineBuffer + strlen(lineBuffer), "-0x%04X", -val); }
					else { sprintf(lineBuffer + strlen(lineBuffer), "0x%04X", val); }
				}
				else if (isHex && !isSigned) { sprintf(lineBuffer + strlen(lineBuffer), "0x%04X", *(uint16_t*)(mainGui->decompParams.fileBytes + i + j)); }
				else if (!isHex && isSigned) { sprintf(lineBuffer + strlen(lineBuffer), "%d", *(int16_t*)(mainGui->decompParams.fileBytes + i + j)); }
				else { sprintf(lineBuffer + strlen(lineBuffer), "%u", *(uint16_t*)(mainGui->decompParams.fileBytes + i + j)); }
				break;
			}
			case FOUR_BYTE_INT_TYPE:
			{
				if (isHex && isSigned)
				{
					int32_t val = *(int32_t*)(mainGui->decompParams.fileBytes + i + j);
					if (val < 0) { sprintf(lineBuffer + strlen(lineBuffer), "-0x%08X", -val); }
					else { sprintf(lineBuffer + strlen(lineBuffer), "0x%08X", val); }
				}
				else if (isHex && !isSigned) { sprintf(lineBuffer + strlen(lineBuffer), "0x%08X", *(uint32_t*)(mainGui->decompParams.fileBytes + i + j)); }
				else if (!isHex && isSigned) { sprintf(lineBuffer + strlen(lineBuffer), "%d", *(int32_t*)(mainGui->decompParams.fileBytes + i + j)); }
				else { sprintf(lineBuffer + strlen(lineBuffer), "%u", *(uint32_t*)(mainGui->decompParams.fileBytes + i + j)); }
				break;
			}
			case EIGHT_BYTE_INT_TYPE:
			{
				if (isHex && isSigned)
				{
					int64_t val = *(int64_t*)(mainGui->decompParams.fileBytes + i + j);
					if (val < 0) { sprintf(lineBuffer + strlen(lineBuffer), "-0x%016llX", -val); }
					else { sprintf(lineBuffer + strlen(lineBuffer), "0x%016llX", val); }
				}
				else if (isHex && !isSigned) { sprintf(lineBuffer + strlen(lineBuffer), "0x%016llX", *(uint64_t*)(mainGui->decompParams.fileBytes + i + j)); }
				else if (!isHex && isSigned) { sprintf(lineBuffer + strlen(lineBuffer), "%lld", *(int64_t*)(mainGui->decompParams.fileBytes + i + j)); }
				else { sprintf(lineBuffer + strlen(lineBuffer), "%llu", *(uint64_t*)(mainGui->decompParams.fileBytes + i + j)); }
				break;
			}
			case FLOAT_TYPE:
			{
				sprintf(lineBuffer + strlen(lineBuffer), "%0.8g", *(float*)(mainGui->decompParams.fileBytes + i + j));
				break;
			}
			case DOUBLE_TYPE:
			{
				sprintf(lineBuffer + strlen(lineBuffer), "%0.16g", *(double*)(mainGui->decompParams.fileBytes + i + j));
				break;
			}
			case ASCII_CHAR_TYPE:
			{
				char c = *(char*)(mainGui->decompParams.fileBytes + i + j);
				if(c >= ' ' && c <= '~')
				{
					sprintf(lineBuffer + strlen(lineBuffer), "'%c'", c);
				}
				else
				{
					sprintf(lineBuffer + strlen(lineBuffer), "0x%02X", mainGui->decompParams.fileBytes[i + j]);
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
	for (int32_t i = 0; i < NUM_OF_DATA_COLORS; i++)
	{
		StyleSetForeground(i, mainGui->colorsMenu->dataColors[i]);
	}

	int32_t firstLine = GetFirstVisibleLine();
	int32_t lastLine = firstLine + LinesOnScreen();

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

	int32_t lineStart = PositionFromLine(firstLine) + 1;
	int32_t end = PositionFromLine(lastLine);
	wxString dataText = GetValue();
	while (lineStart < end)
	{
		int32_t dataStart = dataText.find("\t", lineStart);
		int32_t lineEnd = dataText.find("\n", dataStart);
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

void DataTextCtrl::HighlightBytes(uint64_t fileOffset, uint32_t numOfBytes, enum IndicatorColor color)
{
	ClearIndicators();
	SetIndicatorCurrent(color);

	if (fileOffset < mainGui->decompParams.numOfFileBytes)
	{
		int32_t row = fileOffset / bytesPerLine;
		CenterLine(row);
		UpdateTextCtrl();

		int32_t rowStart = PositionFromLine(row);
		int32_t dataStart = FindText(rowStart, rowStart + 50, "\t") + 1;

		if (selectedType == ONE_BYTE_INT_TYPE && isHex && !isSigned)
		{
			uint32_t remainder = fileOffset % bytesPerLine;
			int32_t start = dataStart + (remainder * 5);
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
			int32_t dataEnd = FindText(rowStart, rowStart + 50, "\n");
			IndicatorFillRange(rowStart, dataEnd - rowStart);
		}
	}
}