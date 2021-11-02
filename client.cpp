#include "communi.h"
#include <iostream>
#include <windows.h>
#include <vector>
#include <string>
constexpr int _Villager = 0, _Wolf = 1, _Prophet = 2, _Hunter = 3, _Witch = 4, _Guard = 5;
std::vector<std::string> con = {"����", "��", "Ԥ�Լ�", "����", "Ů��", "����"}; 
// ��ɫ�б�con �е��±���Ͷ�Ӧ������ƥ�䡣
const std::string version = "(v3.0 stable)";
twt_client cs;
/*
 * ��������Ϣ������
 * ����ͷ���������Ϣ���շ�
 *
 * �ο���communi.h
 * �쳣�����͡�����ʧ�ܺͷ��ͳ��ȳ�������ʱ
 * 
 * rec() ���ؽ��յ�����Ϣ
 * sent(std::string) �������������Ϣ
 */
int id, B, cha, flag, sher, hasShe;
/* 
 * id ������
 * B ������
 * cha �����ɫ
 * flag �Ƿ�����
 * sher ����
 * hasShe �о����� 
 */
std::vector<int> all;
// ������ҡ�

void watch(); 									// ��սģʽ
void erase(int x);								// ɾ����� x
std::string to_string(std::vector<int> x);		// std::vector<int> -> int
std::string getMes();							// ������������Ϣ
void pk();										// ����ƽƱ pk
void pntRes();									// ���ͶƱ���
int vote(std::string s);						// ����һ��ͶƱ
void _getDie(std::string c);					// ִ����ҵ�����
void print(std::string s);						// �����ǰ״̬
void talk(int num);								// ��ҷ��ԣ������Ƿ���������Ĭ�����ִ�����+1����Ϊ�Լ�Ҫ���ԣ� 
void getDie();									// ����������
void getCha();									// ��ý�ɫ
void preWork();									// ��Ϸǰ����
void getInfo();									// �����Ϸ������Ϣ
void eleShe();									// ѡ�پ��� 
void updOrd();									// �����ķ���˳��
void updSher();									// ��������ľ��� 

void wolf();									// ִ���ǲ���
void prophet();									// Ԥ�Լ�
void witch(int no);								// Ů��
void guard();									// ����
void doHunter(bool op, bool wH);				// ���ˣ�����ͬʱ�Ǿ�������� 

bool check();									// �����Ϸ�Ƿ����
void day();										// ����
void night(int x);								// ҹ��

// ���������ӷ����������������Ϣ��Ȼ��ʼ��Ϸ��ѭ����
int main() {
	system("chcp 936"); system("cls");
	std::cout << "��ӭʹ��̷�̷����ɱ��ʦ" + version + "!\n";
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

// ��սģʽ�������е���Ϣ���޳�ϵͳ��Ϣ����ʾ����Ļ��
void watch() {
	system("cls");
	std::cout << "���Ѿ����ˣ�\n�����ǹ�սģʽ��\n"; 
	std::string sc = cs.rec();
	while (1) {
		if (sc != "day" && sc != "night" && (sc[0] > 'A' && sc[0] < 'z')) { 
			sc = cs.rec(); 
			continue; 
		}
		if (sc == "day") std::cout << "���쵽��\n";
		else if (sc == "night") std::cout << "���ϵ���\n";
		else if (sc[0] == '&') std::cout << sc.substr(1, sc.size()-1) << '\n';
		else std::cout << sc << '\n';
		if (sc[0] == '&') { system("pause"); exit(0); }
		sc = cs.rec();		
	}
}

// �����������ɾ�� x
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

// ���ն˻�����������Ϣ��ȷ��������ȷ
std::string getMes() {
	std::string an;
	fflush(stdin), std::getline(std::cin, an);
	while (an.size() < 1 || an.size() > 500) {
		std::cout << "���Գ��Ȳ��Ϸ��������ԣ�\n> "; 
		fflush(stdin), std::getline(std::cin, an);
	}
	return an;
}


// ���ͶƱ��Ϣ 
int getVote() {
	fflush(stdin);
	std::string s;
	std::cin >> s;
	while ([&s]() {
		for (auto c : s) 
			if (c < '0' || c > '9') return true;
		return false;
	}()) {
		std::cout << "�������ʲô�������������\n> ";
		std::cin >> s; 
	}
	return to_int(s);
}

/*
 * ����ƽƱ pk
 * 1. �������Է���˵���Ϣ����ʽΪ �Ƿ� pk + pk ��������
 * 2. �� pk ������ҷ��Է��ظ���������
 * 3. ����������ƽƱ������Ϣ��
 */
void pk() {
	std::string k = cs.rec();
	auto spilt = [](std::string x) {
		int p = x.find(' ');
		return to_int(x.substr(p+1, x.size()-p-1));
	};
	int l = spilt(k);
	if (k[0] == 'y') {
		std::cout << "��ƽƱ�ˣ��췢�ԣ�\n> ";
		k = getMes();
		cs.sent(k); 
	} 
	else std::cout << "�ȴ�ƽƱ����...\n"; 
	for (int i = 1; i <= l; i++)
		k = cs.rec(), std::cout << k << '\n';
//	cs.rec();
}

// ���ͶƱ����������ʽ�ɷ������ṩ
void pntRes() {
	std::string res = cs.rec();
	std::cout << "����ͶƱ��\n";
	std::cout << res; 
}

/*
 * ͶƱ
 * 1. �������������ʾ��ʾ��Ϣ��
 * 2. ��ȡ��Ϣ�ж��Ƿ� pk ֱ������˷��� ok��
 * 3. ��ͶƱ���жϲ��Ϸ��������᷵�ز��Ϸ���Ϣ�������� ok��,��ʱ����ͶƱ��
 * 4. ÿ�����ͶƱ�������󷵻�Ͷ�����ˡ�
 */
int vote(std::string s) {
	if (flag) return 0;
	std::cout << "������Ҫ" << s << "����\n> ";
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

// ������ҪͶƱѡ�����ߵ�
void doHunter(bool op = 0, bool wH = 0) {
	if (!wH) vote("����");
	if (op) updSher();
	watch();
}

void updSher() {
	static bool upfg = 0;
	static int cnt = 0;
	if (upfg) return;
	if (id == sher) vote("�������ո�"), upfg = 1;
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
 * ִ����ҵ�����
 * 1. ���������ң���ʽΪ��ʾ��Ϣ��
 * 2. ���Լ������򷢱����Բ���ǡ�
 * 3. ���Լ�������ִ�����ˡ�
 * 4. �����������ˣ��յ���������Ϣ�����ȡ��һ��������
 * 
 * ע�����Թ��ܵ�����ʵ�֣�
 * ��Ϊ erase һ���Ѿ���ȥ�������û�й�ϵ��
 * �������ｫ����Ҳ��Ϊ�����������˽���
 * �����ж����Ǵ������ı�ź������ * 
 */
void _getDie(std::string c) {
	if (flag) return;
	if (c == "0") return; // ���ڴ������˿�ɱ����� 
	int x, wH = 0;
	if (c[0] == '#') wH = 1;
	// wH ������¼�Ƿ���Ů��ɱ�� 
	if (c[0] == 'y' || c[0] == '#') c = c.substr(1, (int)c.size()-1); 
	std::cout << c << '\n';
	int p = c.find(' ');
	static int hnt = 0, wtyy = 0;
	if (c[p-1] == '*') x = to_int(c.substr(0, p-1)), hnt = x;
	else x = to_int(c.substr(0, p)); 
	erase(x);
	if (x == id) {
		cs.rec();
		std::cout << "������!\n�������ԣ�\n> ";
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

// ��ӡ��Ϣ���������ʱ��
void print(std::string s) {
	system("cls");
	std::cout << "ʱ�� : " << s << '\n';
	std::cout << "��ı���� " << id << "!\n";
	std::cout << "�����ŵ���� : " << to_string(all) << '\n';
	if (flag) std::cout << "���Ѿ�����!\n";
	std::cout << "�ȴ��������...\n";
}

// �յ�������ָ�� you ʱ���Է���������˵ķ���
void talk(int num = 0) {
	int l = all.size() - flag;
	if (num) l = num-1;
	for (int i = 0; i <= l; i++) {
		std::string c = cs.rec();
		if (c == "you") {
			std::cout << "������ķ���:\n> ";
			std::string an;
            an = getMes();
			cs.sent(an);
		}
		else std::cout << c << '\n';
	}
}

// ���յ��Ĳ���ƽ��ҹ�������н���ָ�� over ��һֱ���ղ�ִ���������
void getDie() {
	if (flag) return;
	std::string an = cs.rec();
	while (an[0] != '!' && an[0] != 'o') _getDie(an), an = cs.rec();
	if (an[0] == '!') std::cout << an << '\n';
	system("pause");
}

// �����ݣ�������֪ͨ����
void getCha() {
	std::string c = cs.rec();
	cha = to_int(c);
	std::cout << "�������� " << con[cha] << '\n';
	if (cha == _Wolf) c = cs.rec(), std::cout << "���е����ǣ���Ķ��ѣ� " + c + '\n'; 
}

// ��ͶƱɱ����
void wolf() {
	if (cha != _Wolf || flag) return;
	vote("ɱ��");
}

// Ԥ�Լ�ͶƱ���鲢�ӷ�������ý����+/-��
void prophet() {
	if (cha != _Prophet || flag) return;
	vote("����");
	std::string an = cs.rec();
	if (an == "+") std::cout << "�Ǹ�����!\n";
	else std::cout << "���ǣ�!\n";
}

/*
 * Ů��
 * 1. ��֪������
 * 2. ��û��ʹ�ù���ҩ�Ҳ��Ƿ���ҹ�Լ�������ѯ���Ƿ�ʹ�ý�ҩ��
 * 3. û���ڵ�ǰʹ�ý�ҩ��û�ù���ҩѯ���Ƿ񶾡�
 */
void witch(int no) {
	if (cha != _Witch || flag) return; 
	static int jy = 0, dy = 0, be = 0;
	std::string c = cs.rec();
	std::cout << c << " ���ˣ�\n";
	if (jy && dy) system("pause");
	if (jy == 0 && !(no != 1 && to_int(c) == id)) {
		std::cout << "�Ƿ�ʹ�ý�ҩ��(0 �� 1 ��)\n> ";
		int x = getVote();
		cs.sent(to_string(x));
		if (x) be = 1, jy = 1;
	} 
	if (dy == 0 && !be) {
		if (vote("��") != 0) dy = 1;
	}
	be = 0;
} 

// ͶƱѡ����������
void guard() {
	if (cha != _Guard || flag) return;
	vote("����");
}

// ���յ������� & ��ͷ��Ϣ������������Ӯ�ж�
bool check() {
	std::string s = cs.rec();
	if (s[0] != '&') return false;
	std::cout << s.substr(1, s.size()-1) << '\n';
	return true;
}

void eleShe() {
	std::cout << "�Ƿ�Ҫ��ѡ������1/0���� \n> ";
	int x = getVote();
	cs.sent(to_string(x));
	std::string c = cs.rec();
	if (c == "nonono") {
		std::cout << "����û�˲�ѡ��";
		return;
	}
	else std::cout << c << "��ѡ�������ȴ�����...\n"; 
	talk(all.size()+!!x); vote("ѡΪ����");
    c = cs.rec();
	int p = c.find(' ');
	sher = to_int(c.substr(0, p));
	if (sher) std::cout << c << '\n';
	else std::cout << c.substr(p+1, c.size()-p-1) << '\n';
} 
void updOrd() {
	if (id != sher || flag) return;
	std::cout << "��ת����˳����\n> ";
	int x = getVote();
	cs.sent(to_string(x)); 
}

// ��������ִ����Ҳ���
void night(int x) {
	print("����");
	wolf();
	prophet();
	witch(x);
	guard();
	if (x == 1 && hasShe) eleShe(); 
	getDie();
	updOrd();
}

// ���췢�Բ�ͶƱ������
void day() {
	print("����");
	talk();
	vote("ͶƱ����");
	getDie();
}

// �ڵȴ�ʱ���Ͱ汾��������֤
// Ȼ����շ��������������ֱ������������
void preWork() {
	cs.sent(version);
	std::string per = cs.rec();
	while (per != "over") {
		system("cls");
		std::cout << version + "�ȴ��������... "; 
		std::cout << per << '\n';
		per = cs.rec();
	}
}

// ������������������
void getInfo() {
	std::string c = cs.rec();
	system("cls");
	std::cout << "һ���� " << c << " �����\n";
	B = to_int(c);
	for (int i = 1; i <= B; i++) all.push_back(i);
	c = cs.rec();
	id = to_int(c);
	std::cout << "��ı���� " << id << "!\n";
	getCha();
	c = cs.rec();
	if (c == "sif") hasShe = 1;
	system("pause");
}
