#ifndef PROPELLERVLM_H
#define PROPELLERVLM_H

#include <QVector>
#include <QObject>
#include "airfoil/airfoilsolve.h"
#include "common/mymath.h"
#include "common/structDefinition.h"
#include "wing/wingdefinition.h"
class propellerVLM : public QObject
{
    Q_OBJECT
public:
    propellerVLM();
    bool isAnalyse = false;
    bool isShowStreamLine = false;
    void initialModel(const wingDefinition&);
    void initialAnalyse(const wingDefinition&,const propVLMSetting&);
    void solver();
    void computeStreamLine(const int,const double,const double,const double,const double);//迎角 高度 距离x 时间  时间步
    QVector<QVector<point3d>>streamLine;
    QVector<QVector<double>>pressureColorArray;


    QVector<point3d>gridA;
    QVector<point3d>gridB;
    QVector<point3d>gridC;
    QVector<point3d>gridD;

    QVector<point3d>meshA;
    QVector<point3d>meshB;
    QVector<point3d>meshC;
    QVector<point3d>meshD;

    QVector<point3d>xyA;
    QVector<point3d>xyB;
    QVector<point3d>xyC;
    QVector<point3d>xyD;

    QVector<QVector<point3d>>model;
    QVector<double>chordLengthW;
    void generate3DModel();



    QVector<QVector<double>>resultArray;
    bool isAddXfoilDrag = true;
    int meshNum;
    int propNum = 2;//桨叶数目
    //bool checkGeometry()override;
signals:
    void emitProgressValue(const int);
    //void emitResultValue(const std::vector<double>&);
    void emitAnalyseLog(const QString);


    void progressUpdated(int value);
    void workFinished();

private:
    double thate = 0;
    int threadNum = 10;
    int minThreadNum = 5;
    double vinf = -5;//来流速度
    int streamIndex = 0;
    QVector<double>vArray;//来流速度矩阵
    QVector<double>rpmArray;//转速矩阵
    QVector<double>omegaArray;//角速度矩阵
    QVector<QVector<double>>MVinfArray;
    double rpm = 10000; //转速

    double height;
    double density;
    double mac;
    int wakeStep = 101;

    //
    double ctYte = 0;//翼根尾缘厚度
    bool yteType = true;
    bool airfoilIsEmpty;
    QVector<int>gridU;
    QVector<int>gridV;
    QVector<double>spanW;   //    半展长分布

    QVector<double>twistAngleW;
    QVector<double>dihedralAngleW;
    QVector<double>offsetLengthW;
    QVector<QVector<QVector<double>>>airfoilArray;
    QVector<QVector<double>>cst;
    QVector<QVector<double>>realCst;
    QVector<QVector<double>>airfoilCSTArray;
    QVector<QVector<double>>spanCstArray;
    QVector<double>upperYte;
    QVector<double>lowerYte;

    int cstNum = 12;
    //

    int num;
    int N;


    QVector<double>xAtaque;
    QVector<double>xSalida;

    double meshRatioY;
    double meshRatioX;
    QVector<double>yt;
    QVector<double>spanYt;
    QVector<double>cy;
    QVector<int>index;
    QVector<double>psiTan;
    QVector<double>dihedCos;
    QVector<double>cv;
    QVector<double>spanArea;

    bool airfoilInputType;

    int Ny,Nx;
    QVector<QVector<point3d>>xyvv;




    QVector<QVector<point3d>>xyvvUpper;
    QVector<QVector<point3d>>xyvvLower;

    QVector<point3d>xyTA;
    QVector<point3d>xyTB;
    QVector<point3d>xyTC;
    QVector<point3d>xyTD;






    QVector<point3d>vPQ;






    QVector<double>xp;
    QVector<double>yp;
    QVector<double>zp;
    QVector<double>xp2;
    QVector<double>yp2;
    QVector<double>zp2;
    QVector<double>xp3;
    QVector<double>yp3;
    QVector<double>zp3;
    QVector<double>xp4;
    QVector<double>yp4;
    QVector<double>zp4;
    QVector<double>xp5;
    QVector<double>yp5;
    QVector<double>zp5;
    QVector<double>xp6;
    QVector<double>yp6;
    QVector<double>zp6;
    QVector<double>XAtotal; QVector<QVector<double>>XAtotalArray;
    QVector<double>YAtotal; QVector<QVector<double>>YAtotalArray;
    QVector<double>ZAtotal; QVector<QVector<double>>ZAtotalArray;
    QVector<double>XBtotal; QVector<QVector<double>>XBtotalArray;
    QVector<double>YBtotal; QVector<QVector<double>>YBtotalArray;
    QVector<double>ZBtotal; QVector<QVector<double>>ZBtotalArray;
    QVector<double>XCtotal; QVector<QVector<double>>XCtotalArray;
    QVector<double>YCtotal; QVector<QVector<double>>YCtotalArray;
    QVector<double>ZCtotal; QVector<QVector<double>>ZCtotalArray;
    QVector<double>XDtotal; QVector<QVector<double>>XDtotalArray;
    QVector<double>YDtotal; QVector<QVector<double>>YDtotalArray;
    QVector<double>ZDtotal; QVector<QVector<double>>ZDtotalArray;
    QVector<double>XEtotal; QVector<QVector<double>>XEtotalArray;
    QVector<double>YEtotal; QVector<QVector<double>>YEtotalArray;
    QVector<double>ZEtotal; QVector<QVector<double>>ZEtotalArray;
    QVector<double>XFtotal; QVector<QVector<double>>XFtotalArray;
    QVector<double>YFtotal; QVector<QVector<double>>YFtotalArray;
    QVector<double>ZFtotal; QVector<QVector<double>>ZFtotalArray;


    QVector<double>xFptotal;
    QVector<double>yFptotal;
    QVector<double>zFptotal;
    QVector<QVector<double>>Xptotal;
    QVector<QVector<double>>Yptotal;
    QVector<QVector<double>>Zptotal;
    QVector<QVector<double>>Cyp;
    QVector<QVector<double>>xCurvatura;
    QVector<double>xptotal;
    QVector<double>yptotal;
    QVector<double>zptotal;
    QVector<double>nx;
    QVector<double>ny;
    QVector<double>nz;
    QVector<double>Xn;
    QVector<double>Yn;
    QVector<double>Zn;

    QVector<double>nx2;
    QVector<double>ny2;
    QVector<double>nz2;
    QVector<double>Zn1;
    QVector<double>Yn1;
    QVector<double>Xn1;
    QVector<double>Zn2;
    QVector<double>Yn2;
    QVector<double>Xn2;
    QVector<double>cmedia;
    QVector<double>cyp;

    QVector<QVector<double>>vinx;   QVector<QVector<QVector<double>>>vinxArray;
    QVector<QVector<double>>viny;   QVector<QVector<QVector<double>>>vinyArray;
    QVector<QVector<double>>vinz;   QVector<QVector<QVector<double>>>vinzArray;
    QVector<QVector<double>>vTotal; QVector<QVector<QVector<double>>>vTotalArray;

    QVector<QVector<point3d>>vinfArray;//合速度

    //不同迎角下的涡强
    QVector<QVector<double>>matrixGammaArray;
    //净环量
    QVector<QVector<double>>realGammaArray;
    //每个涡格处的诱导速度分量
    QVector<QVector<point3d>>streamVinfArray;
    //每个涡格处的气动力
    QVector<QVector<point3d>>allForceArray;

    QVector<double>VX;
    QVector<double>VY;
    QVector<double>VZ;
    QVector<double>VM;
    QVector<double>VN;





    QVector<double>cm;
    QVector<QVector<double>>xCur;
    QVector<double>spanForceYt;

    QVector<xfoilSetting>spanInputArray;
    QVector<QVector<double>>spanCstAllArray;

    QVector<double>spanDragArray;//每段处的阻力--所有迎角下
    QVector<double>spanLiftArray;

    QVector<QVector<double>>spanLiftCoefficient;

    //QVector<double>wingCL;
    //QVector<double>wingCD;
    QVector<double>thrustArray;
    QVector<double>torqueArray;
    QVector<QVector<double>>includeForceArray;

    QVector<int>badDragIndex; //坏阻力对应的位置

    QVector<xfoilSetting>badSettingArray;
    QVector<QVector<double>>badCSTArray;

    QVector<QVector<double>>badCdArray;
    QVector<QVector<double>>badClArray;
    QVector<QVector<double>>badAlphaArray;

    QVector<double>reArray;







private:
    void testProp();
    void initialGeometry(const wingDefinition&);

    void initialVinfArray();
    void computeGeometryMesh();//生成模型网格
    void generateSimpleGeometry();//仅需桨距角和弦长生成螺旋桨模型
    void generateGeometry();//根据几何外形生成螺旋桨模型
    void initial3DMesh();
    void twistWing();
    void generate3DMesh();

    void generateVLMMesh();
    void generateWakeMesh();//生成尾迹区网格
    void mergeMesh();//合并网格
    void rotationalCopyValue();//旋转复制物理量

    void computeVector();//向量计算
    void solveInfluenceCoefficientArray();//影响系数
    void computeVLMMatrixArray();//求解线性方程组
    void computeGamma();//求解净涡强
    void computeVelocity();//求解速度分布
    void computeForce();//计算气动力
    void addDrag();
    //后处理
    void dealResult();
    void computePressure();





    void solveInfluenceCoefficient(int);
    void computeVLMMatrix(int);

    void clearAllValue();
    void smoothDrag();
    void smoothDragAgain();









    void initialVelocity();

//fun
    /***************相关函数***************/

    point3d calculateVelocity(const point3d&,const int);
    point3d rungeKutta4(const point3d&,const double,const int);
    myMath mathSolver;
    void leyBiotSavart3D(const double,const double,const double,const QVector<double>&,const QVector<double>&,const QVector<double>&,
                         const QVector<double>&,const QVector<double>&,const QVector<double>&,QVector<double>&,QVector<double>&,QVector<double>&);
    //void leyBiotSavart3D(const double,const double,const double,const QVector<double>&,const QVector<double>&,const QVector<double>&,
                         //const QVector<double>&,const QVector<double>&,const QVector<double>&,QVector<double>&,QVector<double>&,QVector<double>&,double);

    void computeMeanCamberAirfoil();
    void computeMeanCamberCst();
    void interpolationAirfoil();
    double computeZ(const int, const double, const double);
    int nchoosek(const int,const int);
    double computeLowerZ(const int, const double, const double);
    double computeUpperZ(const int, const double, const double);

    void startXfoilInThreadA(const int,const int);
    void startXfoilInThreadB(const int,const int);
    QVector<double> calculateBendingMoment(const QVector<double>&, const QVector<double>&);
    double calculateTorque(const QVector<double>&, const QVector<double>&);


    point3d rotatePointAroundAxis(const point3d&, const point3d&, const point3d&, double);
    point3d calculateVector(point3d,point3d);
    point3d normalizeVector(point3d);
    point3d rotateCod(const point3d&,double);
    double computeNormalInfluences(const point3d&,const point3d&);
    double dotProduct(const point3d,const point3d);
    double max(const QVector<double>&);
    double min(const QVector<double>&);
};

#endif // PROPELLERVLM_H
