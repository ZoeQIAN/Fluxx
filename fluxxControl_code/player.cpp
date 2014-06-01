#include "player.h"

extern int cntPlayer;
Player::Player(std::string name = "FluxxPlayer" + cntPlayer) : playerName(name) {
}

bool Player::useCard(const Card& targetCard) {
	//�ҵ�����,ȷ����Ч��
	std::vector<const Card&>::const_iterator i = hand.begin();
	for(; i != hand.end(); i++) {
		if ((*i) == targetCard) {
			hand.erase(i);
		}
		return true;
	}
	return false;
}