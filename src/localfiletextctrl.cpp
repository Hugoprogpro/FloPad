#include "localfiletextctrl.h"

LocalFileTextCtrl::LocalFileTextCtrl(wxWindow* parent):FileTextCtrlBase(parent),mFilename(wxT(""))
{	
}

LocalFileTextCtrl::~LocalFileTextCtrl()
{
}

bool LocalFileTextCtrl::saveFileAs() {
	wxFileDialog fd( this, _("Save file"), wxT(""), wxT(""), wxT("*.*"),  wxSAVE);
	if ( fd.ShowModal() == wxID_OK ){
		mFilename = fd.GetPath();
		return saveFile();
	}
	return false;
}

bool LocalFileTextCtrl::openFile() {
	wxFileDialog fd( this, _("Open file"), wxT(""), wxT(""), wxT("*.*"),  wxOPEN);
	if ( fd.ShowModal() == wxID_OK ){
		mFilename = fd.GetPath();
		return LoadFile(mFilename.GetFullPath());
	}
	return false;
}


bool LocalFileTextCtrl::saveFile()
{
	if(mFilename.GetFullName() == wxT("")) 
		return saveFileAs();
	else {
		FILE* fp = fopen(mFilename.GetFullPath().ToAscii(), "w");
		if(!fp)
			return false;
		fputs(GetText().ToAscii(), fp);
		fclose(fp);
		return true;
	}
}

wxString LocalFileTextCtrl::getLabel()
{
	if(mFilename.GetFullName() == wxT(""))
		return wxT("New file");
	else
		return mFilename.GetName()+wxT(".")+mFilename.GetExt();
}

wxString LocalFileTextCtrl::getExtension()
{
	if(mFilename.GetExt() != wxT(""))
		return mFilename.GetExt();
	return wxT("html");
}
