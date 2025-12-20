//
// Created by nyuki on 2025/12/20.
//

#ifndef MAMMOTH_PWM_MOTOR_H
#define MAMMOTH_PWM_MOTOR_H
#include <zephyr/device.h>

#include "motor.h"

class PwmMotor:public Motor{
private:
    device *dev;
public:
    void SetDutyCycle(float duty_cycle);
};
#endif //MAMMOTH_PWM_MOTOR_H