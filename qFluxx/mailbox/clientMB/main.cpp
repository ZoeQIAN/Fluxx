#include "clientMB.h"
using namespace std;

int main()
{
	ClientMB client;
	cout << "�ͻ����ѽ���" << endl;
	if (client.connectServer("127.0.0.1"))
		cout << "�����ӵ�������" << endl;
	else
	{
		cout << "���ӳ���" << endl;
		return -1;
	}

	//client����Ϣ����
	vector<string> cards;
	client.createMsg(REGISTER, cards, "Stella");
	cout << "�ѷ��͵�һ����Ϣ:REGISTER" << endl;
	client.createMsg(ACK, cards);
	cout << "�ѷ��͵ڶ�����Ϣ:ACK" << endl;
	client.createMsg(NACK, cards);
	cout << "�ѷ��͵�������Ϣ:NACK" << endl;

	cards.push_back("000");
	if(client.createMsg(PLAY, cards))
	cout << "�ѷ��͵�������Ϣ:PLAY" << endl;
	else cout << "��������Ϣ���ͳ���" << endl;
	cards.clear();
	cards.push_back("102");
	if (client.createMsg(PLAY, cards))
		cout << "�ѷ��͵�������Ϣ:PLAY" << endl;
	else cout << "��������Ϣ���ͳ���" << endl;
	cards.clear();
	cards.push_back("142");
	if (client.createMsg(PLAY, cards))
		cout << "�ѷ��͵�������Ϣ:PLAY" << endl;
	else cout << "��������Ϣ���ͳ���" << endl;

	cards.push_back("158");
	if (client.createMsg(DROPCARD, cards))
		cout << "�ѷ��͵�������Ϣ:DROPCARD" << endl;
	else cout << "��������Ϣ���ͳ���" << endl;

	if (client.createMsg(DROPKEEPER, cards))
		cout << "�ѷ��͵�������Ϣ:DROPKEEPER" << endl;
	else cout << "��������Ϣ���ͳ���" << endl;

	cards.clear();
	cards.push_back("201");
	if (client.createMsg(DROPKEEPER, cards))
		cout << "�ѷ��͵�������Ϣ:DROPKEEPER" << endl;
	else cout << "��������Ϣ���ͳ���" << endl;
	/*client.sendMsg("From client 0 to server!");
	cout << "�ѷ���" << endl;
	cout << "FROM server�� "<<client.recvMsg() << endl;
	while (1)
	{
		string s;
		getline(cin,s);
		client.sendMsg(s);
		cout << "�ѷ���" << endl;
		cout << "FROM server�� " << client.recvMsg() << endl;
	}*/
}