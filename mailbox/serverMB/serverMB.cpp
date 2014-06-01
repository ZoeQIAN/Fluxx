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