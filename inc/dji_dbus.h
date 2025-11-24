#pragma once

#include <zephyr/kernel.h>
#include <zephyr/drivers/uart.h>
#include <cstdint>
#include <cstring>

class DjiDbus
{
public:
    DjiDbus(const struct device *const dbus_dev);

    // 启动接收
    void ReceivingData();

    // Getter 方法 (从影子副本读取，线程安全)
    uint16_t GetCH0();
    uint16_t GetCH1();
    uint16_t GetCH2();
    uint16_t GetCH3();
    uint8_t GetS1();
    uint8_t GetS2();

protected:
    const struct device *const dev_;

    // 定义联合体
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
        } __attribute__((packed)) read; 
    };

    // DMA 缓冲区：必须 4 字节对齐
    buffer_t buf1_ __aligned(4);
    buffer_t buf2_ __aligned(4);
    
    // 指针：追踪下一个要提供给 DMA 的缓冲区
    buffer_t* next_buf_ptr_;

    // 影子副本：存放最新接收成功的完整一帧数据，供 Getter 读取
    buffer_t data_shadow_;

    // 自旋锁：保护 data_shadow_ 的读写原子性
    struct k_spinlock lock_;

    // --- 内部处理逻辑 ---

    // 静态回调函数 (Trampoline)
    static void uart_cb(const struct device *dev, struct uart_event *evt, void *user_data);

    // 实际成员处理函数
    void process_event(struct uart_event *evt);
};