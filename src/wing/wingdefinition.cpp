#include "wingdefinition.h"
#include <cmath>
#include <QDebug>
#include <Eigen/Sparse>
#include <Eigen/Dense>
#define PI 3.1415926

wingDefinition::wingDefinition() {

}


wingDefinition::wingDefinition(QVector<double>&a,QVector<double>&b,QVector<double>&c,QVector<double>&d,QVector<double>&e) {
    spanW = a;
    chordLengthW = b;
    twistAngleW = c;
    dihedralAngleW = d;
    offsetLengthW = e;
}




