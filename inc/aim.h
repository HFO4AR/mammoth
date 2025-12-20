//
// Created by nyuki on 2025/11/19.
//

#ifndef MAMMOTH_AIM_H
#define MAMMOTH_AIM_H
#include "dji_motor.h"

class PTZ {
public:
    typedef struct {
        int max_angle;
        int min_angle;
        int angle;
        int zero_angle;
        int32_t target;
    }data_t;

    PTZ(int yaw_motor_id,int pitch_motor_id,const struct device * can_dev,k_thread_stack_t *stack,size_t stack_size):yaw_motor_(yaw_motor_id,can_dev),pitch_motor_(pitch_motor_id,can_dev),stack_(stack),stack_size_(stack_size) {}
    void Init();
    void SetAngle(float yaw,float pitch);
    DjiM3508 yaw_motor_;
    DjiM3508 pitch_motor_;
    data_t yaw_data_;
    data_t pitch_data_;

    
private:
    void InitMotorDirection(bool is_positive_direction);
    void CheckMotorOnline();
    static void ThreadEntry(void *p1, void *p2, void *p3);
    k_thread_stack_t *stack_;
    size_t stack_size_;
    struct k_thread thread_data_;
};
#endif //MAMMOTH_AIM_H