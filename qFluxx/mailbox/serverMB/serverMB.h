#ifndef SERVERMB_H
#define SERVERMB_H

#include <iostream>
#include <vector>
#include <string>  
#include <sstream>

#if defined(WIN32)//WINDOWS
#include <winsock2.h>
#else//LINUX
#include <sys/stat.h> 
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>      
#include <arpa/inet.h>
#endif

#include "../../cardLib_code/cardLib.h"
#include "../../config.h"

#ifdef WIN32
#pragma comment(lib,"ws2_32.lib") 
#endif

class ServerMB //����������
{
public:
	ServerMB();
	~ServerMB();
	bool acceptNewClient();        //����һ���¿ͻ�
	std::string recvMsg(int num);  //�������Ե�num���ͻ�����Ϣ
	bool sendMsg(int num ,std::string s); //���num���ͻ��˷�����Ϣ

	//getMsg����������ʵ�ֶ��ֵ��÷�ʽ
	//bool getMsg(int playerNum, MsgType m);
	bool getMsg(int playerNum,MsgType m, std::string& name);
	bool getMsg(int playerNum, MsgType m, std::vector<const Card*>& relatedCards);
	bool getMsg(int playerNum, MsgType m, int& relatedPlayer);
	//createMsg����������ʵ�ֶ��ֵ��÷�ʽ
	bool createMsg(int playerNum, MsgType m); //����������Ϣ
	bool createMsg(int playerNum, MsgType m, int relatedPlayer, std::string name);//����ADD_PLAYER��Ϣ
	bool createMsg(int playerNum, MsgType m, std::vector<const Card*> relatedCards);//������GAME_START,RULE��Ϣ
	bool createMsg(int playerNum, MsgType m, int relatedInfo); //����DROP_CARD_C,DROP_KEEPER_C,GAME_OVER,DROP_RULE_C��Ϣ
	bool createMsg(int playerNum, MsgType m, std::vector<const Card*> relatedCards,int relatedInfo); //����CARD_PLAYED,CARD_DROPED,CARD_UPDATE��Ϣ
	bool createMsg(int playerNum, MsgType m, int relatedPlayer, int additional); //����ROUND_BEGIN��CARD_NUM��Ϣ
	bool createMsg(int playerNum, MsgType m, std::vector<const Card*> relatedCards,int relatedPlayer, int additional); //����KEEPER_UPDATE��Ϣ

private:
	int serverSock;
	int _clientNum;
	std::vector<int> clientSock;
	CardLib& _cards;
	const Card* str2Card(std::string s); //����Ϣ�ַ�������Card*
	std::string card2Str(const Card* c); //��Card*������Ϣ�ַ���
};
#endif