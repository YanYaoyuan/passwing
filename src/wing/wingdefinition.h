#ifndef WINGDEFINITION_H
#define WINGDEFINITION_H
#include <QVector>
#include <QPointF>
#include <common/structDefinition.h>


struct VLMSetting{
    double minAlpha;
    double maxAlpha;
    double stepAlpha;
    double vinf;
    double height;
    double referencePointX;
    VLMSetting() : minAlpha(0.0),maxAlpha(10.0),stepAlpha(1.0),vinf(20.0),height(0.0),referencePointX(0.0){}
    VLMSetting(double min,double max,double step,double v,double hei,double x):minAlpha(min),maxAlpha(max),stepAlpha(step),vinf(v),height(hei),referencePointX(x){}

};
struct propVLMSetting
{
    QVector<double>rpmArray;
    QVector<double>vinfArray;
    double height;
    int wakeStep;
    propVLMSetting(){}
    propVLMSetting(QVector<double> &a,QVector<double>&b,double c,double d):rpmArray(a),vinfArray(b),height(c),wakeStep(d){}
};
class wingDefinition
{
public:


    wingDefinition(QVector<double>&a,QVector<double>&b,QVector<double>&c,QVector<double>&d,QVector<double>&e
                   ,QVector<int>&f,QVector<int>&g){
        spanW = a;
        chordLengthW = b;
        offsetLengthW = c;
        twistAngleW = d;
        dihedralAngleW = e;
        gridV = f;
        gridU = g;

        //computeWingMessage();
    }
    wingDefinition() {
        // 默认构造函数的实现
        // 可以在这里初始化成员变量
    }


    //input
    bool airfoilInputType = true;
    QVector<double>spanLengthW;
    QVector<double>spanW;   //    半展长分布
    QVector<double>chordLengthW;
    QVector<double>twistAngleW;
    QVector<double>dihedralAngleW;   
    QVector<double>offsetLengthW;
    QVector<QVector<double>>airfoilMeanCamberCST;//各个截面翼型中弧线参数化
    QVector<QVector<QVector<double>>>airfoilArray;
    QVector<QVector<QVector<double>>>oldAirfoilArray;
    QVector<QVector<airfoilData>>interDataArray;
    QVector<QVector<double>>cstArray;
    QVector<QString>airfoilNameArray;//翼型名称
    QVector<QVector<double>>cstPointYArray;
    double ctYte = 0;//翼根尾缘厚度
    bool yteType = true;//尾缘生成方式
    bool isSymmetry = true;
    int cstNum = 6;
    QString name;
    double realChord;
    int num = 2;//桨叶数目


    //need



    QVector<int>gridU;
    QVector<int>gridV;

    int vMeshType;
    double uMeshType;

    double Area(){return area;}
    double AspectRatio(){return aspectRatio;}
    double RealChord(){return realChord;}
    double Span(){return span;}
    double RealArea(){return realArea;}
    double SweepAngle(){return sweepAngle;}
    double TipRatio(){return tipRatio;}
    int MeshNum(){return meshNum;}
    int TimeNum(){return timeNum;}


private:
    double span;
    double realArea;
    double aspectRatio;
    double area;

    double sweepAngle;
    double tipRatio;  //稍根比
    int meshNum;
    int timeNum;      //耗时
public:
    void computeWingMessage(){
        QVector<double>S;
        QVector<double>MAC;
        double AREA = 0;
        double MACS = 0;
        int GRID = 0;
        int len = spanW.length() - 1;
        for(int i = 0;i<len;i++){
            double cr = chordLengthW[len - i];
            double ct = chordLengthW[len - i - 1];
            double b = spanW[len - i] - spanW[len - i - 1];
            double s = b / 2.0 * (cr + ct);
            double mac = 2.0 / 3.0 * (cr * cr + cr * ct + ct * ct) / (cr + ct);
            AREA = AREA + s;
            S.append(s);
            MAC.append(mac);
            GRID = GRID + gridU[len - i];

        }

        for(int i = 0;i<len;i++){
            MACS = MACS + MAC[i] * S[i];
        }
        realChord = MACS / AREA;
        if(isSymmetry){
            area = AREA *2;
            span = spanW[len] * 2;
        }else{
            area = AREA;
            span = spanW[len];
        }

        tipRatio = chordLengthW[len] / chordLengthW[0];
        aspectRatio = span * span / area;
        meshNum = GRID * gridV[0] * 2;
    }

};

#endif // WINGDEFINITION_H
