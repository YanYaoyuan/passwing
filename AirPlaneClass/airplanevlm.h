#ifndef AIRPLANEVLM_H
#define AIRPLANEVLM_H

#include "PublicClass/structDefinition.h"
#include "WingClass/wingdefinition.h"
#include "WingClass/wingvlm.h"


class airplaneVLM: public QObject
{
    Q_OBJECT
public:
    airplaneVLM();
    void initialize();
    void setComponents(bool a,bool b,bool c){withWing = a;withHTail = b;withVTail = c;}


    void importWingGeometry(const wingDefinition&a){wingData = a;}
    void importHTailGeometry(const wingDefinition&b){hTailData = b;}
    void importVTailGeometry(const wingDefinition&c){vTailData = c;}


    void refreshParaments(const VLMSetting&);
    void setOffsetValue(double a[],double c[],double d[]);
    void setXfoilDrag(bool);
    void setUseLibraries(bool);
    void setZeroLiftDrag(bool);
    void setIsStability(bool);
    void solver();
    void solveStreamLine(const int,const double,const double,const double,const double);//迎角 高度 距离x 时间  时间步


    double viscousCd;

    double density;
    double vinf;
    double m;
    double inertiaMatrix[3][3];//惯性矩阵
    //仅仅因为需要展向数据
    wingVLM *wingModel;
    wingVLM *hTailModel;
    wingVLM *vTailModel;

    QVector<double>cMArray;
    QVector<double>cLArray;
    QVector<double>cNArray;

    QVector<double>clArray;
    QVector<double>cdArray;
    QVector<double>cnArray;
    QVector<double>alphaArray;
    QVector<QVector<point3d>>streamLineArray;
    bool isShowStreamLine = false;
//stability
    void setVinf(QVector<point3d>&);
signals:
    void emitProgressValue(const int);
    void progressUpdated(const int);
    void workFinished();
private:

    point3d cg{0.0,0.0,0.0};
    int streamIndex = 0;
    bool isOptimization = false;    //确定是优化还是计算
    bool isAddXfoilDrag = false;
    bool isAddZeroLiftDrag = false;
    bool isUseLibraries = false;
    bool isStability = false;

    void clearAllValue();

    void solveAirplaneInfluenceCoefficient();
    void computeAirplaneVLMMatrix();
    void computeTrefftzDrag();
    void computePressure();


    point3d eulerMethod(const point3d&,const double);
    point3d calculateVelocity(const point3d&);




    void solveAirplaneGamma();//
    void solveAirplaneForce();//
    void computeAerodynamicCoefficient();
    void updateStripForce();
    void solveViscousDrag();//计算粘性阻力

    void computeAirplaneVelocity();
    void computeAirplaneForce();


    void computeAirplaneCm();
    void computeAirplanePressure();

    void initialAirplaneVortex();
    void initialWakeVortex();
    void initialTrefftzVortex();



    void computeAirplaneFluenceCoefficient(QVector<QVector<double>>&,QVector<QVector<double>>&,QVector<QVector<double>>&,QVector<QVector<double>>&);
    void computeWakeFluenceCoefficient(QVector<QVector<double>>&,QVector<QVector<double>>&,QVector<QVector<double>>&,QVector<QVector<double>>&);
    void computeTrefftzFluenceCoefficient(QVector<QVector<double>>&);
    point3d leyBiotSavart3D(const double,const double,const double,const double,const double,const double,const double,const double,const double);




    wingVLM *airplaneModel;

    wingDefinition wingData;
    wingDefinition hTailData;
    wingDefinition vTailData;

    bool withWing;
    bool withHTail;
    bool withVTail;

    int wingN;//翼面网格数
    int wakeN;//尾迹网格数
    QVector<vortex>vortexArray;
    QVector<vortex>wakeVortexArray;
    QVector<vortex>trefftzArray;

    //不同迎角下的涡强
    QVector<QVector<double>>matrixGammaArray;
    QVector<QVector<double>>wakeGammaArray;
    //每个涡格处的诱导速度分量
    QVector<QVector<point3d>>streamVinfArray;

    myMath mathSolver;

    //input

    double height;

    double designLiftForce;
    QVector<point3d>vinfArray;


    QVector<QVector<QVector<double>>>vNArray;
    QVector<QVector<QVector<double>>>vinxArray;
    QVector<QVector<QVector<double>>>vinyArray;
    QVector<QVector<QVector<double>>>vinzArray;
    QVector<QVector<double>>vNTrefftzArray;

    QVector<QVector<double>>wingGammaArray;
    QVector<QVector<double>>hTailGammaArray;
    QVector<QVector<double>>vTailGammaArray;



    double computeNormalInfluences(const point3d&,const point3d&);
    double dotProduct(const point3d,const point3d);
    double referenceArea;
    //result
    //Test
    void solveZVelocity();






};

#endif // AIRPLANEVLM_H
