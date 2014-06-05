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

bool ClientMB::createMsg(MsgType m, vector<string> relatedCards,string name)
{
	//����������������Ϣ�ַ���
	string s;
	int num;
	switch (m)
	{
	case REGISTER:
		if (name == "")			//δ�����������
			return false;
		else
			s = "0" + name;
		break;
	case ACK:
		s = "1";
		break;
	case NACK:
		s = "2";
		break;
	case PLAY:
		if (relatedCards.size() != 1)//δ����һ����
			return false;
		else{
			if (isEffectiveCard(relatedCards[0]))
				s = "3" + relatedCards[0];
			else
				return false;
			
			break;
		}
	case DROPCARD:
		if (relatedCards.size() == 0)//δ���뿨����Ϣ
			return false;
		else{
			s = "4";
			for (int i = 0; i < relatedCards.size(); i++)
			{
				if (isEffectiveCard(relatedCards[i]))
					s += relatedCards[i];
				else
					return false;
			}
			break;
		}
	case DROPKEEPER:
		if (relatedCards.size() == 0)//δ���뿨����Ϣ
			return false;
		else{
			s = "5";
			for (int i = 0; i < relatedCards.size(); i++)
			{
				//����Ƿ�Ϊ��Ч���������Ʊ��
				int j;
				for (j = 0; j < relatedCards[i].size();j++)
					if (!isdigit(relatedCards[i][j])) break; //��⵽������
				if (j != relatedCards[i].size())   //���������ֵ��ַ�
					return false;
				num = atoi(relatedCards[i].c_str());
				if (num / 100 == 2 && 0 < (num - 200) < 19) 
					s += relatedCards[i];
				else
					return false;
			}
			break;
		}
	default: return false;
	}
	//������Ϣ
	if (!sendMsg(s))
		return false;
	else
		return true;
}
//bool ClientMB::getMsg(MsgType& m, std::string& name, int& playerNum, vector<int>& relatedCards, int& additional)
//{
//	string s = recvMsg()
//}

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

bool ClientMB::isEffectiveCard(string s) const
{
	//����Ƿ����������
	int j;
	for (j = 0; j < s.size(); j++)
		if (!isdigit(s[j])) break; //��⵽������
	if (j !=s.size())   //���������ֵ��ַ�
		return false;
	int num = atoi(s.c_str());
	int type = num / 100;
	int ruleType,ruleNum;
	switch (type)
	{
	case 0: return false; //���ɴ�����������ƻ���������
	case 1:               //�¹�����
		ruleType = (num - 100) / 10;
		ruleNum = num - 100 - ruleType * 10;
		if (ruleType == 1 && 0 <= ruleNum <= 2) //�������޹�����
			return true;
		else if (ruleType == 2 && 2 <= ruleNum <= 4)//���������������
			return true;
		else if (ruleType == 3 && 2 <= ruleNum <= 5)//ץ������������
			return true;
		else if (ruleType == 4 && 2 <= ruleNum <= 4)//��������������
			return true;
		else if (ruleType == 5 && 1 <= ruleNum <= 8)//����������
			return true;
		else
			return false;
	case 2:       //��������
		if (1 <= (num - 200) <= 18)
			return true;
		else
			return false;
	case 3:     //Ŀ����
		if (1 <= (num - 300) <= 23)
			return true;
		else
			return false;
	default: return false;
	}
}