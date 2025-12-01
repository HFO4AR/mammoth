//
// Created by nyuki on 2025/11/19.
//

#ifndef MAMMOTH_AIM_H
#define MAMMOTH_AIM_H
#include "dji_m3508.h"

class PTZ {
public:
    typedef struct {
        int max_angle;
        int min_angle;
        int angle;
        int zero_angle;
        int32_t target;
    }data_t;

    PTZ(int yaw_motor_id,int pitch_motor_id,const struct device * can_dev):yaw_motor_(yaw_motor_id,can_dev),pitch_motor_(pitch_motor_id,can_dev) {}
    void Init();
    void SetAngle(float yaw,float pitch);
    DjiM3508 yaw_motor_;
    DjiM3508 pitch_motor_;
    data_t yaw_data_;
    data_t pitch_data_;

    
private:
    void InitMotorDirection(bool is_positive_direction);
    void CheckMotorOnline();

};
#endif //MAMMOTH_AIM_H