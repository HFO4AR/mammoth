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
#define CAN_HASH_MAP_SIZE 16

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
     * @brief 静态回调函数，用于 Zephyr 的 can_rx_callback 调用
     * 使用哈希查找算法 (O(1)) 快速定位电机
     */
    static void GlobalProcessCanFrame(const struct device *dev, struct can_frame *frame);

    virtual void UpdateFromFrame(struct can_frame *frame) = 0;

    // 获取绑定的 CAN 设备
    const struct device* GetCanDev() const { return can_dev_; }

    // 获取反馈 ID
    uint32_t GetRxId() const { return rx_id_; }

protected:
    const struct device *can_dev_;
    uint32_t rx_id_;     // 接收 ID
    bool is_extended_;   // 是否扩展帧

    // 哈希表相关
    // 静态哈希表数组，存储所有 CanMotor 指针
    static CanMotor* hash_map_[CAN_HASH_MAP_SIZE];

    // 哈希函数
    static uint32_t Hash(uint32_t rx_id, const struct device *dev, bool is_ext);

    // 注册到哈希表
    void RegisterToMap();

    // 允许子类访问哈希表 (用于 SendData 遍历寻找队友)
    // friend class DjiM3508;
};

#endif //MAMMOTH_CAN_MOTOR_H