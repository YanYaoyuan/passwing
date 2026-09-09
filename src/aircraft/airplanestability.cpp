#include "airplanestability.h"
#include <QtMath>
#include <QDebug>
airplaneStability::airplaneStability()
{


}

void airplaneStability::initialRHS(){


    point3d Forcem, Momentm, Vim, Vjm, Vkm, Rism, Rjsm, Rksm;
    point3d Forcep, Momentp, Vip, Vjp, Vkp, Risp, Rjsp, Rksp;
    point3d V0, is, js, ks, CGM, WindDirection, WindNormal;
    QVector<point3d>vinfArray;//存储RHS

    double sina(0), cosa(0), deltaspeed(0), deltarotation(0);
    // Compute stability and control derivatives
    Xu = Xw = Zu = Zw = Mu = Mw = Mq = Zwp = Mwp = 0.0;
    Yv = Yp = Yr = Lv = Lp = Lr = Nv = Np  = Nr  = 0.0;
    deltaspeed    = 0.001;         //  m/s   for forward difference estimation
    deltarotation = 0.001;        //  rad/s for forward difference estimation
    point3d cgm(1.0,0,0);
    // Define the stability axes
    cosa = cos(alpha0*M_PI/180);
    sina = sin(alpha0*M_PI/180);
    WindDirection = point3d(cosa, 0.0, sina);
    WindNormal = point3d(-sina, 0.0, cosa);

    is = point3d(-cosa, 0.0, -sina);
    js = point3d(0.0, 1.0,   0.0);
    ks = point3d(sina, 0.0, -cosa);

    V0 = is * (-u0);
    // RHS for unit speed vectors
    // The change in wind velocity is opposite to the change in plane velocity
    Vim = V0 - is * deltaspeed; //a positive increase in axial speed is a positive increase in wind speed
    Vjm = V0 - js * deltaspeed; //a plane movement to the right is a wind flow to the left, i.e. negative y
    Vkm = V0 - ks * deltaspeed; //a plane movement downwards (Z_stability>0) is a positive increase of V in geometry axes
    Vip = V0 + is * deltaspeed; //a positive increase in axial speed is a positive increase in wind speed
    Vjp = V0 + js * deltaspeed; //a plane movement to the right is a wind flow to the left, i.e. negative y
    Vkp = V0 + ks * deltaspeed; //a plane movement downwards (Z_stability>0) is a positive increase of V in geometry axes

    //
    Risp = is * cgm * (+deltarotation) + V0;
    Rjsp = js * cgm * (+deltarotation) + V0;
    Rksp = ks * cgm * (+deltarotation) + V0;
    Rism = is * cgm * (-deltarotation) + V0;
    Rjsm = js * cgm * (-deltarotation) + V0;
    Rksm = ks * cgm * (-deltarotation) + V0;

    //0赋值
    vinfArray.append(Vim);
    vinfArray.append(Vjm);
    vinfArray.append(Vkm);
    vinfArray.append(Vip);
    vinfArray.append(Vjp);
    vinfArray.append(Vkp);
    vinfArray.append(Risp);
    vinfArray.append(Rjsp);
    vinfArray.append(Rksp);
    vinfArray.append(Rism);
    vinfArray.append(Rjsm);
    vinfArray.append(Rksm);
    //
    perturbationArray = vinfArray;
    qDebug()<<"aa";



    //2 计算Force

    //3 计算 Xu Zu Mu.....


    //vlmSolver->setVinf(Vim);
    printf(Vim);
    printf(Vjm);
    printf(Vkm);
    printf(Vip);
    printf(Vjp);
    printf(Vkp);

}
void airplaneStability::printf(point3d& a){
    qDebug()<<a.x<<" "<<a.y<<" "<<a.z;
}
