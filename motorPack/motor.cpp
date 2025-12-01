//
// Created by nyuki on 2025/11/2.
//

#include "motor.h"

#include <cmath>

void Motor::SetSpeed(int target) {
    spd_pid_.data.actual = spd_;
    spd_pid_.data.target = target;
    spd_pid_.Compuate();
    SetCurrent(spd_pid_.data.output);
}

void Motor::SetSinglePosition(int target) {
    pos_pid_.data.actual = pos_;
    pos_pid_.data.target = target;
    pos_pid_.Compuate();
    SetSpeed(pos_pid_.data.output);
}

void Motor::SetPosition(int target) {
    pos_pid_.data.actual = total_pos_;
    pos_pid_.data.target = target;
    pos_pid_.Compuate();
    SetSpeed(pos_pid_.data.output);
}

void Motor::SetPosPid(float kp, float ki, float kd, float max_output,float deadband,float kaw) {
    if (kaw == -1.0f) {
        pos_pid_.Init(kp, ki, kd, ki / kp, max_output,deadband);
    } else {
        pos_pid_.Init(kp, ki, kd, kaw, max_output,deadband);
    }
}

void Motor::SetSpdPid(float kp, float ki, float kd, float max_output,float deadband, float kaw) {
    if (kaw == -1.0f) {
        spd_pid_.Init(kp, ki, kd, ki / kp, max_output,deadband);
    } else {
        spd_pid_.Init(kp, ki, kd, kaw, max_output,deadband);
    }
}

void Motor::SetSpeedMaxOutput(float val) {
    spd_pid_.data.max_output = val;
}

void Motor::SetPositionMaxOutput(float val) {
    pos_pid_.data.max_output = val;
}

void Motor::SetSpeedDeadband(float val) {
    spd_pid_.data.deadband = val;
}

void Motor::SetPositionDeadband(float val) {
    pos_pid_.data.deadband = val;
}
void Motor::SetCurrentOpenLoop(int target) {

}

void Motor::SetCurrent(int target) {
    if (motor_enable_) {
        SetCurrentOpenLoop(target);
        // motor_enable_ = false;
    }else {
        SetCurrentOpenLoop(0);
        return;
    }
    // motor_enable_ = MOTOR_DISABLE;
}
void Motor::UpdateTotalPosition() {
    if (pos_ - last_pos_ > 4096) {
        round_count_--;
    } else if (pos_ - last_pos_ < -4096) {
        round_count_++;
    }
    last_pos_ = pos_;
    total_pos_ = round_count_ * 8192 + pos_;
}

void Motor::SetMit(float target_pos, float target_spd, float kp, float kd, float t_ff, uint16_t max_output)
{
    float output_f = kp * (target_pos - (float)total_pos_) +
                     kd * (target_spd - (float)spd_) +
                     t_ff;

    int output = (int)output_f;

    if (output > max_output)
    {
        output = max_output;
    }
    else if (output < -max_output)
    {
        output = -max_output;
    }

    SetCurrent(output);
}

int32_t Motor::GetTotalPosition()
{
    return total_pos_;
}

