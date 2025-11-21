//
// Created by nyuki on 2025/11/19.
//

#include "aim.h"

#include <cmath>

#include "dji_rm3508.h"
void PTZ::Init() {
    yaw_motor_.SpdPidInit(2,0.1,0.0,2000);
    pitch_motor_.SpdPidInit(2,0.1,0.0,2000);
    bool pitch_init_complete=false;
    bool yaw_init_complete=false;
    while(!pitch_init_complete||!yaw_init_complete){
        if (!pitch_init_complete) {
            if (abs(pitch_motor_.cur)>800) {
                pitch_init_complete=true;
            }else {
                pitch_motor_.SetSpeed(500);
            }
        }
        k_msleep(10);
        if (!yaw_init_complete) {
            if (abs(yaw_motor_.cur)>800) {
                yaw_init_complete=true;
            }else {
                yaw_motor_.SetSpeed(500);
            }
        }
        k_msleep(10);
    }
}
