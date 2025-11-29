//
// Created by nyuki on 2025/11/19.
//

#include "chassis.h"

#include "can.h"
#include "dji_m3508.h"
extern OmniChassis chassis;
/****remote thread began*****/
K_THREAD_STACK_DEFINE(chassis_stack_area, 2048);
struct k_thread chassis_thread_data;
int b=0;
void chassis_thread_entry(void *p1, void *p2, void *p3)
{
    while (true)
    {
        chassis.SetSpeed();
        k_msleep(10);
    }
}
/****remote thread end*****/

int OmniChassis::Init()
{
    k_msleep(10);
    MotorInit(0.5,0.01,0.5,2000,0.5,0.01,0.5,2000);
    SetTargetSpeed(0,0,0);
    k_thread_create(&chassis_thread_data,
                chassis_stack_area,
                K_THREAD_STACK_SIZEOF(chassis_stack_area),
                chassis_thread_entry,
                NULL,NULL,NULL,
                5, 0,K_NO_WAIT);
}
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

void OmniChassis::SetSpeed() {
    Vector4f wheel_rpm=ComputeInverseKinematics(target_vx_, target_vy_, target_omega_);
    for (int i = 0; i < 4; i++) {
        motors[i]->SetSpeed(static_cast<int>(wheel_rpm(i)));
    }
    // if (!device_is_ready(chassis_can_dev)) {
    //     printk("chassis can device is not ready\n");
    // }else
    // {
    //     motor_fl_.SendData();//发送数据，4个3508同时发送，使用的是motor_fl的can端口，但四个电机使用的是同一个can
    // }

}

void OmniChassis::MotorInit(float spd_pid_kp, float spd_pid_ki, float spd_pid_kd,float spd_pid_max_output, float pos_pid_kp, float pos_pid_ki,
                            float pos_pid_kd,float pos_pid_max_output) {
    for (int i = 0; i < 4; i++) {
        motors[i]->SetSpdPid(spd_pid_kp, spd_pid_ki, spd_pid_kd);
        motors[i]->SetPosPid(pos_pid_kp, pos_pid_ki, pos_pid_kd);
        motors[i]->SetSpeedMaxOutput(spd_pid_max_output);
        motors[i]->SetPositionMaxOutput(pos_pid_max_output);
        // motors[i]->EnableSyncSend();
    }
}

void OmniChassis::SetTargetSpeed(float vx,float vy,float omega)
{
    target_vx_=vx;
    target_vy_=vy;
    target_omega_=omega;
}


