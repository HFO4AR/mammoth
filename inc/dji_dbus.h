#pragma once

#include <zephyr/kernel.h>
#include <zephyr/drivers/uart.h>
#include <cstdint>
#include <cstring>
#include <type_traits>

class DjiDbus
{
public:
    DjiDbus(const struct device *const dbus_dev);

    // 启动接收
    void ReceivingData();

    // Getter 方法 (从影子副本读取，线程安全)


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

    // 影子副本：存放最新接收成功的完整一帧数据
    buffer_t data_shadow_;

    // 自旋锁：保护 data_shadow_ 的读写原子性
    struct k_spinlock lock_;

    // --- 内部处理逻辑 ---

    // 静态回调函数 (Trampoline)
    static void uart_callback(const struct device *dev, struct uart_event *evt, void *user_data);

    // 实际成员处理函数
    void process_event(struct uart_event *evt);

public:
    class Frame {
    private:
        buffer_t data_; // 这里保存的是副本，不是指针！

        // 私有构造，只允许 DjiDbus 创建它
        friend class DjiDbus;
        explicit Frame(const buffer_t& raw) : data_(raw) {}

    public:
        // 默认构造函数 (允许用户在 main 里声明变量)
        Frame() = default;

        // --- 这里就是你想要的逻辑封装 ---
        // 因为 data_ 是本地副本，不需要加锁，也没有位域指针问题

        uint16_t ch0() const { return static_cast<int16_t>(data_.read.ch0) - 1024; }
        uint16_t ch1() const { return static_cast<int16_t>(data_.read.ch1) - 1024; }
        uint16_t ch2() const { return static_cast<int16_t>(data_.read.ch2) - 1024; }
        uint16_t ch3() const { return static_cast<int16_t>(data_.read.ch3) - 1024; }

        uint8_t  s1()  const { return data_.read.s1; }
        uint8_t  s2()  const { return data_.read.s2; }
    };

    // --------------------------------------------------------
    // 主接口：获取当前时刻的帧
    // --------------------------------------------------------
    Frame GetFrame() {
        k_spinlock_key_t key = k_spin_lock(&lock_);
        // 1. 在锁内快速拷贝数据
        buffer_t temp = data_shadow_;
        k_spin_unlock(&lock_, key);

        // 2. 返回封装好的对象
        return Frame(temp);
    }
};
