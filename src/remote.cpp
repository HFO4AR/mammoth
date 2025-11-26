//
// Created by nyuki on 2025/11/19.
//

#include "remote.h"
#include "dji_dbus.h"
#include <zephyr/kernel.h>
const device *const dbus_dev = DEVICE_DT_GET(DT_ALIAS(dbus));
Remote remote;
DjiDbus dbus(dbus_dev);
/****remote thread began*****/
K_THREAD_STACK_DEFINE(remote_stack_area, 1024);
struct k_thread remote_thread_data;
int b=0;
void remote_thread_entry(void *p1, void *p2, void *p3)
{
    while ( true)
    {
        auto remote_date=dbus.GetFrame();
        uint16_t ch0=remote_date.ch0();
        k_msleep(10);

    }

}
/****remote thread end*****/
void RemoteInit()
{
    k_msleep(10);
    int ret=dbus.ReceivingData();
    k_thread_create(&remote_thread_data,
                    remote_stack_area,
                    K_THREAD_STACK_SIZEOF(remote_stack_area),
                    remote_thread_entry,
                    NULL,NULL,NULL,
                    5, 0,K_NO_WAIT);

}
