#include "..\fluxxControl_code\fluxxcontrol.h"

extern enum gameState;

//������Ϣ:�ͻ��˱�ţ���Ϣ���ͣ��йؿ��ƣ� �й���ң� ������Ϣ�� �й��ַ���
//������Ϣ:�ͻ��˱�ţ���Ϣ���ͣ� �йؿ��ƣ��й��ַ���
fluxxControl::fluxxControl():cards(CardLib::getLib()), rule(cards){
	for(int i = 0; i < cards.getCardNum(); i++)  {
		droppeddeck.push_back(cards.getCard(i));
	}//��ʼ���ƶ�
	//��GUI��ȡ���������Ϣ��������һ�����
	clientNum = 0;
	presentState = WAIT_FOR_PLAYERS;
	//�ȴ�������Ҽ���
	while (presentState == WAIT_FOR_PLAYERS) {
		//�ȴ���Ҽ���׶Σ����µ���Ҽ��룬�㲥��Ϣ�����߸�������������Ϣ
		msgbufMsgtype = ADDPLAYER;
		if(msgBox.acceptNewClient()) {
			if(msgBox.getMsg(clientNum, msgbufMsgtype, msgbufCards, msgbufString)){
				players.push_back(Player(msgbufString));
			}
			msgbufMsgtype = ACK;
			msgBox.createMsg(clientNum, msgbufMsgtype, msgbufCards);
			msgbufMsgtype = ADDPLAYER;
			//�㲥�������Ϣ
			for(int i = 0; i < players.size()-1; i++) {
				msgBox.createMsg(i,msgbufMsgtype, msgbufCards, clientNum, msgbufAdditional, msgbufString);
			}
			for(int i = 0; i < players.size()-1; i++) {
				msgBox.createMsg(clientNum, msgbufMsgtype, msgbufCards, i, msgbufAdditional, msgbufString);
			}
		}
		if(clientNum == 4) {
			//���������������Ϊ4
			presentState = DEAL_ORIGINAL_CARD;
		}
	}
	//ϴ��
	srand(int(time(NULL)));
	shuffleCard();
	//�ַ���ʼ����
	for(int i = 0; i < players.size(); i++) {
		setpresentPlayer(players[i]);
		dealCard();
		msgbufMsgtype = GAMESTART;
		msgBox.createMsg(clientNum, msgbufMsgtype, msgbufCards);
		msgbufCards.clear();
	}
	//���ѡ��ʼ�����
	presentPlayer = *(players.begin()+rand()%players.size());
}
void fluxxControl::shuffleCard() {
	deck = droppeddeck;
	droppeddeck.clear();
	for(int i = 0; i < deck.size(); i++) {
		Card* tempCard = deck[i];
		int temp = rand()%deck.size();
		deck[i] = deck[temp];
		deck[temp] = tempCard;
	}
}
void fluxxControl::dealCard() {
	int _dealcardnum = 0;
	bool _isleast = true;
	//1.����������
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
		for(int i = (getclientnum(presentPlayer)+1)%4; i != getclientnum(presentPlayer); i = (i+1)%4) {
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
	//2.��ʼ����
	for(int i = 0; i < _dealcardnum; i++) {
		if (!deck.empty()) {
			presentPlayer.addHand(*deck.back());
			msgbufCards.push_back(deck.back());
			deck.pop_back();
		}
		else {
			shuffleCard();
			i--;
		}
	}
}
void fluxxControl::setpresentPlayer(Player& p) {
	presentPlayer = p;
	clientNum = getclientnum(p);
}
void fluxxControl::playCard() {
	//1.�����������
	int _playcardnum = 0;
	bool _ismost = true;
	for(int i = (getclientnum(presentPlayer)+1)%4; i != getclientnum(presentPlayer); i = (i+1)%4) {
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
	//2.����������ƣ�֪ͨ��һغϿ�ʼ
	if (rule.israndomstart()) {
		msgbufCards = presentPlayer.gethand();
		int _card = rand()%msgbufCards.size();
		Card* _tempcard = msgbufCards[_card];
		msgbufCards.insert(msgbufCards.begin(),_tempcard);
		msgbufAdditional = 1;
		//clientNum = getclientnum(presentPlayer);
		msgbufMsgtype = DRAW;
		msgBox.createMsg(clientNum, msgbufMsgtype, msgbufCards, 0, msgbufAdditional);
	}
	else {
		msgbufCards = presentPlayer.gethand();
		msgbufAdditional = 0;
		//clientNum = getclientnum(presentPlayer);
		msgbufMsgtype = DRAW;
		msgBox.createMsg(clientNum, msgbufMsgtype, msgbufCards);
	}
	//��ʼ����ѭ��
	bool _isHandLimitchanged = false;
	bool _isKeeperLimitchanged = false;
	int _winnernum;
	while(presentPlayer.getConsumedcard() < _playcardnum && !presentPlayer.gethand().empty()) {
		_isHandLimitchanged = false;
		_isKeeperLimitchanged = false;
		//���ƣ�֪ͨ�غϿ�ʼ���ڴ�������Ϣ
		//clientNum = getclientnum(presentPlayer);
		msgbufMsgtype = ROUNDBEGIN;
		msgBox.createMsg(clientNum,msgbufMsgtype,msgbufCards);
		msgbufMsgtype = PLAY;
		if(!msgBox.getMsg(clientNum, msgbufMsgtype,msgbufCards, msgbufString)) {
					//������
		}
		else {
			//���㣬֪ͨ��Ϣ��Ч���㲥������
			if(presentPlayer.removeHand(*msgbufCards[0])){
				msgbufMsgtype = ACK;
				msgBox.createMsg(clientNum,msgbufMsgtype,msgbufCards);
				//������Ч,�㲥������Ϣ
				msgbufMsgtype = CARDPLAYING;
				for( int i = (1+clientNum)%4; i != clientNum; i++) {
					msgBox.createMsg(i,msgbufMsgtype,msgbufCards);
				}
				settleCard(*msgbufCards[0]);
				presentPlayer.setConsumedcard(presentPlayer.getConsumedcard()+1);
				//3.���ʤ�����
				_winnernum = checkWinner();
				if (_winnernum > 0) {
					for (int i = 0; i < players.size(); i++){
						msgbufMsgtype = GAMEOVER;
						msgbufAdditional = _winnernum;
						msgBox.createMsg(i,msgbufMsgtype,msgbufCards,0,msgbufAdditional);
					}//�㲥��ʤ��Ϣ����
				}//��ʤ����������
			}//��Ч���ƴ������
			else {
				//������Ч
			}//��Ч���ƴ������
		}//�յ�������Ϣ����
		//4.������������
		if(_isHandLimitchanged) {
			for (int i = 0; i < players.size(); i++) {
				if(players[i].gethand().size > rule.gethandlimitation()) {
					msgbufMsgtype = DROPCARD;
					msgbufAdditional = players[i].gethand().size()-rule.gethandlimitation();
					msgBox.createMsg(i,msgbufMsgtype,msgbufCards,0,msgbufAdditional);
					msgbufMsgtype = DROPCARD;
					msgBox.getMsg(i,msgbufMsgtype,msgbufCards,msgbufString);
					for(int j = 0; j < msgbufCards.size(); j++) {
						players[i].removeHand(*msgbufCards[j]);
						droppeddeck.push_back(msgbufCards[j]);
					}
					for(int j = 0; j < players.size(); j++) {
						msgbufMsgtype = DROPCARD;
						msgBox.createMsg(j, msgbufMsgtype, msgbufCards);
					}
				}
				msgbufCards.clear();
			}
		}//�������޼�����
		if(_isKeeperLimitchanged) {
			for (int i = 0; i < players.size(); i++) {
				if (players[i].getkeeper().size() < rule.getkeeperlimitation()) {
					msgbufMsgtype = DROPKEEPER;
					msgbufAdditional = players[i].getkeeper().size()-rule.getkeeperlimitation();
					msgBox.createMsg(i, msgbufMsgtype,msgbufCards);
					msgBox.getMsg(i, msgbufMsgtype, msgbufCards, msgbufString);
				}
				for (int j = 0; j < players.size(); j++) {
					players[i].removeKeeper(*msgbufCards[j]);
					droppeddeck.push_back(msgbufCards[j]);
				}
				for (int j = 0; j < players.size(); j++) {
					msgbufMsgtype = DROPKEEPER;
					msgBox.createMsg(j, msgbufMsgtype, msgbufCards);
				}
			}
			msgbufCards.clear();
		}//���������޼�����
	}//һ����ҳ�����������
}
void fluxxControl::dropCard() {
	//1.����Ӧ����������
	int _dropcardnum = 0;
	_dropcardnum = presentPlayer.getHandcnt() - rule.gethandlimitation();
	if (_dropcardnum < 0) {
		return;
	}
	msgbufMsgtype = DROPCARD;
	msgbufAdditional = _dropcardnum;
	clientNum = getclientnum(presentPlayer);
	msgBox.createMsg(clientNum, msgbufMsgtype, msgbufCards, 0, msgbufAdditional);
	msgbufMsgtype = DROPCARD;
	if(!msgBox.getMsg(clientNum, msgbufMsgtype, msgbufCards, msgbufString)) {
				//����������Ϣ
	}
	if (msgbufAdditional > 0) {
		msgbufMsgtype = DROPCARD;
		if(!msgBox.getMsg(clientNum,msgbufMsgtype,msgbufCards,msgbufString)) {
			//WRONG MESSAGE
		}
		//����������Ϣ
		for(int i = 0; i < msgbufCards.size(); i++) {
			if(presentPlayer.removeHand(*msgbufCards[i])) {
				droppeddeck.push_back(msgbufCards[i]);
			}
			else {
				//���ƴ���
			}
		}
		msgbufMsgtype = ACK;
		msgBox.createMsg(clientNum,msgbufMsgtype,msgbufCards);
		//�㲥������Ϣ
		for(int i = (clientNum+1)%4; i != clientNum; i++) {
			//msgbufmsgtype = OTHERDROPCARD
			msgBox.createMsg(i, msgbufMsgtype, msgbufCards);
			msgbufMsgtype = ACK;
			if(!msgBox.getMsg(clientNum,msgbufMsgtype,msgbufCards,msgbufString)) {
				//WRONG MESSAGE
			}
		}//�㲥������Ϣ����
	}//�������ƴ������
}
void fluxxControl::settleCard(const Card& targetCard) {
	//����������Ϣ
	int firstIndex = targetCard.getfirstIndex();
	int secondIndex = targetCard.getsecondIndex();
	int thirdIndex = targetCard.getthirdIndex();
	switch (thirdIndex) {
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
int fluxxControl::checkWinner() {
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
void fluxxControl::stagecontrol() {

}