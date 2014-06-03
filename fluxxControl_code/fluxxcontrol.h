#ifndef FLUXXCONTROL_H
#define FLUXXCONTROL_H

#include <vector>
#inlcude <string>
#include <time.h>
#include "..\fluxxControl_code\fluxxrules.h"
#include "..\cardLib_code\cardLib.h"
#include "..\mailbox\serverMB\serverMB.h"
#include "player.h"

typedef int ruleMsg;
extern enum msgType;
enum gameState{ 
	WaitforPlayers,
	PlayersTurns,
};
class fluxxControl {
public:
	fluxxControl ();
	void initrules();//��ʼ��������Ĭ�Ϲ��캯����ʹ��
	void addplayers();//�����ң���Ĭ�Ϲ��캯���������ʵ���ʱ��ʹ��
	void setpresentPlayer(Player&);
	void shuffleCard();//ϴ��
	void dealCard(ruleMsg); //����,��ڲ���Ϊ�ӹ����еõ�����Ϣ
	void playCard(ruleMsg);//���ƽ׶ο��ƺ�������ڲ���Ϊ�ӹ����еõ�����Ϣ
	void settleCard(const Card&); //���ų�������㣬��ڲ���Ϊ���Ƶ�����
	void dropCard(ruleMsg);//���ƽ׶ο��ƺ������ڲ���Ϊ�ӹ����еõ�����Ϣ
	void removeCard(Player&);//�Ƴ����ƣ���Ҫָ��Ŀ��
	int checkWinner();

	int getclientnum(Player&);
	~fluxxControl ();

private:
	//�����غϿ������
	gameState presentState;
	Player& presentPlayer;
	fluxxRules rule;
	//��Ϣ�������
	int msgbufAdditional;
	int clientNum;
	msgType msgbufMsgtype;
	std::string msgbufString;
	std::vector<const Card&> msgbufCards;
	//������Ϸ�������
	CardLib& cards;
	ServerMB msgBox;
	std::vector<Player> players;
	std::vector<const Card&> deck;
	std::vector<const Card&> droppeddeck;
};

#endif