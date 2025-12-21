//
// Created by nyuki on 2025/11/2.
//

#ifndef MAMMOTH_MOTOR_H
#define MAMMOTH_MOTOR_H


#include "pid.h"
#include <cstdint>
#define PI 3.14159265358979323846f

enum pid_type
{
    kSpd,
    kPos
};

class Motor
{
protected:
    //电机数据
    const int id_;
    float epos_; //电气角度
    float total_epos_;//总电气角度
    int16_t cycle_count_;//多圈周期计数：对于普通电机cycle_count_ = 物理圈数，多圈计数的电机cycle_count_ = 通讯协议的周期数
    float spd_; //单位: rpm 角速度
    float cur_;
    float temp_;
    float last_pos_;
    float target_current_;
    bool motor_enable_ = true;
    float k_pos_; //角度转换系数 pos=k*epos

    //PID对象
    Pid pos_pid_;
    Pid spd_pid_;

    virtual void SetCurrent(float target); //close loop

    virtual void UpdateTotalPosition(float period);

public:

    /**
     * @brief 电机构造函数
     * @param id 电机id
     */
    Motor(const int id) : id_(id)
    {
    }
    /**
     * @brief 初始化
     * @return 是否成功
     */
    virtual bool Begin()
    {
        motor_enable_ = true;
        return motor_enable_;
    }

    /**
     * @brief 停止
     * @return 是否成功
     */
    virtual bool Stop()
    {
        motor_enable_ = false;
        SetCurrentOpenLoop(0.0f);
        return motor_enable_;
    }
    virtual ~Motor() = default;
    /**
     * @brief 设置速度环pid参数
     * @param kp P增益
     * @param ki I增益
     * @param kd D增益
     * @param max_output 输出限幅（默认为1000）
     * @param deadband 死区（默认为0）
     * @param kaw 积分抗饱和系数（默认为ki/kp）
     */
    virtual void SetSpdPid(float kp, float ki, float kd, float max_output = 1000.0f, float deadband = 0,
                           float kaw = -1.0f);
    /**
     * @brief 设置位置环pid参数
     * @param kp P增益
     * @param ki I增益
     * @param kd D增益
     * @param max_output 输出限幅（默认为1000）
     * @param deadband 死区（默认为0）
     * @param kaw 积分抗饱和系数（默认为ki/kp）
     */
    virtual void SetPosPid(float kp, float ki, float kd, float max_output = 1000.0f, float deadband = 0,
                           float kaw = -1.0f);
    /**
     * @brief 开环设置电流
     * @param target 目标值
     */
    virtual void SetCurrentOpenLoop(float target); //open loop
    /**
     * @brief pid位置环
     * @param target 目标值
     */
    virtual void SetSpeed(float target);
    /**
     * @brief 串级pid位置环
     * @param target 目标值
     */
    virtual void SetPosition(float target);
    /**
     * @brief 单环pid位置环
     * @param target 目标值
     */
    virtual void SetPositionSingleLoop(float target);
    /**
     * @brief 设置速度环最大输出（即电流限幅）
     * @param val 值
     */
    virtual void SetSpeedMaxOutput(float val);
    /**
     * @brief 设置位置环最大输出（即速度限幅）
     * @param val 值
     */
    virtual void SetPositionMaxOutput(float val);
    /**
     * @brief 设置速度环死区
     * @param val 值
     */
    virtual void SetSpeedDeadband(float val);
    /**
     * @brief 设置位置环死区
     * @param val 值
     */
    virtual void SetPositionDeadband(float val);
    /**
    * @brief Mit控制
    * @param target_pos 目标位置
    * @param target_spd 速度目标
    * @param kp P增益
    * @param kd D增益
    * @param t_ff  前馈力矩
    * @param max_output 输出限幅（默认为1000）
    */
    virtual bool SetMit(float target_pos, float target_spd, float kp, float kd, float t_ff,float max_output = 1000);

    //数据获取接口
    /**
     * @brief 获取电机多圈位置
     * @return 多圈位置
     */
    float GetTotalPosition() const { return k_pos_ * total_epos_; }
    /**
     * @brief 获取电机位置
     * @return 位置
     */
    float GetPosition() const { return k_pos_ * epos_; }
    /**
     * @brief 获取电机速度
     * @return 速度
     */
    float GetSpeed() const { return spd_; }
    /**
     * @brief 获取电机温度
     * @return 温度
     */
    float GetTemperature() const { return temp_; }
    /**
     * @brief 获取电机电流
     * @return 电流
     */
    float GetCurrent() const { return cur_; }
    /**
     * @brief 获取电机id
     * @return id
     */
    int GetId() const { return id_; }

    /**
     * @brief 获取PID输出
     * @param type PID类型（kSpd/kPos）
     * @return PID输出
     */
    float GetPidOutput(pid_type type) const
    {
        if (type == kSpd)
        {
            return spd_pid_.data.output;
        }
        else if (type == kPos)
        {
            return pos_pid_.data.output;
        }
        else
        {
            return 0.0f;
        }
    }

    /**
     * @brief 弧度转换为角度
     * @param radian 弧度值
     * @return 对应的角度值
     */
    static float Rad2Deg(float radian) { return radian * 180.0f / PI; }

    /**
     * @brief 角度转换为弧度
     * @param degree 角度值
     * @return 对应的弧度值
     */
    static float Deg2Rad(float degree) { return degree * PI / 180.0f; }

    /**
     * @brief 线速度转换为转速(RPM)
     * @param velocity 线速度值
     * @return 对应的转速值(RPM)
     */
    static float Rads2Rpm(float velocity) { return velocity * 60.0f / (2.0f * PI); }

    /**
     * @brief 转速(RPM)转换为线速度
     * @param rpm 转速值(RPM)
     * @return 对应的线速度值
     */
    static float Rpm2Rads(float rpm) { return rpm * (2.0f * PI) / 60.0f; }
    /**
     * @brief 电气角度转换为输出轴机械角度
     * @param epos 电气角度
     * @return 输出轴机械角度
     */
    virtual float Epos2Pos(float epos) { return k_pos_ * epos; }
    /**
     * @brief 输出轴机械角度转换为电气角度
     * @param pos 输出轴机械角度
     * @return 电气角度
     */
    virtual float Pos2Epos(float pos) { return pos / k_pos_; }

protected:
    /**
     * @brief 设置电机角度转换系数（子类构造时调用）
     * @param k 转换系数 pos=k*epos （pos单位：°）
     */
    void SetPositionConversionCoefficient(float k) { k_pos_ = k; }
};


#endif //MAMMOTH_MOTOR_H
