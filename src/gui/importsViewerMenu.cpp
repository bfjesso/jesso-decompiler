#include "importsViewerMenu.h"

wxBEGIN_EVENT_TABLE(ImportsViewer, wxFrame)
EVT_CLOSE(ImportsViewer::CloseMenu)
wxEND_EVENT_TABLE()

ImportsViewer::ImportsViewer() : wxFrame(nullptr, MainWindowID, "Data Viewer", wxPoint(50, 50), wxSize(600, 600))
{
	SetOwnBackgroundColour(backgroundColor);

	importsGrid = new wxGrid(this, wxID_ANY, wxPoint(0, 0), wxSize(500, 500));
	importsGrid->SetLabelBackgroundColour(foregroundColor);
	importsGrid->SetLabelTextColour(textColor);
	importsGrid->SetDefaultCellBackgroundColour(gridColor);
	importsGrid->SetDefaultCellTextColour(textColor);
	importsGrid->CreateGrid(0, 4);
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

	vSizer = new wxBoxSizer(wxVERTICAL);

	vSizer->Add(importsGrid, 1, wxEXPAND);

	SetSizerAndFit(vSizer);
}

void ImportsViewer::OpenMenu(wxPoint position, ImportedFunction* imports, int numOfImports)
{
	position.x += 10;
	position.y += 10;
	SetPosition(position);
	Show();
	Raise();

	int rows = importsGrid->GetNumberRows();
	if (rows > 0)
	{
		importsGrid->DeleteRows(0, importsGrid->GetNumberRows());
	}

	for (int i = 0; i < numOfImports; i++) 
	{
		importsGrid->AppendRows(1);

		char addressStr[10];
		sprintf(addressStr, "%llX", imports[i].address);
		importsGrid->SetCellValue(i, 0, wxString(addressStr));

		importsGrid->SetCellValue(i, 1, imports[i].name.buffer);
	}
}

void ImportsViewer::CloseMenu(wxCloseEvent& e)
{
	Hide();
}
