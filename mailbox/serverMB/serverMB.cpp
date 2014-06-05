#include "serverMB.h"
using namespace std;

ServerMB::ServerMB():
_clientNum(0)
{
	//��ʼ��winsock   
	WSADATA wsaD;
	WSAStartup(MAKEWORD(1, 1), &wsaD);
	//��ʼ������������socket
	serverSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	// ��������ַ�ͼ����׽���  
	struct sockaddr_in servAddr;   //������ַ   
	servAddr.sin_family = AF_INET;
	servAddr.sin_port = htons(PORT); //ָ���˿�  
	servAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	bind(serverSock, (struct sockaddr *)&servAddr, sizeof(servAddr));
	//��������  
	listen(serverSock, MAXCLIENT);   //���ͬʱ����4������
}

ServerMB::~ServerMB()
{
	for (int i = 0; i < _clientNum; i++)
		closesocket(clientSock[i]);
	closesocket(serverSock);
	WSACleanup();
}

bool ServerMB::sendMsg(int num,string s)
{
	if (num >= _clientNum)   //��������ȷ
		return false;
	char buf[MAXBUFF];
	int index = 0;
	while (1)
	{
		string temp = s.substr(index, MAXBUFF - 1);
		memset(buf, 0, sizeof(buf));
		strcpy_s(buf, temp.c_str());
		if (send(clientSock[num], buf, strlen(buf) + 1, 0) == -1) //���ͳ���
			return false;
		else
		{
			index += MAXBUFF - 1;
			if (index >= s.size())  //�ѷ����ַ�����������
				return true;
		}
	}
}

string ServerMB::recvMsg(int num)
{
	if (num >= _clientNum)   //��������ȷ
		return "";

	string s;
	char buf[MAXBUFF];
	memset(buf, 0, sizeof(buf));
	int recvLen = 0;
	if (recv(clientSock[num], buf, MAXBUFF, 0) != -1)
		s = buf;
	return s;
}

bool ServerMB::acceptNewClient()
{
	if (_clientNum == MAXCLIENT)
		return false;

	SOCKADDR_IN clientAddr;
	int len = sizeof(sockaddr);
	SOCKET newClient = accept(serverSock, (sockaddr*)&clientAddr, &len);
	if (newClient != INVALID_SOCKET)
	{
		clientSock.push_back(newClient);
		_clientNum++;
		return true;
	}
	else
		return false;
}

bool ServerMB::getMsg(int playerNum, MsgType& m, std::string& name, std::vector<Card*>& relatedCards)
{
	//�ȴ�������Ϣ
	string s = recvMsg(playerNum);
	
	//������Ϣ
	switch (s[0])
	{
	case '0': m = REGISTER; name = s.substr(1); break;
	case '1': m = ACK; name = ""; break;
	case '2': m = NACK; name = ""; break;
	case '3':
		m = PLAY; name = "";
		relatedCards.clear();
		relatedCards.push_back(str2Card(s.substr(1)));
		break;
	case '4':
		m = DROPCARD; name = "";
		relatedCards.clear();
		for (int i = 1; i < s.size(); i += 3)
			relatedCards.push_back(str2Card(s.substr(i, 3)));
		break;
	case '5':
		m = DROPKEEPER; name = "";
		relatedCards.clear();
		for (int i = 1; i < s.size(); i += 3)
			relatedCards.push_back(str2Card(s.substr(i, 3)));
		break;
	default: return false;
	}
	return true;
}

bool ServerMB::createMsg( int playerNum, MsgType m, vector<Card*> relatedCards,int relatedPlayer, int additional, string name)
{
	//����������������Ϣ�ַ���
	string s;
	stringstream ss;
	switch (m)
	{
	case ADDPLAYER:
		if (name == "" || relatedPlayer < 0 || relatedPlayer>3) //δ������ȷ���ֻ���
			return false;
		else
		{
			ss << relatedPlayer;
			s = "0" + ss.str() + name;
			break;
		}
	case GAMESTART:
		s = "1";
		if (relatedCards.size() != 3) //δ�����ʼ����������Ϣ
			return false;
		for (int i = 0; i < 3; i++)
			s += card2Str(relatedCards[i]);
		break;
	case ROUNDBEGIN:
		if (relatedPlayer < 0 || relatedPlayer >3) //δ������ȷ���ֻ���
			return false;
		else
		{
			ss << relatedPlayer;
			s = "2" + ss.str();
			break;
		}
	case DRAW:
		s = "3";
		if (additional != 0 && additional != 1)  //δ�����Ƿ������������Ч��Ϣ 
			return false;
		else{
			ss << additional;
			s += ss.str();
		}
			

		for (int i = 0; i < 3; i++)
			s += card2Str(relatedCards[i]);
		break;



			

	}
}

//��server�յ����ַ�����Ϣ�õ���Ӧ�Ŀ�����Ϣ(s����ȷ����MB��֤���˴����ټ�⣩
Card* ServerMB::str2Card(string s)
{
	switch (s[0])
	{
	case '1':  //�¹�����
		if (s[1] == '1')      //��������
			return _cards.getCard(atoi(s.c_str()) - 109);
		else if (s[1] == '2')  //����������
			return _cards.getCard(atoi(s.c_str()) - 118);
		else if (s[1] == '3')//ץ������
			return _cards.getCard(atoi(s.c_str()) - 125);
		else if (s[1] == '4')//��������
			return _cards.getCard(atoi(s.c_str()) - 131);
		else                //�����¹���
			return _cards.getCard(atoi(s.c_str()) - 137);
	case '2':  //��������
		return _cards.getCard(atoi(s.c_str()) - 179);
	case '3':  //Ŀ����
		return _cards.getCard(atoi(s.c_str()) - 261);
	default:   //��������
		return _cards.getCard(0);
	}
}

