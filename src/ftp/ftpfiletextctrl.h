#ifndef FTPFILETEXTCTRL_H
#define FTPFILETEXTCTRL_H

#include "asyncftp.h"
#include "../filetextctrlbase.h"
#include "../sharedptr.h"
#include <wx/filename.h>



class FtpFileTextCtrl : public FileTextCtrlBase
{
	wxFileName mFileName;
	SharedPtr<AsyncFtp> mFtp;
	void onFtpEvent(FtpEvent& event);
public:
	FtpFileTextCtrl(wxWindow* parent);
	~FtpFileTextCtrl();
	bool open(SharedPtr<AsyncFtp>& ftp, wxString filename);
	wxString getLabel();
	virtual wxString getExtension();
	bool saveFile();
};

#endif // FTPFILETEXTCTRL_H
