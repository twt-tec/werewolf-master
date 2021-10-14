#include "communi.h"
#include <iostream>
#include <ctime>
#include <cstdlib>
#include <algorithm>
#include <vector>
constexpr int _Villager = 0, _Wolf = 1;
twt_server cs;
int B;
using twt = std::vector<int>;
twt all, wolves, now;
int die[105], cha[105];

// Tools
std::string to_string(twt x) {
	std::string an;
	for (int c : x) an = an + to_string(c) + " ";
	return an; 
}
int to_int(std::string x) {
	int an = 0;
	for (char c : x) an = an * 10 + c - '0';
	return an;
}
int doDis(int x, const twt &chas, const twt &cnt) {
	if (cnt[_Wolf] >= B/2)  return _Villager;
	return chas[rand()%chas.size()];
}
void distri() {
	twt chas = {_Villager, _Wolf};
	twt cnt = {0, 0};
	for (int i = 1; i <= B; i++) {
		int c = doDis(i, chas, cnt);
		cs.sent(i, to_string(c));
		cnt[c] ++, cha[i] = c;
		if (c == _Wolf) wolves.push_back(i);
	}
}
void tele(twt a, std::string s) {
	std::cout << "sending : " << to_string(a) << ' ' << s << '\n';
	for (int c : a) cs.sent(c, s);
}
twt _vote(twt a) {
	twt cnt(all.size()+1, 0), ans;
	for (int c : a) {
		int r = to_int(cs.rec(c));
		cnt[r] ++;
		std::cout << c << '\n';
	}
	int mx = *std::max_element(cnt.begin()+1, cnt.end());
	for (int i = 1; i <= B; i++) 
		if (cnt[i] == mx) ans.push_back(i);
	return ans;
}
int vote(twt a) {
	twt cur = _vote(a);
	while (cur.size() > 1) {
		tele(a, std::string("Please vote again in ") + to_string(cur));
		cur = _vote(a);
	}
	tele(a, "ok");
	return cur[0];
}
void erase(int x) {
	int k = 0;
	for (int i = 0; i < (int)now.size(); i++)
		if (now[i] == x) k = i;
	now.erase(now.begin()+k); 
	k = -1;
	for (int i = 0; i < (int)wolves.size(); i++)
		if (wolves[i] == x) k = i;
	if (k != -1) wolves.erase(wolves.begin()+k);
}
void toDie(int x) {
	die[x] = 1;
	tele(all, to_string(x) + " dead");
	erase(x);
}
void talk(twt x) {
	for (int c : x) {
		cs.sent(c, "you");
		std::string r = cs.rec(c);
		tele(all, to_string(c) + " says : " + r);
	}
}
// end Tools


// Characters
void doWolf() {	toDie(vote(wolves)); }
// end Characters


// Days
void night() { 
	tele(all, "night");
	doWolf(); 
}
void day() {
	tele(all, "day");
	talk(now);
	toDie(vote(now));
}
bool check() {
	int cnt = 0;
	for (int i = 1; i <= B; i++) cnt += (cha[i] == _Villager && !die[i]);
	if (!cnt) return tele(all, std::string("&Wolves win!")), 1;
	cnt = 0;
	for (int i = 1; i <= B; i++) cnt += (cha[i] == _Wolf && !die[i]);
	if (!cnt) return tele(all, std::string("&Villagers win!")), 1;
	return 0;
}

int main() {
	std::cout << "How many players?\n> ";
	std::cin >> B;
	for (int i = 1; i <= B; i++) all.push_back(i);
	now = all;
	system("cls");
	std::cout << "Waiting for players...\n";
	cs.init(1234, B);
	system("cls");
	std::cout << "Game is running!";
	tele(all, to_string(B));
	for (int i = 1; i <= B; i++) cs.sent(i, to_string(i));
	distri();
	tele(wolves, to_string(wolves));
	for (int i = 1; ; i++) {
		night(); if (check()) break;
		day(); if (check()) break;
	}
	system("pause");
}
