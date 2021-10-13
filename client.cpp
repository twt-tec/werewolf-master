//#include "pch.h"
#include<iostream>
#include<winsock.h>
#include <string>
#include <windows.h>
#pragma comment(lib,"ws2_32.lib")
using namespace std;
int B, id, num, flag;
namespace TWT {
int send_len = 0;
int recv_len = 0;
//定义发送缓冲区和接受缓冲区
char send_buf[100];
char recv_buf[100];
//定义服务端套接字，接受请求套接字
SOCKET s_server;
//服务端地址客户端地址
SOCKADDR_IN server_addr;
void initialization();
int init() {
	//定义长度变量
	
	initialization();
	//填充服务端信息
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
	server_addr.sin_port = htons(1234);
	//创建套接字
	s_server = socket(AF_INET, SOCK_STREAM, 0);
	if (connect(s_server, (SOCKADDR *)&server_addr, sizeof(SOCKADDR)) == SOCKET_ERROR) {
		cout << "服务器连接失败！" << endl;
		WSACleanup();
	}
	else {
		cout << "服务器连接成功！" << endl;
	}
}
std::string rec() {
	recv_len = recv(s_server, recv_buf, 100, 0);
	if (recv_len < 0) {
		cout << "接受失败！" << endl;
		return "haha";
	}
	else {
		std::string an = recv_buf;
		return an;
	}
}
void sent(std::string s) {
	send_len = send(s_server, s.c_str(), 100, 0);
	if (send_len < 0) {
		cout << "发送失败！" << endl;
	}
	
}
void initialization() {
	//初始化套接字库
	WORD w_req = MAKEWORD(2, 2);//版本号
	WSADATA wsadata;
	int err;
	err = WSAStartup(w_req, &wsadata);
	if (err != 0) {
		cout << "初始化套接字库失败！" << endl;
	}
	else {
		cout << "初始化套接字库成功！" << endl;
	}
	//检测版本号
	if (LOBYTE(wsadata.wVersion) != 2 || HIBYTE(wsadata.wHighVersion) != 2) {
		cout << "套接字库版本号不符！" << endl;
		WSACleanup();
	}
	else {
		cout << "套接字库版本正确！" << endl;
	}
	//填充服务端地址信息
 
}
}
using namespace TWT;
void night() {
	std::cout << "close your eye!\n";
	if (id && !flag) {
		std::cout << "wating for other wolf please!\n";
		rec();
		std::cout << "now is your turn. input the man that you want to kill\n";
		int x;
		std::cin >> x;
		sent(std::string()+char(x+'0'));
	}
	std::string k = rec();
	std::cout << k << '\n';
	B--;
	if (k.back()-'0' == num) flag = 1;
}
void day() {
	std::cout << "is morning!\n";
	std::cout << "wating for other plays\n";
	for (int i = 0; i <= B; i++) {
		std::string k = rec();
		if (k[0] == '-') {
			std::cout << "is your turn! input what you want to say!\n> ";
			std::string s;
			std::cin >> s;
			sent(s);
		}
		else std::cout << k.back()+1 << " says:" << k << '\n';
	}
	if (!flag) std::cout << "vote who you want to kill!\n> ";
	int x;
	if (!flag) std::cin >> x;
	if (!flag) sent(std::string()+char(x+'0'));
	std::cout << "wating for other plays...\n";
	std::string k = rec();
	std::cout << k << '\n';
	B--;
	if (k.back()-'0' == num) flag = 1;
}
bool check() {
	std::string an = rec(); 
	if (an == "lr") return std::cout << "wolves win", 1;
	else if (an == "pm") return std::cout << "villagers win", 1;
	return 0;
}
int main() {
	system("chcp 936");
	init();
	std::cout << "wait for other players...\n";
	std::string c = rec();
	std::cout << "there are " + c + " players, the game has began!\n";
	B = c[0]-'0';
	c = rec();
	std::cout << "you are player " << c << '\n';
	c = rec();
	num = c[0]-'0'+1;
	if (c == "1") id = 1;
	std::cout << (std::string)"You are " + (c == "0" ? "villager" : "wolf") << '\n';
	while (1) {
		if (!check()) night();
		else break;
		if (!check()) day();
		else break;
	}
	system("pause");
}
