//
// Created by nyuki on 2025/11/19.
//

#include "main.h"
#include "zephyr/kernel.h"
#include "can.h"
#include <zephyr/drivers/gpio.h>
#include <load_ring.h>
static const struct gpio_dt_spec led_blue = GPIO_DT_SPEC_GET(DT_ALIAS(led1), gpios);

int Init() {
    CanInit(chassis_can_dev);
    CanInit(ptz_can_dev);
    ptz.Init();
    return 1;
}
int a=0;

int main() {
    // LoadRing();
    if (Init()) {
        printk("初始化成功");
    } else {
        printk("初始化失败");
        return 0;
    }

    gpio_pin_configure_dt(&led_blue, GPIO_OUTPUT_LOW);
    while (true) {
        // 切换LED状态
        gpio_pin_toggle_dt(&led_blue);
        // 延时200毫秒，实现LED闪烁效果
        k_msleep(200);
        a++;
    }
}
