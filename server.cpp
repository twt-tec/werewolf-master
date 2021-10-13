#include <winsock.h>

#include <iostream>
#include <thread>
#include <windows.h>
#include <string>
#include <ctime>
#include <cstdlib>
//#include "pch.h"
#pragma comment(lib, "ws2_32.lib")
using namespace std;
constexpr int ERR = 2;
int B;
int cha[10], die[10];
struct twt {
	int send_len = 0;
	int recv_len = 0;
	int len = 0;
	char send_buf[100];
	char recv_buf[100];
	//定义服务端套接字，接受请求套接字
	SOCKET s_server;
	SOCKET s_accept[2];
	//服务端地址客户端地址
	SOCKADDR_IN server_addr;
	SOCKADDR_IN accept_addr;
	void initialization() {
	    //初始化套接字库
	    WORD w_req = MAKEWORD(2, 2);  //版本号
	    WSADATA wsadata;
	    int err;
	    err = WSAStartup(w_req, &wsadata);
	    //检测版本号
	    if (LOBYTE(wsadata.wVersion) != 2 || HIBYTE(wsadata.wHighVersion) != 2) {
	        cout << "ERR" << endl;
	        WSACleanup();
	    } 
	    //填充服务端地址信息
	}
	int init(int x) {
//		system("chcp 936");
	    initialization();
	    //填充服务端信息
	    server_addr.sin_family = AF_INET;
	    server_addr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
	    server_addr.sin_port = htons(x);
	    //创建套接字
	    s_server = socket(AF_INET, SOCK_STREAM, 0);
	    if (bind(s_server, (SOCKADDR *)&server_addr, sizeof(SOCKADDR)) ==
	        SOCKET_ERROR) {
	        cout << "ERR" << endl;
	        WSACleanup();
	    }
	    //设置套接字为监听状态
	    if (listen(s_server, SOMAXCONN) < 0) {
	        cout << "ERR" << endl;
	        WSACleanup();
	    } 
	    //接受连接请求
	    len = sizeof(SOCKADDR);
	    for (int i = 0; i < B; i++) {
	    	s_accept[i] = accept(s_server, (SOCKADDR *)&accept_addr, &len);
		    if (s_accept[i] == SOCKET_ERROR) {
		        cout << "ERR" << endl;
		        WSACleanup();
		        return 0;
		    }
		}
	    return 0;
	}
	void close() {
	    closesocket(s_server);
	    closesocket(s_accept[0]);
	    //释放DLL资源
	    WSACleanup();
	}
	std::string rec(int x) {
		recv_len = recv(s_accept[x], recv_buf, 100, 0);
	    if (recv_len < 0) {
	        cout << "ERR" << endl;
			return "haha";
	    } else {
	        std::string an = recv_buf;
	        return an;
	    }
	}
	int sent(int x, std::string s) {
	    int send_len = send(s_accept[x], s.c_str(), 100, 0);
	    std::cout << "senting "+s << '\n';
	    if (send_len < 0) {
	        cout << "ERR" << endl;
	        return ERR;
	    }
	}
} a;
void give() {
	int cnt = 0;
	for (int i = 0; i < B; i++) {
		if (cnt >= B/2) cha[i] = 0;
		else cha[i] = rand() % 2;
		cnt += cha[i];
		a.sent(i, std::string() + char('0' + cha[i]));
	}
	
}
void night() {
	for (int i = 0; i < B; i++) a.sent(i, std::string("nigh"));
	static int cnt[20];
	for (int i = 0; i < B; i++) cnt[i] = 0;
	for (int i = 0; i < B; i++) {
		if (!cha[i] || die[i]) {
			continue;
		}
		a.sent(i, "y");
		std::string an = a.rec(i);
		int x = an[0]-'0'-1;
		cnt[x] ++;
	}
	int maxj = 0;
	for (int i = 0; i < B; i++)
		if (cnt[i] > cnt[maxj]) maxj = i;
	die[maxj] = 1;
	for (int i = 0; i < B; i++) a.sent(i, std::string("dead ")+char(maxj+'0'+1));
}
void day() {
	for (int j = 0; j < B; j++) a.sent(j, "day");
	for (int i = 0; i < B; i++) {
		if (die[i]) continue;
		a.sent(i, "-");
		std::string mes = a.rec(i);
		for (int j = 0; j < B; j++) a.sent(j, mes+char(i+'0'));
	} 
	static int cnt[20];
	for (int i = 0; i < B; i++) cnt[i] = 0;
	for (int i = 0; i < B; i++) {
		if (die[i]) continue;
		std::string x = a.rec(i);
		int k = x[0]-'0'-1;
		cnt[k] ++;
	}
	int maxj = 0;
	for (int i = 0; i < B; i++)
		if (cnt[i] > cnt[maxj]) maxj = i;
	die[maxj] = 1;
	for (int i = 0; i < B; i++) a.sent(i, std::string("dead ")+char(maxj+'0'+1));
}
bool check() {
	int cnt = 0;
	for (int i = 0; i < B; i++) cnt += !die[i] && cha[i];
	if (cnt == 0) {
		for (int j = 0; j < B; j++) a.sent(j, "pm");
		return true;
	}
	cnt = 0;
	for (int i = 0; i < B; i++) cnt += !die[i] && !cha[i];
	if (cnt == 0) {
		for (int j = 0; j < B; j++) a.sent(j, "lr");
		return true;
	}
	return false;
}
int main() {
	srand(time(0));
	std::cout << "num?\n> ";
	std::cin >> B;
	std::cout << "waiting for players...\n" ;//<< std::endl;
	a.init(1234);
	for (int i = 0; i < B; i++) std::cout << i, a.sent(i, std::string()+char(B+'0'));
	for (int i = 0; i < B; i++) std::cout << i, a.sent(i, std::string()+char(i+1+'0'));
	give();
	while (1) {
		night();
		if (check()) break;
		day();
		if (check()) break;
	}
}
