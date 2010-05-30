#include "snippetctrl.h"

SnippetCtrl::SnippetCtrl(wxWindow* parent, FloEditor* editor):wxPanel(parent),mEditor(editor) {
	wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);
	mList = new wxListView(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLC_LIST);
	sizer->Add(mList, 1, wxEXPAND);
	SetSizerAndFit(sizer);
	populate();
	Connect(wxEVT_COMMAND_LIST_ITEM_RIGHT_CLICK, (wxObjectEventFunction)&SnippetCtrl::onRightClick);
	Connect(wxEVT_COMMAND_LIST_ITEM_ACTIVATED, (wxObjectEventFunction)&SnippetCtrl::onDClick);
	Connect(wxEVT_COMMAND_MENU_SELECTED, (wxObjectEventFunction)&SnippetCtrl::onMenuSelected);
}

SnippetCtrl::~SnippetCtrl() {
}


void SnippetCtrl::addSnippet()
{
	wxDialog d(NULL, wxID_ANY, wxT("Add Snippet"));
	
	wxFlexGridSizer* sizer;
	sizer = new wxFlexGridSizer(2);
	wxTextCtrl* title = new wxTextCtrl( &d, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( 150,-1 ), 0 ); 
	sizer->Add( new wxStaticText( &d, wxID_ANY, wxT("Title"), wxDefaultPosition, wxDefaultSize, 0 ), 0, wxALL | wxALIGN_CENTER_VERTICAL , 3 );
	sizer->Add( title, 0, wxALL, 3 );
	
	wxTextCtrl* value = new wxTextCtrl( &d, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( 300,200 ), wxTE_MULTILINE | wxTE_PROCESS_TAB);
	sizer->Add( new wxStaticText( &d, wxID_ANY, wxT("Value"), wxDefaultPosition, wxDefaultSize, 0 ), 0, wxALL | wxALIGN_CENTER_VERTICAL , 3 );
	sizer->Add( value, 0, wxALL, 3 );
		
	sizer->Add( new wxButton( &d, wxID_CANCEL, wxT("Cancel"), wxDefaultPosition, wxDefaultSize, 0 ), 0, wxALL | wxALIGN_CENTER, 3 );
	sizer->Add( new wxButton( &d, wxID_OK, wxT("Add"), wxDefaultPosition, wxDefaultSize, 0 ), 0, wxALL | wxALIGN_CENTER, 3 );

	sizer->AddGrowableCol(0);
	d.SetSizer( sizer );
	d.Fit();
	d.Layout();
	if(d.ShowModal() == wxID_OK){
		(*mEditor->getDb()) << "insert into snippets(title, value) values(\"" << title->GetValue() << "\", \"" 
			<< DbConnector::escape(value->GetValue()) << "\")" << DbConnector::Execute();
		populate();
	}
}


void SnippetCtrl::editSnippet()
{
	wxListItem item;
	item.SetId(mList->GetFirstSelected());
	if(mList->GetItem(item)) {
		(*mEditor->getDb()) << "select * from snippets where title = \"" << item.GetText() << "\"" << DbConnector::Execute();
		wxDialog d(NULL, wxID_ANY, wxT("Edit Snippet"));
		
		wxFlexGridSizer* sizer;
		sizer = new wxFlexGridSizer(2);
		wxTextCtrl* title = new wxTextCtrl( &d, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( 150,-1 ), 0 ); 
		sizer->Add( new wxStaticText( &d, wxID_ANY, wxT("Title"), wxDefaultPosition, wxDefaultSize, 0 ), 0, wxALL | wxALIGN_CENTER_VERTICAL , 3 );
		sizer->Add( title, 0, wxALL, 3 );
		wxString oldTitle = mEditor->getDb()->getString(0, "title");
		title->SetValue(oldTitle);
		
		wxTextCtrl* value = new wxTextCtrl( &d, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( 300,200 ), wxTE_MULTILINE | wxTE_PROCESS_TAB );
		sizer->Add( new wxStaticText( &d, wxID_ANY, wxT("Value"), wxDefaultPosition, wxDefaultSize, 0 ), 0, wxALL | wxALIGN_CENTER_VERTICAL , 3 );
		sizer->Add( value, 0, wxALL, 3 );
		value->SetValue(mEditor->getDb()->getString(0, "value"));

		sizer->Add( new wxButton( &d, wxID_CANCEL, wxT("Cancel"), wxDefaultPosition, wxDefaultSize, 0 ), 0, wxALL | wxALIGN_CENTER, 3 );
		sizer->Add( new wxButton( &d, wxID_OK, wxT("Save"), wxDefaultPosition, wxDefaultSize, 0 ), 0, wxALL | wxALIGN_CENTER, 3 );

		sizer->AddGrowableCol(0);
		d.SetSizer( sizer );
		d.Fit();
		d.Layout();
		if(d.ShowModal() == wxID_OK){
			(*mEditor->getDb()) << "update snippets set title = \"" << DbConnector::escape(title->GetValue()) << "\", value = \"" 
				<< value->GetValue() << "\" where title = \"" << oldTitle << "\"" << DbConnector::Execute();
			populate();
		}
	}
}


void SnippetCtrl::onMenuSelected(wxCommandEvent& event) {
	switch(event.GetId()) {
		case ADD_SNIPPET: {
			addSnippet();
		}
		break;
		case REMOVE_SNIPPET: {
			wxMessageDialog d(NULL, wxT("Are you sure?"), wxT("Confirm"), wxYES_NO | wxICON_QUESTION);
			if(d.ShowModal() == wxID_YES) {
				wxListItem item;
				item.SetId(mList->GetFirstSelected());
				if(mList->GetItem(item)) {
					(*mEditor->getDb()) << "delete from snippets where title = \"" << item.GetText() << "\"" << DbConnector::Execute();
					populate();
				}
			}
		}
		break;
		case EDIT_SNIPPET: {
			editSnippet();
		}
		break;
	}
}


void SnippetCtrl::onRightClick(wxListEvent& event) {
	wxMenu* menu = new wxMenu();
	menu->Append(ADD_SNIPPET, wxT("add snippet"));
	if(event.GetIndex() != -1) {
		menu->Append(REMOVE_SNIPPET, wxT("remove snippet"));
		menu->Append(EDIT_SNIPPET, wxT("edit snippet"));
	}
	mList->PopupMenu(menu, event.GetPoint());
}


void SnippetCtrl::onDClick(wxListEvent& event) {
	wxStyledTextCtrl* ctrl = mEditor->getSelectedFileTextCtrl();
	if(ctrl) {
		SharedPtr<DbConnector> db = mEditor->getDb();
		db->select(wxT("snippets"), wxT("title"), event.GetText());
		if(db->getRowCount() > 0) {
			ctrl->InsertText(ctrl->GetCurrentPos(), db->getString(0, "value"));
		}
	}
}

void SnippetCtrl::populate() {
	mList->DeleteAllItems();
	SharedPtr<DbConnector> db = mEditor->getDb();
	db->query(wxT("select * from snippets order by title"));
	for(int i = 0; i < db->getRowCount(); ++i) {
		wxListItem item;
		item.SetText(db->getString(i, "title"));
		item.SetId(i+1);
		mList->InsertItem(item);
	}
}



