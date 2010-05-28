#ifndef _DBCONNECTOR_H_
#define _DBCONNECTOR_H_

#include <sqlite3.h>
#include <vector>
#include <string>
#include <iostream>
#include <cstring>
#include <wx/string.h>

class DbConnector
{
	private:
		sqlite3 *db;
	
	public:
		std::vector<wxString> head;
		std::vector<wxString> data;

		DbConnector();
		bool open(wxString filename);
		void close();
		bool query(wxString sql);
		int getRowCount();
		int getInt(int row, const char* col);
		int getInt(int row, wxString col);
		wxString getString(int row, wxString col);
		wxString getString(int row, const char* col);
		~DbConnector();
};


#endif

