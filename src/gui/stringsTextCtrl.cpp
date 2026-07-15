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
		unsigned long long address = 0;
		if (txt.ToULongLong(&address, 16))
		{
			int index = findAddressInArr(foundAddresses.data(), foundAddresses.size(), address);
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

	const int ID_FIND_ADDRESS = 100;

	menu.Append(ID_FIND_ADDRESS, "Find string by address");
	menu.Bind(wxEVT_MENU, [&](wxCommandEvent&) {
		ShowFindAddressDialog();
	}, ID_FIND_ADDRESS);

	AddDefaultRightClickOptions(&menu);

	PopupMenu(&menu, ScreenToClient(e.GetPosition()));
}

void StringsTextCtrl::OnStringsKeyDown(wxKeyEvent& e)
{
	int key = e.GetKeyCode();
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
    int numOfStrings = 0;

    for (int i = 0; i < mainGui->decompParams.numOfSections; i++)
    {
        int startIndex = -1;
        for (unsigned int j = 0; j < mainGui->decompParams.sections[i].physicalSize; j++)
        {
            char c = *(char*)(mainGui->decompParams.fileBytes + mainGui->decompParams.sections[i].fileOffset + j);
            if (c > 31 && c < 127)
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
                    unsigned long long address = mainGui->decompParams.imageBase + mainGui->decompParams.sections[i].rva + startIndex;
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
    for (int i = 0; i < NUM_OF_DATA_COLORS; i++)
    {
        StyleSetForeground(i, mainGui->colorsMenu->dataColors[i]);
    }

    int lineStart = 0;
    wxString dataText = GetValue();
    int end = dataText.length();
    while (lineStart < end)
    {
        int stringStart = dataText.find("\t", lineStart);
        int lineEnd = dataText.find("\n", stringStart);
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