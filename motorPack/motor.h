//
// Created by nyuki on 2025/11/2.
//

#ifndef CPP3508_MOTOR_H
#define CPP3508_MOTOR_H

#include "main.h"
#include "pid.h"
#include <cstdint>
#define MOTOR_ENABLE 1
#define MOTOR_DISABLE 0

class Motor {
public:
    Motor (const int id):id(id) {}
    int16_t pos;
    int16_t spd;
    int16_t cur;
    int8_t temp;
    int motor_enable=MOTOR_DISABLE;

    Pid pos_pid;
    Pid spd_pid;

    virtual void spd_pid_init(float kp, float ki, float kd, float kaw=-1.0f, float max_output=1000.0f);

    virtual void pos_pid_init(float kp, float ki, float kd, float kaw=-1.0f, float max_output=1000.0f);

    virtual void set_cur_ol(int target);//open loop

    virtual void set_spd(int target);

    virtual void set_single_pos(int target);

    virtual void set_pos(int target);

    virtual void set_spd_max_output(float val);

    virtual void set_pos_max_output(float val);

    virtual void set_spd_deadband(float val);

    virtual void set_pos_deadband(float val);
protected:
    virtual void total_pos_updata();
    virtual void set_cur(int target);//close loop
    const int id;
    int32_t total_pos=0;
    int16_t last_pos=0;
    int16_t round=0;
};


#endif //CPP3508_MOTOR_H