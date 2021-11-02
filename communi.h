#include<winsock.h>
#include <string>
#include <fstream>
#include <iostream>
using namespace std;
const int ERR = 3;
std::ofstream fout;
int to_int(std::string x) {
	for (char c : x) if (c < '0' || c > '9') return -1;
	int an = 0;
	for (char c : x) an = an * 10 + c - '0';
	return an;
}
struct twt_server {
	int send_len = 0;
	int recv_len = 0;
	int len = 0;
	char send_buf[1200];
	char recv_buf[1200];
	bool ok[1000];
	int B = 0;
	SOCKET s_server;
	SOCKET s_accept[1005];
	SOCKADDR_IN server_addr;
	SOCKADDR_IN accept_addr;
	void initialization() {
	    WORD w_req = MAKEWORD(2, 2); 
	    WSADATA wsadata;
	    int err;
	    err = WSAStartup(w_req, &wsadata);
	    if (LOBYTE(wsadata.wVersion) != 2 || HIBYTE(wsadata.wHighVersion) != 2) {
	        cout << "ERR" << endl;
	        exit(0);
	        WSACleanup();
	    } 
	}
	void contect();
	void antiAd(int noAna);
	int init(int x, int y) {
		for (int i = 0; i < 1000; i++) ok[i] = 0;
		B = y;
		fout.open("log.txt");
	    initialization();
	    server_addr.sin_family = AF_INET;
	    server_addr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
	    server_addr.sin_port = htons(x);
	    s_server = socket(AF_INET, SOCK_STREAM, 0);
	    if (bind(s_server, (SOCKADDR *)&server_addr, sizeof(SOCKADDR)) ==
	        SOCKET_ERROR) {
	        cout << "ERR" << endl;
	        exit(0);
	        WSACleanup();
	    }
	    if (listen(s_server, SOMAXCONN) < 0) {
	        cout << "ERR" << endl;
	        exit(0);
	        WSACleanup();
	    } 
	    len = sizeof(SOCKADDR);
	    contect();
	    return 0;
	}
	void close() {
	    closesocket(s_server);
	    closesocket(s_accept[0]);
	    WSACleanup();
	}
	std::string _rec(int x, int len) {
		recv_len = recv(s_accept[x], recv_buf, len, 0);
	    if (recv_len < 0) {
	        cout << "lose connect from " << x << endl;
	        fout << "lose connect from " << x << endl;
	        exit(0);
			return "ERR";
	    } else {
	        std::string an = recv_buf;
	        return an;
	    }
	}
	int _sent(int x, std::string s, int len) {
		if (ok[x]) return 0;
	    int send_len = send(s_accept[x], s.c_str(), len, 0);
	    if (send_len < 0) {
	        ok[x] = 1;
	        return 1;
	    }
	    return 0;
	}
	std::string rec(int x) {
		std::cout << "waiting rec from " << x << '\n';
		fout << "waiting rec from " << x << '\n';
		int l = to_int(_rec(x, 5));
		recv_len = recv(s_accept[x], recv_buf, l, 0);
		recv_buf[recv_len]=0;
	    if (recv_len < 0) {
	        cout << "lose connect from " << x << endl;
	        fout << "lose connect from " << x << endl;
	        exit(0);
			return "ERR";
	    } else {
	        std::string an = recv_buf;
	        std::cout << "rec from " << x << ' ' << an << '\n';
	        fout << "rec from " << x << ' ' << an << '\n';
	        return an;
	    }
	}
	int sent(int x, std::string s) {
		if (ok[x]) return 0;
		_sent(x, to_string(s.size()), 5);
	    int send_len = send(s_accept[x], s.c_str(), s.size(), 0);
	    std::cout << "sending to " << x << " " << s << '\n';
	    fout << "sending to " << x << " " << s << '\n';
	    if (send_len < 0) {
	        cout << "lose connect from " << x << endl;
	        fout << "lose connect from " << x << endl;
	        ok[x] = 1;
	        return 1;
	    }
	    return 0;
	}
} a;
struct twt_client {
	int send_len = 0;
	int recv_len = 0;
	char send_buf[1200];
	char recv_buf[1200];
	SOCKET s_server;
	SOCKADDR_IN server_addr;
	void initialization() {
		WORD w_req = MAKEWORD(2, 2);
		WSADATA wsadata;
		int err;
		err = WSAStartup(w_req, &wsadata);
		if (LOBYTE(wsadata.wVersion) != 2 || HIBYTE(wsadata.wHighVersion) != 2) {
			WSACleanup();
		}
	}
	int init(int x, std::string ip = "no") {
		initialization();
		server_addr.sin_family = AF_INET;
		if (ip == "no")	std::cout << "ip?\n> ", std::cin >> ip;
		if (ip == "o") ip = "127.0.0.1";
		if (ip.size() <= 3) ip = std::string("10.176.20.") + ip;
		server_addr.sin_addr.S_un.S_addr = inet_addr(ip.c_str());
		server_addr.sin_port = htons(x);
		s_server = socket(AF_INET, SOCK_STREAM, 0);
		if (connect(s_server, (SOCKADDR *)&server_addr, sizeof(SOCKADDR)) == SOCKET_ERROR) {
			WSACleanup();
		}
		return 0;
	}
	std::string _rec(int len) {
		recv_len = recv(s_server, recv_buf, len, 0);
		if (recv_len < 0) {
			return "ERR";
		}
		else {
			std::string an = recv_buf;
			return an;
		}
	}
	bool _sent(std::string s, int len) {
		send_len = send(s_server, s.c_str(), len, 0);
		if (send_len < 0) {
//			exit(0);
			return 0;
		} 
		return 1;
	}
	std::string rec(bool op = 0) {
		int l = to_int(_rec(5));
		recv_len = recv(s_server, recv_buf, l, 0);
		recv_buf[recv_len]=0;
		if (recv_len < 0) {
			if (!op) {
				std::cout << "连接丢失\n";
				system("pause");
				exit(0);
			}
			return "ERR";
		}
		else {
			std::string an = recv_buf;
			return an;
		}
	}
	bool sent(std::string s, bool op = 0) {
		_sent(to_string(s.size()), 5);
		send_len = send(s_server, s.c_str(), s.size(), 0);
		if (send_len < 0) {
			if (!op) {
				std::cout << "连接丢失\n";
				system("pause");
			}
//			exit(0);
			return 0;
		} 
		return 1;
	}
	
};
