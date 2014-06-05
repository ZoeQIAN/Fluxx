#include <iostream>
#include <vector>
#include <string>  
#include <sstream>
#include <winsock2.h>  
#include "CardLib.h"
#pragma comment(lib,"ws2_32.lib") 
const int PORT = 1234; //Ĭ��PORT NUMBER = 1234
const int MAXBUFF = 512;
const int MAXCLIENT = 4;

enum MsgType{
	//���������͵���Ϣ����
	ADDPLAYER = 0,//�������
	GAMESTART,//��Ϸ��ʼ
	DRAW,//������Ϣ
	ROUNDBEGIN,//�غϿ�ʼ
	ACK,//ȷ���ź�
	NACK,//�����ź�
	DROPCARD,//��������
	DROPKEEPER,//������������
	CARDNUM,//���������������
	CARDPLAYING,//��ǰ������Ϣ
	GAMEOVER,//��Ϸ����
	RULE,//������Ϣ
	KEEPERUPDATE,//����������������
	//�ͻ��˷��͵���Ϣ����
	REGISTER,//ע����Ϣ
	PLAY//������Ϣ
	//���⣬�ͻ��˷���ACK,NACK,DROPCARD,DROPKEEPER�����������Ϣ������ͬ
};

class ServerMB //����������
{
public:
	ServerMB();
	~ServerMB();
	bool acceptNewClient();        //����һ���¿ͻ�
	std::string recvMsg(int num);  //�������Ե�num���ͻ�����Ϣ
	bool sendMsg(int num ,std::string s); //���num���ͻ��˷�����Ϣ
	bool getMsg(int playerNum, MsgType& m, std::vector<Card*>& relatedCards, std::string& name);
	bool createMsg(int playerNum, MsgType m, std::vector<Card*> relatedCards, int relatedPlayer = 0, int additional = -1, std::string name = "");

private:
	SOCKET serverSock;
	int _clientNum;
	std::vector<SOCKET> clientSock;
	CardLib& _cards = CardLib::getLib();
	Card* str2Card(std::string s); //����Ϣ�ַ�������Card*
	std::string card2Str(Card* c); //��Card*������Ϣ�ַ���
};