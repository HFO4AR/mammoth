//
// Created by nyuki on 2025/11/24.
//

#ifndef MAMMOTH_DJI_DBUS_H
#define MAMMOTH_DJI_DBUS_H

#include <cstdint>
#include "zephyr/device.h"
#include "zephyr/drivers/uart.h"
class DjiDbus
{
public:
    DjiDbus(const device *const dbus_dev):dev_(dbus_dev){};
    uint16_t GetCH0();
    uint16_t GetCH1();
    uint16_t GetCH2();
    uint16_t GetCH3();
    uint8_t GetS1();
    uint8_t GetS2();
    void ReceivingData();
protected:
    const device *const dev_;
    bool buf1_complete_flag_;
    bool buf2_complete_flag_;
    union buffer_t
    {
        uint8_t input[6];
        struct
        {
            uint64_t ch0:11;
            uint64_t ch1:11;
            uint64_t ch2:11;
            uint64_t ch3:11;
            uint64_t s1:2;
            uint64_t s2:2;
        }read __attribute__((packed));
    };
    buffer_t buf1_;
    buffer_t buf2_;
};

#endif //MAMMOTH_DJI_DBUS_H