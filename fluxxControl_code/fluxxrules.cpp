#include "fluxxrules.h"

fluxxRules::fluxxRules()
	: cntDraw(1),cntPlay(1),cntCardlimit(-1), cntKeeperlimit(-1){
		//Ŀ���Ƶĳ�ʼ��û��ȷ��
}
void fluxxRules::draw(int i) {
	cntDraw = i;
}
void fluxxRules::play(int i) {
	cntPlay = i;
}
void fluxxRules::cardlimitation(int i) {
	cntCardlimit = i;
}
void fluxxRules::keeperlimitation(int i) {
	cntKeeperlimit = i;
}
void fluxxRules::firstgoal(const Card& goal) {
	firstGoal = goal;
}
void fluxxRules::secondgoal(const Card& goal) {
	secondGoal = goal;
}
void fluxxRules::setspecialrule() {
	//�������Ĵ���
}
const int fluxxRules::draw() const {
	return cntDraw;
}
const int fluxxRules::play() const {
	return cntPlay;
}
const int fluxxRules::cardlimitation() const {
	return cntCardlimit;
}
const int fluxxRules::keeperlimitation() const {
	return cntKeeperlimit;
}
const Card& fluxxRules::firstgoal() const {
	return firstGoal;
}
const Card& fluxxRules::secondGoal() const {
	return secondGoal;
}
