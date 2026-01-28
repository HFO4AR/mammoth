//
// Created by nyuki on 2025/11/19.
//

#ifndef MAMMOTH_SHOOT_H
#define MAMMOTH_SHOOT_H
#include "pwm_motor.h"
#include "zephyr/arch/arch_interface.h"
#include "zephyr/kernel/thread.h"

class Shoot
{
public:
    Shoot(const PwmMotor motor_right, const PwmMotor motor_left, k_thread_stack_t* stack, size_t stack_size) :
        motor_right_(motor_right), motor_left_(motor_left), stack_size_(stack_size), stack_(stack)
    {
    }

    void Init();
    void Push();

private:
    PwmMotor motor_right_;
    PwmMotor motor_left_;
    void SetMotorRatio(float ratio_right, float ratio_left);

    static void ThreadEntry(void* p1, void* p2, void* p3);
    k_thread_stack_t* stack_;
    size_t stack_size_;
    struct k_thread thread_data_;
};
#endif //MAMMOTH_SHOOT_H
