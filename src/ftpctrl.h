#ifndef _FTP_H_
#define _FTP_H_

#include <wx/panel.h>
#include <wx/listctrl.h>
#include <wx/listbase.h>
#include <wx/msgdlg.h>
#include <wx/thread.h>
#include "dbconnector.h"
#include "sharedptr.h"
#include "asyncftp.h"
#include <wx/sizer.h>
#include <wx/button.h>
#include "ftpfiletextctrl.h"
#include "editor.h"
#include <wx/imaglist.h>
#include "ftpdialogs.h"

class FloEditor;

class FtpCtrl : public wxPanel {
	enum ViewMode {
		ACCOUNTS = 1,
		FTP,
		FTP_BUSY,
		LAST_VIEWMODE
	};
	FloEditor* mEditor;
	SharedPtr<AsyncFtp> mFtp;
	wxListView* mList;
	wxStatusBar* mStatusBar;
	wxString mPwd;
	ViewMode mViewMode;
	SharedPtr<DbConnector> mDb;
	bool mFtpBusy;
	void onFtpEvent(FtpEvent& event);
	void onDClick(wxListEvent& event);
	void onRightClick(wxListEvent& event);
	void onMenuSelected(wxCommandEvent& event);
	void connect(wxString);
	void initListCtrl(wxSizer* sizer);
	public:
		FtpCtrl(FloEditor* parent, SharedPtr<DbConnector> db);
		virtual ~FtpCtrl();
		void showAccounts();
};

#endif // _FTP_H_
