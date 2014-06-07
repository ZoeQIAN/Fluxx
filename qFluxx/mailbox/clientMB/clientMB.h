#include <iostream>
#include <vector>
#include <string>  
#include <cctype>
//#include <sstream>
#include <winsock2.h>  
#pragma comment(lib,"ws2_32.lib") 
const int PORT = 1234; //Ĭ��PORT NUMBER = 1234
const int BUFFMAX = 512;

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

class ClientMB  //�ͻ�������
{
public:
	ClientMB();
	~ClientMB();
	bool connectServer(const std::string server_ip);
	bool sendMsg(std::string s);
	std::string recvMsg();
	bool getMsg(MsgType& m,std::vector<std::string>& relatedCards, int& playerNum, int& additional, std::string& name);
	bool createMsg(MsgType m, std::vector<std::string>relatedCards, std::string name = "");
	//bool createMsg(MsgType m = NACK, std::string name = "", std::vector<std::string>::iterator*  relatedCards = NULL);
private:
	SOCKET clientSock;
	bool ipCheck(const std::string s) const; //���ڼ��ip��ַ��ʽ�Ƿ���ȷ
	bool isEffectiveCard(std::string s) const;  //���ڼ�⴫�뿨���Ƿ���Ч //δ���ж��ƣ�����������
};

