//
// Created by nyuki on 2025/11/30.
//

#ifndef MAMMOTH_DJI_BUS_HANDLER_H
#define MAMMOTH_DJI_BUS_HANDLER_H


#include <zephyr/drivers/can.h>
#include <cstring>

// 一个 Handler 对应一个物理 CAN 口 (如 CAN1)
class DjiBusHandler {
public:
    // DjiBusHandler(int id_low, int id_high): id_low_(id_low), id_high_(id_high){
    //     memset(buf_low_, 0, 8);
    //     memset(buf_high_, 0, 8);
    // }

    // 初始化绑定设备
    void Init(const struct device *dev, int id_low, int id_high){
        dev_ = dev;
        low_tx_id_ = id_low;
        high_tx_id_ = id_high;
        memset(buf_low_, 0, 8);
        memset(buf_high_, 0, 8);
    }

    // 检查这个 Handler 是否属于某个设备
    bool Match(const struct device *dev) const {
        return (dev_ != nullptr) && (dev_->name == dev->name);
    }

    bool IsFree() const { return dev_ == nullptr; }

    // O(1) 更新缓存
    void Update(int id, int16_t val) {
        if (id <= 4) {
            int idx = (id - 1) * 2;
            buf_low_[idx] = (val >> 8) & 0xFF;
            buf_low_[idx + 1] = val & 0xFF;
        } else {
            int idx = (id - 5) * 2;
            buf_high_[idx] = (val >> 8) & 0xFF;
            buf_high_[idx + 1] = val & 0xFF;
        }
    }

    // O(1) 发送
    void Send(bool low_group) {
        if (!device_is_ready(dev_)) return;

        struct can_frame frame = {0};
        frame.dlc = 8;

        if (low_group) {
            frame.id = low_tx_id_;
            memcpy(frame.data, buf_low_, 8);
        } else {
            frame.id = high_tx_id_;
            memcpy(frame.data, buf_high_, 8);
        }
        (void)can_send(dev_, &frame, K_NO_WAIT, NULL, NULL);
    }

private:
    int low_tx_id_;
    int high_tx_id_;
    const struct device *dev_;
    uint8_t buf_low_[8];  // ID 1-4
    uint8_t buf_high_[8]; // ID 5-8
};


#endif //MAMMOTH_DJI_BUS_HANDLER_H