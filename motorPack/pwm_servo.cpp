//
// Created by nyuki on 2025/12/20.
//

#include "pwm_servo.h"

Servo::Servo(const struct device *pwm_dev, uint32_t channel,
             float max_angle,
             uint32_t min_pulse_us, uint32_t max_pulse_us,
             uint32_t frequency_hz, int id)
    : PwmMotor(pwm_dev, channel, frequency_hz, min_pulse_us, max_pulse_us, id),
      max_angle_(max_angle)
{
    mid_angle_ = max_angle_ / 2.0f;
    // 设置基类 Motor 的角度转换系数
    // 1.0f 表示 epos_ (电气角度变量) 直接存储 机械角度 (度)
    SetPositionConversionCoefficient(1.0f);
}

bool Servo::Begin()
{
    // 调用父类 PwmMotor 初始化硬件
    if (!PwmMotor::Begin()) {
        return false;
    }

    // 初始化成功后，自动归中，防止舵机乱动
    SetPosition(mid_angle_);
    return true;
}

void Servo::SetPosition(float target)
{
    if (!motor_enable_) return;

    // 1. 角度软限幅
    if (target < min_angle_) target = min_angle_;
    if (target > max_angle_) target = max_angle_;

    // 2. 更新基类状态 (让 GetPosition() 返回正确的值)
    // 因为系数是1.0，所以 Pos2Epos(target) 等于 target
    epos_ = Pos2Epos(target);

    // 3. 计算比例 [0.0, 1.0]
    // Ratio = (Target - MinAngle) / (MaxAngle - MinAngle)
    float ratio = (target - min_angle_) / (max_angle_ - min_angle_);

    // 4. 调用父类方法输出 PWM
    SetPwmRatio(ratio);
}

void Servo::SetCurrentOpenLoop(float target)
{
    // 假设 target 是 0.0 ~ 1.0 的控制量
    // 映射到 0度 ~ MaxAngle
    float angle = target * (max_angle_ - min_angle_) + min_angle_;
    SetPosition(angle);
}