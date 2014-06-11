#ifndef CLIENTMB_H
#define CLIENTMB_H

#include <iostream>
#include <vector>
#include <string>  
#include <sstream>
#include <cctype>
#if defined(WIN32)//WINDOWS
#include <winsock2.h>
#else//LINUX
#include <sys/stat.h> 
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>      // define structures like hostent
#endif
//#include "cardLib.h"
#include "../../cardLib_code/cardLib.h"
#include "../../config.h"
#ifdef WIN32
#pragma comment(lib,"ws2_32.lib") 
#endif
//const int PORT = 1234; //Ĭ��PORT NUMBER = 1234
//const int BUFFMAX = 512;
//extern enum MsgType;
/*enum MsgType{
	NACK,//�����ź�, �ͻ��˺ͷ���������
	//���������͵���Ϣ����
	ADD_PLAYER,//�������
	GAME_START,//��Ϸ��ʼ
	//DRAW,//������Ϣ
	ROUND_BEGIN,//�غϿ�ʼ
	CARD_UPDATE,//���Ƹ�����Ϣ
	PLAY_C, //��������
	//ACK,//ȷ���ź�
	CARD_NUM,//��������������������ƻ��ж��ƶ��ı��֪ͨ
	CARD_PLAYED,//��ǰ������Ϣ
	CARD_DROPED,  //�������������Ϣ
	RULE,//������Ϣ
	KEEPER_UPDATE,//������������
	DROP_CARD_C,//��������
	DROP_KEEPER_C,//������������
	GAME_OVER,//��Ϸ����
	//CARD_STOLEN,//��ȡ��������ҿ��ƣ���Ҫ���̴�����������ж��ơ�ȡ����֮��
	CHOOSE_PLAYER_C, //ѡ��һλ��ҵ���������ж��ơ��������ơ���ȡ����֮��
	CHOOSE_KEEPER_C, //ѡ��һ��������ҵ��ж�����������ж��ơ���������͵�ԡ�
	EXCHANGE_KEEPER_C, //ѡ�񽻻�һ������������ж��ơ����������
	DROP_RULE_C,//���������Ƶ���������ж��ơ�ȡ�����򡱡���һ�¡�
	CHOOSE_GOAL_C, //ѡ��Ŀ���Ƶ��������˫��Ŀ��ʱ��ĳĿ����滻

	//�ͻ��˷��͵���Ϣ����
	REGISTER,//ע����Ϣ
	PLAY_I,//������Ϣ
	DROP_CARD_I, //������Ϣ
	DROP_KEEPER_I, //����������Ϣ
	CHOOSE_KEEPER_I,//ѡ�����������Ϣ�������ж��ơ���������͵�ԡ������������
	DROP_RULE_I,//ѡ��Ĺ�������Ϣ�������ж��ơ�ȡ�����򡱡���һ�¡�
	CHOOSE_PLAYER_I, //ѡ�����ұ�ţ� �����ж��ơ��������ơ���ȡ����֮��
	CHOOSE_GOAL_I
};*/
//extern enum MsgType;
class ClientMB  //�ͻ�������
{
public:
	ClientMB();
	~ClientMB();
	bool connectServer(const std::string server_ip);
	bool sendMsg(std::string s);
	std::string recvMsg();
		//createMsg����������ʵ�ֶ��ֵ��÷�ʽ
	bool createMsg(MsgType m); //����NACK��Ϣ
	bool createMsg(MsgType m, std::string name);//����REGISTER��Ϣ
	bool createMsg(MsgType m, std::vector<const Card*>relatedCards);//����PLAY_I,DROP_CARD_I,DROP_KEEPER_I,CHOOSE_KEEPER_I,DROP_RULE_I,CHOOSE_GOAL_I��Ϣ
	bool createMsg(MsgType m, int relatedPlayer); //����CHOOSE_PLAYER_I��Ϣ

		//getMsg����������ʵ�ֶ��ֵ��÷�ʽ
	bool getMsg(MsgType& m); //����������Ϣ����
	bool getMsg(MsgType m, int& relatedPlayer, std::string& name);//����ADD_PLAYER��Ϣ
	bool getMsg(MsgType m, std::vector<const Card*>& relatedCards);//����GAME_START,RULE��Ϣ
	bool getMsg(MsgType m, int& relatedInfo); //����DROP_CARD_C,DROP_KEEPER_C,GAME_OVER,DROP_RULE_C��Ϣ
	bool getMsg(MsgType m, std::vector<const Card*>& relatedCards,int& relatedInfo); //����CARD_PLAYED,CARD_DROPED,CARD_UPDATE��Ϣ
	bool getMsg(MsgType m, int& relatedPlayer, int& additional); //����ROUND_BEGIN��CARD_NUM��Ϣ
	bool getMsg(MsgType m, std::vector<const Card*>& relatedCards,int& relatedPlayer, int& additional); //����KEEPER_UPDATE��Ϣ
private:
	//SOCKET clientSock;
	int clientSock;
	CardLib& _cards;
	bool ipCheck(const std::string s) const; //���ڼ��ip��ַ��ʽ�Ƿ���ȷ
	const Card* str2Card(std::string s); //����Ϣ�ַ�������Card*
	std::string card2Str(const Card* c); //��Card*������Ϣ�ַ���
};

#endif