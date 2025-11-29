#pragma once
#include <zephyr/kernel.h>
#include <zephyr/drivers/uart.h>
#include <cstdint>

#define DBUS_FRAME_SIZE 18
#define DBUS_PAYLOAD_OFFSET 1

class DjiDbus
{
public:
    // 数据结构
    struct RemoteData
    {
        int16_t ch0;
        int16_t ch1;
        int16_t ch2;
        int16_t ch3;
        uint8_t s1;
        uint8_t s2;
        uint16_t mouse_x;
        uint16_t mouse_y;
        uint16_t mouse_z;
        uint8_t mouse_left;
        uint8_t mouse_right;
        uint16_t key;
    };

    DjiDbus(const struct device *const dbus_dev);
    int ReceivingData();

    void GetData(RemoteData& out_data);

protected:
    const struct device *const dev_;

    struct buffer_t
    {
        uint8_t data[DBUS_FRAME_SIZE];
    } __aligned(4);

    buffer_t buf1_;
    buffer_t buf2_;
    buffer_t* next_buf_ptr_;

    // 影子副本
    RemoteData data_safe_;

    struct k_spinlock lock_;

    static void uart_callback(const struct device *dev, struct uart_event *evt, void *user_data);
    void process_event(struct uart_event *evt);

    void parse_raw_to_struct(const uint8_t* raw_ptr, RemoteData& target);
};