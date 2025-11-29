#include "dji_dbus.h"
#include <zephyr/sys/byteorder.h>
#include <cstring>

DjiDbus::DjiDbus(const struct device *const dbus_dev)
    : dev_(dbus_dev)
{
    next_buf_ptr_ = &buf2_;
    // 清零
    memset(&data_safe_, 0, sizeof(data_safe_));
}

int DjiDbus::ReceivingData()
{
    if (!device_is_ready(dev_)) return -1;

    uart_callback_set(dev_, uart_callback, this);
    next_buf_ptr_ = &buf2_;
    
    // 启动接收
    return uart_rx_enable(dev_, buf1_.data, sizeof(buffer_t), 10000);
}

void DjiDbus::GetData(RemoteData& out_data)
{

    k_spinlock_key_t key = k_spin_lock(&lock_);
    out_data = data_safe_; // 结构体拷贝
    k_spin_unlock(&lock_, key);
}

void DjiDbus::uart_callback(const struct device *dev, struct uart_event *evt, void *user_data)
{
    DjiDbus *self = static_cast<DjiDbus*>(user_data);
    if (self) self->process_event(evt);
}

void DjiDbus::process_event(struct uart_event *evt)
{
    switch (evt->type) {
    case UART_RX_BUF_REQUEST:
        break;

    case UART_RX_RDY:
    {
        // 校验长度
        if (evt->data.rx.len == DBUS_FRAME_SIZE) {
            uint8_t* raw = evt->data.rx.buf + evt->data.rx.offset;

            k_spinlock_key_t key = k_spin_lock(&lock_);

            parse_raw_to_struct(raw + DBUS_PAYLOAD_OFFSET, data_safe_);
            
            k_spin_unlock(&lock_, key);
        }
        break;
    }

    case UART_RX_DISABLED:
    case UART_RX_STOPPED: 
    {
        // Ping-Pong 切换并重启
        buffer_t* buf_to_use = next_buf_ptr_;
        next_buf_ptr_ = (next_buf_ptr_ == &buf1_) ? &buf2_ : &buf1_;
        uart_rx_enable(dev_, buf1_.data, DBUS_FRAME_SIZE, 10000);
        break;
    }
    default: break;
    }
}

// 纯粹的解析逻辑，不涉及锁
void DjiDbus::parse_raw_to_struct(const uint8_t* ptr, RemoteData& target)
{
    target.ch0 = ((ptr[0] | (ptr[1] << 8)) & 0x07FF) - 1024;
    target.ch1 = (((ptr[1] >> 3) | (ptr[2] << 5)) & 0x07FF) - 1024;
    target.ch2 = (((ptr[2] >> 6) | (ptr[3] << 2) | (ptr[4] << 10)) & 0x07FF) - 1024;
    target.ch3 = (((ptr[4] >> 1) | (ptr[5] << 7)) & 0x07FF) - 1024;

    // 开关
    target.s1 = (ptr[5] >> 4) & 0x03;
    target.s2 = (ptr[5] >> 6) & 0x03;

    // 鼠标
    target.mouse_x = sys_get_le16(&ptr[6]);
    target.mouse_y = sys_get_le16(&ptr[8]);
    target.mouse_z = sys_get_le16(&ptr[10]);
    target.mouse_left  = ptr[12];
    target.mouse_right = ptr[13];
    
    // 键盘
    target.key = sys_get_le16(&ptr[14]);
}