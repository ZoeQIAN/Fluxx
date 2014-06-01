#include "..\fluxxControl_code\fluxxcontrol.h"

extern enum gameState;

fluxxControl::fluxxControl():cards(CardLib::getLib()) {
	for(int i = 0; i < cards.getCardNum(); i++)  {
		droppeddeck.push_back(cards.getCard(i));
	}//��ʼ���ƶ�
	//��GUI��ȡ���������Ϣ��������һ�����
	int clientNum = 1;
	presentState = WaitforPlayers;
	//�ȴ�������Ҽ���
	while (presentState == WaitforPlayers) {
		if(msgBox.acceptNewClient()) {
			players.push_back(Player(msgBox.recvMsg(clientNum)));
			clientNum++;
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
}
void fluxxControl::playCard(ruleMsg msg) {
	while(presentPlayer.getConsumedcard() < msg) {
		const Card& consumedCard;
		if (presentPlayer.consumeHand(consumedCard)) {
			settleCard(consumedCard);
			presentPlayer.setConsumedcard(presentPlayer.getConsumedcard()+1));
		}
	}