#include <wx/stc/stc.h>

bool isSpace(wxChar c) {
	return c == ' ' || c == '\t' || c == '\n';
}
/*
 *	1. ak je string pokracujeme dokonca stringu bez zmien
 *	2. ak je '<'
 * 		2.1. ak je dalsie '/', zmensi indent
 * 		2.2 zisti ci je <.../>, ak ano, nemen indent
 * 	3. kolabuj viac whitespacov do jedneho
 *	4. bez zmeny
 * */

void formatHtml(wxStyledTextCtrl* ctrl) {
	wxString in = ctrl->GetText();
	wxString out;
	bool inString = false;
	bool inScript = false;
	wxChar beginQuote;
	bool prevSpace = false;
	int indent = 0;
	bool emptyElement = true;
	wxString::iterator i = in.begin();
	wxString::iterator end = in.end();
	while(i != end) {
		if(!isSpace(*i))
			prevSpace = false;
		if(inScript) {
			out += *i;
			if(*i == '?') {
				++i;
				if(i == end) {
					return;
				}
				else {
					out += *i;
					if(*i == '>') { // ?>
						inScript = false;
					}
				}
			}
				
		}
		else if(inString) {
			out += *i;
			if(*i == beginQuote) {
				inString = false;
			}
		}
		else if(*i == '\'' || *i == '\"') {
			beginQuote = *i;
			out += *i;
		}
		else if(*i == '<') {
			++i;
			if(i != end) {
				if(*i == '!') { // doctype
					out += '<';
					out += '!';
				}
				else if(*i == '?') { // php script
					out += '<';
					out += '?';
					inScript = true;
				}
				else if(*i != '/') { // <tag> 
					emptyElement = true;
					out += '\n';
					for(int j = 0; j < indent; ++j) {
						out += '\t';
					}
					out += '<';
					out += *i;
					++indent;
				}
				else { // </tag>
					--indent;
					if(!emptyElement) {
						out += '\n';
						for(int j = 0; j < indent; ++j) {
							out += '\t';
						}
					}
					out += '<';
					out += '/';
					emptyElement = false;
				}
			}
			else {
				out += '<';
				return;
			}
		}
		else if(*i == '/') {
			++i;
			if(i == end) {
				out += *i;
				return;
			}
			else {
				if(*i == '>') { // koniec neparoveho tagu />
					--indent;
				}
				out += '/';
				out += *i;
			}
		}
		else if(isSpace(*i)) {
			if(!prevSpace) {
				out += ' ';
				prevSpace = true;
			}
		}
		else {
			out += *i;
		}
		++i;
	}
	ctrl->SetText(out);
}