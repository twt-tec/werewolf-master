#include "communi.h"
#include <iostream>
#include <algorithm>
#include <vector>
#include <functional>
#include <set>
#include <utility>
#include <map>
#include <random>
#include <chrono>
using twt = std::vector<int>;
template <typename T>
using dwd = std::function<T>;

constexpr int _Villager = 0, _Wolf = 1, _Prophet = 2, _Hunter = 3, _Witch = 4, _Guard = 5;
std::vector<std::string> con = {"����", "��", "Ԥ�Լ�", "����", "Ů��", "����"};
twt chas 				     = {_Villager, _Wolf, _Prophet, _Hunter, _Witch, _Guard};
/* 
 * ��ݲ�ͬ��﷽ʽ֮���ת�� 
 * 1. ���� -> ���
 * 2. ��� -> �ַ���
 * 3. ��� -> ���� 
 */ 

std::vector<std::string> haha = {"˧����", "�ɰ���", "�¸ҵ�", "��������", "��ϰ���Ļ���", "��������", "������Ұ��", "ͨ�� 3.0 ��"};
// �ʵ� 

const std::string notice = "\n�ʶ���Ϸ����\n������Ϸ����\n������ʱ��\n���ܽ�������"
						   "\n��������ϵͳ����������Ϸ����������"; 
const std::string version = "(v3.0 stable)";
// С��ʿ�Ͱ汾�� 

twt_server cs;
/*
 * �������� 
 *
 * void sent(��ұ��, ��������) ��ͻ��˷�����Ϣ 
 * std::string rec(��ұ��) �ӿͻ��˽�����Ϣ 
 */ 
 
int B, gg, isPk, aa, sheId, hasSher;
/*
 * B ������� 
 * gg �Ƿ�������
 * isPk ͨ��ȫ�ֱ��������Ƿ���� pk���ҳ������ʵ��ȷʵ�е㲻�ã� 
 * aa �Ƿ��������� 
 * sheId ��¼������� 
 * hasSher ��¼�Ƿ����������� 
 */ 
 
twt all, wolves, now, prophets, hunters, witches, guards;
// ÿ����ɫ����������б�
 
int die[105], cha[105], lst = -1;
/*
 * die �Ƿ����� 
 * cha ������ 
 * lst ͨ��ȫ�ֱ������������ϴ��ص��� 
 */ 
 
twt goal(con.size());
// ÿ����ݵ�Ŀ������ 

std::set<std::pair<int, int>> tmpDie, tmpHnt;
// ���˶��к����˶���
// *�����ö�����Ϊ�˷��㴫�ݲ���������ֻ��һ����ҵ�Ҳ�� twt ��ͬ���ĵ��� 

unsigned seed;
// ������� 

void getChasGoal();										// ��ò�ͬ��ɫ����
void preWork();											// ����ʱ����
void atAdd();										    // ѯ�ʷ�����
bool in(int x, twt a);									// �ж� x �Ƿ��� a �� 
std::string to_string(twt x);							
void tele(twt a, std::string s);						// �㲥��һЩ����Ϣ
void distri();											// �����ɫ
std::string to_string(const std::map<int, int> &mp);	// ͶƱ�����ʽת��
void talk(twt x);										// һЩ�˷���
int doPd(int x);										// ��һ�����Ƕ���ȡ Yes/No
void askSheriff();										// ѯ���Ƿ���뾯�� 
void makeSher();										// ѡ�پ��� 

void ppPk(twt a);										// ƽƱ pk ����
void defPk(twt a);										// Ĭ�� pk ����
bool guCk(int x);										// ����ͶƱ�жϺ���
bool defCk(int x);										// Ĭ���жϺ���
twt dePro(twt &cnt);									// Ĭ��ͶƱ�ܽắ��
twt fstDayPro(twt &cnt);								// ��һ��ͶƱ�ܽắ��

twt _vote(twt a, std::map<int, int> &mp, dwd<bool(int)> ck, dwd<twt(twt&)> pro);
int vote(twt a, dwd<void(twt)> pk, dwd<bool(int)> ck, dwd<twt(twt&)> pro);
// ͶƱ������ִ��ͶƱ����
void _erase(twt &w, int x);
void erase(int x);
// ɾ���˺�ִ��ɾ���˺���
void _toDie(int x, int c, std::set<std::pair<int, int>> &tmp);
void toDie(int x, int c, int &flag, int &fg2); 
void apDie();
// ����������ִ��������Ӧ����������


void doHunter();
void doWolf();
void doProphet();
void doWitch(int no);
void doGuard();

// �������������ı�˳��ʹ����� 
void chaOrd();
void chaSher();

void night(int no);
void day();
bool check();

// �����򣬻��һЩ��Ϣ��ʼѭ��
int main() {
	system("chcp 936"); system("cls"); 
	seed = std::chrono::system_clock::now ().time_since_epoch ().count ();
	std::cout << version+"\n";
	getChasGoal(); 
	askSheriff();
	atAdd();
	preWork();
	for (int i = 1; ; i++) {
		night(i); if (check()) break;
		day(); if (check()) break;
	}
	if (aa) cs.sent(B+1, "ok");
	system("pause");
}
	
/*
 * ������ϵͳ
 * 1. ����ǵķ�������һ��� B+1��λ��
 * 2. ��ȡ��������Կ��
 * 3. У�鲢�㲥�Ƿ������Ϸ��
 */
void twt_server::antiAd(int noAna) {
	std::swap(s_accept[noAna], s_accept[B+1]);
	all.clear();
	for (int i = 1; i <= B; i++) all.push_back(i);
	std::string fcm = cs.rec(B+1);
	bool flag = 1;
	if (fcm != "6Q&okIlzyOb8DaIO@nuy") tele(all, notice);
	else flag = 0;
	if (flag) system("pause"), exit(0);
}

/*
 * �������
 * 1. У��汾��ͬ�������ӡ�
 * 2. ���������������������һ����������ҡ�
 * 3. �ȴ�������ʱ������Ҽ��뷵������������
 * 4. ������������㲥��Ҽ��������
 */
void twt_server::contect() {
	int noAna = 0;
	for (int i = 1; i <= B+aa; ) {
		s_accept[i] = accept(s_server, (SOCKADDR *)&accept_addr, &len);
		std::string cu = rec(i);
		if (aa && cu == "fcm") {
			if (!noAna) noAna = i, i++;
		}
		else if (aa && i > B && noAna == 0) sent(i, "�������������˳���"); 
		else if (cu != version) sent(i, "\n���İ汾�ͷ�������ͬ�����˳����������ӣ�");
		else { 
			all.push_back(i);
			system("cls");
			tele(all, to_string(i-!!noAna) + "/" + to_string(B));
			i ++;
		}
	}
	if (aa) antiAd(noAna); 
	tele(all, "over");
}

void askSheriff() {
	std::cout << "�Ƿ���뾯����\n> "; 
	int x;
	std::cin >> x;
	if (x) hasSher = 1;
}

// ��ò�ͬ��ɫ����
void getChasGoal() {
	for (int i = 0; i < con.size(); i++) std::cout << con[i] << ((i == con.size()-1) ?"": ", ");
	std::cout << "?\n> ";
	for (int i = 0; i < con.size(); i++) {
		std::cin >> goal[i];
		if (i > 1 && goal[i] != 0) gg = 1;
		B += goal[i];
	}
}

// �ȴ���Ҽ���
void preWork() {
	std::cout << "Waiting for players...\n";
	cs.init(1234, B);
	now = all;
	system("cls");
	std::cout << "Game is running!" + version + "\n";
	tele(all, to_string(B));
	for (int i = 1; i <= B; i++) cs.sent(i, to_string(i));
	distri();
	tele(wolves, to_string(wolves));
	if (hasSher) tele(all, "sif");
	else tele(all, "nonono");
}

// ѯ���Ƿ���������
void atAdd() {
	std::cout << "��Ҫ���������ϵͳ���Ƽ���������\n> ";
	std::cin >> aa;
	if (!aa) std::cout << "������ϵͳ�ѹرգ�\n"; 
} 

std::string to_string(twt x) {
	std::string an;
	for (int c : x) an = an + to_string(c) + " ";
	return an; 
}

void tele(twt a, std::string s) { for (int c : a) cs.sent(c, s); }

/*
 * �����ɫ
 * 1. ������ҽ�ɫ
 * 2. ���մ��Һ��˳�����η����ɫ��������Ӧ twt��
 */
void distri() {
	twt tmp(B);
	for (int i = 0; i < B; i++) tmp[i] = i+1;
	std::shuffle(tmp.begin(), tmp.end(), std::default_random_engine (seed));
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
}

// Ĭ���жϺ������ж��Ƿ�Ͷ�����ŵ����
bool defCk(int x) { 
	return x >= 0 && x <= B && !die[x];
}
// Ĭ��ͶƱ�ܽắ������ȡ����Ʊ�߲����ذ�������ƽƱ�� twt
twt dePro(twt &cnt) {
	twt ans;
	int mx = *std::max_element(cnt.begin()+1, cnt.end());
	for (int i = 1; i <= B; i++) 
		if (cnt[i] == mx) ans.push_back(i);
	return ans;
}

// ��һ��ͶƱ�ܽắ������ȫ����Ʊ���ؽ�����һ�� 0 �� twt
twt fstDayPro(twt &cnt) {
	twt ans;
	int mx = *std::max_element(cnt.begin()+1, cnt.end());
	for (int i = 1; i <= B; i++) 
		if (cnt[i] == mx) ans.push_back(i);
	if (mx != 0) return ans;
	else return twt(1, 0);
}

/*
 * ִ��ͶƱ
 * ���� a  ������ͶƱ�����
 * ���� mp ��ͶƱ����洢
 * ���� ck ���жϺ���
 * ���� pro������������
 *
 * ���ͶƱ���� ck �ж�ֱ���Ϸ����� pro ��������
 */
twt _vote(twt a, std::map<int, int> &mp, dwd<bool(int)> ck, dwd<twt(twt&)> pro) {
	twt cnt(all.size()+1, 0), ans;
	for (int c : a) {
		int r = to_int(cs.rec(c));
		while (!ck(r)) {
			cs.sent(c, "nopk");
			cs.sent(c, "���Ϸ���\n"); 
			cs.sent(c, "ͶƱ���Ϸ��������ԣ�"); 
			r = to_int(cs.rec(c));
		}
		cnt[r] ++, mp[c] = r;
		if (c == sheId) cnt[r] ++;
	}
	return pro(cnt);
}
std::string to_string(const std::map<int, int> &mp) {
	std::string an;
	for (auto c : mp) an += to_string(c.first) + " -> " + to_string(c.second) + '\n'; 
	return an;
}

/*
 *ƽƱ pk ����
 * 1. ��ƽƱ�߷��� you���������˷���ƽƱ״����
 * 2. ��÷�����Ϣ��
 * 3. �㲥�������ˡ�
 */
void ppPk(twt a) {
	std::vector<bool> fg(B+1, 0);
	for (int x : a) {
		fg[x] = 1;
		cs.sent(x, "you " + to_string(a.size()));
	}
	for (int i = 1; i <= B; i++) if (!fg[i]) cs.sent(i, "notyou " + to_string(a.size()));
	for (int x : a) {
		std::string c = cs.rec(x);
		tele(all, to_string(x) + " ˵��" + c);
	}
}

// Ĭ�� pk ���ǲ� pk
void defPk(twt a) {  }

/*
 * ͶƱ
 * ���� a  ������ͶƱ�����
 * ���� pk ��ƽƱ pk ����
 * ���� ck ���жϺ���
 * ���� pro������������
 *
 * 1. �õõ���������ִ��ͶƱֱ��û��ƽƱ��
 * 2. ͬʱ�㲥�Ƿ� pk ��ƽƱ��
 */
bool in(int x, twt a) {
	for (int c : a) if (x == c) return true;
	return false;
}
int vote(twt a, dwd<void(twt)> pk = defPk, dwd<bool(int)> ck = defCk, dwd<twt(twt&)> pro = dePro) {
	std::map<int, int> mp;
	twt cur = _vote(a, mp, ck, pro);
	while (cur.size() > 1) {
		if (isPk) tele(a, "pk"), tele(a, to_string(mp)), pk(cur);
		else tele(a, "nopk"), tele(a, to_string(mp));
		tele(a, std::string("������ЩƽƱ���������ͶƱ��������Ʊ���� ") + to_string(cur));
		mp.clear();
		cur = _vote(a, mp, [&ck, &cur](int x) { return ck(x) && x && in(x, cur); }, pro);
	}
	tele(a, "ok");
	tele(a, to_string(mp));
	return cur[0];
}

// ɾһ��
void _erase(twt &w, int x) {
	int k = -1;
	for (int i = 0; i < (int)w.size(); i++)
		if (w[i] == x) k = i;
	if (k != -1) w.erase(w.begin()+k);
}
// �ںü�������ɾһ��
void erase(int x) {
	_erase(wolves, x), _erase(now, x), _erase(guards, x),
	_erase(prophets, x), _erase(hunters, x), _erase(witches, x);
}

// �� c ɱ�� x ������������ tmp ��
void _toDie(int x, int c, std::set<std::pair<int, int>> &tmp = tmpDie) { 
	tmp.insert({x, c}); 
}

int ddd, ggg; // ռλ
// �㲥������Ϣ����ǣ����������� flag ��ǳ� 1
void toDie(int x, int c = -1, int &flag = ddd, int &fg2 = ggg) {
	if (x == 0) { tele(all, "0"); return; } // �����ɱ��� 
	die[x] = 1;
	std::string star, pre = "y";
	if (hunters.size() && x == hunters[0]) {
		star = "*";
		if (c == _Witch) pre = "#";
	}
	tele(all, pre + to_string(x) + star + " ����");
	if (c != _Witch && hunters.size() && x == hunters[0]) flag = 1;
	else erase(x); 
	if (x == sheId) fg2 = 1;
}

// Ӧ����������Ӧ�� tmpDie �е�������Ӧ���������������е�������û����㲥ƽ��ҹ
void apDie() {
	if (tmpDie.size() && tmpDie.begin() -> second == -1) tmpDie.clear(); 
	int kk = 0, gg = 0;
	for (auto c : tmpDie) {
		if (die[c.first]) continue;
		toDie(c.first, c.second, kk, gg), talk(twt(1, c.first));
		if (kk == 1) doHunter(), kk = 0;
		if (gg == 1) chaSher(), gg = 0;
	}
	if (tmpHnt.size() && !die[tmpHnt.begin()->first]) {
		toDie(tmpHnt.begin()->first, tmpHnt.begin()->second, kk, gg);
		if (tmpHnt.begin()->first) talk(twt(1, tmpHnt.begin()->first));
		tmpHnt.clear();
		if (gg == 1) chaSher(), gg = 0;
	}
	if (tmpDie.size()) tele(all, "over");
	else tele(all, "!��ƽ��ҹ�أ�");
	tmpDie.clear();
}

// ͶƱ������������������
void doHunter() {
	int k = vote(hunters, defPk, [](int x) { return defCk(x) && x != hunters[0]; }, fstDayPro);
	if (k != 0) _toDie(k, _Hunter, tmpHnt);
	else _toDie(0, _Hunter, tmpHnt); 
	erase(hunters[0]);
}

// ���ֵ��߷�����Ϣ�����䷢�Թ㲥��������
void talk(twt x) {
	for (int c : x) {
		cs.sent(c, "you");
		std::string r = cs.rec(c);
		tele(all, to_string(c) + " " + haha[rand()%haha.size()] + "˵ : " + r);
	}
}

// ִ����֮��ͶƱ������
void doWolf() {	_toDie(vote(wolves), _Wolf); }

// ��Ԥ�Լ��м�ͶƱ�����غû���Ϣ
void doProphet() {
	if (prophets.size() == 0) return;
	int x = vote(prophets);
	if (cha[x] == _Wolf) tele(prophets, "-");
	else tele(prophets, "+");
}

// һ���˵� Yes/No �ж�
int doPd(int x) { return to_int(cs.rec(x)); }

/*
 * Ů��
 * 1. ��֪������
 * 2. ��û��ʹ�ù���ҩ�Ҳ��Ƿ���ҹ�Լ�������ѯ���Ƿ�ʹ�ý�ҩ��
 * 3. û���ڵ�ǰʹ�ý�ҩ��û�ù���ҩѯ���Ƿ񶾡�
 * 4. ע��Ů�׾���ֻ���� tmpDie �м�����һ���� -1 �����ˡ�
 */
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
		int k = vote(witches, defPk, [](int x) { 
			return defCk(x) && x != tmpDie.begin()->first;
		}, [](twt &cnt) {
			return twt(1, std::max_element(cnt.begin(), cnt.end())-cnt.begin()); 
		});
		if (k != 0) tmpDie.insert({k, _Witch}), poi = 1;
	}
	be = 0;
}

// ����ͶƱ�жϺ���������������ͬһ��
bool guCk(int x) { return defCk(x) && x != lst; }
/*
 * ����
 * 1. ͶƱ�ҳ�Ҫ�ص��ˡ�
 * 2. ���ȳɹ���ɾ�����׵�һ����
 * 3. 2 ����ʵ���ˡ�Ů������ͬʱ�Ȼ��������Ĺ��ܡ�
 */
void doGuard() {
	if (!guards.size()) return;
	int k = vote(guards, defPk, guCk, fstDayPro);
	if (k == 0) return;
	lst = k;
	auto it = tmpDie.lower_bound({k, -100});
	if (it == tmpDie.end() || it->first != k) return;
	std::cout << "\nGuard saved " << tmpDie.begin()->first << '\n';
	tmpDie.erase(tmpDie.begin()); 
}

void chaSher() {
	std::cout << sheId << "dwd\n";
	if (!sheId) return;
	sheId = vote(twt(1, sheId));
	std::cout << sheId << "twt\n";
	if (sheId) tele(all, to_string(sheId) + " ��Ϊ����һ��������"); 
	else tele(all, "0 �ٺ٣��������ǲ������գ�"); 
} 

// ѯ�ʾ����Ƿ�һ������˳�� 
void chaOrd() {
	if (sheId == 0) return;
	if (doPd(sheId)) std::reverse(all.begin(), all.end());
}

// ��ÿ��ѯ���Ƿ��ѡ����������ͶƱѡ��������
// ͶƱ Ck ����ΪĬ���жϼ��ϱ����ڲ�ѡ��� 
void makeSher() {
	twt elec;
	for (int i : all) if (doPd(i)) elec.push_back(i);
	if (elec.size()) tele(all, to_string(elec));
	else  { tele(all, "nonono"); return; }
	talk(elec);
	for (int i : all) if (!in(i, elec)) tele(all, to_string(i) + " û�в�ѡ������"); 
	sheId = vote(all, defPk, [&elec](int x) { return defCk(x) && in(x, elec); });
	if (sheId) tele(all, to_string(sheId) + " ��ѡ�����ɹ���"); 
	else tele(all, "0 û�о����أ�"); 
} 
void night(int no) { 
	tele(all, "night");
	doProphet();
	doWolf(); 
	doWitch(no == 1);
	doGuard();
	if (no == 1 && hasSher) makeSher();
	apDie();
	chaOrd();
}
void day() {
	static bool fg = 1;
	tele(all, "day");
	talk(now);
	isPk = 1;
	int tmp = vote(now, ppPk, defCk, fg ? fstDayPro : dePro);
	if (tmp) _toDie(tmp, _Villager);
	else tele(all, "!û������!"); 
	isPk = 0;
	if (tmp) apDie();
	fg = 0;
}
bool check() {
	int cnt = 0, cnt2 = 0; 
	for (int i = 1; i <= B; i++) 
		cnt += (cha[i] == _Villager && !die[i]),
		cnt2 += ((cha[i] == _Prophet || cha[i] == _Hunter || 
				  cha[i] == _Witch || cha[i] == _Guard) && !die[i]); 
	if (!cnt || (gg && !cnt2)) return tele(all, std::string("&��Ӯ��!")), 1;
	cnt = 0;
	for (int i = 1; i <= B; i++) cnt += (cha[i] == _Wolf && !die[i]);
	if (!cnt) return tele(all, std::string("&��Ӯ��!")), 1;
	return 0;
}
