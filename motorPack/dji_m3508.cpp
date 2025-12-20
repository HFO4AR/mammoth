//
// Created by nyuki on 2025/10/17.
//
/***
 *使用说明：
 *调用set_cur方法设置电机电流
 *调用set_pos方法设置电机位置
 *调用set_spd方法设置电机速度
 *
 *调用set_spd和set_pos方法之前必须先调用pos_pid_init和spd_pid_init方法进行PID参数初始化，否则PID输出为0
 *
 *调用完set_cur、set_pos、set_spd方法之后必须调用send_3508_data方法发送数据
 *
 *
 ***/
#include "dji_m3508.h"
#include "cmath"
#include <array>
#include <algorithm>
#include <zephyr/sys/byteorder.h>
#include "can.h"
#include <chassis.h>
#include <aim.h>
#include <motor_music.h>
#include <zephyr/logging/log.h>
using namespace std;
LOG_MODULE_REGISTER(dji_3508, CONFIG_LOG_DEFAULT_LEVEL);

// 静态池分配内存 (BSS段)
DjiBusHandler DjiM3508::bus_pool_[MAX_CAN_BUS_COUNT];

void DjiM3508::SendData() const {
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
            frame.id = DJI_3508_TX_ID_LOW;
            int idx = (id_ - 1) * 2;
            frame.data[idx] = ((int16_t)target_current_ >> 8) & 0xFF;
            frame.data[idx + 1] = ((int16_t)target_current_ & 0xFF);
        } else {
            frame.id = DJI_3508_TX_ID_HIGH;
            int idx = (id_ - 5) * 2;
            frame.data[idx] = ((int16_t)target_current_ >> 8) & 0xFF;
            frame.data[idx + 1] = ((int16_t)target_current_ & 0xFF);
        }
        (void)can_send(can_dev_, &frame, K_NO_WAIT, NULL, NULL);
    }

}



void DjiM3508::SetCurrentOpenLoop(float target) {
    target_current_ = target;

    if (my_handler_&&sync_send_mode_)
    {
        my_handler_->Update(id_, target);
    }else
    {
        SendData();
    }
}


void DjiM3508::EnableSyncSend() {
    sync_send_mode_=ENABLE;
}

void DjiM3508::UpdateFromFrame(struct can_frame *frame) {
    this->epos_ = sys_get_be16(&frame->data[0]);
    this->spd_ = sys_get_be16(&frame->data[2]);
    this->cur_ = sys_get_be16(&frame->data[4]);
    this->temp_ = frame->data[6];
    this->motor_enable_ = true;
    this->UpdateTotalPosition();
}

DjiM3508::DjiM3508(int id, const struct device *can_dev)
    : CanMotor(id, can_dev, DJI_3508_RX_BASE_ID + id, false)// 初始化基类
{
    for (int i = 0; i < MAX_CAN_BUS_COUNT; i++) {
        if (bus_pool_[i].Match(can_dev)) {
            my_handler_ = &bus_pool_[i];
            return;
        }
    }

    for (int i = 0; i < MAX_CAN_BUS_COUNT; i++) {
        if (bus_pool_[i].IsFree()) {
            bus_pool_[i].Init(can_dev, DJI_3508_TX_ID_LOW, DJI_3508_TX_ID_HIGH); // 初始化
            my_handler_ = &bus_pool_[i]; // 绑定
            return;
        }
    }
    my_handler_ = nullptr;// 没有可用的 Handler

    //设置角度转换系数
    SetPositionConversionCoefficient((1.0f/4096.0f)*360.0f);
}