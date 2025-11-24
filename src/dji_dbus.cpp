//
// Created by nyuki on 2025/11/24.
//

#include "dji_dbus.h"

DjiDbus::DjiDbus(const struct device *const dbus_dev)
    : dev_(dbus_dev)
{
    // 初始化
    next_buf_ptr_ = &buf2_;

    // 清零数据
    memset(&buf1_, 0, sizeof(buf1_));
    memset(&buf2_, 0, sizeof(buf2_));
    memset(&data_shadow_, 0, sizeof(data_shadow_));
}

void DjiDbus::ReceivingData()
{

    uart_callback_set(dev_, uart_callback, this);

    //启动接收，先使用 buf1_

    uart_rx_enable(dev_, buf1_.input, sizeof(buffer_t), SYS_FOREVER_US);
}

// 静态回调入口
void DjiDbus::uart_callback(const struct device *dev, struct uart_event *evt, void *user_data)
{
    DjiDbus *self = static_cast<DjiDbus*>(user_data);
    if (self) {
        self->process_event(evt);
    }
}


void DjiDbus::process_event(struct uart_event *evt)
{
    switch (evt->type) {

    //请求下一个缓冲区
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

    //数据接收完成
    case UART_RX_RDY:
    {
        // 获取 DMA 收到的数据指针
        uint8_t* received_data = evt->data.rx.buf + evt->data.rx.offset;
        size_t len = evt->data.rx.len;

        // 只有当接收到完整的 6 字节时才更新影子副本

        if (len == sizeof(buffer_t)) {
            k_spinlock_key_t key = k_spin_lock(&lock_);


            memcpy(&data_shadow_, received_data, sizeof(buffer_t));

            k_spin_unlock(&lock_, key);
        }
        break;
    }

    case UART_RX_DISABLED:
        // uart_rx_enable(dev_, next_buf_ptr_->input, sizeof(buffer_t), SYS_FOREVER_US);
        break;

    default:
        break;
    }
}

DjiDbus::Frame DjiDbus::GetFrame()
{
    k_spinlock_key_t key = k_spin_lock(&lock_);
    buffer_t temp = data_shadow_;
    k_spin_unlock(&lock_, key);
    return Frame(temp);
}
