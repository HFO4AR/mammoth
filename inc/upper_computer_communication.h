//
// Created by nyuki on 2025/12/4.
//

#ifndef MAMMOTH_UPPER_COMPUTER_COMMUNICATION_H
#define MAMMOTH_UPPER_COMPUTER_COMMUNICATION_H

#include <zephyr/kernel.h>
#include <serial.h>
#define DATA_MAX_LEN 10
class UpperComputer
{
public:
    UpperComputer(const struct device* dev, k_thread_stack_t* stack, size_t stack_size,int32_t baudrate=115200) :
        serial_(dev, kDma, baudrate),
        stack_(stack),
        stack_size_(stack_size)
    {
    };
    void Init();
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
