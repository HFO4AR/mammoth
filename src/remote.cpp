//
// Created by nyuki on 2025/11/19.
//

#include "remote.h"
#include "dji_dbus.h"
#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>
#include <aim.h>

#include "chassis.h"

const device *const dbus_dev = DEVICE_DT_GET(DT_ALIAS(dbus));
Remote remote;
DjiDbus dbus(dbus_dev);
extern PTZ ptz;
extern OmniChassis chassis;
/****remote thread began*****/
K_THREAD_STACK_DEFINE(remote_stack_area, 1024);
struct k_thread remote_thread_data;
void remote_thread_entry(void *p1, void *p2, void *p3)
{
    DjiDbus::RemoteData local_rc;

    while (true)
    {
        dbus.GetData(local_rc);
        if (local_rc.s1)//安全检测，s1=0时表明数据接收错误，不进入处理
        {
            if (local_rc.s1==1&&local_rc.s2==1)//手动遥控模式
            {
                chassis.SetTargetSpeed(local_rc.ch3/10,local_rc.ch2/10,local_rc.ch0/10);
            }else if (local_rc.s1==1&&local_rc.s2==2)
            {
                printk("ch0:%d,ch1:%d,ch2:%d,ch3:%d\n",local_rc.ch0,local_rc.ch1,local_rc.ch2,local_rc.ch3);

            }else if (local_rc.s1==1&&local_rc.s2==3)
            {
                continue;
            }else if (local_rc.s1==2&&local_rc.s2==1)
            {
                continue;
            }else if (local_rc.s1==2&&local_rc.s2==2)
            {
                continue;
            }else if (local_rc.s1==2&&local_rc.s2==3)
            {
                continue;
            }else if (local_rc.s1==3&&local_rc.s2==1)
            {
                continue;
            }else if (local_rc.s1==3&&local_rc.s2==2)
            {
                continue;
            }else if (local_rc.s1==3&&local_rc.s2==3)
            {
                continue;
            }
        }else
        {
            chassis.SetTargetSpeed(0,0,0);
        }

        k_msleep(10);
    }
}
/****remote thread end*****/
void RemoteInit()
{
    if (!device_is_ready(dbus_dev)) {
        printk("DBUS device is not ready\n");
        return;
    }
    
    k_msleep(10);
    int ret=dbus.ReceivingData();
    if (ret != 0) {
        printk("Failed to start DBUS receiving, error: %d\n", ret);
        return;
    }
    
    k_thread_create(&remote_thread_data,
                    remote_stack_area,
                    K_THREAD_STACK_SIZEOF(remote_stack_area),
                    remote_thread_entry,
                    NULL,NULL,NULL,
                    5, 0,K_NO_WAIT);

}