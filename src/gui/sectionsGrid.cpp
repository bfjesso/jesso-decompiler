#include "sectionsGrid.h"

wxBEGIN_EVENT_TABLE(SectionsGrid, wxGrid)
EVT_GRID_CELL_RIGHT_CLICK(SectionsGrid::RightClickOptions)
wxEND_EVENT_TABLE()

SectionsGrid::SectionsGrid(wxWindow* parent, FileSection* sections, int numOfSections) : wxGrid(parent, wxID_ANY, wxPoint(0, 0), wxSize(600, 300))
{
	SetOwnBackgroundColour(backgroundColor);

	SetLabelBackgroundColour(foregroundColor);
	SetLabelTextColour(textColor);
	SetDefaultCellBackgroundColour(gridColor);
	SetDefaultCellTextColour(textColor);
	CreateGrid(0, 6);
	EnableGridLines(false);
	ShowScrollbars(wxSHOW_SB_NEVER, wxSHOW_SB_ALWAYS);
	SetScrollRate(0, 10);
	SetSelectionMode(wxGrid::wxGridSelectionModes::wxGridSelectRows);
	SetCellHighlightPenWidth(0);
	SetCellHighlightROPenWidth(0);
	DisableDragRowSize();
	EnableEditing(false);
	SetColLabelValue(0, "Name");
	SetColLabelValue(1, "Type");
	SetColLabelValue(2, "Is readonly");
	SetColLabelValue(3, "Virtual address");
	SetColLabelValue(4, "File offset");
	SetColLabelValue(5, "Size");
	HideRowLabels();
	SetColSize(0, 100);
	SetColSize(1, 100);
	SetColSize(2, 100);
	SetColSize(3, 100);
	SetColSize(4, 100);
	SetColSize(5, 9999);
	SetColLabelAlignment(wxALIGN_LEFT, wxALIGN_CENTER);

	if (sections)
	{
		for (int i = 0; i < numOfSections; i++)
		{
			AppendRows(1);

			SetCellValue(i, 0, wxString(sections[i].name.buffer));
			SetCellValue(i, 1, wxString(fileSectionTypeStrs[sections[i].type]));

			if (sections[i].isReadOnly)
			{
				SetCellValue(i, 2, "Yes");
			}
			else
			{
				SetCellValue(i, 2, "No");
			}

			char hexNumStr[10];
			sprintf(hexNumStr, "%llX", sections[i].virtualAddress);
			SetCellValue(i, 3, wxString(hexNumStr));

			sprintf(hexNumStr, "%llX", sections[i].fileOffset);
			SetCellValue(i, 4, wxString(hexNumStr));

			sprintf(hexNumStr, "%X", sections[i].size);
			SetCellValue(i, 5, wxString(hexNumStr));
		}
	}
}

void SectionsGrid::RightClickOptions(wxGridEvent& e)
{
	wxMenu menu;

	int row = e.GetRow(); // row right-clicked on

	const int ID_COPY_VIRTUAL_ADDRESS = 100;
	const int ID_COPY_FILE_OFFSET = 101;
	const int ID_COPY_SIZE = 102;
	const int ID_COPY_NAME = 103;
	const int ID_FIND = 104;

	menu.Append(ID_COPY_VIRTUAL_ADDRESS, "Copy virtual address");
	menu.Bind(wxEVT_MENU, [&](wxCommandEvent& bs) -> void { CopyToClipboard(GetCellValue(row, 3)); }, ID_COPY_VIRTUAL_ADDRESS);

	menu.Append(ID_COPY_FILE_OFFSET, "Copy file offset");
	menu.Bind(wxEVT_MENU, [&](wxCommandEvent& bs) -> void { CopyToClipboard(GetCellValue(row, 4)); }, ID_COPY_FILE_OFFSET);

	menu.Append(ID_COPY_SIZE, "Copy size");
	menu.Bind(wxEVT_MENU, [&](wxCommandEvent& bs) -> void { CopyToClipboard(GetCellValue(row, 5)); }, ID_COPY_SIZE);

	menu.Append(ID_COPY_NAME, "Copy name");
	menu.Bind(wxEVT_MENU, [&](wxCommandEvent& bs) -> void { CopyToClipboard(GetCellValue(row, 0)); }, ID_COPY_NAME);

	menu.Append(ID_FIND, "Find");
	menu.Bind(wxEVT_MENU, [&](wxCommandEvent& bs) -> void {
		wxTextEntryDialog dlg(this, "", "Text");
		if (dlg.ShowModal() == wxID_OK)
		{
			unsigned char found = 0;
			wxString txt = dlg.GetValue();
			if (!txt.IsEmpty())
			{
				int numOfImports = GetNumberRows();
				for (int i = 0; i < numOfImports; i++)
				{
					if (GetCellValue(i, 0).Contains(txt) || GetCellValue(i, 1).Contains(txt))
					{
						GoToCell(i, 0);
						SelectRow(i);
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
