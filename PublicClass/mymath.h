#ifndef MYMATH_H
#define MYMATH_H
#include <Eigen/Dense>
#include <unsupported/Eigen/Polynomials>
#include <QVector>
class myMath
{
public:
    myMath();
    // 获得给定转速和速度下推力的结果
    QVector<double>getInterpolateThrust(const QVector<QVector<double>>&,const QVector<double>&,const double);
    double maxV(const QVector<double>&);
    double minV(const QVector<double>&);
    int minIndex(const QVector<double>&,const double);
    int getMaxIndex(const QVector<double>&);
    int getMinIndex(const QVector<double>&);

    double findMaxValue(const QVector<QVector<double>>&);
    double findMinValue(const QVector<QVector<double>>&);
    //other fun
    double quadraticInterpolation(const QVector<double> &, const QVector<double> &, double);
    static double linearInterpolation(const QVector<double>&, const QVector<double>&, double);
    static double alphaFromClLinearFit(
        const QVector<double>& alpha,
        const QVector<double>& cl,
        double clTarget,
        double alphaMin,
        double alphaMax);
    //other fun
    double calculateAirDensity(double);
    double calculateTemperature(double);
    double calculateAirViscosity(double);
    double getSoundSpeed(double);
    //
    QVector<double> smoothCurve(const QVector<double>&,const QVector<double>&,const int);
    double interpolateWithLinear(const QVector<double>&, const QVector<double>&, double);//xfoil 阻力插值

    double threePointInterpolation(const QVector<double>&, const QVector<double>&, double);
    double quadraticInterpolation(double x0, double y0,
                                 double x1, double y1,
                                 double x2, double y2,
                                 double x);

    static QVector<double> smooth(const QVector<double>& x, const QVector<double>& y,
                                 double threshold = 2.0);
private:
    //GPT support
    // 对某一特定转速下的速度和推力进行插值
    QVector<double> interpolateThrustAtSpeed(const QVector<double>&, const QVector<double>&, const QVector<double>&);

    //
    double cubicSpline(const QVector<double>&, const QVector<double>&,double);

    //
    // 计算三阶导数(差分)
    static QVector<double> thirdDerivative(const QVector<double>& x, const QVector<double>& y);

    // 检测异常点位置
    static QVector<int> findOutliers(const QVector<double>& derivatives, double threshold);

    // 三次样条插值
    static double splineInterp(const QVector<double>& x, const QVector<double>& y, double x0);




};

#endif // MYMATH_H
