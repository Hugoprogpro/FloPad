#ifndef QUICKFINDPANEL_H
#define QUICKFINDPANEL_H

#include <wx/wx.h>
#include <wx/tooltip.h>
#include "editor.h"

class FloEditor;

class QuickFindPanel : public wxToolBar{
	wxToolBar*		mCloseButton;
	wxTextCtrl*		mSearchCtrl;
	wxTextCtrl*		mReplaceCtrl;
	wxCheckBox*		mRegExp;
	FloEditor*		mEditor;
	void onClose(wxCommandEvent& WXUNUSED(event));
	void onFind(wxCommandEvent& WXUNUSED(event));
	void onReplace(wxCommandEvent& event);
	void onKeyUp(wxKeyEvent& event);
	public:
		QuickFindPanel(wxWindow* parent, FloEditor* editor);
		~QuickFindPanel();
		void focus();
};

#endif // QUICKFINDPANEL_H
