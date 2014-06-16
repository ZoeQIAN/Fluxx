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
			switch (m)
			{
			case RULE:
				if (!_mailbox.getMsg(m, cards)) 
					{ cout << "�����Ϣʧ��" << endl; return; }
				//������Ϣ
				_rules = cards; 
				break;
			case PLAY_C:
				//��Ӧ��Ϣ
				play(); 
				break;
			case DROP_CARD_C:
				if (!_mailbox.getMsg(m, additional)){
					cout << "�����Ϣʧ��" << endl; return;
				}
				//��Ӧ��Ϣ
				dropCard(additional); 
				break;
			case DROP_KEEPER_C:
				if (!_mailbox.getMsg(m, additional)) 
					{ cout << "�����Ϣʧ��" << endl; return; }
				//��Ӧ��Ϣ
				dropKeeper(additional);
				break;
			case DROP_RULE_C:
				if (!_mailbox.getMsg(m, additional))
					{ cout << "�����Ϣʧ��" << endl; return; }
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
				if (!_mailbox.getMsg(m, additional)) {
					cout << "�����Ϣʧ��" << endl;return;
				}
				break;
			case CARD_PLAYED: 
				if (!_mailbox.getMsg(m, cards,additional)) { cout << "�����Ϣʧ��" << endl;return; }
				if (additional!= _ownNum)  //�Լ�����������play�м�ȥ
					_allCardNum[additional]--;
				break;
			case CARD_DROPED: 
				if (!_mailbox.getMsg(m, cards, additional)){ cout << "�����Ϣʧ��" << endl; return; }
				//������Ϣ
				if(additional != _ownNum){
					for (int i = 0; i < cards.size();i++)
						_allCardNum[additional]--;
				}
				break;
			case CARD_UPDATE:
				if (!_mailbox.getMsg(m, cards, additional)){ cout << "�����Ϣʧ��" << endl; return; }
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
				//������Ϣ
				break;
			case CARD_NUM:
				if (!_mailbox.getMsg(m, relatedPlayer, additional)) { cout << "�����Ϣʧ��" << endl; return; }
				//������Ϣ
				_allCardNum[relatedPlayer] = additional;
				break;
			case KEEPER_UPDATE:
				if (!_mailbox.getMsg(m, cards, relatedPlayer, additional)){ cout << "�����Ϣʧ��" << endl; return; }
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
	//if (!_mailbox.connectServer("192.168.0.112"))  //���������������
		{
			cout<<"���ӷ�����ʧ��"<<endl;
			return false;
	}
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
				if (!_mailbox.getMsg(m, playerNum, name))
					return false;
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
	return true;
}
//���ƣ���ӦPLAY_C��Ϣ
void AI::play()
{
	int chosen;
	vector<const Card*> cards;
	//���ѡ��
	chosen = rand()%_allCardNum[_ownNum];
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
void AI::dropKeeper(int n)
{
	vector<const Card*> cards;
	vector<const Card*> temp =  _allKeepers[_ownNum];
	int chosen;
	for (int i = 0; i < n; i++)
	{
		chosen = rand() % temp.size();
		cards.push_back(temp[chosen]);
		vector<const Card*>::iterator it = temp.begin() + chosen;
		temp.erase(it);
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
		int j = -1;
		while(j == -1)
		{
			//ѡ�������
			j = rand() % temp.size();
			if(temp[j] ->getType() == Card::BASIC_RULE||temp[j] ->getType() == Card::GOAL) j = -1;
		}
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

advancedAI::~advancedAI()
{
	_ownName.clear();
	_ownCards.clear();
	_allCardNum.clear();
	_rules.clear();
	for (int i = 0; i < _allKeepers.size();i++)
		_allKeepers[i].clear();
	_allKeepers.clear();
}

//���ƣ���ӦPLAY_C��Ϣ
//ʵ�ֳ������ȼ���������>���Ŀ����>��������>�ж���>�޹ع�����>�޹�Ŀ����>��������
void advancedAI::play()
{
	vector<const Card*> cards;
	int chosen;
	int best = 0;
	//ѡ����ǰ���������
	for(int j = 1; j < _allCardNum[_ownNum];j++)
	{
		if(betterCard(_ownCards[j],_ownCards[best]))
			best = j;
	}
	chosen = best;
	//ȡ�����ƴ���
	cards.push_back(_ownCards[chosen]);
	vector<const Card*>::iterator it = _ownCards.begin() + chosen;
	_ownCards.erase(it);
	_allCardNum[_ownNum]--;
	//����Ϣ����
	_mailbox.createMsg(PLAY_I, cards);
}
//���ƣ���ӦDROP_CARD_C��Ϣ
//���Ƶ��������ȼ�
void advancedAI::dropCard(int n)
{
	vector<const Card*> cards;
	int chosen;
	int worst;
	for (int i = 0; i < n; i++)
	{
		worst = 0;
		//ѡ����ǰ���������
		for(int j = 1; j < _allCardNum[_ownNum];j++)
		{
			if(betterCard(_ownCards[worst],_ownCards[j]))
				worst = j;
		}
		chosen = worst;
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
//�����������������
void advancedAI::dropKeeper(int n)
{
	vector<const Card*> cards;
	vector<const Card*> temp =  _allKeepers[_ownNum];
	int chosen;
	for (int i = 0; i < n; i++)
	{
		chosen = -1;
		//����ѡһ�޹�������
		int m = n = rand()%temp.size();
		do{
			if(!isRelatedKeeper(temp[n]))  
			{
				chosen = n;
				break;
			}
			else
				n = (n-1)%temp.size();
		}while(n!= m);
		//�޷���Ҫ�������������ѡ��
		if(chosen == -1)
			chosen = rand() % temp.size();
		//����ѡ�Ƽ�������ƶ�
		cards.push_back(temp[chosen]);
		vector<const Card*>::iterator it = temp.begin() + chosen;
		temp.erase(it);
	}
	//����Ϣ����
	_mailbox.createMsg(DROP_KEEPER_I, cards);
}

//ѡ���ڳ����������ƣ���ӦCHOOSE_KEEPER_C, EXCHANGE_KEEPER_C��Ϣ
//ѡ���˵����������ѡ��ǰĿ���ƶ�Ӧ��������; ѡ�Լ��������������ѡ��ǰĿ���ƶ�Ӧ�������� 
void advancedAI::chooseKeeper(int n)
{
	vector<const Card*> cards;
	int i = -1,j = -1;

	//�������Ƿ������������
	for(int k = 0; k < _playerNum; k++)
		if(k!= _ownNum)
		{
			for(int m = 0; m < _allKeepers[k].size();m++)
			{
				if(isRelatedKeeper(_allKeepers[k][m]))
				{
					i = k; 
					j = m;
					break;
				}
			}
			if(i != -1 && j != -1) break;
		}

	if(i == -1 && j == -1)  //��������������������������������
	{
		while (1)
		{
			i = rand() % _playerNum;
			if (i == _ownNum||_allKeepers[i].empty())  //ѡ���Լ�����ѡ�����������
				continue;
			j = rand() % _allKeepers[i].size();
			break;
		}
	}
	//����ѡ�Ƽ���ѡ��������Ĵ����ƶ�
	cards.push_back(_allKeepers[i][j]);
	
	if (n == 2)//����ѡ�Լ���������һ��
	{
		i = -1;
		int m = n = rand()%_allKeepers[_ownNum].size();
		do{
			if(!isRelatedKeeper(_allKeepers[_ownNum][i])) //��ѡ��Ϊ�޹�������
			{
				i = n;
				break;
			}
			else
				n = (n-1)%_allKeepers[_ownNum].size();
		}while(n!=m);
		//�޷���Ҫ�������������ѡ��
		if(i == -1)
			i = rand() % _allKeepers[_ownNum].size();
		cards.push_back(_allKeepers[_ownNum][i]);
	}
	//��ѡ���������Ϣ
	_mailbox.createMsg(CHOOSE_KEEPER_I, cards);
}

//�����򣬻�ӦDROP_RULE_C��Ϣ
//��������������>�޹ع���>������
void advancedAI::dropRule(int n)
{
	vector<const Card*> cards;
	vector<const Card*> temp;
	temp= _rules;
	int chosen;
	for (int i = 0; i < n; i++)
	{
		chosen = -1;
		//����Ƿ���ڸ�������
		for(int j = 0; j <temp.size();j++)
		{
			if(isRelatedRule(temp[j]) == -1)
			{
				chosen = j;
				break;
			}
		}
	
		if(chosen == -1)//�޸������ƣ�������ѡһ�޹ع���
		{
			int m = n = rand()%temp.size();
			do{
				if(isRelatedRule(temp[n]) == 0)  //�޹ع�����
				{
					chosen = n;
					break;
				}
				else
					n = (n-1)%temp.size();
			}while(n!= m);
			//�޷���Ҫ��Ĺ��������ѡ��������
			while(chosen == -1)
			{
				//ѡ�������
				chosen = rand() % temp.size();
				if(temp[chosen] ->getType() == Card::BASIC_RULE||temp[chosen] ->getType() == Card::GOAL) chosen = -1;
			}
		
		}

		//����ѡ�����������ƶ�
		cards.push_back(temp[chosen]);
		vector<const Card*>::iterator it = temp.begin() + chosen;
		temp.erase(it);
	}
	//��ѡ�������Ϣ
	_mailbox.createMsg(DROP_RULE_I, cards);
}

//ѡ��ң���ӦCHOOSE_PLAYER_C��Ϣ
//��ѡ��ǰ���������������
void advancedAI::choosePlayer()
{
	//Ѱ���������������
	int max = -1;
	for(int i = 0; i < _playerNum; i++)
	{
		if(i != _ownNum && _allCardNum[i]>max)
			max = i;
	}
	//��ѡ��ҵ���Ϣ
	_mailbox.createMsg(CHOOSE_PLAYER_I, max);
}

//ѡĿ�꣬��ӦCHOOSE_GOAL_C��Ϣ
//����ѡ����Լ��޹ص�Ŀ����
void advancedAI::chooseGoal()
{
	int chosen = _rules.size() - 1;   //��Ŀ�����_rules���������
	if(isRelatedGoal(_rules[chosen])) //��Ŀ�����Լ���أ�����һ��Ŀ��
		chosen--;                     
	vector<const Card*> cards;	
	cards.push_back(_rules[chosen]);
	//��ѡĿ�����Ϣ
	_mailbox.createMsg(CHOOSE_GOAL_I,cards);
}

//��������ʵ��
//���������>�����������>���Ŀ����>��������>�ж���>�޹ع�����>�޹�Ŀ����>��������>����������
bool advancedAI::betterCard(const Card* c1, const Card* c2)
{
	int flag1 = -1, flag2 = -1; 
	//7:��������� 6:����������� 5:���Ŀ���� 4:�������� 3:�ж��� 2:�޹ع����� 1:�޹�Ŀ���� 0:�������� -1:����������
	//����c1
	if(c1->getType() == Card::KEEPER)
	{
		if(	isRelatedKeeper(c1)) flag1 = 7;
		else flag1 = 6;
	} 
	else if(c1->getType() == Card::GOAL)
	{
		if(isRelatedGoal(c1)) flag1 = 5;
		else flag1 = 1;
	}
	else if(c1->getType() == Card::NEW_RULE)
	{
		if(isRelatedRule(c1) == 1) flag1 = 4;
		else if(isRelatedRule(c1) == -1) flag1 = 0;
		else flag1 = 2;
	}
	else if(c1->getType() == Card::ACTION)
		flag1 = 3;
	//����c2
	if(c2->getType() == Card::KEEPER)
	{
		if(	isRelatedKeeper(c2)) flag2 = 7;
		else flag2 = 6;
	} 
	else if(c2->getType() == Card::GOAL)
	{
		if(isRelatedGoal(c2)) flag2 = 5;
		else flag2 = 1;
	}
	else if(c2->getType() == Card::NEW_RULE)
	{
		if(isRelatedRule(c2) == 1) flag2 = 4;
		else if(isRelatedRule(c2) == -1) flag2 = 0;
		else flag2 = 2;
	}
	else if(c2->getType() == Card::ACTION)
		flag2 = 3;
	//ѡ����õ��Ʒ���
	if(flag1 >= flag2) return true;
	else 
		return false;
}
//����Լ������ƻ����������Ƿ������Ŀ���Ӧ��������
bool advancedAI::isRelatedGoal(const Card* c)
{
	if(c->getType() != Card::GOAL||c->getNum()>21)  //���ʹ����Ϊ��5���������10�����ơ�Ŀ��
		return false;
	CardLib& lib = CardLib::getLib();
	vector<const Card*> cards;
	lib.getInfo(c,cards);   //�õ�Ŀ���ƵĶ�Ӧ��������Ϣ
	//��������Ƿ������Ӧ������
	for(int i = 0; i != _ownCards.size(); i++)
	{
		if(_ownCards[i] == cards[0] || (c->getNum() < 19 && _ownCards[i] == cards[1])) 
			return true;
	}
	//����������Ƿ������Ӧ������
	for(int i = 0; i != _allKeepers[_ownNum].size(); i++)
	{
		if(_allKeepers[_ownNum][i] == cards[0] || (c->getNum() < 19 && _allKeepers[_ownNum][i] == cards[1])) 
			return true;
	}

	return false;
}
//����Լ������ƻ�ǰĿ�����Ƿ������������
bool advancedAI::isRelatedKeeper(const Card* c)
{
	if(c->getType() != Card::KEEPER) return false;
	CardLib& lib = CardLib::getLib();
	vector<const Card*> cards;
	//��⵱ǰĿ���Ƿ������������
	for(int j = 0;j<_rules.size();j++)
	{
		if(_rules[j]->getType() == Card::GOAL&& _rules[j]->getNum() <22) //�ҳ������Ŀ����
		{
			lib.getInfo(_rules[j],cards);
			if(c == cards[0] || (_rules[j]->getNum() < 19 && c == cards[1])) 
				return true;
		}
	}
	//��������Ƿ������Ӧ���������Ŀ��
	for(int i = 0; i < _ownCards.size(); i++)
	{
		if(_ownCards[i]->getType() == Card::GOAL&& _ownCards[i]->getNum() < 22)
		{
			lib.getInfo(_ownCards[i],cards);
			if(c == cards[0] || (_ownCards[i]->getNum() < 19 && c == cards[1])) 
				return true;
		}
	}

	return false;
}
//return -1: �������Լ������ơ������ﳬ�����ޣ����ܵ����õ�Ӱ�죩;0: �޹ع���; 1:�������Լ����ܵ���Ӱ�죩;-2:�������� 
int advancedAI::isRelatedRule(const Card* c)
{
	if(c->getType() != Card::NEW_RULE) //��������
		return -2;

	int num = c->getNum();
	//��⸺����
	if(num/10==1 && num%10<_allCardNum[_ownNum])//������������������
		return -1;
	if(num/10==2 && num%10<_allKeepers[_ownNum].size())//����������������������
		return -1;
	//���������
	if(num == 52)	//"���˽���"
	{
		//����Լ��Ƿ�Ϊ�����������
		int keeperMax = _allKeepers[_ownNum].size();
		int max = _ownNum;
		for(int i = 0; i < _playerNum; i++)
		{
			if(_allKeepers[i].size()>keeperMax)
			{
				max = i;
				keeperMax = _allKeepers[i].size();
			}
		}
		if(max == _ownNum) return 1;
	}
	else if(num == 51)  //"���˽���"
	{
		//����Լ��Ƿ�Ϊ������������
		int keeperMin = _allKeepers[_ownNum].size();
		int min = _ownNum;
		for(int i = 0; i < _playerNum; i++)
		{
			if(_allKeepers[i].size()<keeperMin)
			{
				min = i;
				keeperMin = _allKeepers[i].size();
			}
		}
		if(min == _ownNum) return 1;
	}
	else if(num = 56&&_allCardNum[_ownNum]== 0) //"�����ƽ���"
		return 1;

	//�޹ع���
	return 0;
}