#include "quickfindpanel.h"

QuickFindPanel::QuickFindPanel(wxWindow* parent, FloEditor* editor):wxToolBar(parent, wxID_ANY),mEditor(editor),mLastSuccess(false)
{
	AddTool(wxID_CLOSE, wxT("cancel"), wxBitmap(wxT("data/cancel.png")), wxT("close"), wxITEM_NORMAL);
	AddControl(new wxStaticText(this, wxID_ANY, wxT("find:")));
	mSearchCtrl = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(150, -1), wxTE_PROCESS_ENTER);
	AddControl(mSearchCtrl);
	AddTool(wxID_FIND, wxT("search"), wxBitmap(wxT("data/find.png")), wxT("search"), wxITEM_NORMAL);
	
	AddControl(new wxStaticText(this, wxID_ANY, wxT("replace with:")));
	mReplaceCtrl = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(150, -1), wxTE_PROCESS_ENTER);
	AddControl(mReplaceCtrl);
	AddTool(wxID_REPLACE, wxT("replace"), wxBitmap(wxT("data/text_replace.png")), wxT("replace"), wxITEM_NORMAL);
	
	mRegExp = new wxCheckBox(this, wxID_ANY, wxT("regexp"), wxDefaultPosition, wxSize(80, -1));
	AddControl(mRegExp);
	Connect(wxID_REPLACE, wxEVT_COMMAND_TOOL_CLICKED, (wxObjectEventFunction)&QuickFindPanel::onReplace);
	Connect(wxID_FIND, wxEVT_COMMAND_TOOL_CLICKED, (wxObjectEventFunction)&QuickFindPanel::onFind);
	Connect(wxID_CLOSE, wxEVT_COMMAND_TOOL_CLICKED, (wxObjectEventFunction)&QuickFindPanel::onClose);
	Connect(wxEVT_COMMAND_TEXT_ENTER, (wxObjectEventFunction)&QuickFindPanel::onFind);
	Connect(wxEVT_KEY_UP, (wxObjectEventFunction)&QuickFindPanel::onKeyUp);
}

void QuickFindPanel::onKeyUp(wxKeyEvent& event) {
	if(event.GetKeyCode() == WXK_ESCAPE) {
		this->GetParent()->GetSizer()->Show(this, false);
		this->GetParent()->GetSizer()->Layout();
	}
}


void QuickFindPanel::focus() {
	mSearchCtrl->SetFocus();
}


QuickFindPanel::~QuickFindPanel()
{
}


void QuickFindPanel::onReplace(wxCommandEvent& event)
{
	wxStyledTextCtrl* ctrl = mEditor->getSelectedFileTextCtrl();
	if(mSearchCtrl->GetValue() != wxT("")) {
		if(!mLastSuccess) {
			onFind(event);
		}
		else {
			if(mRegExp->IsChecked()) {
				ctrl->ReplaceTargetRE(mReplaceCtrl->GetValue());
			}
			else {
				ctrl->ReplaceTarget(mReplaceCtrl->GetValue());
			}
			onFind(event);
		}
	}
}


void QuickFindPanel::onFind(wxCommandEvent& WXUNUSED(event))
{
	wxStyledTextCtrl* ctrl = mEditor->getSelectedFileTextCtrl();
	int selEnd = ctrl->GetSelectionEnd();
	if(selEnd != ctrl->GetSelectionStart()) 
		ctrl->SetTargetStart(selEnd);
	else if(ctrl->GetCurrentPos() != -1)
		ctrl->SetTargetStart(ctrl->GetCurrentPos());
	else 
		ctrl->SetTargetStart(0);
	ctrl->SetTargetEnd(ctrl->GetLength());
	if(mRegExp->IsChecked())
		ctrl->SetSearchFlags(0x00200000); // regexp
	else
		ctrl->SetSearchFlags(0);
	int pos = ctrl->SearchInTarget(mSearchCtrl->GetValue());
	if(pos!= -1) {
		mLastSuccess = true;
		ctrl->SetSelection(ctrl->GetTargetStart(), ctrl->GetTargetEnd());
		ctrl->EnsureCaretVisible();
		mSearchCtrl->SetBackgroundColour(*wxWHITE);
	}
	else {
		ctrl->SetTargetEnd(ctrl->GetTargetStart());
		ctrl->SetTargetStart(0);
		pos = ctrl->SearchInTarget( mSearchCtrl->GetValue());
		if(pos!= -1) {
			mLastSuccess = true;
			ctrl->SetSelection(ctrl->GetTargetStart(), ctrl->GetTargetEnd());
			ctrl->EnsureCaretVisible();
			mSearchCtrl->SetBackgroundColour(*wxWHITE);
		}
		else {
			mLastSuccess = false;
			mSearchCtrl->SetBackgroundColour(wxColour(220, 150, 150));
		}
	}
}

void QuickFindPanel::onClose(wxCommandEvent& WXUNUSED(event))
{
	mLastSuccess = false;
	this->GetParent()->GetSizer()->Show(this, false);
	this->GetParent()->GetSizer()->Layout();
}
