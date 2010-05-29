#include "editor.h"

FloEditor::FloEditor():wxFrame(NULL, -1, wxT("Editor"), wxDefaultPosition, wxSize(800, 600)) 
{
	mDb = new DbConnector;
	mDb->open(wxT("data/feditor.sql"));
	initNotebook();
	initMenuBar();
	initFtp();
}


FloEditor::~FloEditor()
{
}

void FloEditor::initFtp() {
	mFtp = new FtpCtrl(this, mDb);
	mAuiManager->AddPane(mFtp, wxLEFT, wxT("FTP"));
	mAuiManager->GetPane(mFtp).BestSize(200, 200);
	mAuiManager->Update();
}

void FloEditor::initMenuBar() {
	mMenuBar = new wxMenuBar();
	wxMenu* file = new wxMenu();
	file->Append(wxID_NEW, wxT("&New\tCtrl-N"));
	file->Append(wxID_OPEN, wxT("&Open\tCtrl-O"));
	file->Append(wxID_SAVE, wxT("&Save\tCtrl-S"));
	file->Append(wxID_EXIT, wxT("&Quit\tCtrl-Q"));
	mMenuBar->Append(file, wxT("&File"));

	wxMenu* edit = new wxMenu();
	edit->Append(wxID_UNDO, wxT("Undo\tCtrl-z"));
	edit->Append(wxID_REDO, wxT("Redo\tCtrl-y"));
	edit->AppendSeparator();
	edit->Append(wxID_CUT, wxT("Cut\tCtrl-x"));
	edit->Append(wxID_COPY, wxT("Copy\tCtrl-c"));
	edit->Append(wxID_PASTE, wxT("Paste\tCtrl-v"));
	edit->AppendSeparator();
	edit->Append(COMPLETE_WORD, wxT("Complete word\tCtrl-space"));
	mMenuBar->Append(edit, wxT("&Edit"));

	wxMenu* search = new wxMenu();
	search->Append(wxID_FIND, wxT("Find\tCtrl-f"));
	mMenuBar->Append(search, wxT("&Search"));


	wxMenu* view = new wxMenu();
	view->Append(VIEW_FTP, wxT("FTP\tF9"));
	view->AppendSeparator();
	view->Append(VIEW_LINENUMBERS, wxT("Line numbers"));
	view->Append(VIEW_EOL, wxT("End of line"));
	view->Append(VIEW_WHITESPACES, wxT("Whitespaces"));
	view->Append(VIEW_WORDWRAP, wxT("Word wrap"));
	wxMenu* viewAs = new wxMenu();
	view->AppendSeparator();
	view->AppendSubMenu(viewAs, wxT("View as"));
	viewAs->Append(VIEWAS_CSS, wxT("CSS"));
	viewAs->Append(VIEWAS_HTML, wxT("HTML (+JS, PHP, ...)"));
	mMenuBar->Append(view, wxT("&View"));

	wxMenu* settings = new wxMenu();
	settings->Append(GLOBAL_SETTINGS, wxT("Global"));
	mMenuBar->Append(settings, wxT("Settings"));

	Connect(wxEVT_COMMAND_AUINOTEBOOK_PAGE_CLOSE, (wxObjectEventFunction)&FloEditor::onPageClose);
	Connect(COMPLETE_WORD, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(FloEditor::onCompleteWord));
	Connect(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(FloEditor::onMenuSelected));
	Connect(wxEVT_CLOSE_WINDOW, (wxObjectEventFunction)&FloEditor::onClose);
	SetMenuBar(mMenuBar);
}


void FloEditor::onClose(wxCloseEvent& event) {
	if(!mNotebook) {
		Destroy();
		return;
	}
	bool saved = true;
	int i = mNotebook->GetPageCount()-1;
	while(i >= 0) {
		if(((FileTextCtrlBase*)mNotebook->GetPage(i))->GetModify()) {
			saved = false;
			i = -1;
		}
		--i;
	}
	if(saved) {
		Destroy();
		return;
	}
	else {
		wxMessageDialog d(NULL, wxT("Some files are not saved. Do you want to quit anyway?"), wxT("Confirm"), wxICON_QUESTION | wxYES_NO);
		if(d.ShowModal() == wxID_YES) {
			Destroy();
			return;
		}
	}
	event.Veto();
}

void FloEditor::toggleEol() {
	wxStyledTextCtrl* ctrl = getSelectedFileTextCtrl();
	if(!ctrl)
		return;
	ctrl->SetViewEOL(!ctrl->GetViewEOL());
}

void FloEditor::toggleWordWrap() {
	wxStyledTextCtrl* ctrl = getSelectedFileTextCtrl();
	if(!ctrl)
		return;
	ctrl->SetWrapMode(ctrl->GetWrapMode() == 0?1:0);
}


void FloEditor::toggleWhiteSpaces() {
	wxStyledTextCtrl* ctrl = getSelectedFileTextCtrl();
	if(!ctrl)
		return;
	ctrl->SetViewWhiteSpace(ctrl->GetViewWhiteSpace() == 0?1:0);
}


void FloEditor::toggleLineNumbers() {
	wxStyledTextCtrl* ctrl = getSelectedFileTextCtrl();
	if(!ctrl)
		return;
	if(ctrl->GetMarginType(1) == wxSTC_MARGIN_NUMBER) {
		ctrl->SetMarginType(1, 0);
		ctrl->SetMarginWidth(1, ctrl->GetMarginWidth(1)-50);
	}
	else {
		ctrl->SetMarginType(1, wxSTC_MARGIN_NUMBER);
		ctrl->SetMarginWidth(1, ctrl->GetMarginWidth(1)+50);
	}
}

void FloEditor::onMenuSelected(wxCommandEvent& event) {
	switch(event.GetId()) {
		case GLOBAL_SETTINGS: {
			GlobalSettingsDialog* d = new GlobalSettingsDialog(this, this);
			d->Show();
			}
			break;
		case VIEW_WORDWRAP:
			toggleWordWrap();
			break;
		case VIEW_LINENUMBERS:
			toggleLineNumbers();
			break;
		case VIEW_EOL:
			toggleEol();
			break;
		case VIEW_WHITESPACES:
			toggleWhiteSpaces();
			break;
		case VIEWAS_CSS:
			viewAs(wxT("css"));
			break;
		case VIEWAS_HTML:
			viewAs(wxT("html"));
			break;
		case wxID_FIND:
			mQuickFindPanel->GetParent()->GetSizer()->Show(mQuickFindPanel, true);
			mQuickFindPanel->GetParent()->GetSizer()->Layout();
			mQuickFindPanel->focus();
			break;
		case wxID_CUT:
			if(getSelectedFileTextCtrl() != 0)
				getSelectedFileTextCtrl()->Cut();
			break;
		case wxID_UNDO:
			if(getSelectedFileTextCtrl() != 0)
				getSelectedFileTextCtrl()->Undo();
			break;
		case wxID_REDO:
			if(getSelectedFileTextCtrl() != 0)
				getSelectedFileTextCtrl()->Redo();
			break;
		case wxID_COPY:
			if(getSelectedFileTextCtrl() != 0)
				getSelectedFileTextCtrl()->Copy();
			break;
		case wxID_PASTE:
			if(getSelectedFileTextCtrl() != 0)
				getSelectedFileTextCtrl()->Paste();
			break;
		case wxID_OPEN:
			onOpenFile(event);
			break;
		case wxID_SAVE:
			onSave(event);
			break;
		case wxID_NEW:
			createNewFile();
			break;
		case wxID_EXIT: 
			Close(true);
			break;
		case VIEW_FTP:
			mAuiManager->GetPane(mFtp).Show(!mAuiManager->GetPane(mFtp).IsShown());
			mAuiManager->Update();
			break;
		case COMPLETE_WORD:
			onCompleteWord(event);
			break;
	}
}

void FloEditor::initNotebook() {
	wxPanel* panel = new wxPanel(this);
	wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);
	mNotebook = new wxAuiNotebook(panel);
	mQuickFindPanel = new QuickFindPanel(panel, this);
	sizer->Add(mNotebook, 1, wxEXPAND | wxGROW);
	sizer->Add(mQuickFindPanel, 0, wxGROW | wxEXPAND);
	panel->SetSizerAndFit(sizer);
	sizer->Show(mQuickFindPanel, false);
	
	mAuiManager = new wxAuiManager(this);
	mAuiManager->AddPane(panel, wxCENTER);
	mAuiManager->Update();
	createNewFile();
}

void FloEditor::onSavePointLeft(wxStyledTextEvent& event) {
	FileTextCtrlBase* ctrl = (FileTextCtrlBase*)event.GetEventObject();
	if(ctrl)
		mNotebook->SetPageText(mNotebook->GetPageIndex(ctrl), wxT("*")+ctrl->getLabel());
}

void FloEditor::onSavePointReached(wxStyledTextEvent& event) {
	FileTextCtrlBase* ctrl = (FileTextCtrlBase*)event.GetEventObject();
	if(ctrl)
		mNotebook->SetPageText(mNotebook->GetPageIndex(ctrl), ctrl->getLabel());
}

void FloEditor::createNewFile() {
	LocalFileTextCtrl* sc = new LocalFileTextCtrl(mNotebook);
	addFileTextCtrl(sc);
}


void FloEditor::onOpenFile(wxCommandEvent& WXUNUSED(event)) {
	LocalFileTextCtrl* sc = new LocalFileTextCtrl(mNotebook);
	if(sc->openFile()) {
		addFileTextCtrl(sc);
	}
	else
		delete sc;
}

void FloEditor::onSave(wxCommandEvent& WXUNUSED(event))
{
	FileTextCtrlBase* ctrl = (FileTextCtrlBase*)mNotebook->GetPage(mNotebook->GetSelection());
	if(ctrl && !ctrl->saveFile())
		wxMessageBox(wxT("File was not saved"));
	else {
		mNotebook->SetPageText(mNotebook->GetSelection(), ctrl->getLabel());
		ctrl->SetSavePoint();
	}
}


void FloEditor::addFileTextCtrl(FileTextCtrlBase* ctrl)
{
	mNotebook->AddPage(ctrl, ctrl->getLabel());
	mNotebook->SetSelection(mNotebook->GetPageCount()-1);
	Connect(wxEVT_STC_SAVEPOINTREACHED, (wxObjectEventFunction)&FloEditor::onSavePointReached);
	Connect(wxEVT_STC_SAVEPOINTLEFT, (wxObjectEventFunction)&FloEditor::onSavePointLeft);
	viewAs(ctrl->getExtension());
	ctrl->SetTabWidth(mDb->getSettingInt(wxT("scintilla.tabwidth")));
}


wxStyledTextCtrl* FloEditor::getSelectedFileTextCtrl()
{
	int sel = mNotebook->GetSelection();
	if(sel == -1)
		return 0;
	return (wxStyledTextCtrl*)mNotebook->GetPage(sel);
}




void FloEditor::onPageClose(wxAuiNotebookEvent& event)
{
	int sel = mNotebook->GetSelection();
	if(sel != -1) {
		FileTextCtrlBase* ctrl = (FileTextCtrlBase*)mNotebook->GetPage(sel);
		if(ctrl && ctrl->GetModify()) {
			wxMessageDialog d(NULL, wxT("File not saved. Do you want to close it anyway?"), wxT("Confirm"), wxYES | wxNO);
			if(d.ShowModal() == wxID_NO)
				event.Veto();
		}
	}
}


void FloEditor::viewAs(wxString lang)
{
	wxStyledTextCtrl* ctrl = getSelectedFileTextCtrl();
	if(ctrl) {
		CodeStyleSet styleset;
		styleset.load(lang, mDb);
		styleset.apply(ctrl);
	}
}


SharedPtr<DbConnector> FloEditor::getDb()
{
	return mDb;
}


void FloEditor::setTabWidth(int w)
{
	for(int i = mNotebook->GetPageCount()-1; i>= 0; --i) {
		((wxStyledTextCtrl*)mNotebook->GetPage(i))->SetTabWidth(w);
	}
}

