#ifndef PROPELLERBEMT_H
#define PROPELLERBEMT_H

#include <QtMath>
#include "wing/wingdefinition.h"
#include "common/mymath.h"
#include "common/structDefinition.h"
#include "common/myfile.h"
struct AeroCoeff {
    double Cl;
    double Cd;
};
struct ViternaParam {
    double k_cd      = 1.2;   // 最大阻力修正
    double k_cl1     = 1.0;   // sin(2α) 项
    double k_cl2     = 1.2;   // A 项
    double k_slope   = 0.9;   // 失速前升力斜率
    double delta_stall = 3.0 * M_PI / 180.0; // 过渡宽度
};
class propellerBemt
{
public:
    propellerBemt();

    void setVTip(bool);
    void initialAnalyse(const wingDefinition&,const propVLMSetting&);
    //void readInterDrag(QString);
    //void readInterDrag(QString,QString,double);
    void readInterDrag(QStringList);

    void solver();
    int propNum = 2;
    QVector<QVector<double>>resultArray;
    QVector<QVector<double>>dTArray;
    QVector<QVector<double>>dQArray;
    QVector<double>radiusArray;
private:
    myMath mathSolver;
    double a;
    double b;
    double height = 0;
    double density = 1.225;
    double temperature;
    double viscosity;
    double airfoilRatio = 1;
    double clRatio = 1.0;


    double R;
    int iterStep = 1500;
    bool isVTip = false;
    bool isModel = false;
    bool isMoreAirfoil = false;
    QVector<double>rpmArray;
    QVector<double>vinfArray;
    QVector<double>omegaArray;


    QVector<double>spanArray;//
    QVector<double>spanW;
    QVector<double>twistArray;
    QVector<double>chordArray;
    QVector<airfoilData>airfoilResultArrayA;
    QVector<airfoilData>airfoilResultArrayB;
    QVector<QVector<airfoilData>>airfoilResultArray;
    myFile *myAirfoilData;


    wingDefinition propData;
    void iterCompute(int,int,int,double*,double*,double&);
    void computeForce();

    int getReIndex(int);

    void test();
    AeroCoeff getAeroA(int,double,double);
    //AeroCoeff getCdA(int,double,double);
    AeroCoeff getAeroB(int,double,double);
    //AeroCoeff getCdB(int,double,double);

    AeroCoeff viternaModel(double,double,double,double,double,const ViternaParam& p = ViternaParam());


};

#endif // PROPELLERBEMT_H
