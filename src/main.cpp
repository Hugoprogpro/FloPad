#include <iostream>
#include <wx/wx.h>
#include <wx/stc/stc.h>
#include "editor.h"
#include "ftp/ftp.h"



class wxMiniApp : public wxApp
{
public:
	virtual bool OnInit();
	virtual int OnExit();
};

IMPLEMENT_APP(wxMiniApp);

int wxMiniApp::OnExit() {
	wxSocketBase::Shutdown();
	return wxApp::OnExit();
}

bool wxMiniApp::OnInit()
{
	SetTopWindow((wxWindow*) new FloEditor() );
    GetTopWindow()->Show();
	wxSocketBase::Initialize();
    return true;
}
