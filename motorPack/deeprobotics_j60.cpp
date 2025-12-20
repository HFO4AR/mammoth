//
// Created by nyuki on 2025/12/16.
//

#include "deeprobotics_j60.h"

#include "zephyr/sys/byteorder.h"

void DeepRoboticsJ60::SetCurrentOpenLoop(float target)
{
    SetMit(0,0,0,0,target);
}

void DeepRoboticsJ60::SetMit(float target_pos, float target_spd, float kp, float kd, float t_ff)
{
    if (!device_is_ready(can_dev_)) return;
    target_pos=65536*target_pos/80.0f+65545.0f/2;
    target_spd=16383*target_spd/80.0f+16383.0f/2;
    kd=kd*5;
    t_ff=65536*t_ff/80.0f+65545.0f/2;
    struct can_frame frame = {0};
    frame.id= id_+Control_Motor_ID;
    frame.dlc=8;
    frame.data[0]=static_cast<int>(target_pos);
    frame.data[1]=static_cast<int>(target_pos)>>8;
    frame.data[2]=static_cast<int>(target_spd);
    frame.data[3]=((static_cast<int>(target_spd) >> 8 ) & 0x3F) | (static_cast<int>(kp) << 6);
    frame.data[4]=static_cast<int>(kp)>>2;
    frame.data[5]=static_cast<int>(kd);
    frame.data[6]=static_cast<int>(t_ff);
    frame.data[7]=static_cast<int>(t_ff)>>8;
    (void)can_send(can_dev_, &frame, K_NO_WAIT, NULL, NULL);
}

void DeepRoboticsJ60::UpdateFromFrame(struct can_frame* frame)
{
    uint32_t pos= frame->data[0] | frame->data[1] << 8 | ((frame->data[2] << 16) & 0x0FFFFF);
    uint32_t spd = ((frame->data[2]>>4)&0x0F) | frame->data[3] << 4|frame->data[4]<<12;
    uint16_t cur = frame->data[5] | frame->data[6] << 8;
    uint8_t temp_flag_= frame->data[7] & 0x01;
    uint8_t temp = (frame->data[7] >> 1)&0x7F;

    //数据处理
    this->epos_ = 80.f*pos/1048575 -40;
    this->spd_ = 80.0f * spd/ 1048575 -40;
    this->cur_ = 80.0f *cur/ 65535 -40;
    this->temp_flag_ = temp_flag_;
    this->temp_ = (uint8_t )(220 * temp / 127 - 20);
}
