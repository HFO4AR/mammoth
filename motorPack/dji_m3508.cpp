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
using namespace std;

uint8_t TxData[8];
void DjiM3508::SendData() const {
    if (!device_is_ready(can_dev_)) return;

    uint32_t tx_id = (id_ <= 4) ? DJI_3508_TX_ID_LOW : DJI_3508_TX_ID_HIGH;
    uint8_t buffer[8] = {0};

    // 遍历注册表拼包
    for (int i = 0; i < registry_count_; i++) {
        CanMotor *base_m = registry_[i];

        // 1. 基础检查
        if (base_m == nullptr) continue;
        if (base_m->GetCanDev() != this->can_dev_) continue;

        // 2. ID 分组检查
        int peer_id = base_m->id_;
        bool my_group_low = (this->id_ <= 4);
        bool peer_group_low = (peer_id <= 4 && peer_id >= 1);
        bool peer_group_high = (peer_id <= 8 && peer_id >= 5);
        bool same_group = (my_group_low && peer_group_low) || (!my_group_low && peer_group_high);

        if (same_group) {
            // 3. 填入数据 (直接从基类获取 target_current_)
            int idx = (peer_id <= 4) ? (peer_id - 1) * 2 : (peer_id - 5) * 2;

            // 保护数组越界
            if (idx >= 0 && idx < 7) {
                buffer[idx]     = (base_m->target_current_ >> 8) & 0xFF;
                buffer[idx + 1] = base_m->target_current_ & 0xFF;
            }
        }
    }

    struct can_frame frame;
    frame.id = tx_id;
    frame.dlc = 8;
    // frame.flags = 0; // 标准帧
    memcpy(frame.data, buffer, 8);

    (void)can_send(can_dev_, &frame, K_NO_WAIT, NULL, NULL);
}



void DjiM3508::SetCurrentOpenLoop(int target) {
    target_current_ = target;

    // 2. 如果不是同步模式，立即触发发送
    // 注意：SendData 会自动去 registry 里找同一组的电机，把它们的 target_current_ 一起发出去
    // 所以即使是单发，也会带上队友的最新状态，不会覆盖队友的数据
    if (!sync_send_mode_) {
        SendData();
    }
}


void DjiM3508::EnableSyncSend() {
    sync_send_mode_=ENABLE;
}

void DjiM3508::UpdateFromFrame(struct can_frame *frame) {
    this->pos_ = sys_get_be16(&frame->data[0]);
    this->spd_ = sys_get_be16(&frame->data[2]);
    this->cur_ = sys_get_be16(&frame->data[4]);
    this->temp_ = frame->data[6];
    this->motor_enable_ = MOTOR_ENABLE;
    this->UpdateTotalPosition();
}