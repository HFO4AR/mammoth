//
// Created by nyuki on 2025/10/17.
//

#ifndef MAMMOTH_DJI_RM3508_H
#define MAMMOTH_DJI_RM3508_H

#include "motor.h"
#include <zephyr/drivers/can.h>

#include "can_motor.h"


#define DJI_3508_TX_ID_LOW   0x200 // ID 1-4
#define DJI_3508_TX_ID_HIGH  0x1FF // ID 5-8
#define DJI_3508_RX_BASE_ID  0x200
// void GetRm3508Data(can_frame *frame);

class DjiM3508 :public CanMotor{
public:
    /**
 * @brief 构造函数
 * @param id 电机ID (1-8)
 * @param can_dev CAN设备指针
 */

    DjiM3508(int id, const struct device *can_dev): CanMotor(id, can_dev, DJI_3508_RX_BASE_ID + id) {};

    void UpdateFromFrame(struct can_frame *frame) override;
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
    bool sync_send_mode_=DISABLE;

};

// void Motor_Set_Current(int16_t current0, int16_t current1, int16_t current2, int16_t current3);


#endif //MAMMOTH_DJI_RM3508_H