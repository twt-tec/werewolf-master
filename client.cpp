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
//���巢�ͻ������ͽ��ܻ�����
char send_buf[100];
char recv_buf[100];
//���������׽��֣����������׽���
SOCKET s_server;
//����˵�ַ�ͻ��˵�ַ
SOCKADDR_IN server_addr;
void initialization();
int init() {
	//���峤�ȱ���
	
	initialization();
	//���������Ϣ
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
	server_addr.sin_port = htons(1234);
	//�����׽���
	s_server = socket(AF_INET, SOCK_STREAM, 0);
	if (connect(s_server, (SOCKADDR *)&server_addr, sizeof(SOCKADDR)) == SOCKET_ERROR) {
		cout << "����������ʧ�ܣ�" << endl;
		WSACleanup();
	}
	else {
		cout << "���������ӳɹ���" << endl;
	}
}
std::string rec() {
	recv_len = recv(s_server, recv_buf, 100, 0);
	if (recv_len < 0) {
		cout << "����ʧ�ܣ�" << endl;
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
		cout << "����ʧ�ܣ�" << endl;
	}
	
}
void initialization() {
	//��ʼ���׽��ֿ�
	WORD w_req = MAKEWORD(2, 2);//�汾��
	WSADATA wsadata;
	int err;
	err = WSAStartup(w_req, &wsadata);
	if (err != 0) {
		cout << "��ʼ���׽��ֿ�ʧ�ܣ�" << endl;
	}
	else {
		cout << "��ʼ���׽��ֿ�ɹ���" << endl;
	}
	//���汾��
	if (LOBYTE(wsadata.wVersion) != 2 || HIBYTE(wsadata.wHighVersion) != 2) {
		cout << "�׽��ֿ�汾�Ų�����" << endl;
		WSACleanup();
	}
	else {
		cout << "�׽��ֿ�汾��ȷ��" << endl;
	}
	//������˵�ַ��Ϣ
 
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
