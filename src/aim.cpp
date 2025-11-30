//
// Created by nyuki on 2025/11/19.
//

#include "aim.h"

#include <cmath>

#include "dji_m3508.h"

#include <zephyr/kernel.h>

#include <dji_dbus.h>
extern PTZ ptz;
/***云台线程begain***/
K_THREAD_STACK_DEFINE(ptz_stack_area, 4096);
struct k_thread ptz_thread_data;
void ptz_thread_entry(void *p1, void *p2, void *p3)
{
    while (true)
    {
        // ptz.yaw_motor_.SetPosition(ptz.yaw_data_.target);
        // k_msleep(5);

        ptz.pitch_motor_.SetPosition(ptz.pitch_data_.target);
        k_msleep(5);
    }
}
/***云台线程end***/
void PTZ::InitMotorDirection(bool is_positive_direction) {
    const int speed = is_positive_direction ? 1000 : -1000;
    
    yaw_motor_.SetSpdPid(2,0.1,0.0,2000);
    pitch_motor_.SetSpdPid(1,0.1,0.0,2000);
    bool pitch_init_complete = false;
    bool yaw_init_complete = false;
    
    while(!pitch_init_complete || !yaw_init_complete) {
        // Handle pitch motor initialization
        if (!pitch_init_complete) {
            if (abs(pitch_motor_.spd_pid_.data.output) > 1600) {
                pitch_init_complete = true;
                if (is_positive_direction) {
                    pitch_data_.max_angle = pitch_motor_.GetTotalPosition();
                } else {
                    pitch_data_.min_angle = pitch_motor_.GetTotalPosition();
                }
                pitch_motor_.SetSpdPid(8,0.1,0.0,5000);
                pitch_motor_.SetSpeed(0);
            } else {
                pitch_motor_.SetSpeed(speed);
            }
        } else {
            pitch_motor_.SetSpeed(0);
        }
        k_msleep(5);
        
        // Handle yaw motor initialization
        if (!yaw_init_complete) {
            if (abs(yaw_motor_.spd_pid_.data.output) > 1600) {
                yaw_init_complete = true;
                if (is_positive_direction) {
                    yaw_data_.max_angle = yaw_motor_.GetTotalPosition();
                } else {
                    yaw_data_.min_angle = yaw_motor_.GetTotalPosition();
                }
                yaw_motor_.SetSpdPid(8,0.1,0.0,5000);
                yaw_motor_.SetSpeed(0);
            } else {
                yaw_motor_.SetSpeed(speed);
            }
        } else {
            yaw_motor_.SetSpeed(0);
        }
        k_msleep(5);
    }
    
    // Ensure motors are stopped
    for (int i = 0; i < 10; ++i) {
        pitch_motor_.SetSpeed(0);
        k_msleep(5);
        yaw_motor_.SetSpeed(0);
        k_msleep(5);
    }
}

void PTZ::CheckMotorOnline()
{
    int time;
    while ((!pitch_motor_.temp_&&!yaw_motor_.temp_)||time<100)
    {
        k_msleep(10);
        time++;
    }
    if (time==100)
    {
        printk("初始化超时：");
        if (!pitch_motor_.temp_)
        {
            printk("pitch离线\n");
        }else if (!yaw_motor_.temp_)
        {
            printk("yaw离线\n");
        }
    }
}
void PTZ::Init()
{
    CheckMotorOnline();
    // Positive direction initialization
    InitMotorDirection(true);
    
    // Wait between directions
    k_msleep(1000);
    
    // Negative direction initialization
    InitMotorDirection(false);
    yaw_data_.zero_angle=(yaw_data_.max_angle+yaw_data_.min_angle)/2;
    pitch_data_.zero_angle=(pitch_data_.max_angle+pitch_data_.min_angle)/2;
    yaw_data_.target=yaw_data_.zero_angle;
    pitch_data_.target=pitch_data_.zero_angle;
    // 云台PID初始化
    yaw_motor_.SetSpdPid(1,0.1,0.0,2000);
    pitch_motor_.SetSpdPid(3,0.1,0.0,5000);
    yaw_motor_.SetPosPid(1,0,0,2000);
    pitch_motor_.SetPosPid(0.5,0,4,500);
    //云台线程初始化
    k_msleep(1000);
    k_thread_create(&ptz_thread_data,
                    ptz_stack_area,
                    K_THREAD_STACK_SIZEOF(ptz_stack_area),
                    ptz_thread_entry,
                    NULL, NULL, NULL,
                    5,
                    0,
                    K_NO_WAIT);
    printk("云台初始化完成\n");
}


void PTZ::SetAngle(float yaw_target,float pitch_target)
{
    yaw_data_.target=yaw_target;
    pitch_data_.target=pitch_target;
}