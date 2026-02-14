//
// Created by nyuki on 2025/12/21.
//

#ifndef MAMMOTH_DJI_MOTOR_H
#define MAMMOTH_DJI_MOTOR_H
#include <cstdint>

#include "can_motor.h"
#include "zephyr/sys/byteorder.h"
#include "dji_bus_handler.h"
#define MAX_CAN_BUS_COUNT 4
#define DJI_6020_TX_ID_LOW 0x1FF
#define DJI_6020_TX_ID_HIGH 0x2FF
#define DJI_6020_RX_BASE_ID 0x204

#define DJI_3508_TX_ID_LOW   0x200 // ID 1-4
#define DJI_3508_TX_ID_HIGH  0x1FF // ID 5-8
#define DJI_3508_RX_BASE_ID  0x200

#define DJI_2006_TX_ID_LOW   DJI_3508_TX_ID_LOW// ID 1-4
#define DJI_2006_TX_ID_HIGH  DJI_3508_TX_ID_HIGH // ID 5-8
#define DJI_2006_RX_BASE_ID  DJI_3508_RX_BASE_ID

template<uint32_t RX_BASE_ID, uint32_t TX_ID_LOW, uint32_t TX_ID_HIGH>
class GenericDjiMotor : public CanMotor {
protected:
    bool sync_send_mode_=DISABLE;
    // 指向该电机归属的总线处理器
    DjiBusHandler *my_handler_ = nullptr;

    // --- 静态资源池 ---
    // 自动管理所有 CAN 口的拼包缓存
    static DjiBusHandler bus_pool_[MAX_CAN_BUS_COUNT];
public:
    GenericDjiMotor(int id, const struct device *can_dev)
        : CanMotor(id, can_dev, RX_BASE_ID + id, false)
        {
            for (int i = 0; i < MAX_CAN_BUS_COUNT; i++) {
                if (bus_pool_[i].Match(can_dev)) {
                    my_handler_ = &bus_pool_[i];
                    return;
                }
            }

            for (int i = 0; i < MAX_CAN_BUS_COUNT; i++) {
                if (bus_pool_[i].IsFree()) {
                    bus_pool_[i].Init(can_dev, TX_ID_LOW, TX_ID_HIGH); // 初始化
                    my_handler_ = &bus_pool_[i]; // 绑定
                    return;
                }
            }
            my_handler_ = nullptr;// 没有可用的 Handler

            //设置角度转换系数
            SetPositionConversionCoefficient((1.0f/4096.0f)*360.0f);
    }

    void UpdateFromFrame(struct can_frame *frame) override {
        // 通用的帧处理代码
        this->epos_ = sys_get_be16(&frame->data[0]);
        this->spd_ = sys_get_be16(&frame->data[2]);
        this->cur_ = sys_get_be16(&frame->data[4]);
        this->temp_ = frame->data[6];
        this->motor_enable_ = true;
        this->UpdateTotalPosition(8192);
    }

    void SetCurrentOpenLoop(float target) override {
        target_current_ = target;

        if (my_handler_&&sync_send_mode_)
        {
            my_handler_->Update(id_, target);
        }else
        {
            SendData();
        }
    }

    void SendData() const {
        if (sync_send_mode_)
        {
            if (my_handler_) {
                // 委托 Handler 发送整组数据
                bool is_low_group = (id_ <= 4);
                my_handler_->Send(is_low_group);
            }
        }else{
            if (!device_is_ready(can_dev_)) return;

            struct can_frame frame = {0};
            frame.dlc = 8;
            if (id_ <= 4) {
                frame.id = TX_ID_LOW;
                int idx = (id_ - 1) * 2;
                frame.data[idx] = ((int16_t)target_current_ >> 8) & 0xFF;
                frame.data[idx + 1] = ((int16_t)target_current_ & 0xFF);
            } else {
                frame.id = TX_ID_HIGH;
                int idx = (id_ - 5) * 2;
                frame.data[idx] = ((int16_t)target_current_ >> 8) & 0xFF;
                frame.data[idx + 1] = ((int16_t)target_current_ & 0xFF);
            }
            (void)can_send(can_dev_, &frame, K_NO_WAIT, NULL, NULL);
        }
    }
    void EnableSyncSend(){sync_send_mode_=ENABLE;}
};

// 为不同类型的电机定义类型别名
using DjiM3508 = GenericDjiMotor<DJI_3508_RX_BASE_ID, DJI_3508_TX_ID_LOW, DJI_3508_TX_ID_HIGH>;
using DjiGM6020 = GenericDjiMotor<DJI_6020_RX_BASE_ID, DJI_6020_TX_ID_LOW, DJI_6020_TX_ID_HIGH>;
using DjiM2006 = GenericDjiMotor<DJI_2006_RX_BASE_ID,DJI_3508_TX_ID_LOW,DJI_2006_TX_ID_HIGH>;
using DjiC620 = DjiM3508;
using DjiC610 = DjiM2006;

#endif //MAMMOTH_DJI_MOTOR_H