//
// Created by nyuki on 2025/11/24.
//

#ifndef MAMMOTH_SERIAL_H
#define MAMMOTH_SERIAL_H

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/uart.h>
#include <zephyr/sys/ring_buffer.h>
#include <cstdint>
#include <cstdlib>
#include <cstring>

// 定义模式枚举
enum SerialMode {
    kPolling,   // 阻塞/轮询模式
    kInterrupt, // 中断模式
    kDma        // DMA/Async模式
};

/**
 * @brief 异步串口封装类
 * @tparam RingBufferSize 用户读取用的 RingBuffer 大小
 * @tparam DmaBufferSize 单个 DMA 块的大小 (双缓冲模式下会分配两个此类 buffer)
 */
template <size_t RingBufferSize = 1024, size_t DmaBufferSize = 256>
class Serial {
 public:
    /**
     * @brief 构造函数
     * @param dev UART 设备指针 (例如 DEVICE_DT_GET(DT_NODELABEL(usart1)))
     * @param mode 工作模式，默认为阻塞模式
     * @param baudrate 波特率，默认为 115200
     * @param data_bits 数据位，默认为 8
     * @param stop_bits 停止位，默认为 1
     * @param parity 校验位，默认为无校验
     */
    explicit Serial(const struct device* dev, SerialMode mode = kPolling, uint32_t baudrate = 115200,
                    uint8_t data_bits = 8, uint8_t stop_bits = 1, uint8_t parity = UART_CFG_PARITY_NONE);

    ~Serial();

    /**
     * @brief 发送数据
     * @note DMA 模式下会阻塞直到发送完成，但由 DMA 搬运，不消耗 CPU 周期
     */
    bool Write(const uint8_t* data, uint32_t length);

    /**
     * @brief 读取数据 (从 RingBuffer 取)
     */
    int32_t Read(uint8_t* data, uint32_t max_len);

    /**
     * @brief 设置工作模式
     */
    bool SetMode(SerialMode mode);

    bool IsReady() const { return device_is_ready(dev_); }
    /**
     * @brief 数据转换函数
     * @param input 输入字符串
     * @param output 输出浮点数组
     * @param sep 分隔符
     * @param start 起始字符
     * @param end 结束字符
     * @param len 数据个数
     */
    static int char2float(char* input,float* output,char sep,char start,char end,int len);

    int SetBaudrate(uint32_t baudrate);

 protected:
    const struct device* dev_;
    SerialMode mode_;

    // --- 接收路径 (RX) ---
    struct ring_buf rx_ring_buf_;
    uint8_t rx_ring_mem_[RingBufferSize];

    // --- 发送路径 (TX) ---
    // 发送互斥锁：防止多线程同时调用 Write 导致数据错乱
    struct k_mutex tx_mutex_;
    // 发送完成信号量：用于 DMA 模式下的同步等待
    struct k_sem tx_done_sem_;
    // TX 内部缓冲
    uint8_t tx_dma_buf_[DmaBufferSize] __aligned(32);

    // --- DMA 双缓冲管理 ---
    // 定义两个对齐的 DMA 接收缓冲区
    uint8_t rx_dma_buf_a_[DmaBufferSize] __aligned(32);
    uint8_t rx_dma_buf_b_[DmaBufferSize] __aligned(32);
    // 指向下一个应该提供给硬件的 Buffer
    uint8_t* next_dma_buf_;

    // --- 回调函数 ---
    static void UartIsr(const struct device* dev, void* user_data);
    static void AsyncCallback(const struct device* dev, struct uart_event* evt, void* user_data);

    //串口配置
    struct uart_config cfg_;

 private:
    void DisableAll();
};
template <size_t RB_SZ, size_t DMA_SZ>
Serial<RB_SZ, DMA_SZ>::Serial(const struct device* dev, SerialMode mode, uint32_t baudrate, uint8_t data_bits, uint8_t stop_bits, uint8_t parity)
    : dev_(dev), mode_(kPolling), next_dma_buf_(rx_dma_buf_b_)
{
    // 初始化核心 RingBuffer
    ring_buf_init(&rx_ring_buf_, sizeof(rx_ring_mem_), rx_ring_mem_);

    // 初始化互斥锁
    k_mutex_init(&tx_mutex_);
    // 初始化信号量
    k_sem_init(&tx_done_sem_, 0, 1);

    if (device_is_ready(dev_))
    {
        SetMode(mode);
        //获取配置
        uart_config_get(dev_, &cfg_);
        cfg_.baudrate = baudrate;
        cfg_.data_bits = data_bits;
        cfg_.stop_bits = stop_bits;
        cfg_.parity = parity;
        //配置
        uart_configure(dev_, &cfg_);

    }
}

template <size_t RB_SZ, size_t DMA_SZ>
Serial<RB_SZ, DMA_SZ>::~Serial() {
    DisableAll();
}

template <size_t RB_SZ, size_t DMA_SZ>
void Serial<RB_SZ, DMA_SZ>::DisableAll() {
    uart_irq_rx_disable(dev_);
    uart_irq_tx_disable(dev_);
#ifdef CONFIG_UART_ASYNC_API
    if (mode_ == kDma) {
        uart_rx_disable(dev_);
    }
#endif
}

template <size_t RB_SZ, size_t DMA_SZ>
bool Serial<RB_SZ, DMA_SZ>::SetMode(SerialMode mode) {
    if (!device_is_ready(dev_)) return false;

    DisableAll();
    mode_ = mode;

    switch (mode_) {
        case kPolling:
            return true;

        case kInterrupt:
            // 注册中断回调，传入 'this' 作为 user_data
            uart_irq_callback_user_data_set(dev_, UartIsr, this);
            uart_irq_rx_enable(dev_);
            return true;

        case kDma:
#ifdef CONFIG_UART_ASYNC_API
            // 注册异步回调，传入 'this' 作为 user_data
            // 关键点：这里将当前对象实例指针传给底层驱动
            if (uart_callback_set(dev_, AsyncCallback, this) != 0) {
                return false;
            }

            // 初始状态：
            // 1. 启用 RX，使用 Buffer A
            // 2. 将 Buffer B 标记为下一个备用
            next_dma_buf_ = rx_dma_buf_b_;

            // 10ms 超时：如果接收不满一个 buffer 但线路空闲 10ms，也会触发 RX_RDY
            uart_rx_enable(dev_, rx_dma_buf_a_, DMA_SZ, 10000);
            return true;
#else
            return false; // 在 prj.conf 中未开启 CONFIG_UART_ASYNC_API
#endif
        default:
            return false;
    }
}

template <size_t RB_SZ, size_t DMA_SZ>
bool Serial<RB_SZ, DMA_SZ>::Write(const uint8_t* data, uint32_t length) {
    if (!data || length == 0) return false;

    // 1. 加锁，确保多线程安全
    k_mutex_lock(&tx_mutex_, K_FOREVER);
    bool ret = true;

    if (mode_ == kDma) {
#ifdef CONFIG_UART_ASYNC_API
        // DMA 模式分块发送（如果数据长度超过内部 TX Buffer）
        uint32_t sent = 0;
        while (sent < length) {
            uint32_t chunk = length - sent;
            if (chunk > DMA_SZ) chunk = DMA_SZ;

            // 拷贝到内部 RAM buffer (必须步骤，防止 data 指向栈内存或 Flash)
            memcpy(tx_dma_buf_, data + sent, chunk);

            k_sem_reset(&tx_done_sem_);

            // 启动 DMA 发送
            if (uart_tx(dev_, tx_dma_buf_, chunk, SYS_FOREVER_US) == 0) {
                // 阻塞等待发送完成信号量 (由 AsyncCallback 释放)
                k_sem_take(&tx_done_sem_, K_FOREVER);
            } else {
                ret = false;
                break;
            }
            sent += chunk;
        }
#else
        ret = false;
#endif
    } else {
        // 轮询模式 / 中断模式下的发送
        for (uint32_t i = 0; i < length; i++) {
            uart_poll_out(dev_, data[i]);
        }
    }

    k_mutex_unlock(&tx_mutex_);
    return ret;
}

template <size_t RB_SZ, size_t DMA_SZ>
int32_t Serial<RB_SZ, DMA_SZ>::Read(uint8_t* data, uint32_t max_len) {
    if (mode_ == kPolling) {
        // 非阻塞轮询读取
        uint32_t cnt = 0;
        while (cnt < max_len && uart_poll_in(dev_, &data[cnt]) == 0) {
            cnt++;
        }
        return cnt;
    } else {
        // 中断和 DMA 模式都已将数据汇聚到 ring_buf
        return ring_buf_get(&rx_ring_buf_, data, max_len);
    }
}

// -------------------------------------------------------------------------
// Callbacks (ISR context)
// -------------------------------------------------------------------------

/**
 * @brief 异步/DMA 回调函数
 *
 * 逻辑解析：
 * 1. user_data 使得我们可以区分是哪个 Serial 实例。
 * 2. UART_RX_RDY: DMA 搬运了一部分数据，我们将其拷入 RingBuffer。
 * 3. UART_RX_BUF_REQUEST: 硬件当前的 buffer 快满了，我们需要提供下一个 buffer。
 */
template <size_t RB_SZ, size_t DMA_SZ>
void Serial<RB_SZ, DMA_SZ>::AsyncCallback(const struct device* dev,
                                          struct uart_event* evt,
                                          void* user_data) {
#ifdef CONFIG_UART_ASYNC_API
    // 将 user_data 强转回当前类实例指针
    Serial* self = static_cast<Serial*>(user_data);

    switch (evt->type) {
        case UART_TX_DONE:
        case UART_TX_ABORTED:
            // 发送完成，唤醒 Write 函数
            k_sem_give(&self->tx_done_sem_);
            break;

        case UART_RX_RDY:
            // 【数据到达】
            // evt->data.rx.buf: 当前使用的 DMA buffer
            // evt->data.rx.offset: 数据起始偏移
            // evt->data.rx.len: 本次有效数据长度
            if (evt->data.rx.len > 0) {
                ring_buf_put(&self->rx_ring_buf_,
                             evt->data.rx.buf + evt->data.rx.offset,
                             evt->data.rx.len);
            }
            break;

        case UART_RX_BUF_REQUEST:
            // 请求新 Buffer (双缓冲切换)
            uart_rx_buf_rsp(dev, self->next_dma_buf_, DMA_SZ);

            // 交换 buffer 指针，准备下一次切换
            if (self->next_dma_buf_ == self->rx_dma_buf_a_) {
                self->next_dma_buf_ = self->rx_dma_buf_b_;
            } else {
                self->next_dma_buf_ = self->rx_dma_buf_a_;
            }
            break;

        case UART_RX_DISABLED:
            // 如果 RX 意外停止，尝试重启
            if (self->mode_ == kDma) {
                 uart_rx_enable(dev, self->rx_dma_buf_a_, DMA_SZ, 10000);
            }
            break;

        default:
            break;
    }
#endif
}

template <size_t RB_SZ, size_t DMA_SZ>
void Serial<RB_SZ, DMA_SZ>::UartIsr(const struct device* dev, void* user_data) {
    Serial* self = static_cast<Serial*>(user_data);

    uart_irq_update(dev);

    if (uart_irq_rx_ready(dev)) {
        uint8_t buffer[64];
        int len;
        while ((len = uart_fifo_read(dev, buffer, sizeof(buffer))) > 0) {
            ring_buf_put(&self->rx_ring_buf_, buffer, len);
        }
    }
}

template <size_t RingBufferSize, size_t DmaBufferSize>
int Serial<RingBufferSize, DmaBufferSize>::char2float(char* input, float* output, char sep, char start, char end, int len)
{
    {
        if (input== NULL||output==NULL) return -1;
        // 1. 寻找开始符的位置
        const char* startPtr = strchr(input, start);
        if (startPtr == NULL) return 0;

        // 2. 寻找结束符的位置
        const char* endPtr = strchr(startPtr, end);
        if (endPtr == NULL) return 0;

        // 3. 提取有效内容到临时缓冲
        int contentLen = endPtr - startPtr - 1;
        if (contentLen <= 0 || contentLen >= 128) return 0; // 内容为空或过长

        char tempBuf[128]; // 临时缓冲区
        strncpy(tempBuf, startPtr + 1, contentLen);
        tempBuf[contentLen] = '\0'; // 确保字符串结尾

        // 4. 分割字符串并转换
        int count = 0;
        char sepString[2] = {sep, '\0'};
        char* token = strtok(tempBuf, sepString);

        while (token != NULL && count < len) {
            output[count] = (float)atof(token); // 转换为浮点型
            count++;
            token = strtok(NULL, sepString);
        }
        return count;
    }

};

template <size_t RingBufferSize, size_t DmaBufferSize>
int Serial<RingBufferSize, DmaBufferSize>::SetBaudrate(uint32_t baudrate)
{
    cfg_.baudrate = baudrate;
    return uart_configure(dev_, &cfg_);
};


#endif //MAMMOTH_SERIAL_H