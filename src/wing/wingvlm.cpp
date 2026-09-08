#include "wingvlm.h"
#include<QDebug>
#include <Eigen/Sparse>
#include <Eigen/Dense>
#include <QFuture>
#include <QtConcurrent/QtConcurrent>
#include "airfoil/airfoildesign.h"
#include "airfoil/airfoilsolve.h"
#include "common/mymath.h"
// M_PI IN QtConcurrent/QtConcurrent
#if defined(_MSC_VER) && (_MSC_VER >= 1600)
# pragma execution_character_set("utf-8")
#endif
wingVLM::wingVLM() {
    vinf = 20;
    designLiftForce = 60;
    height = 0;
    density = 1.225;

    xOffset = 0.0;
    yOffset = 0.0;
    zOffset = 0.0;
    yTwistAngle = 0.0;



}

void wingVLM::initialGeometry(const wingDefinition&wing){

    clearAllValue();
    /*******************初始化变量*******************/
    gridU = wing.gridU;
    gridV = wing.gridV;
    spanW = wing.spanW;
    chordLengthW = wing.chordLengthW;
    dihedralAngleW = wing.dihedralAngleW;
    twistAngleW = wing.twistAngleW;
    offsetLengthW = wing.offsetLengthW;
    airfoilArray = wing.airfoilArray;

    meshRatioX = wing.vMeshType;
    meshRatioY = wing.uMeshType;
    airfoilInputType = wing.airfoilInputType;
    airfoilCSTArray = wing.cstArray;
    if(wing.cstNum > 12)
        cstNum = wing.cstNum;

    yteType = wing.yteType;
    ctYte = wing.ctYte;
    isSymmetry = wing.isSymmetry;


    mac = wing.realChord;
    interDataArray = wing.interDataArray;





    /*******************初始化网格*******************/
    if(gridV.length() > 0){
        Ny = 0;
        Nx = gridV[0];
        num = gridU.length() - 1;//
        for(int i = 0;i<num;i++){
            Ny = gridU[num - i] + Ny;
        }
        if(isSymmetry)
            Ny = Ny * 2;
        N = Nx * Ny;

        meshNum = Ny * gridV[0];
    }
    /****************初始化涡环模型******************/
    for(int i = 0;i<N;i++){
        vortex a;
        vortexArray.append(a);
    }

    nameArray = wing.airfoilNameArray;








    if(checkGeometry()){

        /*******************计算模型网格******************/
        computeGeometry2DMesh();
        generateZCoordinates();

        if(isSymmetry){
            /*******************扭转角*********************/
            twistWing();
            /*******************上反角*********************/
            dihedralWing();
        }else{
            /*******************扭转角*********************/
            twistHalfWing();
            /*******************上反角*********************/
            dihedralHalfWing();
        }

        translationWing();//平移

        /********************生成计算网格**********************/
        generate3DMesh();

        computeVortex();//
        //generateWakeVortex();
        /********************计算面积********************/


        computeWakeVortex();//生成尾迹

        computeTrefftzVortex();

        computeArea();




    }

}

void wingVLM::solver(){
    isOptimization = false;


    emit emitProgressValue(0);


    /*******************计算影响系数***********************/

    solveInfluenceCoefficient();

    computeVLMMatrix();


    computeGamma();



    computeVelocity();

    computeTreffzDrag();


    computeForce(streamVinfArray);



    computeCm();


    computePressure();



    computeAerodynamicCoefficient();

    computeXfoilInput();




    addDrag();




}
void wingVLM::computeAerodynamicCoefficient(){
    double p = 0.5 * density * vinf * vinf * realArea;


    for(int i = 0;i<vinfArray.length();i++){

        //double CL = totalForceArray[i].z / p;
        //double CD = totalForceArray[i].x / p;
        double CM = -wingCMForce[i] / p / mac;
        // wingCL.append(CL);
        //wingCD.append(CD);
        wingCM.append(CM);

    }

    QVector<QVector<double>>yCL;
    for(int i = 0;i<vinfArray.length();i++){
        QVector<double>tmp;
        double yl = abs(2 * wingCL[i] * realArea / abs(spanW[num])/ M_PI);
        for(int j = 0;j<Ny;j++){
            double x = (yt[j] + yt[j + 1]) / 2;
            tmp.append(sqrt((1 - (x * x / (spanW[num] * spanW[num]))) * yl * yl));
        }
        yCL.append(tmp);



    }
    ideaSpanForce = yCL;
}
void wingVLM::computeAerodynamicCoefficient(double area,double meanAerodynamicChord){

    double p = 0.5 * density * vinf * vinf * area;


    for(int i = 0;i<vinfArray.length();i++){

       // double CL = totalForceArray[i].z / p;
        //double CD = totalForceArray[i].x / p;
        double CM = -wingCMForce[i] / p / meanAerodynamicChord;
        //wingCL.append(CL);
        //wingCD.append(CD);
        wingCM.append(CM);

    }

    QVector<QVector<double>>yCL;
    for(int i = 0;i<vinfArray.length();i++){
        QVector<double>tmp;
        double yl = abs(2 * wingCL[i] * realArea / abs(spanW[num])/ M_PI);
        for(int j = 0;j<Ny;j++){
            double x = (yt[j] + yt[j + 1]) / 2;
            tmp.append(sqrt((1 - (x * x / (spanW[num] * spanW[num]))) * yl * yl));
        }
        yCL.append(tmp);


    }
    ideaSpanForce = yCL;
}
void wingVLM::generate3DMesh(){

    for(int i = 0;i<Nx;i++){
        for(int j = 0;j<Ny;j++){
            meshA.append(xyvvUpper[i][j]);
            meshB.append(xyvvUpper[i][j + 1]);
            meshC.append(xyvvUpper[i + 1][j + 1]);
            meshD.append(xyvvUpper[i + 1][j]);
        }
    }
    for(int i = Nx - 1;i>= 0;i--){
        for(int j = 0;j<Ny;j++){
            meshA.append(xyvvLower[i][j]);
            meshB.append(xyvvLower[i][j + 1]);
            meshC.append(xyvvLower[i + 1][j + 1]);
            meshD.append(xyvvLower[i + 1][j]);
        }
    }

}
void wingVLM::generate3DModel(){
    QVector<QVector<point3d>> tmp1;

    for (int i = 0; i < num + 1; i++) {
        int index = Ny / 2; // 初始化index
        for (int k = 0; k < i; k++) {
            index = index - gridU[k]; // 确保index在内层循环中被正确设置
        }
        QVector<point3d> tmp;
        for (int j = Nx; j>=0; j--) {
            tmp.append(xyvvUpper[j][index]);
        }
        for (int j = 1; j < Nx + 1; j++) {
            tmp.append(xyvvLower[j][index]);
        }

        tmp1.append(tmp);
    }
    model = tmp1;



}
void wingVLM::addDrag(){


    QVector<double>dragForce;
    double zeroLiftDrag = 0;
    int realNy = 0;
    if(isAddZeroLiftDrag)
        zeroLiftDrag = viscousCd;
    if(isSymmetry)
        realNy = Ny / 2;
    else
        realNy = Ny;
    //计算网格中心的CST参数
    QVector<QVector<double>>cstTmp;

    for(int i = 0;i<realNy;i++){
        QVector<double>tmp;
        for(int j = 0;j<cstNum * 2;j++){
            tmp.append((spanCstArray[i][j] + spanCstArray[i + 1][j]) / 2);
        }

        cstTmp.append(tmp);
    }

    if(isAddXfoilDrag){
        if(hasEmptyRow()&&isUseLibraries){
            //计算沿翼展阻力系数
            for(int i = 0;i<vinfArray.length();i++){
                for(int j = 0;j<realNy;j++){

                    spanDragArray.append(getViscosityDrag(i,j));
                }
            }

        }else{
            for(int index = 0;index<vinfArray.length();index++){


                for(int i = 0;i<realNy;i++){
                    xfoilSetting tmp1;
                    double realCy = (cy[i] + cy[i + 1]) / 2;
                    tmp1.Re = int(69000 * realCy * vinf);
                    tmp1.Ma = 0;
                    tmp1.nCrit = 9;
                    tmp1.alphaStepSize = 1;
                    tmp1.minAlpha = 0;
                    tmp1.maxAlpha = 10;
                    tmp1.xtrTop = 1;
                    tmp1.xtrBot = 1;
                    tmp1.designALPHA = 0;
                    tmp1.designCL = spanLiftCoefficient[index][i];
                    tmp1.model = 1;
                    //if(isSymmetry)
                        //tmp1.model = 1;
                    //else
                        //tmp1.model = 0;
                    tmp1.s_IterLim = 200;
                    spanInputArray.append(tmp1);
                    spanCstAllArray.append(cstTmp[i]);
                }
            }

            //线程分配

            int threadSum = realNy * vinfArray.length();
            int step = threadSum / threadNum;
            int remainderTmp = threadSum % threadNum;

            int progressValue;
            for(int i = 0;i<step;i++){
                int index = i * threadNum;
                startXfoilInThread(index,threadNum);
                if(!wingCD.isEmpty()){
                    progressValue = double(i + 1) / step * 50 + 50;
                    emit emitProgressValue(progressValue);
                }
            }

            if(remainderTmp > 0){

                int beginIndex = threadSum - remainderTmp;
                startXfoilInThread(beginIndex,remainderTmp);
            }

               smoothDrag();
        }

    }else{
        for(int i = 0;i<vinfArray.length();i++){
            dragForce.append(0);

        }
    }

    for(int i = 0;i<vinfArray.length();i++){
        QVector<double>tmp;


        double forceTmp = 0;
        for(int j = 0;j<realNy;j++){
            int index = i * realNy + j;
            tmp.append(spanDragArray[index]);
            forceTmp += tmp[j] * spanArea[j];
        }
        forceTmp = forceTmp * 2 / realArea;
        for(int j = realNy - 1;j>=0;j--){
            tmp.append(tmp[j]);
        }
        //dragTmp.append(tmp);
        dragForce.append(forceTmp);

    }





    //飞机设计时不会计算wingCD
    if(!wingCD.isEmpty()){
        for(int i = 0;i<vinfArray.length();i++){
            wingCD[i] = wingCD[i] + dragForce[i] + zeroLiftDrag;

        }
    }


    wingViscousDrag = dragForce;

}
void wingVLM::startXfoilInThread(const int startIndex,const int batchSize){
    QVector<QFuture<void>>futures;

    QFutureSynchronizer<void> sync;
    QVector<airfoilSolve*>testSolve;
    QVector<double>airfoilX;
    for(int i = 0;i<51;i++)
        airfoilX.append(0.5 - cos(static_cast<double>(i) * M_PI / 50 ) * 0.5);

    for(int i = 0;i<batchSize;i++){
        airfoilSolve *solvers = new airfoilSolve();
        testSolve.append(solvers);
        airfoilDesign design(cstNum);
        design.buildAirfoilCurve(spanCstAllArray[startIndex + i],airfoilX);
        solvers->importAirfoil(design.newAirfoilData);
        solvers->refreshParaments(spanInputArray[startIndex + i]);
        QFuture<void>future = QtConcurrent::run([solvers](){
            solvers->emitResult();
        });
        futures.append(future);
    }



    for (auto &f : futures) {
        sync.addFuture(f);
    }

    sync.waitForFinished();   // ✅ 等“所有线程”一起完成



    for(int i = 0;i<batchSize;i++){
        spanDragArray.append(testSolve[i]->onceData.cD);
        spanLiftArray.append(testSolve[i]->onceData.cL);



    }
    qDeleteAll(testSolve);
    testSolve.clear();
}
void wingVLM::computeFixLiftDrag(){
    isOptimization = true;
    //重新初始化迎角
    QVector<double>rAlpha;
    rAlpha<<0<<5<<10;
    alphaArray = rAlpha;
    QVector<point3d>rVinf;
    for(int i = 0;i<3;i++){
        rVinf.append(point3d(vinf * cos(rAlpha[i] / 180 * M_PI),0,vinf * sin(rAlpha[i] / 180 * M_PI)));
    }
    vinfArray = rVinf;





    /*******************计算尾迹***********************/
    //computeWakeVortex();
    /*******************计算影响系数***********************/

    solveInfluenceCoefficient();



    //solveInfluenceCoefficient();


    /*******************求解线性方程组*********************/

    computeVLMMatrix();
    computeGamma();




    /********************计算附着涡中点处诱导系数**************************/
    computeVelocity();


    /************************计算净涡强**************************/

    computeTreffzDrag();


    /********************计算气动力********************/
    computeForce(streamVinfArray);

    computeCm();
    //计算颜色矩阵
    computePressure();

    // /****************总面积***************/
    //double AreaTotal = sum(areas);
    // /****************动压*****************/


    double p = 0.5 * density * vinf * vinf * realArea;
    /****************系数计算***************/

    /*
    for(int i = 0;i<alphaArray.length();i++){

        double CL = totalForceArray[i].z / p;
        double CD = totalForceArray[i].x / p;
        double CM = -wingCMForce[i] / p / mac;
        wingCL.append(CL);
        wingCD.append(CD);
        wingCM.append(CM);
        //qDebug()<<"升力系数"<<CL<<"阻力系数"<<CD<<"参考面积"<<" "<<alphaArray[i]<<"cm"<<wingCM[i];
    }
    */

    designCl = designLiftForce / p;

    int minIndex = 0,maxIndex = 1;
    for(int i = 0;i<2;i++){
        if(designCl >= wingCL[minIndex + i]&& designCl <= wingCL[maxIndex + i]){
            minIndex = minIndex + i;
            maxIndex = maxIndex + i;
            break;
        }
    }





    double k = (alphaArray[maxIndex] - alphaArray[minIndex]) / (wingCL[maxIndex] - wingCL[minIndex]);
    designAlpha = (designCl - wingCL[minIndex]) * k + alphaArray[minIndex];

    if(designAlpha < 10)
        computeInDesignAlpha(designAlpha);
    else{
        fixCl = designCl;
        fixCd = 10;
        fixCm = -1;
    }

}

void wingVLM::smoothDrag(){

    double error = 0.01;
    int realNy;

    if(isSymmetry)
        realNy = Ny / 2;
    else
        realNy = Ny;


    QVector<int>badValueIndex;
    QVector<QVector<double>>spanIndexTmp;
    QVector<QVector<double>>spanDragTmp;



    for(int index = 0;index<alphaArray.length();index++){
        QVector<double>tmp1;
        QVector<double>tmp2;
        for(int i = 0;i<realNy;i++){
            int G = index * realNy + i;

            if(abs(spanLiftCoefficient[index][i] - spanLiftArray[G]) > error ){
                badValueIndex.append(G);

            }else{
                tmp1.append(spanYt[i]);
                tmp2.append(spanDragArray[G]);
            }
        }
        spanIndexTmp.append(tmp1);
        spanDragTmp.append(tmp2);
    }



    for(int i = 0;i<badValueIndex.length();i++){
        int j = badValueIndex[i] / realNy;
        int k = badValueIndex[i] % realNy;
        int G = badValueIndex[i];

        spanDragArray[G] = myMath::linearInterpolation(spanIndexTmp[j],spanDragTmp[j],spanYt[k]);
        //spanDragArray[G] = mySolve.threePointInterpolation(spanIndexTmp[j],spanDragTmp[j],spanYt[k]);

    }




}
void wingVLM::refreshParaments(const VLMSetting&a){
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

    int len = int(abs(max - min) / step);
    for(int i = 0;i<len;i++){
        tmp.append(min + step * double(i));
    }
    tmp.append(max);

    if(a.vinf > 0)
        vinf = a.vinf;
    else
        vinf = v;
    for(int i = 0;i<len + 1;i++){
        vtmp.append(point3d(vinf * cos(tmp[i] / 180 * M_PI),0,vinf * sin(tmp[i] / 180 * M_PI)));
    }

    vinfArray = vtmp;
    alphaArray = tmp;
    height = a.height;

    density = mathSolver.calculateAirDensity(height);
    cgX = a.referencePointX;
}
bool wingVLM::checkGeometry(){
    int len = spanW.length();
    bool state = false;
    if(chordLengthW.length() == len && twistAngleW.length() == len &&dihedralAngleW.length() ==len && offsetLengthW.length() == len){
        if(gridU.length() >= len - 1 && gridV.length() >= len - 1)
            if(gridU[0] > 0 && gridV[0]>0){
                state =  true;
                for(int i = 0;i<len;i++){
                    if(chordLengthW[i] < 0)
                        chordLengthW[i] = 0.1;
                }
            }
    }

    return state;
}

void wingVLM::computeGeometry2DMesh(){
    QVector<QVector<point3d>>xyv;
    /*********************定义弦向网格生成方式*********************/
    QVector<double>cosGridV;

    if(meshRatioX){

        for(int i = 0;i<gridV[0] + 1;i++){
            cosGridV.append(0.5 * (1 - cos(double(i) / gridV[0]* M_PI)));
        }
    }else{
        for(int i = 0;i<gridV[0] + 1;i++){
            cosGridV.append(double(i) / gridV[0]);
        }
    }
    for(int i = 0;i<num;i++){
        for(int j = 0;j<gridU[num - i];j++)
            indexArray.append(i);
    }
    for(int i = 0;i<Ny / 2;i++){
        indexArray.append(indexArray[Ny / 2 - i - 1]);
    }


    /*********************定义展向网格生成方式*********************/
    if(meshRatioY != 1.0){

        yt.append(-spanW[num]);
        cy.append(chordLengthW[num]);
        double psi = (offsetLengthW[num] - offsetLengthW[num - 1]) / (-spanW[num] + spanW[num - 1]);
        xAtaque.append(offsetLengthW[num - 1] + psi * (-spanW[num] + spanW[num - 1]));
        xSalida.append(xAtaque[0] + cy[0]);
        //indexArray.append(0);
        psiTan.append(psi);
        dihedCos.append(cos(dihedralAngleW[num - 1] / 180 * M_PI));

        double tmp1 = 0,tmp2 = 0,tmp3 = 0;

        for(int i = 0;i<num;i++){
            int len = gridU[num - i];

            double psiTanTmp = (offsetLengthW[num - i] - offsetLengthW[num - i - 1]) / (-spanW[num - i] + spanW[num - i - 1]);        //计算每段机翼的后掠角

            double a1 = abs(spanW[num - i] - spanW[num - i - 1]) * (1 - meshRatioY) / (1 - pow(meshRatioY,gridU[num - i]));
            for(int j = 0;j<len;j++){
                if(j != len - 1)
                    tmp1 =a1 * (1 - pow(meshRatioY,j + 1)) / (1 - meshRatioY);
                else
                    tmp1 = spanW[num - i] - spanW[num - i - 1];

                tmp2 = (chordLengthW[num - i - 1] - chordLengthW[num - i]) / (spanW[num - i] - spanW[num - i - 1]) * (tmp1) + chordLengthW[num - i];
                tmp3 = offsetLengthW[num - i - 1] + psiTanTmp * (-spanW[num - i] + tmp1 + spanW[num - i - 1]);
                yt.append(-spanW[num - i] + tmp1);
                cy.append(tmp2);
                xAtaque.append(tmp3);
                xSalida.append(tmp3 + tmp2);
                //indexArray.append(i);
                psiTan.append(psiTanTmp);
                dihedCos.append(cos(dihedralAngleW[num - i - 1] / 180 * M_PI));

            }
        }

    }else{
        /********************多一个点********************/
        int ii = 0;
        for(int i = 0;i<num;i++){
            int len = gridU[num - i];
            if(i == 0)
                len = len +1;
            double psiTanTmp = (offsetLengthW[num - i] - offsetLengthW[num - i - 1]) / (-spanW[num - i] + spanW[num - i - 1]);        //计算每段机翼的后掠角
            for(int j = 0;j<len;j++){
                double tmp1 = (spanW[num - i] - spanW[num - i - 1]) / gridU[num - i] * (j + ii);
                double tmp2 = (chordLengthW[num - i - 1] - chordLengthW[num - i]) / gridU[num - i] * (j + ii) + chordLengthW[num - i];
                double tmp3 = offsetLengthW[num - i - 1] + psiTanTmp * (-spanW[num - i] + tmp1 + spanW[num - i - 1]);
                yt.append(-spanW[num - i] + tmp1);
                cy.append(tmp2);
                xAtaque.append(tmp3);
                xSalida.append(tmp3 + tmp2);
                //indexArray.append(i);
                psiTan.append(psiTanTmp);
                dihedCos.append(cos(dihedralAngleW[num - i - 1] / 180 * M_PI));

            }
            ii = 1;
        }

    }




    spanYt = yt;


    //indexArray.pop_back(); //删除多余元素
    psiTan.pop_back();//删除多余元素
    dihedCos.pop_back();
    

    for(int i = Ny / 2 - 1;i>=0;i--){
        double tmp1 = -yt[i];
        double tmp2 = cy[i];
        double tmp3 = xAtaque[i];
        double tmp4 = xSalida[i];
        double tmp5 = psiTan[i];
        double tmp6 = dihedCos[i];
        //int tmp7 = indexArray[i];

        yt.append(tmp1);
        cy.append(tmp2);
        xAtaque.append(tmp3);
        xSalida.append(tmp4);
        psiTan.append(tmp5);
        dihedCos.append(tmp6);
        //indexArray.append(tmp7);

    }



   /*
    for(int i = 0;i<num;i++){
        int len = gridU[num - i];
        for(int j = 0;j<len;j++){
            indexArray.append(num - i - 1);
        }
    }
    */

    //计算每段的面积
    QVector<double>spanAreaTmp;
    for(int i = 0;i<Ny;i++){
        double areaTmp = (cy[i] + cy[i + 1]) / 2 * abs(yt[i] - yt[i + 1]);
        spanAreaTmp.append(areaTmp);
    }

    spanArea = spanAreaTmp;




    for(int i = 0;i<Ny + 1;i++){
        QVector<point3d>tmp;
        for(int j = 0;j<Nx + 1;j++){
            point3d tmp2;
            tmp2.x = xAtaque[i] + (xSalida[i] - xAtaque[i]) * cosGridV[j];
            tmp2.y = yt[i];
            tmp2.z = 0;

            tmp.append(tmp2);
        }
        xyv.append(tmp);
    }


    for(int i = 0;i<Nx + 1;i++){

        QVector<point3d>xyvTmp;
        for(int j = 0;j<Ny + 1;j++){
            xyvTmp.append(xyv[j][i]);
        }
        xyvv.append(xyvTmp);
    }




    for(int i = 0;i<Nx + 1;i++){
        QVector<double>xCurTmp;

        for(int j = 0;j<Ny + 1;j++){
            double tmp = cosGridV[i];
            xCurTmp.append(tmp);
        }
        xCur.append(xCurTmp);
    }




    QVector<double>ytTmp;
    for(int i = 0;i<Ny;i++){
        ytTmp.append((yt[i] + yt[i + 1]) / 2);
    }
    spanForceYt = ytTmp;
}
void wingVLM::generateZCoordinates(){
    /*********************计算z坐标**********************/
    if(airfoilInputType)
        computeMeanCamberAirfoil();
    else
        computeMeanCamberCst();
    interpolationAirfoil();//计算插值后的翼型

    for(int i = 0;i<Nx + 1;i++){
        for(int j = 0;j<Ny + 1;j++){
            double tmp = computeZ(j,xCur[i][j],cy[j]);
            //xyvv[i][j].setZ(tmp);
            xyvv[i][j].z = tmp;
        }
    }

    //生成机翼上下表面

    xyvvUpper = xyvv;
    xyvvLower = xyvv;
    for(int i = 0;i<Nx + 1;i++){
        for(int j = 0;j<Ny + 1;j++){
            double tmp1 = computeUpperZ(j,xCur[i][j],cy[j]);
            double tmp2 = computeLowerZ(j,xCur[i][j],cy[j]);
            xyvvUpper[i][j].z = tmp1;
            xyvvLower[i][j].z = tmp2;
        }
    }

    //扭转 上反

}

void wingVLM::computeVortex(){

    /****************将点分配给四个坐标****************/
    for(int i = 0;i<Nx;i++){
        for(int j = 0;j<Ny;j++){
            xyA.append(xyvv[i][j]);
            xyB.append(xyvv[i][j + 1]);
            xyC.append(xyvv[i + 1][j + 1]);
            xyD.append(xyvv[i + 1][j]);
        }
    }

    /********************网格***********************/



    for(int i = 0;i < N;i++){
        cm.append(((xyC[i].x + xyD[i].x) - (xyA[i].x + xyB[i].x)) / 2);

        xp.append((xyB[i].x + xyA[i].x) / 2);
        xp2.append((xyC[i].x + xyD[i].x) / 2);
        xp3.append(xyA[i].x + 0.75 * (xyD[i].x - xyA[i].x));
        xp4.append(xyB[i].x + 0.75 * (xyC[i].x - xyB[i].x));
        xp5.append(xyA[i].x + 0.25 * (xyD[i].x - xyA[i].x));
        xp6.append(xyB[i].x + 0.25 * (xyC[i].x - xyB[i].x));
        vortexArray[i].xyzControl.x = xp[i] + 0.75 * (xp2[i] - xp[i]);


        yp.append((xyB[i].y + xyA[i].y) / 2);
        yp2.append((xyC[i].y + xyD[i].y) / 2);
        yp3.append(xyA[i].y + 0.75 * (xyD[i].y - xyA[i].y));
        yp4.append(xyB[i].y + 0.75 * (xyC[i].y - xyB[i].y));
        yp5.append(xyA[i].y + 0.25 * (xyD[i].y - xyA[i].y));
        yp6.append(xyB[i].y + 0.25 * (xyC[i].y - xyB[i].y));
        vortexArray[i].xyzControl.y = yp[i] + 0.75 * (yp2[i] - yp[i]);

        zp.append((xyB[i].z + xyA[i].z) / 2);
        zp2.append((xyC[i].z + xyD[i].z) / 2);
        zp3.append(xyA[i].z + 0.75 * (xyD[i].z - xyA[i].z));
        zp4.append(xyB[i].z + 0.75 * (xyC[i].z - xyB[i].z));
        zp5.append(xyA[i].z + 0.25 * (xyD[i].z - xyA[i].z));
        zp6.append(xyB[i].z + 0.25 * (xyC[i].z - xyB[i].z));
        vortexArray[i].xyzControl.z = zp[i] + 0.75 * (zp2[i] - zp[i]);

    }
    //生成涡环顶点
    for(int i = 0;i<Nx;i++){
        for(int j = 0;j<Ny;j++){
            int G = i * Ny + j;
            double ax = xyA[G].x + (xyD[G].x - xyA[G].x) / 4;
            double ay = xyA[G].y;
            double az = xyA[G].z * 0.75 + xyD[G].z * 0.25;

            double bx = xyB[G].x + (xyC[G].x - xyB[G].x) / 4;
            double by = xyB[G].y;
            double bz = xyB[G].z * 0.75 + xyC[G].z * 0.25;

            vortexArray[G].id = G;
            vortexArray[G].xyzA = point3d(ax,ay,az);
            vortexArray[G].xyzB = point3d(bx,by,bz);
        }
    }

    for(int i = 0;i<Nx - 1;i++){
        for(int j = 0;j<Ny;j++){
            int G = i * Ny + j;
            vortexArray[G].xyzC = vortexArray[G + Ny].xyzB;
            vortexArray[G].xyzD = vortexArray[G + Ny].xyzA;
        }
    }

    for(int i = 0;i<Ny;i++){
        int G = (Nx - 1) * Ny + i;
        double x1 = xSalida[i + 1] + 0.25 * abs(xyB[N - Ny + i].x - xyC[N - Ny + i].x);
        double x2 = xSalida[i] + 0.25 * abs(xyA[N - Ny + i].x - xyD[N - Ny + i].x);
        double y1 = (xyB[N - Ny + i].y - xyC[N - Ny + i].y) /  (xyB[N - Ny + i].x - xyC[N - Ny + i].x) * (x1 - xyC[N - Ny + i].x) + xyC[N - Ny + i].y;
        double y2 = (xyA[N - Ny + i].y - xyD[N - Ny + i].y) /  (xyA[N - Ny + i].x - xyD[N - Ny + i].x) * (x2 - xyD[N - Ny + i].x) + xyD[N - Ny + i].y;
        double z1 = (xyB[N - Ny + i].z - xyC[N - Ny + i].z) /  (xyB[N - Ny + i].x - xyC[N - Ny + i].x) * (x1 - xyC[N - Ny + i].x) + xyC[N - Ny + i].z;
        double z2 = (xyA[N - Ny + i].z - xyD[N - Ny + i].z) /  (xyA[N - Ny + i].x - xyD[N - Ny + i].x) * (x2 - xyD[N - Ny + i].x) + xyD[N - Ny + i].z;
        vortexArray[G].xyzC = point3d(x1,y1,z1);
        vortexArray[G].xyzD = point3d(x2,y2,z2);
    }
    //生成EF点
    for(int i = 0;i<Nx;i++){
        for(int j = 0;j<Ny;j++){
            int G = i * Ny + j;
            vortexArray[G].xyzE = xyD[G];
            vortexArray[G].xyzF = xyC[G];
        }
    }


    //生成中心点

    for(int i = 0;i<N;i++){
        double x = (vortexArray[i].xyzA.x + vortexArray[i].xyzB.x) / 2;
        double y = (vortexArray[i].xyzA.y + vortexArray[i].xyzB.y) / 2;
        double z = (vortexArray[i].xyzA.z + vortexArray[i].xyzB.z) / 2;
        vortexArray[i].xyzCenter = point3d(x,y,z);
    }

    //生成控制点法向分量
    for(int i = 0;i<N;i++){
        double u1x = xp2[i] - xp[i];
        double u1y = yp2[i] - yp[i];
        double u1z = zp2[i] - zp[i];
        double u2x = xp4[i] - xp3[i];
        double u2y = yp4[i] - yp3[i];
        double u2z = zp4[i] - zp3[i];
        double x = u1y * u2z - u1z * u2y;
        double y = u1z * u2x - u1x * u2z;
        double z = u1x * u2y - u1y * u2x;
        double m = sqrt(x * x + y * y + z * z);
        vortexArray[i].nControl = point3d(x / m,y / m,z / m);
    }

    //生成中心点法向分量

    for(int i = 0;i<N;i++){
        vortexArray[i].lengthVortex = point3d(abs(xp6[i] - xp5[i]),abs(yp6[i] - yp5[i]),abs(zp6[i] - zp5[i]));
    }



}
void wingVLM::computeWakeVortex(){

    if(!alphaArray.isEmpty()){
        double length = 5000 * abs(spanW[num]);
        for(int i = 0;i<vinfArray.length();i++){
            //double alpha = alphaArray[i];
            double alpha = 0;
            for(int j = 0;j<Ny;j++){
                vortex a;

                double xc = length * cos(alpha / 180 * M_PI);
                double xd = xc;
                double yc = vortexArray[N - Ny + j].xyzC.y;
                double yd = vortexArray[N - Ny + j].xyzD.y;
                double zc = vortexArray[N - Ny + j].xyzC.z + length *  sin(alpha / 180 * M_PI);
                double zd = vortexArray[N - Ny + j].xyzD.z + length *  sin(alpha / 180 * M_PI);
                double ze = vortexArray[N - Ny + j].xyzD.z + chordLengthW[0] * sin(alpha / 180 * M_PI);
                double zf = vortexArray[N - Ny + j].xyzC.z + chordLengthW[0] * sin(alpha / 180 * M_PI);
                a.xyzA = vortexArray[N - Ny + j].xyzD;
                a.xyzB = vortexArray[N - Ny + j].xyzC;
                a.xyzC = point3d(xc,yc,zc);
                a.xyzD = point3d(xd,yd,zd);


                a.xyzE.y = vortexArray[N - Ny + j].xyzD.y;
                a.xyzF.y = vortexArray[N - Ny + j].xyzC.y;
                a.xyzE.z = ze;
                a.xyzF.z = zf;
                a.xyzE.x = vortexArray[N - Ny + j].xyzD.x;
                a.xyzF.x = vortexArray[N - Ny + j].xyzC.x;

                a.xyzE.x += chordLengthW[0] * cos(alpha / 180 * M_PI);
                a.xyzF.x += chordLengthW[0] * cos(alpha / 180 * M_PI);
                wakeVortexArray.append(a);
            }
        }
    }


}
void wingVLM::computeTrefftzVortex(){

    double x = 500 * abs(spanW[num]);


    for(int i = 0;i<Ny;i++){

        vortex tmp;
        tmp.id = i;
        tmp.xyzA = point3d(x,xyD[N - Ny + i].y,xyD[N - Ny + i].z);
        tmp.xyzB = point3d(x,xyC[N - Ny + i].y,xyC[N - Ny + i].z);
        double y = (xyD[N - Ny + i].y + xyC[N - Ny + i].y) / 2;
        double z = (xyD[N - Ny + i].z + xyC[N - Ny + i].z) / 2;
        double dy = xyC[N - Ny + i].y - xyD[N - Ny + i].y;
        double dz = xyC[N - Ny + i].z - xyD[N - Ny + i].z;
        double nx = 0.0;
        double ny = -dz;
        double nz = dy;
        double length = sqrt(dz * dz + dy * dy);
        tmp.xyzCenter = point3d(x,y,z);
        tmp.nCenter = point3d(nx,ny / length,nz / length);

        trefftzArray.append(tmp);



    }






}
void wingVLM::computeArea(){
    for(int i = 0;i<Nx;i++){
        QVector<double>tmp;
        QVector<double>h;
        for(int j = 0;j<Ny;j++){
            int G = i * Ny + j;
            tmp.append(calculateQuadrilateralArea(xyA[G], xyB[G], xyC[G], xyD[G]));
            h.append(cm[G]);
        }
        areas.append(tmp);
        hs.append(h);
    }

    int len = spanW.length() - 1;
    double areaTmp = 0;
    for(int i = 0;i<len;i++){
        double cr = chordLengthW[len - i];
        double ct = chordLengthW[len - i - 1];
        double b = spanW[len - i] - spanW[len - i - 1];
        double s = b / 2.0 * (cr + ct);
        areaTmp = areaTmp + s;
    }
    realArea = areaTmp * 2;
}

void wingVLM::computeCm(){
    QVector<double>tmpArray;

    for(int alphaIndex = 0;alphaIndex<matrixGammaArray.length();alphaIndex++){
        double cmTmp = 0;
        for(int i = 0;i<N;i++){
            cmTmp = cmTmp + allForceArray[alphaIndex][i].z * (vortexArray[i].xyzControl.x - cgX);
        }
        tmpArray.append(cmTmp);
    }
    wingCMForce = tmpArray;
}


void wingVLM::computeMoments(const point3d cg)
{
    if(allForceArray.isEmpty()) return;

    QVector<double> tmpMy;   // 俯仰
    QVector<double> tmpMx;   // 滚转
    QVector<double> tmpMz;   // 偏航

    for(int alphaIndex = 0; alphaIndex < matrixGammaArray.length(); alphaIndex++)
    {
        double My = 0;
        double Mx = 0;
        double Mz = 0;

        for(int i = 0; i < N; i++)
        {
            // ======== 取完整气动力矢量 ========
            double X = allForceArray[alphaIndex][i].x; // 阻力分量
            double Y = allForceArray[alphaIndex][i].y; // 侧力分量
            double Z = allForceArray[alphaIndex][i].z; // 升力分量

            // ======== 力臂（相对重心） ========
            double dx = vortexArray[i].xyzControl.x - cg.x;
            double dy = vortexArray[i].xyzControl.y - cg.y;
            double dz = vortexArray[i].xyzControl.z - cg.z;

            // ======== 叉乘 r × F（完整三轴力矩） ========
            Mx +=  Y * dz - Z * dy;   // 滚转
            My +=  Z * dx - X * dz;   // 俯仰
            Mz +=  X * dy - Y * dx;   // 偏航
        }

        tmpMy.append(My);
        tmpMx.append(Mx);
        tmpMz.append(Mz);
    }

    wingCMForce = tmpMy;   // 俯仰力矩 My
    wingCLForce = tmpMx;   // 滚转力矩 Mx
    wingCNForce = tmpMz;   // 偏航力矩 Mz
}



void wingVLM::solveInfluenceCoefficient(){

    QVector<QVector<double>>vnWingArray;//翼面涡格对控制点的法向影响系数
    QVector<QVector<double>>vnWakeArray;//尾迹涡格对控制点的法向影响系数
    QVector<QVector<double>>vnTrefftzArray;
    QVector<QVector<double>> nxfSArray, nyfSArray, nzfSArray;//翼面涡格对中心点的影响系数
    QVector<QVector<double>>nxfWakeArray,nyfWakeArray,nzfWakeArray;//尾迹涡格对中心点的影响系数
    computeWingFluenceCoefficient(vnWingArray,nxfSArray, nyfSArray, nzfSArray);
    computeWakeFluenceCoefficient(vnWakeArray,nxfWakeArray, nyfWakeArray, nzfWakeArray);
    computeTrefftzFluenceCoefficient(vnTrefftzArray);

    vNArray.clear();
    vinxArray.clear();
    vinyArray.clear();
    vinzArray.clear();


    int len;
    if(isFreeWake)
        len = alphaArray.length();
    else
        len = 1;

    for(int i = 0;i<len;i++){
        QVector<QVector<double>>vzTotalArray = vnWingArray;//
        QVector<QVector<double>>nxArray = nxfSArray;
        QVector<QVector<double>>nyArray = nyfSArray;
        QVector<QVector<double>>nzArray = nzfSArray;
        for(int j = 0;j<N;j++){
            int G = i * N + j;
            for(int k = 0;k<Ny;k++){
                vzTotalArray[j][N + k] = vnWakeArray[G][k];
                nxArray[j][N + k] = nxfWakeArray[G][k];
                nyArray[j][N + k] = nyfWakeArray[G][k];
                nzArray[j][N + k] = nzfWakeArray[G][k];

            }
        }

        vNArray.append(vzTotalArray);
        vinxArray.append(nxArray);
        vinyArray.append(nyArray);
        vinzArray.append(nzArray);
    }

    vNTrefftzArray = vnTrefftzArray;


}
void wingVLM::computeWingFluenceCoefficient(QVector<QVector<double>>&vnTotal,QVector<QVector<double>>&bx,QVector<QVector<double>>&by,QVector<QVector<double>>&bz){


    for (int i = 0; i < Nx; i++) {
        for (int j = 0; j < Ny; j++) {
            int G = i * Ny + j;

            double xcoloc = vortexArray[G].xyzControl.x;
            double ycoloc = vortexArray[G].xyzControl.y;
            double zcoloc = vortexArray[G].xyzControl.z;

            double xfcoloc = vortexArray[G].xyzCenter.x;
            double yfcoloc = vortexArray[G].xyzCenter.y;
            double zfcoloc = vortexArray[G].xyzCenter.z;

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



            for(int ii = 0;ii < Nx;ii++){
                for(int jj = 0;jj < Ny;jj++){
                    int GG = ii * Ny + jj;
                    point3d a = leyBiotSavart3D(xcoloc,ycoloc,zcoloc,vortexArray[GG].xyzA.x,vortexArray[GG].xyzA.y,vortexArray[GG].xyzA.z,
                                                vortexArray[GG].xyzB.x,vortexArray[GG].xyzB.y,vortexArray[GG].xyzB.z);
                    point3d b = leyBiotSavart3D(xcoloc,ycoloc,zcoloc,vortexArray[GG].xyzB.x,vortexArray[GG].xyzB.y,vortexArray[GG].xyzB.z,
                                                vortexArray[GG].xyzF.x,vortexArray[GG].xyzF.y,vortexArray[GG].xyzF.z);
                    point3d c = leyBiotSavart3D(xcoloc,ycoloc,zcoloc,vortexArray[GG].xyzF.x,vortexArray[GG].xyzF.y,vortexArray[GG].xyzF.z,
                                                vortexArray[GG].xyzC.x,vortexArray[GG].xyzC.y,vortexArray[GG].xyzC.z);
                    point3d d = leyBiotSavart3D(xcoloc,ycoloc,zcoloc,vortexArray[GG].xyzC.x,vortexArray[GG].xyzC.y,vortexArray[GG].xyzC.z,
                                                vortexArray[GG].xyzD.x,vortexArray[GG].xyzD.y,vortexArray[GG].xyzD.z);
                    point3d e = leyBiotSavart3D(xcoloc,ycoloc,zcoloc,vortexArray[GG].xyzD.x,vortexArray[GG].xyzD.y,vortexArray[GG].xyzD.z,
                                                vortexArray[GG].xyzE.x,vortexArray[GG].xyzE.y,vortexArray[GG].xyzE.z);
                    point3d f = leyBiotSavart3D(xcoloc,ycoloc,zcoloc,vortexArray[GG].xyzE.x,vortexArray[GG].xyzE.y,vortexArray[GG].xyzE.z,
                                                vortexArray[GG].xyzA.x,vortexArray[GG].xyzA.y,vortexArray[GG].xyzA.z);
                    point3d a1 = leyBiotSavart3D(xfcoloc,yfcoloc,zfcoloc,vortexArray[GG].xyzA.x,vortexArray[GG].xyzA.y,vortexArray[GG].xyzA.z,
                                                vortexArray[GG].xyzB.x,vortexArray[GG].xyzB.y,vortexArray[GG].xyzB.z);
                    point3d b1 = leyBiotSavart3D(xfcoloc,yfcoloc,zfcoloc,vortexArray[GG].xyzB.x,vortexArray[GG].xyzB.y,vortexArray[GG].xyzB.z,
                                                vortexArray[GG].xyzF.x,vortexArray[GG].xyzF.y,vortexArray[GG].xyzF.z);
                    point3d c1 = leyBiotSavart3D(xfcoloc,yfcoloc,zfcoloc,vortexArray[GG].xyzF.x,vortexArray[GG].xyzF.y,vortexArray[GG].xyzF.z,
                                                vortexArray[GG].xyzC.x,vortexArray[GG].xyzC.y,vortexArray[GG].xyzC.z);
                    point3d d1 = leyBiotSavart3D(xfcoloc,yfcoloc,zfcoloc,vortexArray[GG].xyzC.x,vortexArray[GG].xyzC.y,vortexArray[GG].xyzC.z,
                                                vortexArray[GG].xyzD.x,vortexArray[GG].xyzD.y,vortexArray[GG].xyzD.z);
                    point3d e1 = leyBiotSavart3D(xfcoloc,yfcoloc,zfcoloc,vortexArray[GG].xyzD.x,vortexArray[GG].xyzD.y,vortexArray[GG].xyzD.z,
                                                vortexArray[GG].xyzE.x,vortexArray[GG].xyzE.y,vortexArray[GG].xyzE.z);
                    point3d f1 = leyBiotSavart3D(xfcoloc,yfcoloc,zfcoloc,vortexArray[GG].xyzE.x,vortexArray[GG].xyzE.y,vortexArray[GG].xyzE.z,
                                                vortexArray[GG].xyzA.x,vortexArray[GG].xyzA.y,vortexArray[GG].xyzA.z);

                    vxAB.append(a.x);vxBF.append(b.x);vxFC.append(c.x);vxCD.append(d.x);vxDE.append(e.x);vxEA.append(f.x);
                    vyAB.append(a.y);vyBF.append(b.y);vyFC.append(c.y);vyCD.append(d.y);vyDE.append(e.y);vyEA.append(f.y);
                    vzAB.append(a.z);vzBF.append(b.z);vzFC.append(c.z);vzCD.append(d.z);vzDE.append(e.z);vzEA.append(f.z);
                    vxfAB.append(a1.x);vxfBF.append(b1.x);vxfFC.append(c1.x);vxfCD.append(d1.x);vxfDE.append(e1.x);vxfEA.append(f1.x);
                    vyfAB.append(a1.y);vyfBF.append(b1.y);vyfFC.append(c1.y);vyfCD.append(d1.y);vyfDE.append(e1.y);vyfEA.append(f1.y);
                    vzfAB.append(a1.z);vzfBF.append(b1.z);vzfFC.append(c1.z);vzfCD.append(d1.z);vzfDE.append(e1.z);vzfEA.append(f1.z);
                }
            }


            for (int k = 0; k < N; k++) {
                nxS.append(vxAB[k] + vxBF[k] + vxFC[k] + vxCD[k] + vxDE[k] + vxEA[k]);
                nyS.append(vyAB[k] + vyBF[k] + vyFC[k] + vyCD[k] + vyDE[k] + vyEA[k]);
                nzS.append(vzAB[k] + vzBF[k] + vzFC[k] + vzCD[k] + vzDE[k] + vzEA[k]);

                nxfS.append(vxfAB[k] + vxfBF[k] + vxfFC[k] + vxfCD[k] + vxfDE[k] + vxfEA[k]);
                nyfS.append(vyfAB[k] + vyfBF[k] + vyfFC[k] + vyfCD[k] + vyfDE[k] + vyfEA[k]);
                nzfS.append(vzfAB[k] + vzfBF[k] + vzfFC[k] + vzfCD[k] + vzfDE[k] + vzfEA[k]);
            }

            for (int k = 0; k < N; k++) {
                point3d b1(vortexArray[G].nControl.x, vortexArray[G].nControl.y, vortexArray[G].nControl.z);
                point3d a1(nxS[k], nyS[k], nzS[k]);
                vzTotal.append(computeNormalInfluences(a1, b1));
            }
            //填充矩阵方便后期赋值
            for(int k = 0;k < Ny;k++){
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


}
void wingVLM::computeWakeFluenceCoefficient(QVector<QVector<double>>&vnTotal,QVector<QVector<double>>&bx,QVector<QVector<double>>&by,QVector<QVector<double>>&bz){
    int len;
    if(isFreeWake)
        len = alphaArray.length();
    else
        len = 1;

    for(int i = 0;i < len;i++){
        for(int j = 0;j < Nx;j++){
            for(int k = 0;k < Ny;k++){
                int G = j * Ny + k;
                double xcoloc = vortexArray[G].xyzControl.x;
                double ycoloc = vortexArray[G].xyzControl.y;
                double zcoloc = vortexArray[G].xyzControl.z;

                double xfcoloc = vortexArray[G].xyzCenter.x;
                double yfcoloc = vortexArray[G].xyzCenter.y;
                double zfcoloc = vortexArray[G].xyzCenter.z;

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

                for(int jj = 0;jj < Ny;jj++){
                    //int tmp = jj + i * Ny;
                    int tmp = jj;

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

                for (int kk = 0; kk < Ny; kk++) {
                    nxS.append(vxAB[kk] + vxBF[kk] + vxFC[kk] + vxCD[kk] + vxDE[kk] + vxEA[kk]);
                    nyS.append(vyAB[kk] + vyBF[kk] + vyFC[kk] + vyCD[kk] + vyDE[kk] + vyEA[kk]);
                    nzS.append(vzAB[kk] + vzBF[kk] + vzFC[kk] + vzCD[kk] + vzDE[kk] + vzEA[kk]);

                    nxfS.append(vxfAB[kk] + vxfBF[kk] + vxfFC[kk] + vxfCD[kk] + vxfDE[kk] + vxfEA[kk]);
                    nyfS.append(vyfAB[kk] + vyfBF[kk] + vyfFC[kk] + vyfCD[kk] + vyfDE[kk] + vyfEA[kk]);
                    nzfS.append(vzfAB[kk] + vzfBF[kk] + vzfFC[kk] + vzfCD[kk] + vzfDE[kk] + vzfEA[kk]);
                }

                for (int kk = 0; kk < Ny; kk++) {
                    point3d b1(vortexArray[G].nControl.x, vortexArray[G].nControl.y, vortexArray[G].nControl.z);
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
}
void wingVLM::computeTrefftzFluenceCoefficient(QVector<QVector<double>>&vnTotal){
    int len;
    if(isFreeWake)
        len = alphaArray.length();
    else
        len = 1;
    for(int i = 0;i < len;i++){
        for(int j = 0;j<Ny;j++){
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

            for(int jj = 0;jj < Ny;jj++){
                int tmp = jj + i * Ny;

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

            for (int kk = 0; kk < Ny; kk++) {
                nxS.append(vxAB[kk] + vxBF[kk] + vxFC[kk] + vxCD[kk] + vxDE[kk] + vxEA[kk]);
                nyS.append(vyAB[kk] + vyBF[kk] + vyFC[kk] + vyCD[kk] + vyDE[kk] + vyEA[kk]);
                nzS.append(vzAB[kk] + vzBF[kk] + vzFC[kk] + vzCD[kk] + vzDE[kk] + vzEA[kk]);
            }



            for (int kk = 0; kk < Ny; kk++) {
                point3d b1(trefftzArray[j].nCenter.x, trefftzArray[j].nCenter.y, trefftzArray[j].nCenter.z);
                point3d a1(nxS[kk], nyS[kk], nzS[kk]);
                vzTotal.append(computeNormalInfluences(a1, b1));
            }

            vnTotal.append(vzTotal);


        }
    }



}
double wingVLM::computeNormalInfluences(const point3d &ci, const point3d &n){
    return  ci.x * n.x + ci.y * n.y + ci.z * n.z;
}
void wingVLM::computeVLMMatrix(){
    int progressValue;
    int step = vinfArray.length();
    QVector<QVector<double>>mgammaArray;

    Eigen::MatrixXd MatrixQRA;
    MatrixQRA.resize(N,N);
    Eigen::ColPivHouseholderQR<Eigen::MatrixXd> qr;
    //只计算一次

    //QElapsedTimer timer;

    // 2. 记录开始时间并输出
    //timer.start();
    //qDebug() << "计算开始时间:" << QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz");

    if(!isFreeWake){

        for(int j = 0;j<N ;j++){
            QVector<double>matrixTmp;
            for(int k = 0;k<N;k++){

                matrixTmp.append(vNArray[0][j][k]);

                MatrixQRA.coeffRef(j,k) = vNArray[0][j][k];
            }
        }

        for(int j = 0;j<N ;j++){
            for(int k = 0;k<Ny;k++){
                MatrixQRA.coeffRef(j,N - Ny + k) = vNArray[0][j][N - Ny + k] + vNArray[0][j][N + k];
            }
        }

        qr.compute(MatrixQRA);
    }





    int index = 0;
    for(int i = 0;i<vinfArray.length();i++){

        if(isFreeWake)
            index = i;

        QVector<double>mgamma;
        QVector<double>matrixb;

        Eigen::MatrixXd MatrixA;
        Eigen::VectorXd MatrixGamma;
        Eigen::VectorXd Matrixb;
        Matrixb.resize(N);
        MatrixA.resize(N,N);

        for(int j = 0;j<N ;j++){
            QVector<double>matrixTmp;
            for(int k = 0;k<N;k++){

                matrixTmp.append(vNArray[index][j][k]);

                MatrixA.coeffRef(j,k) = vNArray[index][j][k];
            }
        }

        for(int j = 0;j<N ;j++){
            for(int k = 0;k<Ny;k++){
                MatrixA.coeffRef(j,N - Ny + k) = vNArray[index][j][N - Ny + k] + vNArray[index][j][N + k];
            }
        }





        for(int j = 0;j<N;j++){
            point3d v = vinfArray[i];
            point3d n(vortexArray[j].nControl.x,vortexArray[j].nControl.y,vortexArray[j].nControl.z);
            matrixb.append(-dotProduct(v,n));
            Matrixb.coeffRef(j) = matrixb[j];
        }

        if(!isFreeWake){
            MatrixGamma = qr.solve(Matrixb);

        }
        else{

            MatrixGamma = MatrixA.colPivHouseholderQr().solve(Matrixb);
        }



        for(int k = 0;k<N;k++)
            mgamma.append(MatrixGamma(k));

        mgammaArray.append(mgamma);

        if(!isOptimization){
            if(isAddXfoilDrag && !isUseLibraries){
                progressValue = (double(i) + 1) / (step) * 50;

            }else{
                progressValue = (double(i) + 1) / (step) * 100;

            }
            emit emitProgressValue(progressValue);
        }


    }

    //qint64 elapsed = timer.elapsed();
    //qDebug() << "计算结束时间:" << QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz");
    //qDebug() << "总耗时:" << elapsed << "毫秒";

    matrixGammaArray = mgammaArray;


}


void wingVLM::computeVelocity(){


    QVector<QVector<point3d>>tmp2;
    double vxt,vyt,vzt;

    int index = 0;


    for(int alphaIndex = 0;alphaIndex<vinfArray.length();alphaIndex++){
        QVector<point3d>tmp1;
        if(isFreeWake)
            index = alphaIndex;

        for(int i = 0;i<N;i++){
            vxt = 0;
            vyt = 0;
            vzt = 0;
            for(int j = 0;j<N;j++){
                vxt = vxt + vinxArray[index][i][j] * matrixGammaArray[alphaIndex][j];
                vyt = vyt + vinyArray[index][i][j] * matrixGammaArray[alphaIndex][j];
                vzt = vzt + vinzArray[index][i][j] * matrixGammaArray[alphaIndex][j];

            }
            for(int j = N - Ny;j<N;j++){
                vxt = vxt + vinxArray[index][i][j + Ny] * matrixGammaArray[alphaIndex][j];
                vyt = vyt + vinyArray[index][i][j + Ny] * matrixGammaArray[alphaIndex][j];
                vzt = vzt + vinzArray[index][i][j + Ny] * matrixGammaArray[alphaIndex][j];
            }





            tmp1.append(point3d(vxt + vinfArray[alphaIndex].x,vyt + vinfArray[alphaIndex].y,vzt + vinfArray[alphaIndex].z));
        }



        tmp2.append(tmp1);
    }
    streamVinfArray = tmp2;




}

void wingVLM::computeGamma(const QVector<QVector<double>> &tmp){

    matrixGammaArray = tmp;
    QVector<QVector<double>>realGammaArrayTmp;

    for(int alphaIndex = 0;alphaIndex<matrixGammaArray.length();alphaIndex++){


        QVector<double>matrixGammaTmp = matrixGammaArray[alphaIndex];
        QVector<QVector<double>>Gamma;

        QVector<QVector<double>>GammaTotal2;
        QVector<double>realGam;


        for(int i = 0;i<Nx;i++){
            QVector<double>gammaTmp;
            for(int j = 0;j<Ny;j++){
                int G = i * Ny + j;
                gammaTmp.append(matrixGammaTmp[G]);

            }
            Gamma.append(gammaTmp);
        }

        for(int i = 0;i<Ny;i++){

            QVector<double>gammaTotalTmp2;

            gammaTotalTmp2.append(Gamma[0][i]);
            for(int j = 1;j<Nx;j++){

                gammaTotalTmp2.append(Gamma[j][i] - Gamma[j - 1][i]);


            }


            GammaTotal2.append(gammaTotalTmp2);


    }





    //矩阵转置


        for(int i = 0;i<Nx;i++){

            for(int j = 0;j<Ny;j++){

                realGam.append(GammaTotal2[j][i]);
            }

        }

        realGammaArrayTmp.append(realGam);

    }



    realGammaArray = realGammaArrayTmp;



}
void wingVLM::computeGamma(){
    QVector<QVector<double>>realGammaArrayTmp;

    for(int alphaIndex = 0;alphaIndex<matrixGammaArray.length();alphaIndex++){


        QVector<double>matrixGammaTmp = matrixGammaArray[alphaIndex];
        QVector<QVector<double>>Gamma;

        QVector<QVector<double>>GammaTotal2;
        QVector<double>realGam;


        for(int i = 0;i<Nx;i++){
            QVector<double>gammaTmp;
            for(int j = 0;j<Ny;j++){
                int G = i * Ny + j;
                gammaTmp.append(matrixGammaTmp[G]);

            }
            Gamma.append(gammaTmp);
        }

        for(int i = 0;i<Ny;i++){

            QVector<double>gammaTotalTmp2;

            gammaTotalTmp2.append(Gamma[0][i]);
            for(int j = 1;j<Nx;j++){

                gammaTotalTmp2.append(Gamma[j][i] - Gamma[j - 1][i]);


            }


            GammaTotal2.append(gammaTotalTmp2);


    }





    //矩阵转置


        for(int i = 0;i<Nx;i++){

            for(int j = 0;j<Ny;j++){

                realGam.append(GammaTotal2[j][i]);
            }

        }

        realGammaArrayTmp.append(realGam);

    }



    realGammaArray = realGammaArrayTmp;



}
void wingVLM::computeForce(const QVector<QVector<point3d>>&velocityField){
    streamVinfArray = velocityField;
    QVector<QVector<point3d>>tmpArray;
    QVector<point3d>forceTmpArray;
    QVector<QVector<double>>computedSpanLift;
    //QVector<QVector<double>>spanLiftCoeff;
    QVector<QVector<double>>momentTmp;
    QVector<QVector<double>>force;
    QVector<QVector<double>>mForceArray;
    QVector<QVector<double>>nSpanForce;


    for(int alphaIndex = 0;alphaIndex<vinfArray.length();alphaIndex++){
        double xa = 0;
        double ya = 0;
        double za = 0;
        QVector<point3d>sectionForces;
        QVector<double>liftTmp;
        QVector<double>dragTmp;
        QVector<double>lateralTmp;//侧力
        QVector<double>mForce;


        for(int i = 0;i<Nx;i++){
            for(int j = 0;j<Ny;j++){
                int G = i * Ny + j;
                double a = realGammaArray[alphaIndex][G] * vortexArray[G].lengthVortex.x;
                double b = realGammaArray[alphaIndex][G] * vortexArray[G].lengthVortex.y;
                double c = realGammaArray[alphaIndex][G] * vortexArray[G].lengthVortex.z;

                double VX = streamVinfArray[alphaIndex][G].x;
                double VY = streamVinfArray[alphaIndex][G].y;
                double VZ = streamVinfArray[alphaIndex][G].z;



                double a2 = (VY * c - VZ * b) * density;
                double b2 = (VZ * a - VX * c) * density;
                double c2 = (VX * b - VY * a) * density;



                xa = xa + a2;
                ya = ya + b2;
                za = za + c2;

                liftTmp.append(c2);
                dragTmp.append(a2);
                lateralTmp.append(b2);
                sectionForces.append(point3d(a2,b2,c2));
                mForce.append(sqrt(a2 * a2 + b2 * b2 + c2 * c2) / areas[i][j]);
            }
        }

        tmpArray.append(sectionForces);

        double cosAlpha(1),sinAlpha(0),cosBeta(1),sinBeta(0);
        if(isLateral){
            cosAlpha = cos(fixedAlpha / 180 * M_PI);
            sinAlpha = sin(fixedAlpha / 180 * M_PI);
            cosBeta = cos(betaArray[alphaIndex] / 180 * M_PI);
            sinBeta = sin(betaArray[alphaIndex] / 180 * M_PI);
        }else{
            cosAlpha = cos(alphaArray[alphaIndex] / 180 * M_PI);
            sinAlpha = sin(alphaArray[alphaIndex] / 180 * M_PI);
            cosBeta = cos(fixedBeta / 180 * M_PI);
            sinBeta = sin(fixedBeta / 180 * M_PI);
        }

        double Drag = cosAlpha * cosBeta * xa + sinAlpha * cosBeta * za + sinBeta * ya;
        double Lift = - sinAlpha * xa + cosAlpha * za;
        double Side = -cosAlpha * sinBeta * xa - sinAlpha * sinBeta * za + cosBeta * ya;
        QVector<double>yLiftTmp;
        //QVector<double>yLiftCoeff;
        QVector<double>yliftForce;
        QVector<double>nYForce;//展向法向力

        //计算每段的升力
        for(int i = 0;i<Ny;i++){

            double tmp1 = 0;
            double tmp2 = 0;
            double tmp3 = 0;


            for(int j = 0;j<Nx;j++){
                tmp1 = tmp1 + liftTmp[j * Ny + i];
                tmp2 = tmp2 + dragTmp[j * Ny + i];
                tmp3 = tmp3 + lateralTmp[j * Ny + i];
            }

            double tmpA = - sinAlpha * tmp2 + cosAlpha * tmp1 ;
            double sumTmp = tmpA / spanArea[i] / density / vinf / vinf * 2;
            //double nTmp = tmp3 / spanArea[i] / density / vinf / vinf * 2;
            double nForce = sqrt(tmp1 * tmp1 + tmp3 * tmp3);
            yLiftTmp.append(sumTmp * (cy[i] + cy[i + 1]) / 2);
            //yLiftCoeff.append(sumTmp / dihedCos[i]);
            //yLiftCoeff.append(sqrt(sumTmp * sumTmp + nTmp * nTmp));
            yliftForce.append(tmpA);
            nYForce.append(nForce);


        }
        computedSpanLift.append(yLiftTmp);

        forceTmpArray.append(point3d(Drag,Side,Lift));
        //spanLiftCoeff.append(yLiftCoeff);
        force.append(yliftForce);

        momentTmp.append(calculateBendingMoment(yliftForce,spanForceYt));
        mForceArray.append(mForce);
        nSpanForce.append(nYForce);

    }
    spanLiftForce = force;
    allForceArray = tmpArray;
    totalForceArray = forceTmpArray;//阻力 升力 侧力
    spanForce = computedSpanLift;
    //spanLiftCoefficient = spanLiftCoeff;
    spanMonmentArray = momentTmp;
    totalPressure = mForceArray;
    nSpanForceArray = nSpanForce;
    //qDebug()<<spanLiftCoefficient;

}
/*
void wingVLM::computeForce(){
    QVector<QVector<point3d>>tmpArray;
    QVector<point3d>forceTmpArray;
    QVector<QVector<double>>spanLift;
    //QVector<QVector<double>>spanLiftCoeff;
    QVector<QVector<double>>momentTmp;
    QVector<QVector<double>>force;
    QVector<QVector<double>>mForceArray;
    QVector<QVector<double>>nSpanForce;


    for(int alphaIndex = 0;alphaIndex<matrixGammaArray.length();alphaIndex++){
        double xa = 0;
        double ya = 0;
        double za = 0;
        QVector<point3d>tmp;
        QVector<double>liftTmp;
        QVector<double>dragTmp;
        QVector<double>lateralTmp;//侧力
        QVector<double>mForce;


        for(int i = 0;i<Nx;i++){
            for(int j = 0;j<Ny;j++){
                int G = i * Ny + j;
                double a = realGammaArray[alphaIndex][G] * vortexArray[G].lengthVortex.x;
                double b = realGammaArray[alphaIndex][G] * vortexArray[G].lengthVortex.y;
                double c = realGammaArray[alphaIndex][G] * vortexArray[G].lengthVortex.z;

                double VX = streamVinfArray[alphaIndex][G].x;
                double VY = streamVinfArray[alphaIndex][G].y;
                double VZ = streamVinfArray[alphaIndex][G].z;



                double a2 = (VY * c - VZ * b) * density;
                double b2 = (VZ * a - VX * c) * density;
                double c2 = (VX * b - VY * a) * density;



                xa = xa + a2;
                ya = ya + b2;
                za = za + c2;

                liftTmp.append(c2);
                dragTmp.append(a2);
                lateralTmp.append(b2);
                tmp.append(point3d(a2,b2,c2));
                mForce.append(sqrt(a2 * a2 + b2 * b2 + c2 * c2) / areas[i][j]);
            }
        }

        tmpArray.append(tmp);
        double cosAlpha = cos(alphaArray[alphaIndex] / 180 * M_PI);
        double sinAlpha = sin(alphaArray[alphaIndex] / 180 * M_PI);
        double Drag = cosAlpha * xa + sinAlpha * za;
        double Lift = - sinAlpha * xa + cosAlpha * za;
        QVector<double>yLiftTmp;
        //QVector<double>yLiftCoeff;
        QVector<double>yliftForce;
        QVector<double>nYForce;//展向法向力

        //计算每段的升力
        for(int i = 0;i<Ny;i++){

            double tmp1 = 0;
            double tmp2 = 0;
            double tmp3 = 0;


            for(int j = 0;j<Nx;j++){
                tmp1 = tmp1 + liftTmp[j * Ny + i];
                tmp2 = tmp2 + dragTmp[j * Ny + i];
                tmp3 = tmp3 + lateralTmp[j * Ny + i];
            }

            double tmpA = - sinAlpha * tmp2 + cosAlpha * tmp1 ;
            double sumTmp = tmpA / spanArea[i] / density / vinf / vinf * 2;
            //double nTmp = tmp3 / spanArea[i] / density / vinf / vinf * 2;
            double nForce = sqrt(tmp1 * tmp1 + tmp3 * tmp3);
            yLiftTmp.append(sumTmp * (cy[i] + cy[i + 1]) / 2);
            //yLiftCoeff.append(sumTmp / dihedCos[i]);
            //yLiftCoeff.append(sqrt(sumTmp * sumTmp + nTmp * nTmp));
            yliftForce.append(tmpA);
            nYForce.append(nForce);


        }
        spanLift.append(yLiftTmp);

        forceTmpArray.append(point3d(Drag,ya,Lift));
        //spanLiftCoeff.append(yLiftCoeff);
        force.append(yliftForce);

        momentTmp.append(calculateBendingMoment(yliftForce,spanForceYt));
        mForceArray.append(mForce);
        nSpanForce.append(nYForce);
        //qDebug()<<alphaArray[alphaIndex]<<"  r"<<spanLiftCoeff[alphaIndex];
    }
    spanLiftForce = force;
    allForceArray = tmpArray;
    totalForceArray = forceTmpArray;
    spanForce = spanLift;
    //spanLiftCoefficient = spanLiftCoeff;
    spanMonmentArray = momentTmp;
    totalPressure = mForceArray;
    nSpanForceArray = nSpanForce;

}
*/
void wingVLM::computeTreffzDrag(){

    QVector<double>cdArray;
    QVector<double>clArray;
    QVector<double>cnArray;
    QVector<double>cosT;
    QVector<double>sinT;
    QVector<double>dl;
    QVector<QVector<double>>mForceArray;//合力

    double p = 0.5 * density * realArea * vinf * vinf;

    //计算dl
    for(int i = 0;i<Ny;i++){

        double dy2 = pow(trefftzArray[i].xyzA.y - trefftzArray[i].xyzB.y,2);
        double dz2 = pow(trefftzArray[i].xyzA.z - trefftzArray[i].xyzB.z,2);
        double ds = sqrt(dy2 + dz2);

        double cosTmp = abs(trefftzArray[i].xyzA.y - trefftzArray[i].xyzB.y) / ds;
        double sinTmp = sqrt(1 - cosTmp * cosTmp);


        dl.append(ds);
        cosT.append(cosTmp);
        sinT.append(sinTmp);

    }
    //计算dtheta

    for(int i = 0;i<vinfArray.length();i++){
        double dTmp = 0;
        double lTmp = 0;
        double nTmp = 0;
        QVector<double>mForceTmp;
        QVector<double>inducdArray;
        for(int j = 0;j<Ny;j++){
            double vTmp = 0;
            for(int k = 0;k<Ny;k++){
                vTmp = vTmp + vNTrefftzArray[j][k] * matrixGammaArray[i][N - Ny + k];
            }
            dTmp +=  -0.5 * density * vTmp * matrixGammaArray[i][N - Ny + j] * dl[j] ;
            double xTmp = -0.5 * density * vTmp * matrixGammaArray[i][N - Ny + j] * dl[j];
            double yTmp = matrixGammaArray[i][N - Ny + j] * cosT[j] * dl[j] * vinf * density;
            double zTmp = matrixGammaArray[i][N - Ny + j] * sinT[j] * dl[j] * vinf * density;

            lTmp += yTmp;
            nTmp += zTmp;
            mForceTmp.append(sqrt(xTmp * xTmp + yTmp * yTmp + zTmp * zTmp) / p / spanArea[j] * realArea);



        }


        cdArray.append(dTmp / p);
        clArray.append(lTmp / p);
        cnArray.append(nTmp / p);
        mForceArray.append(mForceTmp);
        //qDebug()<<alphaArray[i]<<" :"<<clArray[i]<<"  "<<cdArray[i];

    }



    wingCL = clArray;
    wingCD = cdArray;
    wingCN = cnArray;
    spanLiftCoefficient = mForceArray;



}
double wingVLM::getChoiceTorque(int alpha){


    int begin = Ny / 2 - gridU[num];
    double torqueForce = 0;
    for(int i = 0;i<gridU[0];i++){
        torqueForce += nSpanForceArray[alpha][i] * (spanForceYt[i] - spanForceYt[begin]);
    }
    return -torqueForce;
}
void wingVLM::computeInDesignAlpha(double alpha){


    QVector<double>rAlpha;
    rAlpha<<alpha;
    alphaArray = rAlpha;
    QVector<point3d>rVinf;
    rVinf.append(point3d(vinf * cos(rAlpha[0] / 180 * M_PI),0,vinf * sin(rAlpha[0] / 180 * M_PI)));
    vinfArray = rVinf;

    /*******************计算尾迹***********************/
    //computeWakeVortex();
    /*******************计算影响系数***********************/

    solveInfluenceCoefficient();
    /*******************求解线性方程组*********************/
    computeVLMMatrix();
    computeGamma();

    /********************计算附着涡中点处诱导系数**************************/
    computeVelocity();

    /************************计算净涡强**************************/


    computeTreffzDrag();
    /********************计算气动力********************/
    computeForce(streamVinfArray);
    computeCm();
    //
    double dragTmp = 0;
    double drag = 0;
    double zeroLiftDrag = 0;

    if(isAddXfoilDrag){
        startXfoil();
        smoothOnceDrag();
        //xfoil阻力计算


        for(int i = 0;i<Ny / 2;i++){
            double tmp = (spanDrag[i] + spanDrag[i + 1]) / 2;
            dragTmp += tmp * spanArea[i];
        }
        drag = dragTmp * 2 / realArea;
    }

    if(isAddZeroLiftDrag)
        zeroLiftDrag = viscousCd;







    computeCm();
    //计算颜色矩阵
    computePressureIndex();
    double p = 0.5 * density * vinf * vinf * realArea;
    fixCl = totalForceArray[0].z / p;

    fixCd = totalForceArray[0].x / p + zeroLiftDrag + drag;
    fixCm = -wingCMForce[0] / p / mac;

    std::vector<double> result(5);
    result[0] = fixCl;
    result[1] = fixCd;
    result[2] = fixCm;
    result[3] = fixCl / fixCd;
    result[4] = std::pow(fixCl, 1.5) / fixCd;


    //计算升力参考值的横坐标
    QVector<double>ytTmp;
    for(int i = 0;i<Ny;i++){
        ytTmp.append((yt[i] + yt[i + 1]) / 2);
    }
    spanForceYt = ytTmp;
    //计算理想升力分布
    QVector<double>tmp;
    double yl = abs(2 * fixCl * realArea / abs(spanW[num])/ M_PI);
    for(int j = 0;j<Ny;j++){
        double x = (yt[j] + yt[j + 1]) / 2;
        tmp.append(sqrt((1 - (x * x / (spanW[num] * spanW[num]))) * yl * yl));
    }
    designSpanEllipseLift = tmp;
    designSpanLift = spanForce[0];



        //qDebug()<<"理想"<<tmp;



    emit emitResultValue(result);
}
void wingVLM::startXfoil(){
    int realNy = 0;
    if(isSymmetry)
        realNy = Ny / 2;
    else
        realNy = Ny;

    airfoilSolve *solvers = new airfoilSolve();
    QVector<double>airfoilX;
    xfoilSetting setting;
    setting.nCrit = 9;
    setting.alphaStepSize = 1;
    setting.minAlpha = 0;
    setting.maxAlpha = 10;
    setting.ReType = 1;
    setting.MaType = 1;
    setting.xtrTop = 1;
    setting.xtrBot = 1;
    setting.designALPHA = 1;
    setting.model = 1;
    setting.s_IterLim = 150;
    double temperature = mathSolver.calculateTemperature(height);
    double Viscosity = mathSolver.calculateAirViscosity(temperature);


    for(int i = 0;i<51;i++)
        airfoilX.append(0.5 - cos(static_cast<double>(i) * M_PI / 50 ) * 0.5);


    for(int i = 0;i<realNy + 1;i++){
        airfoilDesign design(cstNum);
        design.buildAirfoilCurve(spanCstArray[i],airfoilX);

        setting.Re = int(cy[i] * vinf / Viscosity);
        setting.Ma = vinf / mathSolver.getSoundSpeed(temperature);
        setting.designCL = spanLiftCoefficient[0][i];
        solvers->importAirfoil(design.newAirfoilData);
        solvers->refreshParaments(setting);
        solvers->solverOnce();
        spanDrag.append(solvers->onceData.cD);
        spanLift.append(solvers->onceData.cL);
    }


    delete solvers;
    solvers = nullptr;
}

void wingVLM::solveStreamLine(const int streamCaseIndex, const double distance,const double verticalOffset,const double t,const double dt){
    streamIndex = streamCaseIndex;
    QVector<point3d>position;


    for(int i = 0;i<Ny;i++){
        position.append(wakeVortexArray[i].xyzA);
    }
    position.append(wakeVortexArray[Ny - 1].xyzB);

    QVector<QVector<point3d>>tmp;

    // 2. 步长放大系数：根据需求调整（建议1.05~1.2，值越大步长增长越快）
    const double s_XFactor = 1.1;
    // 步长上限：避免后期步长过大导致精度丢失（可根据流场范围调整）
    const double maxDeltaTime = dt * 20;


    for(int i = 0;i<Ny + 1;i++){




        int value = double(i)/ Ny * 100;
        emit progressUpdated(value);


        double deltaTime = dt;
        double totalTime = t;
        double currentTime = 0.0;
        QVector<point3d>streamTmp;
        point3d currentPosition(position[i].x - distance,position[i].y,position[i].z + verticalOffset);

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
            //point3d nextPosition = rungeKutta4(currentPosition,deltaTime);

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


void wingVLM::computePressure(const QVector<double> & minArray, const QVector<double> &maxArray){
    QVector<QVector<double>>colors;

    for(int i = 0;i<vinfArray.length();i++){

        QVector<double>tmpColor;


        double maxT = maxArray[i];
        double minT = minArray[i];
        double maxTmp = maxT - minT;
        for(int j = 0;j<N;j++){
            tmpColor.append(1.0 - (-minT + realGammaArray[i][j]) / maxTmp);
        }
        colors.append(tmpColor);



    }

    pressureColorArray = colors;
}
void wingVLM::computePressure(){
    QVector<QVector<double>>colors;



    for(int i = 0;i<vinfArray.length();i++){

        QVector<double>tmpColor;


        double maxT = max(realGammaArray[i]);
        double minT = min(realGammaArray[i]);
        double maxTmp = maxT - minT;
        for(int j = 0;j<N;j++){
            tmpColor.append(1.0 - (-minT + realGammaArray[i][j]) / maxTmp);
        }
        colors.append(tmpColor);



    }

    pressureColorArray = colors;

}

void wingVLM::computePressureIndex(){

    QVector<double>tmpColor;

    double maxT = max(realGammaArray[0]);
    double minT = min(realGammaArray[0]);
    double maxTmp = maxT - minT;
    for(int j = 0;j<N;j++){
        tmpColor.append(1.0 - (-minT + realGammaArray[0][j]) / maxTmp);
    }


    contourArray = tmpColor;
}
void wingVLM::computeDownwashAngle(){

    for(int i = N - Ny;i<N;i++){
        double x = (xyC[i].x + xyD[i].x) / 2;
        double y = (xyC[i].y + xyD[i].y) / 2;
        double z = (xyC[i].z + xyD[i].z) / 2;
        point3d vm = calculateVelocity(point3d(x,y,z));
        qDebug()<<vm.x<<" "<<vm.y<<" "<<vm.z<<""<<atan(vm.z / vm.x) / M_PI * 180;

    }

}
void wingVLM::solveSpanForce(const QVector<QVector<double> > &array,double /*referenceArea*/){
    if(!spanForce.isEmpty()){
        spanForce.clear();
        spanLiftCoefficient.clear();
        spanMonmentArray.clear();
        spanLiftForce.clear();
    }


    for(int i = 0;i<array.length();i++){
        QVector<double>forceTmp;
        QVector<double>coefficientTmp;

        for(int j = 0;j<Ny;j++){
            double dCl = array[i][j] / spanArea[j] / density / vinf / vinf * 2;
            forceTmp.append(dCl * (cy[i] + cy[i + 1]) / 2);
            coefficientTmp.append(dCl);

        }
        spanForce.append(forceTmp);
        spanLiftCoefficient.append(coefficientTmp);
        spanMonmentArray.append(calculateBendingMoment(array[i],spanForceYt));
        spanLiftForce.append(array[i]);
    }

    computeXfoilInput();
    addDrag();

}
point3d wingVLM::calculateVelocity(const point3d& position){


    QVector<double> vxAB, vyAB, vzAB;
    QVector<double> vxBF, vyBF, vzBF;
    QVector<double> vxFC, vyFC, vzFC;
    QVector<double> vxCD, vyCD, vzCD;
    QVector<double> vxDE, vyDE, vzDE;
    QVector<double> vxEA, vyEA, vzEA;
    for(int i = 0;i < N;i++){
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
    int tmp = streamIndex * Ny;
    for(int i = tmp;i < tmp + Ny;i++){
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
    for(int i = 0;i<N + Ny;i++){
        vx.append(vxAB[i] + vxBF[i] + vxFC[i] + vxCD[i] + vxDE[i] + vxEA[i]);
        vy.append(vyAB[i] + vyBF[i] + vyFC[i] + vyCD[i] + vyDE[i] + vyEA[i]);
        vz.append(vzAB[i] + vzBF[i] + vzFC[i] + vzCD[i] + vzDE[i] + vzEA[i]);

    }

    double vxt = 0;
    double vyt = 0;
    double vzt = 0;

    for(int i = 0;i<N;i++){
        vxt = vxt + vx[i] * matrixGammaArray[streamIndex][i];
        vyt = vyt + vy[i] * matrixGammaArray[streamIndex][i];
        vzt = vzt + vz[i] * matrixGammaArray[streamIndex][i];
    }

    for(int i = N - Ny;i<N;i++){
        vxt = vxt + vx[i + Ny] * matrixGammaArray[streamIndex][i];
        vyt = vyt + vy[i + Ny] * matrixGammaArray[streamIndex][i];
        vzt = vzt + vz[i + Ny] * matrixGammaArray[streamIndex][i];
    }

    return point3d(vxt + vinfArray[streamIndex].x,vyt + vinfArray[streamIndex].y,vzt + vinfArray[streamIndex].z);
}
point3d wingVLM::rungeKutta4(const point3d& position,const double deltaTime){

    point3d k1 = calculateVelocity(position);
    point3d position1(position.x + k1.x * (deltaTime / 2),position.y + k1.y * (deltaTime / 2),position.z + k1.z * (deltaTime / 2));
    point3d k2 = calculateVelocity(position1);
    point3d position2(position.x + k2.x * (deltaTime / 2),position.y + k2.y * (deltaTime / 2),position.z + k2.z * (deltaTime / 2));
    point3d k3 = calculateVelocity(position2);
    point3d position3(position.x + k3.x * deltaTime,position.y + k3.y * deltaTime,position.z + k3.z * deltaTime);
    point3d k4 = calculateVelocity(position3);
    double x1 = position.x + (k1.x + k2.x * 2 + k3.x * 2 + k4.x) * (deltaTime / 6);
    double y1 = position.y + (k1.y + k2.y * 2 + k3.y * 2 + k4.y) * (deltaTime / 6);
    double z1 = position.z + (k1.z + k2.z * 2 + k3.z * 2 + k4.z) * (deltaTime / 6);
    return point3d(x1,y1,z1);
}
point3d wingVLM::eulerMethod(const point3d& position, const double deltaTime) {
    // 1. 计算当前位置的速度向量（与RK4的第一步相同）
    point3d velocity = calculateVelocity(position);

    // 2. 欧拉法核心：当前位置 + 速度×步长（直接累加增量）
    double x1 = position.x + velocity.x * deltaTime;
    double y1 = position.y + velocity.y * deltaTime;
    double z1 = position.z + velocity.z * deltaTime;

    return point3d(x1, y1, z1);
}
point3d wingVLM::leyBiotSavart3D(
    const double xr, const double yr, const double zr,  // 控制点坐标
    const double xP, const double yP, const double zP,  // 涡段起点P
    const double xQ, const double yQ, const double zQ  // 涡段终点Q

) {

    bool finite_core = true; // 新增：是否启用有限涡核（对应bound_induced_velocity的finite_core）
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
point3d wingVLM::leyBiotSavart3D(const double xr, const double yr, const double zr, const double xP, const double yP, const double zP, const double xQ, const double yQ, const double zQ){
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
    double rc = abs(spanW[0] * 0.01);
    double Fv = 1 - exp(-1.25463 * (Lrp * Lrp / (rc * rc)));
    //qDebug()<<Fv;

    // 计算最终的下洗速度 VDW
    vdwX = F2x * L2 / (4 * M_PI) * Fv;
    vdwY = F2y * L2 / (4 * M_PI) * Fv;
    vdwZ = F2z * L2 / (4 * M_PI) * Fv;




    return point3d(vdwX,vdwY,vdwZ);
}
*/
void wingVLM::leyBiotSavart3D(const double xr, const double yr, const double zr,
                              const QVector<double> &xP, const QVector<double> &yP, const QVector<double> &zP,
                              const QVector<double> &xQ, const QVector<double> &yQ, const QVector<double> &zQ,
                              QVector<double> &vX, QVector<double> &vY, QVector<double> &vZ) {

    double hl_limit = 1e-7; // 数值稳定性参数，类似MATLAB的hl_limit
    int segmentCount = xP.length();  // 涡段的数量

    for (int i = 0; i < segmentCount; i++) {
        // 计算 r1 和 r2 向量
        double r1x = xr - xP[i];
        double r1y = yr - yP[i];
        double r1z = zr - zP[i];

        double r2x = xr - xQ[i];
        double r2y = yr - yQ[i];
        double r2z = zr - zQ[i];

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
        bool SW_l0 = (LR0 == 0);

        // 计算点到直线距离 Lrp
        double Lrp = sqrt(LF1) / LR0;
        double Ratio_hl = Lrp / LR0;

        // 判断是否满足hl_limit，避免数值奇异性
        bool SW_hl_limit = (Ratio_hl < hl_limit);

        // 计算 F2 = F1 / LF1
        double F2x = F1x / LF1;
        double F2y = F1y / LF1;
        double F2z = F1z / LF1;

        // 计算 L2 = R0 · L1
        double L2 = R0x * L1x + R0y * L1y + R0z * L1z;

        // 计算最终的下洗速度 VDW
        double vdwX = F2x * L2 / (4 * M_PI);
        double vdwY = F2y * L2 / (4 * M_PI);
        double vdwZ = F2z * L2 / (4 * M_PI);

        // 对hl_limit和涡段长度为0的情况进行处理
        if (SW_hl_limit || SW_l0) {
            vdwX = 0.0;
            vdwY = 0.0;
            vdwZ = 0.0;
        }

        // 将计算出的速度存入vX, vY, vZ
        vX.append(vdwX);
        vY.append(vdwY);
        vZ.append(vdwZ);
    }
}




void wingVLM::twistWing(){


    //计算扭转矢量方向
    QVector<point3d>vp;
    QVector<point3d>vn;
    QVector<double>xcoord;
    for(int i = 0;i<Ny / 2 + 1;i++){
        xcoord.append(xAtaque[i] + cy[i] / 4);
    }
    for(int i = 0;i<num;i++){
        int len = gridU[num - i];

        int ind = 0;
        for(int k = 0;k<i;k++){
            ind = gridU[num - k] + ind;
        }

        for(int j = 0;j<len;j++){

            int G = ind + j;


            point3d vnTmp1(xcoord[G],xyvv[0][G].y,xyvv[0][G].z);
            point3d vnTmp2(xcoord[G],xyvv[0][G + 1].y,xyvv[0][G + 1].z);
            point3d vnTmp = calculateVector(vnTmp1,vnTmp2);
            vp.append(vnTmp1);
            vn.append(vnTmp);

        }
    }

    point3d vntmp1(xcoord[Ny / 2],xyvv[0][Ny / 2].y,xyvv[0][Ny / 2].z);
    point3d vntmp2(xcoord[Ny / 2],xyvv[0][Ny / 2].y + 0.1,xyvv[0][Ny / 2].z);
    // point3d vntmp1(xyvv[0][Ny / 2].x,xyvv[0][Ny / 2].y,xyvv[0][Ny / 2].z);
    // point3d vntmp2(xyvv[0][Ny / 2].x,xyvv[0][Ny / 2].y + 0.1,xyvv[0][Ny / 2].z);
    point3d vntmp = calculateVector(vntmp1,vntmp2);
    vp.append(vntmp1);
    vn.append(vntmp);

    for(int i = Ny / 2 - 1;i>=0;i--){
        point3d vnTmp1 = vp[i];
        point3d vnTmp = vn[i];
        vnTmp1.y = -vnTmp1.y;
        vnTmp.y = -vnTmp.y;
        vp.append(vnTmp1);
        vn.append(vnTmp);
    }


    //计算扭转角
    QVector<double>th;

    for(int i = 0;i<num;i++){
        int len = gridU[num - i];


        int ind = 0;
        for(int k = 0;k<i;k++){
            ind = gridU[num - k] + ind;
        }
        for(int j = 0;j<len;j++){

                int G = ind + j;


            double k = (twistAngleW[num - i - 1] - twistAngleW[num - i]) / (spanW[num - i] - spanW[num - i - 1]);
            double t = (twistAngleW[num - i] +  k *  (spanYt[G] +  spanW[num - i]))/ 180 * M_PI;
            th.append(t);
        }
    }
    th.append(twistAngleW[0] / 180 * M_PI);

    //th.append(twistAngleW[0]);

    for(int i = Ny / 2 - 1;i>=0;i--){

        double t = -th[i];
        th.append(t);
    }




    //扭转机翼
    for(int i =0;i<Nx + 1;i++){
        for(int j = 0;j<Ny + 1;j++){
            xyvv[i][j] = rotatePointAroundAxis(xyvv[i][j],vp[j],vn[j],th[j]);
            xyvvUpper[i][j] = rotatePointAroundAxis(xyvvUpper[i][j],vp[j],vn[j],th[j]);
            xyvvLower[i][j] = rotatePointAroundAxis(xyvvLower[i][j],vp[j],vn[j],th[j]);
        }
    }


    //强制扭转
    double tmpA = yTwistAngle / 180 * M_PI;
    point3d vnTmpA(xyvv[0][0].x,0,xyvv[0][0].z);
    point3d vnTmpB(xyvv[0][0].x,1,xyvv[0][0].z);
    point3d vnTmpC = calculateVector(vnTmpA,vnTmpB);
    for(int i =0;i<Nx + 1;i++){
        for(int j = 0;j<Ny + 1;j++){
            xyvv[i][j] = rotatePointAroundAxis(xyvv[i][j],vnTmpA,vnTmpC,tmpA);
            xyvvUpper[i][j] = rotatePointAroundAxis(xyvvUpper[i][j],vnTmpA,vnTmpC,tmpA);
            xyvvLower[i][j] = rotatePointAroundAxis(xyvvLower[i][j],vnTmpA,vnTmpC,tmpA);
        }
    }



}
void wingVLM::twistHalfWing(){
    //计算扭转矢量方向
    QVector<point3d>vp;
    QVector<point3d>vn;
    QVector<double>xcoord;
    for(int i = 0;i<Ny + 1;i++){
        xcoord.append(xAtaque[i] + cy[i] / 4);
    }

    for(int i = 0;i<num;i++){
        int len = gridU[num - i];
        int ind = 0;
        for(int k = 0;k<i;k++){
            ind = gridU[num - k] + ind;
        }

        for(int j = 0;j<len;j++){
            int G = ind + j;


            point3d vnTmp1(xcoord[G],xyvv[0][G].y,xyvv[0][G].z);
            point3d vnTmp2(xcoord[G + 1],xyvv[0][G + 1].y,xyvv[0][G + 1].z);
            point3d vnTmp = calculateVector(vnTmp1,vnTmp2);
            vp.append(vnTmp1);
            vn.append(vnTmp);

        }
    }

    point3d vntmp1(xcoord[Ny],xyvv[0][Ny].y,xyvv[0][Ny].z);
    point3d vntmp2(xcoord[Ny],xyvv[0][Ny].y + 0.1,xyvv[0][Ny].z);
    point3d vntmp = calculateVector(vntmp1,vntmp2);
    vp.append(vntmp1);
    vn.append(vntmp);

    //计算扭转角
    QVector<double>th;

    for(int i = 0;i<num;i++){
        int len = gridU[num - i];
        int ind = 0;
        for(int k = 0;k<i;k++){
            ind = gridU[num - k] + ind;
        }
        for(int j = 0;j<len;j++){
            int G = ind + j;
            double k = (twistAngleW[num - i - 1] - twistAngleW[num - i]) / (spanW[num - i] - spanW[num - i - 1]);
            double t = (twistAngleW[num - i] +  k *  (spanYt[G] +  spanW[num - i]))/ 180 * M_PI;
            th.append(t);
        }
    }
    th.append(twistAngleW[0] / 180 * M_PI);

    //扭转机翼
    for(int i =0;i<Nx + 1;i++){
        for(int j = 0;j<Ny + 1;j++){
            xyvv[i][j] = rotatePointAroundAxis(xyvv[i][j],vp[j],vn[j],th[j]);
            xyvvUpper[i][j] = rotatePointAroundAxis(xyvvUpper[i][j],vp[j],vn[j],th[j]);
            xyvvLower[i][j] = rotatePointAroundAxis(xyvvLower[i][j],vp[j],vn[j],th[j]);
        }
    }

    //强制扭转
    double tmpA = yTwistAngle / 180 * M_PI;
    point3d vnTmpA(xyvv[0][0].x,0,xyvv[0][0].z);
    point3d vnTmpB(xyvv[0][0].x,1,xyvv[0][0].z);
    point3d vnTmpC = calculateVector(vnTmpA,vnTmpB);
    for(int i =0;i<Nx + 1;i++){
        for(int j = 0;j<Ny + 1;j++){
            xyvv[i][j] = rotatePointAroundAxis(xyvv[i][j],vnTmpA,vnTmpC,tmpA);
            xyvvUpper[i][j] = rotatePointAroundAxis(xyvvUpper[i][j],vnTmpA,vnTmpC,tmpA);
            xyvvLower[i][j] = rotatePointAroundAxis(xyvvLower[i][j],vnTmpA,vnTmpC,tmpA);
        }
    }

}

void wingVLM::setDihedral(double angle){
    dihedralAngleW[num - 2] = angle;
    qDebug()<<dihedralAngleW;
}

void wingVLM::dihedralWing(){

    QVector<double>Dihedral;
    for(int i = 0;i<dihedralAngleW.length();i++){
        Dihedral.append(-dihedralAngleW[i] / 180 * M_PI);
    }

    QVector<double>realDihedral;
    realDihedral.append(Dihedral[0]);
    for(int i = 0;i<num;i++){
        double tmp = Dihedral[i + 1] - Dihedral[i];
        realDihedral.append(tmp);
    }

    int Ny2 = Ny / 2;
    int index = 0;
    for(int i = 0;i<num;i++){
        if(i != 0)
            index = gridU[num - i] + index;
        point3d vnTmp1(xyvv[0][Ny2 - index].x,xyvv[0][Ny2 - index].y,xyvv[0][Ny2 - index].z);
        point3d vnTmp2(xyvv[Nx][Ny2 - index].x,xyvv[Nx][Ny2 - index].y,xyvv[Nx][Ny2 - index].z);
        point3d vnTmp = calculateVector(vnTmp1,vnTmp2);
        for(int j =0;j<Nx + 1;j++){
            for(int k = 0;k<Ny2 - index;k++){
                xyvv[j][k] = rotatePointAroundAxis(xyvv[j][k],vnTmp1,vnTmp,realDihedral[i]);
                xyvvUpper[j][k] = rotatePointAroundAxis(xyvvUpper[j][k],vnTmp1,vnTmp,realDihedral[i]);
                xyvvLower[j][k] = rotatePointAroundAxis(xyvvLower[j][k],vnTmp1,vnTmp,realDihedral[i]);
            }
        }
    }

    for(int j =0;j<Nx + 1;j++){
        for(int k = Ny2 + 1;k<Ny + 1;k++){
            xyvv[j][k].x = xyvv[j][Ny - k].x;
            xyvv[j][k].y = -xyvv[j][Ny - k].y;
            xyvv[j][k].z = xyvv[j][Ny - k].z;

            xyvvUpper[j][k].x = xyvvUpper[j][Ny - k].x;
            xyvvUpper[j][k].y = -xyvvUpper[j][Ny - k].y;
            xyvvUpper[j][k].z = xyvvUpper[j][Ny - k].z;

            xyvvLower[j][k].x = xyvvLower[j][Ny - k].x;
            xyvvLower[j][k].y = -xyvvLower[j][Ny - k].y;
            xyvvLower[j][k].z = xyvvLower[j][Ny - k].z;
        }
    }

}

void wingVLM::dihedralHalfWing(){
    //qDebug()<<dihedralAngleW;
    QVector<double>Dihedral;
    for(int i = 0;i<dihedralAngleW.length();i++){
        Dihedral.append((-90 - dihedralAngleW[i]) / 180 * M_PI);
    }




    QVector<double>realDihedral;
    realDihedral.append(Dihedral[0]);
    for(int i = 0;i<num;i++){
        double tmp = Dihedral[i + 1] - Dihedral[i];
        realDihedral.append(tmp);
    }


    int index = 0;
    for(int i = 0;i<num;i++){

        if(i != 0)
            index = gridU[num - i] + index;

        point3d vnTmp1(xyvv[0][Ny - index].x,xyvv[0][Ny - index].y,xyvv[0][Ny - index].z);
        point3d vnTmp2(xyvv[Nx][Ny - index].x,xyvv[Nx][Ny - index].y,xyvv[Nx][Ny - index].z);
        point3d vnTmp = calculateVector(vnTmp1,vnTmp2);
        for(int j =0;j<Nx + 1;j++){
            for(int k = 0;k<Ny - index + 1;k++){
                xyvv[j][k] = rotatePointAroundAxis(xyvv[j][k],vnTmp1,vnTmp,realDihedral[i]);
                xyvvUpper[j][k] = rotatePointAroundAxis(xyvvUpper[j][k],vnTmp1,vnTmp,realDihedral[i]);
                xyvvLower[j][k] = rotatePointAroundAxis(xyvvLower[j][k],vnTmp1,vnTmp,realDihedral[i]);
            }
        }
    }


}
void wingVLM::translationWing(){
    for(int i =0;i<Nx + 1;i++){
        for(int j = 0;j<Ny + 1;j++){
            xyvv[i][j] =  xyvv[i][j] + point3d(xOffset,yOffset,zOffset);
            xyvvUpper[i][j] = xyvvUpper[i][j] + point3d(xOffset,yOffset,zOffset);
            xyvvLower[i][j] = xyvvLower[i][j] + point3d(xOffset,yOffset,zOffset);
        }
    }

    for(int i = 0;i<Ny + 1;i++){
        xAtaque[i] += xOffset;
        xSalida[i] += xOffset;
    }


}
double wingVLM::computeZ(const int index, const double x, const double c){
    double z = 0;
    for(int i = 1;i<cstNum + 1;i++){
        z = realCst[index][i - 1] * std::pow(x,0.5) * std::pow(1 - x,1) * nchoosek(cstNum - 1,i-1) * std::pow(1 - x,cstNum - i) * std::pow(x,i - 1) + z;
    }


    return z * c;
}
double wingVLM::computeUpperZ(const int index, const double x, const double c){
    double z = 0;
    for(int i = 1;i<cstNum + 1;i++){
        z = spanCstArray[index][i - 1] * std::pow(x,0.5) * std::pow(1 - x,1) * nchoosek(cstNum - 1,i-1) * std::pow(1 - x,cstNum - i) * std::pow(x,i - 1) + z;
    }


    return (z + upperYte[index] * x) * c;
}
double wingVLM::computeLowerZ(const int index, const double x, const double c){
    double z = 0;
    for(int i = 1;i<cstNum + 1;i++){
        z = spanCstArray[index][cstNum * 2 - i] * std::pow(x,0.5) * std::pow(1 - x,1) * nchoosek(cstNum - 1,i-1) * std::pow(1 - x,cstNum - i) * std::pow(x,i - 1) + z;
    }
    return (z + lowerYte[index] * x) * c;
}
void wingVLM::interpolationAirfoil(){





    double yte = ctYte / chordLengthW[0];
    if(!airfoilIsEmpty){
        realCst.append(cst[num]);
        spanCstArray.append(airfoilCSTArray[num]);
        if(yteType){
            upperYte.append(yte / 2);
            lowerYte.append(-yte / 2);
        }else{
            upperYte.append(yte / 2 / cy[0] * chordLengthW[0]);
            lowerYte.append(-yte / 2 / cy[0] * chordLengthW[0]);
        }

        for(int i = 0;i<num;i++){
            int len = gridU[num - i];
            int ind = 0;
            for(int k = 0;k<i;k++){
                ind = gridU[num - k] + ind;
             }
            for(int j = 0;j<len;j++){
                QVector<double>tmp1;
                QVector<double>tmp2;

                int G = ind + j + 1;

                for(int k = 0;k<cstNum;k++){
                    tmp1.append(-(cst[num - i][k] - cst[num - i - 1][k]) / (spanW[num - i] - spanW[num - i - 1]) * (spanYt[G] + spanW[num - i - 1])   + cst[num - i - 1][k]);
                }
                for(int k = 0;k<cstNum * 2;k++){
                    tmp2.append(-(airfoilCSTArray[num - i][k] - airfoilCSTArray[num - i - 1][k]) / (spanW[num - i] - spanW[num - i - 1]) * (spanYt[G] + spanW[num - i - 1])   + airfoilCSTArray[num - i - 1][k]);
                }


                realCst.append(tmp1);
                spanCstArray.append(tmp2);
                //
                if(yteType){
                    upperYte.append(yte / 2);
                    lowerYte.append(-yte / 2);
                }else{
                    upperYte.append(yte / 2 / cy[G] * chordLengthW[0]);
                    lowerYte.append(-yte / 2 / cy[G] * chordLengthW[0]);
                }

            }

        }
        if(isSymmetry){
            for(int i = Ny / 2 - 1;i>=0;i--){
                realCst.append(realCst[i]);
                spanCstArray.append(spanCstArray[i]);
                //
                upperYte.append(upperYte[i]);
                lowerYte.append(lowerYte[i]);
            }
        }


    }else{
        QVector<double>noCst;
        for(int i = 0;i<cstNum;i++){
            noCst.append(0);
        }
        for(int i = 0;i<Ny + 1;i++){
            realCst.append(noCst);
        }
        QVector<double>tmp;
        tmp<<0.1522<<0.1064<<0.0915<<0.1036<<0.0557<<0.1045<<-0.1522<<-0.1064<<-0.0915<<-0.1036<<-0.0557<<-0.1045;
        for(int i = 0;i<Ny + 1;i++){
            spanCstArray.append(tmp);
            upperYte.append(0.002);
            lowerYte.append(-0.002);
        }

    }
}
void wingVLM::computeMeanCamberAirfoil(){
    if(!airfoilArray.isEmpty()){
        QVector<QVector<double>>cstTmp1;
        QVector<QVector<double>>cstTmp2;

        //QVector<QVector<double>>airfoilCSTTmp;
        for(int i = 0;i<airfoilArray.length();i++){
            airfoilDesign designer(cstNum);
            designer.buildAirfoilMeanCamberBenrnstein(airfoilArray[i]);           
            cstTmp1.append(designer.meanCstParameter);
            cstTmp2.append(designer.cstParameter);
            //airfoilCSTTmp.append(designer.cstParameter);
        }
        cst = cstTmp1;
        airfoilCSTArray = cstTmp2;
        //airfoilCSTArray = airfoilCSTTmp;
        airfoilIsEmpty = false;
    }
}
void wingVLM::computeMeanCamberCst(){
    if(!airfoilCSTArray.isEmpty()){
        QVector<QVector<double>>cstTmp;
        for(int i = 0;i<airfoilCSTArray.length();i++){
            QVector<double>tmp;
            for(int j = 0;j<cstNum;j++){
                tmp.append((airfoilCSTArray[i][j] + airfoilCSTArray[i][cstNum * 2  - j - 1]) / 2 );
            }
            cstTmp.append(tmp);
        }
        cst = cstTmp;
        airfoilIsEmpty = false;
    }
}
void wingVLM::computeCst(){


}
void wingVLM::smoothOnceDrag(){
    double error = 0.01;
    int len = Ny / 2 + 1;
    QVector<int>badValueIndex;
    QVector<double>spanIndexTmp;
    QVector<double>spanDragTmp;
    for(int i = 0;i<len;i++){

        if(abs(spanLiftCoefficient[0][i] - spanLift[i]) > error ){
            badValueIndex.append(i);
        }else{
            spanIndexTmp.append(spanYt[i]);
            spanDragTmp.append(spanDrag[i]);
        }
    }



    for(int i = 0;i<badValueIndex.length();i++){
        int G = badValueIndex[i];
        spanDrag[G] = myMath::linearInterpolation(spanIndexTmp,spanDragTmp,spanYt[G]);
    }

}
void wingVLM::computeXfoilInput(){

    double temperature = mathSolver.calculateTemperature(height);
    double Viscosity = mathSolver.calculateAirViscosity(temperature);
    QVector<double>spanArray;
    for(int i = 0;i<num;i++)
        spanArray.append(abs(spanW[i] - spanW[i + 1]));


    for(int i = 0;i<vinfArray.length();i++){
        QVector<airfoilInterInput>tmpArray;
        for(int j = 0;j<Ny;j++){
            airfoilInterInput tmp;
            int ind = num - 1 - indexArray[j];
            tmp.nameA = nameArray[ind + 1];
            tmp.indexA = ind + 1;
            tmp.ratioA = abs(abs(vortexArray[j].xyzCenter.y) - spanW[ind])/ spanArray[ind];
            tmp.ratioB = 1 - tmp.ratioA;
            tmp.nameB = nameArray[ind];
            tmp.indexB = ind;
            tmp.re = int(cy[j] * vinf / Viscosity);
            tmp.designCl = spanLiftCoefficient[i][j];

            tmpArray.append(tmp);
        }
        inTerInputArray.append(tmpArray);



    }
    /*
    for(int i = 0;i<Ny/2;i++){
        qDebug()<<inTerInputArray[0][i].ratioA<<" "<<inTerInputArray[0][i].ratioB;
        qDebug()<<inTerInputArray[0][i].re;
        qDebug()<<inTerInputArray[0][i].designCl;

    }
    */
}


double wingVLM::getViscosityDrag(int m, int n){

    int m1;//雷诺数和升力系数对应的索引


    if(inTerInputArray[m][n].re < 10000){
        m1 = 1;
    }else if(inTerInputArray[m][n].re < 1490000){
        m1 = int(inTerInputArray[m][n].re / 5000);
    }else{
        m1 = 298;
    }
    int re = m1 * 5000;
    int index1 = findStringIndexCaseInsensitive(inTerInputArray[m][n].nameA);
    int index2 = findStringIndexCaseInsensitive(inTerInputArray[m][n].nameB);



    QVector<double>cd3 = interDataArray[index1][m1].cdData;
    QVector<double>cd4 = interDataArray[index2][m1].cdData;
    QVector<double>cl3 = interDataArray[index1][m1].clData;
    QVector<double>cl4 = interDataArray[index2][m1].clData;


    QVector<double>cd1 = interDataArray[index1][m1 - 1].cdData;
    QVector<double>cd2 = interDataArray[index2][m1 - 1].cdData;
    QVector<double>cl1 = interDataArray[index1][m1 - 1].clData;
    QVector<double>cl2 = interDataArray[index2][m1 - 1].clData;


    QVector<double>cd5 = interDataArray[index1][m1 + 1].cdData;
    QVector<double>cd6 = interDataArray[index2][m1 + 1].cdData;
    QVector<double>cl5 = interDataArray[index1][m1 + 1].clData;
    QVector<double>cl6 = interDataArray[index2][m1 + 1].clData;



    double value1 = mathSolver.threePointInterpolation(cl3,cd3,inTerInputArray[m][n].designCl);
    double value2 = mathSolver.threePointInterpolation(cl4,cd4,inTerInputArray[m][n].designCl);


    //double value3 = mathSolver.interpolateWithLinear(cd1,cl1,inTerInputArray[m][n].designCl);
    //double value4 = mathSolver.interpolateWithLinear(cd2,cl2,inTerInputArray[m][n].designCl);

    double value5 = mathSolver.threePointInterpolation(cl5,cd5,inTerInputArray[m][n].designCl);
    double value6 = mathSolver.threePointInterpolation(cl6,cd6,inTerInputArray[m][n].designCl);

    double real1 = value1 + (value5 - value1) / 5000 *(inTerInputArray[m][n].re - re);
    double real2 = value2 + (value6 - value2) / 5000 *(inTerInputArray[m][n].re - re);



    double value = real1 * inTerInputArray[m][n].ratioA + real2 * inTerInputArray[m][n].ratioB;
    //double value = value1 * inTerInputArray[m][n].ratioA + value2 * inTerInputArray[m][n].ratioB;


    return value;

}
int wingVLM::findStringIndexCaseInsensitive(const QString& target) {
    for (int i = 0; i < nameArray.size(); ++i) {
        if (nameArray[i].compare(target, Qt::CaseInsensitive) == 0) {
            return i;
        }
    }
    return 0;  // 未找到
}
double wingVLM::sum(const QVector<QVector<double>> &a){
    double tmp = 0;
    for(int i = 0;i<a.length();i++){
        for(int j = 0;j<a[i].length();j++){
            tmp = a[i][j] + tmp;
        }
    }

    return tmp;

}
bool wingVLM::hasEmptyRow() {
    // 先检查数组是否为空（没有任何行）
    if (interDataArray.empty()) {
        return false; // 数组为空，返回false
    }

    // 遍历每一行
    for (const auto& row : interDataArray) {
        // 检查当前行是否为空
        if (!row.isEmpty()) {
            return true; // 找到空行，返回false
        }
    }

    return true; // 所有行都非空，返回true
}
void wingVLM::clearAllValue(){


    if(!yt.isEmpty()){
        vortexArray.clear();
        wakeVortexArray.clear();
        trefftzArray.clear();
        vNArray.clear();
        vinxArray.clear();
        vinyArray.clear();
        vinzArray.clear();
        inTerInputArray.clear();





        xyA.clear();xyB.clear();xyC.clear();xyD.clear();
        contourArray.clear();


        wingCL.clear();
        wingCD.clear();
        wingCM.clear();
        wingCN.clear();
        spanForceYt.clear();



        yt.clear();
        cy.clear();

        xAtaque.clear();
        xSalida.clear();

        indexArray.clear();



        psiTan.clear();
        xyvv.clear();

        cm.clear();
        xp.clear();
        xp2.clear();
        yp.clear();
        yp2.clear();
        zp.clear();
        zp2.clear();

        xp3.clear();
        xp4.clear();
        xp5.clear();
        xp6.clear();
        yp3.clear();
        yp4.clear();
        yp5.clear();
        yp6.clear();
        zp3.clear();
        zp4.clear();
        zp5.clear();
        zp6.clear();








        //Xn.clear();Yn.clear();Zn.clear();



        areas.clear();
        hs.clear();

        spanDragArray.clear();
        spanLiftArray.clear();
        spanDrag.clear();

        spanCstAllArray.clear();
        realCst.clear();
        spanInputArray.clear();
        xCur.clear();
        meshA.clear();
        meshB.clear();
        meshC.clear();
        meshD.clear();
        xyvvLower.clear();
        xyvvUpper.clear();
        upperYte.clear();
        lowerYte.clear();
    }
}
void wingVLM::zeros(){
    Ny = 0;
    meshNum = 0;
    vortexArray.clear();
    wakeVortexArray.clear();
    trefftzArray.clear();

}
QVector<double> wingVLM::calculateBendingMoment(const QVector<double>& spanLoads, const QVector<double>& spanLocations) {
    QVector<double>tmp;
    int len = spanLocations.length() / 2;
    for(int i = 0;i<len;i++){
        double tmp1 = 0;
        for(int j = 0;j<len - i;j++){
            tmp1 += (spanYt[len - i - 1] - spanLocations[j]) * spanLoads[j];
        }
        tmp.append(tmp1);
    }
    tmp.append(0);
    std::reverse(tmp.begin(), tmp.end());
    for(int i = len - 1;i>=0;i--)
        tmp.append(tmp[i]);


    return tmp;
}

point3d wingVLM::rotatePoint3d(point3d v,  point3d axis, const double theta){
    double cosTheta = cos(theta);
    double sinTheta = sin(theta);
    double axisLength = sqrt(axis.x * axis.x + axis.y * axis.y + axis.z * axis.z);
    axis.x /= axisLength;
    axis.y /= axisLength;
    axis.z /= axisLength;
    point3d cross = crossProduct(axis, v);
    double x1 = v.x * cosTheta + cross.x * sinTheta + axis.x * dotProduct(axis, v) * (1 - cosTheta);
    double y1 = v.y * cosTheta + cross.y * sinTheta + axis.y * dotProduct(axis, v) * (1 - cosTheta);
    double z1 = v.z * cosTheta + cross.z * sinTheta + axis.z * dotProduct(axis, v) * (1 - cosTheta);
    point3d rotated(x1,y1,z1);
    return rotated;
}
point3d wingVLM::crossProduct(const point3d a, const point3d b){
    return point3d(a.y * b.z - a.z * b.y,
                   a.z * b.x - a.x * b.z,
                   a.x * b.y - a.y * b.x);

}
double wingVLM::dotProduct(const point3d a, const point3d b){
    return a.x * b.x + a.y * b.y + a.z * b.z;
}
point3d wingVLM::calculateVector(point3d P1, point3d P2){
    point3d a(P2.x - P1.x, P2.y - P1.y, P2.z - P1.z);
    return normalizeVector(a);
}
point3d wingVLM::calculateVectorB(point3d P1, point3d P2){
    return point3d(P2.x - P1.x, P2.y - P1.y, P2.z - P1.z);
}
point3d wingVLM::normalizeVector(point3d vec){
    double length = sqrt(vec.x * vec.x + vec.y * vec.y + vec.z * vec.z);
    return point3d(vec.x / length, vec.y / length, vec.z / length);
}
double wingVLM::vectorMagnitude(const point3d& vector) {
    return std::sqrt(vector.x * vector.x + vector.y * vector.y + vector.z * vector.z);
}

QVector<double> wingVLM::generateExponentialGrid(const int numPoints, const double minVal, const double maxVal, const double expFactor) {
    QVector<double> grid(numPoints);
    for (int i = 0; i < numPoints; ++i) {
        double ratio = static_cast<double>(i) / (numPoints - 1); // 从0到1的比例
        grid[i] = minVal + (maxVal - minVal) * (std::exp(expFactor * ratio) - 1) / (std::exp(expFactor) - 1);
    }
    return grid;
}
point3d wingVLM::rotatePointAroundAxis(const point3d& point, const point3d& axisPoint, const point3d& axisDir, double angle) {
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
    double cosA = cos(angle);
    double sinA = sin(angle);
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

double wingVLM::calculateTriangleArea(const point3d& A, const point3d& B, const point3d& C){
    point3d AB = calculateVectorB(A, B);
    point3d AC = calculateVectorB(A, C);

    // 计算叉积
    point3d cross = crossProduct(AB, AC);

    // 叉积模的一半即为三角形的面积
    return vectorMagnitude(cross) / 2.0;
}
double wingVLM::calculateQuadrilateralArea(const point3d& A, const point3d& B, const point3d& C, const point3d& D) {
    // 计算两个三角形的面积
    double area1 = calculateTriangleArea(A, B, C);
    double area2 = calculateTriangleArea(A, C, D);

    // 总面积
    return area1 + area2;
}
int wingVLM::nchoosek(const int order,const int index) {

    int result = 1;
    for(int i = 0;i < index;i++) {
        result = result * (order - i) / (i + 1);
    }
    return result;
}
double wingVLM::max(const QVector<double>&a){
    if (a.isEmpty()) {
        throw std::runtime_error("The QVector is empty");
    }
    return *std::max_element(a.begin(), a.end());
}
double wingVLM::min(const QVector<double>&a){
    if (a.isEmpty()) {
        throw std::runtime_error("The QVector is empty");
    }
    return *std::min_element(a.begin(), a.end());
}
double wingVLM::interpolateQuadratic(double x0, double x1, double x2, double y0, double y1, double y2, double x) {
    // Calculate the coefficients of the quadratic polynomial
    double a = (y0 * (x1 - x2) + y1 * (x2 - x0) + y2 * (x0 - x1)) /
               ((x0 - x1) * (x0 - x2) * (x1 - x2));
    double b = (y0 * (x2 * x2 - x1 * x1) + y1 * (x0 * x0 - x2 * x2) + y2 * (x1 * x1 - x0 * x0)) /
               ((x0 - x1) * (x0 - x2) * (x1 - x2));
    double c = (y0 * (x1 * x2 * (x1 - x2)) + y1 * (x2 * x0 * (x2 - x0)) + y2 * (x0 * x1 * (x0 - x1))) /
               ((x0 - x1) * (x0 - x2) * (x1 - x2));

    // Calculate the interpolated value at x
    return a * x * x + b * x + c;
}
void wingVLM::setZeroLiftDrag(bool tmp){
    isAddZeroLiftDrag = tmp;
}
void wingVLM::setXfoilDrag(bool tmp1){
    isAddXfoilDrag = tmp1;
}
void wingVLM::setUseLibraries(bool tmp1){
    isUseLibraries = tmp1;
}

