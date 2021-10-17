#include "communi.h"
#include <iostream>
#include <windows.h>
#include <vector>
#include <string>
constexpr int _Villager = 0, _Wolf = 1, _Prophet = 2, _Hunter = 3, _Witch = 4, _Guard = 5;
std::vector<std::string> con = {"����", "��", "Ԥ�Լ�", "����", "Ů��", "����"};
twt_client cs;
int id, B, cha, flag, hnt;
std::vector<int> all;

int to_int(std::string x) {
	int an = 0;
	for (char c : x) an = an * 10 + c - '0';
	return an;
}
void erase(int x) {
	int k = -1;
	for (int i = 0; i < (int)all.size(); i++)
		if (all[i] == x) k = i;
	if (k != -1) all.erase(all.begin()+k); 
}
std::string to_string(std::vector<int> x) {
	std::string an;
	for (int c : x) an = an + to_string(c) + " ";
	return an; 
}
void pk() {
	std::string c = cs.rec();
	if (c[0] == 'o') return;
	std::string k = cs.rec();
	if (k[0] == 'y') {
		std::cout << "��ƽƱ�ˣ��췢�ԣ�\n> ";
		fflush(stdin);
		std::cin >> k;
		cs.sent(k); 
	} 
	k = cs.rec();
	std::cout << k << '\n';
	cs.rec();
}
int vote(std::string s, int opt = 0) {
	if (flag) return 0;
	std::cout << "������Ҫ" << s << "����\n> ";
	int x;
	fflush(stdin);
	std::cin >> x;
	cs.sent(to_string(x));
	std::string c = cs.rec();
	int lst = 0; 
	while (c != "ok") {
		if (opt) pk();
		std::cout << c << "\n> ";
		fflush(stdin);
		std::cin >> x;
		cs.sent(to_string(x));
		lst = x;
		c = cs.rec();
	}
	std::string res = cs.rec();
	std::cout << "����ͶƱ��\n";
	std::cout << res; 
	return lst;
}
void doHunter() {
	vote("����");
}
void getDie();
void _getDie(std::string c) {
	if (flag) return;
	std::cout << c << '\n';
	int p = c.find(' ');
	int x = to_int(c.substr(0, p));
	erase(x);
//	std::cout << "kk";
	if (x == id) {
		cs.rec();
		std::cout << "������!\n�������ԣ�\n> ";
		std::string yy;
		fflush(stdin); 
		std::cin >> yy;
		cs.sent(yy);
		cs.rec();
	}
	if (cha == _Hunter && x == id) doHunter();
	if (x == id) flag = 1;
	if (x == hnt) _getDie(cs.rec());
}
void print(std::string s) {
	system("cls");
	std::cout << "ʱ�� : " << s << '\n';
	std::cout << "��ı���� " << id << "!\n";
	std::cout << "�����ŵ���� : " << to_string(all) << '\n';
	if (flag) std::cout << "���Ѿ�����!\n";
	std::cout << "�ȴ��������...\n";
}
void wolf() {
	if (cha != _Wolf || flag) return;
	vote("ɱ��");
}
void prophet() {
	if (cha != _Prophet || flag) return;
	vote("����");
	std::string an = cs.rec();
	if (an == "+") std::cout << "�Ǹ�����!\n";
	else std::cout << "���ǣ�!\n";
}
void talk() {
//	std::cout << all.size() << '\n';
	for (int i = 0; i <= (int)all.size()-flag; i++) {
		std::string c = cs.rec();
		if (c == "you") {
			std::cout << "������ķ���:\n> ";
			std::string an;
//			std::cin >> an;
            fflush(stdin);
			std::getline(cin, an);
			cs.sent(an);
		}
		else std::cout << c << '\n';
	}
}
void getDie() {
	if (flag) return;
	std::string an = cs.rec();
	while (an[0] != '!' && an[0] != 'o') _getDie(an), an = cs.rec();
	if (an[0] == '!') std::cout << an << '\n';
	system("pause");
}
void witch(int no) {
	if (cha != _Witch || flag) return; 
	static int jy = 0, dy = 0, be = 0;
	std::string c = cs.rec();
	std::cout << c << " ���ˣ�\n";
	if (jy && dy) system("pause");
	if (jy == 0 && !(no != 1 && to_int(c) == id)) {
		std::cout << "�Ƿ�ʹ�ý�ҩ��(0 �� 1 ��)\n> ";
		int x;
		fflush(stdin);
		std::cin >> x;
		cs.sent(to_string(x));
		jy = 1;
		if (x) be = 1;
	} 
	if (dy == 0 && !be) {
		if (vote("��") != 0) dy = 1;
	}
	be = 0;
} 
void guard() {
	if (cha != _Guard || flag) return;
	vote("����");
}
void night(int x) {
	print("����");
	wolf();
	prophet();
	witch(x);
	guard();
	getDie();
}
void day() {
	print("����");
	talk();
	vote("ͶƱ����", 1);
	getDie();
}
void getCha() {
	std::string c = cs.rec();
	cha = to_int(c);
	c = cs.rec();
	if (c != "no") hnt = to_int(c);
	std::cout << "�������� " << con[cha] << '\n';
	if (cha == _Wolf) c = cs.rec(), std::cout << "���е����ǣ���Ķ��ѣ� " + c + '\n'; 
}
bool check() {
	std::string s = cs.rec();
	if (s[0] != '&') return false;
	std::cout << s.substr(1, s.size()-1) << '\n';
	return true;
}
int main() {
	system("chcp 936"); system("cls");
	std::cout << "��ӭʹ��̷�̷����ɱ��ʦ��\n�ȴ��������...\n";
	cs.init(1234);
	std::string c = cs.rec();
	system("cls");
	std::cout << "һ���� " << c << " �����\n";
	B = to_int(c);
	for (int i = 1; i <= B; i++) all.push_back(i);
	c = cs.rec();
	id = to_int(c);
	std::cout << "��ı���� " << id << "!\n";
	getCha();
	system("pause");
	while (1) {
		static int o = 1;
		if (check()) break; night(o);
		if (check()) break; day();
		o = 0;
	}
	system("pause");
}
