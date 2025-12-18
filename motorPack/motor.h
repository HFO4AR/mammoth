//
// Created by nyuki on 2025/11/2.
//

#ifndef MAMMOTH_MOTOR_H
#define MAMMOTH_MOTOR_H


#include "pid.h"
#include <cstdint>
enum pid_type
{
    kSpd,
    kPos
};
class Motor {
protected:
    float pos_;
    float spd_;
    float cur_;
    float temp_;
    float last_pos_;
    float target_current_;
    bool motor_enable_ =true;

    Pid pos_pid_;
    Pid spd_pid_;
    const int id_;
    virtual void SetCurrent(float target);//close loop
    float total_pos_;
    int16_t round_count_;
public:

    virtual ~Motor() = default;

    Motor (const int id):id_(id) {}

    virtual void SetSpdPid(float kp, float ki, float kd, float max_output=1000.0f, float deadband=0,float kaw=-1.0f);

    virtual void SetPosPid(float kp, float ki, float kd, float max_output=1000.0f, float deadband=0,float kaw=-1.0f);

    virtual void SetCurrentOpenLoop(float target);//open loop
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
    virtual void SetPositionSingleLoop(int target);
    /**
     * @brief 设置最大速度
     * @param val 目标值
     */
    virtual void SetSpeedMaxOutput(float val);

    virtual void SetPositionMaxOutput(float val);

    virtual void SetSpeedDeadband(float val);

    virtual void SetPositionDeadband(float val);

    virtual void SetMit(float target_pos, float target_spd, float kp, float kd, float t_ff,uint16_t max_output);

    virtual void UpdateTotalPosition();

    //数据获取接口
    float GetTotalPosition() const{return total_pos_;}

    float GetPosition() const{return pos_;}

    float GetSpeed() const{return spd_;}

    float GetTemperature() const{return temp_;}

    float GetCurrent() const {return cur_;}

    int GetId() const {return id_;}


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

};


#endif //MAMMOTH_MOTOR_H