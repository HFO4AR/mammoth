//
// Created by nyuki on 2025/11/24.
//

#include "dji_dbus.h"

DjiDbus::DjiDbus(const struct device *const dbus_dev)
    : dev_(dbus_dev)
{
    // 初始化时，假设下一个空闲的是 buf2 (因为 start 会先占用 buf1)
    next_buf_ptr_ = &buf2_;

    // 清零数据
    memset(&buf1_, 0, sizeof(buf1_));
    memset(&buf2_, 0, sizeof(buf2_));
    memset(&data_shadow_, 0, sizeof(data_shadow_));
}

void DjiDbus::ReceivingData()
{
    if (!device_is_ready(dev_)) {
        // 可以在这里打印错误日志
        return;
    }

    // 1. 设置回调，传入 'this' 指针
    uart_callback_set(dev_, uart_cb, this);

    // 2. 启动接收，先使用 buf1_
    // 注意：DJI DBus 通常需要特定的波特率(100k)和偶校验，
    // 请确保设备树(Overlay)中已经配置正确。
    uart_rx_enable(dev_, buf1_.input, sizeof(buffer_t), SYS_FOREVER_US);
}

// 静态回调入口
void DjiDbus::uart_cb(const struct device *dev, struct uart_event *evt, void *user_data)
{
    DjiDbus *self = static_cast<DjiDbus*>(user_data);
    if (self) {
        self->process_event(evt);
    }
}

// 核心事件处理
void DjiDbus::process_event(struct uart_event *evt)
{
    switch (evt->type) {

    // [关键] 硬件请求下一个缓冲区
    case UART_RX_BUF_REQUEST:
    {
        // 提供备用缓冲区
        uart_rx_buf_rsp(dev_, next_buf_ptr_->input, sizeof(buffer_t));

        // 切换指针：如果是 buf1 就切到 buf2，反之亦然
        if (next_buf_ptr_ == &buf1_) {
            next_buf_ptr_ = &buf2_;
        } else {
            next_buf_ptr_ = &buf1_;
        }
        break;
    }

    // [关键] 数据接收完成
    case UART_RX_RDY:
    {
        // 获取 DMA 收到的数据指针
        uint8_t* received_data = evt->data.rx.buf + evt->data.rx.offset;
        size_t len = evt->data.rx.len;

        // 只有当接收到完整的 6 字节时才更新影子副本
        // 防止收到半截数据导致解析错误
        if (len == sizeof(buffer_t)) {
            k_spinlock_key_t key = k_spin_lock(&lock_);

            // 将数据从 DMA Buffer 拷贝到 影子副本
            // 这里可以直接 memcpy，因为 buffer_t 大小就是 6 字节
            memcpy(&data_shadow_, received_data, sizeof(buffer_t));

            k_spin_unlock(&lock_, key);
        }
        break;
    }

    case UART_RX_DISABLED:
        // 如果接收意外停止，可以在这里尝试重新 enable
        // uart_rx_enable(dev_, next_buf_ptr_->input, sizeof(buffer_t), SYS_FOREVER_US);
        break;

    default:
        break;
    }
}

// ================= GETTER 实现 =================
// 所有的 Getter 都从 data_shadow_ 读取，并加锁防止读取时发生 ISR 修改

uint16_t DjiDbus::GetCH0()
{
    k_spinlock_key_t key = k_spin_lock(&lock_);
    uint16_t val = data_shadow_.read.ch0;
    k_spin_unlock(&lock_, key);
    return val;
}

uint16_t DjiDbus::GetCH1()
{
    k_spinlock_key_t key = k_spin_lock(&lock_);
    uint16_t val = data_shadow_.read.ch1;
    k_spin_unlock(&lock_, key);
    return val;
}

uint16_t DjiDbus::GetCH2()
{
    k_spinlock_key_t key = k_spin_lock(&lock_);
    uint16_t val = data_shadow_.read.ch2;
    k_spin_unlock(&lock_, key);
    return val;
}

uint16_t DjiDbus::GetCH3()
{
    k_spinlock_key_t key = k_spin_lock(&lock_);
    uint16_t val = data_shadow_.read.ch3;
    k_spin_unlock(&lock_, key);
    return val;
}

uint8_t DjiDbus::GetS1()
{
    k_spinlock_key_t key = k_spin_lock(&lock_);
    uint8_t val = data_shadow_.read.s1;
    k_spin_unlock(&lock_, key);
    return val;
}

uint8_t DjiDbus::GetS2()
{
    k_spinlock_key_t key = k_spin_lock(&lock_);
    uint8_t val = data_shadow_.read.s2;
    k_spin_unlock(&lock_, key);
    return val;
}