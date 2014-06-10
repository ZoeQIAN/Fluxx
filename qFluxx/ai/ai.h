#include <iostream>
#include <sstream>
//#include <windows.h>
//#include "../fluxxControl_code/player.h"

#include "../mailbox/serverMB/serverMB.h"
#include "../mailbox/clientMB/clientMB.h"

class AI
{
public:
	AI(int n);
	~AI();
	void run();				//���к��������ϲ���ã��յ�gameover��Ϣ���˳�
	bool joinGame();		//�������ע�Ტ������Ϸ
	void play();			//���ƣ���ӦPLAY_C��Ϣ
	void dropCard(int);		//���ƣ���ӦDROP_CARD_C��Ϣ
	void dropKeeper(int);   //���������ӦDROP_KEEPER_C��Ϣ
	void chooseKeeper(int);    //ѡ���ڳ����������ƣ���ӦCHOOSE_KEEPER_C, EXCHANGE_KEEPER_C��Ϣ
	void dropRule(int);        //�����򣬻�ӦDROP_RULE_C��Ϣ
	void choosePlayer();    //ѡ��ң���ӦCHOOSE_PLAYER_C��Ϣ
	void chooseGoal();      //ѡĿ�꣬��ӦCHOOSE_GOAL_C��Ϣ
protected:
	ClientMB _mailbox;
	std::string _ownName;
	int _ownNum;
	int _playerNum;
	std::vector<const Card*> _ownCards;
	//std::vector<const Card*> _ownKeepers;
	std::vector<int> _allCardNum;
	std::vector<std::vector<const Card*>> _allKeepers;
	std::vector<const Card*> _rules;
};

std::string convert(MsgType m);