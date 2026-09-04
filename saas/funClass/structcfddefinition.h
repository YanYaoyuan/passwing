#ifndef STRUCTCFDDEFINITION_H
#define STRUCTCFDDEFINITION_H

#include <QString>
#include <QMap>
#include <QVariant>




// ========== 1. 项目设置 ==========
struct RawGridData {
    QString gridFormat;               // 网格文件格式
    QString dataFormat;               // 数据编码格式
    QString twodMode;                 // 二维模式
    QString swapYZAxes;               // 是否交换YZ轴
    QString fieldviewPrecision;       // FieldView坐标精度
    QString patchLumping;             // Patch合并模式
    QString ignoreEulerNumber;        // 是否忽略欧拉数检查

    // ✅ 构造函数：自动初始化默认值
    RawGridData()
        : gridFormat("\"aflr3\""),
          dataFormat("\"stream\""),
          twodMode(".false."),
          swapYZAxes(".false."),
          fieldviewPrecision("\"double\""),
          patchLumping("\"none\""),
          ignoreEulerNumber(".false.")
    {}

};

struct GridGenData{
    int nsrf;
    int wake;
    int jmax;
    double lesp;
    double tesp;
    double radi;
    double re;
    double yPlus;
    int genType;
    GridGenData()
        : nsrf(100),
          wake(100),
          jmax(100),
          lesp(0.001),
          tesp(0.00002),
          radi(20),
          re(1000000),
          yPlus(1),
          genType(0)
    {}
};



struct ForceMomentData {
    double areaReference;
    double xMomentLength;
    double yMomentLength;
    double xMomentCenter;
    double yMomentCenter;
    double zMomentCenter;

    // 构造函数（带默认初始化值）
    ForceMomentData()
        : areaReference(1.0),
          xMomentLength(1.0),
          yMomentLength(1.0),
          xMomentCenter(0.0),
          yMomentCenter(0.0),
          zMomentCenter(0.0)
    {}
};

struct EqnData {
    QString eqnType;
    double artificialCompress;
    QString viscousTerms;
    QString chemicalKinetics;
    QString thermalEnergyModel;
    double prandtlNumberMolecular;
    double schmidtNumber;
    QString gasRadiation;
    QString radUseImplLines;
    QString multiComponentDiff;
    //double cpivMinFactor;
    //QString augmentKineticsLimiting;
    //QString implicitRateLimiting;

    // 构造函数初始化
    EqnData()
        : eqnType("\"compressible\""),
          artificialCompress(15.0),
          viscousTerms("\"turbulent\""),
          chemicalKinetics("\"finite-rate\""),
          thermalEnergyModel("\"non-equilib\""),
          prandtlNumberMolecular(0.72),
          schmidtNumber(-1.0),
          gasRadiation("\"off\""),
          radUseImplLines(".false."),
          multiComponentDiff(".false.")
          //cpivMinFactor(0.0001),
          //augmentKineticsLimiting(".false."),
          //implicitRateLimiting(".true.")
    {}

};

struct ReferenceData {
    QString dimType;      // "nondimensional" / "dimensional-SI"
    double gridlengthConversion;
    double mach;
    double reynolds;
    double velocity;
    double density;
    double temperature;
    QString tempUnit;
    double alpha;
    double yaw;

    // 构造函数初始化默认值
    ReferenceData()
        : dimType("\"nondimensional\""),
          gridlengthConversion(1.0),
          mach(0.3),
          reynolds(6050300.0),
          velocity(0.0),
          density(0.0),
          temperature(500),
          tempUnit("\"Kelvin\""),
          alpha(0.0),
          yaw(0.0)
    {}

};


struct InviscidFluxData {
    QString fluxConstruction;       // 'roe', 'vanleer', ...
    QString fluxConstructionLHS;    // 'vanleer', 'consistent', ...
    double kappaUmuscl;             // -1.0
    QString fluxLimiter;            // 'none', 'barth', 'venkat', ...
    int firstOrderIterations;       // 0
    int multidmOption;              // 1 or 2
    QString fixedDirection;         // .true. / .false.
    int recalcDirFreq;              // 1
    QString adaptiveEntropyFix;     // .false.
    double rhsUEigenCoef;           // 0.0
    double lhsUEigenCoef;           // 0.0
    double rhsAEigenCoef;           // 0.0
    double lhsAEigenCoef;           // 0.0
    QString entropyFix;             // .false.
    double reMinVswch;              // 50.0
    double reMaxVswch;              // 500.0

    // ✅ 构造函数初始化默认值
    InviscidFluxData()
        : fluxConstruction("\"roe\""),
          fluxConstructionLHS("\"vanleer\""),
          kappaUmuscl(-1.0),
          fluxLimiter("\"none\""),
          firstOrderIterations(0),
          multidmOption(1),
          fixedDirection(".true."),
          recalcDirFreq(1),
          adaptiveEntropyFix(".false."),
          rhsUEigenCoef(0.0),
          lhsUEigenCoef(0.0),
          rhsAEigenCoef(0.0),
          lhsAEigenCoef(0.0),
          entropyFix(".false."),
          reMinVswch(50.0),
          reMaxVswch(500.0)
    {}

};

struct TurbulentDiffusionModelsData {
    QString turbulenceModel;      // e.g. 'sa' / 'sst' / ...
    double turbIntensity;         // 0.001
    double turbMuRatio;           // 0.001
    QString turbCompress;         // 'off'
    QString turbCond;             // 'off'
    double pr;                    // 0.9
    double sc;                    // 1.0

    TurbulentDiffusionModelsData()
        : turbulenceModel("\"sa\""),
          turbIntensity(0.001),
          turbMuRatio(1),
          turbCompress("\"off\""),
          turbCond("\"off\""),
          pr(0.9),
          sc(1.0)
    {}

};


struct SpalartData {
    double turbInf;
    QString daclesMariani;
    QString sarc;
    QString ddes;

    SpalartData()
        : turbInf(3.0),
          daclesMariani(".false."),
          sarc(".false."),
          ddes(".false.")
    {}
};


struct CodeRunData {
    int steps;
    double stoppingTolerance;
    double durationLimit;
    QString noRestart;
    int restartWriteFreq;
    QString restartRead;
    QString smartJupdate;
    int jacobianEvalFreq;
    int jupdateStartupSteps;
    QString dfduc3Jacobians;
    QString alphaSweep;
    int cycleIncrement;
    double alphaIncrement;
    double alphaMax;
    double alphaMin;
    int alphaSwitchbacks;

    CodeRunData()
        : steps(50),
          stoppingTolerance(1e-10),
          durationLimit(0),
          noRestart(".false."),
          restartWriteFreq(250),
          restartRead("\"off\""),
          smartJupdate(".true."),
          jacobianEvalFreq(0),
          jupdateStartupSteps(10),
          dfduc3Jacobians(".false."),
          alphaSweep(".false."),
          cycleIncrement(50),
          alphaIncrement(0.25),
          alphaMax(180.0),
          alphaMin(-180.0),
          alphaSwitchbacks(0)
    {}

};


struct NonlinearSolverData {
    QString timeAccuracy;
    double timeStepNondim;
    QString pseudoTimeStepping;
    int subiterations;
    QString temporalErrControl;
    double temporalErrFloor;
    int scheduleNumber;
    int scheduleIteration1;
    int scheduleIteration2;
    double scheduleCfl1;
    double scheduleCfl2;
    double scheduleCflturb1;
    double scheduleCflturb2;
    double fAllowMinimumM;
    double inviscRelaxFactor;
    double viscRelaxFactor;

    NonlinearSolverData()
        : timeAccuracy("\"steady\""),
          timeStepNondim(0.0),
          pseudoTimeStepping("\"\""),
          subiterations(0),
          temporalErrControl(".false."),
          temporalErrFloor(0.1),
          scheduleNumber(2),
          scheduleIteration1(1),
          scheduleIteration2(50),
          scheduleCfl1(50.0),
          scheduleCfl2(200.0),
          scheduleCflturb1(1.0),
          scheduleCflturb2(50.0),
          fAllowMinimumM(0.01),
          inviscRelaxFactor(1.0),
          viscRelaxFactor(1.0)
    {}

};


struct LinearSolverData {
    int meanflowSweeps;        // 主流方程松弛次数
    int turbulenceSweeps;      // 湍流方程松弛次数
    QString linearProjection;  // 是否启用Krylov投影
    QString lineImplicit;      // 线隐式选项 ("off" / "on")

    LinearSolverData()
        : meanflowSweeps(15),
          turbulenceSweeps(10),
          linearProjection(".false."),
          lineImplicit("\"off\"")
    {}

};

struct GlobalData
{
    // 移动网格选项
    QString movingGrid;
    QString gridMotionOnly;
    QString gridMotionAndDCI;
    QString bodyMotionOnly;

    // 计时选项
    QString timing;
    QString timeMovingGrid;

    // 输出频率
    int boundaryAnimFreq;
    int volumeAnimFreq;
    int sliceFreq;

    // 其他
    QString recordCmd;

    GlobalData()
        : movingGrid(".false."),
          gridMotionOnly(".false."),
          gridMotionAndDCI(".false."),
          bodyMotionOnly(".false."),
          timing(".false."),
          timeMovingGrid(".false."),
          boundaryAnimFreq(0),
          volumeAnimFreq(0),
          sliceFreq(0),
          recordCmd(".false.")
    {}
};

struct VolumeOutputData {
    QString exportTo;
    QString x, y, z;
    QString primitive;
    QString rho, u, v, w, p;
    QString entropy, mach, temperature;
    QString iblank, imesh;
    QString vortMag, vortX, vortY, vortZ;
    QString qCriterion, divVel;
    QString turbFluct;
    QString uuPrime, vvPrime, wwPrime;

    VolumeOutputData()
        : exportTo("tecplot"),
          x(".true."), y(".true."), z(".true."),
          primitive(".true."),
          rho(".false."), u(".false."), v(".false."), w(".false."), p(".false."),
          entropy(".false."), mach(".false."), temperature(".false."),
          iblank(".false."), imesh(".false."),
          vortMag(".false."), vortX(".false."), vortY(".false."), vortZ(".false."),
          qCriterion(".false."), divVel(".false."),
          turbFluct(".false."),
          uuPrime(".false."), vvPrime(".false."), wwPrime(".false.")
    {}

};

struct BoundaryOutputData {
    int numberOfBoundaries;
    QString boundaryList;
    QString exportTo;

    // 坐标
    QString x, y, z;

    // 基础变量
    QString primitive;
    QString rho, u, v, w, p;
    QString entropy, mach, temperature;
    QString iblank, imesh;

    // 涡量/散度
    QString vortMag, vortX, vortY, vortZ;
    QString qCriterion, divVel;

    // 湍流相关
    QString turbFluct;
    QString uuPrime, vvPrime, wwPrime;
    QString uvPrime, uwPrime, vwPrime;
    QString cp;

    BoundaryOutputData()
        : numberOfBoundaries(0),
          boundaryList(""),
          exportTo("tecplot_plt"),
          x(".true."), y(".true."), z(".true."),
          primitive(".true."),
          rho(".false."), u(".false."), v(".false."), w(".false."), p(".false."),
          entropy(".false."), mach(".false."), temperature(".false."),
          iblank(".false."), imesh(".false."),
          vortMag(".false."), vortX(".false."), vortY(".false."), vortZ(".false."),
          qCriterion(".false."), divVel(".false."),
          turbFluct(".false."),
          uuPrime(".false."), vvPrime(".false."), wwPrime(".false."),
          uvPrime(".false."), uwPrime(".false."), vwPrime(".false."),
          cp(".false.")
    {}
};
struct cloudPlotData{
    int index;
    double minValue;
    double maxValue;
    int nums;
    int colorTypeIndex;
    QVector<int>choiceArray;

    cloudPlotData()
        :index(0)
    {}
};


class structCFDDefinition
{
public:


    structCFDDefinition();
    RawGridData rawGridData;
    GridGenData gridGenData;
    ForceMomentData forceMomentData;
    EqnData eqnData;
    ReferenceData referenceData;
    InviscidFluxData inviscidFluxData;
    TurbulentDiffusionModelsData turbulentDiffusionModelsData;
    SpalartData spalartData;
    CodeRunData codeRunData;
    NonlinearSolverData nonlinearSolverData;
    LinearSolverData linearSolverData;
    GlobalData globalData;
    VolumeOutputData volumeOutputData;
    BoundaryOutputData boundaryOutputData;
    QStringList getAllText();

private:
    QStringList getRawGridText();
    QStringList getForceMomentIntegPropertiesText();
    QStringList getGoverningEquationsText();
    QStringList getReferencePhtsicalPropertiesText();
    QStringList getInviscidFluxMethodText();
    QStringList getTurbulentDiffsionModelsText();
    QStringList getSpalartText();
    QStringList getCodeRunControlText();
    QStringList getNonlinearSolverParametersText();
    QStringList getLinearSolverParametersText();
    QStringList getGlobalText();
    QStringList getVolumeOutputText();
    QStringList getBoundaryOutputText();



    static QString formatQuoted(const QString &s);
    inline void appendIfValid(QStringList &list, const QString &label, const QVariant &value);




};

#endif // STRUCTDEFINITION_H
