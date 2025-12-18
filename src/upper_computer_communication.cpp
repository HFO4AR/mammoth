//
// Created by nyuki on 2025/12/4.
//

#include "upper_computer_communication.h"
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


/**
 * @brief 内部辅助：将当前 num_buffer_ 中的字符串转为 float 并存入临时缓冲区
 *
 * @note
 * - 只有在遇到分隔符 ',' 或帧尾 '\n' 时调用。
 * - 转换后的数据存入 temp_data_buf_ (私有缓冲区)，不需要加锁。
 */
void UpperComputer::PushNumber()
{
    if (num_idx_ == 0) return;

    num_buffer_[num_idx_] = '\0'; // 封口

    if (data_idx_ < DATA_MAX_LEN) {
        temp_data_buf_[data_idx_++] = strtof(num_buffer_, NULL);
    }
    num_idx_ = 0; // 重置数字缓冲区
}
/**
 * @brief 状态机核心逻辑：逐字节解析
 *
 * @param byte 从串口接收到的单个字符
 *
 * @details
 * 状态机流程:
 * 1. WAIT_HEADER: 等待 'H'，收到后重置索引，进入 READ_NUMBER。
 * 2. READ_NUMBER:
 *    - 收到数字/小数点/负号: 存入 num_buffer_。
 *    - 收到 ',': 调用 PushNumber() 解析当前数字。
 *    - 收到 '\n': 解析最后一个数字 -> 加锁 -> 同步到 data_buf_ -> 重置状态。
 *    - 收到非法字符: 复位状态机。
 */
void UpperComputer::ProcessByte(char byte)
{
    switch (state_) {
        case WAIT_HEADER:
            if (byte == frame_begin_) {
                state_ = READ_NUMBER;
                num_idx_ = 0;
                data_idx_ = 0;
            }
            break;

        case READ_NUMBER:
            if (byte == frame_sep_) {
                PushNumber();
            }
            else if (byte == frame_end_) {
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
/**
 * @brief 后台工作线程入口
 *
 * @details
 * 持续从 Serial 的 RingBuffer 中读取数据块，并逐字节喂给状态机。
 */
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
