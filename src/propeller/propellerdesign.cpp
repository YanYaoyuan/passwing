#include "propellerdesign.h"
#include <QtMath>
#include <QFuture>
#include <QtConcurrent/QtConcurrent>
#include "airfoil/airfoilsolve.h"
#include "airfoil/airfoildesign.h"

#include <algorithm>
#include <limits>

#if defined(_MSC_VER) && (_MSC_VER >= 1600)
# pragma execution_character_set("utf-8")
#endif

propellerDesign::propellerDesign() {




    //qDebug()<<damiterArray;

    // dealCoefficients();
    // computeInput();


}
void propellerDesign::importAirfoil(const QVector<QVector<double>>&Airfoil){
    airfoil = Airfoil;
}
void propellerDesign::initialSolver(){
    if(!chordArray.isEmpty()){
        chordArray.clear();
        realChordArray.clear();
        damiterArray.clear();
    }
    for(int i = 0;i<propChordLen;i++){
        chordArray.append(0.01 + i * 0.0025);
        realChordArray.append(chordArray[i] * propR);
    }
    double tmp2 = (propR - propRh) / (propNb - 1);
    for(int i = 0;i<propNb;i++){
        damiterArray.append(propRh + i * tmp2);
    }
    propDensity = mathSolve.calculateAirDensity(propHeight);
    double temperature = mathSolve.calculateTemperature(propHeight);
    propViscosity = mathSolve.calculateAirViscosity(temperature);
    propMac = mathSolve.getSoundSpeed(temperature);
    double tmp = propRpm * M_PI / 30;

    propRpm = tmp;
}
void propellerDesign::startAnalyse(){
    if(!airfoil.isEmpty()){
        initialSolver();
        emit emitMessage("设置初始化完成\n");

        initialXfoilSolve();
        emit emitMessage("Xfoil初始化完成\n");

        computeLagrange();
        emit emitMessage("拉格朗日乘子计算完成,结果为：" + QString::number(k,'f',4) + "\n");

        dealCoefficients();
        emit emitMessage("无量纲系数计算完成\n");

        computeInput();
        emit emitMessage("xfoil设置初始化完成\n");

        computeXfoilData();
        solveChordB();
        computeEta();
        emit emitProgressValue(100);

         //if(oldPropChord.isEmpty()){
             //oldPropChord<<0.06<<0.08<<0.12<<0.135<<0.14<<0.12<<0.08<<0.04<<0.02<<0.02<<0.02<<0.02<<0.02<<0.02<<0.02<<0.015<<0.015<<0.015<<0.015<<0.012;
             //oldPropTwist<<60<<55<<50<<45<<40<<35<<30<<26<<24<<22<<35<<30<<26<<24<<22<<35<<30<<26<<24<<22;

             //oldPropChord<<0.06<<0.08<<0.12<<0.135<<0.14<<0.12<<0.08<<0.04<<0.02<<0.02;
             //oldPropTwist<<60<<55<<50<<45<<40<<35<<30<<26<<24<<22;
         //}



        computeShape();
        computeCst();

        initialGeometryMesh();

        twistProp();

        generateMesh();

        generate3DModel();

    }
}
void propellerDesign::updateGeometry(){
    initialGeometryMesh();
    twistProp();
    generateMesh();
    generate3DModel();
}
void propellerDesign::initialXfoilSolve(){
    airfoilSetting.MaType = 1;
    airfoilSetting.ReType = 1;
    airfoilSetting.nCrit = 9;
    airfoilSetting.xtrTop = 1;
    airfoilSetting.xtrBot = 1;
    airfoilSetting.s_IterLim = xfoilStep;
    airfoilSetting.alphaStepSize = 1;
    airfoilSetting.minAlpha = 0;
    airfoilSetting.maxAlpha = 10;
    airfoilSetting.model = 0;
}

void propellerDesign::computeLagrange(){
    if(!kp.isEmpty()){
        kp.clear();
    }
    //QVector<double>tmp2;

    double tmp1 = sqrt(1 + pow((propRpm * propR / propVinf),2));

    for(int i = 0;i<propNb;i++){
        // 计算特普朗修正系数kp
        double exponent = -static_cast<double>(propNum) / 2.0
                          * (1 - damiterArray[i] / propR) * tmp1;
        kp.append(2 / M_PI * acos(exp(exponent)));
        //tmp2.append(propRpm * propRpm * kp[i] / (propVinf * propVinf + pow((propRpm * damiterArray[i]),2)) * pow(damiterArray[i],3));
    }
    //double tmp3 = trapezoidalIntegration(damiterArray,tmp2);
    //k = propThrust / 4 / M_PI / propDensity / propVinf / propVinf / tmp3;



    //for(int i = 0;i<propNb;i++){
        //kp.append(computeKp(damiterArray[i],propR,propRpm,propVinf,propNum));
    //}
    double sum2 = propThrust / (propDensity * 4.0 * M_PI * propVinf * propVinf);
    k = findK(sum2, propVinf, propRpm, damiterArray, kp,1e-7);





}
void propellerDesign::dealCoefficients(){
    //
    propFt = propVinf / propRpm / propR;
    if(!re.isEmpty()){
        re.clear();
        angle.clear();
        alphaI.clear();
        ve.clear();
    }
    for(int i = 0;i<propNb;i++){
        //计算实际来流相对旋转平面的夹角
        re.append(damiterArray[i] / propR);
        double angleValue = propFt / re[i] * (1 + k);
        if (std::isnan(angleValue) || std::isinf(angleValue)) {
            angleValue = 0; // 处理 NaN 和 Inf 的情况
        }
        angle.append(atan(angleValue));
        //计算诱导攻角
        double tmp1 = 1 + k * cos(angle[i]) * cos(angle[i]);
        double aValue = atan((k) * sin(angle[i]) * cos(angle[i]) / tmp1);
        if (std::isnan(aValue) || std::isinf(aValue)) {
            aValue = 0; // 处理 NaN 和 Inf 的情况
        }
        alphaI.append(aValue);
        //计算无量纲的实际速度
        double speedValue = 1 + pow((re[i] / propFt),2) * cos(alphaI[i]);
        if(speedValue < 0)
            speedValue = 0;
        ve.append(sqrt(speedValue));
    }
}
void propellerDesign::computeInput(){
    double reynold = propDensity * propR * propVinf / propViscosity;
    double mach = propVinf / propMac;
    if(!rei.isEmpty()){
        rei.clear();
        mai.clear();
    }
    for(int i = 0;i<propNb;i++){
        //QVector<double>tmp;
        for(int j = 0 ;j< chordArray.length();j++){
            rei.append(ve[i] * reynold * chordArray[j]);
        }
        //rei.append(tmp);
        mai.append(ve[i] * mach);
    }
}
void propellerDesign::computeXfoilData(){

    int threadSum = propNb * propChordLen;
    int step = threadSum / threadNum;
    int remainder = threadSum % threadNum;
    int progressValue;
    emit emitProgressValue(0);

    for(int i = 0;i<step;i++){
        int index = i * threadNum;
        startXfoil(index,threadNum);
        progressValue = static_cast<int>(static_cast<double>(i + 1) / step * 96.0);
        emit emitProgressValue(progressValue);
        emit emitMessage(tr("XFoil batch %1 of %2 completed\n").arg(i + 1).arg(step));
    }

    if(remainder > 0){
        int beginIndex = threadSum - remainder;
        startXfoil(beginIndex,remainder);
    }

}
void propellerDesign::solveChordB(){

    if(!propChord.isEmpty()){
        oldPropChord.clear();
        oldPropTwist.clear();
        maxCl.clear();
        maxCd.clear();
    }

    QVector<double>b;
    double tmp1 = 0;
    double tmp2 = 1;

    for(int i = 0;i<propNb;i++){
        for(int j = 0;j<propChordLen;j++){
            int G = i * propChordLen + j;
            tmp1 = 8 * M_PI / propNum * re[i] * kp[i] * tan(alphaI[i]) * sin(angle[i]);
            tmp2 = resultArray[G][1] - resultArray[G][2] * tan(angle[i]);
            b.append(tmp1 / tmp2);
        }
    }

    QVector<QVector<double>>error;
    for(int i = 0;i<propNb;i++){
        QVector<double>tmp;
        for(int j = 0;j<propChordLen;j++){
            int G = i * propChordLen + j;
            tmp.append(abs(chordArray[j] - b[G]));
        }
        error.append(tmp);
    }


    for(int i = 0;i<propNb;i++){
        int index = mathSolve.getMinIndex(error[i]);
        oldPropChord.append(realChordArray[index]);
        double tmp = resultArray[i * propChordLen + index][0];
        maxCl.append(resultArray[i * propChordLen + index][1]);
        maxCd.append(resultArray[i * propChordLen + index][2]);
        oldPropTwist.append(tmp + angle[i] * 180 / M_PI);
    }

}
void propellerDesign::computeEta(){
    QVector<double>tmp1;
    QVector<double>tmp2;
    double tmp = static_cast<double>(propNum) / 2.0 * propFt * propFt;
    for(int i = 0;i<re.length();i++){
        double cosa = cos(angle[i]);
        double sina = sin(angle[i]);
        tmp1.append(tmp * ve[i] * ve[i] * (maxCl[i] * cosa - maxCd[i] * sina)* (oldPropChord[i] / propR));
        tmp2.append(tmp * ve[i] * ve[i] * (maxCl[i] * sina + maxCd[i] * cosa) * (oldPropChord[i] / propR) * re[i]);
    }
    double to = trapezoidalIntegration(re,tmp1);
    double lo = trapezoidalIntegration(re,tmp2);
    double eta = to * propFt / lo;
    emit emitMessage("效率:" + QString::number(eta,'f',3));
}
void propellerDesign::computeShape(){
    if(!oldXAtaque.isEmpty()){
        //offsetLength.clear();
        oldXAtaque.clear();
        oldXSalida.clear();
    }
    // double tmp1 = propChord[0] * 0.25;
    // for(int i = 0;i<propNb - 1;i++){
    //     double tmp2 = tmp1 - propChord[i] * 0.25;
    //     offsetLength.append(tmp2);
    // }

    for(int i = 0;i<propNb;i++){
        double tmp = oldPropChord[i] * 0.25;
        oldXAtaque.append(tmp);
        oldXSalida.append(tmp + oldPropChord[i]);
    }

    propChord = oldPropChord;
    propTwist = oldPropTwist;
    xAtaque = oldXAtaque;
    xSalida = oldXSalida;
}
void propellerDesign::initialGeometryMesh(){


    if(!xyAUpper.isEmpty()){
        xyAUpper.clear();xyBUpper.clear();xyCUpper.clear();xyDUpper.clear();
        xyALower.clear();xyBLower.clear();xyCLower.clear();xyDLower.clear();
        xyvvUpper.clear();
        xyvvLower.clear();
        yt.clear();
        xAtaqueS.clear();
        xSalidaS.clear();
        model.clear();
        cy.clear();
    }


    int num = propNb - 1;
    QVector<double>cosGridV;


    QVector<QVector<point3d>>xyv;




    int ii = 0;
    int Ny = num * meshY;
    meshNum = meshX * Ny;
    for(int i = 0;i<meshX + 1;i++){
        cosGridV.append(0.5 * (1 - cos(double(i) / meshX* M_PI)));
    }

    for(int i = 0;i<num;i++){
        int len = meshY;
        if(i == 0)
            len = len + 1;
        //double psiTanTmp = (offsetLength[num - i] - offsetLength[num - i - 1]) / (-damiterArray[num - i] + damiterArray[num - i - 1]);
        for(int j = 0;j<len;j++){
            double tmp1 = (damiterArray[num - i] - damiterArray[num - i - 1]) / meshY * (j + ii);
            double tmp2 = (propChord[num - i - 1] - propChord[num - i]) / meshY * (j + ii) + propChord[num - i];
            //double tmp3 = offsetLength[num - i - 1] + psiTanTmp * (-damiterArray[num - i] + tmp1 + damiterArray[num - i - 1]);
            double tmp3 = (xAtaque[num - i - 1] - xAtaque[num - i]) / meshY * (j + ii) + xAtaque[num - i];
            double tmp4 = (xSalida[num - i - 1] - xSalida[num - i]) / meshY * (j + ii) + xSalida[num - i];
            yt.append(-damiterArray[num - i] + tmp1);
            cy.append(tmp2);
            xAtaqueS.append(tmp3);
            xSalidaS.append(tmp4);
        }
        ii = 1;
    }


    for(int i = 0;i<Ny + 1;i++){
        QVector<point3d>tmp;
        for(int j = 0;j<meshX + 1;j++){
            point3d tmp2;
            tmp2.x = xAtaqueS[i] + (xSalidaS[i] - xAtaqueS[i]) * cosGridV[j];
            tmp2.y = yt[i];
            tmp2.z = 0;
            tmp.append(tmp2);
        }
        xyv.append(tmp);
    }
    for(int i = 0;i<meshX + 1;i++){
        QVector<point3d>xyvTmp;
        for(int j = 0;j<Ny + 1;j++){
            xyvTmp.append(xyv[j][i]);
        }
        xyvvUpper.append(xyvTmp);
        xyvvLower.append(xyvTmp);
    }

    QVector<QVector<double>>xCur;   //网格段弦长百分比
    for(int i = 0;i<meshX + 1;i++){
        QVector<double>xCurTmp;
        for(int j = 0;j<Ny + 1;j++){
            double tmp = cosGridV[i];
            xCurTmp.append(tmp);
        }
        xCur.append(xCurTmp);
    }

    double xdistance = xAtaqueS[Ny] + 0.25 * (propChord[0]);
    for(int i = 0;i<meshX + 1;i++){
        for(int j = 0;j<Ny + 1;j++){
            xyvvUpper[i][j].z = computeUpperZ(j,xCur[i][j],cy[j]);
            xyvvLower[i][j].z = computeLowerZ(j,xCur[i][j],cy[j]);
            xyvvUpper[i][j].x = xyvvUpper[i][j].x - xdistance;
            xyvvLower[i][j].x = xyvvLower[i][j].x - xdistance;
        }
    }


}
void propellerDesign::generateMesh(){
    int Ny = meshY * (propNb - 1);

    for(int i = 0;i<meshX;i++){
        for(int j = 0;j<Ny;j++){
            xyAUpper.append(xyvvUpper[i][j]);
            xyBUpper.append(xyvvUpper[i][j + 1]);
            xyCUpper.append(xyvvUpper[i + 1][j + 1]);
            xyDUpper.append(xyvvUpper[i + 1][j]);
            xyALower.append(xyvvLower[i][j]);
            xyBLower.append(xyvvLower[i][j + 1]);
            xyCLower.append(xyvvLower[i + 1][j + 1]);
            xyDLower.append(xyvvLower[i + 1][j]);
        }
    }

    //平移至坐标中点


}
void propellerDesign::generate3DModel(){
    int Ny = meshY * (propNb - 1);
    for(int i = 0;i<propNb;i++){
        int index = Ny; // 初始化index
        for (int previousSection = 0; previousSection < i; previousSection++) {
            index = index - meshY; // 确保index在内层循环中被正确设置
        }
        QVector<point3d>tmp;
        for(int j = meshX;j>=0;j--){
            tmp.append(xyvvUpper[j][index]);
        }
        for(int j = 1;j<meshX + 1;j++){
            tmp.append(xyvvLower[j][index]);
        }
        model.append(tmp);
    }



}
void propellerDesign::twistProp(){
    //计算每段的扭转角
    QVector<double>th;
    int num = propNb - 1;
    int Ny = num * meshY;
    for(int i = 0;i<num;i++){
        for(int j = 0;j<meshY;j++){
            int G = i * meshY + j;
            double twistGradient = (propTwist[num - i - 1] - propTwist[num - i]) /(damiterArray[num - i] - damiterArray[num - i - 1]);
            double t = (propTwist[num - i] + twistGradient * (yt[G] + damiterArray[num - i])) / 180 * M_PI;
            th.append(t);
        }
    }
    th.append(propTwist[0] / 180 * M_PI);

    //计算每段扭转矢量方向
    QVector<point3d>vp;
    QVector<point3d>vn;
    QVector<double>xcooder;
    for(int i = 0;i<Ny + 1;i++){
        xcooder.append(abs(xAtaqueS[i] - xSalidaS[i]) / 4 + xAtaqueS[i]);
    }
    for(int i = 0;i<num;i++){
        for(int j = 0;j<meshY;j++){
            int G = i * meshY + j;
            //point3d vnTmp1(xyvvUpper[0][G].x,xyvvUpper[0][G].y,xyvvUpper[0][G].z);
            //point3d vnTmp2(xyvvUpper[0][G].x,xyvvUpper[0][G].y + 0.1,xyvvUpper[0][G].z);
            point3d vnTmp1(xcooder[G],xyvvUpper[0][G].y,0);
            point3d vnTmp2(xcooder[G],xyvvUpper[0][G].y + 0.1,0);
            point3d vnTmp = calculateVector(vnTmp1,vnTmp2);
            vp.append(vnTmp1);
            vn.append(vnTmp);

        }
    }

    // point3d vntmp1(xyvvUpper[0][Ny].x,xyvvUpper[0][Ny].y,xyvvUpper[0][Ny].z);
    // point3d vntmp2(xyvvUpper[0][Ny].x,xyvvUpper[0][Ny].y + 0.1,xyvvUpper[0][Ny].z);
    point3d vntmp1(xcooder[Ny],xyvvUpper[0][Ny].y,0);
    point3d vntmp2(xcooder[Ny],xyvvUpper[0][Ny].y + 0.1,0);
    point3d vntmp = calculateVector(vntmp1,vntmp2);
    vp.append(vntmp1);
    vn.append(vntmp);

    for(int i = 0;i<meshX + 1;i++){
        for(int j = 0;j<Ny + 1;j++){
            xyvvUpper[i][j] = rotatePointAroundAxis(xyvvUpper[i][j],vp[j],vn[j],th[j]);
            xyvvLower[i][j] = rotatePointAroundAxis(xyvvLower[i][j],vp[j],vn[j],th[j]);
        }
    }
}

void propellerDesign::startXfoil(const int index,const int num){
    QVector<QFuture<void>>futures;

    QFutureSynchronizer<void> sync;
    QVector<airfoilSolve*>testSolve;


    for(int i = 0;i<num;i++){

        xfoilSetting settingTmp = airfoilSetting;

        settingTmp.Ma = mai[index + i];
        settingTmp.Re = rei[index + i];
        airfoilSolve *solvers = new airfoilSolve();
        testSolve.append(solvers);
        solvers->importAirfoil(airfoil);
        solvers->refreshParaments(settingTmp);
        solvers->setMissionId(index + i);
        QFuture<void>future = QtConcurrent::run([solvers](){
            solvers->solver();
        });
        futures.append(future);
    }

    bool allFinished = false;
    while(!allFinished){
        allFinished = true;
        for(int i = 0; i< num;i++){
            if(!futures[i].isFinished()){
                allFinished = false;
                break;
            }
        }
        QCoreApplication::processEvents();
    }



    for(int i = 0;i<num;i++){
        emit emitMessage("线程:" + QString::number(i) + "计算完成\n");
        resultArray.append(getBestSoluation(testSolve[i]->resultData));
    }
    qDeleteAll(testSolve);
    testSolve.clear();
}
void propellerDesign::smoothPropTwist(const int order){
    QVector<double>tmp = mathSolve.smoothCurve(damiterArray,propTwist,order);
    propTwist = tmp;
}
void propellerDesign::smoothPropXAtaque(const int order){
    QVector<double>tmp1 = mathSolve.smoothCurve(damiterArray,xAtaque,order);
    xAtaque = tmp1;
    QVector<double>tmp2;
    for(int i = 0;i<propNb;i++){
        tmp2.append(xSalida[i] - xAtaque[i]);
    }
    propChord = tmp2;
}
void propellerDesign::smoothPropXSalida(const int order){
    QVector<double>tmp1 = mathSolve.smoothCurve(damiterArray,xSalida,order);
    xSalida = tmp1;
    QVector<double>tmp2;
    for(int i = 0;i<propNb;i++){
        tmp2.append(xSalida[i] - xAtaque[i]);
    }
    propChord = tmp2;
}
double propellerDesign::computeUpperZ(const int index,const double x, const double c){
    double z = 0;
    for(int i = 1;i<cstNum + 1;i++){
        z = upperCst[i - 1] * std::pow(x,0.5) * std::pow(1 - x,1) * nchoosek(cstNum - 1,i-1) * std::pow(1 - x,cstNum - i) * std::pow(x,i - 1) + z;
    }

    return (z + upperYte[index] * x) * c;
}
double propellerDesign::computeLowerZ(const int index,const double x, const double c){
    double z = 0;
    for(int i = 1;i<cstNum + 1;i++){
        z = lowerCst[i - 1] * std::pow(x,0.5) * std::pow(1 - x,1) * nchoosek(cstNum - 1,i-1) * std::pow(1 - x,cstNum - i) * std::pow(x,i - 1) + z;
    }

    return (z + lowerYte[index] * x) * c;
}
void propellerDesign::computeCst(){
    if(!upperCst.isEmpty()){
        upperCst.clear();
        lowerCst.clear();
        upperYte.clear();
        lowerYte.clear();
    }
    int num = propNb - 1;
    int Ny = num * meshY;
    double yte = ctYte / propChord[0];

    airfoilDesign designer(cstNum);
    designer.buildBenrnstein(airfoil);
    for(int i = 0;i<cstNum;i++){
        upperCst.append(designer.cstParameter[i]);
        lowerCst.append(designer.cstParameter[cstNum * 2  - 1 - i]);
    }
    for(int i = 0;i<Ny + 1;i++){
        if(yteType){
            upperYte.append(yte / 2);
            lowerYte.append(-yte / 2);
        }else{
            upperYte.append(yte / 2 / cy[i] * propChord[0]);
            lowerYte.append(-yte / 2 / cy[i] * propChord[0]);
        }
    }
}
point3d propellerDesign::rotatePointAroundAxis(const point3d& point, const point3d& axisPoint, const point3d& axisDir, double rotationAngle) {
    // 将角度转换为弧度
    //double rad = angle * M_PI / 180.0;

    // 轴线方向的单位向量
    double u = axisDir.x;
    double v = axisDir.y;
    double w = axisDir.z;

    // 计算单位向量的长度并归一化
    double length = sqrt(u * u + v * v + w * w);
    u /= length;
    v /= length;
    w /= length;

    // 计算点到轴点的相对坐标
    double x = point.x - axisPoint.x;
    double y = point.y - axisPoint.y;
    double z = point.z - axisPoint.z;

    // 计算旋转矩阵的分量
    double cosA = cos(rotationAngle);
    double sinA = sin(rotationAngle);
    double oneMinusCosA = 1.0 - cosA;

    double newX = (u * u + (v * v + w * w) * cosA) * x +
                  (u * v * oneMinusCosA - w * sinA) * y +
                  (u * w * oneMinusCosA + v * sinA) * z;

    double newY = (u * v * oneMinusCosA + w * sinA) * x +
                  (v * v + (u * u + w * w) * cosA) * y +
                  (v * w * oneMinusCosA - u * sinA) * z;

    double newZ = (u * w * oneMinusCosA - v * sinA) * x +
                  (v * w * oneMinusCosA + u * sinA) * y +
                  (w * w + (u * u + v * v) * cosA) * z;

    // 将点平移回旋转轴点的相对位置
    return {newX + axisPoint.x, newY + axisPoint.y, newZ + axisPoint.z};
}
point3d propellerDesign::normalizeVector(point3d vec){
    double length = sqrt(vec.x * vec.x + vec.y * vec.y + vec.z * vec.z);
    return point3d(vec.x / length, vec.y / length, vec.z / length);
}
point3d propellerDesign::calculateVector(point3d P1, point3d P2){
    point3d a(P2.x - P1.x, P2.y - P1.y, P2.z - P1.z);
    return normalizeVector(a);
}
int propellerDesign::nchoosek(const int order,const int index) {

    int result = 1;
    for(int i = 0;i < index;i++) {
        result = result * (order - i) / (i + 1);
    }
    return result;
}
QVector<double> propellerDesign::getBestSoluation(const QVector<QVector<double>>&result){
    QVector<double>a;
    a<<0<<0<<1<<0;
    if(!result.isEmpty()){
        QVector<double>tmp;
        for(int i = 0;i<result.length();i++){
            tmp.append(result[i][3]);
        }
        a = result[mathSolve.getMaxIndex(tmp)];
    }
    return a;
}
double propellerDesign::trapezoidalIntegration(const QVector<double>& r, const QVector<double>& K) {
    int n = r.size();
    double integralValue = 0.0;

    // 使用梯形法计算积分值
    for (int i = 0; i < n - 1; ++i) {
        integralValue += ((K[i] + K[i + 1]) / 2) * (r[i + 1] - r[i]);
    }

    return integralValue;
}
double propellerDesign::computeKp(double r, double R, double omega, double V, int n) {
    return 2.0 / M_PI * acos(exp(-n / 2.0 * (1.0 - r / R) * sqrt(1.0 + pow(omega * R / V, 2.0))));
}

// Function to compute the integral
/*
double propellerDesign::computeIntegral(double K, double V, double omega, const QVector<double>& r, const QVector<double>& kp) {
    double sum = 0.0;
    int num_points = r.size();
    for (int i = 0; i < num_points; ++i) {
        double K1 = K / (1.0 + pow(V / (omega * r[i]), 2.0) * pow(1.0 + K, 2.0));
        double func = (K1 + pow(K1, 2.0)) * kp[i] * r[i];
        sum += func * (r[1] - r[0]);
    }
    return sum;
}
*/
double propellerDesign::computeIntegral(
    double K,
    double V,
    double omega,
    const QVector<double>& r,
    const QVector<double>& lossFactors)
{
    double sum = 0.0;
    int n = r.size();

    for (int i = 0; i < n - 1; ++i) {
        double r1 = r[i];
        double r2 = r[i+1];

        double K1_1 = K / (1.0 + pow(V / (omega * r1), 2.0) * pow(1.0 + K, 2.0));
        double f1 = (K1_1 + K1_1 * K1_1) * lossFactors[i] * r1;

        double K1_2 = K / (1.0 + pow(V / (omega * r2), 2.0) * pow(1.0 + K, 2.0));
        double f2 = (K1_2 + K1_2 * K1_2) * lossFactors[i+1] * r2;

        sum += 0.5 * (f1 + f2) * (r2 - r1);
    }

    return sum;
}

// Function to find K using binary search
double propellerDesign::findK(double sum2, double V, double omega, const QVector<double>& r, const QVector<double>& lossFactors, double tolerance) {
    double K1 = 0.0;
    double K2 = 1.0;
    double K = (K1 + K2) / 2.0;
    const double effectiveTolerance = std::clamp(
        tolerance, std::numeric_limits<double>::epsilon(), 0.5);
    for (int iteration = 0; iteration < 100 && std::abs(K2 - K1) > effectiveTolerance;
         ++iteration) {
        K = (K1 + K2) / 2.0;
        double integral = computeIntegral(K, V, omega, r, lossFactors);
        if (sum2 > integral) {
            K1 = K;
        } else {
            K2 = K;
        }
    }
    return K;
}
