#ifndef FLUXXCONTROL_H
#define FLUXXCONTROL_H

#include <vector>
#include <string>
#include <time.h>
#include "..\fluxxControl_code\fluxxrules.h"
#include "..\cardLib_code\cardLib.h"
#include "..\mailbox\serverMB\serverMB.h"
#include "..\fluxxControl_code\player.h"

//extern enum MsgType;
enum gameState{ 
	WAIT_FOR_PLAYERS,
	DEAL_ORIGINAL_CARD,
	ROUND_PREPARE,
	PLAYING_CARD,
	DROPING_CARD,
	GAME_END,
};
class fluxxControl {
public:
	fluxxControl (fluxxRules&);
	void addPlayers();//������
	/***������������***/
	void dealCard(int); //����
	void playCard(const Card&);//����
	void dropCard(int);//����
	/***************************************/
	void setpresentPlayer(int);//�޸ĵ�ǰǰ�溯�������ö���
	void actioncard(const Card& act_card);
	//������Ϸ����
	void fluxxRun();

	~fluxxControl ();

private:
	void _shuffleCard();//ϴ��
	void _settleCard(const Card&); //���Ž���
	void _updateRules();
	void _updateKeepers();
	void _settleKeepercard(const Card&);
	void _settleRulecard(const Card&);
	void _settleGoalcard(const Card&);
	int _checkCntdraw();//����������
	int _checkCntplay();//���������
	int _checkCntdrop();
	int _checkWinner();//���ʤ��
	int _getclientnum(Player&);//����������Ϣ���ݵ���ұ��
	//�����غϿ������
	Player *presentPlayer;
	fluxxRules rule;
	//��Ϣ�������
	int msgbufAdditional;//������Ϣ���
	int clientNum;//ѡ�е�client���
	MsgType msgbufMsgtype;//��Ϣ���ͻ���
	std::string msgbufString;//�ַ�������
	std::vector<const Card*> msgbufCards;//������Ϣ����
	//������Ϸ�������
	CardLib& cards;
	ServerMB msgBox;
	gameState presentState;
	std::vector<Player*> players;//�������
	std::vector<const Card*> deck;//�ƶ�
	std::vector<const Card*> droppeddeck;//���ƶ�
};
#endif