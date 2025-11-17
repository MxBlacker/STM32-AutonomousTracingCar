#ifndef __ABSOLUTE_MENU_SYS
#define __ABSOLUTE_MENU_SYS

/*
	本菜单系统由麦香包倾情打造，拥有无敌般的可拓展性，虽然不知道有啥用
	总计学习时间10.14 ~ 10.20 从软件安装疯狂推到TIM计时器，无敌了
	
	期间还去学了OOP和一些其他杂碎的知识
	
	但是，注意，这个菜单的每一个子交互界面有且仅有一种访问路径
*/

enum interface_id{
	MENU_MAIN, 		//0
	MENU_LED,		//1
	MENU_PID,		//2
	MENU_IMAGE,		//3
	MENU_ANGLE,		//4
	NONE			//5
};

int get_value(int INTERFACE,int num);

void interface_init(void);
void show_interface(void);

void up(void);
void down(void);
void confirm(void);
void backward(void);

#endif
