//
// Created by nyuki on 2025/12/21.
//

#include "dji_motor.h"

// 定义模板类的静态成员变量
template<uint32_t RX_BASE_ID, uint32_t TX_ID_LOW, uint32_t TX_ID_HIGH>
DjiBusHandler GenericDjiMotor<RX_BASE_ID, TX_ID_LOW, TX_ID_HIGH>::bus_pool_[MAX_CAN_BUS_COUNT];

// 显式实例化项目中使用的模板类型
template class GenericDjiMotor<DJI_3508_RX_BASE_ID, DJI_3508_TX_ID_LOW, DJI_3508_TX_ID_HIGH>;
template class GenericDjiMotor<DJI_6020_RX_BASE_ID, DJI_6020_TX_ID_LOW, DJI_6020_TX_ID_HIGH>;