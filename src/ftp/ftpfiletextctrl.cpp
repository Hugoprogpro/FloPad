#include "ftpfiletextctrl.h"

FtpFileTextCtrl::FtpFileTextCtrl(wxWindow* parent):FileTextCtrlBase(parent)
{
	Connect(wxEVT_FTP, (wxObjectEventFunction)&FtpFileTextCtrl::onFtpEvent);
}

void FtpFileTextCtrl::onFtpEvent(FtpEvent& event) {
	switch(event.type) {
		case FtpEvent::FE_ERROR:
			wxLogError(wxT("FTP error"));
		case FtpEvent::GET:
			SetText(wxString(event.str.c_str(),wxConvUTF8));
			SetSavePoint();
			break;
	}
}

FtpFileTextCtrl::~FtpFileTextCtrl()
{
}

wxString FtpFileTextCtrl::getLabel()
{
	return mFileName.GetName()+wxT(".")+mFileName.GetExt();
}

bool FtpFileTextCtrl::saveFile()
{
	mFtp->createThread(this)->put(mFileName.GetFullPath(), GetText())->Run();
	return true;
}


void FtpFileTextCtrl::open(SharedPtr<AsyncFtp>& ftp, wxString filename)
{
	mFtp = ftp;
	mFileName = filename;
	mFtp->createThread(this)->get(filename)->Run();
}


wxString FtpFileTextCtrl::getExtension() {
	return mFileName.GetExt();
}
