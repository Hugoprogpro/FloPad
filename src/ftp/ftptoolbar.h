#ifndef _FTPTOOLBAR_H_
#define _FTPTOOLBAR_H_

#include <wx/wx.h>
#include "../editor.h"

class FtpToolbar : public wxToolBar {
	public:
		FtpToolbar(wxWindow* parent);
		virtual ~FtpToolbar();

		void enableServerTools(bool enabled);
};

#endif // _FTPTOOLBAR_H_
