#include "ai.h"
using namespace std;

AI::AI(int n):
_ownNum(0),
_playerNum(0)
{
	stringstream ss;
	ss << n;
	_ownName = "AI" + ss.str();	
}

AI::~AI()
{
	_ownName.clear();
	_ownCards.clear();
	//_ownKeepers.clear();
	_allCardNum.clear();
	_rules.clear();
	for (int i = 0; i < _allKeepers.size();i++)
		_allKeepers[i].clear();
	_allKeepers.clear();
}

//���к��������ϲ���ã��յ�gameover��Ϣ���˳�
void AI::run()
{
	//���벢�ȵ���Ϸ
	if (!joinGame()) return;  
	//��Ϸ��ʼ��ѭ�����գ�������Ӧ��Ϣ��ֱ����Ϸ����
	MsgType m;
	vector<const Card*> cards;
	int relatedPlayer, additional;
	while (1)
	{
		if (!_mailbox.getMsg(m))  {
			cout << "�����Ϣʧ��" << endl;
			return;
		}
		else
		{
			cout << "�յ���һ����Ϣ����Ϊ" << convert(m) << endl;
			switch (m)
			{
			case RULE:
					if (!_mailbox.getMsg(m, cards)) 
						{ cout << "�����Ϣʧ��" << endl; return; }
						cout << "�յ��ڶ�����Ϣ: " << convert(m) << "\t";
					for (int i = 0; i < cards.size(); i++)
						cout << cards[i]->getType() << cards[i]->getNum() << "\t";
					cout << endl; 
				//������Ϣ
				_rules = cards; 
				break;
			case PLAY_C:
					cout << "������Ϣ�� PLAY_I" << endl; 
				//��Ӧ��Ϣ
				play(); 
				break;

			case DROP_CARD_C:
					if (!_mailbox.getMsg(m, additional))
					{
						cout << "�����Ϣʧ��" << endl; return;
					}
					cout << "�յ��ڶ�����Ϣ: " << convert(m) << "\t" << additional << endl;
					cout << "������Ϣ�� DROP_CARD_I" << endl; 
				//��Ӧ��Ϣ
				dropCard(additional); 
				break;
			case DROP_KEEPER_C:
				if (!_mailbox.getMsg(m, additional)) 
					{ cout << "�����Ϣʧ��" << endl; return; }
				cout << "�յ��ڶ�����Ϣ: " << convert(m) << "\t" << additional << endl;
				cout << "������Ϣ�� DROP_KEEPER_I" << endl; 
				//��Ӧ��Ϣ
				dropKeeper(additional);
				break;
			case DROP_RULE_C:
				if (!_mailbox.getMsg(m, additional))
					{ cout << "�����Ϣʧ��" << endl; return; }
				cout << "�յ��ڶ�����Ϣ: " << convert(m) << "\t" << additional << endl;
				cout << "������Ϣ�� DROP_RULE_I" << endl;
				//��Ӧ��Ϣ
				dropRule(additional);
				break;
			case CHOOSE_KEEPER_C:
				//��Ӧ��Ϣ
				chooseKeeper(1);
				break;
			case EXCHANGE_KEEPER_C:
				//��Ӧ��Ϣ
				chooseKeeper(2);
				break;
			case CHOOSE_PLAYER_C:
				//��Ӧ��Ϣ
				choosePlayer();
				break;
			case CHOOSE_GOAL_C:
				//��Ӧ��Ϣ
				chooseGoal();
				break;
			case GAME_OVER:
				if (!_mailbox.getMsg(m, additional)) 
				{
					cout << "�����Ϣʧ��" << endl; return;
				}
				cout << "�յ��ڶ�����Ϣ: " << convert(m) << "\t" << additional << endl; 
				break;
			case CARD_PLAYED: 
				if (additional!= _ownNum)  //�Լ�����������play�м�ȥ
					_allCardNum[additional]--;
				break;
			case CARD_DROPED: 
				if (!_mailbox.getMsg(m, cards, additional)){ cout << "�����Ϣʧ��" << endl; return; }
				cout << "�յ��ڶ�����Ϣ: " << convert(m) << "\t" << additional << endl;
				for (int i = 0; i < cards.size(); i++)
					cout << cards[i]->getType() << cards[i]->getNum() << "\t";
				cout << endl; 
				//������Ϣ
				for (int i = 0; i < cards.size();i++)
					_allCardNum[additional]--;
				break;
			case CARD_UPDATE:
				if (!_mailbox.getMsg(m, cards, additional)){ cout << "�����Ϣʧ��" << endl; return; }
				cout << "�յ��ڶ�����Ϣ: " << convert(m) << "\t" << additional << endl;
				for (int i = 0; i < cards.size(); i++)
					cout << cards[i]->getType() << cards[i]->getNum() << "\t";
				cout << endl; 
				//������Ϣ
				if (additional == 0)
				{
					//ɾ������
					for (int i = 0; i < cards.size(); i++)
					{
						for (int j = 0; j < _ownCards.size(); j++)
						{
							if (_ownCards[j] == cards[i])
							{
								vector<const Card*>::iterator it = _ownCards.begin() + j;
								_ownCards.erase(it);
								break;
							}
						}
					}	
				}
				else if (additional == 1)
				{	//�������
					for (int i = 0; i < cards.size(); i++)
						_ownCards.push_back(cards[i]);
				}
				else if (additional == 2)
					//��ȫ��������
					_ownCards = cards;
				//����������
				_allCardNum[_ownNum] = _ownCards.size();
				break;

			case ROUND_BEGIN:
				if (!_mailbox.getMsg(m, relatedPlayer, additional)) { cout << "�����Ϣʧ��" << endl; return; }
				cout << "�յ��ڶ�����Ϣ: " << convert(m) << "\t���Ϊ" << relatedPlayer << "\t������ϢΪ" << additional << endl; 
				//������Ϣ
				break;
			case CARD_NUM:
				if (!_mailbox.getMsg(m, relatedPlayer, additional)) { cout << "�����Ϣʧ��" << endl; return; }
				cout << "�յ��ڶ�����Ϣ: " << convert(m) << "\t���Ϊ" << relatedPlayer << "\t������ϢΪ" << additional << endl;
				//������Ϣ
				_allCardNum[relatedPlayer] = additional;
				break;
			case KEEPER_UPDATE:
				if (!_mailbox.getMsg(m, cards, relatedPlayer, additional)){ cout << "�����Ϣʧ��" << endl; return; }
				cout << "�յ��ڶ�����Ϣ: " << convert(m) << "\t���Ϊ" << relatedPlayer << "\t������ϢΪ" << additional << endl;
				for (int i = 0; i < cards.size(); i++)
					cout << cards[i]->getType() << cards[i]->getNum() << "\t";
				cout << endl; 
				//������Ϣ
				if (additional == 0)
				{
					//ɾ��������
					for (int i = 0; i < cards.size(); i++)
					{
						for (int j = 0; j < _allKeepers[relatedPlayer].size(); j++)
						{
							if (_allKeepers[relatedPlayer][j] == cards[i])
							{
								vector<const Card*>::iterator it = _allKeepers[relatedPlayer].begin() + j;
								_ownCards.erase(it);
								break;
							}
						}
					}

				}
				else if (additional == 1)
				{	//�������
					for (int i = 0; i < cards.size(); i++)
						_allKeepers[relatedPlayer].push_back(cards[i]);
				}
				else if (additional == 2)
					//��ȫ��������
					_allKeepers[relatedPlayer] = cards;
				break;
			default:
				return;
			}
		}
		if (m == GAME_OVER) break;
	}
}
//�������ע�Ტ������Ϸ
bool AI::joinGame()
{
	if (!_mailbox.connectServer("127.0.0.1"))  //���������������
		return false;
	MsgType m = REGISTER;
	string name;
	int playerNum;
	//�������ע��
	_mailbox.createMsg(REGISTER, _ownName);
	//�����ң��ȴ�����
	while (m != GAME_START)
	{
		if (!_mailbox.getMsg(m)) return false;
		else
		{
			if (m == ADD_PLAYER)  //��������Ϣ
			{
				if (!_mailbox.getMsg(m, playerNum, name)) return false;
				vector<const Card*> keepers;
				_allKeepers.push_back(keepers);
				if (name == _ownName)
					_ownNum = playerNum;
				_playerNum++;
			}
			else if (m == GAME_START)  //��Ϸ����
			{
				vector<const Card*> temp;
				if (!_mailbox.getMsg(m, temp)) return false;
				_ownCards = temp;  //��������
				for (int i = 0; i < _playerNum; i++)
					_allCardNum.push_back(3); //��ʼĬ��3������
			}
			else
				return false;
		}
	}
}
//���ƣ���ӦPLAY_C��Ϣ
//�������ȼ���������>�ж���>Ŀ����>������
void AI::play()
{
	vector<const Card*> cards;
	int chosen = -1;
	//ѡ��
	while (1){
		int i = rand() % _ownCards.size();
		//����Ƿ�������������
		int j = i;
		do {
			if (_ownCards[j]->getType() == Card::KEEPER)
			{
				chosen = j;
				break;
			}
			else
				j = (j - 1) % _ownCards.size();
		} while (j != i);
		if (chosen != -1) break;
		//����Ƿ����ж���
		j = i;
		do {
			if (_ownCards[j]->getType() == Card::ACTION)
			{
				chosen = j;
				break;
			}
			else 
				j = (j - 1) % _ownCards.size();
		} while (j != i);
		if (chosen != -1) break;
		//����Ƿ���Ŀ����
		j = i;
		do {
			if (_ownCards[j]->getType() == Card::ACTION)
			{
				chosen = j;
				break;
			}
			else
				j = (j - 1) % _ownCards.size();
		} while (j != i);
		if (chosen != -1) break;
		//��Ϊ�����ƣ����ѡ��һ��
		chosen = i;
		break;
	}
	//ȡ�����ƴ���
	cards.push_back(_ownCards[chosen]);
	vector<const Card*>::iterator it = _ownCards.begin() + chosen;
	_ownCards.erase(it);
	_allCardNum[_ownNum]--;
	//����Ϣ�������
	_mailbox.createMsg(PLAY_I, cards);
}
//���ƣ���ӦDROP_CARD_C��Ϣ
void AI::dropCard(int n)
{
	vector<const Card*> cards;
	int chosen;
	for (int i = 0; i < n; i++)
	{
		chosen = rand() % _allCardNum[_ownNum];
		//ȡ�����ƴ���
		cards.push_back(_ownCards[chosen]);
		vector<const Card*>::iterator it = _ownCards.begin() + chosen;
		_ownCards.erase(it);
		_allCardNum[_ownNum]--;
	}
	//����Ϣ����
	_mailbox.createMsg(DROP_CARD_I, cards);
}

//���������ӦDROP_KEEPER_C��Ϣ
//����������ǰĿ���ƶ�Ӧ��������(δʵ�֣�
void AI::dropKeeper(int n)
{
	vector<const Card*> cards;
	int chosen;
	for (int i = 0; i < n; i++)
	{
		chosen = rand() % _allKeepers[_ownNum].size();
		cards.push_back(_allKeepers[_ownNum][chosen]);
	}
	//����Ϣ����
	_mailbox.createMsg(DROP_KEEPER_I, cards);
}
//ѡ���ڳ����������ƣ���ӦCHOOSE_KEEPER_C, EXCHANGE_KEEPER_C��Ϣ
void AI::chooseKeeper(int n)
{
	vector<const Card*> cards;
	int i,j;
	//ѡ���˵�������һ��
	while (1)
	{
		i = rand() % _playerNum;
		if (i == _ownNum||_allKeepers[i].empty())  //ѡ���Լ�����ѡ�����������
			continue;
		j = rand() % _allKeepers[i].size();
		break;
	}
	cards.push_back(_allKeepers[i][j]);
	
	if (n == 2)//����ѡ�Լ���������һ��
	{
		i = rand() % _allKeepers[_ownNum].size();
		cards.push_back(_allKeepers[_ownNum][i]);
	}
	//��ѡ���������Ϣ
	_mailbox.createMsg(CHOOSE_KEEPER_I, cards);
}
//�����򣬻�ӦDROP_RULE_C��Ϣ
void AI::dropRule(int n)
{
	vector<const Card*> cards;
	vector<const Card*> temp = _rules;
	for (int i = 0; i < n; i++)
	{
		//ѡ�������
		int j = rand() % temp.size();
		cards.push_back(temp[j]);
		vector<const Card*>::iterator it = temp.begin() + j;
		temp.erase(it);
	}
	//��ѡ�������Ϣ
	_mailbox.createMsg(DROP_RULE_I, cards);
}
//ѡ��ң���ӦCHOOSE_PLAYER_C��Ϣ
void AI::choosePlayer()
{
	int chosen;
	//ѡ�����
	while (1)
	{
		chosen = rand() % _playerNum;
		if (chosen != _ownNum)
			break;
	}
	//��ѡ��ҵ���Ϣ
	_mailbox.createMsg(CHOOSE_PLAYER_I, chosen);
}

//ѡĿ�꣬��ӦCHOOSE_GOAL_C��Ϣ
void AI::chooseGoal()
{
	int chosen = _rules.size() - 1 - rand() % 2; //��Ŀ�����_rules���������
	vector<const Card*> cards;
	cards.push_back(_rules[chosen]);
	//��ѡĿ�����Ϣ
	_mailbox.createMsg(CHOOSE_GOAL_I,cards);
}
//for debugging
string convert(MsgType m)
{
	switch (m)
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
	case  CHOOSE_GOAL_C: return "CHOOSE_GOAL_C";
	}
}