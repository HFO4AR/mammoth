//
// Created by nyuki on 2025/11/24.
//

#ifndef MAMMOTH_ASYNC_SERIAL_H
#define MAMMOTH_ASYNC_SERIAL_H
#include "zephyr/device.h"
#include "zephyr/drivers/uart.h"
class AsyncSerial
{
public:
    AsyncSerial(const struct device* dev);
protected:
    const struct device* _dev;
    static void uart_callback_wrapper(const struct device *dev,
                                     struct uart_event *evt,
                                     void *user_data);
};
#endif //MAMMOTH_ASYNC_SERIAL_H
