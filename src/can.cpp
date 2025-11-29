//
// Created by nyuki on 2025/11/19.
//

#include "can.h"
#include "dji_rm3508.h"
void CanRxCallback(const struct device *dev, struct can_frame *frame,
                     void *user_data) {
    // GetRm3508Data(frame);
    DjiRm3508::ProcessCanFrame(dev, frame);
}

void CanInit(const struct device *can_id) {
    can_filter filter={
        .id=0,
        .mask = 0,
        .flags = 0
    };
    can_add_rx_filter(can_id, CanRxCallback, NULL, &filter);
    can_set_mode(can_id, CAN_MODE_NORMAL);
    can_start(can_id);
}