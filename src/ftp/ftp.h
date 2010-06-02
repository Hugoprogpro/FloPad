#ifndef FTP_H
#define FTP_H

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <cstdio>
#include <cstring>
#include <string>
#include <cstdlib>
#include <iostream>
#include <list>


class Socket {
int mSocket;
bool mConnected;
public:
	Socket();
	~Socket();
	bool connect(const char* host, int port);
	bool sendData(const std::string& data) const;
	bool sendData(const char* data, size_t size);
	bool recvData(std::string& data);
	bool setTimeout(int sec, int usec);
	bool isClosed();
	void close();
};


class Ftp {
	public:
		typedef std::list<std::string> StringList;
		typedef std::list<std::pair<std::string, bool> > FileList;
	private:
		std::string mResponse;
		std::string mLastLine;
		std::string mHost, mUsername, mPassword;
		bool	mConnected;
		Socket	mSocket;
		Socket	mDataSocket;
		bool 	sendCommand(const char* command);
		int 	getResponse();
		bool	recvData(std::string& data);
		bool	getLine(std::string& line);
		bool	sendData(const char* data, size_t size);
		StringList splitString(std::string str, std::string delim) const;
	public:
		~Ftp();
		Ftp();
		bool isConnected() { return mConnected; }
		bool connect(const char* host);
		bool reconnect();
		bool login(const char* username, const char* password);
		bool pasv();
		bool chdir(const char* dirname);
		bool mkdir(const char* dirname);
		bool rename(const char* from, const char* to);
		bool remove(const char* filename, bool isDir = false);
		bool pwd(std::string& dirname);
		bool get(const char* filename, std::string& content);
		bool put(const char* filename, const char* data, size_t size);
		bool put(const char* filename, const wchar_t* data, size_t size);
		bool nlist(StringList& list);
		bool list(FileList& list);
		bool close();
};

#endif // FTP_H