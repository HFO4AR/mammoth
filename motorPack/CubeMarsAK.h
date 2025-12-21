//
// Created by nyuki on 2025/12/20.
//

#ifndef MAMMOTH_CUBEMARSAK_H
#define MAMMOTH_CUBEMARSAK_H

#include "can_motor.h"


// AK电机参数结构体 (参考手册 P40)
struct AKMotorParams {
    float v_max = 30.0f;
    float t_max = 18.0f;
    float p_max = 12.5f;
    float kp_max = 500.0f;
    float kd_max = 5.0f;
};

constexpr AKMotorParams AK10_9_PARAMS = {50.0f,65.0f};
constexpr AKMotorParams AK80_6_PARAMS = {45.0f,15.0f};
constexpr AKMotorParams AK70_10_PARAMS = {50.0f,25.0f};

class CubeMarsAK : public CanMotor {
public:
    /*
     * @brief 构造函数
     * @param id 电机CAN ID
     * @param can_dev CAN设备指针
     * @param params 电机限位参数
     */
    CubeMarsAK(int id, const struct device *can_dev, AKMotorParams params);

    bool SetMit(float target_pos, float target_spd, float kp, float kd, float t_ff);



    void SetCurrentOpenLoop(float target) override;

    bool Begin()override;

    bool Stop()override;

    bool SetZero() const;

private:
    AKMotorParams params_;
    void UpdateFromFrame(struct can_frame *frame) override;
    // 内部转换辅助函数
    static int float_to_uint(float x, float x_min, float x_max, int bits);
    static float uint_to_float(int x_int, float x_min, float x_max, int bits);
};

#endif //MAMMOTH_CUBEMARSAK_H