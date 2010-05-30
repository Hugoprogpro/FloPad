#ifndef SNIPPETCTRL_H
#define SNIPPETCTRL_H

#include <wx/wx.h>
#include "editor.h"

class FloEditor;

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
