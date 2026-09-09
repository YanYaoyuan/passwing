#ifndef TORSIONALSPRING_H
#define TORSIONALSPRING_H
/*
  *扭簧动力学模型
  * M = M0 + K * Angle
*/

class torsionalSpring
{
public:
    torsionalSpring();
    torsionalSpring(double,double);//
    double getAngele(double);
private:
    double M0;
    double K;


};

#endif // TORSIONALSPRING_H
