#include "stm32f10x.h"                  // Device header
#include "OLED.h"
#include "Delay.h"

/* ==============================================================================================
                                        枚举类型定义
   ============================================================================================== */

/**
 * @brief 选项模式枚举
 */
enum option_mode {
    PURE_TEXT,      // 0 - 纯文本模式
    SUBINTERFACE,   // 1 - 子界面模式
    EDITABLE,       // 2 - 可编辑模式
    INTERACTIBLE,   // 3 - 可交互模式
    NONE_MODE       // 4 - 无模式
};

/**
 * @brief 界面ID枚举
 */
enum interface_id {
    MENU_MAIN,      // 0 - 主菜单
    MENU_LED,       // 1 - LED控制菜单
    MENU_PID,       // 2 - PID菜单
    MENU_IMAGE,     // 3 - 图像菜单
    MENU_ANGLE,     // 4 - 角度菜单
    NONE_INTERFACE  // 5 - 无界面
};

/**
 * @brief 数值模式枚举
 */
enum value_mode {
    FRACTION,       // 小数模式
    INTEGER,        // 整数模式
    NAN_MODE        // 非数值模式
};

/**
 * @brief 交互状态枚举
 */
enum interact_state {
    SELECT_MODE,    // 选择模式
    EDIT_MODE       // 编辑模式
};

/* ==============================================================================================
                                        结构体定义
   ============================================================================================== */

/**
 * @brief 界面结构体定义
 * @note 使用树状结构组织菜单关系，支持多级菜单和可编辑项
 */
typedef struct {
    short option_count;                 // 选项数量，范围1~4
    char option_text[4][20];           // 选项文本内容
    short option_mode[4];              // 选项模式：pure_text, subinterface, editable, interactible, none
    int option_value[4];               // 选项对应的数值
    short value_mode;                  // 数值模式：fraction, integer, nan
    int value_range[4];                // 数值范围限制
    short value_length;                // 数值显示长度
    short super_interface;             // 父级界面索引
    short subinterface[4];             // 子级界面索引
    short allow_edit;                  // 是否允许编辑模式：0禁止，1允许
    short allow_title;                 // 是否需要标题：0不需要，1需要
    char title[20];                    // 标题文本
} Interface_TypeDef;

/* ==============================================================================================
                                        全局变量
   ============================================================================================== */

/**
 * @brief 界面配置数组
 * @note 预定义所有菜单界面的结构和内容
 */
Interface_TypeDef interface[100] = {
    [0] = {    // 主菜单界面
        .option_count = 4,
        .option_text = {"LED Control", "PID", "Image", "Angle"},
        .option_mode = {SUBINTERFACE, SUBINTERFACE, SUBINTERFACE, SUBINTERFACE},
        .option_value = {-1, -1, -1, -1},
        .value_range = {-1, -1, -1, -1},
        .value_mode = NAN_MODE,
        .value_length = -1,
        .super_interface = MENU_MAIN,
        .subinterface = {MENU_LED, MENU_PID, MENU_IMAGE, MENU_ANGLE},
        .allow_edit = 0,
        .allow_title = 0,
        .title = " "
    },
    [1] = {    // LED控制界面
        .option_count = 2,
        .option_text = {"LED_speed", "LED_dir", " ", " "},
        .option_mode = {EDITABLE, EDITABLE, NONE_MODE, NONE_MODE},
        .option_value = {0, 0, -1, -1},
        .value_range = {2, 1, -1, -1},
        .value_mode = INTEGER,
        .value_length = 1,
        .super_interface = MENU_MAIN,
        .subinterface = {NONE_INTERFACE, NONE_INTERFACE, NONE_INTERFACE, NONE_INTERFACE},
        .allow_edit = 1,
        .allow_title = 1,
        .title = "LED Control"
    },
    [2] = {    // PID参数界面
        .option_count = 3,
        .option_text = {"kp", "ki", "kd", " "},
        .option_mode = {EDITABLE, EDITABLE, EDITABLE, NONE_MODE},
        .option_value = {0, 0, 0, -1},
        .value_range = {100, 100, 100, -1},
        .value_mode = FRACTION,        // 小数用十进制模拟
        .value_length = -1,            // 小数特殊处理
        .super_interface = MENU_MAIN,
        .subinterface = {NONE_INTERFACE, NONE_INTERFACE, NONE_INTERFACE, NONE_INTERFACE},
        .allow_edit = 1,
        .allow_title = 1,
        .title = "PID"
    },
    [3] = {    // 图像界面
        .option_count = 1,
        .option_text = {"Image", " ", " ", " "},
        .option_mode = {INTERACTIBLE, NONE_MODE, NONE_MODE, NONE_MODE},
        .option_value = {-1, -1, -1, -1},
        .value_range = {-1, 1, -1, -1},
        .value_mode = NAN_MODE,
        .value_length = -1,
        .super_interface = MENU_MAIN,
        .subinterface = {NONE_INTERFACE, NONE_INTERFACE, NONE_INTERFACE, NONE_INTERFACE},
        .allow_edit = 0,
        .allow_title = 1,
        .title = "Image"
    },
    [4] = {    // 角度界面
        .option_count = 1,
        .option_text = {"Angle", " ", " ", " "},
        .option_mode = {INTERACTIBLE, NONE_MODE, NONE_MODE, NONE_MODE},
        .option_value = {-1, -1, -1, -1},
        .value_range = {-1, 1, -1, -1},
        .value_mode = NAN_MODE,
        .value_length = -1,
        .super_interface = MENU_MAIN,
        .subinterface = {NONE_INTERFACE, NONE_INTERFACE, NONE_INTERFACE, NONE_INTERFACE},
        .allow_edit = 0,
        .allow_title = 1,
        .title = "Angle"
    },
};

// 系统状态变量
short current_state;        // 当前状态：SELECT_MODE 或 EDIT_MODE
short current_selection;    // 当前选中的选项索引（从1开始）
short current_interface;    // 当前界面ID
short temp_pos;             // 临时位置记录，用于返回时恢复选择位置

/* ==============================================================================================
                                        显示函数
   ============================================================================================== */

/**
 * @brief 初始化菜单系统
 */
void interface_init(void) {
    current_state = SELECT_MODE;
    current_selection = 1;
    current_interface = MENU_MAIN;
}

/**
 * @brief 显示当前界面
 * @note 根据界面配置显示标题、选项、数值和状态指示器
 */
void show_interface(void) {
    short shift = 0;
    
    // 显示标题（如果允许）
    if (interface[current_interface].allow_title == 1) {
        shift = 1;
        OLED_ShowString(1, 1, interface[current_interface].title);
    }
    
    // 显示选择箭头
    for (int i = 1 + shift; i <= interface[current_interface].option_count + shift; i++) {
        if (i == current_selection + shift) {
            OLED_ShowString(i, 1, "> ");
        } else {
            OLED_ShowString(i, 1, "  ");
        }
    }
    
    // 显示选项文本
    for (int i = 1 + shift; i <= interface[current_interface].option_count + shift; i++) {
        OLED_ShowString(i, 3, interface[current_interface].option_text[i - 1 - shift]);
    }
    
    // 显示可编辑项的数值
    for (int i = 1 + shift; i <= interface[current_interface].option_count + shift; i++) {
        if (interface[current_interface].option_mode[i - 1 - shift] == EDITABLE) {
            if (interface[current_interface].value_mode == FRACTION) {
                // 小数显示处理
                int value = interface[current_interface].option_value[i - 1 - shift];
                
                if (value < 0) {
                    value = -value;
                    OLED_ShowChar(i, 13, '-');
                } else {
                    OLED_ShowChar(i, 13, ' ');
                }
                
                int int_part = value / 10;
                int fraction_part = value % 10;
                OLED_ShowNum(i, 14, int_part, 1);
                OLED_ShowChar(i, 15, '.');
                OLED_ShowNum(i, 16, fraction_part, 1);
            } else {
                // 整数显示处理
                int value = interface[current_interface].option_value[i - 1 - shift];
                OLED_ShowNum(i, 14, value, interface[current_interface].value_length);
            }                
        }
    }
    
    // 显示编辑模式指示器
    if (current_state == EDIT_MODE) {
        OLED_ShowChar(1, 16, 'E');
    } else {
        OLED_ShowChar(1, 16, ' ');
    }
}

/* ==============================================================================================
                                        操作函数
   ============================================================================================== */

/**
 * @brief 上移操作
 * @note 在选择模式下移动选择光标，在编辑模式下增加数值
 */
void up(void) {
    short shift = 0;
    
    // 计算标题导致的显示偏移
    if (interface[current_interface].allow_title == 1) {
        shift = 1;
    }
    
    if (current_state == SELECT_MODE) {
        // 选择模式：循环上移选择光标
        current_selection = (((current_selection - 1 - 1) + interface[current_interface].option_count) % 
                            interface[current_interface].option_count) + 1;    
    } else if (current_state == EDIT_MODE) {
        // 编辑模式：增加当前选项的数值
        int current_value = interface[current_interface].option_value[current_selection - shift];
        int value_restriction = interface[current_interface].value_range[current_selection - shift];
        
        interface[current_interface].option_value[current_selection - shift] = (current_value + 1);
    
        // 数值范围检查和处理
        if (interface[current_interface].value_mode == FRACTION) {
            if (current_value >= value_restriction) {
                interface[current_interface].option_value[current_selection - shift] = -value_restriction;
            }
        } else if (interface[current_interface].value_mode == INTEGER) {
            if (current_value >= value_restriction) {
                interface[current_interface].option_value[current_selection - shift] = 0;
            }
        }
    }

    show_interface();
}

/**
 * @brief 下移操作
 * @note 在选择模式下移动选择光标，在编辑模式下减少数值
 */
void down(void) {
    short shift = 0;
    
    // 计算标题导致的显示偏移
    if (interface[current_interface].allow_title == 1) {
        shift = 1;
    }
    
    if (current_state == SELECT_MODE) {
        // 选择模式：循环下移选择光标
        current_selection = (((current_selection - 1 + 1) + interface[current_interface].option_count) % 
                            interface[current_interface].option_count) + 1;    
    } else if (current_state == EDIT_MODE) {
        // 编辑模式：减少当前选项的数值
        int current_value = interface[current_interface].option_value[current_selection - shift];
        int value_restriction = interface[current_interface].value_range[current_selection - shift];
        
        interface[current_interface].option_value[current_selection - shift] = (current_value - 1);
        
        // 数值范围检查和处理
        if (interface[current_interface].value_mode == FRACTION) {
            if (current_value <= -value_restriction) {
                interface[current_interface].option_value[current_selection - shift] = value_restriction;
            }
        } else if (interface[current_interface].value_mode == INTEGER) {
            if (current_value <= 0) {
                interface[current_interface].option_value[current_selection - shift] = value_restriction;
            }
        }
    }

    show_interface();
}

/**
 * @brief 确认操作
 * @note 进入子菜单、切换编辑模式或执行交互功能
 */
void confirm(void) {
    short selection_mode = interface[current_interface].option_mode[current_selection - 1];
    
    if (selection_mode == SUBINTERFACE) {
        // 进入子菜单
        current_interface = interface[current_interface].subinterface[current_selection - 1];
        temp_pos = current_selection;  // 保存当前位置
        current_selection = 1;         // 重置为第一个选项
        OLED_Clear();
    } else if (selection_mode == EDITABLE) {
        // 切换编辑模式
        if (current_state == EDIT_MODE) {
            current_state = SELECT_MODE;
            OLED_ShowChar(1, 16, ' ');
        } else {
            current_state = EDIT_MODE;
            OLED_ShowChar(1, 16, 'E');
        }
    } else if (selection_mode == INTERACTIBLE) {
        // 执行交互功能（待扩展）
    }
}

/**
 * @brief 返回操作
 * @note 退出编辑模式或返回上级菜单
 */
void backward(void) {
    if (current_state == EDIT_MODE) {
        // 退出编辑模式
        current_state = SELECT_MODE;
        OLED_ShowChar(1, 16, ' ');
    } else {
        // 返回上级菜单
        current_interface = interface[current_interface].super_interface;
        current_selection = temp_pos;  // 恢复之前的选择位置
        OLED_Clear();
    }
    show_interface();
}

/**
 * @brief 获取指定界面和选项的数值
 * @param INTERFACE 界面ID
 * @param selection 选项索引
 * @return 选项的数值
 */
int get_value(int INTERFACE, int selection) {
    return interface[INTERFACE].option_value[selection];
}
