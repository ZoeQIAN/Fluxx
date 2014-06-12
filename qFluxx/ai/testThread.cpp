#include "ai.h"
using namespace std;

DWORD WINAPI threadAI(LPVOID lpParameter);
DWORD WINAPI threadServer(LPVOID lpParameter);
extern string convert(MsgType m);

int main()
{
	string ip;
	advancedAI ai(3);
	ai.run();
}


/*
//test ���̳߳ɹ�~~~
int main()
{
	ClientMB client;
	//int pServ = 1, pClient;
	DWORD p[4]; int i = 0;
	CloseHandle(CreateThread(NULL, 0, threadServer, (LPVOID)i, 0, &p[i]));
	i++;
	CloseHandle(CreateThread(NULL, 0, threadAI, (LPVOID)i, 0, &p[i]));

	cout << "player: ������磡" << endl;
	string playerName = "player";
	MsgType m;
	if (client.connectServer("127.0.0.1"))
		cout << playerName << ": ���������������" << endl;
	else
		cout << playerName << ": �����������ʧ��" << endl;
	client.createMsg(MsgType::REGISTER, playerName);
	cout << playerName << ": ����REGISTER��server" << endl;
	client.getMsg(m);
	cout << playerName << ": ����server��Ϣ " << m << endl;

}*/

DWORD WINAPI threadAI(LPVOID lpParameter)
{
	int para = (int)lpParameter;
	ClientMB client;
	MsgType m;
	stringstream ss;
	ss << para;
	string playerName = "AI" + ss.str();
	cout << endl << playerName << ": ������磡" << endl;

	if(client.connectServer("127.0.0.1"))
		cout << playerName << ": ���������������" << endl;
	else
		cout << playerName << ": �����������ʧ��" << endl;
	client.createMsg(MsgType::REGISTER, playerName);
	cout << playerName << ": ����REGISTER��server" << endl;
	client.getMsg(m);
	cout << playerName << ": ����server��Ϣ " <<m<< endl;
	cout << playerName<<": �˳�" << endl;
	return 0;
}

DWORD WINAPI threadServer(LPVOID lpParameter)
{
	int para = (int)lpParameter;
	cout << endl<<"server"<<para<<": ������磡" << endl;
	ServerMB server;
	MsgType m;
	string playerName;
	m = MsgType::REGISTER;
	for (int i = 0; i < 2; i++)
	{
		server.acceptNewClient();
		cout << "server:��һ���ͻ��˽�������" << endl;
	}
	for (int i = 0; i < 2; i++)
	{
		server.getMsg(i, m, playerName);
		cout << "server: �ӵ���" << i << "����ҷ�����Ϣ��" << playerName << endl;
		server.createMsg(i, MsgType::ADD_PLAYER);
		cout << "server: ���͸���" << i << "�����ADD_PLAYER��Ϣ" << endl;
	}
	cout << "server:�˳�" << endl;
	return 0;
}