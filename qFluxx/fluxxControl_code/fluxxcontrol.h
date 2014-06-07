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
	void dealCard(); //����,��ڲ���Ϊ�ӹ����еõ�����Ϣ
	void playCard();//���ƽ׶ο��ƺ�������ڲ���Ϊ�ӹ����еõ�����Ϣ
	void settleCard(const Card&); //���ų�������㣬��ڲ���Ϊ���Ƶ�����
	void dropCard();//���ƽ׶ο��ƺ������ڲ���Ϊ�ӹ����еõ�����Ϣ
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
	int msgbufAdditional;//������Ϣ���
	int clientNum;//ѡ�е�client���
	MsgType msgbufMsgtype;//��Ϣ���ͻ���
	std::string msgbufString;//�ַ�������
	std::vector<Card*> msgbufCards;//������Ϣ����
	//������Ϸ�������
	CardLib& cards;
	ServerMB msgBox;
	std::vector<Player> players;//�������
	std::vector<Card*> deck;//�ƶ�
	std::vector<Card*> droppeddeck;//���ƶ�
};

#endif