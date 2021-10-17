#include<winsock.h>
#include <string>
#include <fstream>
#include <iostream>
using namespace std;
const int ERR = 3;
std::ofstream fout;
struct twt_server {
	int send_len = 0;
	int recv_len = 0;
	int len = 0;
	char send_buf[10005];
	char recv_buf[10005];
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
	int init(int x, int y) {
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
	    for (int i = 1; i <= B; i++) {
	    	s_accept[i] = accept(s_server, (SOCKADDR *)&accept_addr, &len);
	    	system("cls");
	    	std::cout << i << '/' << B;
		    if (s_accept[i] == SOCKET_ERROR) {
		        cout << "ERR" << endl;
				exit(0);
		        WSACleanup();
		        return 0;
		    }
		}
	    return 0;
	}
	void close() {
	    closesocket(s_server);
	    closesocket(s_accept[0]);
	    WSACleanup();
	}
	std::string rec(int x) {
		std::cout << "waiting rec from " << x << '\n';
		fout << "waiting rec from " << x << '\n';
		recv_len = recv(s_accept[x], recv_buf, 100, 0);
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
	    int send_len = send(s_accept[x], s.c_str(), 100, 0);
	    std::cout << "sending to " << x << " " << s << '\n';
	    fout << "sending to " << x << " " << s << '\n';
	    if (send_len < 0) {
	        cout << "lose connect from " << x << endl;
	        fout << "lose connect from " << x << endl;
	        exit(0);
	        return ERR;
	    }
	    return 0;
	}
} a;
struct twt_client {
	int send_len = 0;
	int recv_len = 0;
	char send_buf[10000];
	char recv_buf[10000];
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
	int init(int x) {
		initialization();
		server_addr.sin_family = AF_INET;
		std::string ip;
		std::cout << "ip?\n> ";
		std::cin >> ip;
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
	std::string rec() {
		recv_len = recv(s_server, recv_buf, 100, 0);
		if (recv_len < 0) {
			std::cout << "连接丢失\n";
			system("pause");
			exit(0);
			return "ERR";
		}
		else {
			std::string an = recv_buf;
			return an;
		}
	}
	void sent(std::string s) {
		send_len = send(s_server, s.c_str(), 100, 0);
		if (send_len < 0) {
			std::cout << "连接丢失\n";
			system("pause");
			exit(0);
		} 
	}
	
};
