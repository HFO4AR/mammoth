//
// Created by yuki on 2025/11/30.
//


#include "can_motor.h"
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(can_motor_base, CONFIG_LOG_DEFAULT_LEVEL);

// 初始化静态哈希表 (BSS 段自动清零，默认为 nullptr)
CanMotor* CanMotor::hash_map_[CAN_HASH_MAP_SIZE] = {nullptr};

CanMotor::CanMotor(int id, const struct device *can_dev, uint32_t rx_id, bool is_extended)
    : Motor(id), can_dev_(can_dev), rx_id_(rx_id), is_extended_(is_extended)
{
    RegisterToMap();
}

CanMotor::~CanMotor() {
    uint32_t idx = Hash(rx_id_, can_dev_, is_extended_);
    int start_idx = idx;

    while (hash_map_[idx] != nullptr) {
        if (hash_map_[idx] == this) {
            hash_map_[idx] = nullptr;
            return;
        }
        idx = (idx + 1) & (CAN_HASH_MAP_SIZE - 1);
        if (idx == start_idx) break;
    }
}

// 哈希函数：混合 ID、设备名地址、扩展帧标志
uint32_t CanMotor::Hash(uint32_t rx_id, const struct device *dev, bool is_ext) {
    uint32_t dev_key = (uint32_t)dev->name;

    uint32_t hash = rx_id ^ dev_key ^ (is_ext ? 0xAAAAAAAA : 0x55555555);

    return hash & (CAN_HASH_MAP_SIZE - 1);
}

void CanMotor::RegisterToMap() {
    uint32_t idx = Hash(rx_id_, can_dev_, is_extended_);
    int start_idx = idx;

    // 线性探测寻找空位
    while (hash_map_[idx] != nullptr) {
        idx = (idx + 1) & (CAN_HASH_MAP_SIZE - 1);

        if (idx == start_idx) {
            LOG_ERR("CanMotor Hash Map FULL! Increase CAN_HASH_MAP_SIZE.");
            return;
        }
    }

    hash_map_[idx] = this;
}

void CanMotor::GlobalProcessCanFrame(const struct device *dev, struct can_frame *frame) {
    bool is_ext = (frame->flags & CAN_FRAME_IDE) != 0;

    uint32_t idx = Hash(frame->id, dev, is_ext);
    int start_idx = idx;

    while (true) {
        CanMotor *m = hash_map_[idx];

        if (m == nullptr) {
            return;
        }

        if (m->rx_id_ == frame->id &&
            m->is_extended_ == is_ext &&
            m->can_dev_->name == dev->name)
        {
            m->UpdateFromFrame(frame);
            return;
        }

        idx = (idx + 1) & (CAN_HASH_MAP_SIZE - 1);

        if (idx == start_idx) {
            return;
        }
    }
}