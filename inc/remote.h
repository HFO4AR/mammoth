//
// Created by nyuki on 2025/11/19.
//

#ifndef MAMMOTH_REMOTE_H
#define MAMMOTH_REMOTE_H
#include <zephyr/kernel.h>
class Remote {
public:
    Remote(int deadband,k_thread_stack_t *stack,size_t stack_size):deadband_(deadband),stack_(stack),stack_size_(stack_size){}
    int Init();
protected:
    int deadband_;
private:
    static void ThreadEntry(void *p1, void *p2, void *p3);
    k_thread_stack_t *stack_;
    size_t stack_size_;
    struct k_thread thread_data_;
};

#endif //MAMMOTH_REMOTE_H