#ifndef GLOBALSETTINGS_H
#define GLOBALSETTINGS_H

#include <wx/wx.h>
#include <wx/notebook.h>
#include "editor.h"

class FloEditor;

class GlobalSettingsDialog : public wxDialog {
	wxNotebook* mNotebook;
	wxTextCtrl* mTabWidthCtrl;
	FloEditor*	mEditor;
	void addGeneralPage();
	void onButtonClick(wxCommandEvent& event);
	void apply();
	public:
		GlobalSettingsDialog(wxWindow* parent, FloEditor* editor);
		~GlobalSettingsDialog();
};

#endif // GLOBALSETTINGS_H
