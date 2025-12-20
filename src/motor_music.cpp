//
// Created by nyuki on 2025/11/27.
//

#include "motor_music.h"

#include "zephyr/kernel.h"
#include "musical_source.h"
void MotorMusic::play(const Note* musical_score, size_t length,int unit)
{
    float lock_pos = (float)motor.GetTotalPosition();

    for (size_t i = 0; i < length; i++)
    {
        const Note& note = musical_score[i];
        uint32_t duration_ms = note.duration * unit;

        // --- 休止符处理 ---
        if (note.frequency <= 0) {
            uint32_t start_time = k_uptime_get_32();
            while (k_uptime_get_32() - start_time < duration_ms) {
                motor.SetMit(lock_pos, 0, 5.0f, 8.0f, 0, max_cur_);
                k_usleep(1000); // 1ms更新一次维持力矩
            }
            continue;
        }

        // --- 正常音符处理 ---

        // 计算方波的半周期 (微秒)
        uint32_t half_period_us = 1000000 / note.frequency / 2;

        uint32_t start_time = k_uptime_get_32();

        // 在音符持续时间内，不断切换电流方向
        while (k_uptime_get_32() - start_time < duration_ms) {

            // 正半周
            motor.SetMit(lock_pos, 0, 5.0f, 4.0f, (float)volume_, max_cur_);
            k_busy_wait(half_period_us);

            // 负半周
            motor.SetMit(lock_pos, 0, 5.0f, 4.0f, -(float)volume_, max_cur_);
            k_busy_wait(half_period_us);
        }

        motor.SetMit(lock_pos, 0, 5.0f, 8.0f, 0, max_cur_);
        k_busy_wait(5000);
    }

    // 播放结束
    motor.SetMit(lock_pos, 0, 5.0f, 8.0f, 0, 2000);
    k_msleep(1000);
}

/****remote thread began*****/

void ThreadEntry(void *p1, void *p2, void *p3)
{
    MotorMusic *self = static_cast<MotorMusic*>(p1);
    while (true)
    {
        // self->play(mcdonalds_jingle, ARRAY_SIZE(mcdonalds_jingle),125);
        // self->play( windows_xp_startup, ARRAY_SIZE( windows_xp_startup),100);
        // self->play(happy_birthday,ARRAY_SIZE(happy_birthday),125);
        // self->play(happy_birthday_full,ARRAY_SIZE(happy_birthday_full),125);
        // self->play(haidilao_birthday,ARRAY_SIZE(haidilao_birthday),100);
        // self->play(cantonese_birthday,ARRAY_SIZE(cantonese_birthday),100);
        // self->play(hajimi_original,ARRAY_SIZE(hajimi_original),100);
        // self->play(blue_lotus,ARRAY_SIZE(blue_lotus),125);
        // self->play(lan_lian_ha,ARRAY_SIZE(lan_lian_ha),100);
        // self->play(mr_lawrence,ARRAY_SIZE(mr_lawrence),150);
        self->play(gao_shan_liu_shui,ARRAY_SIZE(gao_shan_liu_shui),100);
        // self->play(cang_hai_yi_sheng_xiao,ARRAY_SIZE(cang_hai_yi_sheng_xiao),150);
        self->play(bai_niao_chao_feng,ARRAY_SIZE(bai_niao_chao_feng),80);
        self->play(nan_er_dang_zi_qiang,ARRAY_SIZE(nan_er_dang_zi_qiang),120);
        // self->play(merry_christmas,ARRAY_SIZE(merry_christmas),100);
        // self->play(jingle_bells,ARRAY_SIZE(jingle_bells),120);
        // motor_music.motor.SetMit(0,0,5,8,0,2000);
        k_msleep(10);
    }
}
/****remote thread end*****/
K_THREAD_STACK_DEFINE(motor_music_stack_area, 2048);
MotorMusic motor_music(8, ptz_can_dev, 3000, 2500, motor_music_stack_area,
                       K_THREAD_STACK_SIZEOF(motor_music_stack_area));
int MotorMusic::Init()
{
    k_msleep(1000);
    // motor_music.motor.SetMit(0, 0, 5.0f, 8.0f, 0, 2000);
    k_thread_create(&thread_data_,
                stack_,
                stack_size_,
                ThreadEntry,
                this,NULL,NULL,
                8, 0,K_NO_WAIT);
    return 1;


    // k_thread_create()
}
