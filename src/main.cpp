//
// Created by nyuki on 2025/11/19.
//

#include "main.h"
#include "zephyr/kernel.h"
#include "can.h"
#include <zephyr/drivers/gpio.h>
#include <load_ring.h>

#include "remote.h"
#include "motor_music.h"
static const struct gpio_dt_spec led_blue = GPIO_DT_SPEC_GET(DT_ALIAS(led1), gpios);
OmniChassis chassis(0.5,0.5,0.15,5000,1,2,3,4,chassis_can_dev);
PTZ ptz(2,1,ptz_can_dev);
int Init() {
    CanInit(chassis_can_dev);
    CanInit(ptz_can_dev);
    // ptz.Init();
    RemoteInit();
    // MusicInit();
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
    return 0;
}
