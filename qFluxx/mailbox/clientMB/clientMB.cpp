#include"clientMB.h"
using namespace std;

ClientMB::ClientMB():
	_cards(CardLib::getLib())
{
#ifdef WIN32
	//��ʼ��winsock  
	WSADATA wsaD;
	WSAStartup(MAKEWORD(1, 1), &wsaD);
#endif
	//��ʼ���ͻ���socket  
	clientSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
}

ClientMB::~ClientMB()
{
#ifdef WIN32
	closesocket(clientSock);
	WSACleanup();
#else
	close(clientSock);
#endif
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
	char buf[BUFF_MAX];
	int index = 0;
	while (1)
	{
		string temp = s.substr(index, BUFF_MAX - 1);
		memset(buf, 0, sizeof(buf));
#ifdef WIN32
		strcpy_s(buf, temp.c_str());
#else
		strcpy(buf,temp.c_str());
#endif
		if (send(clientSock, buf, strlen(buf) + 1, 0) == -1) //���ͳ���
			return false; 
		else
		{
			index += BUFF_MAX - 1;
			if (index >= s.size())  //�ѷ����ַ�����������
				return true;
		}
	}
}

string ClientMB::recvMsg()
{
	string s;
	char buf[BUFF_MAX];
	memset(buf, 0, sizeof(buf));
	if (recv(clientSock, buf, BUFF_MAX, 0) != -1)
		s = buf;
	return s;
}
//����NACK��Ϣ
bool ClientMB::createMsg(MsgType m)  
{
	//����������������Ϣ�ַ���
	string s;
	if(m == NACK) s = "0";
	else return false;

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
		s = "1"+name;
	//������Ϣ
	if (!sendMsg(s))
		return false;
	else
		return true;
}
//����PLAY_I,DROP_CARD_I,DROP_KEEPER_I,CHOOSE_KEEPER_I,DROP_RULE_I,CHOOSE_GOAL_I��Ϣ
bool ClientMB::createMsg(MsgType m, vector<const Card*>relatedCards)
{
	//����������������Ϣ�ַ���
	string s;
	switch (m)
	{
	case PLAY_I:
		if (relatedCards.size() != 1)//δ����һ����
			return false;
		else{
			s = "2" + card2Str(relatedCards[0]);							
			break;
		}
	case DROP_CARD_I:
		if (relatedCards.size() == 0)//δ���뿨����Ϣ
			return false;
		else{
			s = "3";
			for (int i = 0; i < relatedCards.size(); i++)
				s += card2Str(relatedCards[i]);
			break;
		}
	case DROP_KEEPER_I:
		if (relatedCards.size() == 0)//δ���뿨����Ϣ
			return false;
		else{
			s = "4";
			for (int i = 0; i < relatedCards.size(); i++)
			{
				//����Ƿ�Ϊ��Ч���������Ʊ��
				if(relatedCards[i]->getType() == Card::KEEPER && 0<relatedCards[i]->getNum()&&relatedCards[i]->getNum()<19) 
					s += card2Str(relatedCards[i]);
				else
					return false;
			}
			break;
		}
	case CHOOSE_KEEPER_I:
		if (relatedCards.size() != 1 &&relatedCards.size() != 2)//δ������Ч������Ϣ
			return false;
		else{
			s = "5";
			for (int i = 0; i < relatedCards.size(); i++)
			{
				//����Ƿ�Ϊ��Ч����������
				if(relatedCards[i]->getType() == Card::KEEPER && 0<relatedCards[i]->getNum()&&relatedCards[i]->getNum()<19) 
					s += card2Str(relatedCards[i]);
				else
					return false;
			}
			break;
		}
	case DROP_RULE_I:
		if(relatedCards.size()==0)//δ������Ч������Ϣ
			return false;
		else{
			s = "6";
			for(int i = 0; i < relatedCards.size();i++)
			{
				//����Ƿ�Ϊ��Ч�Ĺ�����
				if(relatedCards[i]->getType() == Card::NEW_RULE)
					s += card2Str(relatedCards[i]);
				else 
					return false;
			}
			break;
		}
	case CHOOSE_GOAL_I:
		if(relatedCards.size()!=1)//δ������Ч������Ϣ
			return false;
		else{
			s = "7";
			//����Ƿ�Ϊ��Ч��Ŀ����
			if(relatedCards[0]->getType() == Card::GOAL)
				s += card2Str(relatedCards[0]);
			else 
				return false;
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

//����CHOOSE_PLAYER_I��Ϣ
bool ClientMB::createMsg(MsgType m, int relatedPlayer)
{
	string s;
	stringstream ss;
	if(m != CHOOSE_PLAYER_I) //��Ϣ���Ͳ�ƥ��
		return false;
	else{
		ss<<relatedPlayer;
		s = "8"+ss.str();
		//������Ϣ
		if (!sendMsg(s))
			return false;
		else
			return true;
	}
}

//����������Ϣ
bool ClientMB::getMsg(MsgType& m) 
{
	//�ȴ�������Ϣ
	string s = recvMsg();
	//������Ϣ
	switch (s[0])
	{
	case '0': m = NACK; return false;
	case '1': m = ADD_PLAYER; break;
	case '2': m = GAME_START; break;
	case '3': m = ROUND_BEGIN; break;
	case '4': m = CARD_UPDATE; break;
	case '5': m = PLAY_C; break;
	case '6': m = CARD_NUM; break;
	case '7': m = CARD_PLAYED; break;
	case '8': m = CARD_DROPED; break;
	case '9': m = RULE; break;
	case 'A': m = KEEPER_UPDATE; break;
	case 'B': m = DROP_CARD_C; break;
	case 'C': m = DROP_KEEPER_C; break;
	case 'D': m = GAME_OVER; break;
	case 'F':m = CHOOSE_PLAYER_C; break;
	case 'G': m = CHOOSE_KEEPER_C; break;
	case 'H': m = EXCHANGE_KEEPER_C; break;
	case 'I': m = DROP_RULE_C; break;
	case 'J': m = CHOOSE_GOAL_C ; break;
	default: return false;
	}
	return true;
}
//�ڴ�����ADD_PLAYER��Ϣ
bool ClientMB::getMsg(MsgType m, int& relatedPlayer, std::string& name)
{
	//�ȴ�������Ϣ
	string s = recvMsg();
	//������Ϣ
	switch (s[0])
	{
	case '0': return false;
	case '1': 
		if(m == ADD_PLAYER){
			relatedPlayer = atoi(s.substr(1,1).c_str());
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
	relatedCards.clear();
	//������Ϣ
	switch (s[0])
	{
	case '0': return false;
	case '2': if(m == GAME_START) break;
			  else {
				  createMsg(NACK);
				  return false;
			  }
	case '9': if(m == RULE) break;
			  else {
				  createMsg(NACK);
				  return false;
			  }
	default:
		//������Ϣ���ڴ���ƥ��
		createMsg(NACK); 
		return false;
	}
	for (int i = 1; i < s.size(); i += 3)
		relatedCards.push_back(str2Card(s.substr(i, 3)));
	return true;
}
//�ڴ�����DROP_CARD_C,DROP_KEEPER_C,GAME_OVER,DROP_RULE_C��Ϣ
bool ClientMB::getMsg(MsgType m, int& relatedInfo)
{
	//�ȴ�������Ϣ
	string s = recvMsg();
	//������Ϣ
	switch (s[0])
	{
	case '0': return false;
	case 'I': if(m == DROP_RULE_C)break;
			  else {
				  createMsg(NACK);
				  return false;
			  }
	case 'B': if(m == DROP_CARD_C) break;
			else {
				  createMsg(NACK);
				  return false;
			  }
	case 'C': if(m == DROP_KEEPER_C) break;
			  else{
				  createMsg(NACK);
				  return false;
			  }
	case 'D': if(m == GAME_OVER) break;
	default:  createMsg(NACK);
			  return false;
	}
	relatedInfo = atoi(s.substr(1).c_str());
	return true;
}
//�ڴ�����CARD_PLAYED,CARD_DROPED,CARD_UPDATE��Ϣ
bool ClientMB::getMsg(MsgType m, std::vector<const Card*>& relatedCards,int& relatedInfo)
{
	//�ȴ�������Ϣ
	string s = recvMsg();
	relatedCards.clear();
	//������Ϣ
	switch (s[0])
	{
	case '0': return false;
	case '4': if(m == CARD_UPDATE) break;
			  else{
				  createMsg(NACK);
				  return false;
			  }
	case '7': if(m == CARD_PLAYED) break;
			  else{
				  createMsg(NACK);
				  return false;
			  }
	case '8': if(m == CARD_DROPED) break;
	default: createMsg(NACK);
			  return false;
	}
	relatedInfo = atoi(s.substr(1,1).c_str());
	for(int i = 2; i < s.size(); i += 3)
		relatedCards.push_back(str2Card(s.substr(i, 3)));
	return true;
}
//�ڴ�����ROUND_BEGIN��CARD_NUM��Ϣ
bool ClientMB::getMsg(MsgType m, int& relatedPlayer, int& additional)
{
	//�ȴ�������Ϣ
	string s = recvMsg();
	//������Ϣ
	switch (s[0])
	{
	case '0': return false;
	case '3': if(m == ROUND_BEGIN) break;
			  else{
				  createMsg(NACK);
				  return false;
			  }
	case '6': if(m== CARD_NUM) break;
	default:createMsg(NACK);
			return false;
	}
	relatedPlayer = atoi(s.substr(1,1).c_str());
	additional = atoi(s.substr(2).c_str());
	return true;
}
//�ڴ�����KEEPER_UPDATE��Ϣ
bool ClientMB::getMsg(MsgType m, std::vector<const Card*>& relatedCards,int& relatedPlayer, int& additional)
{
	//�ȴ�������Ϣ
	string s = recvMsg();
	relatedCards.clear();
	//������Ϣ
	switch (s[0])
	{
	case '0': if( m== NACK) return false;
	case 'A': 
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