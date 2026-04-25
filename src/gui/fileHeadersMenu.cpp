#include "fileHeadersMenu.h"
#include <wx/grid.h>
#include "../file-handler/fileHandler.h"

FileHeadersMenu::FileHeadersMenu(wxWindow* parent, wxPoint position, wxString filePath) : wxFrame(parent, MainWindowID, "File Headers", wxPoint(50, 50), wxSize(800, 500))
{
	position.x += 10;
	position.y += 10;
	SetPosition(position);
	Show();
	Raise();

	SetOwnBackgroundColour(backgroundColor);

	if (filePath == "")
	{
		wxMessageBox("No file opened", "Can't get file headers");
		return;
	}

	struct JdcStr fileHeadersInfoJdcStr = initializeJdcStrWithSize(10000);
	if (!generateFileHeadersInfoStr(filePath.c_str().AsWChar(), &fileHeadersInfoJdcStr))
	{
		freeJdcStr(&fileHeadersInfoJdcStr);
		wxMessageBox("Error getting file header info", "Failed to get file header info");
		return;
	}

	wxString fileHeadersInfoStr = wxString(fileHeadersInfoJdcStr.buffer);
	freeJdcStr(&fileHeadersInfoJdcStr);

	vSizer = new wxBoxSizer(wxVERTICAL);

	int headerStartPos = 0;
	int headerEndPos = fileHeadersInfoStr.find("\n\n", 0);
	while (headerEndPos != -1)
	{
		wxStaticText* headerNameLabel = new wxStaticText(this, wxID_ANY, fileHeadersInfoStr.SubString(headerStartPos, headerEndPos));
		headerNameLabel->SetOwnForegroundColour(textColor);
		vSizer->Add(headerNameLabel, 0, wxLEFT | wxTOP, 10);
		
		wxGrid* grid = new wxGrid(this, wxID_ANY, wxPoint(0, 0), wxSize(800, 150));
		grid->SetLabelBackgroundColour(foregroundColor);
		grid->SetLabelTextColour(textColor);
		grid->SetDefaultCellBackgroundColour(gridColor);
		grid->SetDefaultCellTextColour(textColor);
		grid->CreateGrid(0, 4);
		grid->EnableGridLines(false);
		grid->ShowScrollbars(wxSHOW_SB_NEVER, wxSHOW_SB_ALWAYS);
		grid->SetScrollRate(0, 10);
		grid->SetSelectionMode(wxGrid::wxGridSelectionModes::wxGridSelectRows);
		grid->SetCellHighlightPenWidth(0);
		grid->SetCellHighlightROPenWidth(0);
		grid->DisableDragRowSize();
		grid->EnableEditing(false);
		grid->SetColLabelValue(0, "Offset");
		grid->SetColLabelValue(1, "Field");
		grid->SetColLabelValue(2, "Value");
		grid->SetColLabelValue(3, "Meaning");
		grid->HideRowLabels();
		grid->SetColSize(0, 50);
		grid->SetColSize(1, 200);
		grid->SetColSize(2, 200);
		grid->SetColSize(3, 9999);
		grid->SetColLabelAlignment(wxALIGN_LEFT, wxALIGN_CENTER);
		vSizer->Add(grid, 1, wxEXPAND);

		int rowIndex = 0;

		int rowStartPos = headerEndPos + 2;
		int rowEndPos = fileHeadersInfoStr.find("\n", rowStartPos);

		headerEndPos = fileHeadersInfoStr.find("\n\n", rowStartPos);

		while (rowEndPos != -1 && rowEndPos != headerEndPos)
		{
			grid->AppendRows(1);

			int offsetEndPos = fileHeadersInfoStr.find("\t", rowStartPos);
			grid->SetCellValue(rowIndex, 0, fileHeadersInfoStr.SubString(rowStartPos, offsetEndPos));

			int fieldEndPos = fileHeadersInfoStr.find("\t", offsetEndPos + 1);
			grid->SetCellValue(rowIndex, 1, fileHeadersInfoStr.SubString(offsetEndPos, fieldEndPos));

			int valueEndPos = fileHeadersInfoStr.find("\t", fieldEndPos + 1);
			grid->SetCellValue(rowIndex, 2, fileHeadersInfoStr.SubString(fieldEndPos, valueEndPos));

			grid->SetCellValue(rowIndex, 3, fileHeadersInfoStr.SubString(valueEndPos, rowEndPos));

			rowIndex++;

			headerStartPos = rowEndPos + 1;

			rowStartPos = rowEndPos + 1;
			rowEndPos = fileHeadersInfoStr.find("\n", rowStartPos);
		}
	}

	SetSizerAndFit(vSizer);
}