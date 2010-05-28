#include "codestyleset.h"

CodeStyleSet::CodeStyleSet()
{
}

CodeStyleSet::~CodeStyleSet()
{
}

void CodeStyleSet::apply(wxStyledTextCtrl* control) {
	control->StyleClearAll();
	control->SetLexer(lexer);
	control->SetStyleBits(control->GetStyleBitsNeeded());
	wxFont font = wxFont(10, wxFONTFAMILY_TELETYPE, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL);
	for(int i = styles.size()-1; i >= 0; --i) {
		const CodeStyle& tmp = styles[i];
		control->StyleSetFont(tmp.id, font);
		control->StyleSetBackground(tmp.id, tmp.background);
		control->StyleSetForeground(tmp.id, tmp.foreground);
	}
	control->SetMarginType(1, wxSTC_MARGIN_NUMBER);
	control->SetMarginWidth(1, 50);	
	for(int i = keywords.size()-1; i >= 0; --i) {
		control->SetKeyWords(keywords[i].first, keywords[i].second);
	}
	control->Colourise(0, wxSTC_INVALID_POSITION);	
}

bool CodeStyleSet::load(wxString extension, SharedPtr<DbConnector> db) {
	styles.clear();
	keywords.clear();
	if(db->query(stc2wx("select lexer from lexers where filetypes like \"%;")+extension+wxT("%;\""))) {
		if(db->getRowCount() != 1) {
			db->query(wxT("select lexer from lexers where filetypes like \"%html%;\""));
		}
			
		lexer = db->getInt(0, wxT("lexer"));
		if(db->query(stc2wx("select id, fg, bg from styles where lexer = ") << lexer)) {
			for(int i = db->getRowCount() - 1; i >= 0; --i) {
				styles.push_back( CodeStyle(db->getInt(i, "id"), db->getString(i, "bg"), db->getString(i, "fg")) );
			}
			if(db->query(stc2wx("select * from lexer_keywords where lexer = ") << lexer)) {
				for(int i = db->getRowCount() - 1; i >= 0; --i) {
					keywords.push_back(std::make_pair(db->getInt(i, "id"), db->getString(i, "keywords")));
				}
			}
			return true;
		}
		else 
			return false;
	}
	else 
		return false;
}

CodeStyle::CodeStyle(int id, wxString bg, wxString fg)
{
	this->id = id;
	this->foreground = wxColour(fg);
	this->background = wxColour(bg);
}

