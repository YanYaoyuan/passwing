#ifndef AIRFOILDESIGN_H
#define AIRFOILDESIGN_H

#include <QVector>
#include <QTextStream>


class airfoilDesign
{

    //Q_OBJECT
 /*************************************************AreodynamicDesign******************************************************/
public:
    airfoilDesign(const int n) ;
    airfoilDesign();





    void buildBenrnstein(const QVector<QVector<double>> &);//计算cst参数

    void getIntersectionPosition(const QVector<double> &);

    void buildAirfoilCurve(const QVector<double>& );//构建翼型
    void buildAirfoilCurve(const QVector<double> &,const QVector<double> &);
    void deflectedAirfoil(const QVector<QVector<double>> &); //偏转舵面
    void computeOtherParameters(const QVector<QVector<double>>&);   //MAX  HEIGHT
    void computeSimpleParameters(const QVector<QVector<double>>&);
    void buildAirfoilMeanCamberBenrnstein(const QVector<QVector<double>> &);//计算中弧线

    void splitAirfoilData(const QVector<QVector<double>> &);//劈分翼型

    bool isFinshed(){return buildCSTSuccess;}

    void blendingAirfoil(const QVector<double>&,const double,const double);
    void computeParameters(const QVector<double>&);
    double getUpperY(double)const;
    double getLowerY(double)const;

private:

    int nchoosek(const int,const int);
    int maxQVector(const QVector<double>&);
    int minQVector(const QVector<double>&);
    double distance(const double,const double,const double,const double);
    double curvatureRadius(const double,const double,const double,const double,const double,const double);
    double computeTrailingAngle(const double,const double,const double,const double);


public:


    QVector<QVector<double>> upperData;          //分割后的原始翼型上表面
    QVector<QVector<double>> lowerData;         //分割后的原始翼型下表面
    QVector<double> newUpperY;                 //修型后上表面
    QVector<double> newLowerY;                 //修型后下表面
    QVector<QVector<double>> newAirfoilData;   //修型后的参数
    QVector<QVector<double>> flapAirfoilData;
    QVector<QVector<double>> upperMatrix;      //上表面系数矩阵
    QVector<QVector<double>> lowerMatrix;      //下表面系数矩阵
    QVector<double> cstParameter;              //存放cst参数
    QVector<double>Cl;
    QVector<double>Cd;
    QVector<double>Cm;
    QVector<double>Alpha;
    QVector<double>meanCstParameter;//中弧线cst参数

    int cstNum ;            //N阶参数
    double maxThickness,locationThickness;    //最大厚度
    double maxCamber,locationCamber;          //最大弯度
    double minRadius,trailingAngle;     //最小曲率

    double flapAngle = 0; //偏转角初始化
    double rotationAxis = 0.7;       //偏转轴
    double yTe1 = 0;             //厚度初始化
    double yTe2 = 0;


private:

    double intersectionX,intersectionY;
    bool buildCSTSuccess = false;

public slots:

 /*************************************************StructDesign******************************************************/


/*********************************************Hicks-Henne**************************************************************/
public:
    void buildHicksHenne(const QVector<double>&);
    void buildHicksHenne(const QVector<double>&,const double);
private:
    double dy1(const double,const double);
    double dy2(const double,const double,const double);
    double dy3(const double,const double);

};

#endif // AIRFOILDESIGN_H
