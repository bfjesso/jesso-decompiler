#include "colorsMenu.h"

wxBEGIN_EVENT_TABLE(ColorsMenu, wxFrame)
EVT_CLOSE(ColorsMenu::CloseMenu)
wxEND_EVENT_TABLE()

ColorsMenu::ColorsMenu() : wxFrame(nullptr, MainWindowID, "Colors Menu", wxPoint(50, 50), wxSize(400, 600))
{
	SetOwnBackgroundColour(backgroundColor);

	disassemblySizer = new wxBoxSizer(wxVERTICAL);
	decompilationSizer = new wxBoxSizer(wxVERTICAL);
	dataSizer = new wxBoxSizer(wxVERTICAL);
	hSizer = new wxBoxSizer(wxHORIZONTAL);
	vSizer = new wxBoxSizer(wxVERTICAL);

	disassemblyScrollWindow = new wxScrolledWindow(this, wxID_ANY, wxPoint(0, 0), wxSize(350, 500), wxVSCROLL | wxFULL_REPAINT_ON_RESIZE);
	disassemblyScrollWindow->SetOwnBackgroundColour(gridColor);
	disassemblyScrollWindow->SetScrollRate(0, 10);

	decompilationScrollWindow = new wxScrolledWindow(this, wxID_ANY, wxPoint(0, 0), wxSize(350, 500), wxVSCROLL | wxFULL_REPAINT_ON_RESIZE);
	decompilationScrollWindow->SetOwnBackgroundColour(gridColor);
	decompilationScrollWindow->SetScrollRate(0, 10);

	dataScrollWindow = new wxScrolledWindow(this, wxID_ANY, wxPoint(0, 0), wxSize(350, 500), wxVSCROLL | wxFULL_REPAINT_ON_RESIZE);
	dataScrollWindow->SetOwnBackgroundColour(gridColor);
	dataScrollWindow->SetScrollRate(0, 10);

	disassemblyLabel = new wxStaticText(disassemblyScrollWindow, wxID_ANY, "Disassembly colors");
	disassemblyLabel->SetOwnForegroundColour(textColor);
	disassemblySizer->Add(disassemblyLabel, 0, wxCENTER | wxLEFT | wxRIGHT | wxUP, 10);

	decompilationLabel = new wxStaticText(decompilationScrollWindow, wxID_ANY, "Decompilation colors");
	decompilationLabel->SetOwnForegroundColour(textColor);
	decompilationSizer->Add(decompilationLabel, 0, wxCENTER | wxLEFT | wxRIGHT | wxUP, 10);

	dataLabel = new wxStaticText(dataScrollWindow, wxID_ANY, "Data colors");
	dataLabel->SetOwnForegroundColour(textColor);
	dataSizer->Add(dataLabel, 0, wxCENTER | wxLEFT | wxRIGHT | wxUP, 10);

	for (int i = 0; i < NUM_OF_DISASSEMBLY_COLORS; i++)
	{
		wxStaticText* label = new wxStaticText(disassemblyScrollWindow, wxID_ANY, disassemblyColorNames[i]);
		label->SetOwnForegroundColour(textColor);
		wxColourPickerCtrl* ctrl = new wxColourPickerCtrl(disassemblyScrollWindow, wxID_ANY, disassemblyColors[i], wxPoint(0, 0), wxSize(250, 25));

		disassemblySizer->Add(label, 0, wxCENTER | wxLEFT | wxRIGHT | wxUP, 10);
		disassemblySizer->Add(ctrl, 0, wxCENTER | wxLEFT | wxRIGHT | wxDOWN, 10);

		disassemblyColorPickerCtrls.push_back(ctrl);
	}

	for (int i = 0; i < NUM_OF_DECOMP_COLORS; i++) 
	{
		wxStaticText* label = new wxStaticText(decompilationScrollWindow, wxID_ANY, decompColorNames[i]);
		label->SetOwnForegroundColour(textColor);
		wxColourPickerCtrl* ctrl = new wxColourPickerCtrl(decompilationScrollWindow, wxID_ANY, decompColors[i], wxPoint(0, 0), wxSize(250, 25));

		decompilationSizer->Add(label, 0, wxCENTER | wxLEFT | wxRIGHT | wxUP, 10);
		decompilationSizer->Add(ctrl, 0, wxCENTER | wxLEFT | wxRIGHT | wxDOWN, 10);

		decompilationColorPickerCtrls.push_back(ctrl);
	}

	for (int i = 0; i < NUM_OF_DATA_COLORS; i++)
	{
		wxStaticText* label = new wxStaticText(dataScrollWindow, wxID_ANY, dataColorNames[i]);
		label->SetOwnForegroundColour(textColor);
		wxColourPickerCtrl* ctrl = new wxColourPickerCtrl(dataScrollWindow, wxID_ANY, dataColors[i], wxPoint(0, 0), wxSize(250, 25));

		dataSizer->Add(label, 0, wxCENTER | wxLEFT | wxRIGHT | wxUP, 10);
		dataSizer->Add(ctrl, 0, wxCENTER | wxLEFT | wxRIGHT | wxDOWN, 10);

		dataColorPickerCtrls.push_back(ctrl);
	}

	disassemblyScrollWindow->SetSizer(disassemblySizer);
	decompilationScrollWindow->SetSizer(decompilationSizer);
	dataScrollWindow->SetSizer(dataSizer);
	disassemblyScrollWindow->FitInside();
	decompilationScrollWindow->FitInside();
	dataScrollWindow->FitInside();

	hSizer->Add(disassemblyScrollWindow, 1, wxEXPAND | wxALL, 5);
	hSizer->Add(decompilationScrollWindow, 1, wxEXPAND | wxALL, 5);
	hSizer->Add(dataScrollWindow, 1, wxEXPAND | wxALL, 5);

	vSizer->Add(hSizer, 1, wxEXPAND);

	applyButton = new wxButton(this, ApplyButtonID, "Apply", wxPoint(0, 0), wxSize(250, 25));
	applyButton->SetOwnBackgroundColour(foregroundColor);
	applyButton->SetOwnForegroundColour(textColor);
	applyButton->Bind(wxEVT_BUTTON, [&](wxCommandEvent& e) -> void { ApplyColors(); });

	vSizer->Add(applyButton, 0, wxCENTER | wxALL, 10);

	SetSizerAndFit(vSizer);
}

void ColorsMenu::AddTextCtrl(JdcTextCtrl* ctrl) 
{
	textCtrls.push_back(ctrl);
}

void ColorsMenu::ApplyColors()
{
	for (int i = 0; i < textCtrls.size(); i++)
	{
		if (textCtrls[i]->highlightingType == DISASSEMBLY_HIGHLIGHTING)
		{
			for (int j = 0; j < NUM_OF_DISASSEMBLY_COLORS; j++) 
			{
				disassemblyColors[j] = disassemblyColorPickerCtrls[j]->GetColour();
				textCtrls[i]->StyleSetForeground(j, disassemblyColors[j]);
			}
		}
		else if (textCtrls[i]->highlightingType == DECOMPILATION_HIGHLIGHTING)
		{
			for (int j = 0; j < NUM_OF_DECOMP_COLORS; j++)
			{
				decompColors[j] = decompilationColorPickerCtrls[j]->GetColour();
				textCtrls[i]->StyleSetForeground(j, decompColors[j]);
			}
		}
		else if (textCtrls[i]->highlightingType == DATA_HIGHLIGHTING)
		{
			for (int j = 0; j < NUM_OF_DATA_COLORS; j++)
			{
				dataColors[j] = dataColorPickerCtrls[j]->GetColour();
				textCtrls[i]->StyleSetForeground(j, dataColors[j]);
			}
		}
	}
}

void ColorsMenu::OpenMenu(wxPoint position)
{
	position.x += 10;
	position.y += 10;
	SetPosition(position);
	Show();
	Raise();
}

void ColorsMenu::CloseMenu(wxCloseEvent& e) // stops this frame from being destroyed and the data being lost
{
	Hide();
}
