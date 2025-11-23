//
// Created by nyuki on 2025/11/2.
//

#include "motor.h"
void Motor::SetSpeed(int target) {
    spd_pid_.data.actual = spd;
    spd_pid_.data.target = target;
    spd_pid_.Compuate();
    SetCurrent(spd_pid_.data.output);
}

void Motor::SetSinglePosition(int target) {
    pos_pid_.data.actual = pos;
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

void Motor::SetPosPid(float kp, float ki, float kd, float max_output, float kaw) {
    if (kaw == -1.0f) {
        pos_pid_.Init(kp, ki, kd, ki / kp, max_output);
    } else {
        pos_pid_.Init(kp, ki, kd, kaw, max_output);
    }
}

void Motor::SetSpdPid(float kp, float ki, float kd, float max_output, float kaw) {
    if (kaw == -1.0f) {
        spd_pid_.Init(kp, ki, kd, ki / kp, max_output);
    } else {
        spd_pid_.Init(kp, ki, kd, kaw, max_output);
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
    }else {
        SetCurrentOpenLoop(0);
    }
    // motor_enable_ = MOTOR_DISABLE;
}
void Motor::UpdateTotalPosition() {
    if (pos - last_pos_ > 4096) {
        round_count_--;
    } else if (pos - last_pos_ < -4096) {
        round_count_++;
    }
    last_pos_ = pos;
    total_pos_ = round_count_ * 8192 + pos;
}
int32_t Motor::GetTotalPosition()
{
    return total_pos_;
}
