//
// Created by nyuki on 2025/11/19.
//

#include "aim.h"

#include <cmath>

#include "dji_rm3508.h"

void PTZ::Init()
{
    yaw_motor_.SetSpdPid(2,0.1,0.0,2000);
    pitch_motor_.SetSpdPid(1,0.1,0.0,2000);
    bool pitch_init_complete=false;
    bool yaw_init_complete=false;
    while(!pitch_init_complete||!yaw_init_complete){
        if (!pitch_init_complete) {
            if (abs(pitch_motor_.spd_pid_.data.output)>1600) {
                pitch_init_complete=true;
                pitch_angle_=30;
                pitch_motor_.SetSpdPid(8,0.1,0.0,5000);
                pitch_motor_.SetSpeed(0);
            }else {
                pitch_motor_.SetSpeed(1000);
            }
        }else
        {
            pitch_motor_.SetSpeed(0);
        }
        k_msleep(5);
        if (!yaw_init_complete) {
            if (abs(yaw_motor_.spd_pid_.data.output)>1600) {
                yaw_init_complete=true;
                yaw_angle_=30;
                yaw_motor_.SetSpdPid(8,0.1,0.0,5000);
                yaw_motor_.SetSpeed(0);
            }else {
                yaw_motor_.SetSpeed(1000);
            }
        }else
        {
            yaw_motor_.SetSpeed(0);
        }
        k_msleep(5);
    }
    for (int i = 0; i < 10; ++i)
    {
        pitch_motor_.SetSpeed(0);
        k_msleep(5);
        yaw_motor_.SetSpeed(0);
        k_msleep(5);
    }
    // SetAngle(0,0);
}
void PTZ::SetAngle(float yaw,float pitch)
{

}