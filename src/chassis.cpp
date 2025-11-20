//
// Created by nyuki on 2025/11/19.
//

#include "chassis.h"
Vector4f OmniChassis::ComputeInverseKinematics(float vx, float vy, float omega) {
    // 1. 构建输入向量 V [3x1]
    Vector3f chassis_vel;
    chassis_vel << vx, vy, omega;

    // 2. 矩阵乘法：得到轮子线速度 (m/s)  W = J * V
    // 结果向量: [v_fl, v_fr, v_rl, v_rr]^T
    Vector4f wheel_linear_vel = geometry_matrix_ * chassis_vel;
    // 3. 转换为转速 (RPM)
    // v = w * r  =>  w = v / r
    // RPM = (rad/s) * 60 / 2PI
    float mps_to_rpm = 60.0f / (2.0f * EIGEN_PI * wheel_radius_);
    Vector4f wheel_rpm = wheel_linear_vel * mps_to_rpm;

    // 4. 限幅/归一化保护 (防止超过电机物理极限)
    NormalizeSpeed(wheel_rpm);

    return wheel_rpm;
}

void OmniChassis::NormalizeSpeed(Vector4f& rpm) {
    float max_val = rpm.cwiseAbs().maxCoeff();//出电机转速向量中绝对值最大的那个转速值
    // 如果超过最大转速，按比例整体缩小
    if (max_val > max_wheel_rpm_) {
        float scale_factor = max_wheel_rpm_ / max_val;
        rpm *= scale_factor;
    }
}

void OmniChassis::SetSpeed() {
    Vector4f wheel_rpm=ComputeInverseKinematics(world_x_, world_y_, world_yaw_);
    motor_fl.SetSpeed(wheel_rpm(0));
    motor_fr.SetSpeed(wheel_rpm(1));
    motor_rl.SetSpeed(wheel_rpm(2));
    motor_rr.SetSpeed(wheel_rpm(3));
    motor_fl.EnableSyncSeed();
    motor_fr.EnableSyncSeed();
    motor_rl.EnableSyncSeed();
    motor_rr.EnableSyncSeed();
    DjiRm3508::SendData();
    return;
}