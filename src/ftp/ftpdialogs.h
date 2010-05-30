#ifndef _FTPDIALOGS_H_
#define _FTPDIALOGS_H_

#include <wx/wx.h>
#include "../sharedptr.h"
#include "../dbconnector.h"

void addNewFtpAccount(SharedPtr<DbConnector> db);
void editFtpAccount(SharedPtr<DbConnector> db, wxString account);

#endif