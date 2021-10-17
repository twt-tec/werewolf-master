#include "communi.h"
#include <iostream>
#include <ctime>
#include <cstdlib>
#include <algorithm>
#include <vector>
#include <functional>
#include <set>
#include <utility>
#include <map>
using twt = std::vector<int>;
template <typename T>
using dwd = std::function<T>;

constexpr int _Villager = 0, _Wolf = 1, _Prophet = 2, _Hunter = 3, _Witch = 4, _Guard = 5;
std::vector<std::string> con = {"村民", "狼", "预言家", "猎人", "女巫", "守卫"};
twt chas 				     = {_Villager, _Wolf, _Prophet, _Hunter, _Witch, _Guard};
twt_server cs;
int B, gg;
twt all, wolves, now, prophets, hunters, witches, guards;
int die[105], cha[105], lst = -1;
twt goal(con.size());
std::set<std::pair<int, int>> tmpDie, tmpHnt;

std::string to_string(twt x);
int to_int(std::string x);
void tele(twt a, std::string s);
void distri();
twt _vote(twt a, dwd<bool(int)> ck, dwd<twt(twt&)> pro);
int vote(twt a, dwd<void(twt)> pk, dwd<bool(int)> ck, dwd<twt(twt&)> pro);
void _erase(twt &w, int x);
void erase(int x);
void doHunter();
void toDie(int x, int c, int &flag);
void talk(twt x);

void doWolf();
void doProphet();

void night(int no);
void day();
bool check();

int main() {
	system("chcp 936"); system("cls"); 
	srand(time(0));
	for (int i = 0; i < con.size(); i++) std::cout << con[i] << ((i == con.size()-1) ?"": ", ");
	std::cout << "?\n> ";
	for (int i = 0; i < con.size(); i++) {
		std::cin >> goal[i];
		if (i > 1 && goal[i] != 0) gg = 1;
		B += goal[i];
	}
	for (int i = 1; i <= B; i++) all.push_back(i);
	now = all;
	std::cout << "Waiting for players...\n";
	cs.init(1234, B);
	system("cls");
	std::cout << "Game is running!";
	tele(all, to_string(B));
	for (int i = 1; i <= B; i++) cs.sent(i, to_string(i));
	distri();
	tele(wolves, to_string(wolves));
	for (int i = 1; ; i++) {
		night(i); if (check()) break;
		day(); if (check()) break;
	}
	system("pause");
}

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
void tele(twt a, std::string s) { for (int c : a) cs.sent(c, s); }
void distri() {
	twt tmp(B);
	for (int i = 0; i < B; i++) tmp[i] = i+1;
	std::random_shuffle(tmp.begin(), tmp.end());
	int cnt = 0;
	for (int i = 0; i < con.size(); i++)
		for (int j = 1; j <= goal[i]; j++) 	
			cha[tmp[cnt++]] = chas[i];
	for (int i = 1; i <= B; i++) {
		cs.sent(i, to_string(cha[i]));
		if (cha[i] == _Wolf) wolves.push_back(i);
		if (cha[i] == _Prophet) prophets.push_back(i);
		if (cha[i] == _Hunter) hunters.push_back(i);
		if (cha[i] == _Witch) witches.push_back(i);
		if (cha[i] == _Guard) guards.push_back(i);
	} 
	if (hunters.size()) tele(all, to_string(hunters[0]));
	else tele(all, "no");
}
bool defCk(int x) { 
	return x >= 0 && x <= B && !die[x];
}
twt dePro(twt &cnt) {
	twt ans;
	int mx = *std::max_element(cnt.begin()+1, cnt.end());
	for (int i = 1; i <= B; i++) 
		if (cnt[i] == mx) ans.push_back(i);
	return ans;
}
twt _vote(twt a, std::map<int, int> &mp, dwd<bool(int)> ck, dwd<twt(twt&)> pro) {
	twt cnt(all.size()+1, 0), ans;
	for (int c : a) {
		int r = to_int(cs.rec(c));
		while (!ck(r)) {
			cs.sent(c, "投票不合法，请重试！"); 
			r = to_int(cs.rec(c));
		}
		cnt[r] ++, mp[c] = r;
	}
	return pro(cnt);
}
std::string to_string(const std::map<int, int> &mp) {
	std::string an;
	for (auto c : mp) an += to_string(c.first) + " -> " + to_string(c.second) + '\n'; 
	return an;
}
void ppPk(twt a) {
	std::vector<bool> fg(B+1, 0);
	for (int x : a) {
		fg[x] = 1;
		cs.sent(x, "you");
	}
	for (int i = 1; i <= B; i++) if (!fg[i]) cs.sent(i, "not you");
	for (int x : a) {
		std::string c = cs.rec(x);
		tele(all, to_string(x) + " 说：" + c);
	}
	tele(all, "over");
}
void defPk(twt a) {  }
int vote(twt a, dwd<void(twt)> pk = defPk, dwd<bool(int)> ck = defCk, dwd<twt(twt&)> pro = dePro) {
	std::map<int, int> mp;
	twt cur = _vote(a, mp, ck, pro);
	while (cur.size() > 1) {
		pk(cur);
		tele(a, std::string("请在这些平票玩家中重新投票（不能弃票）： ") + to_string(cur));
		mp.clear();
		cur = _vote(a, mp, [&ck](int x) { return ck(x) && x; }, pro);
	}
	tele(a, "ok");
	tele(a, to_string(mp));
	return cur[0];
}
void _erase(twt &w, int x) {
	int k = -1;
	for (int i = 0; i < (int)w.size(); i++)
		if (w[i] == x) k = i;
	if (k != -1) w.erase(w.begin()+k);
}
void erase(int x) {
	_erase(wolves, x), _erase(now, x), 
	_erase(prophets, x), _erase(hunters, x), _erase(witches, x);
}

void _toDie(int x, int c, std::set<std::pair<int, int>> &tmp = tmpDie) { 
	tmp.insert({x, c}); 
}
int ddd;
void toDie(int x, int c = -1, int &flag = ddd) {
	die[x] = 1;
	tele(all, to_string(x) + " 死了");
	if (c != _Witch && hunters.size() && x == hunters[0]) flag = 1;
	else erase(x);
}
void apDie() {
	if (tmpDie.size() && tmpDie.begin() -> second == -1) tmpDie.clear(); 
	int kk = 0;
	for (auto c : tmpDie) {
		toDie(c.first, c.second, kk), talk(twt(1, c.first));
		if (kk) doHunter(), kk = 0;
	}
	if (tmpHnt.size()) 
		toDie(tmpHnt.begin()->first, tmpHnt.begin()->second, kk),
		talk(twt(1, tmpHnt.begin()->first)), tmpHnt.clear();
	if (tmpDie.size()) tele(all, "over");
	else tele(all, "!是平安夜呢！");
	tmpDie.clear();
}

void doHunter() {
//	if (!hunters.size()) return;
	int k = vote(hunters);
	_toDie(k, _Hunter, tmpHnt);
	erase(hunters[0]);
}
void talk(twt x) {
	for (int c : x) {
		cs.sent(c, "you");
		std::string r = cs.rec(c);
		tele(all, to_string(c) + " says : " + r);
	}
}
void doWolf() {	_toDie(vote(wolves), _Wolf); }
void doProphet() {
	if (prophets.size() == 0) return;
	int x = vote(prophets);
	if (cha[x] == _Wolf) tele(prophets, "-");
	else tele(prophets, "+");
}

int doPd(int x) { return to_int(cs.rec(x)); }
void doWitch(int no) {
	if (!witches.size()) return;
	static bool poi = 0, ant = 0, be = 0;
	tele(witches, to_string(tmpDie.begin()->first));
	if (!ant) {
		if (!(no != 1 && tmpDie.begin()->first == witches[0])) {
			int k = doPd(witches[0]);
			if (k)	tmpDie.insert({tmpDie.begin()->first, -1}), ant = 1, be = 1;								
		}
	}
	if (!poi && !be) {
		int k = vote(witches, defPk, defCk, [](twt &cnt) {
			return twt(1, std::max_element(cnt.begin(), cnt.end())-cnt.begin()); 
		});
//		std::cout << "hsshsdfhfsaf " << k << '\n';
		if (k != 0) tmpDie.insert({k, _Witch});
	}
	be = 0;
}
bool guCk(int x) { return defCk(x) && x != lst; }
void doGuard() {
	if (!guards.size()) return;
	int k = vote(guards, defPk, guCk);
	lst = k;
	auto it = tmpDie.lower_bound({k, -100});
	if (it == tmpDie.end() || it->first != k) return;
	std::cout << "\nGuard saved " << tmpDie.begin()->first << '\n';
	tmpDie.erase(tmpDie.begin()); 
	std::cout << "*" << tmpDie.size() << '\n';
}
// Days
void night(int no) { 
	tele(all, "night");
	doProphet();
	doWolf(); 
	doWitch(no == 1);
	doGuard();
	apDie();
}
void day() {
	tele(all, "day");
	talk(now);
	_toDie(vote(now, ppPk), _Villager);
	apDie();
}
bool check() {
	int cnt = 0, cnt2 = 0; 
	for (int i = 1; i <= B; i++) 
		cnt += (cha[i] == _Villager && !die[i]),
		cnt2 += ((cha[i] == _Prophet || cha[i] == _Hunter || 
				  cha[i] == _Witch || cha[i] == _Guard) && !die[i]); 
	if (!cnt || (gg && !cnt2)) return tele(all, std::string("&狼赢了!")), 1;
	cnt = 0;
	for (int i = 1; i <= B; i++) cnt += (cha[i] == _Wolf && !die[i]);
	if (!cnt) return tele(all, std::string("&民赢了!")), 1;
	return 0;
}
