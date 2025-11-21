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
    int16_t pos;
    int16_t spd;
    int16_t cur;
    int8_t temp;
    int motor_enable_ = MOTOR_DISABLE;

    Pid pos_pid_;
    Pid spd_pid_;

    virtual void SpdPidInit(float kp, float ki, float kd, float kaw=-1.0f, float max_output=1000.0f);

    virtual void PosPidInit(float kp, float ki, float kd, float kaw=-1.0f, float max_output=1000.0f);

    virtual void SetCurrentOpenLoop(int target);//open loop

    virtual void SetSpeed(int target);

    virtual void SetSinglePosition(int target);

    virtual void SetPosition(int target);

    virtual void SetSpeedMaxOutput(float val);

    virtual void SetPositionMaxOutput(float val);

    virtual void SetSpeedDeadband(float val);

    virtual void SetPositionDeadband(float val);
protected:
    virtual void UpdateTotalPosition();

    virtual void SetCurrent(int target);//close loop
    const int id_;
    int32_t total_position_ = 0;
    int16_t last_position_ = 0;
    int16_t round_count_ = 0;
};


#endif //MAMMOTH_MOTOR_H