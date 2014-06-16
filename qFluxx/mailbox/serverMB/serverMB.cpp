#include "serverMB.h"
using namespace std;

ServerMB::ServerMB():
_clientNum(0),
_cards(CardLib::getLib())
{
#ifdef WIN32
	//��ʼ��winsock   
	WSADATA wsaD;
	WSAStartup(MAKEWORD(1, 1), &wsaD);
#endif
	//��ʼ������������socket
	serverSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	// ��������ַ�ͼ����׽���  
	struct sockaddr_in servAddr;   //������ַ   
	servAddr.sin_family = AF_INET;
	servAddr.sin_port = htons(PORT); //ָ���˿�  
	servAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	bind(serverSock, (struct sockaddr *)&servAddr, sizeof(servAddr));
	//��������  
	listen(serverSock, CLIENT_MAX);   //���ͬʱ����4������
}

ServerMB::~ServerMB()
{
#ifdef WIN32
	for (int i = 0; i < _clientNum; i++)
		closesocket(clientSock[i]);
	closesocket(serverSock);
	WSACleanup();
#else
	for(int i = 0; i < _clientNum; i++)
		close(clientSock[i]);
	close(serverSock);
#endif
}

bool ServerMB::sendMsg(int num,string s)
{
	if (num >= _clientNum)   //��������ȷ
		return false;
	char buf[BUFF_MAX];
	unsigned int index = 0;
	while (1)
	{
		string temp = s.substr(index, BUFF_MAX - 1);
		memset(buf, 0, sizeof(buf));
#ifdef WIN32
		strcpy_s(buf, temp.c_str());
#else
		strcpy(buf, temp.c_str());
#endif
		if (send(clientSock[num], buf, strlen(buf) + 1, 0) == -1) //���ͳ���
			return false;
		else
		{
			index += BUFF_MAX - 1;
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
	char buf[BUFF_MAX];
	memset(buf, 0, sizeof(buf));
	int recvLen = 0;
	if (recv(clientSock[num], buf, BUFF_MAX, 0) != -1)
		s = buf;
	return s;
}

bool ServerMB::acceptNewClient()
{
	if (_clientNum == CLIENT_MAX)
		return false;

    sockaddr_in clientAddr;
    int len = sizeof(sockaddr);
    int newClient = accept(serverSock, (sockaddr*)&clientAddr, &len);
    if (newClient != -1)
	{
		clientSock.push_back(newClient);
		_clientNum++;
		return true;
	}
	else
		return false;
}
//����ACK��NACK��Ϣ
/*
bool ServerMB::getMsg(int playerNum, MsgType& m)
{
	//�ȴ�������Ϣ
	string s = recvMsg(playerNum);
	//������Ϣ
	switch (s[0])
	{
	case '1': m = ACK; break;
	case '2': m = NACK; break;
	default: return false;
	}
	return true;
}*/

//�ڴ�����REGISTER��Ϣ
bool ServerMB::getMsg(int playerNum, MsgType m, string& name)
{
	//�ȴ�������Ϣ
	string s = recvMsg(playerNum);
cout<<"/////////////////////////\nserverMB: �յ���Ϣ "<< s<<endl<<"/////////////////////////\n";
	//������Ϣ
	switch (s[0])
	{
	case '0': return false;
	case '1':
		if (m == REGISTER){
			name = s.substr(1);
			return true;
		}
	default:
		//������Ϣ���ڴ���ƥ��
		createMsg(playerNum,NACK);
		return false;
	}
}
//�ڴ�����PLAY_I,DROP_CARD_I,DROP_KEEPER_I,CHOOSE_KEEPER_I,DROP_RULE_I,CHOOSE_GOAL_I��Ϣ
bool ServerMB::getMsg(int playerNum, MsgType m, vector<const Card*>& relatedCards)
{
	//�ȴ�������Ϣ
	string s = recvMsg(playerNum);
cout<<"/////////////////////////\nserverMB: �յ���Ϣ "<< s<<endl<<"/////////////////////////\n";
	//������Ϣ
	relatedCards.clear();
	switch (s[0])
	{
	case '0': return false;
	case '2': if (m == PLAY_I) break;
			  else {
				  createMsg(playerNum,NACK);
				  return false;
			  }
	case '3': if (m == DROP_CARD_I) break;
			  else {
				  createMsg(playerNum, NACK);
				  return false;
			  }
	case '4': if (m == DROP_KEEPER_I) break;
			  else {
				  createMsg(playerNum, NACK);
				  return false;
			  }
	case '5':if (m == CHOOSE_KEEPER_I) break;
			 else {
				 createMsg(playerNum, NACK);
				 return false;
			 }
	case '6':if (m == DROP_RULE_I) break;
			 else {
				 createMsg(playerNum, NACK);
				 return false;
			 }
	case '7':if (m == CHOOSE_GOAL_I) break;
	default:
		//������Ϣ���ڴ���ƥ��
		createMsg(playerNum, NACK);
		return false;
	}
	for (unsigned int i = 1; i < s.size(); i += 3)
		relatedCards.push_back(str2Card(s.substr(i, 3)));
	return true;
}
//�ڴ�����CHOOSE_PLAYER_I��Ϣ
bool ServerMB::getMsg(int playerNum, MsgType m, int& relatedPlayer)
{
	//�ȴ�������Ϣ
	string s = recvMsg(playerNum);
cout<<"/////////////////////////\nserverMB: �յ���Ϣ "<< s<<endl<<"/////////////////////////\n";
	//������Ϣ
	switch (s[0])
	{
	case '0': return false;
	case '8':
		if (m == CHOOSE_PLAYER_I){
			relatedPlayer = atoi(s.substr(1).c_str());
			return true;
		}
	default:
		//������Ϣ���ڴ���ƥ��
		createMsg(playerNum, NACK);
		return false;
	}
}

//����������Ϣ
bool ServerMB::createMsg(int playerNum, MsgType m) 
{
	//����������������Ϣ�ַ���
	string s;
	switch (m)
	{
	case NACK: s = "0"; break;
	case ADD_PLAYER: s = "1"; break;
	case GAME_START: s = "2"; break;
	case ROUND_BEGIN: s = "3"; break;
	case CARD_UPDATE: s = "4"; break;
	case PLAY_C: s = "5"; break;
	case CARD_NUM: s = "6"; break;
	case CARD_PLAYED: s = "7"; break;
	case CARD_DROPED: s = "8"; break;
	case RULE: s = "9"; break;
	case KEEPER_UPDATE: s = "A"; break;
	case DROP_CARD_C: s = "B"; break;
	case DROP_KEEPER_C: s = "C"; break;
	case GAME_OVER: s = "D"; break;
	//case CARD_STOLEN: s = "E"; break;
	case CHOOSE_PLAYER_C:s = "F"; break;
	case CHOOSE_KEEPER_C: s = "G"; break;
	case EXCHANGE_KEEPER_C: s = "H"; break;
	case DROP_RULE_C: s = "I"; break;
	case CHOOSE_GOAL_C: s = "J"; break;
	default: return false;
	}
	//������Ϣ
	if (!sendMsg(playerNum, s))
		return false;
	else
		return true;
}
//����ADD_PLAYER��Ϣ
bool ServerMB::createMsg(int playerNum, MsgType m, int relatedPlayer, std::string name)
{
	//����������������Ϣ�ַ���
	string s;
	stringstream ss;
	if (m != ADD_PLAYER||name == "" || relatedPlayer < 0 || relatedPlayer>3) //��Ϣ���ʹ��󣬻�δ������ȷ���ֻ���
			return false;
	else
	{
		ss << relatedPlayer;
		s = "1" + ss.str() + name;
	}
	//������Ϣ
	if (!sendMsg(playerNum, s))
		return false;
	else
		return true;
}
//����GAME_START,RULE��Ϣ
bool ServerMB::createMsg(int playerNum, MsgType m, std::vector<const Card*> relatedCards)
{
	//����������������Ϣ�ַ���
	string s;
	stringstream ss;
	switch (m)
	{
	case GAME_START:
		if (relatedCards.size() != 3) //δ�����ʼ����������Ϣ
			return false;
		else{
			s = "2";
			break;
		}
	case RULE:
		if (relatedCards.size() <2)//δ���뵱ǰ�������Ч��Ϣ
			return false;
		for (unsigned int i = 0; i < relatedCards.size(); i++)  //����Ƿ���ľ�Ϊ�����ƺ�Ŀ����
		{
			Card::Type type = relatedCards[i]->getType();
			if (type != Card::BASIC_RULE&&type != Card::NEW_RULE&&type != Card::GOAL)
				return false;
		}
		s = "9";
		break;
	/*case CARD_STOLEN:
		if (relatedCards.size() != 1) return false;
		s = "E";
		break;
	*/
	default: return false;
	}
	for (unsigned int i = 0; i < relatedCards.size(); i++)
		s += card2Str(relatedCards[i]);
	//������Ϣ
	if (!sendMsg(playerNum, s))
		return false;
	else
		return true;
}
//����DROP_CARD_C,DROP_KEEPER_C,GAME_OVER,DROP_RULE_C��Ϣ
bool ServerMB::createMsg(int playerNum, MsgType m, int relatedInfo) 
{
	//����������������Ϣ�ַ���
	string s;
	stringstream ss;
	switch (m)
	{
	case DROP_CARD_C:
		if (relatedInfo < 0)  //δ����������������Ч��Ϣ
			return false;
		else{
			s = "B";
			break;
		}
	case DROP_KEEPER_C:
		if (relatedInfo<= 0)  //δ��������������������Ч��Ϣ
			return false;
		else{
			s = "C";
			break;
		}
	case GAME_OVER:
		if (relatedInfo<0 || relatedInfo>3) //��ұ���쳣
			return false;
		else{
			s = "D";
			break;
		}
	case DROP_RULE_C:
		if (relatedInfo < 1) return false;
		else{
			s = "I";
			break;
		}
	default: return false;
	}
	ss << relatedInfo;
	s += ss.str();
	//������Ϣ
	if (!sendMsg(playerNum, s))
		return false;
	else
		return true;
}
//����CARD_PLAYED,CARD_DROPED,CARD_UPDATE��Ϣ
bool ServerMB::createMsg(int playerNum, MsgType m, std::vector<const Card*> relatedCards, int relatedInfo) 
{
	//����������������Ϣ�ַ���
	string s;
	stringstream ss;
	switch (m)
	{
	case CARD_PLAYED:
		if (relatedInfo<0 || relatedInfo>3 || relatedCards.size() != 1) //��ұ���쳣��δ������Ƶ���Ч��Ϣ
			return false;
		else{
			s = "7";
			break;
		}
	case CARD_DROPED:
		if (relatedInfo<0 || relatedInfo>3 || relatedCards.size() == 0) //��ұ���쳣��δ������Ƶ���Ч��Ϣ
			return false;
		else{
			s = "8";
			break;
		}
	case CARD_UPDATE:
		if (relatedInfo != 0 && relatedInfo != 1 && relatedInfo != 2) //δ������Ч��־λ
			return false;
		else{
			s = "4";
			break;
		}
	default: return false;
	}
	ss << relatedInfo;
	s += ss.str();
	for (unsigned int i = 0; i < relatedCards.size(); i++)
		s += card2Str(relatedCards[i]);
	//������Ϣ
	if (!sendMsg(playerNum, s))
		return false;
	else
		return true;
}
//����ROUND_BEGIN��CARD_NUM��Ϣ
bool ServerMB::createMsg(int playerNum, MsgType m, int relatedPlayer, int additional) 
{
	//����������������Ϣ�ַ���
	string s;
	stringstream ss;
	switch (m)
	{
	case ROUND_BEGIN:
		if (relatedPlayer<0 || relatedPlayer>3 || (additional != 0 && additional != 1)) //��ұ���쳣��δ������Ч��־λ
			return false;
		else {
			s = "3";
			break;
		}
	case CARD_NUM:
		if (additional < 0 || relatedPlayer<0 || relatedPlayer>3) //δ������Ч������������ұ���쳣
			return false;
		else {
			s = "6";
			break;
		}
	default: return false;
	}
	ss << relatedPlayer << additional;
	s += ss.str();
	//������Ϣ
	if (!sendMsg(playerNum, s))
		return false;
	else
		return true;
}
//����KEEPER_UPDATE��Ϣ
bool ServerMB::createMsg(int playerNum, MsgType m, std::vector<const Card*> relatedCards, int relatedPlayer, int additional) 
{
	//����������������Ϣ�ַ���
	string s;
	stringstream ss;
	if (m != KEEPER_UPDATE || relatedPlayer < 0 || relatedPlayer>3 || additional<0 || additional>2)  //��Ϣ���ʹ��󣬻���ұ���쳣��δ���������Ϣ
		return false;
	ss << relatedPlayer<< additional;
	s = "A" + ss.str();
	for (unsigned int i = 0; i < relatedCards.size(); i++)
		s += card2Str(relatedCards[i]);
	//������Ϣ
	if (!sendMsg(playerNum, s))
		return false;
	else
		return true;
}
/*
bool ServerMB::createMsg(int playerNum, MsgType m, vector<Card*> relatedCards, int relatedPlayer, int additional, string name)
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
			s = "00" + ss.str() + name;
			break;
		}
	case ACK:
		s = "01";
		break;
	case NACK:
		s = "02";
		break;
	case GAMESTART:
		s = "03";
		if (relatedCards.size() != 3) //δ�����ʼ����������Ϣ
			return false;
		for (int i = 0; i < 3; i++)
			s += card2Str(relatedCards[i]);
		break;
	case ROUNDBEGIN:
		if (relatedPlayer < 0 || relatedPlayer >3) //δ������ȷ��ұ��
			return false;
		else
		{
			ss << relatedPlayer;
			s = "04" + ss.str();
			break;
		}
	case DRAW:
		s = "05";
		if (additional != 0 && additional != 1)  //δ�����Ƿ������������Ч��Ϣ 
			return false;
		else{
			ss << additional;
			s += ss.str();
		}

		for (int i = 0; i < relatedCards.size(); i++)
			s += card2Str(relatedCards[i]);
		break;
	case DROPCARD:
		if (additional < 0)  //δ����������������Ч��Ϣ
			return false;
		else{
			ss << additional;
			s = "06" + ss.str();
			break;
		}
	case DROPKEEPER:
		if (additional <= 0)  //δ��������������������Ч��Ϣ
			return false;
		else{
			ss << additional;
			s = "07" + ss.str();
			break;
		}
	case CARDNUM:
		if (additional < 0 || relatedPlayer<0 || relatedPlayer>3) //δ������Ч������������ұ���쳣
			return false;
		else{
			ss << relatedPlayer << additional;
			s = "08" + ss.str();
			break;
		}
	case CARDPLAYING:
		if (relatedPlayer<0 || relatedPlayer>3 || relatedCards.size() != 1) //��ұ���쳣��δ������Ƶ���Ч��Ϣ
			return false;
		else{
			ss << relatedPlayer;
			s = "09" + ss.str() + card2Str(relatedCards[0]);
			break;
		}
	case GAMEOVER:
		if (relatedPlayer<0 || relatedPlayer>3) //��ұ���쳣
			return false;
		else{
			ss << relatedPlayer;
			s = "10" + ss.str();
			break;
		}
	case RULE:
		if (relatedCards.size() == 0)//δ���뵱ǰ�������Ч��Ϣ
			return false;
		for (int i = 0; i < relatedCards.size(); i++)  //����Ƿ���ľ�Ϊ�����ƺ�Ŀ����
		{
			Card::Type type = relatedCards[i]->getType();
			if (type != Card::BASIC_RULE&&type != Card::NEW_RULE&&type != Card::GOAL)
				return false;
		}

		s = "11";
		for (int i = 0; i < relatedCards.size(); i++)
			s += card2Str(relatedCards[i]);
		break;
	case KEEPERUPDATE:
		if (relatedPlayer < 0 || relatedPlayer>3 || additional<0 || additional>2)  //��ұ���쳣��δ���������Ϣ
			return false;
		ss << additional;
		s = "12" + ss.str();
		for (int i = 0; i < relatedCards.size(); i++)
			s += card2Str(relatedCards[i]);
		break;
	default:return false;
	}


}*/

//��server�յ����ַ�����Ϣ�õ���Ӧ�Ŀ�����Ϣ(s����ȷ����MB��client��֤���˴����ټ�⣩
const Card* ServerMB::str2Card(string s)
{
	switch (s[0])
	{
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

string ServerMB::card2Str(const Card* c)
{
	string s;
	stringstream ss;
	ss << c->getNum();
	switch (c->getType())
	{
	case Card::BASIC_RULE: s = "00" + ss.str(); break;
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
