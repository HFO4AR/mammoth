//
// Created by nyuki on 2025/12/4.
//

#ifndef MAMMOTH_UPPER_COMPUTER_COMMUNICATION_H
#define MAMMOTH_UPPER_COMPUTER_COMMUNICATION_H

#include <zephyr/kernel.h>
#include <serial.h>
/** 最大支持接收的浮点数数量 */
#define DATA_MAX_LEN 10
/**
 * @brief 上位机通信解析类 (Thread-Safe)
 *
 * @details
 * 该类负责通过串口 DMA 接收数据，并使用流式状态机解析浮点数组。
 * 实现了双缓冲和互斥锁机制，确保外部读取数据时不会发生“数据撕裂”。
 *
 * **通信协议格式**:
 * H<数据1>,<数据2>,...,<数据N>\n
 * - H: 帧头 (Frame Begin)
 * - ,: 分隔符 (Separator)
 * - \n: 帧尾 (Frame End)
 *
 * 示例: "H12.3,45.6,-7.89\n"
 */

class UpperComputer
{
public:
    /**
 * @brief 构造函数
 *
 * @param dev        UART 设备指针 (例如 DEVICE_DT_GET(DT_NODELABEL(uart1)))
 * @param stack      线程栈内存区域 (使用 K_THREAD_STACK_DEFINE 定义)
 * @param stack_size 线程栈大小 (例如 1024)
 * @param baudrate   串口波特率 (默认 115200)
 */
    UpperComputer(const struct device* dev, k_thread_stack_t* stack, size_t stack_size,int32_t baudrate=115200) :
        serial_(dev, kDma, baudrate),
        stack_(stack),
        stack_size_(stack_size)
    {
    };
    /**
 * @brief 初始化并启动接收线程
 *
 * @note 必须在系统启动后调用一次。
 * 该函数会初始化互斥锁，并创建一个高优先级的后台线程持续解析串口数据。
 */
    void Init();

    /**
     * @brief 获取最新解析的一帧数据
     *
     * @details
     * 该函数会阻塞等待互斥锁，从内部的双缓冲区中拷贝最新的完整数据。
     * 无论串口接收频率多快，此函数总是返回最近一次成功解析的完整帧。
     *
     * @param out_buf [out] 接收数据的外部 float 数组指针
     * @param len     [in]  希望读取的最大浮点数数量 (通常为数组大小)
     */
    void GetData(float* out_buf, size_t len);
protected:
    float data_buf_[DATA_MAX_LEN]{}; // 存储解析后的 float
    enum State {
        WAIT_HEADER,
        READ_NUMBER
    } state_ = WAIT_HEADER;
    // 临时存储当前正在解析的数字字符串
    char num_buffer_[16]{};
    uint8_t num_idx_ = 0;
    // 存储结果的索引
    uint8_t data_idx_ = 0;

    Serial<512,128>  serial_;
    //线程
    k_thread_stack_t *stack_;
    size_t stack_size_;
    struct k_thread thread_data_{};
    static void ThreadEntry(void *p1, void *p2, void *p3);

    struct k_mutex data_mutex_;

    void PushNumber();

    void ProcessByte(char byte);


private:
    float temp_data_buf_[DATA_MAX_LEN];
};
#endif //MAMMOTH_UPPER_COMPUTER_COMMUNICATION_H
