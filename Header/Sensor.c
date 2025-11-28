#include "stm32f10x.h"                  // Device header
#include "Sensor.h"
#include "Motor.h"
#include "Menu.h"
#include "OLED.h"

#define MICRO_TURN_BOOST 1.05f
#define MICRO_TURN_REDUCE 0.905f
#define TURN_BOOST 1.2f
#define TURN_REDUCE 0.8f
#define STRONG_TURN_BOOST 1.45f
#define STRONG_TURN_REDUCE 0.5f
#define EXTREME_TURN_BOOST 1.0f
#define EXTREME_TURN_REDUCE -0.6f

enum COLOR{
    BLACK,
    WHITE,
};

int prev_state;

void TRACK(void){

    int Speed_tier = get_value(MENU_MAIN, 2);

    float Speed = Speed_tier * 80.0f;

    if(OUT_1 == WHITE && OUT_2 == WHITE && OUT_3 == BLACK && OUT_4 == WHITE && OUT_5 == WHITE){         //直行
        Set_Car_Speed(Speed, Speed);
    }else if(OUT_1 == WHITE && OUT_2 == WHITE && OUT_3 == BLACK && OUT_4 == BLACK && OUT_5 == WHITE){   //微右转
        Set_Car_Speed(Speed * MICRO_TURN_REDUCE, Speed * MICRO_TURN_BOOST);
        prev_state = 0;
    }else if(OUT_1 == WHITE && OUT_2 == WHITE && OUT_3 == WHITE && OUT_4 == BLACK && OUT_5 == WHITE){   //右转
        Set_Car_Speed(Speed * TURN_REDUCE, Speed * TURN_BOOST);
        prev_state = 0;
    }else if(OUT_1 == WHITE && OUT_2 == WHITE && OUT_3 == WHITE && OUT_4 == BLACK && OUT_5 == BLACK){   //强右转
        Set_Car_Speed(Speed * STRONG_TURN_REDUCE, Speed * STRONG_TURN_BOOST);
        prev_state = 0;
    }else if(OUT_1 == WHITE && OUT_2 == WHITE && OUT_3 == WHITE && OUT_4 == WHITE && OUT_5 == BLACK){   //极右转
        Set_Car_Speed(Speed * EXTREME_TURN_REDUCE, Speed * EXTREME_TURN_BOOST);
        prev_state = 0;
    }else if(OUT_1 == WHITE && OUT_2 == BLACK && OUT_3 == WHITE && OUT_4 == WHITE && OUT_5 == WHITE){   //微左
        Set_Car_Speed(Speed * MICRO_TURN_BOOST, Speed * MICRO_TURN_REDUCE);
        prev_state = 1;
    }else if(OUT_1 == WHITE && OUT_2 == BLACK && OUT_3 == WHITE && OUT_4 == WHITE && OUT_5 == WHITE){   //左
        Set_Car_Speed(Speed *TURN_BOOST, Speed * TURN_REDUCE);
        prev_state = 1;
    }else if(OUT_1 == BLACK && OUT_2 == BLACK && OUT_3 == WHITE && OUT_4 == WHITE && OUT_5 == WHITE){   //强左
        Set_Car_Speed(Speed * STRONG_TURN_BOOST, Speed * STRONG_TURN_REDUCE);
        prev_state = 1;
    }else if(OUT_1 == BLACK && OUT_2 == WHITE && OUT_3 == WHITE && OUT_4 == WHITE && OUT_5 == WHITE){   //极左
        Set_Car_Speed(Speed * EXTREME_TURN_BOOST, Speed * EXTREME_TURN_REDUCE);
        prev_state = 1;
    }else if(OUT_1 == BLACK && OUT_2 == BLACK && OUT_3 == BLACK && OUT_4 == BLACK && OUT_5 == BLACK){   //十字路口
        Set_Car_Speed(Speed, Speed);
    }else if(OUT_1 == WHITE && OUT_2 == WHITE && OUT_3 == WHITE && OUT_4 == WHITE && OUT_5 == WHITE){   // 打转
        if(prev_state == 1){
            Set_Car_Speed(Speed, -Speed);
        }else if(prev_state == 0){
            Set_Car_Speed(-Speed, Speed);
        }
    }else if(OUT_1 == BLACK && OUT_2 == BLACK && OUT_3 == BLACK && OUT_4 == WHITE && OUT_5 == WHITE){   // 左三
        Set_Car_Speed(Speed * EXTREME_TURN_BOOST, Speed * EXTREME_TURN_REDUCE);
        prev_state = 1;
    }else if(OUT_1 == WHITE && OUT_2 == WHITE && OUT_3 == BLACK && OUT_4 == BLACK && OUT_5 == BLACK){   // 右三
        Set_Car_Speed(Speed * EXTREME_TURN_REDUCE, Speed * EXTREME_TURN_BOOST);
        prev_state = 0;
    }else{
        Set_Car_Speed(Speed, Speed);
    }

    OLED_ShowNum(4,14,Speed_tier,1);
}
