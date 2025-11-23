//
// Created by nyuki on 2025/11/19.
//

#include "aim.h"

#include <cmath>

#include "dji_rm3508.h"

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

void PTZ::Init()
{
    // Positive direction initialization
    InitMotorDirection(true);
    
    // Wait between directions
    k_msleep(1000);
    
    // Negative direction initialization
    InitMotorDirection(false);
}

void PTZ::SetAngle(float yaw,float pitch)
{

}