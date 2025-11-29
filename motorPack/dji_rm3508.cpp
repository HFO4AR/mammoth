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
#include <aim.h>
#include <motor_music.h>
extern MotorMusic motor_music;
extern PTZ ptz;
extern OmniChassis chassis;

using namespace std;

extern OmniChassis chassis;
// DjiRm3508 *motor3508_index = *chassis.motors;
// DjiRm3508 *motor3508_index[] = {&ptz.pitch_motor_};
// DjiRm3508 *motor3508_index[] = {&motor_music.motor};
//can接收函数

// --- 静态成员初始化 ---
DjiRm3508* DjiRm3508::instances_[8] = {nullptr};
uint8_t DjiRm3508::tx_buffer_0x200_[8] = {0};
struct k_mutex DjiRm3508::send_mutex_;

// 静态初始化块（利用 C++ 特性初始化互斥锁，或者在 main 中显式调用）
int DjiRm3508::GlobalInit(void) {
    k_mutex_init(&send_mutex_);
    return 0;
}
static int dji_motor_init_wrapper(void) {
    // 在这里调用类的静态函数
    return DjiRm3508::GlobalInit();
}

// 3. 【修改】将包装函数传给 SYS_INIT
SYS_INIT(dji_motor_init_wrapper, POST_KERNEL, 0);

// void GetRm3508Data(can_frame *frame) {
//     union rx_data_t {
//         char input[8]{};
//         struct {
//             int16_t pos = 0;
//             int16_t spd = 0;
//             int16_t cur = 0;
//             int8_t temp = 0;
//             int8_t null_data = 0;
//         } read;
//     };
//     rx_data_t RxData{};
//     memcpy(RxData.input,frame->data,frame->dlc);
//     int motor_id = frame->id - 0x200 - 1;
//     motor3508_index[motor_id]->pos_ = sys_be16_to_cpu(RxData.read.pos);
//     motor3508_index[motor_id]->cur_ = sys_be16_to_cpu(RxData.read.cur);
//     motor3508_index[motor_id]->spd_ = sys_be16_to_cpu(RxData.read.spd);
//     motor3508_index[motor_id]->temp_ = RxData.read.temp;
//     if (RxData.read.temp) {
//         motor3508_index[motor_id]->motor_enable_ = MOTOR_ENABLE;
//     }
//     motor3508_index[motor_id]->UpdateTotalPosition();
//
// }

//can发送函数

uint8_t TxData[8];
void DjiRm3508::SendData() const {
    can_frame frame{};
    frame.id=0x200;
    frame.dlc=8;
    memcpy(frame.data, TxData, frame.dlc);
    (void)can_send(can_dev_, &frame,K_NO_WAIT, NULL, NULL);
    fill_n(TxData, sizeof(TxData), 0); //清空发送缓存
}



void DjiRm3508::SetCurrentOpenLoop(int target) {
    if (id_ < 1 || id_ > 4) {
        // 目前只演示 ID 1-4 的逻辑
        return;
    }

    // 线程安全地更新共享 buffer
    k_mutex_lock(&send_mutex_, K_FOREVER);

    int idx = (id_ - 1) * 2;
    tx_buffer_0x200_[idx]     = (target >> 8) & 0xFF;
    tx_buffer_0x200_[idx + 1] = target & 0xFF;

    k_mutex_unlock(&send_mutex_);

    if (!sync_seed_mode_) {
        SendData();
    }
    // UpdateTotalPosition();
}


void DjiRm3508::EnableSyncSend() {
    sync_seed_mode_=ENABLE;
}

// ---------------------------------------------------------
// 核心优化：接收函数
// ---------------------------------------------------------
void DjiRm3508::ProcessCanFrame(const struct device *dev, struct can_frame *frame) {
    // 1. 计算电机 ID (假设是 0x201 - 0x208)
    int motor_idx = frame->id - 0x201;

    // 2. 边界检查
    if (motor_idx < 0 || motor_idx > 7) {
        return; // 不是电机反馈帧
    }

    // 3. 查找实例
    DjiRm3508 *motor = instances_[motor_idx];

    // 4. 指针安全检查 & 设备匹配检查 (防止多 CAN 口混淆)
    if (motor == nullptr || motor->can_dev_ != dev) {
        return;
    }

    // 5. 直接解析数据 (零拷贝，无需 memcpy)
    // DJI 反馈格式: [High8 pos, Low8 pos, High8 spd, Low8 spd, High8 cur, Low8 cur, temp, null]

    motor->pos_ = sys_get_be16(&frame->data[0]); // 机械角度
    motor->spd_ = sys_get_be16(&frame->data[2]); // 转速
    motor->cur_ = sys_get_be16(&frame->data[4]); // 实际转矩电流
    motor->temp_ = frame->data[6];               // 温度

    // 6. 更新电机状态逻辑
    // 只要收到数据，就认为电机是在线的（或者温度非0，视具体逻辑而定）
    motor->motor_enable_ = MOTOR_ENABLE;

    // 7. 处理多圈角度累加
    motor->UpdateTotalPosition();
}