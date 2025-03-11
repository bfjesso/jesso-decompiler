#include "../Headers/initApp.h"

wxIMPLEMENT_APP(App);

bool App::OnInit()
{
	mainGui = new MainGui();
	mainGui->Show();

	return true;
}