//
// Created by nyuki on 2025/12/20.
//

#ifndef MAMMOTH_CUBEMARSAK_H
#define MAMMOTH_CUBEMARSAK_H
#include "can_motor.h"

class CubeMarsAK:public CanMotor
{
public:
    CubeMarsAK(int id,const struct device *can_dev) : CanMotor(id, can_dev,0x2900+id ,true)
    {
    }
};
#endif //MAMMOTH_CUBEMARSAK_H