//
// Created by nyuki on 2025/11/2.
//

#ifndef CPP3508_DM_S3519_H
#define CPP3508_DM_S3519_H
#include "motor.h"

#ifdef __cplusplus
extern "C" {
#endif

    void get_dm3519_data();

#ifdef __cplusplus
}
#endif

#ifdef __cplusplus



class DM_S3519 :public Motor{
public:
    DM_S3519(const int id) :  Motor(id){}

    void set_spd(float target);
};



// void Motor_Set_Current(int16_t current0, int16_t current1, int16_t current2, int16_t current3);
#endif
#endif //CPP3508_DM_S3519_H