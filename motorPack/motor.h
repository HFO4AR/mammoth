//
// Created by nyuki on 2025/11/2.
//

#ifndef MAMMOTH_MOTOR_H
#define MAMMOTH_MOTOR_H


#include "pid.h"
#include <cstdint>
#define MOTOR_ENABLE 1
#define MOTOR_DISABLE 0

class Motor {
public:
    virtual ~Motor() = default;

    Motor (const int id):id_(id) {}
    int16_t pos_;
    int16_t spd_;
    int16_t cur_;
    int8_t temp_;
    int16_t last_pos_;
    int motor_enable_ = MOTOR_DISABLE;

    Pid pos_pid_;
    Pid spd_pid_;

    virtual void SetSpdPid(float kp, float ki, float kd, float max_output=1000.0f, float deadband=0,float kaw=-1.0f);

    virtual void SetPosPid(float kp, float ki, float kd, float max_output=1000.0f, float deadband=0,float kaw=-1.0f);

    virtual void SetCurrentOpenLoop(int target);//open loop

    virtual void SetSpeed(int target);

    virtual void SetSinglePosition(int target);

    virtual void SetPosition(int target);

    virtual void SetSpeedMaxOutput(float val);

    virtual void SetPositionMaxOutput(float val);

    virtual void SetSpeedDeadband(float val);

    virtual void SetPositionDeadband(float val);

    virtual void SetMit(float target_pos, float target_spd, float kp, float kd, float t_ff,uint16_t max_output);

    int32_t GetTotalPosition();

    virtual void UpdateTotalPosition();
protected:


    virtual void SetCurrent(int target);//close loop
    const int id_;
    int32_t total_pos_;
    int16_t round_count_;
};


#endif //MAMMOTH_MOTOR_H