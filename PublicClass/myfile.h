#ifndef MYFILE_H
#define MYFILE_H
#include <QFile>
#include "WingClass/wingdefinition.h"
#include "PublicClass/structDefinition.h"

class myFile
{
public:
    myFile();
    void readFile();
    void writeFile();
    void readInterData(QString);
    static QVector<airfoilData>getInterData(QString);

    QVector<airfoilData>interData;

    void initialDataA(const QVector<wingDefinition>&,const QVector<QVector<QVector<double>>>&,const QVector<QString>&);
    void initialDataB(const QVector<wingDefinition>&,const QVector<wingDefinition>&,const QVector<airplaneDefinition>&);

    QVector<QVector<QVector<double>>>airfoilArray;
    QVector<QString>nameArray;
    QVector<wingDefinition>wingArray;

    QVector<wingDefinition>tailArray;
    QVector<wingDefinition>propellerArray;
    QVector<airplaneDefinition>airplaneArray;



private:

    void clearData();


};

#endif // MYFILE_H
