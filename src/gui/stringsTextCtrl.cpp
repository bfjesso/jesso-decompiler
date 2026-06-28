#include "stringsTextCtrl.h"

StringsTextCtrl::StringsTextCtrl(wxWindow* parent, struct DecompilationParameters* decompParams, ColorsMenu* colorMenu) : JdcTextCtrl(parent)
{
	params = decompParams;
	colorsMenu = colorMenu;
	LoadStrings();
}

void StringsTextCtrl::LoadStrings()
{
    if (!params || params->numOfFileBytes == 0)
    {
        wxMessageBox("No file bytes", "Can't load strings");
        return;
    }

    SetReadOnly(false);
    Freeze();

    wxString stringsText = "";
    wxString currentStr = "";
    int numOfStrings = 0;

    for (int i = 0; i < params->numOfSections; i++)
    {
        unsigned char foundStr = 0;
        for (unsigned int j = 0; j < params->sections[i].size; j++)
        {
            char c = *(char*)(params->fileBytes + params->sections[i].fileOffset + j);
            if (c > 31 && c < 127)
            {
                if (!foundStr)
                {
                    currentStr = "";
                    foundStr = 1;
                }

                currentStr += c;
            }
            else
            {
                if (foundStr && c == 0 && currentStr.length() > 1)
                {
                    unsigned long long address = params->imageBase + params->sections[i].virtualAddress + j;
                    char addressStr[50] = { 0 };
                    sprintf(addressStr, "%llX", address);

                    stringsText += wxString(addressStr) + wxString(params->sections[i].name.buffer) + "\t\"" + currentStr + "\"\n";
                    numOfStrings++;
                }

                foundStr = 0;
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
        StyleSetForeground(i, colorsMenu->dataColors[i]);
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