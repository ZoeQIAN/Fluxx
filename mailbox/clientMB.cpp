#include"..\mailbox\mailBox.h"
using namespace std;

ClientMB::ClientMB(string server_ip)
{
	//��ʼ��winsock  
	WSADATA wsaD;
	WSAStartup(MAKEWORD(1, 1), &wsaD);
	//��ʼ���ͻ���socket  
	clientSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	//���ӷ�����  
	sockaddr_in servAddr;
	servAddr.sin_family = AF_INET;
	servAddr.sin_addr.s_addr = inet_addr(server_ip.c_str()); //ָ��������ip 
	servAddr.sin_port = htons(PORT);             //ָ���˿�  
	connect(clientSock, (struct sockaddr*)&servAddr, sizeof(servAddr)); //ͨ���׽�����������
}

ClientMB::~ClientMB()
{
	closesocket(clientSock);
	WSACleanup();
}

bool ClientMB::sendMsg(string s)
{
	char buf[BUFFMAX];
	int index = 0;
	while (1)
	{
		string temp = s.substr(index, BUFFMAX - 1);
		memset(buf, 0, sizeof(buf));
		strcpy(buf, temp.c_str());
		if (send(clientSock, buf, strlen(buf) + 1, 0) == -1) //���ͳ���
			return false; 
		else
		{
			index += BUFFMAX - 1;
			if (index >= s.size())  //�ѷ����ַ�����������
				return true;
		}
	}
}

string ClientMB::recvMsg()
{
	string s;
	char buf[BUFFMAX];
	memset(buf, 0, sizeof(buf));
	int recvLen = 0;
	do {
		recvLen = recv(clientSock, buf, BUFFMAX, 0);
		s += buf;
		memset(buf, 0, sizeof(buf));
	} while (recvLen > 0);
	return s;
}