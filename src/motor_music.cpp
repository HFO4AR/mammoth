//
// Created by nyuki on 2025/11/27.
//

#include "motor_music.h"

#include "zephyr/kernel.h"
#include "musical_source.h"
void MotorMusic::play(const Note* musical_score, size_t length,int unit)
{
    motor.UpdateTotalPosition();
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
void MotorMusic::sweep_test(int start_freq, int end_freq, int duration_ms)
{
    motor.UpdateTotalPosition();
    float lock_pos = (float)motor.GetTotalPosition();

    int steps = 100; // 扫频分多少步
    int step_time_us = (duration_ms * 1000) / steps;
    float freq_increment = (float)(end_freq - start_freq) / steps;

    for (int i = 0; i < steps; i++) {
        float current_freq = start_freq + (freq_increment * i);

        // 计算当前频率下的半周期
        uint32_t half_period_us = 1000000 / (int)current_freq / 2;

        // 在这个频率步长内播放多少个周期？
        // step_time_us / (half_period_us * 2)
        int cycles = step_time_us / (half_period_us * 2);

        // 至少播放一个周期
        if (cycles < 1) cycles = 1;

        for (int c = 0; c < cycles; c++) {
            // 正半周
            motor.SetMit(lock_pos, 0, 5.0f, 0.5f, volume_, max_cur_);
            k_busy_wait(half_period_us);

            // 负半周
            motor.SetMit(lock_pos, 0, 5.0f, 0.5f, -volume_, max_cur_);
            k_busy_wait(half_period_us);
        }
    }

    // 停止
    motor.SetMit(lock_pos, 0, 0, 0, 0, 0);
}
MotorMusic motor_music(1,ptz_can_dev,3000,2500);
int MusicInit()
{
    k_msleep(1000);
    // motor_music.motor.SetMit(0, 0, 5.0f, 8.0f, 0, 2000);
    while (true)
    {
        // motor_music.play(test_chromatic_scale,ARRAY_SIZE(test_chromatic_scale),125);
        // motor_music.play(happy_birthday,ARRAY_SIZE(happy_birthday),125);
        // motor_music.play(air_on_g_string,ARRAY_SIZE(air_on_g_string),150);
        // motor_music.play(canon_in_d, ARRAY_SIZE(canon_in_d),120);
        motor_music.play(mcdonalds_jingle, ARRAY_SIZE(mcdonalds_jingle),125);
        // motor_music.play( windows_xp_startup, ARRAY_SIZE( windows_xp_startup),100);
        motor_music.play(happy_birthday_full,ARRAY_SIZE(happy_birthday_full),125);
        motor_music.play(bach_cello_suite_no1,ARRAY_SIZE(bach_cello_suite_no1),90);
        motor_music.play(bach_minuet_g,ARRAY_SIZE(bach_minuet_g),140);
        // motor_music.play(haidilao_birthday,ARRAY_SIZE(haidilao_birthday),100);
        // motor_music.play(cantonese_birthday,ARRAY_SIZE(cantonese_birthday),100);
        // motor_music.play(hajimi_original,ARRAY_SIZE(hajimi_original),100);
        // motor_music.play(blue_lotus,ARRAY_SIZE(blue_lotus),125);
        // motor_music.play(lan_lian_ha,ARRAY_SIZE(lan_lian_ha),100);
        motor_music.play(mr_lawrence,ARRAY_SIZE(mr_lawrence),150);
        motor_music.play(gao_shan_liu_shui,ARRAY_SIZE(gao_shan_liu_shui),100);
        // motor_music.play(cang_hai_yi_sheng_xiao,ARRAY_SIZE(cang_hai_yi_sheng_xiao),150);
        motor_music.play(bai_niao_chao_feng,ARRAY_SIZE(bai_niao_chao_feng),80);
        // motor_music.play(nan_er_dang_zi_qiang,ARRAY_SIZE(nan_er_dang_zi_qiang),120);
        motor_music.play(merry_christmas,ARRAY_SIZE(merry_christmas),100);
        // motor_music.play(jingle_bells,ARRAY_SIZE(jingle_bells),120);
        // motor_music.motor.SetMit(0,0,5,8,0,2000);
        k_msleep(10);
    }
    // k_thread_create()
}
