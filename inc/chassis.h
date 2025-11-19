//
// Created by nyuki on 2025/11/19.
//

#ifndef MAMMOTH_CHASSIS_H
#define MAMMOTH_CHASSIS_H
#include "Dji_Rm3508.h"
#include <Eigen/Dense>
#include <algorithm>
using namespace std;
using namespace Eigen;
class Chassis {
public:
    Matrix<float,4,3> geometry_matrix;
    float wheel_radius;
    /**
 * @brief 构造函数，初始化几何矩阵
 * @param width_span  左右轮子中心间距 (Track Width)
 * @param length_span 前后轮子中心间距 (Wheel Base)
 * @param wheel_radius 轮子半径 (米)
 * @param max_rpm      电机最大转速 (用于归一化保护)
 */
    Chassis(float width_span,float length_span,float wheel_radius,float max_rpm) {

    }
private:
};
#endif //MAMMOTH_CHASSIS_H