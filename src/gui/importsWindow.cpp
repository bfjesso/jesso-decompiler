#include "importsWindow.h"

ImportsWindow::ImportsWindow(wxWindow* parent, ImportedFunction* imports, int numOfImports) : wxGrid(parent, wxID_ANY, wxPoint(0, 0), wxSize(600, 300))
{
	SetOwnBackgroundColour(backgroundColor);

	SetLabelBackgroundColour(foregroundColor);
	SetLabelTextColour(textColor);
	SetDefaultCellBackgroundColour(gridColor);
	SetDefaultCellTextColour(textColor);
	CreateGrid(0, 2);
	EnableGridLines(false);
	ShowScrollbars(wxSHOW_SB_NEVER, wxSHOW_SB_ALWAYS);
	SetScrollRate(0, 10);
	SetSelectionMode(wxGrid::wxGridSelectionModes::wxGridSelectRows);
	SetCellHighlightPenWidth(0);
	SetCellHighlightROPenWidth(0);
	DisableDragRowSize();
	EnableEditing(false);
	SetColLabelValue(0, "Address");
	SetColLabelValue(1, "Name");
	HideRowLabels();
	SetColSize(0, 100);
	SetColSize(1, 9999);
	SetColLabelAlignment(wxALIGN_LEFT, wxALIGN_CENTER);

	if (imports ) 
	{
		for (int i = 0; i < numOfImports; i++)
		{
			AppendRows(1);

			char addressStr[10];
			sprintf(addressStr, "%llX", imports[i].address);
			SetCellValue(i, 0, wxString(addressStr));

			SetCellValue(i, 1, imports[i].name.buffer);
		}
	}
}

void ImportsWindow::RightClickOptions(wxGridEvent& e)
{
	wxMenu menu;

	int row = e.GetRow(); // row right-clicked on

	const int ID_COPY_ADDRESS = 100;
	const int ID_COPY_NAME = 101;
	const int ID_FIND = 102;

	menu.Append(ID_COPY_ADDRESS, "Copy address");
	menu.Bind(wxEVT_MENU, [&](wxCommandEvent& bs) -> void { CopyToClipboard(GetCellValue(row, 0)); }, ID_COPY_ADDRESS);

	menu.Append(ID_COPY_NAME, "Copy name");
	menu.Bind(wxEVT_MENU, [&](wxCommandEvent& bs) -> void { CopyToClipboard(GetCellValue(row, 1)); }, ID_COPY_NAME);

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