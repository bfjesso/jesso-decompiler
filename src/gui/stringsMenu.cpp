#include "stringsMenu.h"
#include "colorsMenu.h"
#include <string>

wxBEGIN_EVENT_TABLE(StringsMenu, wxFrame)
EVT_CLOSE(StringsMenu::CloseMenu)
wxEND_EVENT_TABLE()

StringsMenu::StringsMenu() : wxFrame(nullptr, MainWindowID, "Strings", wxPoint(50, 50), wxSize(500, 600))
{
    SetOwnBackgroundColour(backgroundColor);

    stringsGrid = new wxGrid(this, wxID_ANY, wxPoint(0, 0), wxSize(500, 500));
	stringsGrid->SetLabelBackgroundColour(foregroundColor);
	stringsGrid->SetLabelTextColour(textColor);
	stringsGrid->SetDefaultCellBackgroundColour(gridColor);
	stringsGrid->SetDefaultCellTextColour(textColor);
	stringsGrid->CreateGrid(0, 2);
	stringsGrid->EnableGridLines(false);
	stringsGrid->ShowScrollbars(wxSHOW_SB_NEVER, wxSHOW_SB_ALWAYS);
	stringsGrid->SetScrollRate(0, 10);
	stringsGrid->SetSelectionMode(wxGrid::wxGridSelectionModes::wxGridSelectRows);
	stringsGrid->SetCellHighlightPenWidth(0);
	stringsGrid->SetCellHighlightROPenWidth(0);
	stringsGrid->DisableDragRowSize();
	stringsGrid->EnableEditing(false);
	stringsGrid->SetColLabelValue(0, "Address");
	stringsGrid->SetColLabelValue(1, "String");
	stringsGrid->HideRowLabels();
	stringsGrid->SetColSize(0, 200);
	stringsGrid->SetColSize(1, 9999);
	stringsGrid->SetColLabelAlignment(wxALIGN_LEFT, wxALIGN_CENTER);

    vSizer = new wxBoxSizer(wxVERTICAL);
    vSizer->Add(stringsGrid, 1, wxEXPAND);
    SetSizerAndFit(vSizer);
}

void StringsMenu::LoadStrings()
{
    if (!fileBytes)
    {
        wxMessageBox("No file bytes", "Can't load strings");
        return;
    }

    stringsGrid->Freeze();

    std::string currentStr = "";
    int numOfStrings = 0;

    int baseIndex = 0;
    for (int i = 0; i < numOfSections; i++)
    {
        unsigned char foundStr = 0;
        for (int j = 0; j < sections[i].size; j++)
        {
            char c = *(char*)(fileBytes + j + baseIndex);
            if (c > 31 && c < 127)
            {
                if (!foundStr)
                {
                    stringsGrid->AppendRows(1);

                    unsigned long long address = imageBase + sections[i].virtualAddress + j;
                    char addressStr[50] = { 0 };
                    sprintf(addressStr, "%llX", address);

                    stringsGrid->SetCellValue(numOfStrings, 0, std::string(addressStr) + std::string(sections[i].name.buffer));

                    currentStr = "";
                    foundStr = 1;
                }

                currentStr += c;

                if(j == sections[i].size - 1 && currentStr.length() > 1)
                {
                    stringsGrid->SetCellValue(numOfStrings, 1, currentStr);
                    numOfStrings++;
                }
            }
            else
            {
                if (foundStr && currentStr.length() > 1)
                {
                    stringsGrid->SetCellValue(numOfStrings, 1, currentStr);
                    numOfStrings++;
                }

                foundStr = 0;
            }
        }
        
        baseIndex += sections[i].size;
    }

    stringsGrid->Thaw();
}

void StringsMenu::ClearData()
{
    int rows = stringsGrid->GetNumberRows();
	if (rows > 0)
	{
		stringsGrid->DeleteRows(0, stringsGrid->GetNumberRows());
	}

    fileBytes = nullptr;
    sections = nullptr;
    numOfSections = 0;
    imageBase = 0;
}

void StringsMenu::OpenMenu(wxPoint position, unsigned long long imageBas, FileSection* secs, int numOfSecs, unsigned char* bytes)
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

        LoadStrings();
    }
}

void StringsMenu::CloseMenu(wxCloseEvent &e) // stops this frame from being destroyed and the data being lost
{
    Hide();
}
