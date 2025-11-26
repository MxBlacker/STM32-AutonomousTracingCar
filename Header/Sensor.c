#include "stm32f10x.h"                  // Device header
#include "Sensor.h"
#include "Motor.h"
#include "Menu.h"

enum COLOR{
    BLACK,
    WHITE,
};

void TRACK(void){

    int boot_state = get_value(MENU_MAIN, 0);
    if(boot_state == 0) return;

    int Speed_tier = (float)get_value(MENU_MAIN, 1);
    float Speed = Speed_tier * 80.0f;

    if(OUT_1 == WHITE && OUT_2 == WHITE && OUT_3 == BLACK && OUT_4 == WHITE && OUT_5 == WHITE){         //直行
        Set_Car_Speed(Speed, Speed);
    }else if(OUT_1 == WHITE && OUT_2 == WHITE && OUT_3 == BLACK && OUT_4 == BLACK && OUT_5 == WHITE){   //微左转
        Set_Car_Speed(Speed * 1.15, 0);
    }else if(OUT_1 == WHITE && OUT_2 == WHITE && OUT_3 == WHITE && OUT_4 == BLACK && OUT_5 == WHITE){   //左转
        Set_Car_Speed(Speed * 1.3, 0);
    }else if(OUT_1 == WHITE && OUT_2 == WHITE && OUT_3 == WHITE && OUT_4 == BLACK && OUT_5 == BLACK){   //强左转
        Set_Car_Speed(Speed * 1.45, 0);
    }else if(OUT_1 == WHITE && OUT_2 == WHITE && OUT_3 == WHITE && OUT_4 == WHITE && OUT_5 == BLACK){   //极左转
        Set_Car_Speed(Speed * 1.6, 0);
    }else if(OUT_1 == WHITE && OUT_2 == BLACK && OUT_3 == BLACK && OUT_4 == WHITE && OUT_5 == WHITE){   //微右
        Set_Car_Speed(0, Speed * 1.15);
    }else if(OUT_1 == WHITE && OUT_2 == BLACK && OUT_3 == WHITE && OUT_4 == WHITE && OUT_5 == WHITE){   //微右
        Set_Car_Speed(0, Speed * 1.3);
    }else if(OUT_1 == WHITE && OUT_2 == BLACK && OUT_3 == WHITE && OUT_4 == WHITE && OUT_5 == WHITE){   //微右
        Set_Car_Speed(0, Speed * 1.3);
    }else if(OUT_1 == BLACK && OUT_2 == BLACK && OUT_3 == WHITE && OUT_4 == WHITE && OUT_5 == WHITE){   //微右
        Set_Car_Speed(0, Speed * 1.45);
    }else if(OUT_1 == BLACK && OUT_2 == WHITE && OUT_3 == WHITE && OUT_4 == WHITE && OUT_5 == WHITE){   //微右
        Set_Car_Speed(0, Speed * 1.6);
    }else if(OUT_1 == BLACK && OUT_2 == BLACK && OUT_3 == BLACK && OUT_4 == BLACK && OUT_5 == BLACK){   //十字路口
        Set_Car_Speed(Speed, Speed);
    }
}
