//
// Created by nyuki on 2025/11/19.
//

#ifndef MAMMOTH_AIM_H
#define MAMMOTH_AIM_H
#include "dji_rm3508.h"

class PTZ {
public:
    typedef struct {
        int max_angle;
        int min_angle;
        int angle;
    }data_t;

    PTZ(int yaw_motor_id,int pitch_motor_id,const struct device * can_dev):yaw_motor_(yaw_motor_id,can_dev),pitch_motor_(pitch_motor_id,can_dev) {}
    void Init();
    void SetAngle(float yaw,float pitch);
    DjiRm3508 yaw_motor_;
    DjiRm3508 pitch_motor_;
    data_t yaw_data_;
    data_t pitch_data_;
    
private:
    void InitMotorDirection(bool is_positive_direction);
    void MotorOnlineCheck();

};
#endif //MAMMOTH_AIM_H