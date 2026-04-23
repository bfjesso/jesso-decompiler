#include "importsViewerMenu.h"

wxBEGIN_EVENT_TABLE(ImportsViewer, wxFrame)
EVT_GRID_CELL_RIGHT_CLICK(ImportsViewer::GridRightClickOptions)
wxEND_EVENT_TABLE()

ImportsViewer::ImportsViewer(wxWindow* parent, wxPoint position, ImportedFunction* imports, int numOfImports) : wxFrame(parent, MainWindowID, "Imports", wxPoint(50, 50), wxSize(600, 600))
{
	position.x += 10;
	position.y += 10;
	SetPosition(position);
	Show();
	Raise();
	
	SetOwnBackgroundColour(backgroundColor);

	importsGrid = new wxGrid(this, wxID_ANY, wxPoint(0, 0), wxSize(500, 500));
	importsGrid->SetLabelBackgroundColour(foregroundColor);
	importsGrid->SetLabelTextColour(textColor);
	importsGrid->SetDefaultCellBackgroundColour(gridColor);
	importsGrid->SetDefaultCellTextColour(textColor);
	importsGrid->CreateGrid(0, 2);
	importsGrid->EnableGridLines(false);
	importsGrid->ShowScrollbars(wxSHOW_SB_NEVER, wxSHOW_SB_ALWAYS);
	importsGrid->SetScrollRate(0, 10);
	importsGrid->SetSelectionMode(wxGrid::wxGridSelectionModes::wxGridSelectRows);
	importsGrid->SetCellHighlightPenWidth(0);
	importsGrid->SetCellHighlightROPenWidth(0);
	importsGrid->DisableDragRowSize();
	importsGrid->EnableEditing(false);
	importsGrid->SetColLabelValue(0, "Address");
	importsGrid->SetColLabelValue(1, "Name");
	importsGrid->HideRowLabels();
	importsGrid->SetColSize(0, 200);
	importsGrid->SetColSize(1, 9999);
	importsGrid->SetColLabelAlignment(wxALIGN_LEFT, wxALIGN_CENTER);

	for (int i = 0; i < numOfImports; i++)
	{
		importsGrid->AppendRows(1);

		char addressStr[10];
		sprintf(addressStr, "%llX", imports[i].address);
		importsGrid->SetCellValue(i, 0, wxString(addressStr));

		importsGrid->SetCellValue(i, 1, imports[i].name.buffer);
	}

	vSizer = new wxBoxSizer(wxVERTICAL);

	vSizer->Add(importsGrid, 1, wxEXPAND);

	SetSizerAndFit(vSizer);
}

void ImportsViewer::GridRightClickOptions(wxGridEvent& e)
{
	wxGrid* grid = (wxGrid*)(e.GetEventObject());
	if(grid != importsGrid)
	{
		return;
	}

	wxMenu menu;

	int row = e.GetRow(); // row right-clicked on

	const int ID_COPY_ADDRESS = 100;
	const int ID_COPY_NAME = 101;
	const int ID_FIND = 102;

	menu.Append(ID_COPY_ADDRESS, "Copy address");
	menu.Bind(wxEVT_MENU, [&](wxCommandEvent& bs) -> void { CopyToClipboard(importsGrid->GetCellValue(row, 0)); }, ID_COPY_ADDRESS);

	menu.Append(ID_COPY_NAME, "Copy name");
	menu.Bind(wxEVT_MENU, [&](wxCommandEvent& bs) -> void { CopyToClipboard(importsGrid->GetCellValue(row, 1)); }, ID_COPY_NAME);

	menu.Append(ID_FIND, "Find");
	menu.Bind(wxEVT_MENU, [&](wxCommandEvent& bs) -> void {
		wxTextEntryDialog dlg(this, "", "Text");
		if (dlg.ShowModal() == wxID_OK)
		{
			unsigned char found = 0;
			wxString txt = dlg.GetValue();
			if (!txt.IsEmpty())
			{
				int numOfImports = importsGrid->GetNumberRows();
				for (int i = 0; i < numOfImports; i++)
				{
					if (importsGrid->GetCellValue(i, 0).Contains(txt) || importsGrid->GetCellValue(i, 1).Contains(txt))
					{
						importsGrid->GoToCell(i, 0);
						importsGrid->SelectRow(i);
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
