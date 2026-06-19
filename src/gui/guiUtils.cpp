#include "guiUtils.h"

void CopyToClipboard(const char* txt)
{
	if (wxTheClipboard->Open())
	{
		wxTheClipboard->SetData(new wxTextDataObject(txt));
		wxTheClipboard->Close();
	}
}