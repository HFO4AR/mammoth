//
// Created by yuki on 2025/11/30.
//


#include "can_motor.h"
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(can_motor_base, CONFIG_LOG_DEFAULT_LEVEL);

CanMotor* CanMotor::registry_[MAX_CAN_MOTORS] = {nullptr};
CanMotor::CanMotor(int id, const struct device *can_dev, uint32_t rx_id, bool is_extended)
    : Motor(id), can_dev_(can_dev), rx_id_(rx_id), is_extended_(is_extended)
{
    Register();
}

CanMotor::~CanMotor() {
    Unregister();
}

void CanMotor::Register() {
    // 寻找空位插入
    for (int i = 0; i < MAX_CAN_MOTORS; i++) {
        if (registry_[i] == nullptr) {
            registry_[i] = this;
            return;
        }
    }
    LOG_ERR("CanMotor registry FULL! Increase MAX_CAN_MOTORS.");
}

void CanMotor::Unregister() {
    // 查找自己并移除
    for (int i = 0; i < MAX_CAN_MOTORS; i++) {
        if (registry_[i] == this) {
            registry_[i] = nullptr;
            return;
        }
    }
}
void CanMotor::GlobalProcessCanFrame(const struct device *dev, struct can_frame *frame) {
    bool is_ext = (frame->flags & CAN_FRAME_IDE) != 0;

    for (int i = 0; i < MAX_CAN_MOTORS; i++) {
        CanMotor* m = registry_[i];

        // 1. 检查指针是否有效
        if (m == nullptr) {
            continue; // 跳过空洞
        }

        // 核心匹配逻辑：ID 匹配 && 扩展帧标志匹配 && 设备指针匹配
        if (m->rx_id_ == frame->id &&
            m->is_extended_ == is_ext &&
            m->can_dev_ == dev)
        {
            m->UpdateFromFrame(frame);
            return; // 找到目标后立即返回
        }
    }
}