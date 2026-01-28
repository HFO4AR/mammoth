//
// Created by nyuki on 2025/12/20.
//

#include "pwm_motor.h"
#include <zephyr/drivers/pwm.h>

PwmMotor::PwmMotor(const struct device *pwm_dev, uint32_t channel,
                   uint32_t frequency_hz,
                   uint32_t min_pulse_us, uint32_t max_pulse_us,
                   int id)
    : Motor(id), pwm_dev_(pwm_dev), channel_(channel)
{
    // 计算周期 (ns) = 1秒 / 频率
    // 1 s = 1,000,000,000 ns
    if (frequency_hz > 0) {
        period_ns_ = 1000000000U / frequency_hz;
    } else {
        period_ns_ = 20000000U; // 默认 50Hz 防止除0
    }

    // 将微秒转换为纳秒
    min_pulse_ns_ = min_pulse_us * 1000U;
    max_pulse_ns_ = max_pulse_us * 1000U;
}

bool PwmMotor::Begin()
{
    if (!device_is_ready(pwm_dev_)) {
        return false;
    }
    motor_enable_ = true;

    // 初始状态：输出最小脉宽（油门归零）
    SetPwmRatio(0.0f);

    return true;
}

bool PwmMotor::Stop()
{
    motor_enable_ = false;
    // 停止时关闭PWM输出 (脉宽设为0)
    return pwm_set(pwm_dev_, channel_, period_ns_, 0, PWM_POLARITY_NORMAL) == 0;
}

void PwmMotor::SetPwmRatio(float ratio)
{
    if (!motor_enable_) return;

    // 1. 限幅 [0, 1]
    if (ratio < 0.0f) ratio = 0.0f;
    if (ratio > 1.0f) ratio = 1.0f;

    // 2. 映射到脉宽 ns
    // Pulse = Min + Ratio * (Max - Min)
    uint32_t pulse_ns = min_pulse_ns_ + (uint32_t)(ratio * (max_pulse_ns_ - min_pulse_ns_));

    // 3. 输出 (Zephyr API)
    pwm_set(pwm_dev_, channel_, period_ns_, pulse_ns, PWM_POLARITY_NORMAL);
}

void PwmMotor::SetCurrentOpenLoop(float target)
{
    // 记录状态供 GetCurrent() 查询 (虽然实际是油门比例)
    cur_ = target;
    SetPwmRatio(target);
}