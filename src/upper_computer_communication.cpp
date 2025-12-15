//
// Created by nyuki on 2025/12/4.
//

#include "upper_computer_communication.h"
#define FRAME_BEGIN 'H'
#define FRAME_END '\n'
#define FRAME_SEP ','
void UpperComputer::Init()
{
    k_mutex_init(&data_mutex_);
    k_msleep(10);
    k_thread_create(&thread_data_,
                stack_,
                stack_size_,
                ThreadEntry,
                this,NULL,NULL,
                5, 0,K_NO_WAIT);
}


// 辅助函数：将缓冲区的小字符串转为 float 并存入数组
void UpperComputer::PushNumber()
{
    if (num_idx_ == 0) return;

    num_buffer_[num_idx_] = '\0'; // 封口

    if (data_idx_ < DATA_MAX_LEN) {
        temp_data_buf_[data_idx_++] = strtof(num_buffer_, NULL);
    }
    num_idx_ = 0; // 重置数字缓冲区
}

// 核心处理逻辑：每收到一个字节调用一次
void UpperComputer::ProcessByte(char byte)
{
    switch (state_) {
        case WAIT_HEADER:
            if (byte == FRAME_BEGIN) {
                state_ = READ_NUMBER;
                num_idx_ = 0;
                data_idx_ = 0;
            }
            break;

        case READ_NUMBER:
            if (byte == FRAME_SEP) {
                PushNumber();
            }
            else if (byte == FRAME_END) {
                PushNumber();
                k_mutex_lock(&data_mutex_, K_FOREVER);
                memcpy(data_buf_, temp_data_buf_, sizeof(data_buf_));
                k_mutex_unlock(&data_mutex_);
                if (data_idx_ < DATA_MAX_LEN) {
                    size_t clear_start = data_idx_;
                    size_t clear_len = DATA_MAX_LEN - data_idx_;
                    memset(data_buf_ + clear_start, 0, clear_len * sizeof(float));
                }
                state_ = WAIT_HEADER; // 回到初始状态等待下一帧
            }
            else if ((byte >= '0' && byte <= '9') || byte == '.' || byte == '-') {
                if (num_idx_ < sizeof(num_buffer_) - 1) {
                    num_buffer_[num_idx_++] = byte;
                }
            }
            else if (byte == '\r' || byte == ' ') {
                // ignore
            }
            else {
                state_ = WAIT_HEADER;
            }
            break;
    }
}

void UpperComputer::ThreadEntry(void *p1, void *p2, void *p3)
{
    auto *self = static_cast<UpperComputer*>(p1);
    uint8_t chunk[64];

    while (true)
    {
        // 1. 批量读取以减少锁的开销
        int32_t len = self->serial_.Read(chunk, sizeof(chunk));

        if (len > 0) {
            // 2. 逐字节喂给状态机
            for (int i = 0; i < len; i++) {
                self->ProcessByte((char)chunk[i]);
            }
        } else {
            k_msleep(5);
        }
    }
}

void UpperComputer::GetData(float* out_buf, size_t len) {
    k_mutex_lock(&data_mutex_, K_FOREVER);
    size_t copy_len = (len > DATA_MAX_LEN) ? DATA_MAX_LEN : len;
    memcpy(out_buf, data_buf_, copy_len * sizeof(float));
    k_mutex_unlock(&data_mutex_);
}
