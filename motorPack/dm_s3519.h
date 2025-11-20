//
// Created by nyuki on 2025/11/2.
//

#ifndef MAMMOTH_DM_S3519_H
#define MAMMOTH_DM_S3519_H
#include "motor.h"

#ifdef __cplusplus
extern "C" {
#endif

    void GetDm3519Data();

#ifdef __cplusplus
}
#endif

#ifdef __cplusplus



class DmS3519 :public Motor{
public:
    DmS3519(const int id) :  Motor(id){}

    void SetSpeed(float target);
};



// void Motor_Set_Current(int16_t current0, int16_t current1, int16_t current2, int16_t current3);
#endif
#endif //MAMMOTH_DM_S3519_H