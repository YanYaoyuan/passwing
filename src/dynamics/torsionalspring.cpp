#include "torsionalspring.h"

torsionalSpring::torsionalSpring()
{

}
torsionalSpring::torsionalSpring(double m0,double k)
{
    M0 = m0;
    K = k;
}
double torsionalSpring::getAngele(double torque){
    if(torque <= M0)
        return 0;
    return (torque - M0) / K;
}
