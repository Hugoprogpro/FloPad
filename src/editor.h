#ifndef EDITOR_H
#define EDITOR_H

#include <wx/frame.h>
#include <wx/menu.h>
#include <wx/sizer.h>
#include <wx/aui/aui.h>
#include <wx/menu.h>
#include "codestyleset.h"
#include "localfiletextctrl.h"
#include "ftpctrl.h"
#include "quickfindpanel.h"
#include "globalsettings.h"

class QuickFindPanel;

class FtpCtrl;

enum {
	COMPLETE_WORD = wxID_HIGHEST,
	VIEW_FTP,
	VIEW_LINENUMBERS,
	VIEW_EOL,
	VIEW_WHITESPACES,
	FTP_NEW_ACCOUNT,
	FTP_REMOVE_ACCOUNT,
	FTP_EDIT_ACCOUNT,
	FTP_REFRESH,
	FTP_CONNECT,
	FTP_CREATE_DIR,
	FTP_REMOVE_DIR,
	FTP_CREATE_FILE,
	FTP_REMOVE_FILE,
	FTP_DISCONNECT,
	VIEWAS_CSS,
	VIEWAS_HTML,
	GLOBAL_SETTINGS,
	LAST_ID
};

class FloEditor : public wxFrame {
	SharedPtr<DbConnector> mDb;
	wxAuiManager* mAuiManager;
	wxAuiNotebook* mNotebook;
	wxMenuBar* mMenuBar;
	FtpCtrl* mFtp;
	QuickFindPanel* mQuickFindPanel;
	void initNotebook();
	void initMenuBar();
	void initFtp();
	void onPageClose(wxAuiNotebookEvent& event);
	void onOpenFile(wxCommandEvent& WXUNUSED(event));
	void onCompleteWord(wxCommandEvent& WXUNUSED(event));
	void onSave(wxCommandEvent& WXUNUSED(event));
	void onSavePointReached(wxStyledTextEvent& event);
	void onSavePointLeft(wxStyledTextEvent& event);
	void onMenuSelected(wxCommandEvent& event);
	void onClose(wxCloseEvent& event);
	void createNewFile();
	void viewAs(wxString lang);
	void toggleLineNumbers();
	void toggleEol();
	void toggleWhiteSpaces();
	public:
		FloEditor();
		~FloEditor();
		SharedPtr<DbConnector> getDb();
		wxAuiNotebook* getNotebook() { return mNotebook; }
		void addFileTextCtrl(FileTextCtrlBase* ctrl);
		wxStyledTextCtrl* getSelectedFileTextCtrl();
		void setTabWidth(int w);
};

#endif // EDITOR_H
