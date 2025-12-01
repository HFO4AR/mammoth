//
// Created by nyuki on 2025/11/27.
//

#ifndef MAMMOTH_MOTOR_MUSIC_H
#define MAMMOTH_MOTOR_MUSIC_H

#include <array>
#include <vector>

#include "motor.h"
#include "dji_m3508.h"
#include "can.h"
struct Note {
    uint16_t frequency; // 频率
    uint16_t duration;  // 时长系数
};
int MusicInit();
class MotorMusic {
public:
    DjiM3508 motor;
    MotorMusic(int id,const struct device * can_dev,int max_cur,float volume):motor(id,can_dev),max_cur_(max_cur),volume_(volume){}

    void play(const Note* musical_score, size_t length,int unit);
protected:
    int max_cur_;
    float volume_;
};
#endif //MAMMOTH_MOTOR_MUSIC_H