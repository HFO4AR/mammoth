//
// Created by nyuki on 2025/11/19.
//

#ifndef MAMMOTH_CAN_H
#define MAMMOTH_CAN_H
#include "zephyr/device.h"

void CanInit(const struct device *can_id);
static const struct device *const chassis_can_dev = DEVICE_DT_GET(DT_NODELABEL(can1));
static const struct device *const ptz_can_dev = DEVICE_DT_GET(DT_NODELABEL(can2));
#endif //MAMMOTH_CAN_H