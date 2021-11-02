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
std::vector<std::string> con = {"村民", "狼", "预言家", "猎人", "女巫", "守卫"};
twt chas 				     = {_Villager, _Wolf, _Prophet, _Hunter, _Witch, _Guard};
/* 
 * 身份不同表达方式之间的转换 
 * 1. 字面 -> 编号
 * 2. 编号 -> 字符串
 * 3. 编号 -> 字面 
 */ 

std::vector<std::string> haha = {"帅气地", "可爱地", "勇敢地", "拍了拍手", "复习完文化课", "拍了拍腿", "拍了拍野方", "通过 3.0 版"};
// 彩蛋 

const std::string notice = "\n适度游戏益脑\n沉迷游戏伤身\n合理安排时间\n享受健康生活"
						   "\n【防沉迷系统】您今日游戏次数已满！"; 
const std::string version = "(v3.0 stable)";
// 小贴士和版本号 

twt_server cs;
/*
 * 服务器类 
 *
 * void sent(玩家编号, 发送内容) 向客户端发送信息 
 * std::string rec(玩家编号) 从客户端接收信息 
 */ 
 
int B, gg, isPk, aa, sheId, hasSher;
/*
 * B 玩家总数 
 * gg 是否有神仙
 * isPk 通过全局变量传递是否进行 pk（我承认这个实现确实有点不好） 
 * aa 是否接入防沉迷 
 * sheId 记录警长编号 
 * hasSher 记录是否开启警长功能 
 */ 
 
twt all, wolves, now, prophets, hunters, witches, guards;
// 每个角色包含的玩家列表
 
int die[105], cha[105], lst = -1;
/*
 * die 是否死亡 
 * cha 玩家身份 
 * lst 通过全局变量传递守卫上次守的人 
 */ 
 
twt goal(con.size());
// 每种身份的目标人数 

std::set<std::pair<int, int>> tmpDie, tmpHnt;
// 死人队列和猎人队列
// *猎人用队列是为了方便传递参数，上面只有一个玩家的也用 twt 是同样的道理 

unsigned seed;
// 随机种子 

void getChasGoal();										// 获得不同角色数量
void preWork();											// 连接时工作
void atAdd();										    // 询问防沉迷
bool in(int x, twt a);									// 判断 x 是否在 a 中 
std::string to_string(twt x);							
void tele(twt a, std::string s);						// 广播给一些人信息
void distri();											// 分配角色
std::string to_string(const std::map<int, int> &mp);	// 投票结果格式转换
void talk(twt x);										// 一些人发言
int doPd(int x);										// 从一个人那儿获取 Yes/No
void askSheriff();										// 询问是否加入警长 
void makeSher();										// 选举警长 

void ppPk(twt a);										// 平票 pk 函数
void defPk(twt a);										// 默认 pk 函数
bool guCk(int x);										// 守卫投票判断函数
bool defCk(int x);										// 默认判断函数
twt dePro(twt &cnt);									// 默认投票总结函数
twt fstDayPro(twt &cnt);								// 第一天投票总结函数

twt _vote(twt a, std::map<int, int> &mp, dwd<bool(int)> ck, dwd<twt(twt&)> pro);
int vote(twt a, dwd<void(twt)> pk, dwd<bool(int)> ck, dwd<twt(twt&)> pro);
// 投票函数和执行投票函数
void _erase(twt &w, int x);
void erase(int x);
// 删除人和执行删除人函数
void _toDie(int x, int c, std::set<std::pair<int, int>> &tmp);
void toDie(int x, int c, int &flag, int &fg2); 
void apDie();
// 加入死亡、执行死亡和应用死亡函数


void doHunter();
void doWolf();
void doProphet();
void doWitch(int no);
void doGuard();

// 俩警长操作：改变顺序和传警徽 
void chaOrd();
void chaSher();

void night(int no);
void day();
bool check();

// 主程序，获得一些信息后开始循环
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
 * 防沉迷系统
 * 1. 将标记的防沉迷玩家换到 B+1号位。
 * 2. 获取防沉迷秘钥。
 * 3. 校验并广播是否进行游戏。
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
 * 连接玩家
 * 1. 校验版本不同重新连接。
 * 2. 若开启防沉迷则需多连接一个防沉迷玩家。
 * 3. 等待防沉迷时若有玩家加入返回人数已满。
 * 4. 若正常加入则广播玩家加入情况。
 */
void twt_server::contect() {
	int noAna = 0;
	for (int i = 1; i <= B+aa; ) {
		s_accept[i] = accept(s_server, (SOCKADDR *)&accept_addr, &len);
		std::string cu = rec(i);
		if (aa && cu == "fcm") {
			if (!noAna) noAna = i, i++;
		}
		else if (aa && i > B && noAna == 0) sent(i, "人数已满！请退出！"); 
		else if (cu != version) sent(i, "\n您的版本和服务器不同，请退出后重新连接！");
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
	std::cout << "是否加入警长？\n> "; 
	int x;
	std::cin >> x;
	if (x) hasSher = 1;
}

// 获得不同角色数量
void getChasGoal() {
	for (int i = 0; i < con.size(); i++) std::cout << con[i] << ((i == con.size()-1) ?"": ", ");
	std::cout << "?\n> ";
	for (int i = 0; i < con.size(); i++) {
		std::cin >> goal[i];
		if (i > 1 && goal[i] != 0) gg = 1;
		B += goal[i];
	}
}

// 等待玩家加入
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

// 询问是否开启防沉迷
void atAdd() {
	std::cout << "您要接入防沉迷系统吗（推荐开启）？\n> ";
	std::cin >> aa;
	if (!aa) std::cout << "防沉迷系统已关闭！\n"; 
} 

std::string to_string(twt x) {
	std::string an;
	for (int c : x) an = an + to_string(c) + " ";
	return an; 
}

void tele(twt a, std::string s) { for (int c : a) cs.sent(c, s); }

/*
 * 分配角色
 * 1. 随机打乱角色
 * 2. 按照打乱后的顺序依次分配角色并加入相应 twt中
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

// 默认判断函数，判断是否投给活着的玩家
bool defCk(int x) { 
	return x >= 0 && x <= B && !die[x];
}
// 默认投票总结函数：获取最大得票者并返回包含所有平票的 twt
twt dePro(twt &cnt) {
	twt ans;
	int mx = *std::max_element(cnt.begin()+1, cnt.end());
	for (int i = 1; i <= B; i++) 
		if (cnt[i] == mx) ans.push_back(i);
	return ans;
}

// 第一天投票总结函数：若全部弃票返回仅包含一个 0 的 twt
twt fstDayPro(twt &cnt) {
	twt ans;
	int mx = *std::max_element(cnt.begin()+1, cnt.end());
	for (int i = 1; i <= B; i++) 
		if (cnt[i] == mx) ans.push_back(i);
	if (mx != 0) return ans;
	else return twt(1, 0);
}

/*
 * 执行投票
 * 参数 a  ：参与投票的玩家
 * 参数 mp ：投票结果存储
 * 参数 ck ：判断函数
 * 参数 pro：处理结果函数
 *
 * 获得投票，用 ck 判断直到合法后用 pro 处理并返回
 */
twt _vote(twt a, std::map<int, int> &mp, dwd<bool(int)> ck, dwd<twt(twt&)> pro) {
	twt cnt(all.size()+1, 0), ans;
	for (int c : a) {
		int r = to_int(cs.rec(c));
		while (!ck(r)) {
			cs.sent(c, "nopk");
			cs.sent(c, "不合法！\n"); 
			cs.sent(c, "投票不合法，请重试！"); 
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
 *平票 pk 函数
 * 1. 向平票者发送 you，向其它人发送平票状况。
 * 2. 获得发言信息。
 * 3. 广播给所有人。
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
		tele(all, to_string(x) + " 说：" + c);
	}
}

// 默认 pk 就是不 pk
void defPk(twt a) {  }

/*
 * 投票
 * 参数 a  ：参与投票的玩家
 * 参数 pk ：平票 pk 函数
 * 参数 ck ：判断函数
 * 参数 pro：处理结果函数
 *
 * 1. 用得到参数不断执行投票直到没有平票。
 * 2. 同时广播是否 pk 及平票。
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
		tele(a, std::string("请在这些平票玩家中重新投票（不能弃票）： ") + to_string(cur));
		mp.clear();
		cur = _vote(a, mp, [&ck, &cur](int x) { return ck(x) && x && in(x, cur); }, pro);
	}
	tele(a, "ok");
	tele(a, to_string(mp));
	return cur[0];
}

// 删一个
void _erase(twt &w, int x) {
	int k = -1;
	for (int i = 0; i < (int)w.size(); i++)
		if (w[i] == x) k = i;
	if (k != -1) w.erase(w.begin()+k);
}
// 在好几个里面删一个
void erase(int x) {
	_erase(wolves, x), _erase(now, x), _erase(guards, x),
	_erase(prophets, x), _erase(hunters, x), _erase(witches, x);
}

// 将 c 杀了 x 放入死亡队列 tmp 中
void _toDie(int x, int c, std::set<std::pair<int, int>> &tmp = tmpDie) { 
	tmp.insert({x, c}); 
}

int ddd, ggg; // 占位
// 广播死亡信息并标记，若是猎人则将 flag 标记成 1
void toDie(int x, int c = -1, int &flag = ddd, int &fg2 = ggg) {
	if (x == 0) { tele(all, "0"); return; } // 处理空杀情况 
	die[x] = 1;
	std::string star, pre = "y";
	if (hunters.size() && x == hunters[0]) {
		star = "*";
		if (c == _Witch) pre = "#";
	}
	tele(all, pre + to_string(x) + star + " 死了");
	if (c != _Witch && hunters.size() && x == hunters[0]) flag = 1;
	else erase(x); 
	if (x == sheId) fg2 = 1;
}

// 应用死亡，先应用 tmpDie 中的死亡再应用猎人死亡队列中的死亡，没人则广播平安夜
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
	else tele(all, "!是平安夜呢！");
	tmpDie.clear();
}

// 投票并塞入猎人死亡队列
void doHunter() {
	int k = vote(hunters, defPk, [](int x) { return defCk(x) && x != hunters[0]; }, fstDayPro);
	if (k != 0) _toDie(k, _Hunter, tmpHnt);
	else _toDie(0, _Hunter, tmpHnt); 
	erase(hunters[0]);
}

// 给轮到者发送信息并将其发言广播给所有人
void talk(twt x) {
	for (int c : x) {
		cs.sent(c, "you");
		std::string r = cs.rec(c);
		tele(all, to_string(c) + " " + haha[rand()%haha.size()] + "说 : " + r);
	}
}

// 执行狼之间投票的死亡
void doWolf() {	_toDie(vote(wolves), _Wolf); }

// 在预言家中间投票并传回好坏信息
void doProphet() {
	if (prophets.size() == 0) return;
	int x = vote(prophets);
	if (cha[x] == _Wolf) tele(prophets, "-");
	else tele(prophets, "+");
}

// 一个人的 Yes/No 判断
int doPd(int x) { return to_int(cs.rec(x)); }

/*
 * 女巫
 * 1. 告知死亡。
 * 2. 若没有使用过解药且不是非守夜自己被刀则询问是否使用解药。
 * 3. 没有在当前使用解药且没用过毒药询问是否毒。
 * 4. 注意女巫救人只是在 tmpDie 中间塞入一个由 -1 毒的人。
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

// 守卫投票判断函数：不能两次守同一个
bool guCk(int x) { return defCk(x) && x != lst; }
/*
 * 守卫
 * 1. 投票找出要守的人。
 * 2. 若救成功，删除队首的一个。
 * 3. 2 巧妙实现了“女巫守卫同时救还是死”的功能。
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
	if (sheId) tele(all, to_string(sheId) + " 成为了下一代警长！"); 
	else tele(all, "0 嘿嘿，警长就是不传警徽！"); 
} 

// 询问警长是否反一反发言顺序。 
void chaOrd() {
	if (sheId == 0) return;
	if (doPd(sheId)) std::reverse(all.begin(), all.end());
}

// 向每人询问是否参选警长并进行投票选出警长。
// 投票 Ck 参数为默认判断加上必须在参选者里。 
void makeSher() {
	twt elec;
	for (int i : all) if (doPd(i)) elec.push_back(i);
	if (elec.size()) tele(all, to_string(elec));
	else  { tele(all, "nonono"); return; }
	talk(elec);
	for (int i : all) if (!in(i, elec)) tele(all, to_string(i) + " 没有参选警长。"); 
	sheId = vote(all, defPk, [&elec](int x) { return defCk(x) && in(x, elec); });
	if (sheId) tele(all, to_string(sheId) + " 竞选警长成功！"); 
	else tele(all, "0 没有警长呢！"); 
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
	else tele(all, "!没人死呢!"); 
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
	if (!cnt || (gg && !cnt2)) return tele(all, std::string("&狼赢了!")), 1;
	cnt = 0;
	for (int i = 1; i <= B; i++) cnt += (cha[i] == _Wolf && !die[i]);
	if (!cnt) return tele(all, std::string("&民赢了!")), 1;
	return 0;
}
