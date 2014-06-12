#include "..\fluxxControl_code\fluxxcontrol.h"
#include <iostream>

//extern enum gameState;
extern enum Card::Type;

fluxxControl::fluxxControl(fluxxRules& r):cards(CardLib::getLib()), rule(r){
	std::cout << "�ƶѳ�ʼ����ʼ" << std::endl;
	for(int i = 0; i < cards.getCardNum(); i++)  {
		if (i >= 2) {
			droppeddeck.push_back(cards.getCard(i));
		}
	}//��ʼ���ƶ�
	clientNum  = 0;
	std::cout << "�ƶѳ�ʼ������,�ȴ���ҿ�ʼ" << std::endl;
	std::cout << "*********��Ϸ׼���׶�*********" << std::endl;
	presentState = WAIT_FOR_PLAYERS;
}
int fluxxControl::_checkCntdraw() {
	int _dealcardnum = 0;
	bool _isleast = true;
	if (presentState == DEAL_ORIGINAL_CARD) {
		_dealcardnum = 3;
	}
	else {
		if(rule.isnohandbonus()) {
			if(presentPlayer->gethand().empty()) {
				if (rule.isinflation()) {
					_dealcardnum = 4;
				}
				else 
					_dealcardnum = 3;
			}
		}
		for(int i = (clientNum+1)%4; i != clientNum; i = (i+1)%4) {
			if(players[i]->getkeeper().size() <= presentPlayer->getkeeper().size()) {
				_isleast = false;
				break;
			}
		}
		if (_isleast) {
			if (rule.isinflation()) {
				_dealcardnum += 2;
			}
			else {
				_dealcardnum += 1;
			}
		}
		if (rule.isinflation()) {
			_dealcardnum += (rule.getdraw()+1);
		}
		else {
			_dealcardnum += rule.getdraw();
		}
	}
	return _dealcardnum;
}
int fluxxControl::_checkCntplay() {
	int _playcardnum = 0;
	if(rule.isrichBonus()) {
		bool _ismost = true;
		for(int i = (clientNum+1)%4; i != clientNum; i = (i+1)%4) {
			if(players[i]->getkeeper().size() >= presentPlayer->getkeeper().size()) {
				_ismost = false;
				break;
			}
		}
		if (_ismost) {
			if(rule.isinflation()) {
				_playcardnum += 2;
			}
			else {
				_playcardnum += 1;
			}
		}
	}
	if (rule.isinflation()) {
		_playcardnum = _playcardnum+1+rule.getplay();
	}
	else {
		_playcardnum += rule.getplay();
	}
	return _playcardnum;
}
int fluxxControl::_checkCntdrop(){
	int _dropcardnum = 0;
	_dropcardnum = presentPlayer->getHandcnt() - rule.gethandlimitation();
	if (_dropcardnum < 0) {
		return -1;
	}
	else {
		return _dropcardnum;
	}
}
int fluxxControl::_checkWinner() {
	std::cout << "##���ƽ׶Σ����ʤ������##" << std::endl;
	//����ÿ����ҵ�������뵱ǰ��Ŀ���ƽ���ƥ��
	//��ȫƥ���򷵻���ұ�ţ�û��ƥ�䷵��-1
	if (rule.firstgoal().getNum() == 0) {
		std::cout << "û�����ʤ��" << std::endl;
		return -1;
	}
	else {
		int _subValue = rule.firstgoal().getNum();
		int _winNum;
		bool _chk1 = false;
		bool _chk2 = false;
		bool _ban = false;
		std::vector<const Card*> _tempKeeper;
		const Card* _keep1;
		const Card* _keep2;
		const Card* _keeperban;
		if (_subValue == 22) {//���������
			std::cout << "##ʤ�����������������##" << std::endl;
			for (unsigned int i = 0; i < players.size(); i++) {
				if(players[i]->getKeepercnt() >= 5) {
					_chk1 = true;
					break;
				}
			}
			if (_chk1) {
				Player* _tempwinner = players[0];
				_chk2 = true;
				_winNum = 0;
				for (int i = 1; 1 < players.size(); i++) {
					if(players[i]->getKeepercnt() > _tempwinner->getKeepercnt()) {
						_tempwinner = players[i];
						_chk2 = true;
						_winNum = i;
					}
					else if (players[i]->getKeepercnt() == _tempwinner->getKeepercnt()) {
						_chk2 = false;
					}
				}
			}
			if(_chk1 && _chk2) {
				std::cout << "���" << _winNum << "ʤ��" << std::endl;
				return _winNum;
			}
			else {
				std::cout << "û�����ʤ��" << std::endl;
				return -1;
			}		
		}//��������������
		else if (_subValue == 23) {//�������
			std::cout << "##ʤ���������������##" << std::endl;
			for (unsigned int i = 0; i < players.size(); i++) {
				if(players[i]->getHandcnt() >= 10) {
					_chk1 = true;
					break;
				}
			}
			if (_chk1) {
				Player* _tempwinner = players[0];
				_chk2 = true;
				_winNum = 0;
				for (int i = 1; 1 < players.size(); i++) {
					if(players[i]->getHandcnt() > _tempwinner->getHandcnt()) {
						_tempwinner = players[i];
						_chk2 = true;
						_winNum = i;
					}
					else if (players[i]->getHandcnt() == _tempwinner->getHandcnt()) {
						_chk2 = false;
					}
				}
			}
			if(_chk1 && _chk2) {
				std::cout << "���" << _winNum << "ʤ��" << std::endl;
				return _winNum;
			}
			else {
				std::cout << "û�����ʤ��" << std::endl;
				return -1;
			}		
		}//������������
		else {//ƥ��������
			cards.getInfo(&rule.firstgoal(),_tempKeeper);
			if (_tempKeeper.empty()) {
				//����һ���쳣���
				std::cout << "�����ʤ����������Ϊ�գ�" << std::endl;
				return -1;
			}//Ŀ��1�쳣
			else {
				_keep1 = _tempKeeper[0];
				if (rule.firstgoal().getNum() == 19) {//ֻ��Ҫ��
					std::cout << "##ʤ��������ֻ��Ҫ��" << std::endl;
					for (unsigned int i = 0; i < players.size(); i++) {
						if(players[i]->getKeepercnt() == 1 && players[i]->getkeeper()[0]->getNum() == 1) {
							std::cout << "��ʤ���Ϊ" << i << std::endl;
							return i;
						}
					}
					return -1;
				}
				else if (rule.firstgoal().getNum() == 20 || rule.firstgoal().getNum() == 21) {
					_keeperban = _tempKeeper[1];
					std::cout << "##ʤ��������ֻ�� " << _keep1->getNum() << " û�� " <<  _keeperban->getNum() << "##" << std::endl;
					//�ȼ���ֹ��Ʒ
					for (unsigned int i = 0; i < players.size(); ++i) {
						if(!players[i]->getkeeper().empty()) {
							for (unsigned int j = 0; j < players[i]->getkeeper().size(); ++j) {
								if((*(players[i]->getkeeper()[j])) == *_keeperban) {
									_ban = true;
									break;
								}
							}
						}//������Ҽ�����
						if(_ban) {
							break;//һ����鵽Υ����Ʒ���˳�
						}
					}//��ֹ��Ʒ������
					if (!_ban) {
						for (unsigned int i = 0; i < players.size(); ++i) {
							if(!players[i]->getkeeper().empty()) {
								for (unsigned int j = 0; j < players[i]->getkeeper().size(); ++j) {
									if((*(players[i]->getkeeper()[j])) == *_keep1) {
										_chk1 = true;
										_winNum = i;
										break;
									}
								}
							}//������Ҽ�����
							if (_chk1) {
								std::cout << "���" << _winNum << "�����ʤ����" << std::endl;
								return _winNum;
							}//�ҵ���Ψһ�����󣬷���ʤ����ұ��
						}//�����������ң�û���˷���Ҫ��
						std::cout << "û�����ʤ��" << std::endl;
						return -1;
					}//�������û��Υ����Ʒ
					else{
						std::cout << "û�����ʤ��" << std::endl;
						return -1;
					}//���������Υ����Ʒ
				}//һ����Ҫһ�ֲ���Ҫ��ʤ�����
				else {
					_keep2 = _tempKeeper[1];
					std::cout << "##ʤ���������� " << _keep1->getNum() << " ���� " <<  _keep2->getNum() << "##" << std::endl;
					for (unsigned int i = 0; i < players.size(); ++i) {
						if (!players[i]->getkeeper().empty()) {
							for (unsigned int j = 0; j < players[i]->getkeeper().size(); ++j) {
								if (*(players[i]->getkeeper()[j]) == *_keep1) {
									_chk1 = true;
								}
								if (*(players[i]->getkeeper()[j]) == *_keep2) {
									_chk2 = true;
								}
							}
						}
						if (_chk1 && _chk1) {
							std::cout << "���" << i << "ʤ��" << std::endl;
							return i;
						}
						else {
							_chk1 = false;
							_chk2 = false;
						}
					}//������Ҷ�������
					std::cout << "û�����ʤ��" << std::endl;
					return -1;
				}//��Ҫ����ʤ����Ʒ�����
			}//Ŀ��1�����������
		}//ƥ�������������
		if (rule.isdoublegoals() && rule.secondgoal().getNum() != 0) {//�ڶ���Ŀ����Ч�Ҵ���
			std::cout << "##���ƽ׶Σ����ʤ������2##" << std::endl;
			_subValue = rule.secondgoal().getNum();
			if (_subValue == 22) {//���������
				std::cout << "##ʤ�����������������##" << std::endl;
				for (unsigned int i = 0; i < players.size(); i++) {
					if(players[i]->getKeepercnt() >= 5) {
						_chk1 = true;
						break;
					}
				}
				if (_chk1) {
					Player* _tempwinner = players[0];
					_chk2 = true;
					_winNum = 0;
					for (int i = 1; 1 < players.size(); i++) {
						if(players[i]->getKeepercnt() > _tempwinner->getKeepercnt()) {
							_tempwinner = players[i];
							_chk2 = true;
							_winNum = i;
						}
						else if (players[i]->getKeepercnt() == _tempwinner->getKeepercnt()) {
							_chk2 = false;
						}
					}
				}
				if(_chk1 && _chk2) {
					std::cout << "���" << _winNum << "ʤ��" << std::endl;
					return _winNum;
				}
				else {
					std::cout << "û�����ʤ��" << std::endl;
					return -1;
				}		
			}//��������������
			else if (_subValue == 23) {//�������
				std::cout << "##ʤ���������������##" << std::endl;
				for (unsigned int i = 0; i < players.size(); i++) {
					if(players[i]->getHandcnt() >= 10) {
						_chk1 = true;
						break;
					}
				}
				if (_chk1) {
					Player* _tempwinner = players[0];
					_chk2 = true;
					_winNum = 0;
					for (int i = 1; 1 < players.size(); i++) {
						if(players[i]->getHandcnt() > _tempwinner->getHandcnt()) {
							_tempwinner = players[i];
							_chk2 = true;
							_winNum = i;
						}
						else if (players[i]->getHandcnt() == _tempwinner->getHandcnt()) {
							_chk2 = false;
						}
					}
				}
				if(_chk1 && _chk2) {
					std::cout << "���" << _winNum << "ʤ��" << std::endl;
					return _winNum;
				}
				else {
					std::cout << "û�����ʤ��" << std::endl;
					return -1;
				}		
			}//������������
			else {
				_tempKeeper.clear();
				cards.getInfo(&rule.secondgoal(), _tempKeeper);
				if (_tempKeeper.empty()) {
					//����һ���쳣���
					std::cout << "�����ʤ����������Ϊ�գ�" << std::endl;
					return -1;
				}//Ŀ��2�쳣
				else {
					_keep1 = _tempKeeper[0];
					if (rule.firstgoal().getNum() == 19) {//ֻ��Ҫ��
						std::cout << "##ʤ��������ֻ��Ҫ��" << std::endl;
						for (unsigned int i = 0; i < players.size(); i++) {
							if(players[i]->getKeepercnt() == 1 && players[i]->getkeeper()[0]->getNum() == 1) {
								std::cout << "��ʤ���Ϊ" << i << std::endl;
								return i;
							}
						}
						return -1;
					}
					else if (rule.firstgoal().getNum() == 20 || rule.firstgoal().getNum() == 21) {
						_keeperban = _tempKeeper[1];
						std::cout << "##ʤ��������ֻ�� " << _keep1->getNum() << " û�� " <<  _keeperban->getNum() << "##" << std::endl;
						//�ȼ���ֹ��Ʒ
						for (unsigned int i = 0; i < players.size(); ++i) {
							if(!players[i]->getkeeper().empty()) {
								for (unsigned int j = 0; j < players[i]->getkeeper().size(); ++j) {
									if(*(players[i]->getkeeper()[j]) == *_keeperban) {
										_ban = true;
										break;
									}
								}
							}//������Ҽ�����
							if(_ban) {
								break;//һ����鵽Υ����Ʒ���˳�
							}
						}//��ֹ��Ʒ������
						if (!_ban) {
							for (unsigned int i = 0; i < players.size(); ++i) {
								if(!players[i]->getkeeper().empty()) {
									for (unsigned int j = 0; j < players[i]->getkeeper().size(); ++j) {
										if(*(players[i]->getkeeper()[j]) == *_keep1) {
											_chk1 = true;
											_winNum = i;
											break;
										}
									}
								}//������Ҽ�����
								if (_chk1) {
									std::cout << "���" << _winNum << "�����ʤ����" << std::endl;
									return _winNum;
								}//�ҵ���Ψһ�����󣬷���ʤ����ұ��
							}//�����������ң�û���˷���Ҫ��
							std::cout << "û�����ʤ��" << std::endl;
							return -1;
						}//�������û��Υ����Ʒ
						else{
							std::cout << "û�����ʤ��" << std::endl;
							return -1;
						}//���������Υ����Ʒ
					}//һ����Ҫһ�ֲ���Ҫ��ʤ�����						
					else {
						_keep2 = _tempKeeper[1];
						std::cout << "##ʤ���������� " << _keep1->getNum() << " ���� " <<  _keep2->getNum() << "##" << std::endl;
						for (unsigned int i = 0; i < players.size(); ++i) {
							if (!players[i]->getkeeper().empty()) {
								for (unsigned int j = 0; j < players[i]->getkeeper().size(); ++j) {
									if (*(players[i]->getkeeper()[j]) == *_keep1) {
										_chk1 = true;
									}
									if (*(players[i]->getkeeper()[j]) == *_keep2) {
										_chk2 = true;
									}
								}
							}
							if (_chk1 && _chk1) {
								std::cout << "���" << i << "ʤ��" << std::endl;
								return i;
							}
							else {
								_chk1 = false;
								_chk2 = false;
							}
						}//������Ҷ�������
						std::cout << "û�����ʤ��" << std::endl;
						return -1;
					}//Ŀ��2��Ҫ����ʤ����Ʒ�����						
				}//Ŀ��2����
			}//Ŀ��2��Ҫƥ���������ʤ������
		}
	}
}
void fluxxControl::_shuffleCard() {
	std::cout << "ϴ���С���" << std::endl;
	//deck = droppeddeck;
	int size = droppeddeck.size();
	int step = size/4;
	for (int i = 0; i < step; i++) {
		for(int j = 0; j < 4; j++) {
			deck.push_back(droppeddeck[i+step*j]);
		}
	}
	for (int i = droppeddeck.size() - deck.size(); i >0 ; i--) {
		deck.push_back(droppeddeck[droppeddeck.size()-i]);
	}
	droppeddeck.clear(); 
	/*for(unsigned int i = 0; i < deck.size(); i++) {
		const Card* tempCard = deck[i];
		int temp = rand()%deck.size();
		deck[i] = deck[temp];
		deck[temp] = tempCard;
	}*/

	std::cout << "ϴ�ƽ���" << std::endl;
}
void fluxxControl::_updateRules() {
	std::cout << "##���¹�����Ϣ##" << std::endl;
	msgbufCards.push_back(&rule.getdrawrule());
	msgbufCards.push_back(&rule.getplayrule());
	if (rule.gethandlimitation()>=0) {
		msgbufCards.push_back(&rule.getChandlimit());
	}
	if (rule.getkeeperlimitation()>=0) {
		msgbufCards.push_back(&rule.getCkeeperlimit());
	}
	if (rule.isorderreverse()) {
		msgbufCards.push_back(&rule.getCorderreverse());
	}
	if (rule.isinflation()) {
		msgbufCards.push_back(&rule.getCinflation());
	}
	if (rule.isnohandbonus()) {
		msgbufCards.push_back(&rule.getCnohandbonus());
	}
	if (rule.isrichBonus()) {
		msgbufCards.push_back(&rule.getCrichbonus());
	}
	if (rule.ispoorBonus()) {
		msgbufCards.push_back(&rule.getCpoorbonus());
	}
	if (rule.israndomstart()) {
		msgbufCards.push_back(&rule.getCrandomstart());
	}
	if (rule.isdoublegoals()) {
		msgbufCards.push_back(&rule.secondgoal());
	}
	if(!(rule.firstgoal() == *cards.getCard(0))) {
		msgbufCards.push_back(&rule.firstgoal());
	}
	if(!(rule.secondgoal() == *cards.getCard(0))) {
		msgbufCards.push_back(&rule.secondgoal());
	}
	std::cout << "���º�Ĺ���Ϊ��" << std::endl;
	for (unsigned i =0; i < msgbufCards.size(); i++) {
		std::cout << msgbufCards[i]->getNum() << "\t";
	}
	std::cout << std::endl;
	//��Ϣδ���ͣ��˴��������buf
}
void fluxxControl::_settleRulecard(const Card* targetCard) {
	int _subType = targetCard->getNum()/10;//������
	int _subValue = targetCard->getNum()%10;
	switch(_subType) {
		case 1:{//�����������
			if(rule.gethandlimitation() >= 0) {
				droppeddeck.push_back(&rule.getChandlimit());
			}
			rule.sethandlimitation(_subValue);
			rule.sethandlimrule(targetCard);
			break;
			}
		case 2: {//�������������
			if (rule.getkeeperlimitation() >= 0) {
				droppeddeck.push_back(&rule.getCkeeperlimit());
			}
			rule.setkeeperlimitation(_subValue);
			rule.setkeeperlimrule(targetCard);
			break;
			}
		case 3: {//�������
			if (rule.getdraw() > 1){
				droppeddeck.push_back(&rule.getdrawrule());
			}
			rule.setdraw(_subValue);
			rule.setdrawrule(targetCard);
			break;
			}
		case 4: {//�������
			if (rule.getplay() > 1){
				droppeddeck.push_back(&rule.getplayrule());
			}
			rule.setplay(_subValue);
			rule.setplayrule(targetCard);
			break;
			}
		case 5:{ //�������
			switch(_subValue) {
				case 1: { if (rule.getplay() > 1){ droppeddeck.push_back(&rule.getplayrule()); } 
						rule.setplay(100);
						rule.setplayrule(targetCard);
						break;
						}//��������
				case 2: { rule.setrichbonus(true); break; } //���˽���
				case 3: { rule.setpoorbonus(true); break; } //���˽���
				case 4: { rule.setinflation(true); break; } //ͨ������
				case 5: { rule.setorderreverse(true); break; } //˳��ת
				case 6: { rule.setnohandbonus(true); break; } //�����ƽ���
				case 7: { rule.setrandomstart(true); break; } //�������
				case 8: { rule.setdoublegoals(true); break; } //˫��Ŀ��
				}
			}//������������
		}//�����ƴ������
	//�㲥�¹���(׼�������ȣ�
	msgbufMsgtype = RULE;
	msgbufCards.clear();//���֮ǰΪ�ûغϳ��Ŀ���
	_updateRules();
	for (unsigned int i = 0; i < players.size(); i++) {
		msgBox.createMsg(i,msgbufMsgtype);//֪ͨ
		msgBox.createMsg(i,msgbufMsgtype,msgbufCards);//����
	}
	msgbufCards.clear();//���֮ǰΪ���µĹ�����Ϣ
	return;//�����ƽ�����ɣ����ص�settlecard
}
void fluxxControl::_settleKeepercard(const Card* targetCard){
	presentPlayer->addKeeper(*targetCard);//������
	msgbufMsgtype = KEEPER_UPDATE;
	msgbufAdditional = 1;
	for(unsigned int i = 0; i < players.size(); i++) {
		Sleep(300);
		msgBox.createMsg(i,msgbufMsgtype);//֪ͨ
		Sleep(300);
		msgBox.createMsg(i,msgbufMsgtype,msgbufCards,clientNum,msgbufAdditional);//��Ϣ
	}
	msgbufCards.clear();//���֮ǰΪ��ǰ�غϵĳ���
	return;//���ص�settlecard
}
void fluxxControl::_settleGoalcard(const Card* targetCard) {
	if (rule.isdoublegoals()) {
	//˫��Ŀ�������
		if (rule.firstgoal().getNum() == 0) {
			rule.setfirstgoal(targetCard);
		}
		else if (rule.secondgoal().getNum() == 0) {
			rule.setsecondgoal(targetCard);
		}
		else {
			msgbufMsgtype = CHOOSE_GOAL_C;
			msgBox.createMsg(clientNum, msgbufMsgtype);
			msgbufMsgtype = CHOOSE_GOAL_I;
			msgbufCards.clear();
			if(msgBox.getMsg(clientNum,msgbufMsgtype,msgbufCards)) {
				if(*msgbufCards[0] == rule.firstgoal()) {
					droppeddeck.push_back(&rule.firstgoal());
					rule.setfirstgoal(msgbufCards[0]);
				}
				else if(*msgbufCards[0] == rule.secondgoal()) {
					droppeddeck.push_back(&rule.secondgoal());
					rule.setsecondgoal(msgbufCards[0]);
				}
			}
		}
	}
	else {
		if (rule.firstgoal().getNum() == 0) {
			rule.setfirstgoal(targetCard);
		}
		else {
			droppeddeck.push_back(&rule.firstgoal());
			rule.setfirstgoal(targetCard);
		}
	}
	msgbufMsgtype = RULE;//�����㲥����Ϣ��ͬ
	msgbufCards.clear();//���֮ǰΪ��ǰ�غϳ���
	_updateRules();//cardbufû����գ����Է�����Ϣ
	for (unsigned int i = 0; i < players.size(); i++) {
		Sleep(300);
		msgBox.createMsg(i,msgbufMsgtype);//֪ͨ
		Sleep(300);
		msgBox.createMsg(i,msgbufMsgtype,msgbufCards);//����
	}
	msgbufCards.clear();//���֮ǰΪ���µĹ�����Ϣ
	return;//���ص�settlecard
}
void fluxxControl::_settleCard(const Card* targetCard) {
	//����������Ϣ
	std::cout << "##���ƽ׶Σ����㿨��##" << std::endl;
	Card::Type cardType = targetCard->getType();
	switch (cardType) {
	case 1:{ //������
		std::cout << "����һ�Ź���" << std::endl;
		_settleRulecard(targetCard);//�Ѿ��㲥��cardbuf�Ѿ����
		break;
	}
	case 2: { 
		std::cout << "����һ��������" << std::endl;
		_settleKeepercard(targetCard);//�Ѿ��㲥��cardbuf�Ѿ����
		break;
		}
	case 3: {//Ŀ�꿨
		std::cout << "����һ��Ŀ�꿨" << std::endl;
		_settleGoalcard(targetCard);	
		break;
	}
	case 4: {//�ж���
			std::cout << "##����һ���ж���##" << std::endl;
			actioncard(*targetCard);
			break;
		}
	}
	return;//���ص�playcard
}
void fluxxControl::addPlayers() {
	std::cout << "���ڴ����µ����" << std::endl;
	Player* _newplayer = new Player(msgbufString);
	this->players.push_back(_newplayer);
	std::cout << "����Ҵ������, �������ϢΪ��" << std::endl;
	players.back()->infoPlayer();
	msgbufMsgtype = ADD_PLAYER;
	//�㲥�������Ϣ
	for(unsigned int i = 0; i < players.size()-1; i++) {
		Sleep(300);
		msgBox.createMsg(i, msgbufMsgtype);//֪ͨ��Ϣ������
		Sleep(300);
		msgBox.createMsg(i, msgbufMsgtype, clientNum, msgbufString);//������������Ϣ
	}
	for(unsigned int i = 0; i < players.size(); i++) {
		msgbufString = players[i]->getName();
		msgBox.createMsg(clientNum, msgbufMsgtype);
		Sleep(300);
		msgBox.createMsg(clientNum, msgbufMsgtype, i, msgbufString);
		Sleep(300);
	}
	if(clientNum == 3) {
		//���������������Ϊ4
		std::cout << "��������Ѿ����룬��ǰ�������Ϊ" << players.size() <<  std::endl;
		presentState = DEAL_ORIGINAL_CARD;
		srand(int(time(NULL)));
		_shuffleCard();
		//�ַ���ʼ����
		std::cout << "�ַ���ʼ����" << std::endl;
		std::cout << "##��ǰ��Ϸ״̬##" << std::endl;
		for(unsigned int i = 0; i < players.size(); i++) {
			setpresentPlayer(i);
			msgbufMsgtype = GAME_START;
			msgbufCards.clear();
			dealCard(_checkCntdraw());
			presentPlayer->infoPlayer();
			msgbufCards = presentPlayer->gethand();
			msgBox.createMsg(i,msgbufMsgtype);//֪ͨ��Ϣ����
			Sleep(300);
			msgBox.createMsg(i, msgbufMsgtype, msgbufCards);//������������Ϣ
			Sleep(300);
		}
		//���ѡ��ʼ�����
		clientNum = rand()%players.size();
		setpresentPlayer(clientNum);
		presentState = ROUND_PREPARE;
		msgbufCards.clear();
		return;
	}
	clientNum++;
	msgbufString.clear();
	return;
}
void fluxxControl::dealCard(int totalDraw) {
	//��ʼ����
	std::cout << "##�����С���##" << std::endl;
	for(int i = 0; i < totalDraw; i++) {
		if (!deck.empty()) {
			presentPlayer->addHand(*deck.back());
			msgbufCards.push_back(deck.back());
			deck.pop_back();
		}
		else {
			_shuffleCard();
			i--;
		}
	}
	if (presentState == DEAL_ORIGINAL_CARD) {
		//GAME_START�����淢
		msgbufCards.clear();
		return;
	}
	else {
		//��������ĵ���
		if (rule.israndomstart()) {
			std::vector<const Card*> _temp  = presentPlayer->gethand();
			int _card = rand()%_temp.size();
			const Card* _tempcard = _temp[_card];
			msgbufCards.insert(msgbufCards.begin(),_tempcard);
		}
		//����ǰ��ҷ�����
		msgbufMsgtype = CARD_UPDATE;
		msgbufAdditional = 1;
		msgBox.createMsg(clientNum, msgbufMsgtype);
		Sleep(300);
		msgBox.createMsg(clientNum, msgbufMsgtype, msgbufCards, msgbufAdditional);
		//��������ҹ㲥��������
		msgbufMsgtype = CARD_NUM;
		msgbufAdditional = presentPlayer->getHandcnt();
		for (int i = (clientNum+1)%4; i != clientNum; i = (i+1)%4) {
			Sleep(300);
			msgBox.createMsg(i, msgbufMsgtype);
			Sleep(300);
			msgBox.createMsg(i, msgbufMsgtype, clientNum, msgbufAdditional);
		}
		presentState = PLAYING_CARD;
		msgbufCards.clear();
		return;
	}
}
void fluxxControl::setpresentPlayer(int nextPlayer) {
	presentPlayer = players[nextPlayer];
}
void fluxxControl::playCard(const Card* targetCard) {
	std::cout << "���" << presentPlayer->getName() << "����˿���" << targetCard->getNum() << std::endl;
	int _winnernum;
	msgbufMsgtype = CARD_PLAYED;
		//���㣬֪ͨ��Ϣ��Ч���㲥������
	if(presentPlayer->removeHand(*targetCard)){
		msgbufMsgtype = CARD_PLAYED;
		if(msgbufCards.empty()) {
			msgbufCards.push_back(targetCard);
		}
		for( unsigned int i = 0; i != players.size(); i++) {
			msgBox.createMsg(i,msgbufMsgtype);//֪ͨ
			Sleep(300);
			msgBox.createMsg(i,msgbufMsgtype,msgbufCards,clientNum);//����
			Sleep(300);
		}
		msgbufCards.clear();//���֮ǰΪ���͵ĳ�����Ϣ
		_settleCard(targetCard);//msgbuf�Ѿ���գ����ƽ������
		//3.���ʤ�����
		_winnernum = _checkWinner();//������
		if (_winnernum >= 0) {
			for (unsigned int i = 0; i < players.size(); i++){
				msgbufMsgtype = GAME_OVER;
				msgbufAdditional = _winnernum;
				msgBox.createMsg(i,msgbufMsgtype);
				Sleep(300);
				msgBox.createMsg(i,msgbufMsgtype,msgbufAdditional);
				Sleep(300);
				presentState = GAME_END;
			}//�㲥��ʤ��Ϣ����
			return;//������Ϸ����
		}//��ʤ����������
		presentPlayer->setConsumedcard(presentPlayer->getConsumedcard()+1);
	}//��Ч���ƴ������,���������Ѿ����ӣ�msgbufcard�Ѿ����
	else {
		std::cout << "�ó�����Ч" << std::endl;
		msgbufCards.clear();
		return;//������Ϸ���ƣ�������Ϣ��û������
	}//������Ч���������msgbufcard�Ѿ���գ�ֱ�ӷ���
	//4.������������
	if(rule.gethandlimitation() > 0) {
		for (unsigned int i = 0; i < players.size(); i++) {
			if(players[i]->gethand().size() > unsigned int(rule.gethandlimitation())) {
				msgbufMsgtype = DROP_CARD_C;
				msgbufAdditional = players[i]->gethand().size()-rule.gethandlimitation();
				msgBox.createMsg(i, msgbufMsgtype);//֪ͨ
				Sleep(300);
				msgBox.createMsg(i,msgbufMsgtype,msgbufAdditional);//����
				msgbufMsgtype = DROP_CARD_I;
				if(msgBox.getMsg(i,msgbufMsgtype,msgbufCards)) {
					for(unsigned int j = 0; j < msgbufCards.size(); j++) {
						players[i]->removeHand(*msgbufCards[j]);
						droppeddeck.push_back(msgbufCards[j]);
					}
					for(unsigned int j = 0; j < players.size(); j++) {//�㲥������Ϣ
						msgbufMsgtype = CARD_DROPED;
						msgBox.createMsg(j,msgbufMsgtype);
						Sleep(300);
						msgBox.createMsg(j,msgbufMsgtype,msgbufCards,clientNum);
						Sleep(300);
					}
				}//�յ���Ч������Ϣ
				else {
					std::cout << "���" << i << "������Ϣ��Ч" << std::endl;
					i--;
				}//û���յ���Ч������Ϣ
				msgbufCards.clear();//���֮ǰΪǰһ����������
			}
		}
	}//�������޼����ϣ�msgbuf�Ѿ����
	if(rule.getkeeperlimitation() > 0) {
		for (unsigned int i = 0; i < players.size(); i++) {
			if (rule.getkeeperlimitation() > 0 && players[i]->getkeeper().size() > unsigned int(rule.getkeeperlimitation())) {
				msgbufMsgtype = DROP_KEEPER_C;
				msgbufAdditional = players[i]->getkeeper().size()-rule.getkeeperlimitation();
				Sleep(300);
				msgBox.createMsg(i, msgbufMsgtype,msgbufAdditional);
				if (msgBox.getMsg(i, msgbufMsgtype, msgbufCards)) {
					for (unsigned int j = 0; j < players.size(); j++) {
						players[i]->removeKeeper(*msgbufCards[j]);
						droppeddeck.push_back(msgbufCards[j]);
					}
					for (unsigned int j = 0; j < players.size(); j++) {//�㲥����������Ϣ
						msgbufMsgtype = KEEPER_UPDATE;
						msgbufAdditional = 0;
						msgBox.createMsg(j, msgbufMsgtype);
						Sleep(300);
						msgBox.createMsg(j, msgbufMsgtype, msgbufCards, i, msgbufAdditional);
						Sleep(300);
						msgbufCards.clear();//���֮ǰΪǰһ���������������
					}
				}//�����������ﴦ�����
			}
		}
	}//���������޼�����,msgbuf�Ѿ����
	//5.����µ����ƹ���
	if (rule.getdraw() > presentPlayer->getcntAddcard()) {
		dealCard(rule.getdraw() - presentPlayer->getcntAddcard());
	}//�µ����ƽ�����msgbuf�Ѿ����
}
void fluxxControl::dropCard(int totalDrop) {
	std::cout << "##���##" << std::endl;
	msgbufMsgtype = DROP_CARD_C;
	msgbufAdditional = totalDrop;
	msgBox.createMsg(clientNum, msgbufMsgtype, msgbufAdditional);
	msgbufMsgtype = DROP_CARD_I;
	if(msgBox.getMsg(clientNum,msgbufMsgtype,msgbufCards)) {
		for(unsigned int i = 0; i < msgbufCards.size(); i++) {
			if(presentPlayer->removeHand(*msgbufCards[i])) {
				droppeddeck.push_back(msgbufCards[i]);
			}
		}
		msgbufMsgtype = CARD_DROPED;
		for(unsigned int i = 0; i < players.size(); i++) {//�㲥������Ϣ
			msgBox.createMsg(clientNum,msgbufMsgtype);
			msgBox.createMsg(i, msgbufMsgtype, msgbufCards);
		}//�㲥������Ϣ����
		msgbufCards.clear();//���֮ǰΪ������
	}//�������ƴ������
	else {
		std::cout << "������Ϣ����ʧ��" << std::endl;
		msgbufCards.clear();//���֮ǰΪ�����ƣ�����
	}
	return;//���أ�msgbuf�Ѿ����
}
void fluxxControl::fluxxRun() {
	while(1) {
		if (presentState == WAIT_FOR_PLAYERS) {
			msgbufMsgtype = REGISTER;
			if(msgBox.acceptNewClient()) {
				std::cout << "���µ����ע�ᣡ" << std::endl;
				msgBox.getMsg(clientNum,msgbufMsgtype,msgbufString);
				addPlayers();
			}
		}
		else if (presentState == ROUND_PREPARE) {
			//�㲥ROUNDBEGIN
			std::cout<< "**********�غϿ�ʼ**********" << std::endl;
			//msgbufCards.clear();
			presentPlayer->infoPlayer();
			msgbufMsgtype = ROUND_BEGIN;
			if (rule.israndomstart()) {
				std::cout << "����غ��������" << std::endl;
				msgbufAdditional = 1;
			}
			else {
				msgbufAdditional = 0;
			}
			for(unsigned int i = 0; i < players.size(); i++) {
				Sleep(300);
				msgBox.createMsg(i, msgbufMsgtype);
				Sleep(300);
				msgBox.createMsg(i, msgbufMsgtype, clientNum, msgbufAdditional);
			}
			presentPlayer->setcntAddcard(_checkCntdraw());
			dealCard(presentPlayer->getcntAddcard());
		}
		else if (presentState == PLAYING_CARD) {
			std::cout << "##�����С���##" << std::endl;
			presentPlayer->infoPlayer();
			while (presentPlayer->getConsumedcard() < _checkCntplay()) {
				msgbufMsgtype = PLAY_C;
				msgbufCards.clear();
				Sleep(300);
				msgBox.createMsg(clientNum, msgbufMsgtype);
				msgbufMsgtype = PLAY_I;
				if(msgBox.getMsg(clientNum, msgbufMsgtype, msgbufCards)) {
					playCard(msgbufCards[0]);//msgbuf�Ѿ���գ��������������Ѿ�����
				}
				else {
					std::cout << "���ճ�����Ϣʧ��" << std::endl;
				}
			}
			presentState = DROPING_CARD;
		}
		else if (presentState == DROPING_CARD) {
			std::cout << "##�����С���##" << std::endl;
			if (rule.gethandlimitation() >= 0) {
				msgbufAdditional = presentPlayer->gethand().size()-rule.gethandlimitation();
				if (msgbufAdditional > 0) {
					dropCard(msgbufAdditional);//msgbuf�Ѿ����
				}
			}
			std::cout << "********���غϽ���********" << std::endl;
			presentPlayer->infoPlayer();
			presentPlayer->setcntAddcard(0);
			presentPlayer->setConsumedcard(0);
			if(rule.isorderreverse()) {
				clientNum = (clientNum+1)%players.size();
			}
			else {
				clientNum = (clientNum+3)%players.size();
			}
			setpresentPlayer(clientNum);
			presentState = ROUND_PREPARE;
		}
		else if (presentState == GAME_END) {
			//��Ϸʤ����Ϣ�Ѿ��㲥
			while (1) {}
		}
	}
}
fluxxControl::~fluxxControl() {
	for (unsigned int i = 0; i < players.size(); i++) {
		delete players[i];
	}
}