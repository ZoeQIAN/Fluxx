#ifndef FLUXXCONTROL_H
#define FLUXXCONTROL_H

#include <vector>
#include "..\fluxxControl_code\fluxxrules.h"
#include "..\cardLib_code\cardLib.h"
#include "..\mailbox\mailBox.h"
class Player;
typedef int ruleMsg;
typedef int cardNum;

class fluxxControl {
public:
	fluxxControl ();
	void initcards();//��ʼ�����ƣ����ƿ��ȡ���ù�������Ĭ�Ϲ��캯����ʹ��
	void initrules();//��ʼ��������Ĭ�Ϲ��캯����ʹ��
	void addplayers(const Player&);//�����ң���Ĭ�Ϲ��캯���������ʵ���ʱ��ʹ��
	void shuffleCard();//ϴ��
	void dealCard(ruleMsg); //����,��ڲ���Ϊ�ӹ����еõ�����Ϣ
	void playCard(ruleMsg);//���ƽ׶ο��ƺ�������ڲ���Ϊ�ӹ����еõ�����Ϣ
	void settleCard(cardNum); //���ų�������㣬��ڲ���Ϊ���Ʊ���
	void dropCard(ruleMsg);//���ƽ׶ο��ƺ������ڲ���Ϊ�ӹ����еõ�����Ϣ
	~fluxxControl ();

private:
	MailBox communitor;
	fluxxRules rule;
	std::vector<Player> players;
	CardLib& cards;
};

#endif