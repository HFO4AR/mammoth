//
// Created by nyuki on 2025/12/20.
//

#include "pwm_motor.h"
#include <algorithm>
#include <zephyr/drivers/pwm.h>

void PwmMotor::SetFrequencyByPercentage(float percentage) const
{
    // 限制percentage在0-1范围内
    percentage = std::clamp(percentage, 0.0f, 1.0f);
    
    // 线性映射：根据最大最小频率计算目标频率
    int target_frequency = min_frequency_ + static_cast<int>(percentage * (max_frequency_ - min_frequency_));
    
    // 在这里添加实际的PWM设置代码
    // 示例（需要根据实际Zephyr PWM API调整）：
    pwm_set(tim_dev_, 0, target_frequency, target_frequency/2, PWM_POLARITY_NORMAL);
}