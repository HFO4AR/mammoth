//
// Created by nyuki on 2025/12/20.
//

#include "pwm_servo.h"

void PwmServo::SetPosition(float target)
{
    SetFrequencyByPercentage(target/max_pos_);
}