//
// Created by nyuki on 2025/12/29.
//

#ifndef MAMMOTH_KALMAN_FILTER_H
#define MAMMOTH_KALMAN_FILTER_H
#include <Eigen/Dense>

/**
 * @brief 卡尔曼滤波
 *
 * @tparam StateDim 状态向量 x 的维度
 * @tparam MeasureDim 测量向量 z 的维度
 * @tparam ControlDim 控制向量 u 的维度 (默认为0)
 */
template <int StateDim, int MeasureDim, int ControlDim = 0>
class KalmanFilter
{
public:
    using VectorState = Eigen::Matrix<double, StateDim, 1>;
    using VectorMeasure = Eigen::Matrix<double, MeasureDim, 1>;
    using VectorControl = Eigen::Matrix<double, ControlDim, 1>;

    using MatrixState = Eigen::Matrix<double, StateDim, StateDim>;
    using MatrixMeasure = Eigen::Matrix<double, MeasureDim, MeasureDim>;
    using MatrixStateMeasure = Eigen::Matrix<double, StateDim, MeasureDim>;
    using MatrixMeasureState = Eigen::Matrix<double, MeasureDim, StateDim>;
    using MatrixStateControl = Eigen::Matrix<double, StateDim, ControlDim>;

    VectorState x; // 状态向量
    MatrixState P; // 状态协方差矩阵
    MatrixState Q; // 过程噪声协方差矩阵
    MatrixMeasure R; // 测量噪声协方差矩阵

    MatrixState A; // 状态转移矩阵
    MatrixStateControl B; // 控制输入矩阵
    MatrixMeasureState H; // 测量矩阵

    /**
     * @brief 构造函数
     */
    KalmanFilter()
    {
        x.setZero();
        P.setIdentity();
        Q.setIdentity();
        R.setIdentity();
        A.setIdentity();
        B.setZero();
        H.setZero();
    }

    /**
     * @brief 初始化状态和协方差
     */
    void Init(const VectorState& initial_x, const MatrixState& initial_P)
    {
        x = initial_x;
        P = initial_P;
    }

    /**
     * @brief 预测步骤 (包含控制输入 u)
     * x' = A*x + B*u
     * P' = A*P*A^T + Q
     */
    void Predict(const VectorControl& u)
    {
        x = A * x + B * u;
        P = A * P * A.transpose() + Q;
    }

    /**
     * @brief 预测步骤 (无控制输入)
     * 重载版本，当没有 u 时调用
     */
    void Predict()
    {
        x = A * x;
        P = A * P * A.transpose() + Q;
    }

    /**
     * @brief 更新步骤
     * K = P * H^T * (H * P * H^T + R)^-1
     * x = x + K * (z - H * x)
     * P = (I - K * H) * P
     */
    void Update(const VectorMeasure& z)
    {
        MatrixMeasure S = H * P * H.transpose() + R;

        // 计算 K = P * H^T * S^-1
        // MatrixStateMeasure K = P * H.transpose() * S.inverse();
        MatrixStateMeasure K = (S.ldlt().solve(H * P)).transpose();

        VectorMeasure y = z - H * x; // 测量残差
        x = x + K * y;

        MatrixState I = MatrixState::Identity();
        P = (I - K * H) * P;
    }
};

#endif //MAMMOTH_KALMAN_FILTER_H
