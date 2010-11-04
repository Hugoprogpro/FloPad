#include "ftp.h"

Socket::Socket()
{
}

Socket::~Socket()
{
	close();
}

void Socket::close()
{
	mSocket.Close();
}

bool Socket::isClosed()
{
	return !mSocket.IsConnected();
}

bool Socket::connect(const char* host, int port)
{
	wxIPV4address addr;
	addr.Service(port);
	addr.Hostname(wxString(host, wxConvUTF8));
	mSocket.Connect(addr);
	return true;
}


bool Socket::recvData(std::string& data)
{
	char buffer[256] = "";
	mSocket.Read(buffer, 255);
	data = buffer;
	return true;
}

bool Socket::sendData(const char* data, size_t size)
{
	mSocket.Write(data, size);
	return true;
}

bool Socket::sendData(const std::string& data)
{
	mSocket.Write(data.c_str(), data.size());
	return true;
}


void Socket::setTimeout(int sec, int usec)
{
	mSocket.SetTimeout(sec);
}

Ftp::Ftp():mConnected(false) {
	
}

bool Ftp::connect(const char* host)
{
	mSocket.close();
	if (!mSocket.connect(host, 21))
		return false;
	mSocket.setTimeout(10, 0);
	mConnected = true;
	int r = getResponse();
	if (r != 220) {
		return false;
		close();
	}
	mHost = host;
	return true;
}

bool Ftp::pasv()
{
	if(!mConnected)
		return false;
	sendCommand("PASV\r\n");
	std::string data;
	mSocket.recvData(data);
	if (data.find("227") == 0) {
		unsigned int pos = data.find("(");
		int ip[6];
		if (pos == std::string::npos)
			return false;
		sscanf(data.c_str()+pos+1, "%d,%d,%d,%d,%d,%d", &ip[0], &ip[1], &ip[2], &ip[3], &ip[4], &ip[5]);
		int port = (ip[4] << 8) | ip[5];
		char ipaddr[16];
		sprintf(ipaddr, "%d.%d.%d.%d", ip[0], ip[1], ip[2], ip[3]);
		mDataSocket.close();
		bool r =  mDataSocket.connect(ipaddr, port);
		if(r)
			mDataSocket.setTimeout(10, 0);
		return r;
	} else {
	}
	return false;
}

bool Ftp::login(const char* username, const char* password)
{
	if(!mConnected)
		return false;
	std::string com = "USER ";
	com += username;
	com += "\r\n";
	sendCommand(com.c_str());
	if (getResponse() == 331) {
		com = "PASS ";
		com += password;
		com += "\r\n";
		sendCommand(com.c_str());
		mUsername = username;
		mPassword = password;
		return getResponse() == 230;
	} else return false;
}

bool Ftp::sendCommand(const char* command)
{
	if(!mConnected)
		return false;
	return mSocket.sendData(command);
}

bool Ftp::close()
{
	if (mConnected) {
		sendCommand("QUIT\r\n");
		bool r = (getResponse()/100)%10 == 2;
		mSocket.close();
		mDataSocket.close();
		mConnected = false;
		return r;
	}
	return false;
}

bool Ftp::getLine(std::string& line) {
	if(mConnected) {
		unsigned int pos = mResponse.find("\r\n");
		std::string tmp;
		while(pos == std::string::npos) {
			if(!mSocket.recvData(tmp))
				return false;
			mResponse += tmp;
			pos = mResponse.find("\r\n");
		}
		line = mResponse.substr(0, pos);
		mResponse = mResponse.substr(pos+2);
		return true;
	}
	return false;
}

int Ftp::getResponse()
{
	if(!mConnected)
		return -1;
	std::string line;
	while(true) {
		if(!getLine(line))
			return -1;
		int r = atoi(line.c_str());
		if(r) {
			mLastLine = line;
			return r;
		}
	}
	/*std::string tmp;
	if (mResponse == "") {
		if (!mSocket.recvData(tmp))
			return -1;
		mResponse += tmp;
	}
	int r = atoi(mResponse.c_str());
	if (r != 0) {
		size_t pos =mResponse.find("\r\n");
		if (pos != std::string::npos)
			mResponse = mResponse.substr(pos+2);
		else
			mResponse = "";
		return r;
	}
	int pos = mResponse.find("\r\n", 0);
	if (pos != std::string::npos) {
		r = atoi(mResponse.c_str()+pos+2);
		if (r != 0) {
			size_t pos = mResponse.find("\r\n");
			if (pos == std::string::npos)
				mResponse = "";
			else
				mResponse = mResponse.substr(pos+2);
			return r;
		}
	}
	return -1;*/
}

Ftp::~Ftp()
{
	close();
}


bool Ftp::sendData(const char* data, size_t size)
{
	if(!mConnected)
		return false;
	if (!mDataSocket.isClosed()) {

		return mDataSocket.sendData(data);
	}
	return false;
}

// keep receiving until connection is closed
bool Ftp::recvData(std::string& data)
{
	if(mConnected) {
		std::string s;
		data = "";
		while (!mDataSocket.isClosed() && mDataSocket.recvData(s)) {
			data += s;
		}
		return true;
	}
	return false;
}

bool Ftp::nlist(Ftp::StringList& list)
{
	if(mConnected) {
		if (pasv()) {
			sendCommand("NLST\r\n");
			if (getResponse() == 150) {
				std::string s;
				if(!recvData(s))
					return false;
				list = splitString(s, "\r\n");
				if ((getResponse()/100)%10 == 2)
					return true;
			}
		}
	}
	return false;
}


bool Ftp::pwd(std::string& dirname)
{
	if(mConnected) {
		sendCommand("PWD\n");
		if ((getResponse()/100)%10 == 2) {
			unsigned int pos = mLastLine.find("\"");
			if (pos == std::string::npos)
				return false;
			++pos;
			while (pos < mLastLine.size()) {
				if (mLastLine.at(pos) != '\"')
					dirname += mLastLine.at(pos);
				else if (mLastLine.at(pos+1) == '\"') {
					dirname += "\"";
					++pos;
				} else
					return true;
				++pos;
			}
		}
	}
	return false;
}


bool Ftp::chdir(const char* filename)
{
	if(!mConnected)
		return false;
	std::string s = "CWD ";
	s+= filename;
	s+= "\r\n";
	sendCommand(s.c_str());
	return (getResponse()/100)%10 == 2;
}


bool Ftp::get(const char* filename, std::string& content)
{
	if(mConnected) {
		if (pasv()) {
			std::string com = "RETR ";
			com += filename;
			com += "\r\n";
			sendCommand(com.c_str());
			if (getResponse() == 150) {
				if(!recvData(content))
					return false;
				if((getResponse()/100)%10 == 2);
					return true;
			}
		}
	}
	return false;
}

bool Ftp::put(const char* filename, const wchar_t* data, size_t size)
{
	return put(filename, (const char*)data, sizeof(wchar_t)*size);
}

bool Ftp::put(const char* filename, const char* data, size_t size)
{
	if(!mConnected)
		return false;
	if (pasv()) {
		std::string com = "STOR ";
		com += filename;
		com += "\r\n";
		sendCommand(com.c_str());
		if (getResponse() == 150) {
			bool r = sendData(data, size);
			mDataSocket.close();
			getResponse();
			return r;
		}
	}
	return false;
}


bool Ftp::remove(const char* filename, bool isDir)
{
	if(!mConnected)
		return false;
	std::string com;
	if (isDir)
		com = "RMD ";
	else
		com = "DELE ";
	com += filename;
	com += "\r\n";
	sendCommand(com.c_str());
	return (getResponse()/100)%10 == 2;
}

Ftp::StringList Ftp::splitString(std::string str, std::string delim) const {
	StringList list;
	if(mConnected) {
		unsigned int from = 0;
		unsigned int to = 1;
		int delimSize = delim.size();
		while (to != std::string::npos) {
			to = str.find(delim, from);
			std::string tmp;
			if (to != std::string::npos)
				tmp = str.substr(from, to-from);
			else
				tmp = str.substr(from);
			from = to + delimSize;
			if(tmp != "")
				list.push_back(tmp);
		}
	}
	return list;
}

bool Ftp::mkdir(const char* dirname)
{
	if(!mConnected)
		return false;
	std::string com("MKD ");
	com += dirname;
	com += "\r\n";
	sendCommand(com.c_str());
	return (getResponse()/100)%10 == 2;
}

bool Ftp::rename(const char* from, const char* to)
{
	if(!mConnected)
		return false;
	std::string com("RNFR ");
	com += from;
	com += "\r\n";
	sendCommand(com.c_str());
	if((getResponse()/100)%10 == 3) {
		com = "RNTO ";
		com += to;
		com += "\r\n";
		sendCommand(com.c_str());
		return (getResponse()/100)%10 == 2;
	}
	return false;
}

bool Ftp::list(Ftp::FileList& list)
{
	if (mConnected && pasv()) {
		sendCommand("LIST\r\n");
		if (getResponse() == 150) {
			std::string s; 
			if(!recvData(s))
				return false;
			StringList fl = splitString(s, "\r\n");
			for(StringList::iterator i = fl.begin(); i != fl.end(); ++i) {
				if(i->at(0) == '-' ) {
					StringList tmp = splitString(*i, " ");
					list.push_back(make_pair(tmp.back(), false));
				}
				else if(i->at(0) == 'd') {
					StringList tmp = splitString(*i, " ");
					if(tmp.back() != "." && tmp.back() != "..")
						list.push_back(make_pair(tmp.back(), true));
				}
			}
			if ((getResponse()/100)%10 != 2)
				return false;
			else
				return true;
		}
	}
	return false;
}

bool Ftp::reconnect()
{
	if(!mConnected) {
		bool ret = connect(mHost.c_str());
		return ret && login(mUsername.c_str(), mPassword.c_str());
	}
	return false;
}

