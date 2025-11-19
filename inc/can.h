//
// Created by nyuki on 2025/11/19.
//

#ifndef MAMMOTH_CAN_H
#define MAMMOTH_CAN_H
#include "zephyr/device.h"

void can_init();
static const struct device *const can2 = DEVICE_DT_GET(DT_NODELABEL(can2));
#endif //MAMMOTH_CAN_H