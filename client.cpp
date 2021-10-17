#include "communi.h"
#include <iostream>
#include <windows.h>
#include <vector>
#include <string>
constexpr int _Villager = 0, _Wolf = 1, _Prophet = 2, _Hunter = 3, _Witch = 4, _Guard = 5;
std::vector<std::string> con = {"村民", "狼", "预言家", "猎人", "女巫", "守卫"};
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
		std::cout << "你平票了，快发言！\n> ";
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
	std::cout << "输入你要" << s << "的人\n> ";
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
	std::cout << "本次投票：\n";
	std::cout << res; 
	return lst;
}
void doHunter() {
	vote("带走");
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
		std::cout << "你死了!\n发表遗言！\n> ";
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
	std::cout << "时间 : " << s << '\n';
	std::cout << "你的编号是 " << id << "!\n";
	std::cout << "还活着的玩家 : " << to_string(all) << '\n';
	if (flag) std::cout << "你已经死了!\n";
	std::cout << "等待其它玩家...\n";
}
void wolf() {
	if (cha != _Wolf || flag) return;
	vote("杀死");
}
void prophet() {
	if (cha != _Prophet || flag) return;
	vote("检验");
	std::string an = cs.rec();
	if (an == "+") std::cout << "是个好人!\n";
	else std::cout << "是狼！!\n";
}
void talk() {
//	std::cout << all.size() << '\n';
	for (int i = 0; i <= (int)all.size()-flag; i++) {
		std::string c = cs.rec();
		if (c == "you") {
			std::cout << "输入你的发言:\n> ";
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
	std::cout << c << " 死了！\n";
	if (jy && dy) system("pause");
	if (jy == 0 && !(no != 1 && to_int(c) == id)) {
		std::cout << "是否使用解药？(0 否 1 是)\n> ";
		int x;
		fflush(stdin);
		std::cin >> x;
		cs.sent(to_string(x));
		jy = 1;
		if (x) be = 1;
	} 
	if (dy == 0 && !be) {
		if (vote("毒") != 0) dy = 1;
	}
	be = 0;
} 
void guard() {
	if (cha != _Guard || flag) return;
	vote("保卫");
}
void night(int x) {
	print("晚上");
	wolf();
	prophet();
	witch(x);
	guard();
	getDie();
}
void day() {
	print("白天");
	talk();
	vote("投票处死", 1);
	getDie();
}
void getCha() {
	std::string c = cs.rec();
	cha = to_int(c);
	c = cs.rec();
	if (c != "no") hnt = to_int(c);
	std::cout << "你的身份是 " << con[cha] << '\n';
	if (cha == _Wolf) c = cs.rec(), std::cout << "所有的狼是（你的队友） " + c + '\n'; 
}
bool check() {
	std::string s = cs.rec();
	if (s[0] != '&') return false;
	std::cout << s.substr(1, s.size()-1) << '\n';
	return true;
}
int main() {
	system("chcp 936"); system("cls");
	std::cout << "欢迎使用谭炜谭狼人杀大师！\n等待其它玩家...\n";
	cs.init(1234);
	std::string c = cs.rec();
	system("cls");
	std::cout << "一共有 " << c << " 个玩家\n";
	B = to_int(c);
	for (int i = 1; i <= B; i++) all.push_back(i);
	c = cs.rec();
	id = to_int(c);
	std::cout << "你的编号是 " << id << "!\n";
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
