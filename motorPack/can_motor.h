//
// Created by yuki on 2025/11/30.
//

#ifndef MAMMOTH_CAN_MOTOR_H
#define MAMMOTH_CAN_MOTOR_H
#include "motor.h"
#include <zephyr/drivers/can.h>
#include <zephyr/kernel.h>

#define MAX_CAN_MOTORS 32 // 系统支持的最大 CAN 电机数量

class CanMotor : public Motor {
public:
    /**
     * @brief 构造函数
     * @param id 电机逻辑 ID
     * @param can_dev 绑定的 CAN 设备
     * @param rx_id 该电机接收反馈数据的 CAN ID
     */
    CanMotor(int id, const struct device *can_dev, uint32_t rx_id);
    virtual ~CanMotor();

    /**
     * @brief 静态回调函数，用于 Zephyr 的 can_rx_callback 调用
     * 遍历注册表，找到匹配的电机并调用其 UpdateFromFrame
     */
    static void GlobalProcessCanFrame(const struct device *dev, struct can_frame *frame);

    /**
     * @brief 纯虚函数：子类必须实现具体的协议解析逻辑
     */
    virtual void UpdateFromFrame(struct can_frame *frame) = 0;

    // 获取绑定的 CAN 设备
    const struct device* GetCanDev() const { return can_dev_; }

    // 获取反馈 ID
    uint32_t GetRxId() const { return rx_id_; }

protected:
    const struct device *can_dev_;
    uint32_t rx_id_; // 这一帧 CAN 数据的 ID

    // 全局注册表
    static CanMotor* registry_[MAX_CAN_MOTORS];
    static int registry_count_;
};


#endif //MAMMOTH_CAN_MOTOR_H