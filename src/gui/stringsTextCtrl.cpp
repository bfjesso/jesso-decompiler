#include "stringsTextCtrl.h"
#include "mainGui.h"
#include "../decompiler/decompilationUtils.h"

StringsTextCtrl::StringsTextCtrl(wxWindow* parent, MainGui* mainGuiRef) : JdcTextCtrl(parent, mainGuiRef, "Strings")
{
    Bind(wxEVT_CONTEXT_MENU, &StringsTextCtrl::StringsRightClickOptions, this);
    Bind(wxEVT_CHAR_HOOK, &StringsTextCtrl::OnStringsKeyDown, this);

	LoadStrings();
}

void StringsTextCtrl::ShowFindAddressDialog()
{
	wxTextEntryDialog dlg(this, "", "Find address");
	if (dlg.ShowModal() == wxID_OK)
	{
		wxString txt = dlg.GetValue();
		uint64_t address = 0;
		if (txt.ToULongLong(&address, 16))
		{
			int32_t index = findAddressInArr(foundAddresses.data(), foundAddresses.size(), address);
			if (index == -1)
			{
				wxMessageBox("Address not found", "Failed to find address");
				return;
			}

			HighlightLine(index, YELLOW_INDICATOR, 1);
			return;
		}

		wxMessageBox("Not valid hex number", "Failed to find address");
	}
}

void StringsTextCtrl::StringsRightClickOptions(wxContextMenuEvent& e)
{
	wxMenu menu;

	const int32_t ID_FIND_ADDRESS = 100;

	menu.Append(ID_FIND_ADDRESS, "Find string by address");
	menu.Bind(wxEVT_MENU, [&](wxCommandEvent&) {
		ShowFindAddressDialog();
	}, ID_FIND_ADDRESS);

	AddDefaultRightClickOptions(&menu);

	PopupMenu(&menu, ScreenToClient(e.GetPosition()));
}

void StringsTextCtrl::OnStringsKeyDown(wxKeyEvent& e)
{
	int32_t key = e.GetKeyCode();
	if ((e.GetModifiers() & wxMOD_CONTROL) != 0 && key != 0)
	{
		if (key == 'G')
		{
			ShowFindAddressDialog();
		}
	}

	OnKeyDown(e);
	e.Skip();
}

void StringsTextCtrl::LoadStrings()
{
    if (mainGui->decompParams.numOfFileBytes == 0)
    {
        wxMessageBox("No file bytes", "Can't load strings");
        return;
    }

    foundAddresses.clear();
    foundAddresses.shrink_to_fit();

    SetReadOnly(false);
    Freeze();

    wxString stringsText = "";
    wxString currentStr = "";
    int32_t numOfStrings = 0;

    for (int32_t i = 0; i < mainGui->decompParams.numOfSections; i++)
    {
        int32_t startIndex = -1;
        for (uint32_t j = 0; j < mainGui->decompParams.sections[i].physicalSize; j++)
        {
            char c = *(char*)(mainGui->decompParams.fileBytes + mainGui->decompParams.sections[i].fileOffset + j);
            if (c >= ' ' && c <= '~')
            {
                if (startIndex == -1)
                {
                    currentStr = "";
                    startIndex = j;
                }

                currentStr += c;
            }
            else
            {
                if (startIndex != -1 && c == 0 && currentStr.length() > 1)
                {
                    uint64_t address = mainGui->decompParams.imageBase + mainGui->decompParams.sections[i].rva + startIndex;
                    foundAddresses.push_back(address);

                    char addressStr[50] = { 0 };
                    sprintf(addressStr, "0x%llX", address);

                    stringsText += wxString(addressStr) + wxString(mainGui->decompParams.sections[i].name.buffer) + "\t\"" + currentStr + "\"\n";
                    numOfStrings++;
                }

                startIndex = -1;
            }
        }
    }

    SetText(stringsText);
    ApplyStringsHighlighting();
    Thaw();
    SetReadOnly(true);
}

void StringsTextCtrl::ApplyStringsHighlighting()
{
    for (int32_t i = 0; i < NUM_OF_DATA_COLORS; i++)
    {
        StyleSetForeground(i, mainGui->colorsMenu->dataColors[i]);
    }

    int32_t lineStart = 0;
    wxString dataText = GetValue();
    int32_t end = dataText.length();
    while (lineStart < end)
    {
        int32_t stringStart = dataText.find("\t", lineStart);
        int32_t lineEnd = dataText.find("\n", stringStart);
        if (stringStart != wxNOT_FOUND && lineEnd != wxNOT_FOUND)
        {
            StartStyling(stringStart);
            SetStyling(lineEnd - stringStart + 1, STRING_DATA_COLOR);

            lineStart = lineEnd + 1;
        }
        else
        {
            break;
        }
    }
}