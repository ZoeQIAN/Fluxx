#include <iostream>
#include <vector>
#include <string>  
#include <cctype>
#include <winsock2.h>  
#include "cardLib.h"
#pragma comment(lib,"ws2_32.lib") 
const int PORT = 1234; //Ĭ��PORT NUMBER = 1234
const int BUFFMAX = 512;

enum MsgType{
	//���������͵���Ϣ����
	ADD_PLAYER = 0,//�������
	GAME_START,//��Ϸ��ʼ
	DRAW,//������Ϣ
	ROUND_BEGIN,//�غϿ�ʼ
	ACK,//ȷ���ź�
	NACK,//�����ź�
	DROP_CARD_C,//��������
	DROP_KEEPER_C,//������������
	CARD_NUM,//���������������
	CARD_PLAYED,//��ǰ������Ϣ
	GAME_OVER,//��Ϸ����
	RULE,//������Ϣ
	KEEPER_UPDATE,//����������������
	CARD_DROPED,  //�������������Ϣ
	//�ͻ��˷��͵���Ϣ����
	REGISTER,//ע����Ϣ
	PLAY,//������Ϣ
	DROP_CARD_I, //������Ϣ
	DROP_KEEPER_I //����������Ϣ
	//���⣬�ͻ��˷���ACK,NACK�����������Ϣ������ͬ
};
	
class ClientMB  //�ͻ�������
{
public:
	ClientMB();
	~ClientMB();
	bool connectServer(const std::string server_ip);
	bool sendMsg(std::string s);
	std::string recvMsg();
		//createMsg����������ʵ�ֶ��ֵ��÷�ʽ
	bool createMsg(MsgType m); //����ACK��NACK��Ϣ
	bool createMsg(MsgType m, std::string name);//����REGISTER��Ϣ
	bool createMsg(MsgType m, std::vector<const Card*>relatedCards);//����PLAY,DROP_CARD_I,DROP_KEEPER_I��Ϣ

		//getMsg����������ʵ�ֶ��ֵ��÷�ʽ
	bool getMsg(int playerNum, MsgType& m); //����ACK,NACK��Ϣ
	bool getMsg(int playerNum, MsgType& m, int& relatedPlayer, std::string& name);//����ADD_PLAYER��Ϣ
	bool getMsg(int plaerNum, MsgType& m, std::vector<const Card*>& relatedCards);//����GAME_START,RULE��Ϣ
	bool getMsg(int playerNum, MsgType& m, int relatedInfo); //����ROUND_BEGIN,DROP_CARD_C,DROP_KEEPER_C,GAME_OVER��Ϣ
	bool getMsg(int playerNum, MsgType& m, std::vector<const Card*> relatedCards,int relatedInfo); //����DRAW,CARD_PLAYED,CARD_DROPED��Ϣ
	bool getMsg(int playerNum, MsgType& m, int relatedPlayer, int additional); //����CARD_NUM��Ϣ
	bool getMsg(int playerNum, MsgType& m, std::vector<const Card*> relatedCards,int relatedPlayer, int additional); //����KEEPER_UPDATE��Ϣ
private:
	SOCKET clientSock;
	CardLib& _cards;
	bool ipCheck(const std::string s) const; //���ڼ��ip��ַ��ʽ�Ƿ���ȷ
	const Card* str2Card(std::string s); //����Ϣ�ַ�������Card*
	std::string card2Str(const Card* c); //��Card*������Ϣ�ַ���
};

