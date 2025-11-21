//
// Created by nyuki on 2025/10/17.
//

#ifndef MAMMOTH_DJI_RM3508_H
#define MAMMOTH_DJI_RM3508_H

#include "motor.h"
#include <zephyr/drivers/can.h>

# define DISABLE 0
# define ENABLE 1

void GetRm3508Data(can_frame *frame);

class DjiRm3508 :public Motor{
public:
    DjiRm3508(const int id, const struct device * can_dev) :  Motor(id), can_dev_(can_dev){}
    void SendData() const;
    void SetCurrentOpenLoop(int target) override;//open loop
    void EnableSyncSend();
private:
    const device * can_dev_;
    int sync_seed_mode_=DISABLE;
};

// void Motor_Set_Current(int16_t current0, int16_t current1, int16_t current2, int16_t current3);


#endif //MAMMOTH_DJI_RM3508_H