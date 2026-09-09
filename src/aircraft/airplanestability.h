#ifndef AIRPLANESTABILITY_H
#define AIRPLANESTABILITY_H
#include "aircraft/airplanevlm.h"
#include "common/structDefinition.h"
class airplaneStability
{
public:
    airplaneStability();
    void setStabilityValue(double v,double a){u0 = v;alpha0 = a;}
    //void solveStabilityDerivatives();
    // longitudinal stability derivatives
    double Xu, Xw, Zu, Zw, Xq, Zq, Mu, Mw, Mq;//first order
    double Zwp, Mwp;                          //second order derivatives, cannot be calculated by a panel method, set to zero.

    // latal stability derivatives
    double Yv, Yp, Yr, Lv, Lp, Lr, Nv, Np, Nr;//first order

    void initialRHS();


    QVector<point3d>perturbationArray;//小扰动矩阵



private:


    double u0;
    double v0;
    double w0;
    double alpha0;
    double m;
    //double inertiaMatrix[3][3];
    //扰动场




    //初始化速度场

private:
    void printf(point3d&);




};

#endif // AIRPLANESTABILITY_H
