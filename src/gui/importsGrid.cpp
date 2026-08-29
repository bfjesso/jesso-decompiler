#include "importsGrid.h"

wxBEGIN_EVENT_TABLE(ImportsGrid, wxGrid)
EVT_GRID_CELL_RIGHT_CLICK(ImportsGrid::RightClickOptions)
wxEND_EVENT_TABLE()

ImportsGrid::ImportsGrid(wxWindow* parent, ImportedFunction* imports, int numOfImports, JdcStr* libraryNames, int numOfLibraries) : wxGrid(parent, wxID_ANY)
{
	SetMinSize(wxSize(100, 100));
	SetOwnBackgroundColour(backgroundColor);

	SetLabelBackgroundColour(foregroundColor);
	SetLabelTextColour(textColor);
	SetDefaultCellBackgroundColour(gridColor);
	SetDefaultCellTextColour(textColor);
	CreateGrid(0, 3);
	EnableGridLines(false);
	SetScrollRate(10, 10);
	SetSelectionMode(wxGrid::wxGridSelectionModes::wxGridSelectRows);
	SetCellHighlightPenWidth(0);
	SetCellHighlightROPenWidth(0);
	DisableDragRowSize();
	EnableEditing(false);
	SetColLabelValue(0, "Address");
	SetColLabelValue(1, "Library");
	SetColLabelValue(2, "Name");
	HideRowLabels();
	SetColSize(0, 100);
	SetColSize(1, 300);
	SetColSize(2, 300);
	SetColLabelAlignment(wxALIGN_LEFT, wxALIGN_CENTER);

	if (imports ) 
	{
		for (int i = 0; i < numOfImports; i++)
		{
			AppendRows(1);

			char addressStr[10];
			sprintf(addressStr, "%llX", imports[i].address);
			SetCellValue(i, 0, wxString(addressStr));
			SetCellValue(i, 1, libraryNames[imports[i].libraryNameIndex].buffer);
			SetCellValue(i, 2, imports[i].name.buffer);
		}
	}
}

void ImportsGrid::RightClickOptions(wxGridEvent& e)
{
	wxMenu menu;

	int row = e.GetRow(); // row right-clicked on

	const int ID_COPY_ADDRESS = 100;
	const int ID_COPY_LIBRARY = 101;
	const int ID_COPY_NAME = 102;
	const int ID_FIND = 103;

	menu.Append(ID_COPY_ADDRESS, "Copy address");
	menu.Bind(wxEVT_MENU, [&](wxCommandEvent& bs) -> void { CopyToClipboard(GetCellValue(row, 0)); }, ID_COPY_ADDRESS);

	menu.Append(ID_COPY_LIBRARY, "Copy library");
	menu.Bind(wxEVT_MENU, [&](wxCommandEvent& bs) -> void { CopyToClipboard(GetCellValue(row, 1)); }, ID_COPY_LIBRARY);

	menu.Append(ID_COPY_NAME, "Copy name");
	menu.Bind(wxEVT_MENU, [&](wxCommandEvent& bs) -> void { CopyToClipboard(GetCellValue(row, 2)); }, ID_COPY_NAME);

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
					if (GetCellValue(i, 0).Contains(txt) || GetCellValue(i, 1).Contains(txt) || GetCellValue(i, 2).Contains(txt))
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