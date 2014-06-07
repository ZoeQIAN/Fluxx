#ifndef FLUXXCONTROL_H
#define FLUXXCONTROL_H

#include <vector>
#include <string>
#include <time.h>
#include "..\fluxxControl_code\fluxxrules.h"
#include "..\cardLib_code\cardLib.h"
#include "..\mailbox\serverMB\serverMB.h"
#include "..\fluxxControl_code\player.h"

typedef int ruleMsg;
extern enum MsgType;
enum gameState{ 
	WAIT_FOR_PLAYERS,
	DEAL_ORIGINAL_CARD,
};
class fluxxControl {
public:
	fluxxControl ();
	void initrules();//��ʼ������
	void addplayers();//������
	void setpresentPlayer(Player&);
	void shuffleCard();//ϴ��
	void dealCard(); //����
	void playCard();//����
	void settleCard(const Card&); //���ų�������㣬��ڲ���Ϊ���Ƶ�����
	void dropCard();//���ƽ׶ο��ƺ���
	void removeCard(Player&);//�Ƴ�����
	void stagecontrol();
	void updateRules();
	void updateKeepers();
	int checkWinner();
	int getclientnum(Player&);
	~fluxxControl (){};

private:
	//�����غϿ������
	Player presentPlayer;
	fluxxRules rule;
	//��Ϣ�������
	int msgbufAdditional;//������Ϣ���
	int clientNum;//ѡ�е�client���
	MsgType msgbufMsgtype;//��Ϣ���ͻ���
	std::string msgbufString;//�ַ�������
	std::vector<Card*> msgbufCards;//������Ϣ����
	//������Ϸ�������
	CardLib& cards;
	ServerMB msgBox;
	gameState presentState;
	std::vector<Player> players;//�������
	std::vector<Card*> deck;//�ƶ�
	std::vector<Card*> droppeddeck;//���ƶ�
};

#endif