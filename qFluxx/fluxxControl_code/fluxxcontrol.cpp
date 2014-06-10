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
	_playcardnum += rule.getplay();
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
	int _subValue = rule.firstgoal().getNum();
	int _winNum;
	bool _chk1 = false;
	bool _chk2 = false;
	bool _ban = true;
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
		std::vector<const Card*> _tempKeeper;
		const Card* _keep1 = cards.getCard(0);
		const Card* _keep2 = cards.getCard(0);
		const Card* _keeperban = cards.getCard(0);
		cards.getInfo(&rule.firstgoal(),_tempKeeper);
		_keep1 = _tempKeeper[0];
		if (rule.firstgoal().getNum() == 20 || rule.firstgoal().getNum() == 21) {
			_keeperban = _tempKeeper[1];
			std::cout << "##ʤ��������ֻ�� " << _keep1->getNum() << " û�� " <<  _keeperban->getNum() << "##" << std::endl;
		}
		else {
			_keep2 = _tempKeeper[1];
			std::cout << "##ʤ���������� " << _keep1->getNum() << " ���� " <<  _keep2->getNum() << "##" << std::endl;
		}
		for(unsigned int i = 0; i < players.size(); i++) {
			if (!players[i]->getkeeper().empty()) {
				std::vector<const Card*>::const_iterator j;
				for (j = players[i]->getkeeper().begin(); j != players[i]->getkeeper().end(); j++) {
					if (**j == *_keep1) {
						_chk1 = true;
					}
					if (**j == *_keep2) {
						_chk2 = true;
					}
					if (**j == *_keep2) {
						_ban = false;
					}
				}
				if (_chk1 && _chk2 && _ban) {
					std::cout << "���" << i << "ʤ��" << std::endl;
					return i;
				}
			}
			if(!_ban) {
				break;
			}
			else {
			_chk1 = false;
			_chk2 = false;
			}
		}
	}
	if (rule.isdoublegoals()) {
		std::cout << "##���ƽ׶Σ����ʤ������2##" << std::endl;
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
			std::vector<const Card*> _tempKeeper;
			const Card* _keep1 = cards.getCard(0);
			const Card* _keep2 = cards.getCard(0);
			const Card* _keeperban = cards.getCard(0);
			cards.getInfo(&rule.firstgoal(),_tempKeeper);
			_keep1 = _tempKeeper[0];
			if (rule.firstgoal().getNum() == 20 || rule.firstgoal().getNum() == 21) {
				_keeperban = _tempKeeper[1];
				std::cout << "##ʤ��������ֻ�� " << _keep1->getNum() << " û�� " <<  _keeperban->getNum() << "##" << std::endl;
			}
			else {
				_keep2 = _tempKeeper[1];
				std::cout << "##ʤ���������� " << _keep1->getNum() << " ���� " <<  _keep2->getNum() << "##" << std::endl;
			}
			for(unsigned int i = 0; i < players.size(); i++) {
				if (!players[i]->getkeeper().empty()) {
					std::vector<const Card*>::const_iterator j;
					for (j = players[i]->getkeeper().begin(); j != players[i]->getkeeper().end(); j++) {
						if (**j == *_keep1) {
							_chk1 = true;
						}
						if (**j == *_keep2) {
							_chk2 = true;
						}
						if (**j == *_keep2) {
							_ban = false;
						}
					}
					if (_chk1 && _chk2 && _ban) {
						std::cout << "���" << i << "ʤ��" << std::endl;
						return i;
					}
				}
				if(!_ban) {
					break;
				}
				else {
				_chk1 = false;
				_chk2 = false;
				}
			}//������Ҽ�����
		}//�ڶ�Ŀ��������ƥ�������
	}//�ڶ�Ŀ�����п��ܼ�����
	std::cout << "û�����ʤ��" << std::endl;
	return -1;
}
void fluxxControl::_shuffleCard() {
	std::cout << "ϴ���С���" << std::endl;
	deck = droppeddeck;
	droppeddeck.clear();
	for(unsigned int i = 0; i < deck.size(); i++) {
		const Card* tempCard = deck[i];
		int temp = rand()%deck.size();
		deck[i] = deck[temp];
		deck[temp] = tempCard;
	}
	std::cout << "ϴ�ƽ���" << std::endl;
}
void fluxxControl::_updateRules() {
	std::cout << "##���¹�����Ϣ����" << std::endl;
	msgbufCards.clear();
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
}
void fluxxControl::_settleRulecard(const Card& targetCard) {
	int _subType = targetCard.getNum()/10;//������
	int _subValue = targetCard.getNum()%10;
	switch(_subType) {
		case 1:{//�����������
			if(rule.gethandlimitation() >= 0) {
				droppeddeck.push_back(&rule.getChandlimit());
			}
			rule.sethandlimitation(_subValue);
			rule.setdrawrule(targetCard);
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
		//�㲥�¹�����������(׼��������= =+��
	std::cout << "����ǰ��bufΪ��" << std::endl;
	for (unsigned i =0; i < msgbufCards.size(); i++) {
		std::cout << msgbufCards[i]->getNum() << "\t";
	}
	std::cout << std::endl;
		msgbufMsgtype = RULE;
		msgbufCards.clear();
		_updateRules();
		for (unsigned int i = 0; i < players.size(); i++) {
			msgBox.createMsg(i,msgbufMsgtype);//֪ͨ
			msgBox.createMsg(i,msgbufMsgtype,msgbufCards);//����
		}
}
void fluxxControl::_settleKeepercard(const Card& targetCard){
	presentPlayer->addKeeper(targetCard);//������
	msgbufMsgtype = KEEPER_UPDATE;
	msgbufAdditional = 1;
	for(unsigned int i = 0; i < players.size(); i++) {
		Sleep(300);
		msgBox.createMsg(i,msgbufMsgtype);//֪ͨ
		Sleep(300);
		msgBox.createMsg(i,msgbufMsgtype,msgbufCards,clientNum,msgbufAdditional);//��Ϣ
	}
}
void fluxxControl::_settleGoalcard(const Card& targetCard) {
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
					rule.setfirstgoal(*msgbufCards[0]);
				}
				else if(*msgbufCards[0] == rule.secondgoal()) {
					droppeddeck.push_back(&rule.secondgoal());
					rule.setsecondgoal(*msgbufCards[0]);
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
	msgbufCards.clear();
	_updateRules();
	for (unsigned int i = 0; i < players.size(); i++) {
		Sleep(300);
		msgBox.createMsg(i,msgbufMsgtype);//֪ͨ
		Sleep(300);
		msgBox.createMsg(i,msgbufMsgtype,msgbufCards);//����
	}
}
void fluxxControl::_settleCard(const Card& targetCard) {
	//����������Ϣ
	std::cout << "##���ƽ׶Σ����㿨��##" << std::endl;
	Card::Type cardType = targetCard.getType();
	switch (cardType) {
	case 1:{ //������
		std::cout << "����һ�Ź���" << std::endl;
		_settleRulecard(targetCard);
		break;
	}
	case 2: { 
		std::cout << "����һ��������" << std::endl;
		_settleKeepercard(targetCard);
		break;
		}
	case 3: {//Ŀ�꿨
		std::cout << "����һ��Ŀ�꿨" << std::endl;
		_settleGoalcard(targetCard);	
		break;
	}
	case 4: {//�ж���
			std::cout << "##����һ���ж���##" << std::endl;
			actioncard(targetCard);
			break;
		}
	}
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
			dealCard(_checkCntdraw());
			presentPlayer->infoPlayer();
			msgbufCards = presentPlayer->gethand();
			msgBox.createMsg(i,msgbufMsgtype);//֪ͨ��Ϣ����
			Sleep(300);
			msgBox.createMsg(i, msgbufMsgtype, msgbufCards);//������������Ϣ
			Sleep(300);
			msgbufCards.clear();
		}
		//���ѡ��ʼ�����
		clientNum = rand()%4;
		setpresentPlayer(clientNum);
		presentState = ROUND_PREPARE;
		return;
	}
	clientNum++;
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
	//��������ĵ���
	if (rule.israndomstart()) {
		std::vector<const Card*> _temp  = presentPlayer->gethand();
		int _card = rand()%_temp.size();
		const Card* _tempcard = _temp[_card];
		msgbufCards.insert(msgbufCards.begin(),_tempcard);
	}
	//����������������
	if (presentState == DEAL_ORIGINAL_CARD) {
		return;
	}
	msgbufMsgtype = CARD_UPDATE;
	msgbufAdditional = 1;
	msgBox.createMsg(clientNum, msgbufMsgtype);
	Sleep(300);
	msgBox.createMsg(clientNum, msgbufMsgtype, msgbufCards, msgbufAdditional);
	//�㲥���Ƹ���������������
	msgbufMsgtype = CARD_NUM;
	msgbufAdditional = presentPlayer->getHandcnt();
	for (int i = (clientNum+1)%4; i != clientNum; i = (i+1)%4) {
		Sleep(300);
		msgBox.createMsg(i, msgbufMsgtype);
		Sleep(300);
		msgBox.createMsg(i, msgbufMsgtype, clientNum, msgbufAdditional);
	}
	presentState = PLAYING_CARD;
	return;
}
void fluxxControl::setpresentPlayer(int nextPlayer) {
	presentPlayer = players[nextPlayer];
}
void fluxxControl::playCard(const Card& targetCard) {
	//��ʼ����ѭ��
	std::cout << "���" << presentPlayer->getName() << "����˿���" << targetCard.getNum() << std::endl;
	//bool _isHandLimitchanged = false;
	//bool _isKeeperLimitchanged = false;
	int _winnernum;
	msgbufMsgtype = CARD_PLAYED;
		//���㣬֪ͨ��Ϣ��Ч���㲥������
	if(presentPlayer->removeHand(*msgbufCards[0])){
		msgbufMsgtype = CARD_PLAYED;
		for( unsigned int i = 0; i != players.size(); i++) {
			msgBox.createMsg(i,msgbufMsgtype);//֪ͨ
			Sleep(300);
			msgBox.createMsg(i,msgbufMsgtype,msgbufCards,clientNum);//����
			Sleep(300);
		}
		_settleCard(*msgbufCards[0]);
		presentPlayer->setConsumedcard(presentPlayer->getConsumedcard()+1);
		msgbufCards.clear();
		//3.���ʤ�����
		_winnernum = _checkWinner();
		if (_winnernum > 0) {
			for (unsigned int i = 0; i < players.size(); i++){
				msgbufMsgtype = GAME_OVER;
				msgbufAdditional = _winnernum;
				msgBox.createMsg(i,msgbufMsgtype);
				Sleep(300);
				msgBox.createMsg(i,msgbufMsgtype,msgbufAdditional);
				Sleep(300);
			}//�㲥��ʤ��Ϣ����
		}//��ʤ����������
	}//��Ч���ƴ������
	//4.������������
	if(rule.gethandlimitation() > 0) {
		for (unsigned int i = 0; i < players.size(); i++) {
			if(rule.gethandlimitation() > 0 && players[i]->gethand().size() > unsigned int(rule.gethandlimitation())) {
				msgbufMsgtype = DROP_CARD_C;
				msgbufAdditional = players[i]->gethand().size()-rule.gethandlimitation();
				msgbufCards.clear();
				msgBox.createMsg(i, msgbufMsgtype);//֪ͨ
				Sleep(300);
				msgBox.createMsg(i,msgbufMsgtype,msgbufAdditional);//����
				msgbufMsgtype = DROP_CARD_I;
				if(msgBox.getMsg(i,msgbufMsgtype,msgbufCards)) {
					for(unsigned int j = 0; j < msgbufCards.size(); j++) {
						players[i]->removeHand(*msgbufCards[j]);
						droppeddeck.push_back(msgbufCards[j]);
					}
					for(unsigned int j = 0; j < players.size(); j++) {
					//�㲥������Ϣ����������
						msgbufMsgtype = CARD_DROPED;
						msgBox.createMsg(j,msgbufMsgtype);
						Sleep(300);
						msgBox.createMsg(j,msgbufMsgtype,msgbufCards,clientNum);
						Sleep(300);
					}
				}
				msgbufCards.clear();
			}
		}
	}//�������޼�����
	if(rule.getkeeperlimitation() > 0) {
		for (unsigned int i = 0; i < players.size(); i++) {
			if (rule.getkeeperlimitation() > 0 && players[i]->getkeeper().size() > unsigned int(rule.getkeeperlimitation())) {
				msgbufMsgtype = DROP_KEEPER_C;
				msgbufCards.clear();
				msgbufAdditional = players[i]->getkeeper().size()-rule.getkeeperlimitation();
				msgBox.createMsg(i, msgbufMsgtype,msgbufAdditional);
			}
			if (msgBox.getMsg(i, msgbufMsgtype, msgbufCards)) {
				for (unsigned int j = 0; j < players.size(); j++) {
					players[i]->removeKeeper(*msgbufCards[j]);
					droppeddeck.push_back(msgbufCards[j]);
				}
				//�㲥����������Ϣ������
				for (unsigned int j = 0; j < players.size(); j++) {
					msgbufMsgtype = KEEPER_UPDATE;
					msgbufAdditional = 0;
					msgBox.createMsg(j, msgbufMsgtype);
					Sleep(300);
					msgBox.createMsg(j, msgbufMsgtype, msgbufCards, i, msgbufAdditional);
					Sleep(300);
				}
			}
			msgbufCards.clear();
		}
	}//���������޼�����
	//5.����µ����ƹ���
	if (rule.getdraw() > presentPlayer->getcntAddcard()) {
		
		dealCard(presentPlayer->getcntAddcard() - rule.getdraw());
	}
}
void fluxxControl::dropCard(int totalDrop) {
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
		//�㲥������Ϣ
		for(unsigned int i = 0; i < players.size(); i++) {
			msgbufMsgtype = CARD_DROPED;
			msgBox.createMsg(clientNum,msgbufMsgtype);
			msgBox.createMsg(i, msgbufMsgtype, msgbufCards);
		}//�㲥������Ϣ����
	}//�������ƴ������
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
			//�㲥ROUNDBEGIN������
			std::cout<< "**********�غϿ�ʼ**********" << std::endl;
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
				msgbufAdditional = 0;
				msgBox.createMsg(clientNum, msgbufMsgtype);
				msgbufMsgtype = PLAY_I;
				if(msgBox.getMsg(clientNum, msgbufMsgtype, msgbufCards)) {
					playCard(*msgbufCards[0]);
					presentPlayer->setConsumedcard(presentPlayer->getConsumedcard()+1);
				}
			}
			presentState = DROPING_CARD;
		}
		else if (presentState == DROPING_CARD) {
			std::cout << "##�����С���##" << std::endl;
			if (rule.gethandlimitation() > 0) {
				msgbufAdditional = presentPlayer->gethand().size()-rule.gethandlimitation();
				if (msgbufAdditional > 0) {
					dropCard(msgbufAdditional);
				}
			}
			std::cout << "********���غϽ���********" << std::endl;
			presentPlayer->infoPlayer();
			if(rule.isorderreverse()) {
				clientNum++;
			}
			else {
				clientNum--;
			}
			setpresentPlayer(clientNum);
			presentState = ROUND_PREPARE;
		}
		else if (presentState == GAME_END) {
		}
	}
}
fluxxControl::~fluxxControl() {
	for (unsigned int i = 0; i < players.size(); i++) {
		delete players[i];
	}
}