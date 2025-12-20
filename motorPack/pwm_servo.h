//
// Created by nyuki on 2025/12/20.
//

#ifndef MAMMOTH_PWM_SERVO_H
#define MAMMOTH_PWM_SERVO_H
#include "pwm_motor.h"
class PwmServo:PwmMotor
{
protected:
    float max_pos_;
public:
    PwmServo(const device *pwm_dev, int max_frequency, int min_frequency,float max_pos, const int id=0)
        : PwmMotor(pwm_dev, max_frequency, min_frequency, id), max_pos_(max_pos)
    {
    }

    void SetPosition(float target)override;
};
#endif //MAMMOTH_PWM_SERVO_H