#include "serverMB.h"
using namespace std;
//For test!!
int main()
{
	ServerMB server;
	cout << "�ѽ���������" << endl;
	server.acceptNewClient();
	//server.acceptNewClient();
	cout << "�ѽ�������" << endl;
	MsgType m;
	vector<Card*> c;
	string s;
	for (int j = 0; j < 6; j++)
	{
		server.getMsg(0, m, s, c);
		cout << "���յ�"<<j<<"����Ϣ�� m = " << m << "\ts = " << s << endl << "������Ϣ��";
		for (int i = 0; i < c.size(); i++)
			cout << c[i]->getType() << c[i]->getNum() << "\t";
		cout << endl << endl;
	}

	/*while (1){
		cout << "From client0:" << server.recvMsg(0) << endl;
		cout << "From client1:" << server.recvMsg(1) << endl;
		server.sendMsg(0, "from server to client 0!");
		server.sendMsg(1, "from server to client 1!");
		cout << "�ѷ���" << endl;
	}*/
	//server.sendMsg(0);
}
