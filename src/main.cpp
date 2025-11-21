//
// Created by nyuki on 2025/11/19.
//

#include "main.h"
#include "zephyr/kernel.h"
#include "can.h"
int Init() {
    CanInit(chassis_can_dev);
    return 1;
}

int main() {
    if (Init()) {
        printk("初始化成功");
    }else {
        printk("初始化失败");
        return 0;
    }
}
