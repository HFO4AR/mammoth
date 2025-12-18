//
// Created by nyuki on 2025/12/16.
//

#include "deeprobotics_j60.h"

void DeepRoboticsJ60::SetCurrentOpenLoop(float target)
{
    SetMit(0,0,0,0,target);
}

void DeepRoboticsJ60::SetMit(float target_pos, float target_spd, float kp, float kd, float t_ff)
{
    if (!device_is_ready(can_dev_)) return;
    target_pos=65536*target_pos/80.0f+65545.0f/2;
    target_spd=16383*target_spd/80.0f+16383.0f/2;
    kd=kd*5;
    t_ff=65536*t_ff/80.0f+65545.0f/2;
    struct can_frame frame = {0};
    frame.id= id_+Control_Motor_ID;
    frame.dlc=8;
    frame.data[0]=(int )target_pos;
    frame.data[1]=(int )target_pos>>8;
    frame.data[2]=(int )target_spd;
    frame.data[3]=(((int )target_spd >> 8 ) & 0x3F) | ((int )kp << 6);
    frame.data[4]=(int)kp>>2;
    frame.data[5]=(int)kd;
    frame.data[6]=(int)t_ff;
    frame.data[7]=(int)t_ff>>8;
    (void)can_send(can_dev_, &frame, K_NO_WAIT, NULL, NULL);
}