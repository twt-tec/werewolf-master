#include "communi.h"
#include <iostream>
#include <windows.h>
#include <vector>
#include <string>
constexpr int _Villager = 0, _Wolf = 1, _Prophet = 2, _Hunter = 3, _Witch = 4, _Guard = 5;
std::vector<std::string> con = {"村民", "狼", "预言家", "猎人", "女巫", "守卫"}; 
// 角色列表，con 中的下标需和对应变量相匹配。
const std::string version = "(v3.0 stable)";
twt_client cs;
/*
 * 局域网信息发送类
 * 负责和服务器间信息的收发
 *
 * 参考：communi.h
 * 异常：发送、接收失败和发送长度超过限制时
 * 
 * rec() 返回接收到的信息
 * sent(std::string) 向服务器发送信息
 */
int id, B, cha, flag, sher, hasShe;
/* 
 * id 自身编号
 * B 总人数
 * cha 自身角色
 * flag 是否死亡
 * sher 警长
 * hasShe 有警长吗 
 */
std::vector<int> all;
// 存活的玩家。

void watch(); 									// 观战模式
void erase(int x);								// 删除玩家 x
std::string to_string(std::vector<int> x);		// std::vector<int> -> int
std::string getMes();							// 获得玩家输入信息
void pk();										// 进行平票 pk
void pntRes();									// 输出投票结果
int vote(std::string s);						// 进行一次投票
void _getDie(std::string c);					// 执行玩家的死亡
void print(std::string s);						// 输出当前状态
void talk(int num);								// 玩家发言，参数是发言人数，默认是现存人数+1（因为自己要发言） 
void getDie();									// 获得死亡玩家
void getCha();									// 获得角色
void preWork();									// 游戏前操作
void getInfo();									// 获得游戏所需信息
void eleShe();									// 选举警长 
void updOrd();									// 警长改发言顺序
void updSher();									// 警长死后改警长 

void wolf();									// 执行狼操作
void prophet();									// 预言家
void witch(int no);								// 女巫
void guard();									// 守卫
void doHunter(bool op, bool wH);				// 猎人，特判同时是警长的情况 

bool check();									// 检测游戏是否结束
void day();										// 白天
void night(int x);								// 夜晚

// 主程序连接服务器并获得所需信息，然后开始游戏的循环。
int main() {
	system("chcp 936"); system("cls");
	std::cout << "欢迎使用谭炜谭狼人杀大师" + version + "!\n";
	cs.init(1234);
	preWork();
	getInfo();
	while (1) {
		static int o = 1;
		if (check()) break; !flag ? night(o) : watch();
		if (check()) break; !flag ? day() : watch();
		o = 0;
	}
	system("pause");
}

// 观战模式接受所有的信息，剔除系统消息后显示在屏幕上
void watch() {
	system("cls");
	std::cout << "你已经死了！\n现在是观战模式！\n"; 
	std::string sc = cs.rec();
	while (1) {
		if (sc != "day" && sc != "night" && (sc[0] > 'A' && sc[0] < 'z')) { 
			sc = cs.rec(); 
			continue; 
		}
		if (sc == "day") std::cout << "白天到了\n";
		else if (sc == "night") std::cout << "晚上到了\n";
		else if (sc[0] == '&') std::cout << sc.substr(1, sc.size()-1) << '\n';
		else std::cout << sc << '\n';
		if (sc[0] == '&') { system("pause"); exit(0); }
		sc = cs.rec();		
	}
}

// 在所有玩家中删除 x
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

// 从终端获得玩家输入消息并确保长度正确
std::string getMes() {
	std::string an;
	fflush(stdin), std::getline(std::cin, an);
	while (an.size() < 1 || an.size() > 500) {
		std::cout << "发言长度不合法，请重试！\n> "; 
		fflush(stdin), std::getline(std::cin, an);
	}
	return an;
}


// 获得投票信息 
int getVote() {
	fflush(stdin);
	std::string s;
	std::cin >> s;
	while ([&s]() {
		for (auto c : s) 
			if (c < '0' || c > '9') return true;
		return false;
	}()) {
		std::cout << "这输入的什么玩意儿，再来！\n> ";
		std::cin >> s; 
	}
	return to_int(s);
}

/*
 * 进行平票 pk
 * 1. 接收来自服务端的信息，格式为 是否 pk + pk 总人数。
 * 2. 若 pk 则获得玩家发言返回给服务器。
 * 3. 随后接收所有平票发言信息。
 */
void pk() {
	std::string k = cs.rec();
	auto spilt = [](std::string x) {
		int p = x.find(' ');
		return to_int(x.substr(p+1, x.size()-p-1));
	};
	int l = spilt(k);
	if (k[0] == 'y') {
		std::cout << "你平票了，快发言！\n> ";
		k = getMes();
		cs.sent(k); 
	} 
	else std::cout << "等待平票发言...\n"; 
	for (int i = 1; i <= l; i++)
		k = cs.rec(), std::cout << k << '\n';
//	cs.rec();
}

// 输出投票结果，结果格式由服务器提供
void pntRes() {
	std::string res = cs.rec();
	std::cout << "本次投票：\n";
	std::cout << res; 
}

/*
 * 投票
 * 1. 传入参数用于显示提示信息。
 * 2. 获取信息判断是否 pk 直到服务端返回 ok。
 * 3. 若投票经判断不合法服务器会返回不合法信息（即不是 ok）,此时重新投票。
 * 4. 每次输出投票结果，最后返回投出的人。
 */
int vote(std::string s) {
	if (flag) return 0;
	std::cout << "输入你要" << s << "的人\n> ";
	int x = getVote();
	int lst = 0;
	lst = x;
	cs.sent(to_string(x));
	std::string c = cs.rec();
	while (c != "ok") {
		pntRes();
		if (c == "pk") pk();
		c = cs.rec();
		std::cout << c << "\n> ";
		x = getVote();
		cs.sent(to_string(x));
		lst = x;
		c = cs.rec();
	}
	pntRes();
	return lst;
}

// 猎人需要投票选出带走的
void doHunter(bool op = 0, bool wH = 0) {
	if (!wH) vote("带走");
	if (op) updSher();
	watch();
}

void updSher() {
	static bool upfg = 0;
	static int cnt = 0;
	if (upfg) return;
	if (id == sher) vote("交出警徽给"), upfg = 1;
	else {
		cnt ++;
		if (cnt & 1) return;
		std::string c = cs.rec();
		int p = c.find(' ');
		sher = to_int(c.substr(0, p));
		if (sher) std::cout << c << '\n';
		else std::cout << c.substr(2, c.size()-2) << '\n';
	}
}

/* 
 * 执行玩家的死亡
 * 1. 获得死亡玩家，格式为提示信息。
 * 2. 若自己死亡则发表遗言并标记。
 * 3. 若自己是猎人执行猎人。
 * 4. 若死亡是猎人（收到服务器信息）则获取下一个死亡。
 * 
 * 注意遗言功能的巧妙实现！
 * 因为 erase 一个已经死去的玩家是没有关系的
 * 所以这里将遗言也作为死亡参数传了进来
 * 猎人判定则是传进来的编号后面带了 * 
 */
void _getDie(std::string c) {
	if (flag) return;
	if (c == "0") return; // 用于处理猎人空杀的情况 
	int x, wH = 0;
	if (c[0] == '#') wH = 1;
	// wH 用来记录是否是女巫杀的 
	if (c[0] == 'y' || c[0] == '#') c = c.substr(1, (int)c.size()-1); 
	std::cout << c << '\n';
	int p = c.find(' ');
	static int hnt = 0, wtyy = 0;
	if (c[p-1] == '*') x = to_int(c.substr(0, p-1)), hnt = x;
	else x = to_int(c.substr(0, p)); 
	erase(x);
	if (x == id) {
		cs.rec();
		std::cout << "你死了!\n发表遗言！\n> ";
		std::string yy;
		yy = getMes();
		cs.sent(yy);
		cs.rec();
		wtyy = 1;
	}
	if (cha == _Hunter && x == id) doHunter(x == sher, wH);
	if (x == sher) updSher();
	if (x == id) flag = 1;
	if (x == hnt) _getDie(cs.rec());
	wtyy = 0;
}

// 打印信息，传入的是时间
void print(std::string s) {
	system("cls");
	std::cout << "时间 : " << s << '\n';
	std::cout << "你的编号是 " << id << "!\n";
	std::cout << "还活着的玩家 : " << to_string(all) << '\n';
	if (flag) std::cout << "你已经死了!\n";
	std::cout << "等待其它玩家...\n";
}

// 收到服务器指令 you 时发言否则接收它人的发言
void talk(int num = 0) {
	int l = all.size() - flag;
	if (num) l = num-1;
	for (int i = 0; i <= l; i++) {
		std::string c = cs.rec();
		if (c == "you") {
			std::cout << "输入你的发言:\n> ";
			std::string an;
            an = getMes();
			cs.sent(an);
		}
		else std::cout << c << '\n';
	}
}

// 若收到的不是平安夜死亡队列结束指令 over 则一直接收并执行玩家死亡
void getDie() {
	if (flag) return;
	std::string an = cs.rec();
	while (an[0] != '!' && an[0] != 'o') _getDie(an), an = cs.rec();
	if (an[0] == '!') std::cout << an << '\n';
	system("pause");
}

// 获得身份，是狼则通知队友
void getCha() {
	std::string c = cs.rec();
	cha = to_int(c);
	std::cout << "你的身份是 " << con[cha] << '\n';
	if (cha == _Wolf) c = cs.rec(), std::cout << "所有的狼是（你的队友） " + c + '\n'; 
}

// 狼投票杀死人
void wolf() {
	if (cha != _Wolf || flag) return;
	vote("杀死");
}

// 预言家投票检验并从服务器获得结果（+/-）
void prophet() {
	if (cha != _Prophet || flag) return;
	vote("检验");
	std::string an = cs.rec();
	if (an == "+") std::cout << "是个好人!\n";
	else std::cout << "是狼！!\n";
}

/*
 * 女巫
 * 1. 告知死亡。
 * 2. 若没有使用过解药且不是非守夜自己被刀则询问是否使用解药。
 * 3. 没有在当前使用解药且没用过毒药询问是否毒。
 */
void witch(int no) {
	if (cha != _Witch || flag) return; 
	static int jy = 0, dy = 0, be = 0;
	std::string c = cs.rec();
	std::cout << c << " 死了！\n";
	if (jy && dy) system("pause");
	if (jy == 0 && !(no != 1 && to_int(c) == id)) {
		std::cout << "是否使用解药？(0 否 1 是)\n> ";
		int x = getVote();
		cs.sent(to_string(x));
		if (x) be = 1, jy = 1;
	} 
	if (dy == 0 && !be) {
		if (vote("毒") != 0) dy = 1;
	}
	be = 0;
} 

// 投票选出保卫的人
void guard() {
	if (cha != _Guard || flag) return;
	vote("保卫");
}

// 若收到服务器 & 打头信息则结束，输出输赢判定
bool check() {
	std::string s = cs.rec();
	if (s[0] != '&') return false;
	std::cout << s.substr(1, s.size()-1) << '\n';
	return true;
}

void eleShe() {
	std::cout << "是否要竞选警长（1/0）？ \n> ";
	int x = getVote();
	cs.sent(to_string(x));
	std::string c = cs.rec();
	if (c == "nonono") {
		std::cout << "根本没人参选！";
		return;
	}
	else std::cout << c << "参选警长。等待发言...\n"; 
	talk(all.size()+!!x); vote("选为警长");
    c = cs.rec();
	int p = c.find(' ');
	sher = to_int(c.substr(0, p));
	if (sher) std::cout << c << '\n';
	else std::cout << c.substr(p+1, c.size()-p-1) << '\n';
} 
void updOrd() {
	if (id != sher || flag) return;
	std::cout << "反转发言顺序吗？\n> ";
	int x = getVote();
	cs.sent(to_string(x)); 
}

// 晚上依次执行玩家操作
void night(int x) {
	print("晚上");
	wolf();
	prophet();
	witch(x);
	guard();
	if (x == 1 && hasShe) eleShe(); 
	getDie();
	updOrd();
}

// 白天发言并投票处死人
void day() {
	print("白天");
	talk();
	vote("投票处死");
	getDie();
}

// 在等待时发送版本号用于验证
// 然后接收服务器的玩家人数直到所有人连上
void preWork() {
	cs.sent(version);
	std::string per = cs.rec();
	while (per != "over") {
		system("cls");
		std::cout << version + "等待其它玩家... "; 
		std::cout << per << '\n';
		per = cs.rec();
	}
}

// 获得玩家总数和自身编号
void getInfo() {
	std::string c = cs.rec();
	system("cls");
	std::cout << "一共有 " << c << " 个玩家\n";
	B = to_int(c);
	for (int i = 1; i <= B; i++) all.push_back(i);
	c = cs.rec();
	id = to_int(c);
	std::cout << "你的编号是 " << id << "!\n";
	getCha();
	c = cs.rec();
	if (c == "sif") hasShe = 1;
	system("pause");
}
