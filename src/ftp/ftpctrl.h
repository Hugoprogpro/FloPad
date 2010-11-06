#ifndef _FTP_H_
#define _FTP_H_

#include <wx/panel.h>
#include <wx/listctrl.h>
#include <wx/listbase.h>
#include <wx/msgdlg.h>
#include <wx/thread.h>
#include <wx/sizer.h>
#include <wx/button.h>
#include <wx/imaglist.h>
#include "../dbconnector.h"
#include "../sharedptr.h"
#include "../editor.h"
#include "asyncftp.h"
#include "ftpfiletextctrl.h"
#include "ftpdialogs.h"
#include "ftptoolbar.h"


class FtpToolbar;
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
	FtpToolbar* mToolbar;
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
	void onItemSelect(wxListEvent& event);
	void onItemDeselect(wxListEvent& event);
	void connect(wxString);
	void initToolbar(wxSizer* sizer);
	void initListCtrl(wxSizer* sizer);
	public:
		FtpCtrl(FloEditor* parent, SharedPtr<DbConnector> db);
		virtual ~FtpCtrl();
		void showAccounts();
};

#endif // _FTP_H_
