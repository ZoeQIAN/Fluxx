#include"clientMB.h"
using namespace std;

ClientMB::ClientMB():
	_cards(CardLib::getLib())
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
//����ACK��NACK��Ϣ
bool ClientMB::createMsg(MsgType m)  
{
	//����������������Ϣ�ַ���
	string s;
	switch(m)
	{
		case ACK: s = "1"; break;
		case NACK: s = "2"; break;
		default: return false;
	}
	//������Ϣ
	if (!sendMsg(s))
		return false;
	else
		return true;
}
//����REGISTER��Ϣ
bool ClientMB::createMsg(MsgType m, std::string name)
{
	//����������������Ϣ�ַ���
	string s;
	if(m != REGISTER||name == "")//��Ϣ���ʹ����δ�����������
		return false;
	else 
		s = "0"+name;
	//������Ϣ
	if (!sendMsg(s))
		return false;
	else
		return true;
}
//����PLAY,DROP_CARD_I,DROP_KEEPER_I��Ϣ
bool ClientMB::createMsg(MsgType m, vector<const Card*>relatedCards)
{
	//����������������Ϣ�ַ���
	string s;
	int num;
	switch (m)
	{
		case PLAY:
		if (relatedCards.size() != 1)//δ����һ����
			return false;
		else{
			s = "3" + card2Str(relatedCards[0]);							
			break;
		}
	case DROP_CARD_I:
		if (relatedCards.size() == 0)//δ���뿨����Ϣ
			return false;
		else{
			s = "4";
			for (int i = 0; i < relatedCards.size(); i++)
				s += card2Str(relatedCards[i]);
			break;
		}
	case DROP_KEEPER_I:
		if (relatedCards.size() == 0)//δ���뿨����Ϣ
			return false;
		else{
			s = "5";
			for (int i = 0; i < relatedCards.size(); i++)
			{
				//����Ƿ�Ϊ��Ч���������Ʊ��
				if(relatedCards[i]->getType() == Card::KEEPER && 0<relatedCards[i]->getNum()<19) 
					s += card2Str(relatedCards[i]);
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
//�ڴ�����ACK��Ϣ
bool ClientMB::getMsg(MsgType m) 
{
	//�ȴ�������Ϣ
	string s = recvMsg();
	//������Ϣ
	switch (s[0])
	{	
	case '2': return false;
	case '1': if(m == ACK) return true;
	default: 	
		//������Ϣ���ڴ���ƥ��
		createMsg(NACK); 
		return false;
	}

}
//�ڴ�����ADD_PLAYER��Ϣ
bool ClientMB::getMsg(MsgType m, int& relatedPlayer, std::string& name)
{
	//�ȴ�������Ϣ
	string s = recvMsg();
	//������Ϣ
	switch (s[0])
	{
	case '2': return false;
	case '0': 
		if(m == ADD_PLAYER){
			relatedPlayer = s[1];
			name = s.substr(2); 
			return true;
		}
	default: 
		//������Ϣ���ڴ���ƥ��
		createMsg(NACK); 
		return false;
	}
}
//�ڴ�����GAME_START,RULE��Ϣ
bool ClientMB::getMsg(MsgType m, std::vector<const Card*>& relatedCards)
{
	//�ȴ�������Ϣ
	string s = recvMsg();
	//������Ϣ
	switch (s[0])
	{
	case '2': return false;
	case '3': if(m == GAME_START) break;
			  else {
				  createMsg(NACK);
				  return false;
			  }
	case 'B': if(m == RULE) break;
	default:
		//������Ϣ���ڴ���ƥ��
		createMsg(NACK); 
		return false;
	}
	for (int i = 1; i < s.size(); i += 3)
		relatedCards.push_back(str2Card(s.substr(i, 3)));
	return true;
}
//�ڴ�����ROUND_BEGIN,DROP_CARD_C,DROP_KEEPER_C,GAME_OVER��Ϣ
bool ClientMB::getMsg(MsgType m, int& relatedInfo)
{
	//�ȴ�������Ϣ
	string s = recvMsg();
	//������Ϣ
	switch (s[0])
	{
	case '2': return false;
	case '4': if(m == ROUND_BEGIN)break;
			  else {
				  createMsg(NACK);
				  return false;
			  }
	case '6': if(m == DROP_CARD_C) break;
			else {
				  createMsg(NACK);
				  return false;
			  }
	case '7': if(m == DROP_KEEPER_C) break;
			  else{
				  createMsg(NACK);
				  return false;
			  }
	case 'A': if(m == GAME_OVER) break;
	default:  createMsg(NACK);
			  return false;
	}
	relatedInfo = atoi(s.substr(1).c_str());
	return true;
}
//�ڴ�����DRAW,CARD_PLAYED,CARD_DROPED��Ϣ
bool ClientMB::getMsg(MsgType m, std::vector<const Card*>& relatedCards,int& relatedInfo)
{
	//�ȴ�������Ϣ
	string s = recvMsg();
	//������Ϣ
	switch (s[0])
	{
	case '2': return false;
	case '5': if(m == DRAW) break;
			  else{
				  createMsg(NACK);
				  return false;
			  }
	case '9': if(m == CARD_PLAYED) break;
			  else{
				  createMsg(NACK);
				  return false;
			  }
	case 'D': if(m == CARD_DROPED) break;
	default: createMsg(NACK);
			  return false;
	}
	relatedInfo = atoi(s.substr(1,1).c_str());
	for(int i = 2; i < s.size(); i += 3)
		relatedCards.push_back(str2Card(s.substr(i, 3)));
	return true;
}
//�ڴ�����CARD_NUM��Ϣ
bool ClientMB::getMsg(MsgType m, int& relatedPlayer, int& additional)
{
	//�ȴ�������Ϣ
	string s = recvMsg();
	//������Ϣ
	switch (s[0])
	{
	case '2': return false;
	case '8': 
		if(m== CARD_NUM){
		relatedPlayer = s[1];
		additional = atoi(s.substr(2).c_str());
		return true;
		}
	default:createMsg(NACK);
			return false;
	}

}
//�ڴ�����KEEPER_UPDATE��Ϣ
bool ClientMB::getMsg(MsgType m, std::vector<const Card*>& relatedCards,int& relatedPlayer, int& additional)
{
	//�ȴ�������Ϣ
	string s = recvMsg();
	//������Ϣ
	switch (s[0])
	{
	case '2': if( m== NACK) return false;
	case 'C': 
		if(m== KEEPER_UPDATE){
		relatedPlayer = atoi(s.substr(1,1).c_str());
		additional = atoi(s.substr(2,1).c_str());
		for(int i = 3; i < s.size(); i += 3)
			relatedCards.push_back(str2Card(s.substr(i, 3)));
		return true;
		}
	default: createMsg(NACK);
			 return false;
	}
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

string ClientMB::card2Str(const Card* c)
{
	string s;
	stringstream ss;
	ss << c->getNum();
	switch (c->getType())
	{
	case Card::NEW_RULE: s = "1" + ss.str(); break;
	case Card::KEEPER: 
		s = "2" + (c->getNum() < 10 ? "0" + ss.str() : ss.str()); break;
	case Card::GOAL:
		s = "3" + (c->getNum() < 10 ? "0" + ss.str() : ss.str()); break;
	case Card::ACTION:
		s = "4" + (c->getNum() < 10 ? "0" + ss.str() : ss.str()); break;
	default: break;
	}
	return s;
}

const Card* ClientMB::str2Card(string s)
{
	switch (s[0])
	{
	case '0':  //����������
		return _cards.getCard(atoi(s.c_str()));
	case '1':  //�¹�����
		if (s[1] == '1')      //��������
			return _cards.getCard(atoi(s.c_str()) - 108);
		else if (s[1] == '2')  //����������
			return _cards.getCard(atoi(s.c_str()) - 117);
		else if (s[1] == '3')//ץ������
			return _cards.getCard(atoi(s.c_str()) - 124);
		else if (s[1] == '4')//��������
			return _cards.getCard(atoi(s.c_str()) - 130);
		else                //�����¹���
			return _cards.getCard(atoi(s.c_str()) - 136);
	case '2':  //��������
		return _cards.getCard(atoi(s.c_str()) - 178);
	case '3':  //Ŀ����
		return _cards.getCard(atoi(s.c_str()) - 260);
	case '4':  //�ж���
		return _cards.getCard(atoi(s.c_str()) - 337);
	default:   //��������
		return _cards.getCard(0);
	}
}
