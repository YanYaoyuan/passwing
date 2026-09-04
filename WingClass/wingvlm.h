#ifndef WINGVLM_H
#define WINGVLM_H
#include <QVector3D>
#include <QObject>
#include "wingdefinition.h"
#include "PublicClass/structDefinition.h"
#include "PublicClass/mymath.h"

//2025/1/6增加弹性铰链功能                       setDihedral();

//2025/7/8增加初始化判断，处理非对称或垂尾翼面      bool isSymmtery;
/*                              涡环模型
  o(A) * * * * * * * * * * * * o(Control) * * * * * * * * * * * * o(B)
  *                                                               *
  *                                                               *
  *                            o(Center)                          *
  *                                                               *
  *                                                               *
  *                                                               *
  *                                                               *
  *                                                               *
  o(E) * * * * * * * * * * * * * * * * * * * * * * * * * * * * *  o(F)
  *                                                               *
  *                                                               *
  o(D) * * * * * * * * * * * * o * * * * * * * * * * * * * * * *  o(C)
*/
struct vortex
{
    int id;
    point3d xyzA;
    point3d xyzB;
    point3d xyzC;
    point3d xyzD;
    point3d xyzE;
    point3d xyzF;

    point3d xyzControl;//控制点
    point3d xyzCenter;//

    point3d nControl;
    point3d nCenter;

    point3d lengthVortex;//附着涡长度向量

    double area;




    vortex() {}
};
struct airfoilInterInput
{
    QString nameA;
    QString nameB;
    int indexA;
    int indexB;
    double ratioA;
    double ratioB;
    double re;
    double designCl;

};

class wingVLM : public QObject
{
    Q_OBJECT
public:
    wingVLM();

    QVector<vortex>vortexArray;//机翼涡环模型
    QVector<vortex>wakeVortexArray;//机翼尾涡模型
    QVector<vortex>trefftzArray;

    //input


    double vinf;
    double density;
    double height;
    double designLiftForce;
    double designCl = 0.5;
    double designAlpha;
    double viscousCd;
    double cgX = 0.0;
    double ctYte = 0;//翼根尾缘厚度
    bool yteType = true;
    QVector<int>gridU;
    QVector<int>gridV;
    QVector<double>spanW;   //    半展长分布
    QVector<double>chordLengthW;
    QVector<double>twistAngleW;
    QVector<double>dihedralAngleW;
    QVector<double>offsetLengthW;
    int modefineIndex = 0;//弹性铰链修改位置


    int cstNum = 12;
    bool isShowStreamLine = false;
    bool isFreeWake = false;         // 来流是否平行尾迹
    bool isSymmetry = true;         //确定翼面是否镜像

    QVector<QVector<double>>cst;
    int threadNum = 10;

    QVector<QVector<QVector<double>>>airfoilArray;
    QVector<QVector<airfoilData>>interDataArray;//气动插值数据

    bool airfoilInputType;
    void initialGeometry(const wingDefinition&);//初始化机翼模型


    void refreshParaments(const VLMSetting&);


    virtual bool checkGeometry();
    void solver();


    void solveStreamLine(const int,const double,const double,const double,const double);//迎角 高度 距离x 时间  时间步

    void computeFixLiftDrag();

    void setZeroLiftDrag(bool);
    void setXfoilDrag(bool);
    void setUseLibraries(bool);

    //弹性铰链
    void setDihedral(double);//设置上反角
    double getChoiceTorque(int);//计算指定位置指定迎角弯矩
    //强制平移、旋转
    void setOffsetValue(const double x,const double y,const double z,const double a){
        xOffset = x;
        yOffset = y;
        zOffset = z;
        yTwistAngle = a;
    };

    //friend airplaneVLM CPP
    void computeGamma(const QVector<QVector<double>>&);
    void computeForce(const QVector<QVector<point3d>>&);
    void computePressure(const QVector<double>&,const QVector<double>&);
    void solveSpanForce(const QVector<QVector<double>>&,double);
    void computeAerodynamicCoefficient(double,double);


    void computeMoments(const point3d);

    void zeros();


    double getMac(){return mac;};
    double getRealArea(){return realArea;};










    //3dMesh

    void generate3DMesh();
    void generate3DModel();
    QVector<point3d>meshA;
    QVector<point3d>meshB;
    QVector<point3d>meshC;
    QVector<point3d>meshD;
    //xfoil drag


    //gemoteryDisplay

    QVector<point3d>xyA; QVector<point3d>xyB; QVector<point3d>xyC; QVector<point3d>xyD;
    QVector<double>contourArray;
    //QVector<point3d>xyTA; QVector<point3d>xyTB;
    QVector<point3d>xyTC; QVector<point3d>xyTD;
    QVector<QVector<point3d>>model;

    //result

    int getMeshNum()const {return  meshNum;}
    int getNyNum()const {return Ny;}

    QVector<double>yt; 
    QVector<double>wingCL;
    QVector<double>wingCD;
    QVector<double>wingCM;
    QVector<double>wingCN;
    QVector<double>wingViscousDrag;
    QVector<double>spanForceYt;
    QVector<QVector<double>>spanLiftForce;
    QVector<double>designSpanLift;
    QVector<double>designSpanEllipseLift;
    QVector<QVector<double>>spanForce;
    QVector<QVector<double>>spanLiftCoefficient;
    QVector<QVector<double>>ideaSpanForce;
    QVector<QVector<double>>spanMonmentArray;
    QVector<QVector<point3d>>streamLineArray;

    QVector<double>alphaArray;
    QVector<double>betaArray;
    double fixedAlpha = 0;
    double fixedBeta = 0;


    QVector<point3d>vinfArray;

    QVector<QVector<double>>pressureColorArray;
    //净环量
    QVector<QVector<double>>realGammaArray;
    double fixCl;
    double fixCd;
    double fixCm;







signals:
    void emitProgressValue(const int);
    void emitResultValue(const std::vector<double>&);


    void progressUpdated(int value);
    void workFinished();

private:

    double xOffset;   //强制偏移
    double yOffset;
    double zOffset;
    double yTwistAngle;//强制扭转
    QVector<QVector<airfoilInterInput>>inTerInputArray;//气动插值参数


    QVector<QString>nameArray;


    bool isOptimization = false;    //确定是优化还是计算

    QVector<QVector<double>>xCur;   //网格段弦长百分比

    QVector<QVector<point3d>>xyvvUpper;
    QVector<QVector<point3d>>xyvvLower;
    QVector<QVector<point3d>>xyvv;
    //

    int meshNum;
    int num;
    int Nx;
    int Ny;





    QVector<double>cy;

    QVector<double>xAtaque;
    QVector<double>xSalida;

    QVector<int>indexArray;


    QVector<double>psiTan;


    QVector<double>cm;
    QVector<double>xp;
    QVector<double>xp2;
    QVector<double>yp;
    QVector<double>yp2;
    QVector<double>zp;
    QVector<double>zp2;

    QVector<double>xp3;
    QVector<double>xp4;
    QVector<double>xp5;
    QVector<double>xp6;
    QVector<double>yp3;
    QVector<double>yp4;
    QVector<double>yp5;
    QVector<double>yp6;
    QVector<double>zp3;
    QVector<double>zp4;
    QVector<double>zp5;
    QVector<double>zp6;



    QVector<QVector<QVector<double>>>vNArray;
    QVector<QVector<QVector<double>>>vinxArray;
    QVector<QVector<QVector<double>>>vinyArray;
    QVector<QVector<QVector<double>>>vinzArray;

    QVector<QVector<double>>vNTrefftzArray;











    QVector<double>wingCMForce;//俯仰力矩
    QVector<double>wingCLForce;//滚转力矩
    QVector<double>wingCNForce;//偏航力矩

    void addDrag();
    void computeDownwashAngle();

    /***************几何处理类**************/

    void computeVortex();
    void computeWakeVortex();
    void computeTrefftzVortex();

    void clearAllValue();

    void computeGeometry2DMesh();//计算模型平面网格
    void generateZCoordinates();//生成模型Z坐标
    
    void computeArea();



    void computeVector();

    void twistWing();
    void dihedralWing();
    void dihedralHalfWing();//处理垂尾
    void twistHalfWing();//处理半模型垂尾
    void translationWing();//平移操作


    double computeZ(const int,const double,const double);//展位、无量纲位置、弦长
    double computeUpperZ(const int,const double,const double);//展位、无量纲位置、弦长
    double computeLowerZ(const int,const double,const double);//展位、无量纲位置、弦长
    /***************翼型处理***************/
    void computeMeanCamberAirfoil();
    void computeMeanCamberCst();
    void computeCst();//计算上下表面的cst参数
    /***************网格划分*****************/
    QVector<double> generateExponentialGrid(const int, const double, const double, const double);
    void interpolationAirfoil();
    void interpolate();
    /*****************计算类***************/


    void solveInfluenceCoefficient();
    void computeAerodynamicCoefficient();
    void computeWingFluenceCoefficient(QVector<QVector<double>>&,QVector<QVector<double>>&,QVector<QVector<double>>&,QVector<QVector<double>>&);//翼面法向贡献控制点和中心点的影响系数
    void computeWakeFluenceCoefficient(QVector<QVector<double>>&,QVector<QVector<double>>&,QVector<QVector<double>>&,QVector<QVector<double>>&);//尾迹法向贡献控制点和中心点的影响系数
    void computeTrefftzFluenceCoefficient(QVector<QVector<double>>&);//尾迹对远场中心点的影响系数
    //void solveHalfInfluenceCoefficient();
    void computeVLMMatrix();//PP

    void computeTreffzDrag();


    void computeVelocity();
    void computeGamma();


    void startXfoilInThread(const int,const int);//线程
    void startXfoil();//

    /***************弹性铰链***************/


    /**************后处理类*****************/
    //void computeForce();
    void computeCm();

    void computePressure();
    void computeInDesignAlpha(double);
    void computePressureIndex();
    void smoothDrag();
    void smoothOnceDrag();
    QVector<double> calculateBendingMoment(const QVector<double>&, const QVector<double>&);

    void computeXfoilInput();



private:





    myMath mathSolver;
    bool isAddZeroLiftDrag = false;
    bool isAddXfoilDrag = false;
    bool isUseLibraries = false;
    bool isLateral = false;

    int N;

    int meshRatioX;
    double meshRatioY;
    double realArea = 1;
    bool airfoilIsEmpty = true;
    double mac;
    int streamIndex = 0;
    //point3d positionYVelocity;
    //xfoil粘性插值

    QVector<double>dihedCos;//上反角的余弦值
    QVector<double>spanArea;//每段处的面积
    QVector<double>spanDragArray;//每段处的阻力--所有迎角下
    QVector<double>spanLiftArray;
    QVector<xfoilSetting>spanInputArray;//每段处的设置--所有迎角下
    QVector<QVector<double>>spanCstAllArray;//每段处的cst--所有迎角下
    QVector<double>spanDrag;//设计点处的迎角
    QVector<double>spanLift;//设计点处的迎角b
    QVector<double>upperYte;
    QVector<double>lowerYte;
    bool isInterXfoil = false;









    QVector<double>Xn;QVector<double>Yn;QVector<double>Zn;








    QVector<QVector<double>>areas;
    QVector<QVector<double>>hs;

    // QVector<QVector<double>>Gamma;
    // QVector<QVector<double>>GammaTotal;
    // QVector<QVector<double>>GammaTotal2;
    QVector<QVector<double>>realCst;
    QVector<QVector<double>>airfoilCSTArray;
    QVector<QVector<double>>spanCstArray;

    QVector<QVector<double>>upperCstArray;
    QVector<QVector<double>>lowerCstArray;


    /*************************************/





    //不同迎角下的涡强
    QVector<QVector<double>>matrixGammaArray;//飞机设计类需要使用

    //QVector<QVector<QVector<double>>>matrixGammaArrayA;

    //每个涡格处的诱导速度分量
    QVector<QVector<point3d>>streamVinfArray;
    //每个涡格处的气动力
    QVector<QVector<point3d>>allForceArray;
    //XYZ总气动力
    QVector<point3d>totalForceArray;
    QVector<QVector<double>>totalPressure;
    //沿展向总法向力
    QVector<QVector<double>>nSpanForceArray;







    void leyBiotSavart3D(const double,const double,const double,const QVector<double>&,const QVector<double>&,const QVector<double>&,
                         const QVector<double>&,const QVector<double>&,const QVector<double>&,QVector<double>&,QVector<double>&,QVector<double>&);
    point3d leyBiotSavart3D(const double,const double,const double,const double,const double,const double,const double,const double,const double);

    void computeNVelocity(const QVector<QVector<double>>&);


    double computeNormalInfluences(const point3d&,const point3d&);






    double sum(const QVector<QVector<double>>&);
    QVector<double>spanYt;





    double getViscosityDrag(int,int);

    /***************相关函数***************/

    point3d calculateVelocity(const point3d&);
    point3d rungeKutta4(const point3d&,const double);
    point3d eulerMethod(const point3d&,const double);
    /****************普通函数***************/
    int nchoosek(const int,const int);
    double max(const QVector<double>&);
    double min(const QVector<double>&);
    double interpolateQuadratic(double, double, double, double, double, double, double);
    bool hasEmptyRow();



    point3d rotatePoint3d(point3d,point3d,const double);
    point3d crossProduct(const point3d,const point3d);
    double dotProduct(const point3d,const point3d);
    point3d calculateVector(point3d, point3d);
    point3d calculateVectorB(point3d, point3d);
    point3d normalizeVector(point3d);
    double vectorMagnitude(const point3d&);
    double calculateQuadrilateralArea(const point3d&, const point3d&, const point3d&, const point3d&);

    point3d rotatePointAroundAxis(const point3d&, const point3d&, const point3d&, double);
    double calculateTriangleArea(const point3d&, const point3d&, const point3d&);

    int findStringIndexCaseInsensitive(const QString&);









};

#endif // WINGVLM_H
