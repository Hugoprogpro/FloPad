#ifndef _CODESTYLE_H_
#define _CODESTYLE_H_

#include "dbconnector.h"
#include <wx/stc/stc.h>
#include <utility>
#include <wx/font.h>
#include <wx/colour.h>
#include "sharedptr.h"

struct CodeStyle {
	int id;
	wxColour background;
	wxColour foreground;
	CodeStyle(int id, wxString bg, wxString fg);
};

class CodeStyleSet {
	int lexer;
	std::vector<CodeStyle> styles;
	std::vector<std::pair<int, wxString> > keywords;
	public:
		CodeStyleSet();
		~CodeStyleSet();
		bool load(wxString ext, SharedPtr<DbConnector> db);
		void apply(wxStyledTextCtrl* control);
};

#endif // _CODESTYLE_H_
