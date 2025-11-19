//
// Created by nyuki on 2025/11/19.
//

#include "main.h"
#include "zephyr/kernel.h"
#include "can.h"
int init() {

    return 1;
}

int main() {
    if (init()) {
        printk("初始化成功");
    }else {
        printk("初始化失败");
    }
}
