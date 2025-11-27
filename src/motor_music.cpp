//
// Created by nyuki on 2025/11/27.
//

#include "motor_music.h"

#include "zephyr/kernel.h"
#define C4  262
#define Cs4 277
#define D4  294
#define Ds4 311
#define E4  330
#define F4  349
#define Fs4 370
#define G4  392
#define Gs4 415
#define A4  440
#define As4 466
#define B4  494

#define C5  523
#define Cs5 554
#define D5  587
#define Ds5 622
#define E5  659
#define F5  698
#define Fs5 740
#define G5  784
#define Gs5 831
#define A5  880
#define As5 932
#define B5  988

#define C6  1047
#define Cs6 1109
#define D6  1175
#define Ds6 1245
#define E6  1319
#define F6  1397
#define Fs6 1480
#define G6  1568
#define Gs6 1661
#define A6  1760
#define As6 1865
#define B6  1976

#define C7  2093
#define Cs7 2217
#define D7  2349
#define Ds7 2489
#define E7  2637
#define F7  2794
#define Fs7 2960
#define G7  3136
#define Gs7 3322
#define A7  3520
#define As7 3729
#define B7  3951

// #define UNIT 125

static const Note happy_birthday[] = {
    // === 第一句: Happy Birthday to You ===
    {G4, 1}, {0, 1},  // Hap-
    {G4, 1}, {0, 1},  // py
    {A4, 2}, {0, 1},  // Birth-
    {G4, 2}, {0, 1},  // day
    {C5, 2}, {0, 1},  // to
    {B4, 4}, {0, 2},  // You (长音)

    // === 第二句: Happy Birthday to You ===
    {G4, 1}, {0, 1},  // Hap-
    {G4, 1}, {0, 1},  // py
    {A4, 2}, {0, 1},  // Birth-
    {G4, 2}, {0, 1},  // day
    {D5, 2}, {0, 1},  // to
    {C5, 4}, {0, 2},  // You (长音)

    // === 第三句: Happy Birthday Dear [Name] (高潮部分) ===
    {G4, 1}, {0, 1},  // Hap-
    {G4, 1}, {0, 1},  // py
    {G5, 2}, {0, 1},  // Birth- (高音索)
    {E5, 2}, {0, 1},  // day
    {C5, 2}, {0, 1},  // Dear
    {B4, 2}, {0, 1},  // Na-
    {A4, 4}, {0, 2},  // me (长音)

    // === 第四句: Happy Birthday to You ===
    {F5, 1}, {0, 1},  // Hap- (注意这里用了F5，法)
    {F5, 1}, {0, 1},  // py
    {E5, 2}, {0, 1},  // Birth-
    {C5, 2}, {0, 1},  // day
    {D5, 2}, {0, 1},  // to
    {C5, 6}, {0, 4},  // You (最后结束，拖长一点)

    // 结束休止
    {0, 10}
};
// 海底捞名曲：《对所有的烦恼说 Bye Bye》
// 建议 tempo: UNIT = 100 ms

static const Note haidilao_birthday[] = {
    // === 第一句：对所有的烦恼说 Bye Bye ===
    // (3 3 3 4 5 | 5 4 3 2 2)
    {E5, 1}, {0, 1}, {E5, 1}, {0, 1}, {E5, 1}, {0, 1}, {F5, 1}, {0, 1}, {G5, 2}, {0, 1}, // 对所有的
    {G5, 1}, {0, 1}, {F5, 1}, {0, 1}, {E5, 1}, {0, 1}, {D5, 2}, {0, 1}, {D5, 2}, {0, 1}, // 烦恼说 Bye Bye

    // === 第二句：对所有的快乐说 Hi Hi ===
    // (2 2 2 3 4 | 4 3 2 3 3)
    {D5, 1}, {0, 1}, {D5, 1}, {0, 1}, {D5, 1}, {0, 1}, {E5, 1}, {0, 1}, {F5, 2}, {0, 1}, // 对所有的
    {F5, 1}, {0, 1}, {E5, 1}, {0, 1}, {D5, 1}, {0, 1}, {E5, 2}, {0, 1}, {E5, 2}, {0, 1}, // 快乐说 Hi Hi

    // === 第三句：亲爱的亲爱的 生日快乐 ===
    // (3 3 3 4 5 | 5 4 3 2 2)
    {E5, 1}, {0, 1}, {E5, 1}, {0, 1}, {E5, 1}, {0, 1}, {F5, 1}, {0, 1}, {G5, 2}, {0, 1}, // 亲爱的
    {G5, 1}, {0, 1}, {F5, 1}, {0, 1}, {E5, 1}, {0, 1}, {D5, 2}, {0, 1}, {D5, 2}, {0, 1}, // 亲爱的...

    // === 第四句：每一天都精彩 ===
    // (2 2 3 2 1)
    {D5, 1}, {0, 1}, {D5, 1}, {0, 1}, {E5, 1}, {0, 1}, {D5, 1}, {0, 1}, {C5, 4}, {0, 2}, // 每一天都精彩

    // === 间奏：噔 噔 噔噔噔 (和弦音效模拟) ===
    {C5, 1}, {0, 1}, {E5, 1}, {0, 1}, {G5, 1}, {E5, 1}, {C6, 4}, {0, 4},

    // === 结尾：看最后一眼 (Ha-ppy Birth-day) ===
    {G4, 1}, {0, 1}, {G4, 1}, {0, 1}, {A4, 2}, {0, 1}, {B4, 2}, {0, 1}, {C5, 6}, {0, 6}
};
// 广东名曲：《祝寿歌》 (恭祝你福寿与天齐)
// 建议 UNIT = 125 或 130

static const Note cantonese_birthday[] = {
    // === 第一句：恭祝你福寿与天齐 ===
    // (So Do Do Re Mi Do Re Mi)
    {G4, 1}, {0, 1},  // 恭
    {C5, 1}, {0, 1},  // 祝
    {C5, 1}, {0, 1},  // 你
    {D5, 1}, {0, 1},  // 福
    {E5, 1}, {0, 1},  // 寿
    {C5, 1}, {0, 1},  // 与
    {D5, 1}, {0, 1},  // 天
    {E5, 4}, {0, 2},  // 齐 (长音)

    // === 第二句：庆贺你生辰快乐 ===
    // (Re Mi Fa So Fa Mi Re Do)
    {D5, 1}, {0, 1},  // 庆
    {E5, 1}, {0, 1},  // 贺
    {F5, 1}, {0, 1},  // 你
    {G5, 1}, {0, 1},  // 生
    {F5, 1}, {0, 1},  // 辰
    {E5, 1}, {0, 1},  // 快
    {D5, 1}, {0, 1},  // 乐
    {C5, 4}, {0, 2},  // (长音)

    // === 第三句：年年都有今日 ===
    // (Do Do Re Do So So)
    {C5, 1}, {0, 1},  // 年
    {C5, 1}, {0, 1},  // 年
    {D5, 1}, {0, 1},  // 都
    {C5, 1}, {0, 1},  // 有
    {G5, 2}, {0, 1},  // 今
    {G5, 2}, {0, 1},  // 日

    // === 第四句：岁岁都有今朝 ===
    // (La So Fa Mi Re Do)
    {A5, 1}, {0, 1},  // 岁
    {G5, 1}, {0, 1},  // 岁
    {F5, 1}, {0, 1},  // 都
    {E5, 1}, {0, 1},  // 有
    {D5, 2}, {0, 1},  // 今
    {C5, 4}, {0, 2},  // 朝

    // === 第五句：恭喜你 恭喜你 ===
    // (Do Re Mi, Re Mi Fa)
    {C5, 1}, {0, 1}, {D5, 1}, {0, 1}, {E5, 3}, {0, 1}, // 恭喜你
    {D5, 1}, {0, 1}, {E5, 1}, {0, 1}, {F5, 3}, {0, 1}, // 恭喜你

    // === 结尾：恭喜你 (So Re Mi Fa) ===
    {G5, 1}, {0, 1}, {D5, 1}, {0, 1}, {E5, 1}, {0, 1}, {F5, 1}, {0, 1}, // 恭喜你
    {E5, 4}, {0, 2}, {C5, 6}, {0, 4}, // (结束长音)

    // 结束休止
    {0, 10}
};
void MotorMusic::play(const Note* musical_score, size_t length,int unit)
{
    // 播放前先锁住当前位置，防止电机乱转
    motor.UpdateTotalPosition();
    float lock_pos = (float)motor.GetTotalPosition();

    // 遍历方式修改：使用索引 i 遍历长度 length
    for (size_t i = 0; i < length; i++)
    {
        const Note& note = musical_score[i]; // 获取当前音符
        uint32_t duration_ms = note.duration * unit;

        // --- 休止符处理 ---
        if (note.frequency <= 0) {
            // 保持您的原有逻辑：循环发送指令维持位置
            // 参数保留：Kp=5.0, Kd=8.0
            uint32_t start_time = k_uptime_get_32();
            while (k_uptime_get_32() - start_time < duration_ms) {
                motor.SetMit(lock_pos, 0, 5.0f, 8.0f, 0, max_cur_);
                k_usleep(1000); // 1ms更新一次维持力矩
            }
            continue;
        }

        // --- 正常音符处理 (方波生成) ---

        // 计算方波的半周期 (微秒)
        // 这里的除法是安全的，因为上面已经排除了 frequency <= 0 的情况
        uint32_t half_period_us = 1000000 / note.frequency / 2;

        uint32_t start_time = k_uptime_get_32();

        // 在音符持续时间内，不断切换电流方向
        while (k_uptime_get_32() - start_time < duration_ms) {

            // 正半周
            // 参数保留：Kp=5.0, Kd=0.5
            motor.SetMit(lock_pos, 0, 5.0f, 0.5f, (float)volume_, max_cur_);
            k_busy_wait(half_period_us);

            // 负半周
            // 参数保留：Kp=5.0, Kd=0.5
            motor.SetMit(lock_pos, 0, 5.0f, 0.5f, -(float)volume_, max_cur_);
            k_busy_wait(half_period_us);
        }

        // 音符之间稍微断开一点点
        // 参数保留：Kp=5.0, Kd=8.0
        motor.SetMit(lock_pos, 0, 5.0f, 8.0f, 0, max_cur_);
        k_busy_wait(5000);
    }

    // 播放结束
    // 参数保留：Kp=5.0, Kd=8.0, max_cur=2000
    motor.SetMit(lock_pos, 0, 5.0f, 8.0f, 0, 2000);
    k_msleep(1000);
}

MotorMusic motor_music(1,ptz_can_dev,2000,1000);
int MusicInit()
{
    k_msleep(1000);
    while (true)
    {
        motor_music.play(happy_birthday,ARRAY_SIZE(happy_birthday),125);
        // motor_music.play(haidilao_birthday,ARRAY_SIZE(haidilao_birthday),100);
        motor_music.play(cantonese_birthday,ARRAY_SIZE(cantonese_birthday),100);
        // motor_music.motor.SetMit(0,0,5,8,0,2000);
        // k_msleep(5);
    }
    // k_thread_create()
}
