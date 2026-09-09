#include "airplanevlm.h"
#include <QDebug>
#include <QtMath>
airplaneVLM::airplaneVLM()
{
    vinf = 20;
    designLiftForce = 60;
    height = 0;
    density = 1.225;
    wingN = 0;
    wakeN = 0;
    cg.x = 0.0;

    referenceArea = 1.0;//防止除0
    viscousCd = 0;
    wingModel = new wingVLM();
    hTailModel = new wingVLM();
    vTailModel = new wingVLM();
    airplaneModel = new wingVLM();

    withWing = false;
    withHTail = false;
    withVTail = false;
}

void airplaneVLM::solver(){
    emit emitProgressValue(0);
    clearAllValue();
    initialAirplaneVortex();
    initialWakeVortex();
    initialTrefftzVortex();

    solveAirplaneInfluenceCoefficient();

    computeAirplaneVLMMatrix();

    solveAirplaneGamma();

    computeAirplaneVelocity();

    solveAirplaneForce();

    computeAerodynamicCoefficient();
    computeTrefftzDrag();

    computePressure();
    //solveZVelocity();





    /*

    if(withWing)
        qDebug()<<wingModel->wingCL<<" "<<wingModel->wingCD;
    if(withHTail)
        qDebug()<<hTailModel->wingCL<<" "<<hTailModel->wingCD;
    if(withVTail)
        qDebug()<<vTailModel->wingCL<<" "<<vTailModel->wingCD;

    */
    for(int i = 0;i<vinfArray.length();i++){
        qDebug()<<alphaArray[i]<<" :"<<clArray[i]<<" "<<cdArray[i]<<" "<<cMArray[i];
    }


}
void airplaneVLM::initialize(){




    if(withWing){

        wingModel->initialGeometry(wingData);

    }
    else
        wingModel->zeros();


    if(withHTail){
        hTailModel->initialGeometry(hTailData);

    }
    else
        hTailModel->zeros();


    if(withVTail){
        vTailModel->initialGeometry(vTailData);
    }
    else
        vTailModel->zeros();




    wingN = wingModel->getMeshNum() + hTailModel->getMeshNum() + vTailModel->getMeshNum();
    wakeN = wingModel->getNyNum() + hTailModel->getNyNum() + vTailModel->getNyNum();
}
void airplaneVLM::setOffsetValue(double a[],double b[],double c[]){
    wingModel->setOffsetValue(a[0],a[1],a[2],a[3]);
    hTailModel->setOffsetValue(b[0],b[1],b[2],b[3]);
    vTailModel->setOffsetValue(c[0],c[1],c[2],c[3]);

}
void airplaneVLM::refreshParaments(const VLMSetting&a){


    wingModel->refreshParaments(a);
    hTailModel->refreshParaments(a);
    vTailModel->refreshParaments(a);

    QVector<double>tmp;
    QVector<point3d>vtmp;
    double min = 0,max = 5,step = 1;
    double v = 20;
    if(a.maxAlpha < a.minAlpha){
        min = a.maxAlpha;
        max = a.minAlpha;
    }else{
        min = a.minAlpha;
        max = a.maxAlpha;
    }

    if(a.stepAlpha >0)
        step = a.stepAlpha;



    if(a.vinf > 0)
        vinf = a.vinf;
    else
        vinf = v;
    if(isStability){
        for(int i = 0;i<vinfArray.length();i++)
            tmp.append(0);

    }else{

        int len = int(abs(max - min) / step);
        for(int i = 0;i<len;i++){
            tmp.append(min + step * double(i));
        }
        tmp.append(max);

        for(int i = 0;i<len + 1;i++){
            vtmp.append(point3d(vinf * cos(tmp[i] / 180 * M_PI),0,vinf * sin(tmp[i] / 180 * M_PI)));
        }

        vinfArray = vtmp;
    }

    alphaArray = tmp;

    wingModel->alphaArray = alphaArray;
    wingModel->vinfArray = vinfArray;

    hTailModel->alphaArray = alphaArray;
    hTailModel->vinfArray = vinfArray;

    vTailModel->alphaArray = alphaArray;
    vTailModel->vinfArray = vinfArray;


    height = a.height;
    cg.x = a.referencePointX;

    density = mathSolver.calculateAirDensity(height);

}

void airplaneVLM::initialAirplaneVortex(){
    for(int i = 0;i<wingModel->vortexArray.length();i++){
        vortexArray.append(wingModel->vortexArray[i]);

    }

    for(int i = 0;i<hTailModel->vortexArray.length();i++){
        vortexArray.append(hTailModel->vortexArray[i]);

    }

    for(int i = 0;i<vTailModel->vortexArray.length();i++){
        vortexArray.append(vTailModel->vortexArray[i]);
    }
}
void airplaneVLM::initialWakeVortex(){
    int len1 = wingModel->getNyNum();
    int len2 = hTailModel->getNyNum();
    int len3 = vTailModel->getNyNum();
    for(int i = 0;i<vinfArray.length();i++){
        for(int ii = 0;ii<len1;ii++){
            wakeVortexArray.append(wingModel->wakeVortexArray[len1 * i + ii]);
        }
        for(int jj = 0;jj<len2;jj++){
            wakeVortexArray.append(hTailModel->wakeVortexArray[len2 * i + jj]);

        }
        for(int kk = 0;kk<len3;kk++){
            wakeVortexArray.append(vTailModel->wakeVortexArray[len3 * i + kk]);
        }
    }

}
void airplaneVLM::initialTrefftzVortex(){
    int len1 = wingModel->getNyNum();
    int len2 = hTailModel->getNyNum();
    int len3 = vTailModel->getNyNum();
    for(int i = 0;i<len1;i++){
        trefftzArray.append(wingModel->trefftzArray[i]);
    }
    for(int i = 0;i<len2;i++){
        trefftzArray.append(hTailModel->trefftzArray[i]);
    }
    for(int i = 0;i<len3;i++){
        trefftzArray.append(vTailModel->trefftzArray[i]);
    }
}
void airplaneVLM::solveAirplaneInfluenceCoefficient(){

    QVector<QVector<double>>vnWingArray;//翼面涡格对控制点的法向影响系数
    QVector<QVector<double>>vnWakeArray;//尾迹涡格对控制点的法向影响系数
    QVector<QVector<double>>vnTrefftzArray;
    QVector<QVector<double>> nxfSArray, nyfSArray, nzfSArray;//翼面涡格对中心点的影响系数
    QVector<QVector<double>>nxfWakeArray,nyfWakeArray,nzfWakeArray;//尾迹涡格对中心点的影响系数
    computeAirplaneFluenceCoefficient(vnWingArray,nxfSArray, nyfSArray, nzfSArray);
    computeWakeFluenceCoefficient(vnWakeArray,nxfWakeArray, nyfWakeArray, nzfWakeArray);
    computeTrefftzFluenceCoefficient(vnTrefftzArray);

    for(int i = 0;i<vinfArray.length();i++){
        QVector<QVector<double>>vzTotalArray = vnWingArray;//
        QVector<QVector<double>>nxArray = nxfSArray;
        QVector<QVector<double>>nyArray = nyfSArray;
        QVector<QVector<double>>nzArray = nzfSArray;
        for(int j = 0;j<wingN;j++){
            int G = i * wingN + j;
            for(int k = 0;k<wakeN;k++){
                vzTotalArray[j][wingN + k] = vnWakeArray[G][k];
                nxArray[j][wingN + k] = nxfWakeArray[G][k];
                nyArray[j][wingN + k] = nyfWakeArray[G][k];
                nzArray[j][wingN + k] = nzfWakeArray[G][k];

            }
        }
        vNArray.append(vzTotalArray);
        vinxArray.append(nxArray);
        vinyArray.append(nyArray);
        vinzArray.append(nzArray);

    }
    vNTrefftzArray = vnTrefftzArray;



}
void airplaneVLM::computeAirplaneVLMMatrix(){
    int progressValue;
    int step = vinfArray.length();
    int n1 = wingModel->getMeshNum();
    int ny1 = wingModel->getNyNum();
    int n2 = hTailModel->getMeshNum();
    int ny2 = hTailModel->getNyNum();
    //int n3 = vTailModel->getMeshNum();
    int ny3 = vTailModel->getNyNum();

    QVector<QVector<double>>mgammaArray;
    QVector<QVector<double>>wakeMGama;

    Eigen::MatrixXd MatrixQRA;
    MatrixQRA.resize(wingN,wingN);
    Eigen::ColPivHouseholderQR<Eigen::MatrixXd> qr;


    for(int j = 0;j<wingN ;j++){
        QVector<double>matrixTmp;
        for(int k = 0;k<wingN;k++){

            matrixTmp.append(vNArray[0][j][k]);

            MatrixQRA.coeffRef(j,k) = vNArray[0][j][k];
        }
        //matrixA.append(matrixTmp);
    }

    for(int j = 0;j<wingN ;j++){

        for(int k = 0;k<ny1;k++){
            MatrixQRA.coeffRef(j,n1 - ny1 + k) = vNArray[0][j][n1 - ny1 + k] + vNArray[0][j][wingN + k];
        }
        for(int k = 0;k<ny2;k++){
            MatrixQRA.coeffRef(j,n1 + n2 - ny2 + k) = vNArray[0][j][n1 + n2 - ny2 + k] + vNArray[0][j][wingN + ny1 + k];
        }
        for(int k = 0;k<ny3;k++){
            MatrixQRA.coeffRef(j,wingN - ny3 + k) = vNArray[0][j][wingN - ny3 + k] + vNArray[0][j][wingN + wakeN - ny3 + k];
        }
    }

    qr.compute(MatrixQRA);







    for(int i = 0;i<vinfArray.length();i++){

        //QVector<QVector<double>>matrixA;
        QVector<double>mgamma;
        QVector<double>matrixb;
        QVector<double>wakeGama;

        Eigen::MatrixXd MatrixA;
        Eigen::VectorXd MatrixGamma;
        Eigen::VectorXd Matrixb;
        Matrixb.resize(wingN);
        MatrixA.resize(wingN,wingN);

        for(int j = 0;j<wingN ;j++){
            QVector<double>matrixTmp;
            for(int k = 0;k<wingN;k++){

                matrixTmp.append(vNArray[i][j][k]);

                MatrixA.coeffRef(j,k) = vNArray[i][j][k];
            }
            //matrixA.append(matrixTmp);
        }

        for(int j = 0;j<wingN ;j++){

            for(int k = 0;k<ny1;k++){
                MatrixA.coeffRef(j,n1 - ny1 + k) = vNArray[i][j][n1 - ny1 + k] + vNArray[i][j][wingN + k];
            }
            for(int k = 0;k<ny2;k++){
                MatrixA.coeffRef(j,n1 + n2 - ny2 + k) = vNArray[i][j][n1 + n2 - ny2 + k] + vNArray[i][j][wingN + ny1 + k];
            }
            for(int k = 0;k<ny3;k++){
                MatrixA.coeffRef(j,wingN - ny3 + k) = vNArray[i][j][wingN - ny3 + k] + vNArray[i][j][wingN + wakeN - ny3 + k];
            }
        }



        for(int j = 0;j<wingN;j++){
            point3d v = vinfArray[i];
            point3d n(vortexArray[j].nControl.x,vortexArray[j].nControl.y,vortexArray[j].nControl.z);
            matrixb.append(-dotProduct(v,n));
            Matrixb.coeffRef(j) = matrixb[j];
        }

        MatrixGamma = qr.solve(Matrixb);



        //MatrixGamma = MatrixA.colPivHouseholderQr().solve(Matrixb);


        for(int k = 0;k<wingN;k++)
            mgamma.append(MatrixGamma(k));

        for(int k = 0;k<ny1;k++){
            wakeGama.append(mgamma[n1 - ny1 + k]);
        }
        for(int k = 0;k<ny2;k++){
            wakeGama.append(mgamma[n1 + n2 - ny2 + k]);
        }
        for(int k = 0;k<ny3;k++){
            wakeGama.append(mgamma[wingN - ny3]);
        }




        mgammaArray.append(mgamma);
        wakeMGama.append(wakeGama);

        if(!isOptimization){
            if(isAddXfoilDrag && !isUseLibraries){
                progressValue = (double(i) + 1) / (step) * 50;

            }else{
                progressValue = (double(i) + 1) / (step) * 100;

            }

            emit emitProgressValue(progressValue);
        }




    }

    matrixGammaArray = mgammaArray;
    wakeGammaArray = wakeMGama;

}

void airplaneVLM::solveAirplaneGamma(){
    QVector<QVector<double>>wingGama;
    QVector<QVector<double>>hTailGama;
    QVector<QVector<double>>vTailGama;
    int len1 = wingModel->getMeshNum();
    int len2 = hTailModel->getMeshNum();
    int len3 = vTailModel->getMeshNum();
    for(int i = 0;i<vinfArray.length();i++){
        QVector<double>wingGamaTmp;
        QVector<double>hTailGamaTmp;
        QVector<double>vTailGamaTmp;
        for(int j = 0;j<len1;j++){
            wingGamaTmp.append(matrixGammaArray[i][j]);
        }
        for(int j = 0;j<len2;j++){
            hTailGamaTmp.append(matrixGammaArray[i][len1 + j]);
        }
        for(int j = 0;j<len3;j++){
            vTailGamaTmp.append(matrixGammaArray[i][len1 + len2 + j]);
        }

        wingGama.append(wingGamaTmp);
        hTailGama.append(hTailGamaTmp);
        vTailGama.append(vTailGamaTmp);
    }


    if(withWing)
        wingModel->computeGamma(wingGama);
    if(withHTail)
        hTailModel->computeGamma(hTailGama);
    if(withVTail)
        vTailModel->computeGamma(vTailGama);



}
void airplaneVLM::computeAerodynamicCoefficient(){

    QVector<double>wingCm;
    QVector<double>hTailCm;
    QVector<double>vTailCm;
    QVector<double>cmTmp;
    referenceArea = wingModel->getRealArea();
    for(int i = 0;i<vinfArray.length();i++){
        wingCm.append(0);
        hTailCm.append(0);
        vTailCm.append(0);
    }
    if(withWing){
        wingModel->computeMoments(cg);
        wingModel->computeAerodynamicCoefficient(referenceArea,wingModel->getMac());
        wingCm = wingModel->wingCM;
    }

    if(withHTail){
        hTailModel->computeMoments(cg);
        hTailModel->computeAerodynamicCoefficient(referenceArea,wingModel->getMac());
        hTailCm = hTailModel->wingCM;
    }

    if(withVTail){
        vTailModel->computeMoments(cg);
        vTailModel->computeAerodynamicCoefficient(referenceArea,wingModel->getMac());
        vTailCm = vTailModel->wingCM;
    }

    for(int i = 0;i<vinfArray.length();i++){
        cmTmp.append(wingCm[i] + hTailCm[i] + vTailCm[i]);
    }

    cMArray = cmTmp;


}
void airplaneVLM::solveAirplaneForce(){
    QVector<QVector<point3d>>tmpWing;
    QVector<QVector<point3d>>tmpHTail;
    QVector<QVector<point3d>>tmpVTail;
    int len1 = wingModel->getMeshNum();
    int len2 = hTailModel->getMeshNum();
    int len3 = vTailModel->getMeshNum();

    for(int i = 0;i<vinfArray.length();i++){
        QVector<point3d>tmp1;
        QVector<point3d>tmp2;
        QVector<point3d>tmp3;

        for(int j = 0;j<len1;j++){
            tmp1.append(streamVinfArray[i][j]);
        }
        for(int j = 0;j<len2;j++){
            tmp2.append(streamVinfArray[i][len1 + j]);
        }
        for(int j = 0;j<len3;j++){
            tmp3.append(streamVinfArray[i][len1 + len2 + j]);
        }

        tmpWing.append(tmp1);
        tmpHTail.append(tmp2);
        tmpVTail.append(tmp3);
    }


    if(withWing)
        wingModel->computeForce(tmpWing);
    if(withHTail)
        hTailModel->computeForce(tmpHTail);
    if(withVTail)
        vTailModel->computeForce(tmpVTail);

}


void airplaneVLM::computeAirplaneVelocity(){
    QVector<QVector<point3d>>tmp2;
    double vxt,vyt,vzt;
    for(int alphaIndex = 0;alphaIndex<vinfArray.length();alphaIndex++){
        QVector<point3d>tmp1;

        for(int i = 0;i<wingN;i++){
            vxt = 0;
            vyt = 0;
            vzt = 0;
            for(int j = 0;j<wingN;j++){
                vxt = vxt + vinxArray[alphaIndex][i][j] * matrixGammaArray[alphaIndex][j];
                vyt = vyt + vinyArray[alphaIndex][i][j] * matrixGammaArray[alphaIndex][j];
                vzt = vzt + vinzArray[alphaIndex][i][j] * matrixGammaArray[alphaIndex][j];

            }
            for(int j = wingN - wakeN;j<wingN;j++){
                vxt = vxt + vinxArray[alphaIndex][i][j + wakeN] * wakeGammaArray[alphaIndex][j - wingN + wakeN];
                vyt = vyt + vinyArray[alphaIndex][i][j + wakeN] * wakeGammaArray[alphaIndex][j - wingN + wakeN];
                vzt = vzt + vinzArray[alphaIndex][i][j + wakeN] * wakeGammaArray[alphaIndex][j - wingN + wakeN];
            }

            tmp1.append(point3d(vxt + vinfArray[alphaIndex].x,vyt + vinfArray[alphaIndex].y,vzt + vinfArray[alphaIndex].z));
        }



        tmp2.append(tmp1);
    }
    streamVinfArray = tmp2;

}
void airplaneVLM::computeTrefftzDrag(){
    QVector<double>cdArrayTmp;
    QVector<double>clArrayTmp;
    QVector<QVector<double>>wingForceArray;
    QVector<QVector<double>>hTailForceArray;
    QVector<QVector<double>>vTailForceArray;
    QVector<double>cosT;

    QVector<double>dl;
    QVector<QVector<double>>mForceArray;

    int ny1 = wingModel->getNyNum();
    int ny2 = hTailModel->getNyNum();
    int ny3 = vTailModel->getNyNum();

    double zeroLiftDrag = 0;
    if(isAddZeroLiftDrag)
        zeroLiftDrag = viscousCd;


    //计算dl
    for(int i = 0;i<wakeN;i++){
        double dy2 = pow(trefftzArray[i].xyzA.y - trefftzArray[i].xyzB.y,2);
        double dz2 = pow(trefftzArray[i].xyzA.z - trefftzArray[i].xyzB.z,2);
        double ds = sqrt(dy2 + dz2);
        double cosTmp = abs(trefftzArray[i].xyzA.y - trefftzArray[i].xyzB.y) / ds;
        dl.append(ds);
        cosT.append(cosTmp);
    }
    QVector<double>wingVis;
    QVector<double>hTailVis;
    QVector<double>vTailVis;

    for(int i = 0;i<vinfArray.length();i++){
        double dTmp = 0;
        double lTmp = 0;


        QVector<double>wingForce;
        QVector<double>hTailForce;
        QVector<double>vTailForce;
        QVector<double>inducdArray;
        wingVis.append(0.0);
        hTailVis.append(0.0);
        vTailVis.append(0.0);
        //double cosAlpha = cos(alphaArray[i] / 180 * M_PI);
        //double sinAlpha = sin(alphaArray[i] / 180 * M_PI);

        for(int j = 0;j<wakeN;j++){


            bool ok1 = (j < ny1);
            bool ok2 = (j >= ny1) && (j < ny1 + ny2);
            bool ok3 = (j >= ny1 + ny2) && (j < ny1 + ny2 + ny3);
            double vTmp = 0;
            for(int k = 0;k<wakeN;k++){
                vTmp = vTmp + vNTrefftzArray[j][k] * wakeGammaArray[i][k];
            }
            dTmp +=  -0.5 * density * vTmp * wakeGammaArray[i][j] * dl[j] ;
            double tmp = wakeGammaArray[i][j] * cosT[j] * dl[j] * vinf * density;

            lTmp += tmp;
            if(withWing && ok1){

                 wingForce.append(tmp);

            }
            if(withHTail && ok2){

                 hTailForce.append(tmp);

            }
            if(withVTail && ok3){

                 vTailForce.append(tmp);

            }

        }
        //double drag = cosAlpha * dTmp + sinAlpha * lTmp;
        //double lift = - sinAlpha * dTmp + cosAlpha * lTmp;
        cdArrayTmp.append(dTmp * 2 / density / referenceArea / vinf / vinf);
        clArrayTmp.append(lTmp * 2 / density / referenceArea / vinf / vinf);
        wingForceArray.append(wingForce);
        hTailForceArray.append(hTailForce);
        vTailForceArray.append(vTailForce);

    }
    int len = 0;
    if(withWing)
        len++;
    if(withHTail)
        len++;
    if(withVTail)
        len++;

    if(withWing){
        wingModel->setXfoilDrag(isAddXfoilDrag);
        wingModel->setUseLibraries(isUseLibraries);
        wingModel->setZeroLiftDrag(isAddZeroLiftDrag);
        wingModel->viscousCd = viscousCd;
        wingModel->solveSpanForce(wingForceArray,referenceArea);
        wingVis = wingModel->wingViscousDrag;

        emit emitProgressValue(int(50 + 50.0 / len));
    }

    if(withHTail){
        hTailModel->setXfoilDrag(isAddXfoilDrag);
        hTailModel->setUseLibraries(isUseLibraries);
        hTailModel->setZeroLiftDrag(isAddZeroLiftDrag);
        hTailModel->viscousCd = viscousCd;
        hTailModel->solveSpanForce(hTailForceArray,referenceArea);

        hTailVis = hTailModel->wingViscousDrag;
        emit emitProgressValue(int(50 + 50.0 / len * 2));
    }

    if(withVTail){
        vTailModel->setXfoilDrag(isAddXfoilDrag);
        vTailModel->setUseLibraries(isUseLibraries);
        vTailModel->setZeroLiftDrag(isAddZeroLiftDrag);
        vTailModel->viscousCd = viscousCd;
        vTailModel->solveSpanForce(vTailForceArray,referenceArea);

        vTailVis = vTailModel->wingViscousDrag;

        emit emitProgressValue(100);
    }

    double ratio1 = hTailModel->getRealArea() / wingModel->getRealArea();
    double ratio2 = vTailModel->getRealArea() / wingModel->getRealArea();


    for(int i = 0;i<vinfArray.length();i++){
        cdArrayTmp[i] += wingVis[i] + hTailVis[i] * ratio1 + vTailVis[i] * ratio2 + zeroLiftDrag;
    }
    clArray = clArrayTmp;
    cdArray = cdArrayTmp;


}
void airplaneVLM::computeAirplaneForce(){




}
void airplaneVLM::computeAirplaneCm(){

}
void airplaneVLM::computeAirplanePressure(){

}
void airplaneVLM::computeAirplaneFluenceCoefficient(QVector<QVector<double>>&vnTotal,QVector<QVector<double>>&bx,QVector<QVector<double>>&by,QVector<QVector<double>>&bz){
    for(int i = 0;i<wingN;i++){
        double xcoloc = vortexArray[i].xyzControl.x;
        double ycoloc = vortexArray[i].xyzControl.y;
        double zcoloc = vortexArray[i].xyzControl.z;

        double xfcoloc = vortexArray[i].xyzCenter.x;
        double yfcoloc = vortexArray[i].xyzCenter.y;
        double zfcoloc = vortexArray[i].xyzCenter.z;

        QVector<double> vxAB, vyAB, vzAB;
        QVector<double> vxBF, vyBF, vzBF;
        QVector<double> vxFC, vyFC, vzFC;
        QVector<double> vxCD, vyCD, vzCD;
        QVector<double> vxDE, vyDE, vzDE;
        QVector<double> vxEA, vyEA, vzEA;
        QVector<double> nxS, nyS, nzS;

        QVector<double> vxfAB, vyfAB, vzfAB;
        QVector<double> vxfBF, vyfBF, vzfBF;
        QVector<double> vxfFC, vyfFC, vzfFC;
        QVector<double> vxfCD, vyfCD, vzfCD;
        QVector<double> vxfDE, vyfDE, vzfDE;
        QVector<double> vxfEA, vyfEA, vzfEA;
        QVector<double> nxfS, nyfS, nzfS;
        //QVector<double> vnx;

        QVector<double> vzTotal;

        for(int ii = 0;ii<wingN;ii++){
            point3d a = leyBiotSavart3D(xcoloc,ycoloc,zcoloc,vortexArray[ii].xyzA.x,vortexArray[ii].xyzA.y,vortexArray[ii].xyzA.z,
                                        vortexArray[ii].xyzB.x,vortexArray[ii].xyzB.y,vortexArray[ii].xyzB.z);
            point3d b = leyBiotSavart3D(xcoloc,ycoloc,zcoloc,vortexArray[ii].xyzB.x,vortexArray[ii].xyzB.y,vortexArray[ii].xyzB.z,
                                        vortexArray[ii].xyzF.x,vortexArray[ii].xyzF.y,vortexArray[ii].xyzF.z);
            point3d c = leyBiotSavart3D(xcoloc,ycoloc,zcoloc,vortexArray[ii].xyzF.x,vortexArray[ii].xyzF.y,vortexArray[ii].xyzF.z,
                                        vortexArray[ii].xyzC.x,vortexArray[ii].xyzC.y,vortexArray[ii].xyzC.z);
            point3d d = leyBiotSavart3D(xcoloc,ycoloc,zcoloc,vortexArray[ii].xyzC.x,vortexArray[ii].xyzC.y,vortexArray[ii].xyzC.z,
                                        vortexArray[ii].xyzD.x,vortexArray[ii].xyzD.y,vortexArray[ii].xyzD.z);
            point3d e = leyBiotSavart3D(xcoloc,ycoloc,zcoloc,vortexArray[ii].xyzD.x,vortexArray[ii].xyzD.y,vortexArray[ii].xyzD.z,
                                        vortexArray[ii].xyzE.x,vortexArray[ii].xyzE.y,vortexArray[ii].xyzE.z);
            point3d f = leyBiotSavart3D(xcoloc,ycoloc,zcoloc,vortexArray[ii].xyzE.x,vortexArray[ii].xyzE.y,vortexArray[ii].xyzE.z,
                                        vortexArray[ii].xyzA.x,vortexArray[ii].xyzA.y,vortexArray[ii].xyzA.z);
            point3d a1 = leyBiotSavart3D(xfcoloc,yfcoloc,zfcoloc,vortexArray[ii].xyzA.x,vortexArray[ii].xyzA.y,vortexArray[ii].xyzA.z,
                                        vortexArray[ii].xyzB.x,vortexArray[ii].xyzB.y,vortexArray[ii].xyzB.z);
            point3d b1 = leyBiotSavart3D(xfcoloc,yfcoloc,zfcoloc,vortexArray[ii].xyzB.x,vortexArray[ii].xyzB.y,vortexArray[ii].xyzB.z,
                                        vortexArray[ii].xyzF.x,vortexArray[ii].xyzF.y,vortexArray[ii].xyzF.z);
            point3d c1 = leyBiotSavart3D(xfcoloc,yfcoloc,zfcoloc,vortexArray[ii].xyzF.x,vortexArray[ii].xyzF.y,vortexArray[ii].xyzF.z,
                                        vortexArray[ii].xyzC.x,vortexArray[ii].xyzC.y,vortexArray[ii].xyzC.z);
            point3d d1 = leyBiotSavart3D(xfcoloc,yfcoloc,zfcoloc,vortexArray[ii].xyzC.x,vortexArray[ii].xyzC.y,vortexArray[ii].xyzC.z,
                                        vortexArray[ii].xyzD.x,vortexArray[ii].xyzD.y,vortexArray[ii].xyzD.z);
            point3d e1 = leyBiotSavart3D(xfcoloc,yfcoloc,zfcoloc,vortexArray[ii].xyzD.x,vortexArray[ii].xyzD.y,vortexArray[ii].xyzD.z,
                                        vortexArray[ii].xyzE.x,vortexArray[ii].xyzE.y,vortexArray[ii].xyzE.z);
            point3d f1 = leyBiotSavart3D(xfcoloc,yfcoloc,zfcoloc,vortexArray[ii].xyzE.x,vortexArray[ii].xyzE.y,vortexArray[ii].xyzE.z,
                                        vortexArray[ii].xyzA.x,vortexArray[ii].xyzA.y,vortexArray[ii].xyzA.z);

            vxAB.append(a.x);vxBF.append(b.x);vxFC.append(c.x);vxCD.append(d.x);vxDE.append(e.x);vxEA.append(f.x);
            vyAB.append(a.y);vyBF.append(b.y);vyFC.append(c.y);vyCD.append(d.y);vyDE.append(e.y);vyEA.append(f.y);
            vzAB.append(a.z);vzBF.append(b.z);vzFC.append(c.z);vzCD.append(d.z);vzDE.append(e.z);vzEA.append(f.z);
            vxfAB.append(a1.x);vxfBF.append(b1.x);vxfFC.append(c1.x);vxfCD.append(d1.x);vxfDE.append(e1.x);vxfEA.append(f1.x);
            vyfAB.append(a1.y);vyfBF.append(b1.y);vyfFC.append(c1.y);vyfCD.append(d1.y);vyfDE.append(e1.y);vyfEA.append(f1.y);
            vzfAB.append(a1.z);vzfBF.append(b1.z);vzfFC.append(c1.z);vzfCD.append(d1.z);vzfDE.append(e1.z);vzfEA.append(f1.z);
        }

        for (int k = 0; k < wingN; k++) {
            nxS.append(vxAB[k] + vxBF[k] + vxFC[k] + vxCD[k] + vxDE[k] + vxEA[k]);
            nyS.append(vyAB[k] + vyBF[k] + vyFC[k] + vyCD[k] + vyDE[k] + vyEA[k]);
            nzS.append(vzAB[k] + vzBF[k] + vzFC[k] + vzCD[k] + vzDE[k] + vzEA[k]);

            nxfS.append(vxfAB[k] + vxfBF[k] + vxfFC[k] + vxfCD[k] + vxfDE[k] + vxfEA[k]);
            nyfS.append(vyfAB[k] + vyfBF[k] + vyfFC[k] + vyfCD[k] + vyfDE[k] + vyfEA[k]);
            nzfS.append(vzfAB[k] + vzfBF[k] + vzfFC[k] + vzfCD[k] + vzfDE[k] + vzfEA[k]);
        }

        for (int k = 0; k < wingN; k++) {
            point3d b1(vortexArray[i].nControl.x, vortexArray[i].nControl.y, vortexArray[i].nControl.z);
            point3d a1(nxS[k], nyS[k], nzS[k]);
            vzTotal.append(computeNormalInfluences(a1, b1));
        }
        //填充矩阵方便后期赋值
        for(int k = 0;k < wakeN;k++){
            vzTotal.append(0.0);
            nxfS.append(0.0);
            nyfS.append(0.0);
            nzfS.append(0.0);
        }

        bx.append(nxfS);
        by.append(nyfS);
        bz.append(nzfS);

        vnTotal.append(vzTotal);


    }

}
void airplaneVLM::computeWakeFluenceCoefficient(QVector<QVector<double>>&vnTotal,QVector<QVector<double>>&bx,QVector<QVector<double>>&by,QVector<QVector<double>>&bz){
    for(int i = 0;i < alphaArray.length();i++){
        for(int ii = 0;ii<wingN;ii++){
            double xcoloc = vortexArray[ii].xyzControl.x;
            double ycoloc = vortexArray[ii].xyzControl.y;
            double zcoloc = vortexArray[ii].xyzControl.z;

            double xfcoloc = vortexArray[ii].xyzCenter.x;
            double yfcoloc = vortexArray[ii].xyzCenter.y;
            double zfcoloc = vortexArray[ii].xyzCenter.z;

            QVector<double> vxAB, vyAB, vzAB;
            QVector<double> vxBF, vyBF, vzBF;
            QVector<double> vxFC, vyFC, vzFC;
            QVector<double> vxCD, vyCD, vzCD;
            QVector<double> vxDE, vyDE, vzDE;
            QVector<double> vxEA, vyEA, vzEA;
            QVector<double> nxS, nyS, nzS;

            QVector<double> vxfAB, vyfAB, vzfAB;
            QVector<double> vxfBF, vyfBF, vzfBF;
            QVector<double> vxfFC, vyfFC, vzfFC;
            QVector<double> vxfCD, vyfCD, vzfCD;
            QVector<double> vxfDE, vyfDE, vzfDE;
            QVector<double> vxfEA, vyfEA, vzfEA;
            QVector<double> nxfS, nyfS, nzfS;
            QVector<double> vzTotal;
            for(int jj = 0;jj<wakeN;jj++){
                int tmp = jj + i * wakeN;
                //int tmp = jj;
                point3d a = leyBiotSavart3D(xcoloc,ycoloc,zcoloc,wakeVortexArray[tmp].xyzA.x,wakeVortexArray[tmp].xyzA.y,wakeVortexArray[tmp].xyzA.z,
                                            wakeVortexArray[tmp].xyzB.x,wakeVortexArray[tmp].xyzB.y,wakeVortexArray[tmp].xyzB.z);
                point3d b = leyBiotSavart3D(xcoloc,ycoloc,zcoloc,wakeVortexArray[tmp].xyzB.x,wakeVortexArray[tmp].xyzB.y,wakeVortexArray[tmp].xyzB.z,
                                            wakeVortexArray[tmp].xyzF.x,wakeVortexArray[tmp].xyzF.y,wakeVortexArray[tmp].xyzF.z);
                point3d c = leyBiotSavart3D(xcoloc,ycoloc,zcoloc,wakeVortexArray[tmp].xyzF.x,wakeVortexArray[tmp].xyzF.y,wakeVortexArray[tmp].xyzF.z,
                                            wakeVortexArray[tmp].xyzC.x,wakeVortexArray[tmp].xyzC.y,wakeVortexArray[tmp].xyzC.z);
                point3d d = leyBiotSavart3D(xcoloc,ycoloc,zcoloc,wakeVortexArray[tmp].xyzC.x,wakeVortexArray[tmp].xyzC.y,wakeVortexArray[tmp].xyzC.z,
                                            wakeVortexArray[tmp].xyzD.x,wakeVortexArray[tmp].xyzD.y,wakeVortexArray[tmp].xyzD.z);
                point3d e = leyBiotSavart3D(xcoloc,ycoloc,zcoloc,wakeVortexArray[tmp].xyzD.x,wakeVortexArray[tmp].xyzD.y,wakeVortexArray[tmp].xyzD.z,
                                            wakeVortexArray[tmp].xyzE.x,wakeVortexArray[tmp].xyzE.y,wakeVortexArray[tmp].xyzE.z);
                point3d f = leyBiotSavart3D(xcoloc,ycoloc,zcoloc,wakeVortexArray[tmp].xyzE.x,wakeVortexArray[tmp].xyzE.y,wakeVortexArray[tmp].xyzE.z,
                                            wakeVortexArray[tmp].xyzA.x,wakeVortexArray[tmp].xyzA.y,wakeVortexArray[tmp].xyzA.z);
                point3d a1 = leyBiotSavart3D(xfcoloc,yfcoloc,zfcoloc,wakeVortexArray[tmp].xyzA.x,wakeVortexArray[tmp].xyzA.y,wakeVortexArray[tmp].xyzA.z,
                                            wakeVortexArray[tmp].xyzB.x,wakeVortexArray[tmp].xyzB.y,wakeVortexArray[tmp].xyzB.z);
                point3d b1 = leyBiotSavart3D(xfcoloc,yfcoloc,zfcoloc,wakeVortexArray[tmp].xyzB.x,wakeVortexArray[tmp].xyzB.y,wakeVortexArray[tmp].xyzB.z,
                                            wakeVortexArray[tmp].xyzF.x,wakeVortexArray[tmp].xyzF.y,wakeVortexArray[tmp].xyzF.z);
                point3d c1 = leyBiotSavart3D(xfcoloc,yfcoloc,zfcoloc,wakeVortexArray[tmp].xyzF.x,wakeVortexArray[tmp].xyzF.y,wakeVortexArray[tmp].xyzF.z,
                                            wakeVortexArray[tmp].xyzC.x,wakeVortexArray[tmp].xyzC.y,wakeVortexArray[tmp].xyzC.z);
                point3d d1 = leyBiotSavart3D(xfcoloc,yfcoloc,zfcoloc,wakeVortexArray[tmp].xyzC.x,wakeVortexArray[tmp].xyzC.y,wakeVortexArray[tmp].xyzC.z,
                                            wakeVortexArray[tmp].xyzD.x,wakeVortexArray[tmp].xyzD.y,wakeVortexArray[tmp].xyzD.z);
                point3d e1 = leyBiotSavart3D(xfcoloc,yfcoloc,zfcoloc,wakeVortexArray[tmp].xyzD.x,wakeVortexArray[tmp].xyzD.y,wakeVortexArray[tmp].xyzD.z,
                                            wakeVortexArray[tmp].xyzE.x,wakeVortexArray[tmp].xyzE.y,wakeVortexArray[tmp].xyzE.z);
                point3d f1 = leyBiotSavart3D(xfcoloc,yfcoloc,zfcoloc,wakeVortexArray[tmp].xyzE.x,wakeVortexArray[tmp].xyzE.y,wakeVortexArray[tmp].xyzE.z,
                                            wakeVortexArray[tmp].xyzA.x,wakeVortexArray[tmp].xyzA.y,wakeVortexArray[tmp].xyzA.z);

                vxAB.append(a.x);vxBF.append(b.x);vxFC.append(c.x);vxCD.append(d.x);vxDE.append(e.x);vxEA.append(f.x);
                vyAB.append(a.y);vyBF.append(b.y);vyFC.append(c.y);vyCD.append(d.y);vyDE.append(e.y);vyEA.append(f.y);
                vzAB.append(a.z);vzBF.append(b.z);vzFC.append(c.z);vzCD.append(d.z);vzDE.append(e.z);vzEA.append(f.z);
                vxfAB.append(a1.x);vxfBF.append(b1.x);vxfFC.append(c1.x);vxfCD.append(d1.x);vxfDE.append(e1.x);vxfEA.append(f1.x);
                vyfAB.append(a1.y);vyfBF.append(b1.y);vyfFC.append(c1.y);vyfCD.append(d1.y);vyfDE.append(e1.y);vyfEA.append(f1.y);
                vzfAB.append(a1.z);vzfBF.append(b1.z);vzfFC.append(c1.z);vzfCD.append(d1.z);vzfDE.append(e1.z);vzfEA.append(f1.z);
            }

            for (int kk = 0; kk < wakeN; kk++) {
                nxS.append(vxAB[kk] + vxBF[kk] + vxFC[kk] + vxCD[kk] + vxDE[kk] + vxEA[kk]);
                nyS.append(vyAB[kk] + vyBF[kk] + vyFC[kk] + vyCD[kk] + vyDE[kk] + vyEA[kk]);
                nzS.append(vzAB[kk] + vzBF[kk] + vzFC[kk] + vzCD[kk] + vzDE[kk] + vzEA[kk]);

                nxfS.append(vxfAB[kk] + vxfBF[kk] + vxfFC[kk] + vxfCD[kk] + vxfDE[kk] + vxfEA[kk]);
                nyfS.append(vyfAB[kk] + vyfBF[kk] + vyfFC[kk] + vyfCD[kk] + vyfDE[kk] + vyfEA[kk]);
                nzfS.append(vzfAB[kk] + vzfBF[kk] + vzfFC[kk] + vzfCD[kk] + vzfDE[kk] + vzfEA[kk]);
            }

            for (int kk = 0; kk < wakeN; kk++) {
                point3d b1(vortexArray[ii].nControl.x, vortexArray[ii].nControl.y, vortexArray[ii].nControl.z);
                point3d a1(nxS[kk], nyS[kk], nzS[kk]);
                vzTotal.append(computeNormalInfluences(a1, b1));
            }


            bx.append(nxfS);
            by.append(nyfS);
            bz.append(nzfS);

            vnTotal.append(vzTotal);

        }
    }

}
void airplaneVLM::computeTrefftzFluenceCoefficient(QVector<QVector<double>> &vnTotal){
    for(int j = 0;j<wakeN;j++){
        double xcoloc = trefftzArray[j].xyzCenter.x;
        double ycoloc = trefftzArray[j].xyzCenter.y;
        double zcoloc = trefftzArray[j].xyzCenter.z;
        QVector<double> vxAB, vyAB, vzAB;
        QVector<double> vxBF, vyBF, vzBF;
        QVector<double> vxFC, vyFC, vzFC;
        QVector<double> vxCD, vyCD, vzCD;
        QVector<double> vxDE, vyDE, vzDE;
        QVector<double> vxEA, vyEA, vzEA;
        QVector<double> nxS, nyS, nzS;
        QVector<double> vzTotal;
        for(int k = 0;k<wakeN;k++){
            int tmp = k ;
            point3d a = leyBiotSavart3D(xcoloc,ycoloc,zcoloc,wakeVortexArray[tmp].xyzA.x,wakeVortexArray[tmp].xyzA.y,wakeVortexArray[tmp].xyzA.z,
                                        wakeVortexArray[tmp].xyzB.x,wakeVortexArray[tmp].xyzB.y,wakeVortexArray[tmp].xyzB.z);
            point3d b = leyBiotSavart3D(xcoloc,ycoloc,zcoloc,wakeVortexArray[tmp].xyzB.x,wakeVortexArray[tmp].xyzB.y,wakeVortexArray[tmp].xyzB.z,
                                        wakeVortexArray[tmp].xyzF.x,wakeVortexArray[tmp].xyzF.y,wakeVortexArray[tmp].xyzF.z);
            point3d c = leyBiotSavart3D(xcoloc,ycoloc,zcoloc,wakeVortexArray[tmp].xyzF.x,wakeVortexArray[tmp].xyzF.y,wakeVortexArray[tmp].xyzF.z,
                                        wakeVortexArray[tmp].xyzC.x,wakeVortexArray[tmp].xyzC.y,wakeVortexArray[tmp].xyzC.z);
            point3d d = leyBiotSavart3D(xcoloc,ycoloc,zcoloc,wakeVortexArray[tmp].xyzC.x,wakeVortexArray[tmp].xyzC.y,wakeVortexArray[tmp].xyzC.z,
                                        wakeVortexArray[tmp].xyzD.x,wakeVortexArray[tmp].xyzD.y,wakeVortexArray[tmp].xyzD.z);
            point3d e = leyBiotSavart3D(xcoloc,ycoloc,zcoloc,wakeVortexArray[tmp].xyzD.x,wakeVortexArray[tmp].xyzD.y,wakeVortexArray[tmp].xyzD.z,
                                        wakeVortexArray[tmp].xyzE.x,wakeVortexArray[tmp].xyzE.y,wakeVortexArray[tmp].xyzE.z);
            point3d f = leyBiotSavart3D(xcoloc,ycoloc,zcoloc,wakeVortexArray[tmp].xyzE.x,wakeVortexArray[tmp].xyzE.y,wakeVortexArray[tmp].xyzE.z,
                                        wakeVortexArray[tmp].xyzA.x,wakeVortexArray[tmp].xyzA.y,wakeVortexArray[tmp].xyzA.z);

            vxAB.append(a.x);vxBF.append(b.x);vxFC.append(c.x);vxCD.append(d.x);vxDE.append(e.x);vxEA.append(f.x);
            vyAB.append(a.y);vyBF.append(b.y);vyFC.append(c.y);vyCD.append(d.y);vyDE.append(e.y);vyEA.append(f.y);
            vzAB.append(a.z);vzBF.append(b.z);vzFC.append(c.z);vzCD.append(d.z);vzDE.append(e.z);vzEA.append(f.z);

        }

        for (int kk = 0; kk < wakeN; kk++) {
            nxS.append(vxAB[kk] + vxBF[kk] + vxFC[kk] + vxCD[kk] + vxDE[kk] + vxEA[kk]);
            nyS.append(vyAB[kk] + vyBF[kk] + vyFC[kk] + vyCD[kk] + vyDE[kk] + vyEA[kk]);
            nzS.append(vzAB[kk] + vzBF[kk] + vzFC[kk] + vzCD[kk] + vzDE[kk] + vzEA[kk]);
        }

        for (int kk = 0; kk < wakeN; kk++) {
            point3d b1(trefftzArray[j].nCenter.x, trefftzArray[j].nCenter.y, trefftzArray[j].nCenter.z);
            point3d a1(nxS[kk], nyS[kk], nzS[kk]);
            vzTotal.append(computeNormalInfluences(a1, b1));
        }

        vnTotal.append(vzTotal);

    }
}

void airplaneVLM::computePressure(){
    QVector<double>minGammaArray;
    QVector<double>maxGammaArray;

    for(int i = 0;i<alphaArray.length();i++){
        QVector<double>tmpGamma;
        if(withWing){
            for(int j = 0;j<wingModel->getMeshNum();j++){
                tmpGamma.append(wingModel->realGammaArray[i][j]);
            }
        }
        if(withHTail){
            for(int j = 0;j<hTailModel->getMeshNum();j++){
                tmpGamma.append(hTailModel->realGammaArray[i][j]);
            }
        }
        if(withVTail){
            for(int j = 0;j<vTailModel->getMeshNum();j++){
                tmpGamma.append(vTailModel->realGammaArray[i][j]);
            }
        }
        minGammaArray.append(mathSolver.minV(tmpGamma));
        maxGammaArray.append(mathSolver.maxV(tmpGamma));
    }

    if(withWing)
        wingModel->computePressure(minGammaArray,maxGammaArray);
    if(withHTail)
        hTailModel->computePressure(minGammaArray,maxGammaArray);
    if(withVTail)
        vTailModel->computePressure(minGammaArray,maxGammaArray);


}
void airplaneVLM::solveStreamLine(const int index, const double distance,const double seedHeight,const double t,const double dt){
    streamIndex = index;
    QVector<point3d>position;

    int Ny1 = wingModel->getNyNum();
    int Ny2 = hTailModel->getNyNum();
    int Ny3 = vTailModel->getNyNum();
    int len = 0;

    if(withWing){
        for(int i = 0;i<Ny1;i++){
            position.append(wakeVortexArray[i].xyzA);
        }
        position.append(wakeVortexArray[Ny1 - 1].xyzB);
        len += Ny1 + 1;
    }
    if(withHTail){
        for(int i = 0;i<Ny2;i++){
            position.append(wakeVortexArray[i + Ny1].xyzA);
        }
        position.append(wakeVortexArray[Ny1 + Ny2 - 1].xyzB);
        len += Ny2 + 1;
    }
    if(withVTail){
        for(int i = 0;i<Ny3;i++){
            position.append(wakeVortexArray[i + Ny1 + Ny2].xyzA);
        }
        position.append(wakeVortexArray[wakeN - 1].xyzB);
        len += Ny3 + 1;
    }


    QVector<QVector<point3d>>tmp;

    // 2. 步长放大系数：根据需求调整（建议1.05~1.2，值越大步长增长越快）
    const double s_XFactor = 1.1;
    // 步长上限：避免后期步长过大导致精度丢失（可根据流场范围调整）
    const double maxDeltaTime = dt * 20;


    for(int i = 0;i<len;i++){

        int value = double(i)/ len * 100;
        emit progressUpdated(value);


        double deltaTime = dt;
        double totalTime = t;
        double currentTime = 0.0;
        QVector<point3d>streamTmp;
        point3d currentPosition(position[i].x - distance,position[i].y,position[i].z + seedHeight);

        streamTmp.append(currentPosition);




        while(currentTime < totalTime){

            // 防止步长过大：超过上限后固定为最大值
            if (deltaTime > maxDeltaTime) {
                deltaTime = maxDeltaTime;
            }
            // 避免最后一步超出总时间（比如剩余时间不足一个步长时，用剩余时间作为步长）
            if (currentTime + deltaTime > t) {
                deltaTime = t - currentTime;
            }

            point3d nextPosition = eulerMethod(currentPosition,deltaTime);

            streamTmp.append(nextPosition);
            currentPosition = nextPosition;
            currentTime += deltaTime;

            // 4. 关键：每次迭代后放大步长（远离机翼后步长增大）
            deltaTime *= s_XFactor;
        }
        tmp.append(streamTmp);
    }
    emit workFinished();
    streamLineArray = tmp;

}
point3d airplaneVLM::calculateVelocity(const point3d& position){


    QVector<double> vxAB, vyAB, vzAB;
    QVector<double> vxBF, vyBF, vzBF;
    QVector<double> vxFC, vyFC, vzFC;
    QVector<double> vxCD, vyCD, vzCD;
    QVector<double> vxDE, vyDE, vzDE;
    QVector<double> vxEA, vyEA, vzEA;
    for(int i = 0;i < wingN;i++){
        point3d a = leyBiotSavart3D(position.x,position.y,position.z,vortexArray[i].xyzA.x,vortexArray[i].xyzA.y,vortexArray[i].xyzA.z,
                                    vortexArray[i].xyzB.x,vortexArray[i].xyzB.y,vortexArray[i].xyzB.z);
        point3d b = leyBiotSavart3D(position.x,position.y,position.z,vortexArray[i].xyzB.x,vortexArray[i].xyzB.y,vortexArray[i].xyzB.z,
                                    vortexArray[i].xyzF.x,vortexArray[i].xyzF.y,vortexArray[i].xyzF.z);
        point3d c = leyBiotSavart3D(position.x,position.y,position.z,vortexArray[i].xyzF.x,vortexArray[i].xyzF.y,vortexArray[i].xyzF.z,
                                    vortexArray[i].xyzC.x,vortexArray[i].xyzC.y,vortexArray[i].xyzC.z);
        point3d d = leyBiotSavart3D(position.x,position.y,position.z,vortexArray[i].xyzC.x,vortexArray[i].xyzC.y,vortexArray[i].xyzC.z,
                                    vortexArray[i].xyzD.x,vortexArray[i].xyzD.y,vortexArray[i].xyzD.z);
        point3d e = leyBiotSavart3D(position.x,position.y,position.z,vortexArray[i].xyzD.x,vortexArray[i].xyzD.y,vortexArray[i].xyzD.z,
                                    vortexArray[i].xyzE.x,vortexArray[i].xyzE.y,vortexArray[i].xyzE.z);
        point3d f = leyBiotSavart3D(position.x,position.y,position.z,vortexArray[i].xyzE.x,vortexArray[i].xyzE.y,vortexArray[i].xyzE.z,
                                    vortexArray[i].xyzA.x,vortexArray[i].xyzA.y,vortexArray[i].xyzA.z);
        vxAB.append(a.x);vxBF.append(b.x);vxFC.append(c.x);vxCD.append(d.x);vxDE.append(e.x);vxEA.append(f.x);
        vyAB.append(a.y);vyBF.append(b.y);vyFC.append(c.y);vyCD.append(d.y);vyDE.append(e.y);vyEA.append(f.y);
        vzAB.append(a.z);vzBF.append(b.z);vzFC.append(c.z);vzCD.append(d.z);vzDE.append(e.z);vzEA.append(f.z);

    }
    int tmp = streamIndex * wakeN;
    for(int i = tmp;i < tmp + wakeN;i++){
        point3d a = leyBiotSavart3D(position.x,position.y,position.z,wakeVortexArray[i].xyzA.x,wakeVortexArray[i].xyzA.y,wakeVortexArray[i].xyzA.z,
                                    wakeVortexArray[i].xyzB.x,wakeVortexArray[i].xyzB.y,wakeVortexArray[i].xyzB.z);
        point3d b = leyBiotSavart3D(position.x,position.y,position.z,wakeVortexArray[i].xyzB.x,wakeVortexArray[i].xyzB.y,wakeVortexArray[i].xyzB.z,
                                    wakeVortexArray[i].xyzF.x,wakeVortexArray[i].xyzF.y,wakeVortexArray[i].xyzF.z);
        point3d c = leyBiotSavart3D(position.x,position.y,position.z,wakeVortexArray[i].xyzF.x,wakeVortexArray[i].xyzF.y,wakeVortexArray[i].xyzF.z,
                                    wakeVortexArray[i].xyzC.x,wakeVortexArray[i].xyzC.y,wakeVortexArray[i].xyzC.z);
        point3d d = leyBiotSavart3D(position.x,position.y,position.z,wakeVortexArray[i].xyzC.x,wakeVortexArray[i].xyzC.y,wakeVortexArray[i].xyzC.z,
                                    wakeVortexArray[i].xyzD.x,wakeVortexArray[i].xyzD.y,wakeVortexArray[i].xyzD.z);
        point3d e = leyBiotSavart3D(position.x,position.y,position.z,wakeVortexArray[i].xyzD.x,wakeVortexArray[i].xyzD.y,wakeVortexArray[i].xyzD.z,
                                    wakeVortexArray[i].xyzE.x,wakeVortexArray[i].xyzE.y,wakeVortexArray[i].xyzE.z);
        point3d f = leyBiotSavart3D(position.x,position.y,position.z,wakeVortexArray[i].xyzE.x,wakeVortexArray[i].xyzE.y,wakeVortexArray[i].xyzE.z,
                                    wakeVortexArray[i].xyzA.x,wakeVortexArray[i].xyzA.y,wakeVortexArray[i].xyzA.z);
        vxAB.append(a.x);vxBF.append(b.x);vxFC.append(c.x);vxCD.append(d.x);vxDE.append(e.x);vxEA.append(f.x);
        vyAB.append(a.y);vyBF.append(b.y);vyFC.append(c.y);vyCD.append(d.y);vyDE.append(e.y);vyEA.append(f.y);
        vzAB.append(a.z);vzBF.append(b.z);vzFC.append(c.z);vzCD.append(d.z);vzDE.append(e.z);vzEA.append(f.z);

    }

    QVector<double>vx;QVector<double>vy;QVector<double>vz;
    for(int i = 0;i<wingN + wakeN;i++){
        vx.append(vxAB[i] + vxBF[i] + vxFC[i] + vxCD[i] + vxDE[i] + vxEA[i]);
        vy.append(vyAB[i] + vyBF[i] + vyFC[i] + vyCD[i] + vyDE[i] + vyEA[i]);
        vz.append(vzAB[i] + vzBF[i] + vzFC[i] + vzCD[i] + vzDE[i] + vzEA[i]);

    }

    double vxt = 0;
    double vyt = 0;
    double vzt = 0;

    for(int i = 0;i<wingN;i++){
        vxt = vxt + vx[i] * matrixGammaArray[streamIndex][i];
        vyt = vyt + vy[i] * matrixGammaArray[streamIndex][i];
        vzt = vzt + vz[i] * matrixGammaArray[streamIndex][i];
    }

    for(int i = wingN - wakeN;i<wingN;i++){
        vxt = vxt + vx[i + wakeN] * matrixGammaArray[streamIndex][i];
        vyt = vyt + vy[i + wakeN] * matrixGammaArray[streamIndex][i];
        vzt = vzt + vz[i + wakeN] * matrixGammaArray[streamIndex][i];
    }

    return point3d(vxt + vinfArray[streamIndex].x,vyt + vinfArray[streamIndex].y,vzt + vinfArray[streamIndex].z);
}
point3d airplaneVLM::eulerMethod(const point3d& position, const double deltaTime) {
    // 1. 计算当前位置的速度向量（与RK4的第一步相同）
    point3d velocity = calculateVelocity(position);

    // 2. 欧拉法核心：当前位置 + 速度×步长（直接累加增量）
    double x1 = position.x + velocity.x * deltaTime;
    double y1 = position.y + velocity.y * deltaTime;
    double z1 = position.z + velocity.z * deltaTime;

    return point3d(x1, y1, z1);
}
void airplaneVLM::updateStripForce(){

}
void airplaneVLM::clearAllValue(){
    if(!wakeVortexArray.isEmpty()){
        wakeVortexArray.clear();
        vortexArray.clear();
        trefftzArray.clear();
        vNArray.clear();
        vinxArray.clear();
        vinyArray.clear();
        vinxArray.clear();

    }


}
point3d airplaneVLM::leyBiotSavart3D(
    const double xr, const double yr, const double zr,  // 控制点坐标
    const double xP, const double yP, const double zP,  // 涡段起点P
    const double xQ, const double yQ, const double zQ  // 涡段终点Q

) {

    bool finite_core = false; // 新增：是否启用有限涡核（对应bound_induced_velocity的finite_core）
    double core_size = 0.001;   // 新增：涡核半径（对应bound_induced_velocity的core_size）

    double vdwX = 0.0, vdwY = 0.0, vdwZ = 0.0;

    // 1. 计算控制点到涡段两端的向量 r1（P→控制点）、r2（Q→控制点）
    double r1x = xr - xP;
    double r1y = yr - yP;
    double r1z = zr - zP;

    double r2x = xr - xQ;
    double r2y = yr - yQ;
    double r2z = zr - zQ;

    // 2. 计算 r1、r2 的模长（避免重复计算）
    double Lr1 = sqrt(r1x * r1x + r1y * r1y + r1z * r1z);
    double Lr2 = sqrt(r2x * r2x + r2y * r2y + r2z * r2z);

    // 3. 核心：按 bound_induced_velocity 思想，分“有限涡核”和“点涡”计算诱导速度
    if (finite_core) {
        // -------------------------- 有限涡核模式（对应bound_induced_velocity的finite_core=true）--------------------------
        // 计算 bound_induced_velocity 中的关键变量：r1s(r1模长平方)、r2s(r2模长平方)、rdot(r1·r2)
        double r1s = Lr1 * Lr1;
        double r2s = Lr2 * Lr2;
        double rdot = r1x * r2x + r1y * r2y + r1z * r2z;  // r1与r2的点积
        double eps_s = core_size * core_size;  // 涡核半径平方（对应bound_induced_velocity的εs）

        // 计算 f1：对应bound_induced_velocity的f1（叉积/修正分母）
        // f1 = cross(r1, r2) / [r1s*r2s - rdot² + eps_s*(r1s + r2s - 2*Lr1*Lr2)]
        double F1x = r1y * r2z - r1z * r2y;  // r1×r2 的x分量
        double F1y = r1z * r2x - r1x * r2z;  // r1×r2 的y分量
        double F1z = r1x * r2y - r1y * r2x;  // r1×r2 的z分量

        double denom_f1 = r1s * r2s - rdot * rdot + eps_s * (r1s + r2s - 2 * Lr1 * Lr2);
        if (fabs(denom_f1) < 1e-12) {  // 极端情况分母趋近于0（理论上因eps_s存在不会发生）
            return point3d(0.0, 0.0, 0.0);
        }
        double f1x = F1x / denom_f1;
        double f1y = F1y / denom_f1;
        double f1z = F1z / denom_f1;

        // 计算 f2：对应bound_induced_velocity的f2（距离衰减项）
        // f2 = (r1s - rdot)/sqrt(r1s + eps_s) + (r2s - rdot)/sqrt(r2s + eps_s)
        double term1 = (r1s - rdot) / sqrt(r1s + eps_s);
        double term2 = (r2s - rdot) / sqrt(r2s + eps_s);
        double f2 = term1 + term2;

        // 计算最终诱导速度：Vhat = (f1 * f2) / (4π)（对应bound_induced_velocity的核心公式）
        const double pi = M_PI;
        vdwX = (f1x * f2) / (4 * pi);
        vdwY = (f1y * f2) / (4 * pi);
        vdwZ = (f1z * f2) / (4 * pi);

    } else {
        // -------------------------- 点涡模式（对应bound_induced_velocity的finite_core=false）--------------------------
        // 公式：f1 = cross(r1, r2)/(Lr1*Lr2 + rdot)，f2 = (1/Lr1 + 1/Lr2)，Vhat = (f1*f2)/(4π)
        double F1x = r1y * r2z - r1z * r2y;  // r1×r2 的x分量
        double F1y = r1z * r2x - r1x * r2z;  // r1×r2 的y分量
        double F1z = r1x * r2y - r1y * r2x;  // r1×r2 的z分量

        double denom_f1 = Lr1 * Lr2 + (r1x * r2x + r1y * r2y + r1z * r2z);  // Lr1*Lr2 + rdot
        if (fabs(denom_f1) < 1e-12) {  // 点涡模式下需避免分母为0（奇异性未修正）
            return point3d(0.0, 0.0, 0.0);
        }
        double f1x = F1x / denom_f1;
        double f1y = F1y / denom_f1;
        double f1z = F1z / denom_f1;

        double f2 = (1.0 / Lr1) + (1.0 / Lr2);  // 点涡模式的距离衰减项

        // 计算最终诱导速度
        const double pi = M_PI;
        vdwX = (f1x * f2) / (4 * pi);
        vdwY = (f1y * f2) / (4 * pi);
        vdwZ = (f1z * f2) / (4 * pi);
    }

    return point3d(vdwX, vdwY, vdwZ);
}
/*
point3d airplaneVLM::leyBiotSavart3D(const double xr, const double yr, const double zr, const double xP, const double yP, const double zP, const double xQ, const double yQ, const double zQ){
    double hl_limit = 1e-7; // 数值稳定性参数，类似MATLAB的hl_limit

    double vdwX,vdwY,vdwZ;


    // 计算 r1 和 r2 向量
    double r1x = xr - xP;
    double r1y = yr - yP;
    double r1z = zr - zP;

    double r2x = xr - xQ;
    double r2y = yr - yQ;
    double r2z = zr - zQ;

    // 计算 r1 和 r2 的模长
    double Lr1 = sqrt(r1x * r1x + r1y * r1y + r1z * r1z);
    double Lr2 = sqrt(r2x * r2x + r2y * r2y + r2z * r2z);

    // 单位化 r1 和 r2 向量
    double R1x = r1x / Lr1;
    double R1y = r1y / Lr1;
    double R1z = r1z / Lr1;

    double R2x = r2x / Lr2;
    double R2y = r2y / Lr2;
    double R2z = r2z / Lr2;

    // 计算 L1 = R1 - R2
    double L1x = R1x - R2x;
    double L1y = R1y - R2y;
    double L1z = R1z - R2z;

    // 计算 F1 = r1 × r2 (叉积)
    double F1x = r1y * r2z - r1z * r2y;
    double F1y = r1z * r2x - r1x * r2z;
    double F1z = r1x * r2y - r1y * r2x;

    // 计算 F1 的模长
    double LF1 = F1x * F1x + F1y * F1y + F1z * F1z;

    // 计算 r1 - r2
    double R0x = r1x - r2x;
    double R0y = r1y - r2y;
    double R0z = r1z - r2z;

    // 计算涡段长度 LR0
    double LR0 = sqrt(R0x * R0x + R0y * R0y + R0z * R0z);

    // 判断是否涡段长度为0
    bool SW_l0 = (LR0 == 1e-12);

    // 计算点到直线距离 Lrp
    double Lrp = sqrt(LF1) / LR0;
    double Ratio_hl = Lrp / LR0;

    // 判断是否满足hl_limit，避免数值奇异性
    bool SW_hl_limit = (Ratio_hl < hl_limit);

    // 对hl_limit和涡段长度为0的情况进行处理
    if(SW_hl_limit || SW_l0)
        return point3d(0,0,0);

    // 计算 F2 = F1 / LF1
    double F2x = F1x / LF1;
    double F2y = F1y / LF1;
    double F2z = F1z / LF1;

    // 计算 L2 = R0 · L1
    double L2 = R0x * L1x + R0y * L1y + R0z * L1z;

    //
    double rc = abs(wingModel->spanW[0] * 0.001);
    double Fv = 1 - exp(-1.25463 * (Lrp * Lrp / (rc * rc)));

    // 计算最终的下洗速度 VDW
    vdwX = F2x * L2 / (4 * M_PI) * Fv;
    vdwY = F2y * L2 / (4 * M_PI) * Fv;
    vdwZ = F2z * L2 / (4 * M_PI) * Fv;



    return point3d(vdwX,vdwY,vdwZ);
}
*/
double airplaneVLM::computeNormalInfluences(const point3d &ci, const point3d &n){
    return  ci.x * n.x + ci.y * n.y + ci.z * n.z;
}
double airplaneVLM::dotProduct(const point3d a, const point3d b){
    return a.x * b.x + a.y * b.y + a.z * b.z;
}
void airplaneVLM::solveZVelocity(){
    int len = 21;
    double begin = -1;
    double end = 1;
    double step = abs(begin - end) / (len - 1);
    QVector<double>zArray;
    for(int i = 0;i<len;i++){
        point3d a(1.5,begin + (step * i),0);
        double z = calculateVelocity(a).z;
        zArray.append(z);
        //cod.append(point3d(begin + step * double(i),0,0));
    }

}
void airplaneVLM::setXfoilDrag(bool ok){
    isAddXfoilDrag = ok;
}
void airplaneVLM::setZeroLiftDrag(bool ok){
    isAddZeroLiftDrag = ok;
}
void airplaneVLM::setIsStability(bool ok){
    isStability = ok;
}
void airplaneVLM::setUseLibraries(bool tmp1){
    isUseLibraries = tmp1;
}
void airplaneVLM::setVinf(QVector<point3d>&a){
    vinfArray = a;

}
