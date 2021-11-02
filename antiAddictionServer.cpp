#include "communi.h"
twt_client cs;
int main() {
	std::string ip;
	std::cin >> ip;
	while (1) { 
		cs.init(1234, ip);
		if (!cs.sent("fcm", 1)) continue; 
		std::cout << "running...";
		int x;
		std::ifstream in("aa.twt");
		in >> x;
		in.close();
		if (x >= 5) cs.sent("kyks");
		else cs.sent("6Q&okIlzyOb8DaIO@nuy");
		std::ofstream out("aa.twt");
		out << x+1; 
		out.close();
		cs.rec(1);
		system("cls");
	}
}
