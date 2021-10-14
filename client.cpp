#include "communi.h"
#include <iostream>
#include <windows.h>
#include <vector>
#include <string>
constexpr int _Wolf = 1, _Villager = 0;
twt_client cs;
int id, B, cha, flag;
std::vector<int> all;

int to_int(std::string x) {
	int an = 0;
	for (char c : x) an = an * 10 + c - '0';
	return an;
}
void erase(int x) {
	int k = 0;
	for (int i = 0; i < (int)all.size(); i++)
		if (all[i] == x) k = i;
	all.erase(all.begin()+k); 
}
std::string to_string(std::vector<int> x) {
	std::string an;
	for (int c : x) an = an + to_string(c) + " ";
	return an; 
}
void getDie() {
	std::string c = cs.rec();
	std::cout << c << '\n';
	int p = c.find(' ');
	int x = to_int(c.substr(0, p));
	erase(x);
	if (x == id) std::cout << "YOU DIED!\n", flag = 1;
	system("pause");
}
void print(std::string s) {
	system("cls");
	std::cout << "sta : " << s << '\n';
	std::cout << "players alive : " << to_string(all) << '\n';
	if (flag) std::cout << "YOU HAVE ALREADY DIED!\n";
	std::cout << "Waiting for others...\n";
}
void vote() {
	if (flag) return;
	std::cout << "Vote now!\n> ";
	int x;
	std::cin >> x;
	cs.sent(to_string(x));
	std::string c = cs.rec();
	while (c != "ok") {
		std::cout << c << "\n> ";
		std::cin >> x;
		cs.sent(to_string(x));
		c = cs.rec();
	}
}
void wolf() {
	if (cha != _Wolf || flag) return;
	vote();
}
void night() {
	print("night");
	wolf();
	getDie();
}
void talk() {
	for (int i = 0; i <= (int)all.size()-flag; i++) {
		std::string c = cs.rec();
		if (c == "you") {
			std::cout << "Write your message now:\n> ";
			std::string an;
//			std::cin >> an;
			std::getline(cin, an);
			cs.sent(an);
		}
		else std::cout << c << '\n';
	}
}
void day() {
	print("morning");
	talk();
	vote();
	getDie();
}
void getCha() {
	std::string c = cs.rec();
	if (to_int(c) == _Wolf) std::cout << "You are Wolf\n", cha = _Wolf;
	else std::cout << "You are Villager\n", cha = _Villager;
	if (cha == _Wolf) c = cs.rec(), std::cout << "wolves are player " + c + '\n'; 
}
bool check() {
	std::string s = cs.rec();
	if (s[0] != '&') return false;
	std::cout << s.substr(1, s.size()-1) << '\n';
	return true;
}
int main() {
	system("chcp 936"); system("cls");
	std::cout << "Welcome!\nWaing for other players...\n";
	cs.init(1234);
	std::string c = cs.rec();
	system("cls");
	std::cout << "There are " << c << " players\n";
	B = to_int(c);
	for (int i = 1; i <= B; i++) all.push_back(i);
	c = cs.rec();
	id = to_int(c);
	std::cout << "You are player " << id << "!\n";
	getCha();
	system("pause");
	while (1) {
		if (check()) break; night();
		if (check()) break; day();
	}
	system("pause");
}
