#include <iostream>
#include <wx/wx.h>
#include <wx/stc/stc.h>
#include "src/editor.h"
#include "src/ftp/ftp.h"



class wxMiniApp : public wxApp
{
public:
	virtual bool OnInit();
	virtual int OnExit();
};

IMPLEMENT_APP(wxMiniApp);

int wxMiniApp::OnExit() {
	wxSocketBase::Shutdown();
}

bool wxMiniApp::OnInit()
{
	SetTopWindow( new FloEditor() );
    GetTopWindow()->Show();
	wxSocketBase::Initialize();
    return true;
}
