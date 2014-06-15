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
#include <netdb.h>   
#include <arpa/inet.h>
#endif

#include "../../cardLib_code/cardLib.h"
#include "../../config.h"
#ifdef WIN32
#pragma comment(lib,"ws2_32.lib") 
#endif

class ClientMB  //�ͻ�������
{
public:
	ClientMB();
	~ClientMB();
	bool connectServer(const std::string server_ip);

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
protected:
	CardLib& _cards;
	bool ipCheck(const std::string s) const; //���ڼ��ip��ַ��ʽ�Ƿ���ȷ
	const Card* str2Card(std::string s); //����Ϣ�ַ�������Card*
	std::string card2Str(const Card* c); //��Card*������Ϣ�ַ���
private:
	int clientSock;
	bool sendMsg(std::string s);
	std::string recvMsg();
};

#endif