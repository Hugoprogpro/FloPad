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
	public: 
		struct Execute {};
	private:
		sqlite3 *db;
		wxString internalBuffer;
	public:
		std::vector<wxString> head;
		std::vector<wxString> data;

		DbConnector();
		~DbConnector();
		bool 	open(wxString filename);
		void 	close();
		bool 	query(wxString sql);
		int		getRowCount();
		int		getInt(int row, const char* col);
		int		getInt(int row, wxString col);
		wxString 	getString(int row, wxString col);
		wxString 	getString(int row, const char* col);
		void		select(const wxString& table, const wxString& key, const wxString& value);
		int 		getSettingInt(const wxString& key);
		wxString	getSettingString(const wxString& key);
		void		updateSettings(const wxString key, const wxString& value);
		void		updateSettings(const wxString key, int value);
		DbConnector& operator <<(const char* str);
		DbConnector& operator <<(const wxString& str);
		DbConnector& operator <<(const Execute& e);
		DbConnector& operator <<(int i);
		static wxString escape(const wxString& str);
		sqlite3* getSqlite3Object() { return db; }
};


#endif

