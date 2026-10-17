#include "main.h"
#include <wx/cmdline.h>

wxIMPLEMENT_APP(App);

bool App::OnInit()
{
    mainGui = new MainGui();
    mainGui->Show();

    if (wxApp::argc > 1) 
    {
        wxCmdLineParser parser(wxApp::argc, wxApp::argv);
        parser.AddParam("file", wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL);
        OnInitCmdLine(parser);

        if (parser.Parse() == 0 && parser.GetParamCount() > 0)
        {
            mainGui->OpenFileFromPath(parser.GetParam(0));
        }
    }

	return true;
}
