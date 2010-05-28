#include "asyncftp.h"

const wxEventType wxEVT_FTP = wxNewEventType();

AsyncFtp::AsyncFtp():mMutex(new wxMutex),mFtp(new Ftp)
{
}

AsyncFtp::~AsyncFtp()
{
}


FtpThread::FtpThread(SharedPtr<Ftp> ftp, SharedPtr<wxMutex> mutex, wxEvtHandler* evtHandler)
	:wxThread(),mFtp(ftp),mMutex(mutex),mEventHandler(evtHandler)
{
	if(Create() != wxTHREAD_NO_ERROR) {
		wxLogError(wxT("FtpThread::FtpThread"));
	}
	
}


FtpThread::~FtpThread()
{
}

FtpThread* FtpThread::connect(wxString host, wxString username, wxString password)
{
	mAction = CONNECT;
	mHost = host;
	mUsername = username;
	mPassword = password;
	return this;
}

FtpThread* FtpThread::listFiles()
{
	mAction = LIST;
	return this;
}

FtpThread* FtpThread::close() {
	mAction = CLOSE;
	return this;
}

FtpThread* FtpThread::put(wxString filename, wxString data) {
	
	mFileName = filename;
	mAction = PUT;
	mFileContent = data;
	return this;
}

FtpThread* FtpThread::get(wxString filename) {
	mFileName = filename;
	mAction = GET;
	return this;
}

void FtpThread::reconnect() {
	mFtp->reconnect();
}


void FtpThread::sendErrorEvent(int errorCode) {
	FtpEvent event(FtpEvent::ERROR);
	event.errorCode = errorCode;
	mEventHandler->AddPendingEvent(event);
}

wxThread::ExitCode FtpThread::Entry()
{
	wxMutexLocker lock(*mMutex);
	if(!mFtp->isConnected() && mAction != CONNECT) {
		reconnect();
	}
	switch (mAction) {
		case CONNECT: {
			if(!mFtp->connect((const char*)mHost.To8BitData())) {
				sendErrorEvent(CONNECT);
				return 0;
			}
			if(!mFtp->login((const char*)mUsername.To8BitData(), (const char*)mPassword.To8BitData())) {
				sendErrorEvent(CONNECT);
				return 0;
			}
			FtpEvent event(FtpEvent::CONNECT);
			mEventHandler->AddPendingEvent(event);
			}
			break;
		case LIST: {
			FtpEvent event(FtpEvent::LIST);
			if(mFtp->list(event.fileList))
				mEventHandler->AddPendingEvent(event);
			else
				sendErrorEvent(LIST);
			}
			break;
		case PWD: {
			FtpEvent event(FtpEvent::PWD);
			std::string s;
			if(mFtp->pwd(s)) { 
				event.str = wxString(s.c_str(), wxConvUTF8);
				mEventHandler->AddPendingEvent(event);
			}
			else 
				sendErrorEvent(PWD);
			}
			break;
		case CWD: {
			FtpEvent event(FtpEvent::CWD);
			if(mFtp->chdir((const char*)mDirName.To8BitData()))
				mEventHandler->AddPendingEvent(event);
			else
				sendErrorEvent(CWD);
			}
			break;
		case CLOSE: {
			FtpEvent event(FtpEvent::CLOSE);
			if(mFtp->close())
				mEventHandler->AddPendingEvent(event);
			else
				sendErrorEvent(CLOSE);
			}
			break;
		case MKDIR: {
			FtpEvent event(FtpEvent::MKDIR);
			if(mFtp->mkdir((const char*)mDirName.To8BitData()))
				mEventHandler->AddPendingEvent(event);
			else
				sendErrorEvent(MKDIR);
			}
			break;
		case RMDIR: {
			FtpEvent event(FtpEvent::RMDIR);
			if(mFtp->remove((const char*)mDirName.To8BitData(), true))
				mEventHandler->AddPendingEvent(event);
			else
				sendErrorEvent(RMDIR);
			}
			break;
		case RMFILE: {
			FtpEvent event(FtpEvent::RMFILE);
			if(mFtp->remove((const char*)mFileName.To8BitData(), false))
				mEventHandler->AddPendingEvent(event);
			else
				sendErrorEvent(RMFILE);
			}
			break;
		case PUT: {
			FtpEvent event(FtpEvent::PUT);
			if(mFtp->put((const char*)mFileName.To8BitData(), (const char*)mFileContent.To8BitData(),mFileContent.size()))
				mEventHandler->AddPendingEvent(event);
			else
				sendErrorEvent(PUT);
			}
			break;
		case GET: {
			FtpEvent event(FtpEvent::GET);
			std::string s;
			if(mFtp->get((const char*)mFileName.To8BitData(), s)) {
				event.str = wxString(s.c_str(), wxConvUTF8);
				mEventHandler->AddPendingEvent(event);
			}
			else
				sendErrorEvent(GET);
			}
			break;
		default: {
			sendErrorEvent(UNKNOWN);
			}
			break;
	}
	return 0;
}

FtpThread* AsyncFtp::createThread(wxEvtHandler* handler)
{
	return new FtpThread(mFtp, mMutex, handler);
}

FtpThread* FtpThread::cwd(wxString dir)
{
	mAction = CWD;
	mDirName = dir;
	return this;
}

FtpThread* FtpThread::pwd()
{
	mAction = PWD;
	return this;
}

FtpThread* FtpThread::mkdir(wxString dirname)
{
	mAction = MKDIR;
	mDirName = dirname;
	return this;
}

FtpThread* FtpThread::remove(wxString filename, bool isDir)
{
	if(isDir) {
		mAction = RMDIR;
		mDirName = filename;
	}
	else {
		mAction = RMFILE;
		mFileName = filename;
	}
	return this;
}

