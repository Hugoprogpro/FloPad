#include "ftpctrl.h"




FtpCtrl::FtpCtrl(FloEditor* parent, SharedPtr<DbConnector> db):wxPanel((wxWindow*)parent),mFtp(new AsyncFtp),mViewMode(ACCOUNTS),mDb(db) {
	this->mEditor = parent;
	Connect(wxEVT_FTP, (wxObjectEventFunction)&FtpCtrl::onFtpEvent);
	Connect(wxEVT_COMMAND_LIST_ITEM_ACTIVATED, (wxObjectEventFunction)&FtpCtrl::onDClick);
	Connect(wxEVT_COMMAND_LIST_ITEM_RIGHT_CLICK, (wxObjectEventFunction)&FtpCtrl::onRightClick);
	Connect(wxEVT_COMMAND_MENU_SELECTED, (wxObjectEventFunction)&FtpCtrl::onMenuSelected);
	wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);
	SetSizer(sizer);
	initListCtrl(sizer);
	mStatusBar = new wxStatusBar(this);
	sizer->Add(mStatusBar, 0, wxGROW);
	showAccounts();
}

void FtpCtrl::initListCtrl(wxSizer* sizer) {
	mList = new wxListView(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLC_LIST);
	sizer->Add(mList, 1, wxGROW | wxEXPAND);
	//mList->InsertColumn(0, wxT("Title"));
	wxImageList* imagelist = new wxImageList(16, 16, true);
	imagelist->Add(wxBitmap(wxT("data/folder.gif")));
	mList->SetImageList(imagelist, wxIMAGE_LIST_SMALL);
}

void FtpCtrl::onRightClick(wxListEvent& event) {
	switch(mViewMode) {
	case ACCOUNTS: {
		wxMenu* menu = new wxMenu();
		if(event.GetIndex() != -1) {
			menu->Append(FTP_CONNECT, wxT("connect"));
			menu->Append(FTP_REMOVE_ACCOUNT, wxT("remove server"));
			menu->Append(FTP_EDIT_ACCOUNT, wxT("edit server"));
		}
		menu->Append(FTP_NEW_ACCOUNT, wxT("add server"));
		mList->PopupMenu(menu, event.GetPoint());
	}
	break;
	case FTP: {
		wxMenu* menu = new wxMenu();
		menu->Append(FTP_REFRESH, wxT("refresh"));
		menu->Append(FTP_CREATE_FILE, wxT("new file"));
		menu->Append(FTP_CREATE_DIR, wxT("new directory"));
		if(event.GetIndex() != -1) {
			if((bool)event.GetData())
				menu->Append(FTP_REMOVE_DIR, wxT("remove"));
			else
				menu->Append(FTP_REMOVE_FILE, wxT("delete"));
		}
		menu->Append(FTP_DISCONNECT, wxT("disconnect"));
		PopupMenu(menu, event.GetPoint());
	}
	break;
	}
}

void FtpCtrl::onMenuSelected(wxCommandEvent& event) {
	switch(event.GetId()) {
	case FTP_REFRESH: {
		mStatusBar->SetStatusText(wxT("getting file list..."));
		mFtp->createThread(this)->listFiles()->Run();
	}
	break;
	case FTP_EDIT_ACCOUNT: {
		wxListItem item;
		item.SetId(mList->GetFirstSelected());
		mList->GetItem(item);
		editFtpAccount(mDb, item.GetText());
		showAccounts();
	}
	break;
	case FTP_NEW_ACCOUNT: {
		addNewFtpAccount(mDb);
		showAccounts();
	}
	break;
	case FTP_REMOVE_ACCOUNT: {
		wxListItem item;
		item.SetId(mList->GetFirstSelected());
		mList->GetItem(item);
		wxMessageDialog d(NULL, wxT("Are you sure?"), wxT("Confirm"), wxYES | wxNO);
		if(d.ShowModal() == wxID_YES)
			mDb->query(wxT("delete from ftpaccounts where title = \"")+item.GetText()+wxT("\""));
		showAccounts();
	}
	break;
	case FTP_CREATE_FILE: {
		wxString name = wxGetTextFromUser(wxT("File name:"));
		if(name != wxT("")) {
			mViewMode = FTP_BUSY;
			mStatusBar->SetStatusText(wxT("creating file..."));
			mFtp->createThread(this)->put(name, wxT(""))->Run();
		}
	}
	break;
	case FTP_REMOVE_DIR: {
		int n = mList->GetFirstSelected();
		if(n != -1) {
			wxMessageDialog d(NULL, wxT("Are you sure?"), wxT("Confirm"), wxYES | wxNO);
			if(d.ShowModal() == wxID_YES) {
				wxListItem item;
				item.SetId(n);
				if(mList->GetItem(item)) {
					mViewMode = FTP_BUSY;
					mStatusBar->SetStatusText(wxT("removing directory..."));
					mFtp->createThread(this)->remove(item.GetText(), true)->Run();
				}
			}
		}
	}
	break;
	case FTP_REMOVE_FILE: {
		int n = mList->GetFirstSelected();
		if(n != -1) {
			wxMessageDialog d(NULL, wxT("Are you sure?"), wxT("Confirm"), wxYES | wxNO);
			if(d.ShowModal() == wxID_YES) {
				wxListItem item;
				item.SetId(n);
				if(mList->GetItem(item)) {
					mViewMode = FTP_BUSY;
					mStatusBar->SetStatusText(wxT("removing file..."));
					mFtp->createThread(this)->remove(item.GetText(), false)->Run();
				}
			}
		}
	}
	break;
	case FTP_CREATE_DIR: {
		wxString name = wxGetTextFromUser(wxT("Directory name:"));
		if(name != wxT("")) {
			mViewMode = FTP_BUSY;
			mStatusBar->SetStatusText(wxT("creating directory..."));
			mFtp->createThread(this)->mkdir(name)->Run();
		}
	}
	break;
	case FTP_CONNECT: {
		wxListItem item;
		item.SetId(mList->GetFirstSelected());
		mList->GetItem(item);
		connect(item.GetText());
	}
	break;
	case FTP_DISCONNECT: {
		mViewMode = FTP_BUSY;
		mStatusBar->SetStatusText(wxT("disconnecting..."));
		mFtp->createThread(this)->close()->Run();
	}
	break;
	}
}

void FtpCtrl::onDClick(wxListEvent& event) {
	if(mViewMode != FTP_BUSY) {
		if(mViewMode == FTP) {
			if((bool)event.GetData()) {
				mViewMode = FTP_BUSY;
				mFtp->createThread(this)->cwd(event.GetText())->Run();
				mStatusBar->SetStatusText(wxT("Changing directory"));
			} else {
				FtpFileTextCtrl* ctrl = new FtpFileTextCtrl(mEditor->getNotebook());
				ctrl->open(mFtp, mPwd + wxT("/")+ event.GetText());
				mEditor->addFileTextCtrl(ctrl);
			}
		} else {
			if(mList->GetFirstSelected() != -1)
				connect(event.GetText());
		}
	}
}

void FtpCtrl::connect(wxString host) {
	mDb->query(stc2wx("select * from ftpaccounts where title = \"") + host + wxT("\""));
	if(mDb->data.size() > 0) {
		mViewMode = FTP_BUSY;
		mFtp->createThread(this)->connect(mDb->getString(0, "host"), mDb->getString(0, "username"), mDb->getString(0, "password"))->Run();
		mStatusBar->SetStatusText(wxT("Connecting"));
	}
}

void FtpCtrl::showAccounts() {
	mList->DeleteAllItems();
	mDb->query(wxT("select * from ftpaccounts order by title"));
	for(int i = 0; i < mDb->getRowCount(); ++i) {
		mList->InsertItem(i, mDb->getString(i, "title"));
	}
	mViewMode = ACCOUNTS;
	mStatusBar->SetStatusText(wxT("Account list"));
}


void FtpCtrl::onFtpEvent(FtpEvent& event) {
	switch(event.type) {
	case FtpEvent::ERROR: {
		wxLogError(wxT("FTP error"));
		showAccounts();
	}
	break;
	case FtpEvent::CWD:
		mFtp->createThread(this)->pwd()->Run();
		break;
	case FtpEvent::CLOSE:
		mFtp = new AsyncFtp;
		showAccounts();
		break;
	case FtpEvent::PWD:
		mPwd = event.str;
		// no break;
	case FtpEvent::RMDIR:
	case FtpEvent::RMFILE:
	case FtpEvent::CONNECT:
	case FtpEvent::MKDIR:
	case FtpEvent::PUT:
		mStatusBar->SetStatusText(wxT("getting file list..."));
		mFtp->createThread(this)->listFiles()->Run();
		break;
	case FtpEvent::LIST:
		mViewMode = FTP;
		mStatusBar->SetStatusText(wxT("Connected"));
		mList->DeleteAllItems();
		wxListItem item;
		item.SetText(wxT(".."));
		item.SetData((long)true);
		mList->InsertItem(item);
		int n = 1;
		for(Ftp::FileList::iterator i = event.fileList.begin(); i != event.fileList.end(); ++i) {
			item.SetText(wxString(i->first.c_str(), wxConvUTF8));
			item.SetData((long)i->second);
			item.SetId(n);
			item.SetImage(i->second?0:-1);
			mList->InsertItem(item);
			++n;
		}
		break;
	}
}



FtpCtrl::~FtpCtrl() {
}
