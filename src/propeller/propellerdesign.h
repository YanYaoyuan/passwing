#ifndef PROPELLERDESIGN_H
#define PROPELLERDESIGN_H
#include <QObject>
#include "common/mymath.h"
#include "common/structDefinition.h"

class propellerDesign : public QObject {

    Q_OBJECT



public:
    propellerDesign();

    void startAnalyse();
    void importAirfoil(const QVector<QVector<double>>&);
    //更新模型
    void computeCst();//参数翼型
    void updateGeometry();


    //主动光顺
    void smoothPropTwist(const int);
    void smoothPropXAtaque(const int);
    void smoothPropXSalida(const int);

    //input
    double propR = 0.7;
    double propRh = 0.06;
    double propHeight = 50;
    double propVinf = 25;
    double propRpm = 219.9;
    int propNum = 2;
    double propThrust = 370;


    int propNb = 10;
    int threadNum = 30;
    int propChordLen = 81;
    int xfoilStep = 100;
    int meshX = 20;
    int meshY = 5;
    int meshNum;
    int cstNum = 6;
    double ctYte = 0;//翼根尾缘厚度
    bool yteType = true;


    //update
    QVector<double>propChord;
    QVector<double>propTwist;
    QVector<double>xAtaque;
    QVector<double>xSalida;

    //output
    QVector<double>oldPropChord;
    QVector<double>oldPropTwist;
    QVector<double>oldXAtaque;
    QVector<double>oldXSalida;
    QVector<double>damiterArray;
    //model
    QVector<QVector<point3d>>model;

    QVector<point3d>xyAUpper;QVector<point3d>xyBUpper;QVector<point3d>xyCUpper;QVector<point3d>xyDUpper;
    QVector<point3d>xyALower;QVector<point3d>xyBLower;QVector<point3d>xyCLower;QVector<point3d>xyDLower;
private:
    //compute


    double propDensity = 1.225;
    double propViscosity = 0.0000176;
    double propMac = 340;
    QVector<double>chordArray;
    QVector<double>realChordArray;
    QVector<double>upperYte;
    QVector<double>lowerYte;

    double propFt;
    double k;
    QVector<double>kp;

    //几何模型

    QVector<QVector<point3d>>xyvvUpper;
    QVector<QVector<point3d>>xyvvLower;

    QVector<double>xAtaqueS;
    QVector<double>xSalidaS;

    QVector<double>upperCst;
    QVector<double>lowerCst;
    QVector<double>yt;
    QVector<double>cy;



    QVector<double>re;//无量纲弦长位置
    QVector<double>angle;
    QVector<double>alphaI;
    QVector<double>ve;
    //xfoil input
    //QVector<QVector<double>>rei;
    QVector<double>rei;
    QVector<double>mai;
    //
    QVector<double>maxCl;
    QVector<double>maxCd;


    //fixed
    double chordRatio[100];



    xfoilSetting airfoilSetting;
signals:
    void emitMessage(const QString);
    void emitProgressValue(const int);
private:
    //生成模型网格
    void initialGeometryMesh();
    //fun

    void initialSolver();//赋值
    void computeLagrange();//计算拉格朗日乘子
    void dealCoefficients();//处理系数
    void computeInput();//计算马赫数和雷诺数
    void initialXfoilSolve();//初始化xfoil设置
    void computeXfoilData();//计算xfoil
    void solveChordB();//计算弦长
    void computeEta();//计算效率

    //geometry
    void computeShape();//计算前缘和后缘

    double computeUpperZ(const int,const double,const double);
    double computeLowerZ(const int,const double,const double);
    void twistProp();
    void generateMesh();
    void generate3DModel();
    //compute k
    double computeKp(double, double, double, double, int);
    double computeIntegral(double, double, double, const QVector<double>&, const QVector<double>&);
    double findK(double, double, double, const QVector<double>&, const QVector<double>&, double);



    point3d rotatePointAroundAxis(const point3d&, const point3d&, const point3d&, double);
    point3d normalizeVector(point3d vec);
    point3d calculateVector(point3d, point3d);
    int nchoosek(const int,const int);


    myMath mathSolve;
    QVector<QVector<double>>resultArray;
    void startXfoil(const int,const int);
    double trapezoidalIntegration(const QVector<double>&, const QVector<double>&);
    QVector<double> getBestSoluation(const QVector<QVector<double>>&);


    QVector<QVector<double>>airfoil;


};

#endif // PROPELLERDESIGN_H
