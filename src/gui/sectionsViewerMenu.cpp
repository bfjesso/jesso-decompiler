#include "sectionsViewerMenu.h"

wxBEGIN_EVENT_TABLE(SectionsViewer, wxFrame)
EVT_CLOSE(SectionsViewer::CloseMenu)
EVT_GRID_CELL_RIGHT_CLICK(SectionsViewer::GridRightClickOptions)
wxEND_EVENT_TABLE()

SectionsViewer::SectionsViewer() : wxFrame(nullptr, MainWindowID, "File Sections", wxPoint(50, 50), wxSize(600, 600))
{
	SetOwnBackgroundColour(backgroundColor);

	sectionsGrid = new wxGrid(this, wxID_ANY, wxPoint(0, 0), wxSize(600, 600));
	sectionsGrid->SetLabelBackgroundColour(foregroundColor);
	sectionsGrid->SetLabelTextColour(textColor);
	sectionsGrid->SetDefaultCellBackgroundColour(gridColor);
	sectionsGrid->SetDefaultCellTextColour(textColor);
	sectionsGrid->CreateGrid(0, 6);
	sectionsGrid->EnableGridLines(false);
	sectionsGrid->ShowScrollbars(wxSHOW_SB_NEVER, wxSHOW_SB_ALWAYS);
	sectionsGrid->SetScrollRate(0, 10);
	sectionsGrid->SetSelectionMode(wxGrid::wxGridSelectionModes::wxGridSelectRows);
	sectionsGrid->SetCellHighlightPenWidth(0);
	sectionsGrid->SetCellHighlightROPenWidth(0);
	sectionsGrid->DisableDragRowSize();
	sectionsGrid->EnableEditing(false);
	sectionsGrid->SetColLabelValue(0, "Name");
	sectionsGrid->SetColLabelValue(1, "Type");
	sectionsGrid->SetColLabelValue(2, "Is readonly");
	sectionsGrid->SetColLabelValue(3, "Virtual address");
	sectionsGrid->SetColLabelValue(4, "File offset");
	sectionsGrid->SetColLabelValue(5, "Size");
	sectionsGrid->HideRowLabels();
	sectionsGrid->SetColSize(0, 100);
	sectionsGrid->SetColSize(1, 100);
	sectionsGrid->SetColSize(2, 100);
	sectionsGrid->SetColSize(3, 100);
	sectionsGrid->SetColSize(4, 100);
	sectionsGrid->SetColSize(5, 9999);
	sectionsGrid->SetColLabelAlignment(wxALIGN_LEFT, wxALIGN_CENTER);

	vSizer = new wxBoxSizer(wxVERTICAL);

	vSizer->Add(sectionsGrid, 1, wxEXPAND);

	SetSizerAndFit(vSizer);
}

void SectionsViewer::OpenMenu(wxPoint position, FileSection* sections, int numOfSections)
{
	position.x += 10;
	position.y += 10;
	SetPosition(position);
	Show();
	Raise();

	int rows = sectionsGrid->GetNumberRows();
	if (rows > 0)
	{
		sectionsGrid->DeleteRows(0, sectionsGrid->GetNumberRows());
	}

	for (int i = 0; i < numOfSections; i++)
	{
		sectionsGrid->AppendRows(1);

		sectionsGrid->SetCellValue(i, 0, wxString(sections[i].name.buffer));
		sectionsGrid->SetCellValue(i, 1, wxString(fileSectionTypeStrs[sections[i].type]));

		if (sections[i].isReadOnly) 
		{
			sectionsGrid->SetCellValue(i, 2, "Yes");
		}
		else 
		{
			sectionsGrid->SetCellValue(i, 2, "No");
		}

		char hexNumStr[10];
		sprintf(hexNumStr, "%llX", sections[i].virtualAddress);
		sectionsGrid->SetCellValue(i, 3, wxString(hexNumStr));

		sprintf(hexNumStr, "%llX", sections[i].fileOffset);
		sectionsGrid->SetCellValue(i, 4, wxString(hexNumStr));

		sprintf(hexNumStr, "%X", sections[i].size);
		sectionsGrid->SetCellValue(i, 5, wxString(hexNumStr));
	}
}

void SectionsViewer::GridRightClickOptions(wxGridEvent& e)
{
	wxMenu menu;

	int row = e.GetRow(); // row right-clicked on

	const int ID_COPY_VIRTUAL_ADDRESS = 100;
	const int ID_COPY_FILE_OFFSET = 101;
	const int ID_COPY_SIZE = 102;
	const int ID_COPY_NAME = 103;
	const int ID_FIND = 104;

	menu.Append(ID_COPY_VIRTUAL_ADDRESS, "Copy virtual address");
	menu.Bind(wxEVT_MENU, [&](wxCommandEvent& bs) -> void { CopyToClipboard(sectionsGrid->GetCellValue(row, 3)); }, ID_COPY_VIRTUAL_ADDRESS);

	menu.Append(ID_COPY_FILE_OFFSET, "Copy file offset");
	menu.Bind(wxEVT_MENU, [&](wxCommandEvent& bs) -> void { CopyToClipboard(sectionsGrid->GetCellValue(row, 4)); }, ID_COPY_FILE_OFFSET);

	menu.Append(ID_COPY_SIZE, "Copy size");
	menu.Bind(wxEVT_MENU, [&](wxCommandEvent& bs) -> void { CopyToClipboard(sectionsGrid->GetCellValue(row, 5)); }, ID_COPY_SIZE);

	menu.Append(ID_COPY_NAME, "Copy name");
	menu.Bind(wxEVT_MENU, [&](wxCommandEvent& bs) -> void { CopyToClipboard(sectionsGrid->GetCellValue(row, 0)); }, ID_COPY_NAME);

	menu.Append(ID_FIND, "Find");
	menu.Bind(wxEVT_MENU, [&](wxCommandEvent& bs) -> void {
		wxTextEntryDialog dlg(this, "", "Text");
		if (dlg.ShowModal() == wxID_OK)
		{
			unsigned char found = 0;
			wxString txt = dlg.GetValue();
			if (!txt.IsEmpty())
			{
				int numOfImports = sectionsGrid->GetNumberRows();
				for (int i = 0; i < numOfImports; i++)
				{
					if (sectionsGrid->GetCellValue(i, 0).Contains(txt) || sectionsGrid->GetCellValue(i, 1).Contains(txt))
					{
						sectionsGrid->GoToCell(i, 0);
						sectionsGrid->SelectRow(i);
						found = 1;
						break;
					}
				}
			}

			if (!found)
			{
				wxMessageBox("Text not found", "Failed to find text");
			}
		}
		}, ID_FIND);

	PopupMenu(&menu, ScreenToClient(wxGetMousePosition()));
	e.Skip();
}


void SectionsViewer::CloseMenu(wxCloseEvent& e)
{
	Hide();
}
