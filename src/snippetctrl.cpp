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
		if(d.ShowModal() == wxID_OK) {
			(*mEditor->getDb()) << "update snippets set title = \"" << title->GetValue() << "\", value = \"" 
				<< DbConnector::escape(value->GetValue()) << "\" where title = \"" << oldTitle << "\"" << DbConnector::Execute();
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

wxString getIndentString(wxStyledTextCtrl* ctrl) {
	int pos = ctrl->GetCurrentPos();
	wxString text;
	if(pos > 100) 
		text = ctrl->GetTextRange(pos-100, pos);
	else
		text = ctrl->GetTextRange(0, pos);
	int indent = 0;
	wxString::reverse_iterator i = text.rbegin(), end = text.rend();
	while(i != end && *i != '\n') {
		++i;
	}
	if(i != end) {
		wxString::iterator i2 = i.base(), e2 = text.end();
		while(i2 != e2 && *i2 == '\t') {
			++i2;
			++indent;
		}
	}
	wxString ret;
	for(int i = 0; i < indent; ++i) {
		ret += '\t';
	}
	return ret;
}

void insertSnippet(wxStyledTextCtrl* ctrl, wxString& value) {
	wxString indent = getIndentString(ctrl);
	value.Replace(wxT("\n"), wxString(wxT("\n"))+indent, true);
	int pos = value.Find(wxT("{$}"));
	if(pos != wxNOT_FOUND) {
		value.Replace(wxT("{$}"), wxT(""), false);
	}
	ctrl->ReplaceTarget(value);
	if(pos != wxNOT_FOUND) {
		pos = ctrl->GetCurrentPos() + pos;
		ctrl->SetCurrentPos(pos);
		ctrl->SetSelection(pos, pos);
	}
}

void SnippetCtrl::onDClick(wxListEvent& event) {
	wxStyledTextCtrl* ctrl = mEditor->getSelectedFileTextCtrl();
	if(ctrl) {
		mEditor->getDb()->select(wxT("snippets"), wxT("title"), event.GetText());
		if(mEditor->getDb()->getRowCount() > 0) {
			wxString value = mEditor->getDb()->getString(0, "value");
			ctrl->SetTargetStart(ctrl->GetCurrentPos());
			ctrl->SetTargetEnd(ctrl->GetCurrentPos());
			insertSnippet(ctrl, value);
			ctrl->SetFocus();
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



void snippetAutoCompComplete(FloEditor* editor, wxStyledTextEvent& event) {
	(*editor->getDb()) << "select * from snippets where title = \"" << event.GetText() << "\"" << DbConnector::Execute();
	wxStyledTextCtrl* ctrl = editor->getSelectedFileTextCtrl();
	if(ctrl) {
		wxString value = editor->getDb()->getString(0, "value");
		insertSnippet(ctrl, value);
	}
}

void showSnippetAutoComp(FloEditor* editor)
{
	wxStyledTextCtrl* ctrl = editor->getSelectedFileTextCtrl();
	if(ctrl) {
		wxString title;
		int pos = ctrl->GetCurrentPos();
		wxString text;
		if(pos > 20) 
			text = ctrl->GetTextRange(pos-20, pos);
		else
			text = ctrl->GetTextRange(0, pos);
		title = getWord(text.rbegin(), text.rend());
		(*editor->getDb()) << "select * from snippets where title like \"" << title << "%\"" << DbConnector::Execute();
		wxString list;
		for(int i = 0; i < editor->getDb()->getRowCount(); ++i) {
			if(i != 0)
				list += wxT(",");
			list += editor->getDb()->getString(i, "title");
		}
		if(list != wxT("")) {
			ctrl->SetTargetEnd(pos);
			ctrl->SetTargetStart(pos-title.length());
			ctrl->UserListShow(1, list);
			if(editor->getDb()->getRowCount() == 1) {
				ctrl->AutoCompComplete();
			}
		}
	}
}
