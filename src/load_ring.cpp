//
// Created by nyuki on 2025/11/19.
//

#include "load_ring.h"
#include <zephyr/drivers/pwm.h>

#include "zephyr/kernel.h"
static const struct device *const pwm0= DEVICE_DT_GET(DT_ALIAS(pwm0));
int ms2ns(float ms)
{
    return static_cast<int> (ms* 1000000.0f);
}
int us2ns(float us)
{
    return static_cast<int> (us* 1000.0f);
}
int ret;
void LoadRing(){
    while (true)
    {
        ret = pwm_set(pwm0, 4, ms2ns(20), us2ns(600), 0);
        k_msleep(1000);
        ret = pwm_set(pwm0, 4, ms2ns(20), us2ns(2400), 0);
        k_msleep(1000);
    }

}