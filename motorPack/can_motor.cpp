//
// Created by yuki on 2025/11/30.
//

#include "can_motor.h"
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(can_motor_base, CONFIG_LOG_DEFAULT_LEVEL);

// 初始化静态成员
CanMotor* CanMotor::registry_[MAX_CAN_MOTORS] = {nullptr};
int CanMotor::registry_count_ = 0;

CanMotor::CanMotor(int id, const struct device *can_dev, uint32_t rx_id)
    : Motor(id), can_dev_(can_dev), rx_id_(rx_id)
{
    // 自动注册到全局数组
    if (registry_count_ < MAX_CAN_MOTORS) {
        registry_[registry_count_] = this;
        registry_count_++;
    } else {
        LOG_ERR("CanMotor registry full!");
    }
}

CanMotor::~CanMotor() {
    for (int i = 0; i < registry_count_; i++) {
        if (registry_[i] == this) {
            registry_count_--;
            // 如果要删除的元素不是最后一个，就把最后一个元素搬过来填坑
            if (i != registry_count_) {
                registry_[i] = registry_[registry_count_];
            }
            registry_[registry_count_] = nullptr;
            break;
        }
    }
}

// --- 核心分发逻辑 ---
void CanMotor::GlobalProcessCanFrame(const struct device *dev, struct can_frame *frame) {
    // 遍历所有 CAN 电机
    for (int i = 0; i < registry_count_; i++) {
        CanMotor *m = registry_[i];
        if (m == nullptr) continue;

        // 设备相同 && 反馈ID相同
        if (m->can_dev_ == dev && m->rx_id_ == frame->id) {
            // 多态调用：调用具体子类的解析函数
            m->UpdateFromFrame(frame);
            return;
        }
    }
}