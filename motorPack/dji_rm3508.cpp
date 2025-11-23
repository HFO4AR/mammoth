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
#include "dji_rm3508.h"
#include "cmath"
#include <array>
#include <algorithm>
#include <zephyr/sys/byteorder.h>
#include "can.h"
#include <chassis.h>

#include "main.h"

using namespace std;

extern OmniChassis chassis;
// DjiRm3508 *motor3508_index = *chassis.motors;
DjiRm3508 *motor3508_index[] = {&ptz.pitch_motor_};
//can接收函数


void GetRm3508Data(can_frame *frame) {
    union rx_data_t {
        char input[8]{};
        struct {
            int16_t pos = 0;
            int16_t spd = 0;
            int16_t cur = 0;
            int8_t temp = 0;
            int8_t null_data = 0;
        } read;
    };
    rx_data_t RxData{};
    memcpy(RxData.input,frame->data,frame->dlc);
    int motor_id = frame->id - 0x200 - 1;
    motor3508_index[motor_id]->pos = sys_be16_to_cpu(RxData.read.pos);
    motor3508_index[motor_id]->cur = sys_be16_to_cpu(RxData.read.cur);
    motor3508_index[motor_id]->spd = sys_be16_to_cpu(RxData.read.spd);
    motor3508_index[motor_id]->temp = RxData.read.temp;
    if (RxData.read.temp) {
        motor3508_index[motor_id]->motor_enable_ = MOTOR_ENABLE;
    }
    motor3508_index[motor_id]->UpdateTotalPosition();

}

//can发送函数

uint8_t TxData[8];
void DjiRm3508::SendData() const {
    can_frame frame{};
    frame.id=0x200;
    frame.dlc=8;
    memcpy(frame.data, TxData, frame.dlc);
    (void)can_send(can_dev_, &frame, K_MSEC(100), NULL, NULL);
    fill_n(TxData, sizeof(TxData), 0); //清空发送缓存
}



void DjiRm3508::SetCurrentOpenLoop(int target) {
    switch (id_) {
        case 1:
            TxData[0] = (target >> 8) & 0xFF;
            TxData[1] = target & 0xFF;
            break;
        case 2:
            TxData[2] = (target >> 8) & 0xFF;
            TxData[3] = target & 0xFF;
            break;
        case 3:
            TxData[4] = (target >> 8) & 0xFF;
            TxData[5] = target & 0xFF;
            break;
        case 4:
            TxData[6] = (target >> 8) & 0xFF;
            TxData[7] = target & 0xFF;
            break;
        default:
            break;
    }
    if (!sync_seed_mode_) {
        SendData();
    }
    UpdateTotalPosition();
}


void DjiRm3508::EnableSyncSend() {
    sync_seed_mode_=ENABLE;
}