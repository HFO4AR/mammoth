//
// Created by nyuki on 2025/11/19.
//

#include "chassis.h"
#include "dji_rm3508.h"
Vector4f OmniChassis::ComputeInverseKinematics(float vx, float vy, float omega) const {
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

void OmniChassis::NormalizeSpeed(Vector4f& rpm) const {
    float max_val = rpm.cwiseAbs().maxCoeff();//出电机转速向量中绝对值最大的那个转速值
    // 如果超过最大转速，按比例整体缩小
    if (max_val > max_wheel_rpm_) {
        float scale_factor = max_wheel_rpm_ / max_val;
        rpm *= scale_factor;
    }
}

void OmniChassis::SetSpeed(float vx,float vy,float omega) {
    Vector4f wheel_rpm=ComputeInverseKinematics(vx, vy, omega);
    for (int i = 0; i < 4; i++) {
        motors[i]->SetSpeed(static_cast<int>(wheel_rpm(i)));
    }
    motor_fl_.SendData();//发送数据，4个3508同时发送，使用的是motor_fl的can端口，但四个电机使用的是同一个can
}

void OmniChassis::MotorInit(float spd_pid_kp, float spd_pid_ki, float spd_pid_kd,float spd_pid_max_output, float pos_pid_kp, float pos_pid_ki,
                            float pos_pid_kd,float pos_pid_max_output) {
    for (int i = 0; i < 4; i++) {
        motors[i]->SetSpdPid(spd_pid_kp, spd_pid_ki, spd_pid_kd);
        motors[i]->SetPosPid(pos_pid_kp, pos_pid_ki, pos_pid_kd);
        motors[i]->SetSpeedMaxOutput(spd_pid_max_output);
        motors[i]->SetPositionMaxOutput(pos_pid_max_output);
        motors[i]->EnableSyncSend();
    }
}


