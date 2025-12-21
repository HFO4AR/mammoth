//
// Created by nyuki on 2025/12/16.
//

#ifndef MAMMOTH_DEEPROBOTICS_J60_H
#define MAMMOTH_DEEPROBOTICS_J60_H
#include "can_motor.h"
#include <algorithm>
#include <cmath>

// --- 宏定义：J60 命令基地址 (高6位) ---
// 对应手册 Bit5~Bit10 的命令索引值
// 0x20 = 1 << 5
#define J60_CMD_DISABLE_OFFSET    (0x01 << 5)
// 0x40 = 2 << 5
#define J60_CMD_ENABLE_OFFSET     (0x02 << 5)
// 0x80 = 4 << 5
#define J60_CMD_CONTROL_OFFSET    (0x04 << 5)
// 0x220 = 17 << 5
#define J60_CMD_ERROR_RESET_OFFSET (0x11 << 5)

// 反馈偏移量 (手册 P26: 驱动器返回 ID = 自身ID + 0x10)
#define J60_FEEDBACK_OFFSET       0x10

class DeepRoboticsJ60 : public CanMotor {
public:
    DeepRoboticsJ60(int id, const struct device *can_dev);
    bool Begin() override;
    bool Stop() override;
    void SetMit(float target_pos, float target_spd, float kp, float kd, float t_ff);
    void SetCurrentOpenLoop(float target) override;

private:
    // J60 物理参数限制
    static constexpr float P_MIN = -40.0f;
    static constexpr float P_MAX = 40.0f;
    static constexpr float V_MIN = -40.0f;
    static constexpr float V_MAX = 40.0f;
    static constexpr float T_MIN = -40.0f;
    static constexpr float T_MAX = 40.0f;
    static constexpr float KP_MIN = 0.0f;
    static constexpr float KP_MAX = 1023.0f;
    static constexpr float KD_MIN = 0.0f;
    static constexpr float KD_MAX = 51.0f;

    uint8_t temp_flag_;
    void UpdateFromFrame(struct can_frame *frame) override;
    static uint32_t float_to_uint(float x, float x_min, float x_max, int bits);
    static float uint_to_float(uint32_t x_int, float x_min, float x_max, int bits);
};

#endif //MAMMOTH_DEEPROBOTICS_J60_H