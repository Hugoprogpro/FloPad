#include "ftpdialogs.h"


void addNewFtpAccount(SharedPtr<DbConnector> db)
{
	wxDialog d(NULL, wxID_ANY, (wxString)wxT("Add FTP server"));
	//d.SetSize(300,140);
	//d.SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	wxFlexGridSizer* sizer;
	sizer = new wxFlexGridSizer(2);
	wxTextCtrl* title = new wxTextCtrl( &d, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( 150,-1 ), 0 ); 
	sizer->Add( new wxStaticText( &d, wxID_ANY, wxT("Title"), wxDefaultPosition, wxDefaultSize, 0 ), 0, wxALL | wxALIGN_CENTER_VERTICAL , 3 );
	sizer->Add( title, 0, wxALL, 3 );
	
	wxTextCtrl* host = new wxTextCtrl( &d, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( 150,-1 ), 0 );
	sizer->Add( new wxStaticText( &d, wxID_ANY, wxT("Host"), wxDefaultPosition, wxDefaultSize, 0 ), 0, wxALL | wxALIGN_CENTER_VERTICAL , 3 );
	sizer->Add( host, 0, wxALL, 3 );
	
	wxTextCtrl* username = new wxTextCtrl( &d, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( 150,-1 ), 0 );
	sizer->Add( new wxStaticText(&d, wxID_ANY, wxT("Username"), wxDefaultPosition, wxDefaultSize, 0 ), 0, wxALL | wxALIGN_CENTER_VERTICAL , 3 );
	sizer->Add( username, 0, wxALL, 3 );
	
	wxTextCtrl* password = new wxTextCtrl( &d, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( 150,-1 ), 0 );
	sizer->Add( new wxStaticText( &d, wxID_ANY, wxT("Password"), wxDefaultPosition, wxDefaultSize, 0 ), 0, wxALL | wxALIGN_CENTER_VERTICAL , 3 );
	sizer->Add( password, 0, wxALL, 3 );
	
	sizer->Add( new wxButton( &d, wxID_CANCEL, wxT("Cancel"), wxDefaultPosition, wxDefaultSize, 0 ), 0, wxALL | wxALIGN_CENTER, 3 );
	sizer->Add( new wxButton( &d, wxID_OK, wxT("Save"), wxDefaultPosition, wxDefaultSize, 0 ), 0, wxALL | wxALIGN_CENTER, 3 );

	sizer->AddGrowableCol(0);
	d.SetSizer( sizer );
	d.Fit();
	d.Layout();
	if(d.ShowModal() == wxID_OK){
		wxString query = wxT("insert into ftpaccounts(host, password, title, username) values(\"");
		query += host->GetValue() + wxT("\", \"") + password->GetValue() + wxT("\", \"") + title->GetValue() + wxT("\", \"") + username->GetValue();
		query += wxT("\")");
		db->query(query);
	}
}


void editFtpAccount(SharedPtr<DbConnector> db, wxString account)
{
	db->query(wxT("select * from ftpaccounts where title = \"")+account+wxT("\""));
	if(db->getRowCount() <= 0)
		return;
	wxDialog d(NULL, wxID_ANY, (wxString)wxT("Add FTP server"));
	wxFlexGridSizer* sizer;
	sizer = new wxFlexGridSizer(2);
	wxTextCtrl* title = new wxTextCtrl( &d, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( 150,-1 ), 0 ); 
	title->SetValue(db->getString(0, "title"));
	sizer->Add( new wxStaticText( &d, wxID_ANY, wxT("Title"), wxDefaultPosition, wxDefaultSize, 0 ), 0, wxALL | wxALIGN_CENTER_VERTICAL , 3 );
	sizer->Add( title, 0, wxALL, 3 );
	
	wxTextCtrl* host = new wxTextCtrl( &d, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( 150,-1 ), 0 );
	host->SetValue(db->getString(0, "host"));
	sizer->Add( new wxStaticText( &d, wxID_ANY, wxT("Host"), wxDefaultPosition, wxDefaultSize, 0 ), 0, wxALL | wxALIGN_CENTER_VERTICAL , 3 );
	sizer->Add( host, 0, wxALL, 3 );
	
	wxTextCtrl* username = new wxTextCtrl( &d, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( 150,-1 ), 0 );
	username->SetValue(db->getString(0, "username"));
	sizer->Add( new wxStaticText(&d, wxID_ANY, wxT("Username"), wxDefaultPosition, wxDefaultSize, 0 ), 0, wxALL | wxALIGN_CENTER_VERTICAL , 3 );
	sizer->Add( username, 0, wxALL, 3 );
	
	wxTextCtrl* password = new wxTextCtrl( &d, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( 150,-1 ), 0 );
	password->SetValue(db->getString(0, "password"));
	sizer->Add( new wxStaticText( &d, wxID_ANY, wxT("Password"), wxDefaultPosition, wxDefaultSize, 0 ), 0, wxALL | wxALIGN_CENTER_VERTICAL , 3 );
	sizer->Add( password, 0, wxALL, 3 );
	
	sizer->Add( new wxButton( &d, wxID_CANCEL, wxT("Cancel"), wxDefaultPosition, wxDefaultSize, 0 ), 0, wxALL | wxALIGN_CENTER, 3 );
	sizer->Add( new wxButton( &d, wxID_OK, wxT("Save"), wxDefaultPosition, wxDefaultSize, 0 ), 0, wxALL | wxALIGN_CENTER, 3 );

	sizer->AddGrowableCol(0);
	d.SetSizer( sizer );
	d.Fit();
	d.Layout();
	if(d.ShowModal() == wxID_OK){
		wxString query = wxT("update ftpaccounts set host = \"");
		query += host->GetValue() + wxT("\", password = \"") + password->GetValue() + wxT("\", title = \"") + title->GetValue();
		query += wxT("\", username = \"") + username->GetValue() + wxT("\" where title = \"") + db->getString(0, "title") + wxT("\"");
		db->query(query);
	}
}
