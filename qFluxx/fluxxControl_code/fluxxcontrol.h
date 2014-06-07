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
	//�׶δ�����
	void initrules();//��ʼ������
	void addPlayers();//������
	void shuffleCard();//ϴ��
	void dealCard(); //����
	void playCard();//����
	void dropCard();//����
	void setpresentPlayer(Player&);//�غϿ���
	void stagecontrol();
	int getclientnum(Player&);
	void run();
	~fluxxControl (){};

private:
	void _settleCard(const Card&); //���ų�������㣬��ڲ���Ϊ���Ƶ�����
	void _removeCard(Player&);//�Ƴ�����
	void _updateRules();
	void _updateKeepers();
	int _checkWinner();
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