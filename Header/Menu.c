#include "stm32f10x.h"                  // Device header
#include "OLED.h"
#include "Delay.h"

/*
	最近刚学了OOP，所以想做一个可拓展的菜单系统
	所以这里定义了一个结构体，以树的方式来链接
	好吧我也不知道这是啥，总之专门开了个变量放子父级的索引
*/

typedef struct{
	
	short option_count; 				
//存选项数量 取值范围 1~4
	
	char option_text[4][20];
//存选项文本
	
	short option_mode[4];			
//存选项模式
//pure_text , subinterface , editable , interactible , none
	
	int option_value[4];				
//存选项的值，如果有的话
	
	short value_mode;
//值的模式
//fraction , integer , nan
	
	int value_range[4];
//值的范围，如果有的话
	
	short value_length;
//值的显示长度，如果有的话
	
/*
	MENU_MAIN , MENU_LED , MENU_PID , MENU_IMAGE , MENU_ANGLE , NONE
*/

	short super_interface;				
//存父级的索引
	
	short subinterface[4];				
//存子集的索引，如果有的话
	
	short allow_Edit;
//是否允许为编辑模式 0 , 1
	
	short allow_title;
//判断是否需要标题 0 , 1

	char title[20];
//标题，如果有的话

}Interface_TypeDef;

enum option_mode{
	pure_text,			// 0 为 pure_text
	subinterface,		// 1 为 sub_interface
	editable,			// 2 为 editable
	interactible,	// 3 为 interactible
	none
};

enum interface_id{
	MENU_MAIN, 		//0
	MENU_LED,		//1
	MENU_PID,		//2
	MENU_IMAGE,		//3
	MENU_ANGLE,		//4
	NONE			//5
};

enum value_mode{
	fraction,
	integer,
	nan
};

/*
	下面就是对界面的定义了，这就叫Expandibility!这就叫Customizable!
*/

Interface_TypeDef interface[100] = {
	[0] = {
	.option_count = 4,
	.option_text = {"LED Control","PID","Image","Angle"},
	.option_mode = {subinterface, subinterface, subinterface, subinterface},
    .option_value = {-1, -1, -1, -1},
	.value_range = {-1, -1, -1, -1},
	.value_mode = nan,
	.value_length = -1,
    .super_interface = MENU_MAIN,
    .subinterface = {MENU_LED, MENU_PID, MENU_IMAGE, MENU_ANGLE},
	.allow_title = 0,
	.title = " "
	},
	[1] = {
	.option_count = 2,
	.option_text = {"LED_speed","LED_dir"," "," "},
	.option_mode = {editable, editable, none, none},
    .option_value = {0, 0, -1, -1},
	.value_range = {2, 1, -1, -1},
	.value_mode = integer,
	.value_length = 1,
    .super_interface = MENU_MAIN,
    .subinterface = { NONE, NONE, NONE, NONE},
	.allow_title = 1,
	.title = "LED Control"
	},
	[2] = {
	.option_count = 3,
	.option_text = {"kp","ki","kd"," "},
	.option_mode = {editable, editable, editable, none},
    .option_value = {0, 0, 0, -1},
	.value_range = {100, 100, 100, -1},
	.value_mode = fraction, //小数我们用十进制模拟
	.value_length = -1,		//小数比较特殊，我们分开讨论
    .super_interface = MENU_MAIN,
    .subinterface = { NONE, NONE, NONE, NONE},
	.allow_title = 1,
	.title = "PID"
	},
	[3] = {
	.option_count = 1,
	.option_text = {"Image"," "," "," "},
	.option_mode = {interactible, none, none, none},
    .option_value = {-1, -1, -1, -1},
	.value_range = {-1, 1, -1, -1},
	.value_mode = nan,
	.value_length = -1,
    .super_interface = MENU_MAIN,
    .subinterface = { NONE, NONE, NONE, NONE},
	.allow_title = 1,
	.title = "Image"
	},
	[4] = {
	.option_count = 1,
	.option_text = {"Angle"," "," "," "},
	.option_mode = {interactible, none, none, none},
    .option_value = {-1, -1, -1, -1},
	.value_range = {-1, 1, -1, -1},
	.value_mode = nan,
	.value_length = -1,
    .super_interface = MENU_MAIN,
    .subinterface = { NONE, NONE, NONE, NONE},
	.allow_title = 1,
	.title = "Angle"
	},
};

enum INTERACT_STATE{SELECT_MODE,EDIT_MODE};

short current_state;
short current_selection;
short current_interface;
short temp_pos; //赫赫你猜这个是干嘛的

/*
==============================================================================================

									以下是显示函数

==============================================================================================
*/

void interface_init(void){
	
	current_state = SELECT_MODE;
	current_selection = 1;
	current_interface = MENU_MAIN;
	
}

void show_interface(void){
	
	short shift = 0;
	if(interface[current_interface].allow_title == 1){ //标题及其导致的偏移
		shift = 1;
		OLED_ShowString( 1, 1,interface[current_interface].title);
	}
	
	for(int i = 1 + shift;i <= interface[current_interface].option_count + shift;i++){	//箭头打印，这样主要是为了覆盖那些莫名其妙出现的像素块
		if(i == current_selection + shift) OLED_ShowString(i, 1,"> ");
		else OLED_ShowString(i, 1,"  ");
	}
	
	for(int i = 1 + shift;i <= interface[current_interface].option_count + shift;i++) 	//文本打印
		OLED_ShowString(i, 3,interface[current_interface].option_text[i - 1 - shift]);
	
	for(int i = 1 + shift;i <= interface[current_interface].option_count + shift;i++){ 	//数据打印，在第14列
		if(interface[current_interface].option_mode[i - 1 - shift] == editable){
			if(interface[current_interface].value_mode == fraction){					//小数
				int value = interface[current_interface].option_value[i - 1 - shift];
				
				if(value < 0){
					value = -value;
					OLED_ShowChar(i, 13, '-');
				}else{
					OLED_ShowChar(i, 13, ' ');
				}
				
				int int_part = value / 10;
				int fraction_part = value % 10;
				OLED_ShowNum(i, 14, int_part, 1);
				OLED_ShowChar(i, 15, '.');
				OLED_ShowNum(i, 16, fraction_part, 1);
			}else{																		//整数
				int value = interface[current_interface].option_value[i - 1 - shift];
				OLED_ShowNum(i, 14, value, interface[current_interface].value_length);
			}				
		}
	}
	
	if(current_state == EDIT_MODE) OLED_ShowChar( 1, 16,'E'); //模式打印，在第16列
	else OLED_ShowChar( 1, 16,' ');

	//test(0,0);
}

/*
==============================================================================================

									以下是操作函数

==============================================================================================
*/

void up(void){
	
	short shift = 0;
	if(interface[current_interface].allow_title == 1){ //标题及其导致的偏移
		shift = 1;
	}
	
	if(current_state == SELECT_MODE){ //选择模式上移
		current_selection = (((current_selection - 1 - 1) + interface[current_interface].option_count ) % interface[current_interface].option_count) + 1;	
	}
	else if(current_state == EDIT_MODE){ //编辑模式上移
		
		int current_value = interface[current_interface].option_value[current_selection - shift];
		int value_restriction = interface[current_interface].value_range[current_selection - shift];
		
		interface[current_interface].option_value[current_selection - shift] = (current_value + 1);
	
		if(interface[current_interface].value_mode == fraction){
			if(current_value >= (value_restriction)) interface[current_interface].option_value[current_selection - shift] = -(value_restriction);
		}
		else if(interface[current_interface].value_mode == integer){
			if(current_value >= (value_restriction)) interface[current_interface].option_value[current_selection - shift] = 0;
		}

	}

	show_interface();
	//test(1,0);
}

void down(void){
	
	short shift = 0;
	if(interface[current_interface].allow_title == 1){ //标题及其导致的偏移
		shift = 1;
	}
	
	if(current_state == SELECT_MODE){ //选择模式下移
		current_selection = (((current_selection - 1 + 1) + interface[current_interface].option_count ) % interface[current_interface].option_count) + 1;	
	}
	else if(current_state == EDIT_MODE){ //编辑模式下移
		
		int current_value = interface[current_interface].option_value[current_selection - shift];
		int value_restriction = interface[current_interface].value_range[current_selection - shift];
		
		interface[current_interface].option_value[current_selection - shift] = (current_value - 1);
		if(interface[current_interface].value_mode == fraction){
			if(current_value <= -(value_restriction)) interface[current_interface].option_value[current_selection - shift] = value_restriction;
		}
		else if(interface[current_interface].value_mode == integer){
			if(current_value <= 0) interface[current_interface].option_value[current_selection - shift] = value_restriction;
		}
	}

	show_interface();
	//test(2,0);
}

void confirm(void){
	
	short selection_mode = interface[current_interface].option_mode[current_selection - 1];
	
	if(selection_mode == subinterface){ //下级菜单选项
		current_interface = interface[current_interface].subinterface[current_selection - 1];
		temp_pos = current_selection;
		current_selection = 1;
		OLED_Clear();
	}
	else if(selection_mode == editable){ //编辑模式，启动！
		if(current_state == EDIT_MODE){
			current_state = SELECT_MODE;
			OLED_ShowChar( 1, 16,' ');
		}else{
			current_state = EDIT_MODE;
			OLED_ShowChar( 1, 16,'E');
		}
	}else if(selection_mode == interactible){
		//有类似功能了再说
	}


	show_interface();
	//test(3,0);

}
void backward(void){
	
	if(current_state == EDIT_MODE){
		current_state = SELECT_MODE;
		OLED_ShowChar( 1, 16,' ');
	}
	else{
		current_interface = interface[current_interface].super_interface;
		current_selection = temp_pos;
		OLED_Clear();
	}


	show_interface();
	
	//test(4,0);
}

int get_value(int INTERFACE,int selection){
	return interface[INTERFACE].option_value[selection];
}

