#ifndef SNIPPETCTRL_H
#define SNIPPETCTRL_H

#include <wx/wx.h>
#include "editor.h"

class FloEditor;

wxString getWord(wxString::reverse_iterator it, wxString::reverse_iterator rend); //completeword.cpp
wxString getWord(wxString::iterator it, wxString::iterator end); //completeword.cpp
void showSnippetAutoComp(FloEditor* editor);
void snippetAutoCompComplete(FloEditor* editor, wxStyledTextEvent& event);

class SnippetCtrl : public wxPanel {
	FloEditor* mEditor;
	wxListView* mList;
	void populate();
	void onDClick(wxListEvent& event);
	void onRightClick(wxListEvent& event);
	void onMenuSelected(wxCommandEvent& event);
	void addSnippet();
	void editSnippet();
public:
	SnippetCtrl(wxWindow* parent, FloEditor* editor);
	~SnippetCtrl();

};

#endif // SNIPPETCTRL_H
