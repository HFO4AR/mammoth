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
#include "serial.h"
#include "upper_computer_communication.h"
#include "test.h"
//led指示灯
static const struct gpio_dt_spec led_blue = GPIO_DT_SPEC_GET(DT_ALIAS(led1), gpios);
//底盘
K_THREAD_STACK_DEFINE(chassis_stack_area, 4096);
OmniChassis chassis(0.5, 0.5, 0.15, 5000,
                    1, 2, 4, 3,
                    chassis_can_dev, chassis_stack_area,
                    K_THREAD_STACK_SIZEOF(chassis_stack_area));
//云台
K_THREAD_STACK_DEFINE(ptz_stack_area, 4096);
PTZ ptz(6,5,ptz_can_dev,ptz_stack_area,K_THREAD_STACK_SIZEOF(ptz_stack_area));
//遥控器
extern Remote remote;
//上位机
static const struct device *upper_uart_dev = DEVICE_DT_GET(DT_NODELABEL(usart1));
K_THREAD_STACK_DEFINE(upper_stack_area, 1024);
UpperComputer upper_computer(upper_uart_dev, upper_stack_area, K_THREAD_STACK_SIZEOF(upper_stack_area));


bool Init() {
    Test();
    CanInit(chassis_can_dev);
    CanInit(ptz_can_dev);
    // ptz.Init();
    remote.Init();
    chassis.Init();
    upper_computer.Init();
    // MusicInit();
    return true;
}
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
        //运行状态指示灯
        gpio_pin_toggle_dt(&led_blue);
        k_msleep(200);
    }
    return 0;
}
