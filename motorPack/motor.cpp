//
// Created by nyuki on 2025/11/2.
//

#include "motor.h"

void Motor::SetSpeed(float target) {
    spd_pid_.data.actual = spd_;
    spd_pid_.data.target = target;
    spd_pid_.Compuate();
    SetCurrent(spd_pid_.data.output);
}

void Motor::SetPosition(float target) {
    target = Pos2Epos(target);
    pos_pid_.data.actual = total_epos_;
    pos_pid_.data.target = target;
    pos_pid_.Compuate();
    SetSpeed(pos_pid_.data.output);
}

void Motor::SetPositionSingleLoop(float target)
{
    target = Pos2Epos(target);
    pos_pid_.data.actual = total_epos_;
    pos_pid_.data.target = target;
    pos_pid_.Compuate();
    SetCurrent(pos_pid_.data.output);
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
void Motor::SetCurrentOpenLoop(float target) {

}

void Motor::SetCurrent(float target) {
    if (motor_enable_) {
        SetCurrentOpenLoop(target);
        // motor_enable_ = false;
    }else {
        SetCurrentOpenLoop(0);
        return;
    }
    // motor_enable_ = MOTOR_DISABLE;
}
void Motor::UpdateTotalPosition(float max_single_epos) {
    if (epos_ - last_pos_ > max_single_epos/2) {
        round_count_--;
    } else if (epos_ - last_pos_ < -max_single_epos/2) {
        round_count_++;
    }
    last_pos_ = epos_;
    total_epos_ = round_count_ * max_single_epos + epos_;
}

void Motor::SetMit(const float target_pos, const float target_spd, const float kp, const float kd, const float t_ff, const uint16_t max_output)
{
    const float output_f = kp * (target_pos - (float)total_epos_) +
                     kd * (target_spd - (float)spd_) +
                     t_ff;

    int output = static_cast<int>(output_f);

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


