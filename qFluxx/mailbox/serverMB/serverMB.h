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
	ADD_PLAYER = 0,//�������
	GAME_START,//��Ϸ��ʼ
	//DRAW,//������Ϣ
	ROUND_BEGIN,//�غϿ�ʼ
	//ACK,//ȷ���ź�
	NACK,//�����ź�
	DROP_CARD_C,//��������
	DROP_KEEPER_C,//������������
	CARD_NUM,//��������������������ƻ��ж��ƶ��ı��֪ͨ
	CARD_PLAYED,//��ǰ������Ϣ
	GAME_OVER,//��Ϸ����
	RULE,//������Ϣ
	KEEPER_UPDATE,//������������
	CARD_DROPED,  //�������������Ϣ
	CARD_UPDATE,//���Ƹ�����Ϣ
	PLAY_C, //��������
	CARD_STOLEN,//��ȡ��������ҿ��ƣ���Ҫ���̴�����������ж��ơ�ȡ����֮��
	CHOOSE_PLAYER_C, //ѡ��һλ��ҵ���������ж��ơ��������ơ���ȡ����֮��
	CHOOSE_KEEPER_C, //ѡ��һ��������ҵ��ж�����������ж��ơ���������͵�ԡ�
	EXCHANGE_KEEPER_C, //ѡ�񽻻�һ������������ж��ơ����������
	DROP_RULE_C,//���������Ƶ���������ж��ơ�ȡ�����򡱡���һ�¡�

	//�ͻ��˷��͵���Ϣ����
	REGISTER,//ע����Ϣ
	PLAY_I,//������Ϣ
	DROP_CARD_I, //������Ϣ
	DROP_KEEPER_I, //����������Ϣ
	CHOOSE_KEEPER_I,//ѡ�����������Ϣ�������ж��ơ���������͵�ԡ������������
	DROP_RULE_I,//ѡ��Ĺ�������Ϣ�������ж��ơ�ȡ�����򡱡���һ�¡�
	CHOOSE_PLAYER_I //ѡ�����ұ�ţ� �����ж��ơ��������ơ���ȡ����֮��
	//���⣬�ͻ��˷���NACK�����������Ϣ������ͬ
};

class ServerMB //����������
{
public:
	ServerMB();
	~ServerMB();
	bool acceptNewClient();        //����һ���¿ͻ�
	std::string recvMsg(int num);  //�������Ե�num���ͻ�����Ϣ
	bool sendMsg(int num ,std::string s); //���num���ͻ��˷�����Ϣ
	//bool getMsg(int playerNum, MsgType m, std::vector<Card*>& relatedCards, std::string& name);
	//getMsg����������ʵ�ֶ��ֵ��÷�ʽ
	bool getMsg(int playerNum, MsgType m);
	bool getMsg(int playerNum,MsgType m, std::string& name);
	bool getMsg(int playerNum, MsgType m, std::vector<const Card*>& relatedCards);
	bool getMsg(int PlayerNum, MsgType m, int relatedPlayer);
	//createMsg����������ʵ�ֶ��ֵ��÷�ʽ
	bool createMsg(int playerNum, MsgType m); //����������Ϣ
	bool createMsg(int playerNum, MsgType m, int relatedPlayer, std::string name);//����ADD_PLAYER��Ϣ
	bool createMsg(int playerNum, MsgType m, std::vector<const Card*> relatedCards);//������GAME_START,RULE,CARD_STOLEN��Ϣ
	bool createMsg(int playerNum, MsgType m, int relatedInfo); //����DROP_CARD_C,DROP_KEEPER_C,GAME_OVER,DROP_RULE_C��Ϣ
	bool createMsg(int playerNum, MsgType m, std::vector<const Card*> relatedCards,int relatedInfo); //����CARD_PLAYED,CARD_DROPED,CARD_UPDATE��Ϣ
	bool createMsg(int playerNum, MsgType m, int relatedPlayer, int additional); //����ROUND_BEGIN��CARD_NUM��Ϣ
	bool createMsg(int playerNum, MsgType m, std::vector<const Card*> relatedCards,int relatedPlayer, int additional); //����KEEPER_UPDATE��Ϣ

private:
	SOCKET serverSock;
	int _clientNum;
	std::vector<SOCKET> clientSock;
	CardLib& _cards;
	const Card* str2Card(std::string s); //����Ϣ�ַ�������Card*
	std::string card2Str(const Card* c); //��Card*������Ϣ�ַ���
};