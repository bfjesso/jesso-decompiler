#pragma once
#include <wx/wx.h>
#include "mainGui.h"

class App : public wxApp
{
public:
	virtual bool OnInit();

private:
	MainGui* mainGui = nullptr;
};