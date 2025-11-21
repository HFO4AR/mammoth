//
// Created by nyuki on 2025/11/19.
//

#ifndef MAMMOTH_AIM_H
#define MAMMOTH_AIM_H
#include "dji_rm3508.h"

class PTZ {
public:
    int max_yaw_angle_;
    int max_pitch_angle_;
    PTZ(int yaw_motor_id,int pitch_motor_id,const struct device * can_dev):yaw_motor_(yaw_motor_id,can_dev),pitch_motor_(pitch_motor_id,can_dev) {

    }
    void Init();
    void SetAngle(float yaw,float pitch);
protected:
    DjiRm3508 yaw_motor_;
    DjiRm3508 pitch_motor_;
};
#endif //MAMMOTH_AIM_H