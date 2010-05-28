#include "dbconnector.h"


bool DbConnector::query(wxString sql)
{
	if(db == 0)
		return false;
	int rows, cols;
	char** result;
	int rc = sqlite3_get_table(db, (const char*)sql.ToAscii(), &result, &rows, &cols, NULL);

	head.clear();
	data.clear();
	if ( rc == SQLITE_OK ) {
		for (int i=0; i < cols; ++i) {
			head.push_back(wxString(result[i], wxConvUTF8));   /* First row heading */
		}
		for (int i=0; i < cols*rows; ++i){
			data.push_back(wxString(result[cols+i], wxConvUTF8));
		}
	}
	
	sqlite3_free_table(result);
	return rc == SQLITE_OK;
}

wxString DbConnector::getString(int row, wxString col) {
	for(int i = head.size()-1; i >= 0; --i)
		if(head[i] == col) {
			return data[row*head.size()+i];
		}
	return wxT("");
}

DbConnector::DbConnector():db(0)
{
}

void DbConnector::close() {
	if(db != 0)
		sqlite3_close(db);
}

bool DbConnector::open(wxString filename) {
	close();
	
	if (sqlite3_open((const char*)filename.ToAscii(), &db)) {
		sqlite3_close(db);
		db = 0;
		return false;
	}
	return true;
}

DbConnector::~DbConnector()
{
	close();
}

int DbConnector::getRowCount()
{
	int h = head.size();
	if(h == 0)
		return 0;
	return data.size() / h;
}

int DbConnector::getInt(int row, wxString col)
{
	for(int i = head.size()-1; i >= 0; --i)
		if(head[i] == col) {
			long l;
			data[row*head.size()+i].ToLong(&l, 0);
			return l;
		}
	return 0;
}

int DbConnector::getInt(int row, const char* col)
{
	wxString c(col, wxConvUTF8);
	return getInt(row, c);
}

wxString DbConnector::getString(int row, const char* col)
{
	wxString c(col, wxConvUTF8);
	return getString(row, c);
}

