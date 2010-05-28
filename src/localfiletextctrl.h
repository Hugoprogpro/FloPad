#ifndef LOCALFILETEXTCTRL_H
#define LOCALFILETEXTCTRL_H

#include <string>
#include <wx/filename.h>
#include "filetextctrlbase.h"

class LocalFileTextCtrl : public FileTextCtrlBase {
	wxFileName mFilename;
	virtual bool saveFileAs();
	public:
		LocalFileTextCtrl(wxWindow* parent);
		virtual ~LocalFileTextCtrl();
		virtual bool saveFile();
		virtual wxString getLabel();
		virtual wxString getExtension();
		bool openFile();
};

#endif // LOCALFILETEXTCTRL_H
