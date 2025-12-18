//
// Created by nyuki on 2025/10/17.
//

#ifndef MAMMOTH_DJI_RM3508_H
#define MAMMOTH_DJI_RM3508_H

#include "motor.h"
#include <zephyr/drivers/can.h>

#include "can_motor.h"

#include "dji_bus_handler.h"
#define DJI_3508_TX_ID_LOW   0x200 // ID 1-4
#define DJI_3508_TX_ID_HIGH  0x1FF // ID 5-8
#define DJI_3508_RX_BASE_ID  0x200
#define MAX_CAN_BUS_COUNT 4
// void GetRm3508Data(can_frame *frame);

class DjiM3508 :public CanMotor{
public:
    /**
 * @brief 构造函数
 * @param id 电机ID
 * @param can_dev CAN设备指针
 */
    DjiM3508(int id, const struct device *can_dev);

    void UpdateFromFrame(struct can_frame *frame) override;
    void SendData() const;
    void SetCurrentOpenLoop(float target) override;//open loop
    void EnableSyncSend();
    static int low_tx_id_;
    static int high_tx_id_;
private:
    bool sync_send_mode_=DISABLE;
    // 指向该电机归属的总线处理器
    DjiBusHandler *my_handler_ = nullptr;

    // --- 静态资源池 ---
    // 自动管理所有 CAN 口的拼包缓存
    static DjiBusHandler bus_pool_[MAX_CAN_BUS_COUNT];



};

// void Motor_Set_Current(int16_t current0, int16_t current1, int16_t current2, int16_t current3);


#endif //MAMMOTH_DJI_RM3508_H