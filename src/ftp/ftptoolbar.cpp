#include "ftptoolbar.h"

FtpToolbar::FtpToolbar(wxWindow* parent):	wxToolBar(parent, wxID_ANY) {
	AddTool(FTP_NEW_ACCOUNT, wxT("Add server"), wxBitmap(wxT("data/server_add.png")));
	AddTool(FTP_REMOVE_ACCOUNT, wxT("Remove server"), wxBitmap(wxT("data/server_delete.png")));
	AddTool(FTP_EDIT_ACCOUNT, wxT("Edit server"), wxBitmap(wxT("data/server_edit.png")));
	AddTool(FTP_CONNECT, wxT("Connect"), wxBitmap(wxT("data/server_connect.png")));
	Realize();
}

void FtpToolbar::enableServerTools(bool enabled) {
	EnableTool(FTP_REMOVE_ACCOUNT, enabled);	
	EnableTool(FTP_EDIT_ACCOUNT, enabled);	
	EnableTool(FTP_CONNECT, enabled);	
}

FtpToolbar::~FtpToolbar() {
}
