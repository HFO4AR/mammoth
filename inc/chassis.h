//
// Created by nyuki on 2025/11/19.
//

#ifndef MAMMOTH_CHASSIS_H
#define MAMMOTH_CHASSIS_H
#include "dji_rm3508.h"
#include <Eigen/Dense>
using namespace std;
using namespace Eigen;
class Chassis {
public:
    float max_wheel_rpm_;
    Chassis(float max_rpm):max_wheel_rpm_(max_rpm){};
    ~Chassis() = default;
    float max_speed_;
    float world_x_;
    float world_y_;
    float world_z_;
    float world_yaw_;
    float world_pitch_;
    float world_roll_;
    float vx_;
    float vy_;
    float yaw_;
    //目标
    float target_vx_;
    float target_vy_;
    float target_omega_;
    virtual void SetSpeed() {}
};
class OmniChassis:protected Chassis{
public:
    DjiRm3508 motor_fl_;
    DjiRm3508 motor_fr_;
    DjiRm3508 motor_rl_;
    DjiRm3508 motor_rr_;
    DjiRm3508* motors[4] = {&motor_fl_, &motor_fr_, &motor_rl_, &motor_rr_};
    Matrix<float,4,3> geometry_matrix_;
    float wheel_radius_;
    /**
     * @brief 构造函数，初始化几何矩阵
     * @param width_span  左右轮子中心间距 (Track Width)
     * @param length_span 前后轮子中心间距 (Wheel Base)
     * @param wheel_radius 轮子半径 (米)
     * @param max_wheel_rpm      电机最大转速 (用于归一化保护)
     * @param motor_fl_id   左前电机 ID
     * @param motor_fr_id   右前电机 ID
     * @param motor_rl_id   左后电机 ID
     * @param motor_rr_id   右后电机 ID
     */
    OmniChassis(float width_span, float length_span, float wheel_radius, float max_wheel_rpm,int motor_fl_id,int motor_fr_id,int motor_rl_id,int motor_rr_id,const struct device * can_dev) : Chassis(max_wheel_rpm),
        wheel_radius_(wheel_radius) ,motor_fl_(motor_fl_id, can_dev), motor_fr_(motor_fr_id,can_dev), motor_rl_(motor_rl_id,can_dev), motor_rr_(motor_rr_id,can_dev){
    float a = width_span / 2.0f;
    float b = length_span / 2.0f;
    float k = a + b; // 几何系数

    // 初始化逆运动学矩阵 (对应 X 型布局)
    // 顺序: [FL, FR, RL, RR]
    // 输入: [vx, vy, omega]
    geometry_matrix_ <<
            1, -1, -k, // FL: vx - vy - k*w
            1, 1, k, // FR: vx + vy + k*w
            1, 1, -k, // RL: vx + vy - k*w
            1, -1, k; // RR: vx - vy + k*w
}

    void MotorInit(float spd_pid_kp, float spd_pid_ki, float spd_pid_kd,float spd_pid_max_output=2000,float pos_pid_kp=0, float pos_pid_ki=0, float pos_pid_kd=0,float pos_pid_max_output=2000);
    //实际底盘速度发送函数
    void SetSpeed();
    /**
     * @brief 设置底盘目标速度，此接口用于供遥控器调用
     * @param vx     底盘 X 轴速度 (m/s)
     * @param vy     底盘 Y 轴速度 (m/s)
     * @param omega  底盘自转角速度 (rad/s)
     */
    void SetTargetSpeed(float vx,float vy,float omega);

    int Init();
private:
    /**
     * @brief 逆运动学解算：输入底盘速度，输出电机转速(RPM)
     * @param vx     底盘 X 轴速度 (m/s)
     * @param vy     底盘 Y 轴速度 (m/s)
     * @param omega  底盘自转角速度 (rad/s)
     * @return Vector4f 包含四个轮子的转速 [FL, FR, RL, RR] 单位: RPM
     */
    Vector4f ComputeInverseKinematics(float vx, float vy, float omega) const;
    void NormalizeSpeed(Vector4f& rpm) const;

};
#endif //MAMMOTH_CHASSIS_H