#include <iostream>
#include <sstream>
//#include <windows.h>
//#include "../fluxxControl_code/player.h"

#include "../mailbox/serverMB/serverMB.h"
#include "../mailbox/clientMB/clientMB.h"
//AI���࣬��ģʽ
class AI
{
public:
	AI(int n);
	virtual ~AI();
	void run();				//���к��������ϲ���ã��յ�gameover��Ϣ���˳�
	
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

	bool joinGame();		//�������ע�Ტ������Ϸ
	virtual void play();			//���ƣ���ӦPLAY_C��Ϣ
	virtual void dropCard(int);		//���ƣ���ӦDROP_CARD_C��Ϣ
	virtual void dropKeeper(int);   //���������ӦDROP_KEEPER_C��Ϣ
	virtual void chooseKeeper(int);    //ѡ���ڳ����������ƣ���ӦCHOOSE_KEEPER_C, EXCHANGE_KEEPER_C��Ϣ
	virtual void dropRule(int);        //�����򣬻�ӦDROP_RULE_C��Ϣ
	virtual void choosePlayer();    //ѡ��ң���ӦCHOOSE_PLAYER_C��Ϣ
	virtual void chooseGoal();      //ѡĿ�꣬��ӦCHOOSE_GOAL_C��Ϣ
};

//AI���࣬����ģʽ
class advancedAI:public AI
{
public:
	advancedAI(int n): AI(n){}
	virtual ~advancedAI();
protected:
	virtual void play();			//���ƣ���ӦPLAY_C��Ϣ
	virtual void dropCard(int);		//���ƣ���ӦDROP_CARD_C��Ϣ
	virtual void dropKeeper(int);   //���������ӦDROP_KEEPER_C��Ϣ
	virtual void chooseKeeper(int);    //ѡ���ڳ����������ƣ���ӦCHOOSE_KEEPER_C, EXCHANGE_KEEPER_C��Ϣ
	virtual void dropRule(int);        //�����򣬻�ӦDROP_RULE_C��Ϣ
	virtual void choosePlayer();    //ѡ��ң���ӦCHOOSE_PLAYER_C��Ϣ
	virtual void chooseGoal();      //ѡĿ�꣬��ӦCHOOSE_GOAL_C��Ϣ
};


std::string convert(MsgType m);