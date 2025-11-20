//
// Created by nyuki on 2025/11/2.
//

#ifndef MAMMOTH_PID_H
#define MAMMOTH_PID_H
#ifdef __cplusplus
class Pid {
public:
    typedef struct {
        float Kp;
        float Ki;
        float Kd;
        float Kaw;
        float last_pos;
        float Kp_output;
        float Ki_output;
        float Kd_output;
        float output;
        float deadband;
        float error;
        float last_error;
        float target;
        float actual;
        float max_output;
    } pid_data_t;

    pid_data_t data;

    Pid () =default;
    void Compuate();
    void Init(float kp, float ki, float kd, float kaw, float max_output);


};
#endif

#endif //MAMMOTH_PID_H