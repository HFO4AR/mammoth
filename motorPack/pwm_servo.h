//
// Created by nyuki on 2025/12/20.
//

#ifndef MAMMOTH_PWM_SERVO_H
#define MAMMOTH_PWM_SERVO_H
#include "pwm_motor.h"

/**
 * @brief PWM 舵机类
 * 继承自 PwmMotor，将角度映射为PWM脉宽
 */
class Servo : public PwmMotor
{
private:
    float min_angle_ = 0.0f;
    float max_angle_;     // 舵机物理最大角度 (例如 180.0)
    float mid_angle_;     // 中位角度

public:
    /**
     * @brief Servo 构造函数
     * @param pwm_dev PWM设备
     * @param channel 通道
     * @param max_angle 最大物理角度 (默认180度)
     * @param min_pulse_us 0度对应的脉宽 (默认500us)
     * @param max_pulse_us 最大角度对应的脉宽 (默认2500us)
     * @param frequency_hz 频率 (默认50Hz)
     * @param id ID
     */
    Servo(const struct device *pwm_dev, uint32_t channel,
          float max_angle = 180.0f,
          uint32_t min_pulse_us = 500, uint32_t max_pulse_us = 2500,
          uint32_t frequency_hz = 50, int id = 0);

    virtual ~Servo() = default;

    /**
     * @brief 初始化并归中
     */
    virtual bool Begin() override;

    /**
     * @brief 设置舵机角度 (核心功能)
     * @param target 目标角度 (度)
     */
    virtual void SetPosition(float target) override;

    /**
     * @brief 适配开环接口
     * 将 0.0-1.0 映射为 0度-最大角度
     */
    virtual void SetCurrentOpenLoop(float target) override;
};
#endif //MAMMOTH_PWM_SERVO_H