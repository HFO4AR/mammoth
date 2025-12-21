//
// Created by yuki on 2025/11/30.
//

// --- START OF FILE can_motor.h ---

#ifndef MAMMOTH_CAN_MOTOR_H
#define MAMMOTH_CAN_MOTOR_H

#include "motor.h"
#include <zephyr/drivers/can.h>
#include <zephyr/kernel.h>

// 哈希表大小：必须是 2 的幂次方 (32, 64, 128...)
// 建议设置为最大电机数量的 2 倍左右，以降低哈希冲突率
// #define CAN_HASH_MAP_SIZE 16
#define MAX_CAN_MOTORS 32
class CanMotor : public Motor {
public:
    /**
     * @brief 构造函数
     * @param id 电机逻辑 ID (用户自定义，用于应用层)
     * @param can_dev 绑定的 CAN 设备
     * @param rx_id 该电机接收反馈数据的 CAN ID
     * @param is_extended 是否为扩展帧 (true=29bit, false=11bit)
     */
    CanMotor(int id, const struct device *can_dev, uint32_t rx_id, bool is_extended = false);
    virtual ~CanMotor();

    /**
     * @brief 静态回调函数，用于处理接收到的CAN帧数据
     * 通过ID匹配快速定位对应的电机对象并更新状态
     * @param dev CAN设备指针
     * @param frame 接收到的CAN帧
     */
    static void GlobalProcessCanFrame(const struct device *dev, struct can_frame *frame);



    // 获取绑定的 CAN 设备
    const struct device* GetCanDev() const { return can_dev_; }

    // 获取反馈 ID
    uint32_t GetRxId() const { return rx_id_; }

protected:
    virtual void UpdateFromFrame(struct can_frame *frame) = 0;
    const struct device *can_dev_;
    uint32_t rx_id_;     // 接收 ID
    bool is_extended_;   // 是否扩展帧

    // --- 注册表管理 ---
    // 静态数组，存储所有 CanMotor 指针
    static CanMotor* registry_[MAX_CAN_MOTORS];

    // 注册到数组
    void Register();
    // 从数组注销
    void Unregister();
};

#endif //MAMMOTH_CAN_MOTOR_H