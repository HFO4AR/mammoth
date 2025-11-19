//
// Created by nyuki on 2025/11/2.
//

#include "motor.h"
void Motor::set_spd(int target) {
    spd_pid.data.actual = spd;
    spd_pid.data.target = target;
    spd_pid.compuate();
    set_cur(spd_pid.data.output);
}

void Motor::set_single_pos(int target) {
    pos_pid.data.actual = pos;
    pos_pid.data.target = target;
    pos_pid.compuate();
    set_spd(pos_pid.data.output);
}

void Motor::set_pos(int target) {
    pos_pid.data.actual = total_pos;
    pos_pid.data.target = target;
    pos_pid.compuate();
    set_spd(pos_pid.data.output);
}

void Motor::pos_pid_init(float kp, float ki, float kd, float kaw, float max_output) {
    if (kaw == -1.0f) {
        pos_pid.init(kp, ki, kd, ki / kp, max_output);
    } else {
        pos_pid.init(kp, ki, kd, kaw, max_output);
    }
}

void Motor::spd_pid_init(float kp, float ki, float kd, float kaw, float max_output) {
    if (kaw == -1.0f) {
        spd_pid.init(kp, ki, kd, ki / kp, max_output);
    } else {
        spd_pid.init(kp, ki, kd, kaw, max_output);
    }
}

void Motor::set_spd_max_output(float val) {
    spd_pid.data.max_output = val;
}

void Motor::set_pos_max_output(float val) {
    pos_pid.data.max_output = val;
}

void Motor::set_spd_deadband(float val) {
    spd_pid.data.deadband = val;
}

void Motor::set_pos_deadband(float val) {
    pos_pid.data.deadband = val;
}
void Motor::set_cur_ol(int target) {

}

void Motor::set_cur(int target) {
    if (motor_enable) {
        set_cur_ol(target);
    }else {
        set_cur_ol(0);
    }
    motor_enable = MOTOR_DISABLE;
}
void Motor::total_pos_updata() {
    if (pos - last_pos > 4096) {
        round--;
    } else if (pos - last_pos < -4096) {
        round++;
    }
    last_pos = pos;
    total_pos = round * 8192 + pos;
}