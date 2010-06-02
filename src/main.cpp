#include <iostream>
#include <wx/wx.h>
#include <wx/stc/stc.h>
#include "src/editor.h"
#include "src/ftp/ftp.h"



class wxMiniApp : public wxApp
{
public:
	virtual bool OnInit();
};

IMPLEMENT_APP(wxMiniApp);

bool wxMiniApp::OnInit()
{
	SetTopWindow( new FloEditor() );
    GetTopWindow()->Show();

    return true;
}
