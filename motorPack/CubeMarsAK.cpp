//
// Created by nyuki on 2025/12/20.
//

#include "CubeMarsAK.h"

#include <algorithm>
#include <cstring>

CubeMarsAK::CubeMarsAK(int id, const struct device *can_dev, AKMotorParams params)
    : CanMotor(id, can_dev, id, false), params_(params)
{

}

bool CubeMarsAK::Begin()
{
    if (!device_is_ready(can_dev_)) return false;

    struct can_frame frame = {0};
    frame.id = id_; // 标准帧 ID
    frame.dlc = 8;
    frame.flags = 0; // 标准帧

    // 手册 P39: 进入电机控制模式
    uint8_t cmd[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFC};
    memcpy(frame.data, cmd, 8);

    can_send(can_dev_, &frame, K_NO_WAIT, NULL, NULL);
    return true;
}

bool CubeMarsAK::Stop() {
    if (!device_is_ready(can_dev_)) return false;

    struct can_frame frame = {0};
    frame.id = id_;
    frame.dlc = 8;

    // 手册 P39: 退出电机控制模式
    uint8_t cmd[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFD};
    memcpy(frame.data, cmd, 8);

    return can_send(can_dev_, &frame, K_NO_WAIT, NULL, NULL);
}

bool CubeMarsAK::SetZero() const
{
    if (!device_is_ready(can_dev_)) return false;

    struct can_frame frame = {0};
    frame.id = id_;
    frame.dlc = 8;

    // 手册 P39: 设置当前位置为0
    uint8_t cmd[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFE};
    memcpy(frame.data, cmd, 8);

    return can_send(can_dev_, &frame, K_NO_WAIT, NULL, NULL);
}

void CubeMarsAK::SetCurrentOpenLoop(float target) {
    SetMit(0.0f, 0.0f, 0.0f, 0.0f, target);
}

bool CubeMarsAK::SetMit(float target_pos, float target_spd, float kp, float kd, float t_ff) {
    if (!device_is_ready(can_dev_)) return false;

    // 1. 单位转换
    float p_des_rad = Deg2Rad(target_pos);
    float v_des_rad = Rpm2Rads(target_spd); // rpm -> rad/s

    // 2. 限制范围
    p_des_rad = std::clamp(p_des_rad, -params_.p_max, params_.p_max);
    v_des_rad = std::clamp(v_des_rad, -params_.v_max, params_.v_max);
    kp        = std::clamp(kp,        -params_.kp_max, params_.kp_max);
    kd        = std::clamp(kd,        -params_.kd_max, params_.kd_max);
    t_ff      = std::clamp(t_ff,      -params_.t_max,  params_.t_max);

    // 3. 浮点转整型 (手册 P41)
    int p_int = float_to_uint(p_des_rad, -params_.p_max, params_.p_max, 16);
    int v_int = float_to_uint(v_des_rad, -params_.v_max, params_.v_max, 12);
    int kp_int = float_to_uint(kp, -params_.kp_max, params_.kp_max, 12);
    int kd_int = float_to_uint(kd, -params_.kd_max, params_.kd_max, 12);
    int t_int = float_to_uint(t_ff, -params_.t_max, params_.t_max, 12);

    // 4. 打包 CAN 帧
    struct can_frame frame = {0};
    frame.id = id_;
    frame.dlc = 8;

    frame.data[0] = p_int >> 8;
    frame.data[1] = p_int & 0xFF;
    frame.data[2] = v_int >> 4;
    frame.data[3] = ((v_int & 0xF) << 4) | (kp_int >> 8);
    frame.data[4] = kp_int & 0xFF;
    frame.data[5] = kd_int >> 4;
    frame.data[6] = ((kd_int & 0xF) << 4) | (t_int >> 8);
    frame.data[7] = t_int & 0xFF;

    return can_send(can_dev_, &frame, K_NO_WAIT, NULL, NULL);
}

// 核心：解析反馈
void CubeMarsAK::UpdateFromFrame(struct can_frame *frame) {

    int p_int = (frame->data[1] << 8) | frame->data[2];
    int v_int = (frame->data[3] << 4) | (frame->data[4] >> 4);
    int t_int = ((frame->data[4] & 0xF) << 8) | frame->data[5];
    int temp_int = frame->data[6];
    int error_code = frame->data[7];

    // 2. 转换为物理单位
    float pos_rad = uint_to_float(p_int, -params_.p_max, params_.p_max, 16);
    float spd_rad = uint_to_float(v_int, -params_.v_max, params_.v_max, 12);
    float torque  = uint_to_float(t_int, -params_.t_max, params_.t_max, 12);
    float temp    = (float)temp_int - 40.0f;

    this->epos_ = Rad2Deg(pos_rad);

    // 基类使用 RPM
    this->spd_  = Rads2Rpm(spd_rad);

    // 基类 cur_ 通常存电流，但 AK 返回的是扭矩(Nm)。
    // 这里存入扭矩，如果需要电流需除以 Kt (扭矩常数)
    this->cur_  = torque;

    this->temp_ = temp;
    this->motor_enable_ = true; // 收到反馈视为已使能/在线

    // 4. 更新多圈位置 (处理 -12.5 ~ 12.5 rad 的过零/溢出)
    // 注意：AK 的 MIT 模式返回的是单圈(或受限多圈)绝对位置
    // Motor::UpdateTotalPosition 需要传入 "最大单圈计数值" 对应的物理量
    // 这里的 range 是 p_max - p_min = 25.0 rad ≈ 1432 度
    // 传入该范围用于判定圈数跳变
    float range_deg = Rad2Deg(params_.p_max - (-params_.p_max));
    this->UpdateTotalPosition(range_deg);
}

// 内部实现：float -> uint
int CubeMarsAK::float_to_uint(float x, float x_min, float x_max, int bits) {
    float span = x_max - x_min;
    float offset = x_min;
    if (x < x_min) x = x_min;
    else if (x > x_max) x = x_max;
    return (int)((x - offset) * ((float)((1 << bits) - 1)) / span);
}

// 内部实现：uint -> float
float CubeMarsAK::uint_to_float(int x_int, float x_min, float x_max, int bits) {
    float span = x_max - x_min;
    float offset = x_min;
    return ((float)x_int) * span / ((float)((1 << bits) - 1)) + offset;
}