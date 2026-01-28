//
// Created by nyuki on 2025/12/20.
//

#ifndef MAMMOTH_PWM_MOTOR_H
#define MAMMOTH_PWM_MOTOR_H

#include <zephyr/device.h>
#include <cstdint>
#include "motor.h"

/**
 * @brief 基于PWM驱动的电机基类
 * 适用于：无刷电调(ESC)驱动的电机(如U10)、有刷电机驱动器等
 */
class PwmMotor : public Motor
{
protected:
    const struct device *pwm_dev_;
    const uint32_t channel_;

    uint32_t period_ns_;     // PWM周期 (纳秒)
    uint32_t min_pulse_ns_;  // 0% 油门对应的脉宽 (纳秒)
    uint32_t max_pulse_ns_;  // 100% 油门对应的脉宽 (纳秒)

public:
    /**
     * @brief PwmMotor 构造函数
     * @param pwm_dev PWM设备句柄 (device*)
     * @param channel PWM通道
     * @param frequency_hz PWM频率 (Hz) (例如: 舵机50, 普通电调400)
     * @param min_pulse_us 最小脉宽 (微秒) (例如: 1000)
     * @param max_pulse_us 最大脉宽 (微秒) (例如: 2000)
     * @param id 电机ID
     */
    PwmMotor(const struct device *pwm_dev, uint32_t channel,
             uint32_t frequency_hz,
             uint32_t min_pulse_us, uint32_t max_pulse_us,
             int id = 0);

    virtual ~PwmMotor() = default;

    /**
     * @brief 初始化PWM设备
     * @return true if successful
     */
    virtual bool Begin() override;

    /**
     * @brief 停止电机 (PWM输出0或最小脉宽)
     */
    virtual bool Stop() override;

    /**
     * @brief 设置PWM占空比比例
     * @param ratio 范围 [0.0, 1.0]
     */
    void SetPwmRatio(float ratio);

    /**
     * @brief 开环控制 (实现基类接口)
     * @param target 目标油门 [0.0, 1.0]
     */
    virtual void SetCurrentOpenLoop(float target) override;
};

#endif //MAMMOTH_PWM_MOTOR_H