//
// Created by nyuki on 2025/12/4.
//

#ifndef MAMMOTH_UPPER_COMPUTER_COMMUNICATION_H
#define MAMMOTH_UPPER_COMPUTER_COMMUNICATION_H

#include <zephyr/kernel.h>
class UpperComputer
{
public:
    UpperComputer();
    void Init();
protected:
    device *const uart_dev;
};
#endif //MAMMOTH_UPPER_COMPUTER_COMMUNICATION_H
