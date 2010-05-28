#ifndef FILETEXTCTRLBASE_H
#define FILETEXTCTRLBASE_H

#include <wx/stc/stc.h>

class FileTextCtrlBase : public wxStyledTextCtrl {

public:
	FileTextCtrlBase(wxWindow* parent):wxStyledTextCtrl(parent) {};
	virtual ~FileTextCtrlBase() {};
	virtual bool saveFile() = 0;
	virtual wxString getLabel() = 0;
	virtual wxString getExtension() = 0;
};

#endif // FILETEXTCTRLBASE_H
