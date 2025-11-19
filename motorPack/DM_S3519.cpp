//
// Created by nyuki on 2025/11/2.
//

#include "DM_S3519.h"

#include <algorithm>
#include <iostream>

#include "can.h"
#include "array"
#define PI 3.14159265358979323846
using namespace std;

//数据转换
/**
************************************************************************
* @brief:      	float_to_uint: 浮点数转换为无符号整数函数
* @param[in]:   x_float:	待转换的浮点数
* @param[in]:   x_min:		范围最小值
* @param[in]:   x_max:		范围最大值
* @param[in]:   bits: 		目标无符号整数的位数
* @retval:     	无符号整数结果
* @details:    	将给定的浮点数 x 在指定范围 [x_min, x_max] 内进行线性映射，映射结果为一个指定位数的无符号整数
************************************************************************
**/
int float_to_uint(float x_float, float x_min, float x_max, int bits)
{
    /* Converts a float to an unsigned int, given range and number of bits */
    float span = x_max - x_min;
    float offset = x_min;
    return (int)((x_float - offset) * ((float)((1 << bits) - 1)) / span);
}

/**
************************************************************************
* @brief:      	uint_to_float: 无符号整数转换为浮点数函数
* @param[in]:   x_int: 待转换的无符号整数
* @param[in]:   x_min: 范围最小值
* @param[in]:   x_max: 范围最大值
* @param[in]:   bits:  无符号整数的位数
* @retval:     	浮点数结果
* @details:    	将给定的无符号整数 x_int 在指定范围 [x_min, x_max] 内进行线性映射，映射结果为一个浮点数
************************************************************************
**/
float uint_to_float(int x_int, float x_min, float x_max, int bits)
{
    /* converts unsigned int to float, given range and number of bits */
    float span = x_max - x_min;
    float offset = x_min;
    return ((float)x_int) * span / ((float)((1 << bits) - 1)) + offset;
}


extern "C" void get_dm3519_data() {
    union rx_data_t {
        array<uint8_t, 8> input;
        struct {
            int64_t temp_rotor:8;
            int64_t temp_mos:8;
            int64_t torque:12;
            int64_t spd:12;
            int64_t pos:16;
            int64_t err:4;
            int64_t mst_id:4;
        }read;
    };
    rx_data_t RxData{};
    CAN_RxHeaderTypeDef RxHeader;
    HAL_CAN_GetRxMessage(&hcan2, CAN_RX_FIFO0, &RxHeader, RxData.input.data());
    reverse(RxData.input.begin(), RxData.input.end());
    float pos=uint_to_float(RxData.read.pos,0,0xffff,16);

}



void DM_S3519::set_spd(float target) {
    union tx_data_t {
        float target;
        uint8_t seed[4];
    };

    tx_data_t TxData{};
    TxData.target=target;
    uint32_t TxMailbox;
    CAN_TxHeaderTypeDef tx;
    tx.StdId = 0x200+ id;
    tx.IDE = CAN_ID_STD;
    tx.RTR = CAN_RTR_DATA;
    tx.DLC = 4;
    HAL_CAN_AddTxMessage(&hcan2, &tx, TxData.seed, &TxMailbox);
}
