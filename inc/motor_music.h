//
// Created by nyuki on 2025/11/27.
//

#ifndef MAMMOTH_MOTOR_MUSIC_H
#define MAMMOTH_MOTOR_MUSIC_H

#include <array>
#include <vector>

#include "motor.h"
#include "dji_m3508.h"
#include <zephyr/kernel.h>
#include "can.h"
struct Note {
    uint16_t frequency; // 频率
    uint16_t duration;  // 时长系数
};

class MotorMusic {
public:
    DjiM3508 motor;
    MotorMusic(int id,const struct device * can_dev,int max_cur,float volume,k_thread_stack_t *stack,size_t stack_size):motor(id,can_dev),max_cur_(max_cur),volume_(volume),stack_(stack),stack_size_(stack_size){}

    void play(const Note* musical_score, size_t length,int unit);

    int Init();
protected:
    int max_cur_;
    float volume_;
private:
    k_thread_stack_t *stack_;
    size_t stack_size_;
    struct k_thread thread_data_;
    // static void ThreadEntry(void *p1, void *p2, void *p3);

};
#endif //MAMMOTH_MOTOR_MUSIC_H