#include "mymath.h"
#include <QDebug>

#include <algorithm>
#include <limits>
const double SEA_LEVEL_PRESSURE = 101325.0; // Pa
const double SEA_LEVEL_TEMPERATURE = 288.15; // K
const double TEMPERATURE_LAPSE_RATE = 0.0065; // K/m
const double GAS_CONSTANT_AIR = 287.05; // J/(kg·K)
const double GRAVITATIONAL_ACCELERATION = 9.80665; // m/s²
const double SUTHERLAND_CONSTANT = 110.4; // K
const double REFERENCE_TEMPERATURE = 273.15; // K
const double REFERENCE_VISCOSITY = 1.716e-5; // Pa·s
const double R_TMP = 287.05; // 空气气体常数 J/(kg·K)
const double GAMMA = 1.4; // 绝热指数
myMath::myMath() {

}
QVector<double> myMath::getInterpolateThrust(const QVector<QVector<double>>&thrusts,const QVector<double>&rpms,const double rpm){


    QVector<double>result;
    for(int i = 0;i<thrusts[0].length();i++){
        QVector<double>tmp = {thrusts[0][i],thrusts[1][i],thrusts[2][i]};
        result.append(cubicSpline(rpms,tmp,rpm));
    }


    return result;
}

QVector<double> myMath::interpolateThrustAtSpeed(const QVector<double>& speeds, const QVector<double>& thrusts, const QVector<double>& querySpeeds) {
    QVector<double> result(querySpeeds.size());

    // 确保 speeds 和 thrusts 的大小一致
    if (speeds.size() != thrusts.size() || speeds.size() < 2) {
        throw std::invalid_argument("Speeds and thrusts must have the same size and at least two points.");
    }

    for (int q = 0; q < querySpeeds.size(); ++q) {
        double querySpeed = querySpeeds[q];

        // 找到查询速度所在的区间
        int i = 0;
        while (i < speeds.size() - 1 && speeds[i + 1] < querySpeed) {
            ++i;
        }

        // 处理查询速度超出数据范围的情况
        if (querySpeed <= speeds[0]) {
            // 超出数据范围下限
            i = 0;
        } else if (querySpeed >= speeds[speeds.size() - 1]) {
            // 超出数据范围上限
            i = speeds.size() - 2;
        }

        // 线性插值
        double x0 = speeds[i];
        double x1 = speeds[i + 1];
        double y0 = thrusts[i];
        double y1 = thrusts[i + 1];

        // 计算插值
        double t = (querySpeed - x0) / (x1 - x0);
        result[q] = y0 + t * (y1 - y0);
    }

    return result;
}


double myMath::quadraticInterpolation(const QVector<double> &x, const QVector<double> &y, double xInterp) {
    int n = x.size();

    // 检查输入是否有效
    if (n != y.size() || n < 3) {
        return 0.0;
    }

    double result = 0.0;

    // 拉格朗日插值法
    for (int i = 0; i < n; ++i) {
        double term = y[i];
        for (int j = 0; j < n; ++j) {
            if (j != i) {
                term *= (xInterp - x[j]) / (x[i] - x[j]);
            }
        }
        result += term;
    }

    return result;
}
double myMath::quadraticInterpolation(double x0, double y0,
                             double x1, double y1,
                             double x2, double y2,
                             double x) {
    // 检查x坐标是否重复（避免除以零）
    if (qFuzzyCompare(x0, x1) || qFuzzyCompare(x1, x2) || qFuzzyCompare(x0, x2)) {
        throw std::invalid_argument("Duplicate x coordinates in input points");
    }

    // 计算二次插值多项式的系数
    double denom = (x0 - x1) * (x0 - x2) * (x1 - x2);
    double a = (y0 * (x1 - x2) + y1 * (x2 - x0) + y2 * (x0 - x1)) / denom;
    double b = (y0 * (x1*x1 - x2*x2) + y1 * (x2*x2 - x0*x0) + y2 * (x0*x0 - x1*x1)) / denom;
    double c = (y0 * x1 * x2 * (x1 - x2) + y1 * x0 * x2 * (x2 - x0) + y2 * x0 * x1 * (x0 - x1)) / denom;

    // 返回二次多项式在x处的值: y = a*x² + b*x + c
    return a * x * x + b * x + c;
}
double myMath::threePointInterpolation(const QVector<double>& xVec, const QVector<double>& yVec, double x) {
    if (xVec.size() != yVec.size() || xVec.size() < 3) {
        throw std::invalid_argument("Input vectors must have the same size and contain at least three points.");
    }

    // If x is within the range of xVec
    for (int i = 1; i < xVec.size() - 1; ++i) {
        if (x >= xVec[i-1] && x <= xVec[i+1]) {
            // Get three neighboring points for interpolation
            double x0 = xVec[i-1];
            double y0 = yVec[i-1];
            double x1 = xVec[i];
            double y1 = yVec[i];
            double x2 = xVec[i+1];
            double y2 = yVec[i+1];

            // Calculate the interpolation using a quadratic polynomial
            double a = (y2 - (x2 - x1) * (y1 - y0) / (x1 - x0) - y0) / ((x2 - x0) * (x2 - x1));
            double b = (y1 - y0) / (x1 - x0) - a * (x1 + x0);
            double c = y0 - a * x0 * x0 - b * x0;

            double y = a * x * x + b * x + c;
            return y;
        }
    }

    // If x is out of range, use the first or last three points for interpolation
    if (x < xVec.first()) {
        double x0 = xVec[0];
        double y0 = yVec[0];
        double x1 = xVec[1];
        double y1 = yVec[1];
        double x2 = xVec[2];
        double y2 = yVec[2];

        double a = (y2 - (x2 - x1) * (y1 - y0) / (x1 - x0) - y0) / ((x2 - x0) * (x2 - x1));
        double b = (y1 - y0) / (x1 - x0) - a * (x1 + x0);
        double c = y0 - a * x0 * x0 - b * x0;

        return a * x * x + b * x + c;
    } else if (x > xVec.last()) {
        int n = xVec.size();
        double x0 = xVec[n - 3];
        double y0 = yVec[n - 3];
        double x1 = xVec[n - 2];
        double y1 = yVec[n - 2];
        double x2 = xVec[n - 1];
        double y2 = yVec[n - 1];

        double a = (y2 - (x2 - x1) * (y1 - y0) / (x1 - x0) - y0) / ((x2 - x0) * (x2 - x1));
        double b = (y1 - y0) / (x1 - x0) - a * (x1 + x0);
        double c = y0 - a * x0 * x0 - b * x0;

        return a * x * x + b * x + c;
    }

    return 0; // Should theoretically never reach here
}
double myMath::linearInterpolation(const QVector<double>& xVec, const QVector<double>& yVec, double x) {
    if (xVec.size() != yVec.size() || xVec.size() == 0) {

        //throw std::invalid_argument("Input vectors must have the same non-zero size.");
        return 0;
    }

    // 如果 x 在 xVec 范围内
    for (int i = 0; i < xVec.size() - 1; ++i) {
        if (x >= xVec[i] && x <= xVec[i+1]) {
            double x1 = xVec[i];
            double y1 = yVec[i];
            double x2 = xVec[i+1];
            double y2 = yVec[i+1];

            // 计算线性插值
            double y = y1 + (y2 - y1) * (x - x1) / (x2 - x1);
            return y;
        }
    }

    // 如果 x 超出范围，使用末尾的两个点进行插值
    if (x < xVec.first()) {
        double x1 = xVec.first();
        double y1 = yVec.first();
        double x2 = xVec[1];
        double y2 = yVec[1];
        double y = y1 + (y2 - y1) * (x - x1) / (x2 - x1);
        return y;
    } else if (x > xVec.last()) {
        double x1 = xVec[xVec.size() - 2];
        double y1 = yVec[yVec.size() - 2];
        double x2 = xVec.last();
        double y2 = yVec.last();
        double y = y1 + (y2 - y1) * (x - x1) / (x2 - x1);
        return y;
    }

    return 0;

    // // 理论上不会到达这里，添加以防万一
    // throw std::out_of_range("Unhandled interpolation case.");
}

double myMath::alphaFromClLinearFit(
    const QVector<double>& alpha,
    const QVector<double>& cl,
    double clTarget,
    double alphaMin,
    double alphaMax)
{
    int n = alpha.size();
    if (n != cl.size() || n < 2)
        return 0.0;

    // ---------------- 线性段数据 ----------------
    QVector<double> aLin, clLin;
    for (int i = 0; i < n; ++i) {
        if (alpha[i] >= alphaMin && alpha[i] <= alphaMax) {
            aLin.push_back(alpha[i]);
            clLin.push_back(cl[i]);
        }
    }

    int m = aLin.size();
    if (m < 2)
        return 0.0;

    // ---------------- 最小二乘拟合 Cl = k*α + b ----------------
    double Sx = 0, Sy = 0, Sxx = 0, Sxy = 0;
    for (int i = 0; i < m; ++i) {
        Sx  += aLin[i];
        Sy  += clLin[i];
        Sxx += aLin[i] * aLin[i];
        Sxy += aLin[i] * clLin[i];
    }

    double denom = m * Sxx - Sx * Sx;
    if (std::abs(denom) < 1e-12)
        return 0.0;

    double k = (m * Sxy - Sx * Sy) / denom;
    double b = (Sy - k * Sx) / m;

    // ---------------- 反算 α ----------------
    if (std::abs(k) < 1e-12)
        return 0.0;

    return (clTarget - b) / k;
}

//根据高度计算密度
double myMath::calculateAirDensity(double altitude){


    double temperature = SEA_LEVEL_TEMPERATURE - TEMPERATURE_LAPSE_RATE * altitude;
    double pressure = SEA_LEVEL_PRESSURE * pow((1 - TEMPERATURE_LAPSE_RATE * altitude / SEA_LEVEL_TEMPERATURE),
                                               (GRAVITATIONAL_ACCELERATION / (GAS_CONSTANT_AIR * TEMPERATURE_LAPSE_RATE)));
    double density = pressure / (GAS_CONSTANT_AIR * temperature);
    return density;
}
// 根据高度计算空气温度的函数
double myMath::calculateTemperature(double altitude) {
    return SEA_LEVEL_TEMPERATURE - TEMPERATURE_LAPSE_RATE * altitude;
}
double myMath::getSoundSpeed(double temperature) {
    return sqrt(GAMMA * R_TMP * temperature);
}
// 使用 Sutherland's 公式计算空气粘度的函数
double myMath::calculateAirViscosity(double temperature) {
    return REFERENCE_VISCOSITY * (temperature / REFERENCE_TEMPERATURE) * pow((REFERENCE_TEMPERATURE + SUTHERLAND_CONSTANT) / (temperature + SUTHERLAND_CONSTANT), 1.5);
}

QVector<double> myMath::smoothCurve(const QVector<double>&x,const QVector<double>&y,const int order){
    // 将 QVector 转换为 Eigen 的向量
    int n = x.size();
    Eigen::VectorXd eigenX(n);
    Eigen::VectorXd eigenY(n);
    for (int i = 0; i < n; ++i) {
        eigenX(i) = x[i];
        eigenY(i) = y[i];
    }

    // 构造拟合矩阵 A 和向量 b
    Eigen::MatrixXd A(n, order + 1);
    Eigen::VectorXd b(n);

    // 填充矩阵 A 和向量 b
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j <= order; ++j) {
            A(i, j) = pow(eigenX(i), j);
        }
        b(i) = eigenY(i);
    }

    // 求解拟合系数
    Eigen::VectorXd coeffs = A.householderQr().solve(b);

    // 将 Eigen 的向量转换为 QVector
    QVector<double> fittedY(n);
    for (int i = 0; i < n; ++i) {
        double fitted_y = 0;
        for (int j = 0; j <= order; ++j) {
            fitted_y += coeffs(j) * pow(eigenX(i), j);
        }
        fittedY[i] = fitted_y;
    }
    return fittedY;
}


int myMath::getMaxIndex(const QVector<double>&data){
    int index = 0;
    double maxValue = data[0];
    for(int i = 1;i<data.size();i++){
        if(data[i] > maxValue){
            maxValue = data[i];
            index = i;
        }
    }
    return index;
}

int myMath::getMinIndex(const QVector<double>&data){
    int index = 0;
    double minValue = data[0];
    for(int i = 1;i<data.size();i++){
        if(data[i] < minValue){
            minValue = data[i];
            index = i;
        }
    }
    return index;
}
int myMath::minIndex(const QVector<double>&a,double t){
    QVector<double>error;
    for(int i = 0;i<a.length();i++)
        error.append(abs(a[i] - t));
    int j = 0;
    double minValue = error[0];
    for(int i = 1;i<error.size();i++){
        if(error[i] < minValue){
            minValue = error[i];
            j++;
        }
    }
    return j;
}
double myMath::maxV(const QVector<double>& data){
    double maxValue = data[0];
    for(int i = 1;i<data.size();i++){
        if(data[i] > maxValue){
            maxValue = data[i];
        }
    }
    return maxValue;
}
double myMath::minV(const QVector<double>& data){
    double minValue = data[0];
    for(int i = 1;i<data.size();i++){
        if(data[i] < minValue){
            minValue = data[i];
        }
    }
    return minValue;
}
double myMath::findMaxValue(const QVector<QVector<double>>& data) {
    double maxValue = std::numeric_limits<double>::lowest();  // 初始化为最小的 double 值
    for (const auto& row : data) {
        for (double value : row) {
            if (value > maxValue) {
                maxValue = value;
            }
        }
    }
    return maxValue;
}
double myMath::findMinValue(const QVector<QVector<double>>& data) {
    double minValue = std::numeric_limits<double>::max();  // 初始化为最大的 double 值
    for (const auto& row : data) {
        for (double value : row) {
            if (value < minValue) {
                minValue = value;
            }
        }
    }
    return minValue;
}
double myMath::cubicSpline(const QVector<double>& x, const QVector<double>& y, double xq) {
    int n = x.size();
    if (n != y.size() || n < 2) {
        qFatal("x and y must have the same length and length must be at least 2.");
    }

    QVector<double> h(n - 1);
    QVector<double> a = y;
    QVector<double> b(n - 1);
    QVector<double> c(n); // 用于存储每个区间的二次项系数
    QVector<double> d(n - 1);

    // 计算 h
    for (int i = 0; i < n - 1; ++i) {
        h[i] = x[i + 1] - x[i];
        if (h[i] <= 0.0) {
            qWarning() << "cubicSpline requires strictly increasing x values";
            return std::numeric_limits<double>::quiet_NaN();
        }
    }

    // 构建矩阵 A 和向量 B
    Eigen::MatrixXd A = Eigen::MatrixXd::Zero(n, n);
    Eigen::VectorXd B = Eigen::VectorXd::Zero(n);

    A(0, 0) = 1;
    A(n - 1, n - 1) = 1;

    for (int i = 1; i < n - 1; ++i) {
        A(i, i - 1) = h[i - 1];
        A(i, i) = 2 * (h[i - 1] + h[i]);
        A(i, i + 1) = h[i];
        B[i] = 3 * ((y[i + 1] - y[i]) / h[i] - (y[i] - y[i - 1]) / h[i - 1]);
    }

    // 解线性方程组 A * c = B
    Eigen::VectorXd c_vec = A.lu().solve(B);

    // 将 Eigen::VectorXd 转换回 QVector<double>
    for (int i = 0; i < n; ++i) {
        c[i] = c_vec[i];
    }

    // 计算 b 和 d
    for (int i = 0; i < n - 1; ++i) {
        b[i] = (y[i + 1] - y[i]) / h[i] - h[i] * (2 * c[i] + c[i + 1]) / 3;
        d[i] = (c[i + 1] - c[i]) / (3 * h[i]);
    }

    // 查找 xq 所在的区间
    int i = 0;
    while (i < n - 1 && xq > x[i + 1]) {
        ++i;
    }
    i = std::min(i, n - 2);

    // 计算插值
    double dx = xq - x[i];
    double yq = a[i] + b[i] * dx + c[i] * dx * dx + d[i] * dx * dx * dx;

    return yq;
}

QVector<double> myMath::smooth(const QVector<double>& x, const QVector<double>& y,
                                       double threshold) {
    // 检查输入有效性
    if (x.size() != y.size() || x.size() < 4) {
        return y; // 数据不匹配或太少，直接返回原始数据
    }

    // 1. 计算三阶导数(基于x-y坐标)
    QVector<double> thirdDiff = thirdDerivative(x, y);

    // 2. 检测异常点
    QVector<int> outliers = findOutliers(thirdDiff, threshold);

    // 3. 修正异常点
    QVector<double> correctedY = y;
    int n = x.size();

    for (int pos : outliers) {
        // 收集周围正常点
        QVector<double> validX, validY;
        for (int i = qMax(0, pos - 3); i <= qMin(n - 1, pos + 3); ++i) {
            // 排除其他异常点
            if (!std::count(outliers.begin(), outliers.end(), i)) {
                validX.append(x[i]);
                validY.append(y[i]);
            }
        }

        // 至少需要4个点进行插值
        if (validX.size() >= 4) {
            correctedY[pos] = splineInterp(validX, validY, x[pos]);
        }
    }

    // 4. 全局三次样条平滑，确保整体连续性
    QVector<double> smoothedY(n);
    for (int i = 0; i < n; ++i) {
        smoothedY[i] = splineInterp(x, correctedY, x[i]);
    }

    return smoothedY;
}
double myMath::interpolateWithLinear(const QVector<double>& dragArray, const QVector<double>& liftArray, double targetLift) {
    int n = liftArray.size();

    // 检查输入的数组是否长度一致
    if (dragArray.size() != n) {
        throw std::invalid_argument("dragArray and liftArray must have the same length");
    }

    // 遍历找到最接近目标值的两个点
    for (int i = 0; i < n - 1; ++i) {
        double x0 = liftArray[i];
        double x1 = liftArray[i + 1];

        // 如果targetLift在区间[x0, x1]之间，进行插值
        if (targetLift >= x0 && targetLift <= x1) {
            double y0 = dragArray[i];
            double y1 = dragArray[i + 1];
            // 线性插值公式
            return y0 + (targetLift - x0) * (y1 - y0) / (x1 - x0);
        }
    }

    // 如果targetLift超出范围，则选择距离它最近的两个点进行插值
    if (targetLift < liftArray[0]) {
        // 选择前两个点进行插值
        double x0 = liftArray[0];
        double x1 = liftArray[1];
        double y0 = dragArray[0];
        double y1 = dragArray[1];
        return y0 + (targetLift - x0) * (y1 - y0) / (x1 - x0);
    } else if (targetLift > liftArray[n - 1]) {
        // 选择最后两个点进行插值
        double x0 = liftArray[n - 2];
        double x1 = liftArray[n - 1];
        double y0 = dragArray[n - 2];
        double y1 = dragArray[n - 1];

        return y0 + (targetLift - x0) * (y1 - y0) / (x1 - x0);
    }

    // 理论上不会到这里，但如果到达这里，说明数组为空或没有合适点
    throw std::invalid_argument("No valid interpolation range found");
}
QVector<double> myMath::thirdDerivative(const QVector<double>& x, const QVector<double>& y) {
    QVector<double> derivatives;
    int n = x.size();

    for (int i = 0; i < n - 3; ++i) {
        // 基于x坐标计算三阶导数近似值
        double h1 = x[i+1] - x[i];
        double h2 = x[i+2] - x[i+1];
        double h3 = x[i+3] - x[i+2];

        // 三阶差分计算(考虑不等距x坐标)
        double d1 = (y[i+1] - y[i]) / h1;
        double d2 = (y[i+2] - y[i+1]) / h2;
        double d3 = (y[i+3] - y[i+2]) / h3;

        double dd1 = (d2 - d1) / ((x[i+2] - x[i]) / 2);
        double dd2 = (d3 - d2) / ((x[i+3] - x[i+1]) / 2);

        double ddd = (dd2 - dd1) / ((x[i+3] - x[i]) / 3);

        derivatives.append(ddd);
    }

    return derivatives;
}

QVector<int> myMath::findOutliers(const QVector<double>& derivatives, double threshold) {
    QVector<int> outliers;
    if (derivatives.isEmpty()) return outliers;

    // 计算均值和标准差
    double mean = 0.0, std = 0.0;
    for (double d : derivatives) mean += d;
    mean /= derivatives.size();

    for (double d : derivatives) std += pow(d - mean, 2);
    std = sqrt(std / derivatives.size() + 1e-12); // 避免除零

    // 检测超过阈值的异常点
    for (int i = 0; i < derivatives.size(); ++i) {
        if (qAbs(derivatives[i] - mean) > threshold * std) {
            outliers.append(i + 2); // 映射到原始索引
        }
    }

    // 去重并排序
    std::sort(outliers.begin(), outliers.end());
    auto last = std::unique(outliers.begin(), outliers.end());
    outliers.erase(last, outliers.end());

    return outliers;
}

double myMath::splineInterp(const QVector<double>& x, const QVector<double>& y, double x0) {
    int n = x.size();
    if (n < 2) return y[0];

    // 找到x0所在区间
    int i = 0;
    while (i < n - 1 && x0 > x[i+1]) {
        i++;
    }

    // 处理边界情况
    if (i >= n - 1) return y.back();
    if (i < 0) return y.front();

    // 三次样条插值计算
    double h = x[i+1] - x[i];
    if (h < 1e-9) return (y[i] + y[i+1]) / 2.0;

    double t = x0 - x[i];
    double a = y[i];
    double b = (y[i+1] - y[i]) / h;

    // 估计二阶和三阶导数(确保平滑过渡)
    double c = 0.0, d = 0.0;
    if (n > 2) {
        // 计算相邻区间的斜率
        double prevSlope = 0.0, nextSlope = 0.0;

        if (i > 0) {
            prevSlope = (y[i] - y[i-1]) / (x[i] - x[i-1]);
        } else {
            prevSlope = 2 * b - (y[i+2] - y[i+1]) / (x[i+2] - x[i+1]);
        }

        if (i < n - 2) {
            nextSlope = (y[i+2] - y[i+1]) / (x[i+2] - x[i+1]);
        } else {
            nextSlope = 2 * b - (y[i] - y[i-1]) / (x[i] - x[i-1]);
        }

        // 计算二阶和三阶系数
        c = (nextSlope - prevSlope) / (2 * (x[i+1] - x[i-1]));
        d = (nextSlope + prevSlope - 2 * b) / (h * h);
    }

    return a + b * t + c * t * t + d * t * t * t;
}
