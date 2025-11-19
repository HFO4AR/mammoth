//
// Created by nyuki on 2025/11/19.
//

#include "can.h"
#include "Dji_Rm3508.h"
void can_rx_callback(const struct device *dev, struct can_frame *frame,
                      void *user_data) {
    get_rm3508_data(frame);
}

void can_init(const struct device *can_id) {
    can_filter filter={
        .id=0,
        .mask = 0,
        .flags = 0
    };
    can_add_rx_filter(can_id, can_rx_callback, NULL, &filter);
    can_set_mode(can2, CAN_MODE_NORMAL);
    can_start(can2);
}