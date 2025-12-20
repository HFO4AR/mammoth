//
// Created by nyuki on 2025/12/20.
//

#ifndef MAMMOTH_PWM_MOTOR_H
#define MAMMOTH_PWM_MOTOR_H
#include <zephyr/device.h>

#include "motor.h"

class PwmMotor:Motor{
private:
    const device* tim_dev_;
    int max_frequency_;//hz
    int min_frequency_;
    const int channel_;//pwm通道

public:
    /**
     * @brief 构造函数
     * @param tim_dev 定时器设备指针
     * @param max_frequency 最大频率，单位为hz
     * @param min_frequency 最小频率，单位为hz
     * @param channel PWM通道
     * @param id 电机ID
     */
    PwmMotor(const device *tim_dev,int max_frequency,int min_frequency,const int channel,const int id=0) : Motor(id), tim_dev_(tim_dev), max_frequency_(max_frequency), min_frequency_(min_frequency), channel_(channel){}

    /**
     * @brief 根据百分比设置PWM频率
     * @param percentage 百分比，范围[0,1]
     */
    void SetFrequencyByPercentage(float percentage) const;

};
#endif //MAMMOTH_PWM_MOTOR_H