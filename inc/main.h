//
// Created by nyuki on 2025/11/19.
//

#ifndef MAMMOTH_MAIN_H
#define MAMMOTH_MAIN_H
#include "aim.h"
#include "chassis.h"
#include "can.h"

inline OmniChassis chassis(0.5,0.5,0.15,5000,1,2,3,4,chassis_can_dev);
inline PTZ ptz(2,1,ptz_can_dev);

#endif //MAMMOTH_MAIN_H