//
// Created by yuki on 2026/3/25.
// Author HFO4AR https://github.com/HFO4AR
//

#ifndef MAMMOTH_GETTIME_H
#define MAMMOTH_GETTIME_H
#ifdef __ZEPHYR__
#include "zephyr/kernel.h"
#endif

#ifdef __HAL__
#include "stm32f4xx_hal.h"
#endif

inline int64_t GetTime()
{
#ifdef __ZEPHYR__
    return k_uptime_ticks();
#endif
#ifdef __HAL__
    return HAL_GetTick();
#endif
}
#endif //MAMMOTH_GETTIME_H