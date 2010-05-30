#include "editor.h"
#include <sstream>
#include <algorithm>

// a-zA-Z0-9_
bool isWordChar(char c) {
	return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9') || (c == '_') || (c == '-');
}

wxString getWord(wxString::reverse_iterator it, wxString::reverse_iterator rend) {
	wxString ret;
	while(it != rend && isWordChar(*it)) {
		ret += *it;
		++it;
	}
	std::reverse(ret.begin(),ret.end());
	return ret;
}

wxString getWord(wxString::iterator it, wxString::iterator end) {
	wxString ret;
	while(it != end && isWordChar(*it)) {
		ret += *it;
		++it;
	}
	return ret;
}

void showTags(wxStyledTextCtrl* ctrl, SharedPtr<DbConnector> db, const wxString tag) {
	wxString query;
	query = wxT("select distinct(tag) from html_tags where tag like \"")+tag+wxT("%\" order by tag");
	db->query(query);
	if(db->getRowCount()) {
		wxString s;
		for(int i = 0; i < db->getRowCount(); ++i) {
			if(i != 0)
				s += ',';
			s += db->getString(i, "tag");
		}
		ctrl->AutoCompShow(tag.size(), s);
	}
}

void showAttributes(wxStyledTextCtrl* ctrl, SharedPtr<DbConnector> db, const wxString attr, const wxString tag) {
	wxString query;
	query = wxT("select distinct(attr) from html_attrs where tag = \"") + tag + wxT("\" and attr like \"")+attr+wxT("%\"");
	db->query(query);
	if(db->getRowCount()) {
		wxString s;
		for(int i = 0; i < db->getRowCount(); ++i) {
			if(i != 0)
				s += ',';
			s += db->getString(i, "attr");
		}
		ctrl->AutoCompShow(attr.size(), s);
	}
}

wxString getAttribute(wxString text) {
	wxString::reverse_iterator it = text.rbegin();
	while(it != text.rend() && *it != '\"') {
		++it;
	}
	++it;
	while(it != text.rend() && !isWordChar(*it)) {
		++it;
	}
	return getWord(it, text.rend());
}

void showValues(wxStyledTextCtrl* ctrl, SharedPtr<DbConnector> db, const wxString tag, const wxString attr, wxString value) {
	wxString query;
	query = wxT("select distinct(value) from html_values where tagattr = \"")+tag+wxT(",")+attr+wxT("\" and value like \"")+value+wxT("%\" order by value");
	db->query(query);
	if(db->getRowCount()) {
		wxString s;
		for(int i = 0; i < db->getRowCount(); ++i) {
			if(i != 0)
				s += ',';
			s += db->getString(i, "value");
		}
		ctrl->AutoCompShow(value.size(), s);
	}
}

void showEntities(wxStyledTextCtrl* ctrl, SharedPtr<DbConnector> db, const wxString ent) {
	wxString query;
	query = wxT("select entity from html_entities where entity like \"")+ent+wxT("%\" order by entity");
	db->query(query);
	if(db->getRowCount()) {
		wxString s;
		for(int i = 0; i < db->getRowCount(); ++i) {
			if(i != 0)
				s += ',';
			s += db->getString(i, "entity");
		}
		ctrl->AutoCompShow(ent.size(), s);
	}
}

void showPhpFunctions(wxStyledTextCtrl* ctrl, SharedPtr<DbConnector> db, const wxString foo){
	wxString query;
	query = wxT("select function from phpfunctions where function like \"")+foo+wxT("%\" order by function");
	db->query(query);
	if(db->getRowCount()) {
		wxString s;
		for(int i = 0; i < db->getRowCount(); ++i) {
			if(i != 0)
				s += ',';
			s += db->getString(i, "function");
		}
		ctrl->AutoCompShow(foo.size(), s);
	}
}

void showCssProperties(wxStyledTextCtrl* ctrl, SharedPtr<DbConnector> db, const wxString property){
	wxString query;
	query = wxT("select property from cssproperties where property like \"")+property+wxT("%\" order by property");
	db->query(query);
	if(db->getRowCount()) {
		wxString s;
		for(int i = 0; i < db->getRowCount(); ++i) {
			if(i != 0)
				s += ',';
			s += db->getString(i, "property");
		}
		ctrl->AutoCompShow(property.size(), s);
	}
}

void FloEditor::onCompleteWord(wxCommandEvent& WXUNUSED(event)) {
	wxStyledTextCtrl* ctrl = getSelectedFileTextCtrl();
	if(ctrl) {
		int pos = ctrl->GetCurrentPos();
		wxString text;
		if(pos > 100) 
			text = ctrl->GetTextRange(pos-100, pos);
		else
			text = ctrl->GetTextRange(0, pos);
		if(ctrl->GetLexer() == wxSTC_LEX_CSS) {
			showCssProperties(ctrl, mDb, getWord(text.rbegin(), text.rend()));
			return;
		}
		int style = ctrl->GetStyleAt(pos);
		if(style >= wxSTC_HPHP_DEFAULT && style <= wxSTC_HPHP_OPERATOR) {
			showPhpFunctions(ctrl, mDb, getWord(text.rbegin(), text.rend()));
			return;
		}
		wxString::reverse_iterator it = text.rbegin();
		bool nonWordChar = false;
		int quoteCount = 0;
		while(it != text.rend()) {
			if(*it == '&') {
				if(!nonWordChar) {
					showEntities(ctrl, mDb, getWord(text.rbegin(), text.rend()));
					return;
				}
			}
			else if(*it == '<') {
				if(nonWordChar) {
					if(quoteCount % 2 == 1)
						showValues(ctrl, mDb, getWord(it.base(), text.end()), getAttribute(text), getWord(text.rbegin(), text.rend()));
					else
						showAttributes(ctrl, mDb, getWord(text.rbegin(), text.rend()), getWord(it.base(), text.end()));
					return;
				}
				else {
					showTags(ctrl, mDb, getWord(it.base(), text.end()));
					return;
				}
			}
			else if(*it == '>')
				return;
			else if(!isWordChar(*it)) { 
				nonWordChar = true;
				if(*it == '\"')
					++quoteCount;
			}
			++it;
		}
	}
}

void snippetAutoCompComplete(FloEditor* editor, wxStyledTextEvent& event) {
	(*editor->getDb()) << "select * from snippets where title = \"" << event.GetText() << "\"" << DbConnector::Execute();
	wxStyledTextCtrl* ctrl = editor->getSelectedFileTextCtrl();
	if(ctrl) {
		ctrl->ReplaceTarget(editor->getDb()->getString(0, "value"));
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