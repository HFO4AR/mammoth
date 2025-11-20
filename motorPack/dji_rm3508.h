//
// Created by nyuki on 2025/10/17.
//

#ifndef MAMMOTH_DJI_RM3508_H
#define MAMMOTH_DJI_RM3508_H

#include "motor.h"
#include <zephyr/drivers/can.h>
# define DISABLE 0
# define ENABLE 1

void GetRm3508Data(can_frame *frame);


class DjiRm3508 :public Motor{
public:
    DjiRm3508(const int id) :  Motor(id){}
    static void SendData();
    void SetCurrentOpenLoop(int target);//open loop
    void EnableSyncSeed();
private:
    int sync_seed_mode_=DISABLE;
};

inline DjiRm3508 motor0(0);
inline DjiRm3508 motor1(1);
inline DjiRm3508 motor2(2);
inline DjiRm3508 motor3(3);


// void Motor_Set_Current(int16_t current0, int16_t current1, int16_t current2, int16_t current3);


#endif //MAMMOTH_DJI_RM3508_H