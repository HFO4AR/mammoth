//
// Created by nyuki on 2025/12/16.
//

#ifndef MAMMOTH_DEEPROBOTICS_J60_H
#define MAMMOTH_DEEPROBOTICS_J60_H
#define Disable_Motor_ID (0x020) //1<<5
#define Able_Motor_ID    (0x040) //2<<5
#define Control_Motor_ID (0x080) //4<<5
#define Error_Motor_ID 	 (0x2E0) //23<<5
#include "can_motor.h"
class DeepRoboticsJ60 : public CanMotor {
public:
    DeepRoboticsJ60(int id,const struct device *can_dev) : CanMotor(id,can_dev,id+Control_Motor_ID + 0x10,true) {}
    void UpdateFromFrame(struct can_frame *frame) override;
    void SetCurrentOpenLoop(float target) override;
    void SetMit(float target_pos, float target_spd, float kp, float kd, float t_ff);
protected:
    int temp_flag_;
};

#endif //MAMMOTH_DEEPROBOTICS_J60_H