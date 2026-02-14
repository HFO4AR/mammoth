//
// Created by nyuki on 2025/11/19.
//

#include "shoot_ring.h"
#include "pwm_motor.h"
#include "zephyr/kernel.h"
#include "zephyr/kernel/thread_stack.h"
static const device *const u10_pwm=DEVICE_DT_GET(DT_NODELABEL(timers1));

PwmMotor u10_right(u10_pwm,0,400,0,500);
PwmMotor u10_left(u10_pwm,1,400,0,500);
K_THREAD_STACK_DEFINE(shoot_stack_area, 2048);
Shoot shoot(u10_right,u10_left,shoot_stack_area,K_THREAD_STACK_SIZEOF(shoot_stack_area));

void Shoot::Init()
{
    shoot.SetMotorRatio(10, 10);
    k_msleep(1000);
    shoot.SetMotorRatio(0, 0);
    k_thread_create(&thread_data_,
                stack_,
                stack_size_,
                ThreadEntry,
                this,NULL,NULL,
                5, 0,K_NO_WAIT);
}

void Shoot::ThreadEntry(void* p1, void* p2, void* p3)
{

}
