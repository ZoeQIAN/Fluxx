#include "..\fluxxControl_code\fluxxcontrol.h"

extern enum gameState;
extern enum msgType;

bool msgBox::parseMsg(const msgType msgtype, int& senderNum, std::string name, std::vector<const Card&>& relatedCard);
bool msgBox::creatMsg(const int msgtype, std::string name, int playerNum, std::vector<const Card&>& relatedCard, const int additionalMsg)

fluxxControl::fluxxControl():cards(CardLib::getLib()) {
	for(int i = 0; i < cards.getCardNum(); i++)  {
		droppeddeck.push_back(cards.getCard(i));
	}//��ʼ���ƶ�
	//��GUI��ȡ���������Ϣ��������һ�����
	clientNum = 1;
	presentState = WaitforPlayers;
	//�ȴ�������Ҽ���
	while (presentState == WaitforPlayers) {
		//�ȴ���Ҽ���׶Σ����µ���Ҽ��룬����Ϣ��ת����Ϣ���㲥��Ϣ�����߸�������������Ϣ
		//msgbufMsgtype = REGISTER;
		if(msgBox.acceptNewClient()) {
			msgBox.recvMsg(clientNum);
			if(msgBox.parseMsg(msgbufMsgtype, clientNum, &msgbufString, &msgbufCards)){
				players.push_back(Player(msgbufString));
			}
			//msgbufMsgtype = ACK;
			msgBox.creatMsg(msgbufMsgtype, msgbufString, msgbufString, clientNum, &msgbufCards, msgbufAdditional);
			msgBox.sendMsg(clientNum);
			//msgbufMsgtype = ADDPLAYER;
			//broadcast new player info
			for(int i = 0; i < players.size()-1, i++) {
				msgBox.creatMsg(msgbufMsgtype, msgbufString, msgbufString, i, &msgbufCards, msgbufAdditional);
				msgBox.sendMsg(i);
				//msgbufMsgtype = ACK
				msgBox.recvMsg(i);
				if(!msgBox.parseMsg(msgbufMsgtype, clientNum, &msgbufString, &msgbufCards)) {
					//wrong message
				}
				//msgbufMsgtype = ADDPLAYER;
			}
			for(int i = 0; i< players.size()-1, i++) {
				msgbufString = players[i].getName();
				msgBox.creatMsg(msgbufMsgtype, msgbufString, clientNum, &msgbufCards, msgbufAdditional);
				msgBox.sendMsg(clientNum);
				//msgbufmSGTYPE = ACK
				msgBox.recvMsg(clientNum);
				if(!msgBox.parseMsg(msgbufMsgtype, clientNum, &msgbufString, &msgbufCards)) {
					//WRONG MESSAGE
				}
				//msgbufMsgtype = ADDPLAYER;
			}
		}
		if(clientNum == 4) {
			//���������������Ϊ4
			presentState = PlayersTurns;
		}
	}
	srand(int(time(NULL)));
	//ϴ��
	shuffleCard();
	//�ַ���ʼ����
	for(std::vector<Player>::iterator i = players.begin(); i != players.end(); i++) {
		setpresentPlayer(*i);
		dealCard(rule.draw());
		//msgbufMsgtype = GAMESTART
		clientNum = distance(players.begin(), i);
		msgBox.creatMsg(msgbufMsgtype, msgbufString, clientNum, &msgbufCards, msgbufAdditional);
		msgBox.sendMsg(clientNum);
		//msgbufMsgtype = ACK
		msgBox.recvMsg(clientNum);
		if(!msgBox.parseMsg(msgbufMsgtype, clientNum, &msgbufString, &msgbufCards)) {
				//WRONG MESSAGE
		}
	}
	//���ѡ��ʼ�����
	presentPlayer = *(players.begin()+rand()%players.size());
}
void fluxxControl::shuffleCard() {
	deck = droppeddeck;
	droppeddeck.clear();
	for(int i = 0; i < deck.size(); i++) {
		const Card& tempCard = deck[i];
		int temp = rand()%deck.size();
		deck[i] = deck[temp];
		deck[temp] = tempCard;
	}
}
void fluxxControl::dealCard(ruleMsg msg) {
	for(int i = 0; i < msg; i++) {
		if (!deck.empty()) {
			presentPlayer.addHand(deck.back());
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
void fluxxControl::playCard(ruleMsg msg) {
	while(presentPlayer.getConsumedcard() < msg) {
		//���ƣ�֪ͨ�غϿ�ʼ���ڴ�������Ϣ
		clientNum = getclientnum();
		dealCard(rule.draw());
		//msgbufMsgtype = ROUNDBEGIN;
		msgBox.creatMsg(msgbufMsgtype, msgbufString, clientNum, &msgbufCards, msgbufAdditional);
		msgBox.sendMsg(clientNum);
		//msgbufMsgtype = ACK;
		msgBox.recvMsg();
		if(!msgBox.parseMsg(msgbufMsgtype, clientNum, &msgbufString, &msgbufCards)) {
					//WRONG MESSAGE
		}
		//msgbufMsgtype = PLAY
		msgBox.recvMsg(clientNum);
		if(!msgBox.parseMsg(msgbufMsgtype, clientNum, &msgbufString, &msgbufCards)) {
					//WRONG MESSAGE
		}
		else {
			//RIGHT MESSAGE
			//���㣬֪ͨ��Ϣ��Ч���㲥������
			if(presentPlayer.comsumeHand()){
				//������Ч,�㲥������Ϣ
				for( int i = (1+clientNum)%4; i != clientNum; i++) {
					//msgbufMsgtype = CARDPLAYING;
					msgBox.creatMsg(msgbufMsgtype, msgbufString, clientNum, &msgbufCards, msgbufAdditional);
					msgBox.sendMsg(i);
					//msgbufMsgtype = ACK;
					msgBox.recvMsg(i);
					if(!msgBox.parseMsg(msgbufMsgtype, clientNum, &msgbufString, &msgbufCards)) {
						//WRONG MESSAGE
					}
				}
				settleCard(msgbufCards[0]);
				presentPlayer.setConsumedcard(presentPlayer.getConsumedcard()+1);
				//���ʤ�����
				if (checkWinner()) {
					for (int i = 0; i < players.size(); i++){
						//msgbufMsgtype = GAMEOVER;
						clientNum = i;
						msgBox.creatMsg(msgbufMsgtype, msgbufString, clientNum, &msgbufCards, msgbufAdditional);
						msgBox.sendMsg(i);
						//msgbufMsgtype = ACK;
						msgBox.recvMsg(i);
						if(!msgBox.parseMsg(msgbufMsgtype, clientNum, &msgbufString, &msgbufCards)) {
							//WRONG MESSAGE
						}
					}
				}
			}
			else ��
				//������Ч
			}
		}
	}
}
void fluxxControl::dropCard(ruleMsg msg) {
	//���ƣ�������������
	//���Ҫ������������1���ȴ�����������Ϣ
	//�㲥������Ϣ
	//msgbufMsgtype = DROPCARD;
	if (msg < 0) {
		return;//û�����ƹ���ֱ���˳�
	}
	msgbufAdditional = presentPlayer.getHandcnt()-msg;//�����Ǹ�����������
	msgBox.creatMsg(msgbufMsgtype, msgbufString, clientNum, &msgbufCards, msgbufAdditional);
	msgBox.sendMsg(clientNum);
	//msgbufMsgtype = ACK;
	msgBox.recvMsg();
	if(!msgBox.parseMsg(msgbufMsgtype, clientNum, &msgbufString, &msgbufCards)) {
				//WRONG MESSAGE
	}
	if (msgbufAdditional > 0) {
		//msgbufMsgtype = DROPCARD
		msgBox.recvMsg();
		if(!msgBox.parseMsg(msgbufMsgtype, clientNum, &msgbufString, &msgbufCards)) {
			//WRONG MESSAGE
		}
		//�㲥������Ϣ
		for(int i = 0; i < msgbufCards.size(); i++) {
			if(presentPlayer.removeHand(msgbufCards[i])) {
				droppeddeck.push_back(msgbufCards[i]);
			}
			else {
				//���ƴ���
			}
		}
		//msgbufMsgtype = ACK
		msgBox.creatMsg(msgbufMsgtype, msgbufString, clientNum, &msgbufCards, msgbufAdditional);
		msgBox.sendMsg(clientNum);
		for(int i = (clientNum+1)%4; i != clientNum; i++) {
			//msgbufmsgtype = OTHERDROPCARD
			msgBox.creatMsg(msgbufMsgtype, msgbufString, clientNum, &msgbufCards, msgbufAdditional);
			msgBox.sendMsg(i);
			//msgbufMsgtype = ACK;
			msgBox.recvMsg(i);
			if(!msgBox.parseMsg(msgbufMsgtype, clientNum, &msgbufString, &msgbufCards)) {
				//WRONG MESSAGE
			}
		}
	}
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
	//��ȫƥ���򷵻���ұ�ţ�û��ƥ�䷵��0��
	int keeperone;
	int keepertwo;
	int keeperban;
	bool chk1 = false;
	bool chk2 = false;
	bool ban = true;
	for(int i = 1; i <= players.size(); i++) {
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
	return 0;
}