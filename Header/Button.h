#ifndef __BUTTON_CHECK
#define __BUTTON_CHECK

enum EVENT{
    MENU_UP,
    MENU_DOWN,
    MENU_FORWARD,
    MENU_BACKWARD,
    MENU_TEMP_BOOT,
    MENU_TEMP_SPEED,
};

void Button_Check(GPIO_TypeDef * GPIOx , uint16_t Pin , uint16_t EVENT_ID);

#endif
