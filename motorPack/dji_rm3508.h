//
// Created by nyuki on 2025/10/17.
//

#ifndef MAMMOTH_DJI_RM3508_H
#define MAMMOTH_DJI_RM3508_H

#include "motor.h"
#include <zephyr/drivers/can.h>

# define DISABLE false
# define ENABLE true

// void GetRm3508Data(can_frame *frame);

class DjiRm3508 :public Motor{
public:
    /**
 * @brief 构造函数
 * @param id 电机ID (1-8)
 * @param can_dev CAN设备指针
 */

    DjiRm3508(const int id, const struct device * can_dev) :  Motor(id), can_dev_(can_dev)
    {
        // 校验 ID 范围
        if (id_ >= 1 && id_ <= 8) {
            // 自动注册到静态数组中
            if (instances_[id_-1] == nullptr) {
                instances_[id_-1] = this;
            } else {
                printk("Motor ID %d already registered!", id_);
            }
        } else {
            printk("Invalid Motor ID %d", id_);
        }
    }

    ~DjiRm3508()
    {
        if (id_ >= 1 && id_ <= 8)
        {
            instances_[id_-1] = nullptr;
        }
    }
    static int GlobalInit(void);
    void SendData() const;
    void SetCurrentOpenLoop(int target) override;//open loop
    void EnableSyncSend();
    /**
     * @brief 静态接收处理函数
     * @param dev CAN 设备句柄
     * @param frame 接收到的 CAN 帧
     */
    static void ProcessCanFrame(const struct device *dev, struct can_frame *frame);
private:
    const device * can_dev_;
    bool sync_seed_mode_=DISABLE;
    // --- 静态成员管理 ---

    // 发送互斥锁，防止多线程同时修改发送缓存
    static struct k_mutex send_mutex_;

    // 发送缓存：[0-3] 对应 0x200 (ID 1-4), [4-7] 对应 0x1FF (ID 5-8)
    // 这里简化处理，假设只用 ID 1-4 (0x200)
    static uint8_t tx_buffer_0x200_[8];

    // 实例注册表：索引 0 对应 ID 1 ... 索引 7 对应 ID 8
    // 支持多个 CAN 口时，需要更复杂的查找，这里假设单 CAN 或不同 CAN ID 不冲突
    static DjiRm3508* instances_[8];

};

// void Motor_Set_Current(int16_t current0, int16_t current1, int16_t current2, int16_t current3);


#endif //MAMMOTH_DJI_RM3508_H