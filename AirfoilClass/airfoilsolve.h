#ifndef AIRFOILSOLVE_H
#define AIRFOILSOLVE_H
#include "XFoil.h"
#include <QObject>
#include <QVector>
#include <QProgressBar>
#include <QMetaType>
#include "PublicClass/structDefinition.h"

class airfoilSolve:public QObject
{
    Q_OBJECT


private:
    // double Re,minAlpha,maxAlpha,alphaStepSize,Ma,xtrTop,xtrBot;
    // int nCrit,ReType,MaType;
    QVector<QVector<double>> airfoilData;
    //QProgressBar *progressBar;


    int m_Iterations = 0;
    //int s_IterLim = 200;
    bool m_bErrors = false;
    bool s_bAutoInitBL = true;
    int missionId;


public:
    xfoilSetting input;


    airfoilSolve();
    ~airfoilSolve();


    airfoilSolve(const xfoilSetting&);
    void setMissionId(int id){
        missionId = id;
    }
    

    bool iterate(XFoil* );
    bool importAirfoil(const QVector<QVector<double>>&);
    void refreshParaments(const xfoilSetting&);
    void refreshSimpleParaments(const xfoilSetting&,const int);


    std::stringstream ss;

    QVector<QVector<double>> resultData;
    QVector<QVector<double>> cpxResultData;
    QVector<QVector<QVector<double>>>allData;
    //边界层流场结构
    QVector<QVector<double>>xblDataUpper;//上弧面
    QVector<QVector<double>>xblDataLower;//下弧面

    QVector<QVector<double>>xWakeArray;
    QVector<QVector<double>>yWakeArray;






    //double onceData;

    fixClResult onceData;
    double cDData;
    //int id;

    int progressSignal;
    int solver();
    int solverOnce();
    void solverInThread();
public:
    //input
    QVector<double>clArray;
    //翼型阻力及边界层插值
    void computeInterDrag();
    QVector<double>cdArray;






public slots:

    QVector<QVector<QVector<double>>> getResult();
    fixClResult simpleResult();
    void emitResult();
signals:
    void progressUpdat(int value);
    void workFinished(int index);
  
    
    //void workDone(fixClResult res);

};

#endif // AIRFOILSOLVE_H
