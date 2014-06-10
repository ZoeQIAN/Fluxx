#include "player.h"

extern int cntPlayer;
Player::Player(std::string name = "FluxxPlayer" + cntPlayer) : playerName(name) {
	cntConsumedcard = 0;
	cntPlayer++;
}

bool Player::consumeHand(const Card& targetCard) {
	//�ҵ�����,ȷ����Ч��
	std::vector<const Card*>::const_iterator i = hand.begin();
	for(; i != hand.end(); i++) {
		if ((**i) == targetCard) {
			hand.erase(i);
		}
		return true;
	}
	return false;
}
bool Player::removeKeeper(const Card& targetCard) {
	std::vector<const Card*>::const_iterator i = keeper.begin();
	for(; i != keeper.end(); i++) {
		if ((**i) == targetCard) {
			hand.erase(i);
		}
		return true;
	}
	return false;
}
void Player::addHand(const Card& newhand) {
	hand.push_back(&newhand);
}
void Player::addKeeper(const Card& newkeeper) {
	keeper.push_back(&newkeeper);
}
bool Player::removeHand(const Card& removedhand) {
	std::vector<const Card*>::const_iterator i;
	for (i = hand.begin(); i != hand.end(); i++) {
		if ((**i) == removedhand) {
			hand.erase(i);
			return true;
		}
	}
	return false;
}
void Player::infoPlayer() {
	std::cout << "��ǰ�����Ϣ�� " << std::endl;
	std::cout << "���������" << this->getNameref() << std::endl;
	std::cout << "������ƣ�" ;
	for (unsigned int i = 0; i < this->hand.size(); i++) {
		std::cout << hand[i]->getNum() << "\t";
	}
	std::cout << "\n��������Ϣ��";
	for (unsigned int i = 0; i < this->keeper.size(); i++) {
		std::cout << keeper[i]->getNum() << "\t" ;
	}
	std::cout << "\n";
}