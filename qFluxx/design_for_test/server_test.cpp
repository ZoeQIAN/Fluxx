#include "clientMB.h"
using namespace std;


string convert(MsgType m);
int main()
{
	ClientMB client;
	cout << "�ͻ����ѽ���" << endl;
	if (client.connectServer("127.0.0.1"))
		cout << "�����ӵ�������" << endl;
	else
	{
		cout << "���ӳ���" << endl;
		return -1;
	}
	CardLib& lib = CardLib::getLib();

	int command;
	string s;
	int card,player,additional;
	vector<const Card*> cards;
	MsgType m;
	while(1)
	{
		cout<<"ѡ�����:\n1.CLIENT����Ϣ\t2.CLIENT����Ϣ\n3.�˳�����\n";
		cin>>command;
		if(command == 1)
		{
			cout<<"ѡ����Ϣ�����ͣ�\n1.NACK\t2.REGISTER\t3.PLAY_I\n4.DROP_CARD_I\t5.DROP_KEEPER_I\t6.CHOOSE_KEEPER_I\n7.DROP_RULE_I\t8.CHOOSE_GOAL_I\t9.CHOOSE_PLAYER_I\n";
			cin>>command;
			switch(command)
			{
			case 1: client.createMsg(NACK);break;
			case 2: cout<<"������������� "; cin >>s; client.createMsg(REGISTER,s);break;
			case 3: 
				cards.clear();
				cout<<"���뿨�����ƿ�ı�ţ�\n";cin >>card;
				cards.push_back(lib.getCard(card));
				client.createMsg(PLAY_I,s);break;
			case 4:
				cards.clear();
				cout<<"���뿨�����ƿ�ı�ţ�\n";cin >>card;
				cards.push_back(lib.getCard(card));
				client.createMsg(DROP_CARD_I,s);break;
			case 5:
				cards.clear();
				cout<<"���뿨�����ƿ�ı�ţ�\n";cin >>card;
				cards.push_back(lib.getCard(card));
				client.createMsg(DROP_KEEPER_I,s);break;
			case 6:
				cards.clear();
				cout<<"���뿨�����ƿ�ı�ţ�\n";cin >>card;
				cards.push_back(lib.getCard(card));
				client.createMsg(CHOOSE_KEEPER_I,s);break;
			case 7:
				cards.clear();
				cout<<"���뿨�����ƿ�ı�ţ�\n";cin >>card;
				cards.push_back(lib.getCard(card));
				client.createMsg(DROP_RULE_I,s);break;
			case 8:
				cards.clear();
				cout<<"���뿨�����ƿ�ı�ţ�\n";cin >>card;
				cards.push_back(lib.getCard(card));
				client.createMsg(CHOOSE_GOAL_I,s);break;
			case 9:
				cout<<"������ұ�ţ� "; cin>>player;
				client.createMsg(CHOOSE_PLAYER_I,player);break;
			default: break;
			}
		}
		else if(command == 2)
		{
			if(!client.getMsg(m)) return 1;
			else
			{
				cout<<"�յ���һ����Ϣ����Ϊ"<<convert(m)<<endl;
				switch(m)
				{
				case ADD_PLAYER: if(!client.getMsg(m,player,s)) return 1;
								 cout<<"�յ��ڶ�����Ϣ: "<<convert(m)<<"\t"<<player<<"\t"<<s<<endl;break;
				case GAME_START: 
				case RULE:
					if(!client.getMsg(m,cards)) return 1;
					cout<<"�յ��ڶ�����Ϣ: "<<convert(m)<<"\t";
					for(int i =0 ; i < cards.size(); i++)
						 cout<<cards[i]->getType()<<cards[i]->getNum()<<"\t";
					cout<<endl;break;
				case DROP_CARD_C:
				case DROP_KEEPER_C:
				case GAME_OVER:
				case DROP_RULE_C:
					if(!client.getMsg(m,additional)) return 1;
					cout<<"�յ��ڶ�����Ϣ: "<<convert(m)<<"\t"<<additional<<endl;break;
				case CARD_PLAYED:
				case CARD_DROPED:
				case CARD_UPDATE:
					if(!client.getMsg(m,cards,additional)) return 1;
					cout<<"�յ��ڶ�����Ϣ: "<<convert(m)<<"\t"<<additional<<endl;
					for(int i =0 ; i < cards.size(); i++)
						 cout<<cards[i]->getType()<<cards[i]->getNum()<<"\t";
					cout<<endl;break;
				case ROUND_BEGIN:
				case CARD_NUM:
					if(!client.getMsg(m,player,additional)) return 1;
					cout<<"�յ��ڶ�����Ϣ: "<<convert(m)<<"\t���Ϊ"<<player<<"\t������ϢΪ"<<additional<<endl;
				case KEEPER_UPDATE:
					if(!client.getMsg(m,cards,player,additional)) return 1;
					cout<<"�յ��ڶ�����Ϣ: "<<convert(m)<<"\t���Ϊ"<<player<<"\t������ϢΪ"<<additional<<endl;
					for(int i =0 ; i < cards.size(); i++)
						 cout<<cards[i]->getType()<<cards[i]->getNum()<<"\t";
					cout<<endl;break;
				}
			}
		}
		else if(command == 3) break;
		else cout<<"������������\n";
	}

	/*//client����Ϣ����
	vector<const Card*> cards;
	client.createMsg(REGISTER,"Stella");
	cout << "�ѷ��͵�һ����Ϣ:REGISTER" << endl;
	//client.createMsg(ACK);
	//cout << "�ѷ��͵ڶ�����Ϣ:ACK" << endl;
	client.createMsg(NACK);
	cout << "�ѷ��͵�������Ϣ:NACK" << endl;

	cards.push_back(lib.getCard(401));
	//if(client.createMsg(PLAY, cards))
	//cout << "�ѷ��͵�������Ϣ:PLAY" << endl;
	//else cout << "��������Ϣ���ͳ���" << endl;

	cards.push_back(lib.getCard(402));
	if (client.createMsg(DROP_CARD_I, cards))
		cout << "�ѷ��͵�������Ϣ:DROPCARD" << endl;
	else cout << "��������Ϣ���ͳ���" << endl;
	cards. clear();
	cards.push_back(lib.getCard(30));
	if (client.createMsg(DROP_KEEPER_I, cards))
		cout << "�ѷ��͵�������Ϣ:DROPKEEPER" << endl;
	else cout << "��������Ϣ���ͳ���" << endl;
	*/
	//����Ϣ
	/*client.sendMsg("From client 0 to server!");
	cout << "�ѷ���" << endl;
	cout << "FROM server�� "<<client.recvMsg() << endl;
	while (1)
	{
		string s;
		getline(cin,s);
		client.sendMsg(s);
		cout << "�ѷ���" << endl;
		cout << "FROM server�� " << client.recvMsg() << endl;
	}*/
}

string convert(MsgType m)
{
	switch(m)
	{
	case NACK: return "NACK";
	case ADD_PLAYER: return "ADD_PLAYER";
	case GAME_START: return "GAME_START";
	case ROUND_BEGIN: return "ROUND_BEGIN";
	case CARD_UPDATE: return "CARD_UPDATE";
	case PLAY_C: return "PLAY_C";
	case CARD_NUM: return "CARD_NUM";
	case CARD_PLAYED: return "CARD_PLAYED";
	case CARD_DROPED: return "CARD_DROPED";
	case RULE: return "RULE";
	case KEEPER_UPDATE: return "KEEPER_UPDATE";
	case  DROP_CARD_C: return "DROP_CARD_C";
	case  DROP_KEEPER_C: return "DROP_KEEPER_C";
	case  GAME_OVER: return "GAME_OVER";
	//case 'E': m = CARD_STOLEN: return "NACK";
	case  CHOOSE_PLAYER_C: return "CHOOSE_PLAYER_C";
	case  CHOOSE_KEEPER_C: return "CHOOSE_KEEPER_C";
	case  EXCHANGE_KEEPER_C: return "EXCHANGE_KEEPER_C";
	case  DROP_RULE_C: return "DROP_RULE_C";
	case  CHOOSE_GOAL_C : return "CHOOSE_GOAL_C";
	}
}