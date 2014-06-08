#include "..\fluxxControl_code\fluxxcontrol.h"

extern enum gameState;
extern enum Card::Type;

fluxxControl::fluxxControl():cards(CardLib::getLib()), rule(cards), presentPlayer(players[0]){
	for(int i = 0; i < cards.getCardNum(); i++)  {
		droppeddeck.push_back(cards.getCard(i));
	}//��ʼ���ƶ�
	clientNum  = -1;
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
			if(presentPlayer.gethand().empty()) {
				if (rule.isinflation()) {
					_dealcardnum = 4;
				}
				else 
					_dealcardnum = 3;
			}
		}
		for(int i = (clientNum+1)%4; i != clientNum; i = (i+1)%4) {
			if(players[i].getkeeper().size() <= presentPlayer.getkeeper().size()) {
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
		if(players[i].getkeeper().size() >= presentPlayer.getkeeper().size()) {
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
	_dropcardnum = presentPlayer.getHandcnt() - rule.gethandlimitation();
	if (_dropcardnum < 0) {
		return -1;
	}
	else {
		return _dropcardnum;
	}
}
void fluxxControl::_shuffleCard() {
	deck = droppeddeck;
	droppeddeck.clear();
	for(int i = 0; i < deck.size(); i++) {
		const Card* tempCard = deck[i];
		int temp = rand()%deck.size();
		deck[i] = deck[temp];
		deck[temp] = tempCard;
	}
}
void fluxxControl::_settleCard(const Card& targetCard) {
	//����������Ϣ
	Card::Type cardType = targetCard.getType();
	switch (cardType) {
		case 1:{
			//������
			switch (secondIndex) {
				case 1: {
					//�����������
					if (rule.handlimitation() >=0 ) {
						//ԭ���Ѿ�����,�滻
						rule.sethandlimrule(targetCard);
					}
					rule.handlimitation(firstIndex);
					break;
				}
				case 2: {
					//�������������
					if (rule.keeperlimitiation() >= 0) {
						rule,setkeeperlimrule(targetCard);
					}
					rule.keeperlimitiation(firstIndex);
					break;
				}
				case 3: {
					//�������
					if (rule.draw() >= 0){
						rule.setdrawrule(targetCard);
					}
					rule.darw(firstIndex);
					break;
				}
				case 4: {
					//�ж������
					break;
				}
			}
			break;
		}
		case 2: {
			//������
			presentPlayer.addKeeper(targetCard);
			break;
		}
		case 3: {
			//Ŀ�꿨
			if (rule.isdoublegoals) {
				//˫��Ŀ�������
				if (rule.firstgoal().getCardNum() == 0) {
					rule.firstgoal(targetCard);
				}
				else if (rule.secondgoal().getCardNum() == 0) {
					rule.secondgoal(targetCard);
				}
				else {
					if (additionalMsg == 1) {
						rule.firstgoal(targetCard);
					}
					else {
						ruleMsg.secondgoal(targetCard);
					}
				}
			}
			else {
				ruleMsg.firstgoal(targetCard);
			}
			break;
		}
		case 4: {
			//�ж���
		}
	}
}
void fluxxControl::addPlayers() {
	players.push_back(Player(msgbufString));
	msgbufMsgtype = ADD_PLAYER;
	//�㲥�������Ϣ
	for(int i = 0; i < players.size()-1; i++) {
		msgBox.createMsg(i, msgbufMsgtype);//֪ͨ��Ϣ������
		msgBox.createMsg(i, msgbufMsgtype, clientNum, msgbufString);//������������Ϣ
	}
	for(int i = 0; i < players.size()-1; i++) {
		msgBox.createMsg(clientNum,msgbufMsgtype);
		msgBox.createMsg(clientNum, msgbufMsgtype, i, msgbufString);
	}
	if(clientNum == 3) {
		//���������������Ϊ4
		presentState = DEAL_ORIGINAL_CARD;
		//ϴ��
		srand(int(time(NULL)));
		_shuffleCard();
		//�ַ���ʼ����
		for(int i = 0; i < players.size(); i++) {
			setpresentPlayer(i);
			dealCard(_checkCntdraw());
			msgbufMsgtype = GAME_START;
			msgbufCards = presentPlayer.gethand();
			msgBox.createMsg(i,msgbufMsgtype);//֪ͨ��Ϣ����
			msgBox.createMsg(i, msgbufMsgtype, msgbufCards);//������������Ϣ
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
	for(int i = 0; i < totalDraw; i++) {
		if (!deck.empty()) {
			presentPlayer.addHand(*deck.back());
			msgbufCards.push_back(deck.back());
			deck.pop_back();
		}
		else {
			_shuffleCard();
			i--;
		}
	}
	//�㲥ROUNDBEGIN������
	msgbufMsgtype = ROUND_BEGIN;
	if (rule.israndomstart()) {
		msgbufAdditional = 1;
	}
	else {
		msgbufAdditional = 0;
	}
	for(int i = 0; i < players.size(); i++) {
		msgBox.createMsg(i, msgbufMsgtype);
		msgBox.createMsg(i, msgbufMsgtype, clientNum, msgbufAdditional);
	}
	//��������ĵ���
	if (rule.israndomstart()) {
		std::vector<const Card*> _temp  =presentPlayer.gethand();
		int _card = rand()%_temp.size();
		const Card* _tempcard = _temp[_card];
		msgbufCards.insert(msgbufCards.begin(),_tempcard);
	}
	//����������������
	msgbufMsgtype = CARD_UPDATE;
	msgbufAdditional = 1;
	msgBox.createMsg(clientNum, msgbufMsgtype, msgbufCards, msgbufAdditional);
	presentState = PLAYING_CARD;
}
void fluxxControl::setpresentPlayer(int nextPlayer) {
	presentPlayer = players[nextPlayer];
}
void fluxxControl::playCard(const Card& targetCard) {
	//��ʼ����ѭ��
	bool _isHandLimitchanged = false;
	bool _isKeeperLimitchanged = false;
	int _winnernum;
	msgbufMsgtype = CARD_PLAYED;
		//���㣬֪ͨ��Ϣ��Ч���㲥������
	if(presentPlayer.removeHand(*msgbufCards[0])){
		msgbufMsgtype = CARD_PLAYED;
		for( int i = 0; i != players.size(); i++) {
			msgBox.createMsg(i,msgbufMsgtype);//֪ͨ
			msgBox.createMsg(i,msgbufMsgtype,msgbufCards);//����
		}
		_settleCard(*msgbufCards[0]);
		presentPlayer.setConsumedcard(presentPlayer.getConsumedcard()+1);
		//3.���ʤ�����
		_winnernum = _checkWinner();
		if (_winnernum > 0) {
			for (int i = 0; i < players.size(); i++){
				msgbufMsgtype = GAME_OVER;
				msgbufAdditional = _winnernum;
				msgBox.createMsg(i,msgbufMsgtype);
				msgBox.createMsg(i,msgbufMsgtype,msgbufAdditional);
			}//�㲥��ʤ��Ϣ����
		}//��ʤ����������
	}//��Ч���ƴ������
	//4.������������
	if(_isHandLimitchanged) {
		for (int i = 0; i < players.size(); i++) {
			if(players[i].gethand().size > rule.gethandlimitation()) {
				msgbufMsgtype = DROP_CARD_C;
				msgbufAdditional = players[i].gethand().size()-rule.gethandlimitation();
				msgbufCards.clear();
				msgBox.createMsg(i, msgbufMsgtype);//֪ͨ
				msgBox.createMsg(i,msgbufMsgtype,msgbufAdditional);//����
				msgbufMsgtype = DROP_CARD_I;
				if(msgBox.getMsg(i,msgbufMsgtype,msgbufCards)) {
					for(int j = 0; j < msgbufCards.size(); j++) {
						players[i].removeHand(*msgbufCards[j]);
						droppeddeck.push_back(msgbufCards[j]);
					}
					for(int j = 0; j < players.size(); j++) {
					//�㲥������Ϣ����������
						msgbufMsgtype = CARD_DROPED;
						msgBox.createMsg(j,msgbufMsgtype);
						msgBox.createMsg(j,msgbufMsgtype,msgbufCards,clientNum);
					}
				}
				msgbufCards.clear();
			}
		}
	}//�������޼�����
	if(_isKeeperLimitchanged) {
		for (int i = 0; i < players.size(); i++) {
			if (players[i].getkeeper().size() > rule.getkeeperlimitation()) {
				msgbufMsgtype = DROP_KEEPER_C;
				msgbufCards.clear();
				msgbufAdditional = players[i].getkeeper().size()-rule.getkeeperlimitation();
				msgBox.createMsg(i, msgbufMsgtype,msgbufAdditional);
			}
			if (msgBox.getMsg(i, msgbufMsgtype, msgbufCards)) {
				for (int j = 0; j < players.size(); j++) {
					players[i].removeKeeper(*msgbufCards[j]);
					droppeddeck.push_back(msgbufCards[j]);
				}
				//�㲥����������Ϣ������
				for (int j = 0; j < players.size(); j++) {
					msgbufMsgtype = KEEPER_UPDATE;
					msgbufAdditional = 0;
					msgBox.createMsg(j, msgbufMsgtype);
					msgBox.createMsg(j, msgbufMsgtype, msgbufCards, i, msgbufAdditional);
				}
			}
			msgbufCards.clear();
		}//���������޼�����
	}
}
void fluxxControl::dropCard(int totalDrop) {
	msgbufMsgtype = DROP_CARD_C;
	msgbufAdditional = totalDrop;
	msgBox.createMsg(clientNum, msgbufMsgtype, msgbufAdditional);
	msgbufMsgtype = DROP_CARD_I;
	if(msgBox.getMsg(clientNum,msgbufMsgtype,msgbufCards)) {
		for(int i = 0; i < msgbufCards.size(); i++) {
			if(presentPlayer.removeHand(*msgbufCards[i])) {
				droppeddeck.push_back(msgbufCards[i]);
			}
		}
		//�㲥������Ϣ
		for(int i = 0; i < players.size(); i++) {
			msgbufMsgtype = CARD_DROPED;
			msgBox.createMsg(clientNum,msgbufMsgtype);
			msgBox.createMsg(i, msgbufMsgtype, msgbufCards);
		}//�㲥������Ϣ����
	}//�������ƴ������
}
int fluxxControl::_checkWinner() {
	//����ÿ����ҵ�������뵱ǰ��Ŀ���ƽ���ƥ��
	//��ȫƥ���򷵻���ұ�ţ�û��ƥ�䷵��-1
	int keeperone;
	int keepertwo;
	int keeperban;
	bool chk1 = false;
	bool chk2 = false;
	bool ban = true;
	for(int i = 0; i < players.size(); i++) {
		if (!players[i].getkeeper().empty()) {
			std::vector<const Card&>::const_interator i;
			for (i = players[i].keeper.begin(), i != keeper.end(); i++) {
				if ((*i).getCardNum() == keeperone) {
					chk1 = true;
				}
				if ((*i).getCardNum() == keepertwo) {
					chk2 = true;
				}
				if ((*i).getCardNum() == keeperban) {
					ban = false;
				}
			}
			if (chk1 && chk2 && ban) {
				return i;
			}
		}
		chk1 = false;
		chk2 = false;
	}
	if (rule.isdoublegoals()) {
		keeperone;
		keepertwo;
		keeperban;
		chk1 = false;
		chk2 = false;
		ban = true;
		for (int i = 0; i <players.size(); i++) {
			if (!players[i].getkeeper().empty()) {
				std::vector<const Card&>::const_interator i;
				for (i = players[i].keeper.begin(), i != keeper.end(); i++) {
					if ((*i).getCardNum() == keeperone) {
						chk1 = true;
					}
					if ((*i).getCardNum() == keepertwo) {
						chk2 = true;
					}
					if ((*i).getCardNum() == keeperban) {
						ban = false;
					}
				}
				if (chk1 && chk2 && ban) {
					return i;
				}
			}
		}
		chk1 = false;
		chk2 = false;
	}
	return -1;
}
void fluxxControl::fluxxRun() {
	bool _isRoundbegin = false;
	while(1) {
		if (presentState == WAIT_FOR_PLAYERS) {
			msgbufMsgtype = REGISTER;
			if(msgBox.getMsg(clientNum, msgbufMsgtype, msgbufString)) {
				addPlayers();
			}
		}
		else if (presentState == ROUND_PREPARE) {
			dealCard(_checkCntdraw());
		}
		else if (presentState == PLAYING_CARD) {
			while (presentPlayer.getConsumedcard() < _checkCntplay()) {
				msgbufMsgtype = PLAY_C;
				msgbufCards.clear();
				msgbufAdditional = 0;
				msgBox.createMsg(clientNum, msgbufMsgtype);
				msgbufMsgtype = PLAY_I;
				if(msgBox.getMsg(clientNum, msgbufMsgtype, msgbufCards)) {
					playCard(*msgbufCards[0]);
					presentPlayer.setConsumedcard(presentPlayer.getConsumedcard()+1);
				}
			}
			presentState = DROPING_CARD;
		}
		else if (presentState == DROPING_CARD) {
			msgbufAdditional = presentPlayer.gethand().size()-rule.gethandlimitation();
			if (msgbufAdditional > 0) {
				dropCard(msgbufAdditional);
			}
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