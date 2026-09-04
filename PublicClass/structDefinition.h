#ifndef STRUCTDEFINITION_H
#define STRUCTDEFINITION_H
#include <QString>
#include <QVector>
struct AirfoilParameters{

    QString Name = "name";
    double MaxThickness = 0.3;
    double ThicknessLoaction = 0.3;
    double MaxCamber = 0.1;
    double CamberLoaction = 1;
    double minRadius = 0.1;
    double trailingAngle = 1;
    int id = 0;

};
struct xfoilSetting
{
    double Re,minAlpha,maxAlpha,alphaStepSize,Ma,xtrTop,xtrBot;
    int ReType = 1;
    int MaType = 1;
    int nCrit = 9;
    int s_IterLim = 150;
    int model = 0;
    double designCL;
    double designALPHA;


};
struct fixClResult
{
    double K;
    double cD;
    double cM;
    double cL;
    QVector<double>cpx;
};
struct propRPMData
{
    int RPM;
    QVector<QVector<double>>propData;

};
struct airfoilData{
    int Re;
    QVector<double>alphaData;
    QVector<double>clData;
    QVector<double>cdData;
    QVector<double>cmData;

};

struct point3d
{
    double x;
    double y;
    double z;
    // Default constructor
    point3d() : x(0.0), y(0.0), z(0.0) {}

    // Constructor with parameters
    point3d(double x1, double y1, double z1) : x(x1), y(y1), z(z1) {}

    // 加法运算符重载（成员函数版本）
    point3d operator+(const point3d& other) const {
        return point3d(x + other.x, y + other.y, z + other.z);
    }

        // 减法运算符重载（成员函数版本）
    point3d operator-(const point3d& other) const {
        return point3d(x - other.x, y - other.y, z - other.z);
    }

    // 左侧是 point3d，右侧是 double
    point3d operator*(double scalar) const {
        return point3d(x * scalar, y * scalar, z * scalar);
    }
    //
    point3d operator*(const point3d& other)const{
        return point3d(x * other.x,y * other.y,z * other.z);
    }

};

struct airplaneDefinition
{
    int compoentIndex[5]{0,0,0,0,0};
    //int wingIndex;
    //int hTailIndex;
    //int vTailIndex;
    //int tTailIndex;
    //int fuselageIndex;



    //int motorIndexA;
    //int motorIndexB;
    double xLocation[5]{0.0,0.0,0.0,0.0,0.0};
    double aAlpha[5]{0.0,0.0,0.0,0.0,0.0};
    double zLocation[5]{0.0,0.0,0.0,0.0,0.0};
    bool isChoice[5]{false,false,false,false,false};

    QString name;

    airplaneDefinition() {}
};





















#endif // STRUCTDEFINITION_H
