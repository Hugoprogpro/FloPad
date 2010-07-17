#include "globalsettings.h"

GlobalSettingsDialog::GlobalSettingsDialog(wxWindow* parent, FloEditor* editor):wxDialog(parent, wxID_ANY, (wxString)wxT("Global settings")),mEditor(editor)
{
	wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);
	mNotebook = new wxNotebook(this, wxID_ANY);
	sizer->Add(mNotebook, 1, wxEXPAND);
	addGeneralPage();
	wxBoxSizer* subsizer = new wxBoxSizer(wxHORIZONTAL);
	sizer->Add(subsizer, 0, wxALL | wxALIGN_CENTER_HORIZONTAL);
	subsizer->Add(new wxButton(this, wxID_OK, wxT("OK")));
	subsizer->Add(new wxButton(this, wxID_CANCEL, wxT("Cancel")));
	subsizer->Add(new wxButton(this, wxID_APPLY, wxT("Apply")));
	this->SetSizerAndFit(sizer);
	SetSize(500, 400);
	Connect(wxEVT_COMMAND_BUTTON_CLICKED, (wxObjectEventFunction)&GlobalSettingsDialog::onButtonClick);
}

GlobalSettingsDialog::~GlobalSettingsDialog()
{
}

void GlobalSettingsDialog::addGeneralPage() {
	wxPanel* panel = new wxPanel(mNotebook);
	wxFlexGridSizer* sizer = new wxFlexGridSizer(2);

	sizer->Add(new wxStaticText(panel, wxID_ANY, wxT("Tab width:")), 1, wxALIGN_CENTER_VERTICAL | wxALL);
	mTabWidthCtrl = new wxTextCtrl(panel, wxID_ANY);
	mTabWidthCtrl->SetValue(mEditor->getDb()->getSettingString(wxT("scintilla.tabwidth")));
	sizer->Add(mTabWidthCtrl);


	sizer->AddGrowableCol(0);
	sizer->AddGrowableCol(1);
	panel->SetSizerAndFit(sizer);
	mNotebook->AddPage(panel, wxT("General"));
}

void GlobalSettingsDialog::apply() {
	wxString pageLabel = mNotebook->GetPageText(mNotebook->GetSelection());
	SharedPtr<DbConnector> db = mEditor->getDb();
	if(pageLabel == wxT("General")) {
		db->updateSettings(wxT("scintilla.tabwidth"), mTabWidthCtrl->GetValue());
		long l;
		mTabWidthCtrl->GetValue().ToLong(&l);
		mEditor->setTabWidth(l);
	}
}

void GlobalSettingsDialog::onButtonClick(wxCommandEvent& event)
{
	if(event.GetId() == wxID_APPLY) {
		apply();
	} 
	else if(event.GetId() == wxID_OK) {
		apply();
		Destroy();
	} 
	else if(event.GetId() == wxID_CANCEL) {
		Destroy();
	} 
}


