#ifndef ASYNCFTP_H
#define ASYNCFTP_H

#include "ftp/ftp.h"
#include "sharedptr.h"
#include <wx/wx.h>

class FtpThread;

class AsyncFtp {
	SharedPtr<Ftp> mFtp;
	SharedPtr<wxMutex> mMutex;
	public:
		AsyncFtp();
		~AsyncFtp();
		bool isConnected() { return mFtp->isConnected(); }
		FtpThread* createThread(wxEvtHandler* handler);
};

extern const wxEventType wxEVT_FTP;

struct FtpEvent : public wxEvent {
	enum Type {
		CONNECT,
		LIST,
		GET,
		PUT,
		CWD,
		PWD,
		MKDIR,
		RMDIR,
		RMFILE,
		CLOSE,
		ERROR,
		LAST_EVENT
	};
	Ftp::FileList fileList;
	int errorCode;
	wxString str;
	Type type;
	FtpEvent():wxEvent(wxID_ANY, wxEVT_FTP) {};
	FtpEvent(Type type):wxEvent(wxID_ANY, wxEVT_FTP) {
		this->type = type;
	}
	virtual wxEvent* Clone() const {
		return new FtpEvent(*this);
	}
};

class FtpThread : public wxThread {
	wxEvtHandler* mEventHandler;
	SharedPtr<Ftp> mFtp;
	enum Errors {
		LOGIN_FAILED = 1,
		CONNECT_FAILED
	};
	enum Action {
		CONNECT = 1,
		LIST,
		GET,
		PUT,
		CWD,
		PWD,
		MKDIR,
		RMDIR,
		RMFILE,
		CLOSE,
		UNKNOWN,
		LAST_ACTION
	};
	Action mAction;
	wxString mHost, mUsername, mPassword;
	wxString mFileName;
	wxString mFileContent;
	wxString mDirName;
	SharedPtr<wxMutex> mMutex;
	void reconnect();
	void sendErrorEvent(int errorCode);
	public:
		FtpThread(SharedPtr<Ftp> ftp, SharedPtr<wxMutex> mutex, wxEvtHandler* evtHandler);
		~FtpThread();
		FtpThread* close();
		FtpThread* listFiles();
		FtpThread* pwd();
		FtpThread* mkdir(wxString dirname);
		FtpThread* remove(wxString filename, bool isDir = false);
		FtpThread* cwd(wxString dir);
		FtpThread* put(wxString filename, wxString data);
		FtpThread* get(wxString filename);
		FtpThread* connect(wxString host, wxString username, wxString password);
		virtual ExitCode Entry();
};

#endif // ASYNCFTP_H
