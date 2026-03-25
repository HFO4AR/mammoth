//
// Created by nyuki on 2025/11/2.
//

#include "motor.h"

#include <algorithm>
#include <cmath>

#include "zephyr/kernel.h"
#include "get_time.h"


void Motor::SetSpeed(float target)
{
    spd_pid_.data.actual = spd_;
    spd_pid_.data.target = target;
    spd_pid_.Compuate();
    SetCurrent(spd_pid_.data.output);
}

void Motor::SetPosition(float target)
{
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

void Motor::SetPositionTrapezoid(float target, float max_speed, float accel, float dt, float decel)
{
    if (decel < 0) decel = accel;

    // 1. 初始化：如果是第一次运行，或者长时间未更新，从当前实际位置开始规划
    if (!trap_init_)
    {
        last_trap_pos_ = GetTotalPosition();
        last_trap_vel_ = GetSpeed(); // 或者从0开始
        trap_init_ = true;
    }

    // 2. 计算当前位移偏差
    float current_pos = GetTotalPosition();
    float pos_error = target - current_pos;
    float dir = (pos_error > 0) ? 1.0f : -1.0f;
    pos_error = std::abs(pos_error);

    // 3. 计算“刹车距离” (Stopping Distance)
    // 公式: s = v^2 / (2 * a)
    // 注意：这里的 v 是 RPM，需要保持单位一致或转换。这里假设 accel 单位是 RPM/s
    float current_vel_abs = std::abs(last_trap_vel_);
    float stop_dist = (current_vel_abs * current_vel_abs) / (2.0f * decel);

    // 将角度误差转换为“预计停止需要的角度”
    // 注意：这里需要考虑 RPM 到 Deg/s 的转换。 1 RPM = 6 Deg/s
    // 停止所需时间 t = v / decel
    // 停止所需角度 stop_angle = (v * 6) * (t / 2) = (v * 6) * (v / (2 * decel))
    float stop_angle = (current_vel_abs * 6.0f) * (current_vel_abs / (2.0f * decel));

    float target_vel = 0;

    if (pos_error < 0.01f)
    {
        // 到达死区
        target_vel = 0;
        last_trap_vel_ = 0;
    }
    else if (pos_error <= stop_angle)
    {
        // 4. 减速阶段：目标速度向0靠拢
        target_vel = std::sqrt(2.0f * decel * pos_error / 6.0f) * dir;
    }
    else
    {
        // 5. 加速或匀速阶段
        target_vel = max_speed * dir;
    }

    // 6. 速度斜坡限制 (限制最大加速度)
    float vel_step = accel * dt;
    float vel_error = target_vel - last_trap_vel_;

    if (vel_error > vel_step)
    {
        last_trap_vel_ += vel_step;
    }
    else if (vel_error < -vel_step)
    {
        last_trap_vel_ -= vel_step;
    }
    else
    {
        last_trap_vel_ = target_vel;
    }

    // 7. 最终限幅
    last_trap_vel_ = std::clamp(last_trap_vel_, -max_speed, max_speed);

    SetSpeed(last_trap_vel_);
}

void Motor::SetPositionProfile(float target_pos, float max_spd, float accel, float decel,float deadband,float dead_spd)
{
    // if (pos_pid_initialized_&&std::abs(target_pos-Epos2Pos(total_epos_))<1)
    // {
    //     float temp=pos_pid_.data.deadband;
    //     SetPositionDeadband(deadband);
    //     SetPosition(target_pos);
    //     SetPositionDeadband(temp);
    //     return;
    // }
    if (decel < 0) decel = accel;
    //计算时间间隔
    uint32_t now = GetTime();
    if (last_timestamp_ == 0) last_timestamp_ = now;
    float dt = (now - last_timestamp_) / 1000.0f;
    last_timestamp_ = now;

    if (dt <= 0) dt = 0.001f;

    float current_pos = GetTotalPosition();
    float pos_error = target_pos - current_pos;
    float dist = std::abs(pos_error);
    float dir = (pos_error > 0) ? 1.0f : -1.0f;

    //减速限速
    float v_dec_limit = std::sqrt(decel * dist / 3.0f);

    //起步加速限速
    float v_acc_limit = std::abs(last_cmd_spd_) + accel * dt;

    //目标速度限幅
    float target_vel = max_spd;
    target_vel = std::min(target_vel, v_dec_limit);
    target_vel = std::min(target_vel, v_acc_limit);

    //如果当前指令方向与目标方向相反，先减速到0
    if ((last_cmd_spd_ * dir) < 0) {
        target_vel = std::abs(last_cmd_spd_) - accel * dt;
        if (target_vel < 0) target_vel = 0;
        //此时维持原方向，直到速度减到0后再转向
        target_vel = target_vel * (last_cmd_spd_ > 0 ? 1.0f : -1.0f);
    } else {
        target_vel = target_vel * dir;
    }

    if (dist < deadband && std::abs(target_vel) < dead_spd) {
        target_vel = 0;
    }

    last_cmd_spd_ = target_vel;
    SetSpeed(target_vel);
}

void Motor::SetPosPid(float kp, float ki, float kd, float max_output, float deadband, float kaw)
{
    pos_pid_initialized_= true;
    if (kaw == -1.0f)
    {
        pos_pid_.Init(kp, ki, kd, ki / kp, max_output, deadband);
    }
    else
    {
        pos_pid_.Init(kp, ki, kd, kaw, max_output, deadband);
    }
}

void Motor::SetSpdPid(float kp, float ki, float kd, float max_output, float deadband, float kaw)
{
    spd_pid_initialized_= true;
    if (kaw == -1.0f)
    {
        spd_pid_.Init(kp, ki, kd, ki / kp, max_output, deadband);
    }
    else
    {
        spd_pid_.Init(kp, ki, kd, kaw, max_output, deadband);
    }
}

void Motor::SetSpeedMaxOutput(float val)
{
    spd_pid_.data.max_output = val;
}

void Motor::SetPositionMaxOutput(float val)
{
    pos_pid_.data.max_output = val;
}

void Motor::SetSpeedDeadband(float val)
{
    spd_pid_.data.deadband = val;
}

void Motor::SetPositionDeadband(float val)
{
    pos_pid_.data.deadband = val;
}

void Motor::SetCurrentOpenLoop(float target)
{
}

void Motor::SetCurrent(float target)
{
    if (motor_enable_)
    {
        SetCurrentOpenLoop(target);
    }
    else
    {
        SetCurrentOpenLoop(0);
    }
    // motor_enable_ = MOTOR_DISABLE;
}

void Motor::UpdateTotalPosition(float period)
{
    if (epos_ - last_pos_ > period / 2)
    {
        cycle_count_--;
    }
    else if (epos_ - last_pos_ < -period / 2)
    {
        cycle_count_++;
    }
    last_pos_ = epos_;
    // total_epos_ = static_cast<float>(cycle_count_) * period + epos_;
    total_epos_+= epos_- last_pos_;//TODO:未验证
}

bool Motor::SetMit(const float target_pos, const float target_spd, const float kp, const float kd, const float t_ff,
                   const float max_output)
{
    float output = kp * (target_pos - (float)total_epos_) +
        kd * (target_spd - (float)spd_) +
        t_ff;

    //输出限幅
    // if (output > max_output)
    // {
    //     output = max_output;
    // }
    // else if (output < -max_output)
    // {
    //     output = -max_output;
    // }
    output = std::clamp(output, -max_output, max_output);

    SetCurrent(output);
    return true;
}
