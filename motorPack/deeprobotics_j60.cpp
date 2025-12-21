//
// Created by nyuki on 2025/12/16.
//

#include "deeprobotics_j60.h"

#include "zephyr/sys/byteorder.h"

#include <cstring>

DeepRoboticsJ60::DeepRoboticsJ60(int id, const struct device *can_dev)
    : CanMotor(id, can_dev, J60_CMD_CONTROL_OFFSET | (id + J60_FEEDBACK_OFFSET), false)
{
    SetPositionConversionCoefficient(57.29578f);
}

bool DeepRoboticsJ60::Begin() {
    Motor::Begin();
    if (!device_is_ready(can_dev_)) return  false;

    struct can_frame frame = {0};

    // 使用宏生成 ID: 0x40 | ID
    frame.id = J60_CMD_ENABLE_OFFSET | id_;
    frame.dlc = 0;
    frame.flags = 0;

    return can_send(can_dev_, &frame, K_NO_WAIT, NULL, NULL);
}

bool DeepRoboticsJ60::Stop() {
    Motor::Stop();
    if (!device_is_ready(can_dev_)) return  false;

    struct can_frame frame = {0};

    // 使用宏生成 ID: 0x20 | ID
    frame.id = J60_CMD_DISABLE_OFFSET | id_;
    frame.dlc = 0;
    frame.flags = 0;

    return can_send(can_dev_, &frame, K_NO_WAIT, NULL, NULL);
}

void DeepRoboticsJ60::SetCurrentOpenLoop(float target) {
    SetMit(0, 0, 0, 0, target);
}

void DeepRoboticsJ60::SetMit(float target_pos, float target_spd, float kp, float kd, float t_ff) {
    if (!motor_enable_) return;
    if (!device_is_ready(can_dev_)) return;

    // 1. 单位转换
    float p_des = Deg2Rad(target_pos);
    float v_des = Rpm2Rads(target_spd);

    // 2. 限制范围
    p_des = std::clamp(p_des, P_MIN, P_MAX);
    v_des = std::clamp(v_des, V_MIN, V_MAX);
    kp    = std::clamp(kp,    KP_MIN, KP_MAX);
    kd    = std::clamp(kd,    KD_MIN, KD_MAX);
    t_ff  = std::clamp(t_ff,  T_MIN,  T_MAX);

    // 3. 映射到整型 (你的例程中是在发送函数里做的，这里逻辑一致)
    uint16_t p_int = float_to_uint(p_des, P_MIN, P_MAX, 16);
    uint16_t v_int = float_to_uint(v_des, V_MIN, V_MAX, 14);
    uint16_t kp_int = (uint16_t)kp;
    uint16_t kd_int = float_to_uint(kd, KD_MIN, KD_MAX, 8);
    uint16_t t_int = float_to_uint(t_ff, T_MIN, T_MAX, 16);

    // 4. 拼包
    uint64_t data64 = 0;
    data64 |= (uint64_t)p_int;                   // Bit 0-15
    data64 |= ((uint64_t)v_int << 16);           // Bit 16-29
    data64 |= ((uint64_t)kp_int << 30);          // Bit 30-39
    data64 |= ((uint64_t)kd_int << 40);          // Bit 40-47
    data64 |= ((uint64_t)t_int  << 48);          // Bit 48-63

    struct can_frame frame = {0};

    // 使用宏生成 ID: 0x80 | ID
    frame.id = J60_CMD_CONTROL_OFFSET | id_;
    frame.dlc = 8;
    frame.flags = 0;

    memcpy(frame.data, &data64, 8);

    can_send(can_dev_, &frame, K_NO_WAIT, NULL, NULL);
}

void DeepRoboticsJ60::UpdateFromFrame(struct can_frame *frame) {
    uint64_t data64 = 0;
    memcpy(&data64, frame->data, 8);

    uint32_t p_raw = data64 & 0xFFFFF;
    uint32_t v_raw = (data64 >> 20) & 0xFFFFF;
    uint16_t t_raw = (data64 >> 40) & 0xFFFF;
    uint8_t  temp_flag = (data64 >> 56) & 0x01;
    uint8_t  temp_val = (data64 >> 57) & 0x7F;

    // 转换 (80.0f * val / 1048575 - 40)
    float pos_rad = uint_to_float(p_raw, P_MIN, P_MAX, 20);
    float spd_rad = uint_to_float(v_raw, V_MIN, V_MAX, 20);
    float torque  = uint_to_float(t_raw, T_MIN, T_MAX, 16);

    this->epos_ = Rad2Deg(pos_rad);
    this->spd_  = Rads2Rpm(spd_rad);
    this->cur_  = torque;
    this->temp_flag_ = temp_flag;
    this->temp_ = (float)temp_val;

    // 计算多圈
    float range_deg = Rad2Deg(P_MAX - P_MIN);
    this->UpdateTotalPosition(range_deg);

    this->motor_enable_ = true;
}
// 辅助函数实现
uint32_t DeepRoboticsJ60::float_to_uint(float x, float x_min, float x_max, int bits) {
    float span = x_max - x_min;
    float offset = x_min;
    if (x < x_min) x = x_min;
    else if (x > x_max) x = x_max;
    return (uint32_t)((x - offset) * ((float)((1 << bits) - 1)) / span);
}

float DeepRoboticsJ60::uint_to_float(uint32_t x_int, float x_min, float x_max, int bits) {
    float span = x_max - x_min;
    float offset = x_min;
    return ((float)x_int) * span / ((float)((1 << bits) - 1)) + offset;
}