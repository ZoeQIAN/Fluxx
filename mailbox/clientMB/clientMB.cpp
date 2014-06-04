#include"clientMB.h"
using namespace std;

ClientMB::ClientMB()
{
	//��ʼ��winsock  
	WSADATA wsaD;
	WSAStartup(MAKEWORD(1, 1), &wsaD);
	//��ʼ���ͻ���socket  
	clientSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
}

ClientMB::~ClientMB()
{
	closesocket(clientSock);
	WSACleanup();
}

bool ClientMB::connectServer(const string server_ip)
{
	if (!ipCheck(server_ip))   //ip��ַ�ĸ�ʽ����
		return false;
	//���ӷ�����  
	sockaddr_in servAddr;
	servAddr.sin_family = AF_INET;
	servAddr.sin_addr.s_addr = inet_addr(server_ip.c_str()); //ָ��������ip 
	servAddr.sin_port = htons(PORT);             //ָ���˿�  
	if (connect(clientSock, (struct sockaddr*)&servAddr, sizeof(servAddr)) == -1) //ͨ���׽�����������
		return false;
	else
		return true;
}

bool ClientMB::sendMsg(string s)
{
	char buf[BUFFMAX];
	int index = 0;
	while (1)
	{
		string temp = s.substr(index, BUFFMAX - 1);
		memset(buf, 0, sizeof(buf));
		strcpy_s(buf, temp.c_str());
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
	if (recv(clientSock, buf, BUFFMAX, 0) != -1)
		s = buf;
	return s;
}

bool ClientMB::ipCheck(const string s) const
{
	int counter = 0, cnum = 0, cpoint = 0;
	string numStr;
	for (int i = 0; i < s.size(); i++)
	{
		if (isdigit(s[i]))
			counter++;
		else if (s[i] == '.')
		{
			cpoint++;
			numStr = s.substr(i - counter, counter);
		}
		else                //���������ֺ�'.'֮����ַ�
			return false;
		if (i == s.size() - 1)  //�����ַ���ĩβ
		{
			if (numStr == "")
				numStr = s.substr(i + 1 - counter, counter);
			else                //ĩβΪ'.'
				return false;
		}

		if (numStr != "")   //��⵽���ִ�
		{
			counter = 0;
			int tempInt = atoi(numStr.c_str());
			numStr = "";
			if (tempInt < 0 || tempInt > 255)  //���ֲ���0~255��Χ��
				return false;
			else
			{
				cnum++;
			}
		}
	}
	return (cnum == 4 && cpoint == 3);      //����Ƿ����4�����ֺ�3��'.'
}