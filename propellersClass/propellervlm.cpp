#include "propellervlm.h"
#include "AirfoilClass/airfoildesign.h"
#include <qmath.h>
#include <Eigen/Sparse>
#include <Eigen/Dense>
#include <QFuture>
#include <QtConcurrent/QtConcurrent>
#include <qDebug>

#if defined(_MSC_VER) && (_MSC_VER >= 1600)
# pragma execution_character_set("utf-8")
#endif
propellerVLM::propellerVLM()
{


}
void propellerVLM::testProp(){
    wingDefinition tmp;
    QVector<double>a;//翼展
    a<<0.1<<0.2<<0.3<<0.4<<0.5<<0.6<<0.7<<0.8<<0.9<<1.0;
    QVector<double>b;//弦长
    b<<0.1<<0.15<<0.15<<0.15<<0.2<<0.25<<0.2<<0.2<<0.2<<0.2;
    QVector<double>c;//扭转
    c<<30<<28<<26<<24<<22<<20<<18<<16<<14<<12;
    QVector<double>d;//偏移
    d<<0<<0<<0<<0<<0<<0<<0<<0<<0<<0;
    QVector<double>e;
    e<<0<<0<<0<<0<<0<<0<<0<<0<<0<<0;


    QVector<int>f;//y网格
    f<<2<<2<<2<<2<<2<<2<<2<<2<<2<<2;
    QVector<int>g;//x网格
    g<<10<<10<<10<<10<<10<<10<<10<<10<<10<<10;

    tmp.spanW = a;
    tmp.chordLengthW = b;
    tmp.twistAngleW = c;
    tmp.offsetLengthW = d;
    tmp.dihedralAngleW = e;

    tmp.gridU = f;
    tmp.gridV = g;

    tmp.vMeshType = 1.0;
    tmp.uMeshType = 1.0;
    tmp.cstNum = 6;
    tmp.airfoilInputType = false;
    tmp.yteType = false;
    tmp.ctYte = 1.0;

    QVector<QVector<double>>cstTmp;
    QVector<double>cstT;
    for(int i = 0;i<12;i++)
        cstT.append(0);
    for(int i = 0;i<tmp.gridU.length();i++)
        cstTmp.append(cstT);

    tmp.cstArray = cstTmp;
    rpmArray.append(rpm);


    initialGeometry(tmp);

    computeGeometryMesh();
    initial3DMesh();


    twistWing();

    generateVLMMesh();

    generateWakeMesh();

    generate3DMesh();

    initialVinfArray();

    computeVector();

    solveInfluenceCoefficientArray();
    computeVLMMatrixArray();


    computeGamma();
    computeVelocity();
    computeForce();





    //airfoilIsEmpty = true;


}
void propellerVLM::initialModel(const wingDefinition& propData){
    rpmArray.append(8000);
    rpmArray.append(10000);
    rpmArray.append(12000);

    vArray.append(-10);
    vArray.append(-15);
    vArray.append(-20);
    clearAllValue();
    initialGeometry(propData);

    computeGeometryMesh();
    initial3DMesh();


    twistWing();

    generateVLMMesh();

    generateWakeMesh();
    generate3DMesh();

    initialVinfArray();

    computeVector();
    rotationalCopyValue();


}
void propellerVLM::initialAnalyse(const wingDefinition&propData,const propVLMSetting&b){

    QVector<double>a;
    for(int i = 0;i<b.vinfArray.length();i++)
        a.append(-b.vinfArray[i]);


    rpmArray = b.rpmArray;
    vArray = a;
    height = b.height;
    wakeStep = b.wakeStep * 2 + 1;
    isAnalyse = false;

    clearAllValue();
    initialGeometry(propData);

    computeGeometryMesh();
    initial3DMesh();


    twistWing();

    generateVLMMesh();
    mergeMesh();

    generateWakeMesh();
    generate3DMesh();

    initialVinfArray();

    computeVector();
    rotationalCopyValue();


}
void propellerVLM::solver(){


    solveInfluenceCoefficientArray();

    computeVLMMatrixArray();



    computeGamma();

    computeVelocity();

    computeForce();
    computePressure();

    addDrag();

    dealResult();
    isAnalyse = true;



    emit emitAnalyseLog("计算完成!");
    emit emitProgressValue(100);

}
void propellerVLM::generateSimpleGeometry(){

    //计算偏移距离
    if(!offsetLengthW.isEmpty())
        offsetLengthW.clear();
    for(int i = 0;i<chordLengthW.length();i++){
        double tmp = chordLengthW[i] * 0.25;
        offsetLengthW.append(-tmp);
    }
}
void propellerVLM::initialGeometry(const wingDefinition&wing){
    /*******************初始化变量*******************/
    gridU = wing.gridU;
    gridV = wing.gridV;
    spanW = wing.spanW;

    chordLengthW = wing.chordLengthW;
    //dihedralAngleW = wing.dihedralAngleW;
    twistAngleW = wing.twistAngleW;
    offsetLengthW = wing.offsetLengthW;
    //cst = wing.airfoilMeanCamberCST;
    airfoilArray = wing.airfoilArray;
    meshRatioX = wing.vMeshType;
    meshRatioY = wing.uMeshType;
    airfoilInputType = wing.airfoilInputType;
    airfoilCSTArray = wing.cstArray;
    //cstNum = wing.cstNum;
    yteType = wing.yteType;
    ctYte = wing.ctYte;
    propNum = wing.num;
    if(wing.cstNum > 12)
        cstNum = wing.cstNum;



    mac = wing.realChord;

    density = mathSolver.calculateAirDensity(height);
    /*******************初始化网格*******************/
    if(gridV.length() > 0){
        Ny = 0;
        Nx = gridV[0];
        num = gridU.length() - 1;//
        for(int i = 0;i<num;i++){
            Ny = gridU[num - i] + Ny;
        }
        //Ny = Ny * 2;
        N = Nx * Ny;

        meshNum = Ny * gridV[0];
    }
}
void propellerVLM::computeGeometryMesh(){
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

    /*********************定义展向网格生成方式*********************/
    if(meshRatioY != 1.0){

        yt.append(-spanW[num]);
        cy.append(chordLengthW[num]);
        double psi = (offsetLengthW[num] - offsetLengthW[num - 1]) / (-spanW[num] + spanW[num - 1]);
        xAtaque.append(offsetLengthW[num - 1] + psi * (-spanW[num] + spanW[num - 1]));
        xSalida.append(xAtaque[0] + cy[0]);
        index.append(0);
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
                index.append(i);
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
                index.append(i);
                psiTan.append(psiTanTmp);
                dihedCos.append(cos(dihedralAngleW[num - i - 1] / 180 * M_PI));

            }
            ii = 1;
        }

    }



    spanYt = yt;


    /*
    index.pop_back(); //删除多余元素
    psiTan.pop_back();//删除多余元素
    dihedCos.pop_back();
    for(int i = Ny / 2 - 1;i>=0;i--){
        double tmp1 = -yt[i];
        double tmp2 = cy[i];
        double tmp3 = xAtaque[i];
        double tmp4 = xSalida[i];
        double tmp5 = psiTan[i];
        double tmp6 = dihedCos[i];
        yt.append(tmp1);
        cy.append(tmp2);
        xAtaque.append(tmp3);
        xSalida.append(tmp4);
        psiTan.append(tmp5);
        dihedCos.append(tmp6);
    }


    for(int i = 0;i<num;i++){
        int len = gridU[num - i];
        for(int j = 0;j<len;j++){
            index.append(num - i - 1);
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
            cv.append(cy[i]);
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





    /*********************计算z坐标**********************/


    if(airfoilInputType)
        computeMeanCamberAirfoil();
    else
        computeMeanCamberCst();




    interpolationAirfoil();//计算插值后的翼型


    for(int i = 0;i<Nx + 1;i++){
        QVector<double>xCurTmp;

        for(int j = 0;j<Ny + 1;j++){
            double tmp = cosGridV[i];
            xCurTmp.append(tmp);
        }
        xCur.append(xCurTmp);
    }

    for(int i = 0;i<Nx + 1;i++){
        for(int j = 0;j<Ny + 1;j++){
            double tmp = computeZ(j,xCur[i][j],cy[j]);
            //xyvv[i][j].setZ(tmp);
            xyvv[i][j].z = tmp;
        }
    }


    QVector<double>ytTmp;
    for(int i = 0;i<Ny;i++){
        ytTmp.append((yt[i] + yt[i + 1]) / 2);
    }
    spanForceYt = ytTmp;
}
void propellerVLM::initial3DMesh(){
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
void propellerVLM::twistWing(){


    /****************计算扭转矢量方向****************/
    QVector<point3d>vp;
    QVector<point3d>vn;
    QVector<double>xcoord;
    for(int i = 0;i<Ny + 1;i++){
        xcoord.append(xAtaque[i] + cy[i] / 4);
    }
    for(int i = 0;i<num;i++){
        int len = gridU[num - i];

        // if(i == 0)
        //     len = len +1;
        int ind = 0;
        for(int k = 0;k<i;k++){
            ind = gridU[num - k] + ind;
        }

        for(int j = 0;j<len;j++){
            // int G = ind + j + 1;
            // if(i == 0)
            int G = ind + j;
            // point3d vnTmp1(xyvv[0][G].x,xyvv[0][G].y,xyvv[0][G].z);
            // point3d vnTmp2(xyvv[0][G].x,xyvv[0][G].y + 0.1,xyvv[0][G].z);

            point3d vnTmp1(xcoord[G],xyvv[0][G].y,xyvv[0][G].z);
            point3d vnTmp2(xcoord[G + 1],xyvv[0][G + 1].y,xyvv[0][G + 1].z);
            point3d vnTmp = calculateVector(vnTmp1,vnTmp2);
            vp.append(vnTmp1);
            vn.append(vnTmp);

        }
    }


    point3d vntmp1(xcoord[Ny],xyvv[0][Ny].y,xyvv[0][Ny].z);
    point3d vntmp2(xcoord[Ny],xyvv[0][Ny].y + 0.1,xyvv[0][Ny].z);
    // point3d vntmp1(xyvv[0][Ny / 2].x,xyvv[0][Ny / 2].y,xyvv[0][Ny / 2].z);
    // point3d vntmp2(xyvv[0][Ny / 2].x,xyvv[0][Ny / 2].y + 0.1,xyvv[0][Ny / 2].z);
    point3d vntmp = calculateVector(vntmp1,vntmp2);
    vp.append(vntmp1);
    vn.append(vntmp);




    /*******************计算扭转角*******************/
    QVector<double>th;

    for(int i = 0;i<num;i++){
        int len = gridU[num - i];
        // if(i == 0)
        //     len = len +1;

        int ind = 0;
        for(int k = 0;k<i;k++){
            ind = gridU[num - k] + ind;
        }
        for(int j = 0;j<len;j++){
            // int G = ind + j + 1;
            // if(i == 0)
                int G = ind + j;
            //qDebug()<<G;

            double k = (twistAngleW[num - i - 1] - twistAngleW[num - i]) / (spanW[num - i] - spanW[num - i - 1]);
            double t = (twistAngleW[num - i] +  k *  (spanYt[G] +  spanW[num - i]))/ 180 * M_PI;
            th.append(t);
        }
    }
    th.append(twistAngleW[0] / 180 * M_PI);

    //th.append(twistAngleW[0]);






    /********************扭转机翼**********************/
    for(int i =0;i<Nx + 1;i++){
        for(int j = 0;j<Ny + 1;j++){
            xyvv[i][j] = rotatePointAroundAxis(xyvv[i][j],vp[j],vn[j],th[j]);
            xyvvUpper[i][j] = rotatePointAroundAxis(xyvvUpper[i][j],vp[j],vn[j],th[j]);
            xyvvLower[i][j] = rotatePointAroundAxis(xyvvLower[i][j],vp[j],vn[j],th[j]);
        }
    }
}

void propellerVLM::rotationalCopyValue(){
    int len1 = XAtotalArray[0].length();
    int l = rpmArray.length() * vArray.length();
    for (int p = 0; p < l; p++) {
        for (int i = 1; i < propNum; i++) {
            double angle = M_PI / propNum * (i + 1); // 提前计算角度
            for (int j = 0; j < len1; j++) {
                // 创建原始点
                point3d pointsA(XAtotalArray[p][j], YAtotalArray[p][j], ZAtotalArray[p][j]);
                point3d pointsB(XBtotalArray[p][j], YBtotalArray[p][j], ZBtotalArray[p][j]);
                point3d pointsC(XCtotalArray[p][j], YCtotalArray[p][j], ZCtotalArray[p][j]);
                point3d pointsD(XDtotalArray[p][j], YDtotalArray[p][j], ZDtotalArray[p][j]);
                point3d pointsE(XEtotalArray[p][j], YEtotalArray[p][j], ZEtotalArray[p][j]);
                point3d pointsF(XFtotalArray[p][j], YFtotalArray[p][j], ZFtotalArray[p][j]);

                // 旋转并将结果存储在局部变量
                point3d rotatedA = rotateCod(pointsA, angle);
                point3d rotatedB = rotateCod(pointsB, angle);
                point3d rotatedC = rotateCod(pointsC, angle);
                point3d rotatedD = rotateCod(pointsD, angle);
                point3d rotatedE = rotateCod(pointsE, angle);
                point3d rotatedF = rotateCod(pointsF, angle);

                // 将旋转后的点一次性添加到数组
                XAtotalArray[p].append(rotatedA.x); YAtotalArray[p].append(rotatedA.y); ZAtotalArray[p].append(rotatedA.z);
                XBtotalArray[p].append(rotatedB.x); YBtotalArray[p].append(rotatedB.y); ZBtotalArray[p].append(rotatedB.z);
                XCtotalArray[p].append(rotatedC.x); YCtotalArray[p].append(rotatedC.y); ZCtotalArray[p].append(rotatedC.z);
                XDtotalArray[p].append(rotatedD.x); YDtotalArray[p].append(rotatedD.y); ZDtotalArray[p].append(rotatedD.z);
                XEtotalArray[p].append(rotatedE.x); YEtotalArray[p].append(rotatedE.y); ZEtotalArray[p].append(rotatedE.z);
                XFtotalArray[p].append(rotatedF.x); YFtotalArray[p].append(rotatedF.y); ZFtotalArray[p].append(rotatedF.z);
            }
        }
    }

    /*
    for(int i = 1;i<propNum;i++){
        double angle = M_PI / propNum * (i + 1);
        for(int j = 0;j<len1;j++){
            //
            point3d xA = rotateCod(point3d(XAtotal[j],YAtotal[j],ZAtotal[j]),angle);
            point3d xB = rotateCod(point3d(XBtotal[j],YBtotal[j],ZBtotal[j]),angle);
            point3d xC = rotateCod(point3d(XCtotal[j],YCtotal[j],ZCtotal[j]),angle);
            point3d xD = rotateCod(point3d(XDtotal[j],YDtotal[j],ZDtotal[j]),angle);
            point3d xE = rotateCod(point3d(XEtotal[j],YEtotal[j],ZEtotal[j]),angle);
            point3d xF = rotateCod(point3d(XFtotal[j],YFtotal[j],ZFtotal[j]),angle);
            XAtotal.append(xA.x);YAtotal.append(xA.y);ZAtotal.append(xA.z);
            XBtotal.append(xB.x);YBtotal.append(xB.y);ZBtotal.append(xB.z);
            XCtotal.append(xC.x);YCtotal.append(xC.y);ZCtotal.append(xC.z);
            XDtotal.append(xD.x);YDtotal.append(xD.y);ZDtotal.append(xD.z);
            XEtotal.append(xE.x);YEtotal.append(xE.y);ZEtotal.append(xE.z);
            XFtotal.append(xF.x);YFtotal.append(xF.y);ZFtotal.append(xF.z);
        }
    }
    */
}
void propellerVLM::computeVector(){
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

        double u3x = xp[i] - xFptotal[i];
        double u3y = yp[i] - yFptotal[i];
        double u3z = zp[i] - zFptotal[i];
        double u4x = XAtotal[i] - xFptotal[i];
        double u4y = YAtotal[i] - yFptotal[i];
        double u4z = ZAtotal[i] - zFptotal[i];
        double x2 = u3y * u4z - u3z * u4y;
        double y2 = u3z * u4x - u3x * u4z;
        double z2 = u3x * u4y - u3y * u4x;
        double m2 = sqrt(x2 * x2 + y2 * y2 + z2 * z2);


        nx.append(x / m);
        ny.append(y / m);
        nz.append(z / m);

        nx2.append(x2 / m2);
        ny2.append(y2 / m2);
        nz2.append(z2 / m2);

    }

    for(int i = 0;i<Ny;i++){
        nx.append(0);
        ny.append(0);
        nz.append(1);
    }
    for(int i = 0;i<Nx;i++){
        QVector<double>nzTmp;
        QVector<double>nyTmp;
        QVector<double>nxTmp;
        QVector<double>nz2Tmp;
        QVector<double>ny2Tmp;
        QVector<double>nx2Tmp;
        for(int j = 0;j<Ny;j++){
            int G = i * Ny + j;
            nzTmp.append(nz[G]);
            nyTmp.append(ny[G]);
            nxTmp.append(nx[G]);
            nz2Tmp.append(nz2[G]);
            ny2Tmp.append(ny2[G]);
            nx2Tmp.append(nx2[G]);
        }
        Zn1.append(nzTmp);
        Yn1.append(nyTmp);
        Xn1.append(nxTmp);
        Zn2.append(nz2Tmp);
        Yn2.append(ny2Tmp);
        Xn2.append(nx2Tmp);
    }


    Xn = nx;
    Yn = ny;
    Zn = nz;

    /*****************计算向量******************/


    for(int i = 0;i<N;i++){
        vPQ.append(point3d(xp6[i] - xp5[i],yp6[i] - yp5[i],zp6[i] - zp5[i]));
    }
}
void propellerVLM::solveInfluenceCoefficient(int n){
    int len1 = N + Ny * (wakeStep - 1) / 2;
    QVector<QVector<double>>vinxTmp;
    QVector<QVector<double>>vinyTmp;
    QVector<QVector<double>>vinzTmp;
    QVector<QVector<double>>vTotalTmp;
    for (int i = 0; i < Nx; i++) {
        for (int j = 0; j < Ny; j++) {
            int G = i * Ny + j;

            double xcoloc = xptotal[G];
            double ycoloc = yptotal[G];
            double zcoloc = zptotal[G];

            //double xfcoloc = xptotal[G];
            //double yfcoloc = yptotal[G];
            //double zfcoloc = zptotal[G];

            double xfcoloc = xFptotal[G];
            double yfcoloc = yFptotal[G];
            double zfcoloc = zFptotal[G];

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


            leyBiotSavart3D(xcoloc, ycoloc, zcoloc, XAtotalArray[n], YAtotalArray[n], ZAtotalArray[n], XBtotalArray[n], YBtotalArray[n], ZBtotalArray[n], vxAB, vyAB, vzAB);
            leyBiotSavart3D(xcoloc, ycoloc, zcoloc, XBtotalArray[n], YBtotalArray[n], ZBtotalArray[n], XFtotalArray[n], YFtotalArray[n], ZFtotalArray[n], vxBF, vyBF, vzBF);
            leyBiotSavart3D(xcoloc, ycoloc, zcoloc, XFtotalArray[n], YFtotalArray[n], ZFtotalArray[n], XCtotalArray[n], YCtotalArray[n], ZCtotalArray[n], vxFC, vyFC, vzFC);
            leyBiotSavart3D(xcoloc, ycoloc, zcoloc, XCtotalArray[n], YCtotalArray[n], ZCtotalArray[n], XDtotalArray[n], YDtotalArray[n], ZDtotalArray[n], vxCD, vyCD, vzCD);
            leyBiotSavart3D(xcoloc, ycoloc, zcoloc, XDtotalArray[n], YDtotalArray[n], ZDtotalArray[n], XEtotalArray[n], YEtotalArray[n], ZEtotalArray[n], vxDE, vyDE, vzDE);
            leyBiotSavart3D(xcoloc, ycoloc, zcoloc, XEtotalArray[n], YEtotalArray[n], ZEtotalArray[n], XAtotalArray[n], YAtotalArray[n], ZAtotalArray[n], vxEA, vyEA, vzEA);

            leyBiotSavart3D(xfcoloc, yfcoloc, zfcoloc, XAtotalArray[n], YAtotalArray[n], ZAtotalArray[n], XBtotalArray[n], YBtotalArray[n], ZBtotalArray[n], vxfAB, vyfAB, vzfAB);
            leyBiotSavart3D(xfcoloc, yfcoloc, zfcoloc, XBtotalArray[n], YBtotalArray[n], ZBtotalArray[n], XFtotalArray[n], YFtotalArray[n], ZFtotalArray[n], vxfBF, vyfBF, vzfBF);
            leyBiotSavart3D(xfcoloc, yfcoloc, zfcoloc, XFtotalArray[n], YFtotalArray[n], ZFtotalArray[n], XCtotalArray[n], YCtotalArray[n], ZCtotalArray[n], vxfFC, vyfFC, vzfFC);
            leyBiotSavart3D(xfcoloc, yfcoloc, zfcoloc, XCtotalArray[n], YCtotalArray[n], ZCtotalArray[n], XDtotalArray[n], YDtotalArray[n], ZDtotalArray[n], vxfCD, vyfCD, vzfCD);
            leyBiotSavart3D(xfcoloc, yfcoloc, zfcoloc, XDtotalArray[n], YDtotalArray[n], ZDtotalArray[n], XEtotalArray[n], YEtotalArray[n], ZEtotalArray[n], vxfDE, vyfDE, vzfDE);
            leyBiotSavart3D(xfcoloc, yfcoloc, zfcoloc, XEtotalArray[n], YEtotalArray[n], ZEtotalArray[n], XAtotalArray[n], YAtotalArray[n], ZAtotalArray[n], vxfEA, vyfEA, vzfEA);



            /*
            double r = (cy[j] + cy[j + 1]) / 2 * 0.1;
            leyBiotSavart3D(xcoloc, ycoloc, zcoloc, XAtotalArray[n], YAtotalArray[n], ZAtotalArray[n], XBtotalArray[n], YBtotalArray[n], ZBtotalArray[n], vxAB, vyAB, vzAB,r);
            leyBiotSavart3D(xcoloc, ycoloc, zcoloc, XBtotalArray[n], YBtotalArray[n], ZBtotalArray[n], XFtotalArray[n], YFtotalArray[n], ZFtotalArray[n], vxBF, vyBF, vzBF,r);
            leyBiotSavart3D(xcoloc, ycoloc, zcoloc, XFtotalArray[n], YFtotalArray[n], ZFtotalArray[n], XCtotalArray[n], YCtotalArray[n], ZCtotalArray[n], vxFC, vyFC, vzFC,r);
            leyBiotSavart3D(xcoloc, ycoloc, zcoloc, XCtotalArray[n], YCtotalArray[n], ZCtotalArray[n], XDtotalArray[n], YDtotalArray[n], ZDtotalArray[n], vxCD, vyCD, vzCD,r);
            leyBiotSavart3D(xcoloc, ycoloc, zcoloc, XDtotalArray[n], YDtotalArray[n], ZDtotalArray[n], XEtotalArray[n], YEtotalArray[n], ZEtotalArray[n], vxDE, vyDE, vzDE,r);
            leyBiotSavart3D(xcoloc, ycoloc, zcoloc, XEtotalArray[n], YEtotalArray[n], ZEtotalArray[n], XAtotalArray[n], YAtotalArray[n], ZAtotalArray[n], vxEA, vyEA, vzEA,r);

            leyBiotSavart3D(xfcoloc, yfcoloc, zfcoloc, XAtotalArray[n], YAtotalArray[n], ZAtotalArray[n], XBtotalArray[n], YBtotalArray[n], ZBtotalArray[n], vxfAB, vyfAB, vzfAB,r);
            leyBiotSavart3D(xfcoloc, yfcoloc, zfcoloc, XBtotalArray[n], YBtotalArray[n], ZBtotalArray[n], XFtotalArray[n], YFtotalArray[n], ZFtotalArray[n], vxfBF, vyfBF, vzfBF,r);
            leyBiotSavart3D(xfcoloc, yfcoloc, zfcoloc, XFtotalArray[n], YFtotalArray[n], ZFtotalArray[n], XCtotalArray[n], YCtotalArray[n], ZCtotalArray[n], vxfFC, vyfFC, vzfFC,r);
            leyBiotSavart3D(xfcoloc, yfcoloc, zfcoloc, XCtotalArray[n], YCtotalArray[n], ZCtotalArray[n], XDtotalArray[n], YDtotalArray[n], ZDtotalArray[n], vxfCD, vyfCD, vzfCD,r);
            leyBiotSavart3D(xfcoloc, yfcoloc, zfcoloc, XDtotalArray[n], YDtotalArray[n], ZDtotalArray[n], XEtotalArray[n], YEtotalArray[n], ZEtotalArray[n], vxfDE, vyfDE, vzfDE,r);
            leyBiotSavart3D(xfcoloc, yfcoloc, zfcoloc, XEtotalArray[n], YEtotalArray[n], ZEtotalArray[n], XAtotalArray[n], YAtotalArray[n], ZAtotalArray[n], vxfEA, vyfEA, vzfEA,r);
             */


            for(int k = 0;k<len1;k++){
                double tmp1 = 0;
                double tmp2 = 0;
                double tmp3 = 0;
                double tmp4 = 0;
                double tmp5 = 0;
                double tmp6 = 0;
                for(int l = 0;l<propNum;l++){
                    int ind = k + l * len1;
                    tmp1 += (vxAB[ind] + vxBF[ind] + vxFC[ind] + vxCD[ind] + vxDE[ind] + vxEA[ind]);
                    tmp2 += (vyAB[ind] + vyBF[ind] + vyFC[ind] + vyCD[ind] + vyDE[ind] + vyEA[ind]);
                    tmp3 += (vzAB[ind] + vzBF[ind] + vzFC[ind] + vzCD[ind] + vzDE[ind] + vzEA[ind]);
                    tmp4 += (vxfAB[ind] + vxfBF[ind] + vxfFC[ind] + vxfCD[ind] + vxfDE[ind] + vxfEA[ind]);
                    tmp5 += (vyfAB[ind] + vyfBF[ind] + vyfFC[ind] + vyfCD[ind] + vyfDE[ind] + vyfEA[ind]);
                    tmp6 += (vzfAB[ind] + vzfBF[ind] + vzfFC[ind] + vzfCD[ind] + vzfDE[ind] + vzfEA[ind]);
                }
                nxS.append(tmp1);
                nyS.append(tmp2);
                nzS.append(tmp3);
                nxfS.append(tmp4);
                nyfS.append(tmp5);
                nzfS.append(tmp6);
            }


            for (int k = 0; k < len1; k++) {
                point3d b1(nx[G], ny[G], nz[G]);
                //point3d b2(nx2[G], ny2[G], nz2[G]);

                point3d a1(nxS[k], nyS[k], nzS[k]);
                //point3d a3(nxfS[k], nyfS[k], nzfS[k]);

                vzTotal.append(computeNormalInfluences(a1, b1));
                //vnx.append(computeNormalInfluences(b2, a3));
            }



            vinxTmp.append(nxfS);
            vinyTmp.append(nyfS);
            vinzTmp.append(nzfS);
            //vinn.append(vnx);
            vTotalTmp.append(vzTotal);
        }
    }
    vinxArray.append(vinxTmp);
    vinyArray.append(vinyTmp);
    vinzArray.append(vinzTmp);
    vTotalArray.append(vTotalTmp);
}
void propellerVLM::solveInfluenceCoefficientArray(){
    if(!vinxArray.isEmpty()){
        vinxArray.clear();
        vinyArray.clear();
        vinzArray.clear();
        vTotalArray.clear();
    }

    int l = rpmArray.length() * vArray.length();
    for(int i = 0;i<l;i++){
        solveInfluenceCoefficient(i);
    }
}
/*
void propellerVLM::solveInfluenceCoefficientArray() {

    //emit emitProgressValue(0);

    //int len2 = (N + Ny * (wakeStep - 1) / 2) * propNum;
    int len1 = N + Ny * (wakeStep - 1) / 2;

    for(int p = 0;p<vinfArray.length();p++){
        QVector<QVector<double>>vinxTmp;
        QVector<QVector<double>>vinyTmp;
        QVector<QVector<double>>vinzTmp;
        QVector<QVector<double>>vTotalTmp;

    }

    for (int i = 0; i < Nx; i++) {
        for (int j = 0; j < Ny; j++) {
            int G = i * Ny + j;

            double xcoloc = xptotal[G];
            double ycoloc = yptotal[G];
            double zcoloc = zptotal[G];

            //double xfcoloc = xptotal[G];
            //double yfcoloc = yptotal[G];
            //double zfcoloc = zptotal[G];

            double xfcoloc = xFptotal[G];
            double yfcoloc = yFptotal[G];
            double zfcoloc = zFptotal[G];

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

            leyBiotSavart3D(xcoloc, ycoloc, zcoloc, XAtotal, YAtotal, ZAtotal, XBtotal, YBtotal, ZBtotal, vxAB, vyAB, vzAB);
            leyBiotSavart3D(xcoloc, ycoloc, zcoloc, XBtotal, YBtotal, ZBtotal, XFtotal, YFtotal, ZFtotal, vxBF, vyBF, vzBF);
            leyBiotSavart3D(xcoloc, ycoloc, zcoloc, XFtotal, YFtotal, ZFtotal, XCtotal, YCtotal, ZCtotal, vxFC, vyFC, vzFC);
            leyBiotSavart3D(xcoloc, ycoloc, zcoloc, XCtotal, YCtotal, ZCtotal, XDtotal, YDtotal, ZDtotal, vxCD, vyCD, vzCD);
            leyBiotSavart3D(xcoloc, ycoloc, zcoloc, XDtotal, YDtotal, ZDtotal, XEtotal, YEtotal, ZEtotal, vxDE, vyDE, vzDE);
            leyBiotSavart3D(xcoloc, ycoloc, zcoloc, XEtotal, YEtotal, ZEtotal, XAtotal, YAtotal, ZAtotal, vxEA, vyEA, vzEA);

            leyBiotSavart3D(xfcoloc, yfcoloc, zfcoloc, XAtotal, YAtotal, ZAtotal, XBtotal, YBtotal, ZBtotal, vxfAB, vyfAB, vzfAB);
            leyBiotSavart3D(xfcoloc, yfcoloc, zfcoloc, XBtotal, YBtotal, ZBtotal, XFtotal, YFtotal, ZFtotal, vxfBF, vyfBF, vzfBF);
            leyBiotSavart3D(xfcoloc, yfcoloc, zfcoloc, XFtotal, YFtotal, ZFtotal, XCtotal, YCtotal, ZCtotal, vxfFC, vyfFC, vzfFC);
            leyBiotSavart3D(xfcoloc, yfcoloc, zfcoloc, XCtotal, YCtotal, ZCtotal, XDtotal, YDtotal, ZDtotal, vxfCD, vyfCD, vzfCD);
            leyBiotSavart3D(xfcoloc, yfcoloc, zfcoloc, XDtotal, YDtotal, ZDtotal, XEtotal, YEtotal, ZEtotal, vxfDE, vyfDE, vzfDE);
            leyBiotSavart3D(xfcoloc, yfcoloc, zfcoloc, XEtotal, YEtotal, ZEtotal, XAtotal, YAtotal, ZAtotal, vxfEA, vyfEA, vzfEA);



            for(int k = 0;k<len1;k++){
                double tmp1 = 0;
                double tmp2 = 0;
                double tmp3 = 0;
                double tmp4 = 0;
                double tmp5 = 0;
                double tmp6 = 0;
                for(int l = 0;l<propNum;l++){
                    int ind = k + l * len1;
                    tmp1 += (vxAB[ind] + vxBF[ind] + vxFC[ind] + vxCD[ind] + vxDE[ind] + vxEA[ind]);
                    tmp2 += (vyAB[ind] + vyBF[ind] + vyFC[ind] + vyCD[ind] + vyDE[ind] + vyEA[ind]);
                    tmp3 += (vzAB[ind] + vzBF[ind] + vzFC[ind] + vzCD[ind] + vzDE[ind] + vzEA[ind]);
                    tmp4 += (vxfAB[ind] + vxfBF[ind] + vxfFC[ind] + vxfCD[ind] + vxfDE[ind] + vxfEA[ind]);
                    tmp5 += (vyfAB[ind] + vyfBF[ind] + vyfFC[ind] + vyfCD[ind] + vyfDE[ind] + vyfEA[ind]);
                    tmp6 += (vzfAB[ind] + vzfBF[ind] + vzfFC[ind] + vzfCD[ind] + vzfDE[ind] + vzfEA[ind]);
                }
                nxS.append(tmp1);
                nyS.append(tmp2);
                nzS.append(tmp3);
                nxfS.append(tmp4);
                nyfS.append(tmp5);
                nzfS.append(tmp6);
            }


            for (int k = 0; k < len1; k++) {
                point3d b1(nx[G], ny[G], nz[G]);
                //point3d b2(nx2[G], ny2[G], nz2[G]);

                point3d a1(nxS[k], nyS[k], nzS[k]);
                //point3d a3(nxfS[k], nyfS[k], nzfS[k]);

                vzTotal.append(computeNormalInfluences(a1, b1));
                //vnx.append(computeNormalInfluences(b2, a3));
            }



            vinx.append(nxfS);
            viny.append(nyfS);
            vinz.append(nzfS);
            //vinn.append(vnx);
            vTotal.append(vzTotal);
        }
    }


    //qDebug()<<vTotal;



}
*/
void propellerVLM::initialVinfArray(){

    if(!vinfArray.isEmpty())
        vinfArray.clear();
    for(int p = 0;p<vArray.length();p++){
        for(int q = 0;q<rpmArray.length();q++){
            QVector<point3d> tmp;  // 使用 QVector<point3d> 来存储 3D 向量对象
            for(int i = 0; i < Nx; i++){

                for(int j = 0; j < Ny; j++){
                    //int G = i * Ny + j;
                    //double r = sqrt(xptotal[G] * xptotal[G] + yptotal[G] * yptotal[G]);
                    //double r = sqrt(xptotal[j] * xptotal[j] + yptotal[j] * yptotal[j]);
                    double r = abs(yptotal[j]);
                    point3d rv(r * omegaArray[q], 0, vArray[p]);  // 创建 3D 向量

                    tmp.append(rv);  // 将 3D 向量添加到 tmp 中

                }

            }
            vinfArray.append(tmp);  // 将 tmp 添加到 vinfArray 中
        }
    }



}
void propellerVLM::computeVLMMatrixArray(){
    if(!matrixGammaArray.isEmpty())
        matrixGammaArray.clear();
    int l = rpmArray.length() * vArray.length();
    for(int i = 0;i<l;i++){
        computeVLMMatrix(i);
        //log
        int value = (double(i) + 1) / l / 2 * 20;
        QString txt = "线性方程组(" + QString::number(i + 1) + ")求解完成";
        emit emitAnalyseLog(txt);
        emit emitProgressValue(value);
    }


}
void propellerVLM::computeVLMMatrix(int n){
    QVector<QVector<double>>matrixA;




    Eigen::MatrixXd MatrixA;

    Eigen::VectorXd MatrixGamma;

    Eigen::VectorXd Matrixb;
    Matrixb.resize(N);
    QVector<double>mgamma;
    QVector<double>matrixb;


    MatrixA.resize(N,N);



    for(int i = 0;i<N ;i++){
        QVector<double>matrixTmp;
        for(int j = 0;j<N;j++){
            matrixTmp.append(vTotalArray[n][i][j]);
            MatrixA.coeffRef(i,j) = vTotalArray[n][i][j];
        }
        matrixA.append(matrixTmp);
    }
    //尾迹贡献和
    QVector<QVector<double>>sumTmp;




    for(int i = 0;i<N;i++){
        QVector<double>tmpM;

        for(int j = 0;j<Ny;j++){
            double tmp = 0.0;
            for(int k = 0;k<(wakeStep - 1) / 2;k++){
                tmp += vTotalArray[n][i][N + k * Ny + j];
            }
            tmpM.append(tmp);
        }

        sumTmp.append(tmpM);
    }

    for(int i = 0;i<N ;i++){
        for(int j = 0;j<Ny;j++){
            matrixA[i][N - Ny + j] = vTotalArray[n][i][N - Ny + j] + sumTmp[i][j];
            MatrixA.coeffRef(i,N - Ny + j) = matrixA[i][N - Ny + j];
        }
    }




    for(int j = 0;j<N;j++){
        point3d v = vinfArray[n][j];
        point3d n(Xn[j],Yn[j],Zn[j]);
        matrixb.append(-dotProduct(v,n));
        Matrixb.coeffRef(j) = matrixb[j];
    }
    MatrixGamma = MatrixA.colPivHouseholderQr().solve(Matrixb);



    for(int k = 0;k<N;k++)
        mgamma.append(MatrixGamma(k));


    for(int i = 0;i<(wakeStep - 1) / 2;i++){
        for(int j = 0;j<Ny;j++){
            mgamma.append(MatrixGamma(N - Ny + j));
        }
    }
    matrixGammaArray.append(mgamma);



}
/*
void propellerVLM::computeVLMMatrix(){

    QVector<QVector<double>>matrixA;


    QVector<QVector<double>>mgammaArray;

    Eigen::MatrixXd MatrixA;

    Eigen::VectorXd MatrixGamma;


    MatrixA.resize(N,N);



    for(int i = 0;i<N ;i++){
        QVector<double>matrixTmp;
        for(int j = 0;j<N;j++){
            matrixTmp.append(vTotal[i][j]);
            MatrixA.coeffRef(i,j) = vTotal[i][j];
        }
        matrixA.append(matrixTmp);
    }
    //尾迹贡献和

    QVector<QVector<double>>sumTmp;




    for(int i = 0;i<N;i++){
        QVector<double>tmpM;

        for(int j = 0;j<Ny;j++){
            double tmp = 0.0;
            for(int k = 0;k<(wakeStep - 1) / 2;k++){
                tmp += vTotal[i][N + k * Ny + j];
            }
            tmpM.append(tmp);
        }

        sumTmp.append(tmpM);
    }

    for(int i = 0;i<N ;i++){
        for(int j = 0;j<Ny;j++){
            matrixA[i][N - Ny + j] = vTotal[i][N - Ny + j] + sumTmp[i][j];
            MatrixA.coeffRef(i,N - Ny + j) = matrixA[i][N - Ny + j];
        }
    }



    int progressValue;

    int l = rpmArray.length() * vArray.length();
    for(int i = 0;i < l;i++){
        Eigen::VectorXd Matrixb;
        Matrixb.resize(N);
        QVector<double>mgamma;
        QVector<double>matrixb;
        for(int j = 0;j<N;j++){
            point3d v = vinfArray[i][j];
            point3d n(Xn[j],Yn[j],Zn[j]);
            matrixb.append(-dotProduct(v,n));
            Matrixb.coeffRef(j) = matrixb[j];
            //qDebug()<<vinfArray[i][j].x<<" "<<vinfArray[i][j].z;
        }
        MatrixGamma = MatrixA.colPivHouseholderQr().solve(Matrixb);



        for(int k = 0;k<N;k++)
            mgamma.append(MatrixGamma(k));


        for(int i = 0;i<(wakeStep - 1) / 2;i++){
            for(int j = 0;j<Ny;j++){
                mgamma.append(MatrixGamma(N - Ny + j));
            }
        }

        //for(int k = 0;k<Ny * wakeStep - Ny;k++)
            //mgamma.append(MatrixGamma(N - Ny + k));



        mgammaArray.append(mgamma);

        if(isAddXfoilDrag){
            progressValue = (double(i) + 1) / (l) * 50;
            //emit emitProgressValue(progressValue);
        }else{
            progressValue = (double(i) + 1) / (l) * 100;
            //emit emitProgressValue(progressValue);
        }


    }



    matrixGammaArray = mgammaArray;





}
*/
void propellerVLM::computeStreamLine(const int index, const double distance,const double height,const double t,const double dt){
    streamIndex = index;
    //int rpmIndex = index%(rpmArray.length());
    int rpmIndex = index % rpmArray.length();


    //qDebug()<<index%(rpmArray.length());
    QVector<point3d>position;
    for(int i = N - Ny;i<N;i++){
        position.append(xyTD[i]);

    }
    position.append(xyTC[N - 1]);

    QVector<QVector<point3d>>tmp;


    for(int i = 0;i<Ny + 1;i++){

        int value = double(i)/ Ny * 100;
        emit progressUpdated(value);

        double deltaTime = dt;
        double totalTime = t;
        double currentTime = 0.0;
        QVector<point3d>streamTmp;
        point3d currentPosition(position[i].x - distance,position[i].y,position[i].z + height);

        streamTmp.append(currentPosition);
        //positionYVelocity = point3d(VX[N - Ny + i],VY[N - Ny + i],VZ[N - Ny + i]);

        while(currentTime < totalTime){
            thate = currentTime * omegaArray[rpmIndex];
            point3d nextPosition = rungeKutta4(currentPosition,deltaTime,i);
            //qDebug()<<"x:"<<nextPosition.x<<"y:"<<nextPosition.y<<"z:"<<nextPosition.z;
            streamTmp.append(nextPosition);
            currentPosition = nextPosition;

            currentTime += deltaTime;

        }
        thate = 0;
        tmp.append(streamTmp);
    }
    emit workFinished();
    streamLine = tmp;

}
void propellerVLM::computeGamma(){
    QVector<QVector<double>>realGammaArrayTmp;

    for(int alphaIndex = 0;alphaIndex<matrixGammaArray.length();alphaIndex++){


        QVector<double>matrixGammaTmp = matrixGammaArray[alphaIndex];
        QVector<QVector<double>>Gamma;
        //QVector<QVector<double>>GammaTotal;
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
            //QVector<double>gammaTotalTmp;
            QVector<double>gammaTotalTmp2;
            //gammaTotalTmp.append(Gamma[0][i] / hs[0][i]);
            gammaTotalTmp2.append(Gamma[0][i]);
            for(int j = 1;j<Nx;j++){
                //gammaTotalTmp.append((Gamma[j][i] - Gamma[j - 1][i]) / hs[j][i]);
                gammaTotalTmp2.append(Gamma[j][i] - Gamma[j - 1][i]);


            }

            //GammaTotal.append(gammaTotalTmp);
            GammaTotal2.append(gammaTotalTmp2);
            //matrixGammaArray[alphaIndex][N + i] = gammaTotalTmp2[Nx - 1];

    }



    //矩阵转置


        for(int i = 0;i<Nx;i++){
            //QVector<double>gammaTmp;
            for(int j = 0;j<Ny;j++){
                //gammaTmp.append(GammaTotal[j][i]);
                realGam.append(GammaTotal2[j][i]);
            }
            //gammaTotal.append(gammaTmp);
        }

        realGammaArrayTmp.append(realGam);
    }

    realGammaArray = realGammaArrayTmp;

}
void propellerVLM::computeVelocity(){


    QVector<QVector<point3d>>tmp2;
    for(int index = 0;index<matrixGammaArray.length();index++){
        QVector<point3d>tmp1;

        for(int i = 0;i<N;i++){
            double vxt = 0;
            double vyt = 0;
            double vzt = 0;

            for(int j = 0;j<N + Ny * (wakeStep - 1) / 2;j++){
                vxt = vxt + vinxArray[index][i][j] * matrixGammaArray[index][j];
                vyt = vyt + vinyArray[index][i][j] * matrixGammaArray[index][j];
                vzt = vzt + vinzArray[index][i][j] * matrixGammaArray[index][j];
            }


            //VX.append(vxt + vinfArray[index][i].x);
            //VY.append(vyt + vinfArray[index][i].y);
            //VZ.append(vzt + vinfArray[index][i].z);
            //VM.append(sqrt(VX[i] * VX[i] + VY[i] * VY[i] + VZ[i] * VZ[i]));
            tmp1.append(point3d(vxt + vinfArray[index][i].x,vyt + vinfArray[index][i].y,vzt + vinfArray[index][i].z));
        }

        tmp2.append(tmp1);
    }
    streamVinfArray = tmp2;
}


void propellerVLM::computeForce(){
    if(!MVinfArray.isEmpty())
        MVinfArray.clear();
    QVector<QVector<point3d>>tmpArray;
    QVector<point3d>forceTmpArray;
    QVector<QVector<double>>spanLift;
    QVector<QVector<double>>spanLiftCoeff;
    //QVector<QVector<double>>momentTmp;
    QVector<QVector<double>>incTmp;//诱导阻力
    QVector<QVector<double>>force;
    //QVector<double>tArray;

    for(int p = 0;p<vArray.length();p++){
        for(int q = 0;q<rpmArray.length();q++){
            int alphaIndex = p * rpmArray.length() + q;
            double xa = 0;
            double ya = 0;
            double za = 0;
            QVector<point3d>tmp;
            QVector<double>liftTmp;
            QVector<double>dragTmp;
            QVector<double>rVTmp;


            for(int i = 0;i<Nx;i++){
                for(int j = 0;j<Ny;j++){
                    int G = i * Ny + j;
                    double a = realGammaArray[alphaIndex][G] * vPQ[G].x;
                    double b = realGammaArray[alphaIndex][G] * vPQ[G].y;
                    double c = realGammaArray[alphaIndex][G] * vPQ[G].z;
                    //if(j > Ny / 2 - 1)
                        //c = -c;
                    double VX = streamVinfArray[alphaIndex][G].x;
                    double VY = streamVinfArray[alphaIndex][G].y;
                    double VZ = streamVinfArray[alphaIndex][G].z;


                    double a2 = (VY * c - VZ * b) * density;
                    double b2 = -(VZ * a - VX * c) * density;
                    double c2 = (VX * b - VY * a) * density;

                    //if(j < Ny / 2){
                        //b2 = -b2;
                    //}

                    xa = xa + a2;
                    ya = ya + b2;
                    za = za + c2;

                    point3d f(a,b,c);
                    //totalForce.append(f);
                    liftTmp.append(c2);
                    dragTmp.append(a2);
                    tmp.append(point3d(a2,b2,c2));
                }
            }

            tmpArray.append(tmp);
            //double cosAlpha = cos(alphaArray[alphaIndex] / 180 * M_PI);
            //double sinAlpha = sin(alphaArray[alphaIndex] / 180 * M_PI);
            //double Drag = cosAlpha * xa + sinAlpha * za;
            //double Lift = - sinAlpha * xa + cosAlpha * za;

            double cosAlpha = 1;
            double sinAlpha = 0;
            double Drag = xa;
            double Lift = za;


            QVector<double>yLiftTmp;
            QVector<double>yLiftCoeff;
            QVector<double>yDragForce;

            //计算每段的升力
            for(int i = 0;i<Ny;i++){

                double tmp1 = 0;
                double tmp2 = 0;


                for(int j = 0;j<Nx;j++){
                    tmp1 = tmp1 + liftTmp[j * Ny + i];
                    tmp2 = tmp2 + dragTmp[j * Ny + i];
                }

                double tmpA = - sinAlpha * tmp2 + cosAlpha * tmp1 ;
                double tmpB = sinAlpha * tmp1 + cosAlpha * tmp2;
                double realVinfX = vinfArray[alphaIndex][i].x * vinfArray[alphaIndex][i].x;
                double realVinf = sqrt(realVinfX + vArray[p] * vArray[p]);


                double sumTmp = tmpA / spanArea[i] / density / realVinf / realVinf * 2;
                if(sumTmp > 2)
                    sumTmp = 1.5;
                rVTmp.append(realVinf);
                yLiftTmp.append(sumTmp * (cy[i] + cy[i + 1]) / 2);
                yLiftCoeff.append(sumTmp / dihedCos[i]);
                yDragForce.append(tmpB);
                //qDebug()<<vinfArray[alphaIndex][i].x<<" "<<realVinf<<"每段升力"<<sumTmp;


                //yLiftTmp.append((- sinAlpha * tmp2 + cosAlpha * tmp1) / spanArea[i] / density / vinf / vinf * 2 );
            }
            spanLift.append(yLiftTmp);

            forceTmpArray.append(point3d(Drag,ya,Lift));
            spanLiftCoeff.append(yLiftCoeff);

            force.append(yDragForce);
            MVinfArray.append(rVTmp);

            //tArray.append(calculateTorque(yDragForce,spanForceYt));
            thrustArray.append(Lift * propNum);
            //qDebug()<<Lift * propNum;
            incTmp.append(yDragForce);
            //qDebug()<<"升力系数"<<yLiftCoeff;
            //momentTmp.append(calculateBendingMoment(yDragForce,spanForceYt));
        }
    }

    spanLiftCoefficient = spanLiftCoeff;
    //torqueArray = tArray;
    includeForceArray = incTmp;

    //qDebug()<<spanLiftCoefficient;



    //qDebug()<<vArray;
    //qDebug()<<rpmArray;
    //qDebug()<<tArray;




    /*
    spanLiftForce = force;
    allForceArray = tmpArray;
    totalForceArray = forceTmpArray;
    spanForce = spanLift;
    spanLiftCoefficient = spanLiftCoeff;
    spanMonmentArray = momentTmp;
    //qDebug()<<spanLiftCoefficient;
    */

}
void propellerVLM::addDrag(){

    QVector<double>dragForce;

    if(isAddXfoilDrag){
        for(int index = 0;index < vinfArray.length();index++){
            for(int i = 0;i<Ny;i++){
                xfoilSetting tmp1;
                QVector<double>cstTmp;

                double temperature = mathSolver.calculateTemperature(height);
                double Viscosity = mathSolver.calculateAirViscosity(temperature);
                //tmp1.Ma = 0;
                double chord = (cy[i] + cy[i + 1]) / 2;
                double velocity = MVinfArray[index][i];
                tmp1.Re = int(chord * velocity / Viscosity);
                //tmp1.Ma = MVinfArray[index][i] / mathSolver.getSoundSpeed(temperature);
                tmp1.Ma = 0;
                tmp1.nCrit = 9;
                tmp1.alphaStepSize = 1;
                tmp1.minAlpha = -10;
                tmp1.maxAlpha = 20;
                tmp1.xtrTop = 1;
                tmp1.xtrBot = 1;
                tmp1.designALPHA = 1;
                tmp1.designCL = spanLiftCoefficient[index][i];
                tmp1.model = 1;
                tmp1.s_IterLim = 150;
                spanInputArray.append(tmp1);
                for(int j = 0;j<spanCstArray[i].length();j++){
                    cstTmp.append((spanCstArray[i][j] + spanCstArray[i + 1][j]) / 2);
                }
                spanCstAllArray.append(cstTmp);
                reArray.append(tmp1.Re);
                //qDebug()<<"设计升力系数"<<tmp1.designCL;


            }
        }

        int threadSum = Ny * vinfArray.length();
        int step = threadSum / threadNum;

        int remainderTmp = threadSum % threadNum;

        int progressValue;
        for(int i = 0;i<step;i++){
            int index = i * threadNum;
            startXfoilInThreadA(index,threadNum);
            progressValue = double(i + 1) / step * 40 + 20;
            QString txt = "第(" + QString::number(i + 1) + ")批翼型迭代完成";
            emit emitProgressValue(progressValue);
            emit emitAnalyseLog(txt);
        }

        if(remainderTmp > 0){
            int beginIndex = threadSum - remainderTmp;
            startXfoilInThreadA(beginIndex,remainderTmp);
        }

        smoothDrag();
        //qDebug()<<spanDragArray;
        smoothDragAgain();
        //qDebug()<<spanDragArray;




        QVector<QVector<double>>dragTmp;
        for(int i = 0;i<vinfArray.length();i++){
            QVector<double>tmp;
            for(int j = 0;j<Ny;j++){
                int index = i * Ny + j;
                double velocity = MVinfArray[i][j];
                double p = 0.5 * density * velocity * velocity * spanArea[j];
                double xDrag = p * spanDragArray[index];
                double sumDrag = xDrag + includeForceArray[i][j];
                tmp.append(sumDrag);
            }
            dragTmp.append(tmp);

        }


        for(int i = 0;i<vinfArray.length();i++){
            torqueArray.append(calculateTorque(dragTmp[i],spanForceYt) * propNum);
        }

    }else{
        for(int i = 0;i<vinfArray.length();i++)
            dragForce.append(0);
    }







}
void propellerVLM::startXfoilInThreadA(const int index,const int num){
    QVector<QFuture<void>>futures;

    QFutureSynchronizer<void> sync;
    QVector<airfoilSolve*>testSolve;
    QVector<double>airfoilX;
    for(int i = 0;i<101;i++)
        airfoilX.append(0.5 - cos(static_cast<double>(i) * M_PI / 100 ) * 0.5);

    for(int i = 0;i<num;i++){
        airfoilSolve *solvers = new airfoilSolve();
        testSolve.append(solvers);
        airfoilDesign design(cstNum);
        design.buildAirfoilCurve(spanCstAllArray[index + i],airfoilX);
        solvers->importAirfoil(design.newAirfoilData);
        solvers->refreshParaments(spanInputArray[index + i]);
        QFuture<void>future = QtConcurrent::run([solvers](){
            solvers->emitResult();
        });
        futures.append(future);
    }

    for (auto &f : futures) {
        sync.addFuture(f);
    }

    // 🚩 标准写法：等待所有线程完成
    sync.waitForFinished();


    for(int i = 0;i<num;i++){
        spanDragArray.append(testSolve[i]->onceData.cD);
        spanLiftArray.append(testSolve[i]->onceData.cL);
        //qDebug()<<testSolve[i]->onceData.cD<<" "<<testSolve[i]->onceData.cL;
    }
    qDeleteAll(testSolve);
    testSolve.clear();
}
void propellerVLM::startXfoilInThreadB(const int index,const int num){
    QVector<QFuture<void>>futures;

    QFutureSynchronizer<void> sync;
    QVector<airfoilSolve*>testSolve;
    QVector<double>airfoilX;
    for(int i = 0;i<101;i++)
        airfoilX.append(0.5 - cos(static_cast<double>(i) * M_PI / 100 ) * 0.5);

    for(int i = 0;i<num;i++){
        airfoilSolve *solvers = new airfoilSolve();
        testSolve.append(solvers);
        airfoilDesign design(cstNum);
        design.buildAirfoilCurve(badCSTArray[index + i],airfoilX);
        solvers->importAirfoil(design.newAirfoilData);
        solvers->refreshParaments(badSettingArray[index + i]);
        QFuture<void>future = QtConcurrent::run([solvers](){
            solvers->solver();
        });
        futures.append(future);
    }



    for (auto &f : futures) {
        sync.addFuture(f);
    }
    sync.waitForFinished();





    for(int i = 0;i<num;i++){
        QVector<double>tmp1;
        QVector<double>tmp2;
        QVector<double>tmp3;
        for(int j = 0;j<testSolve[i]->resultData.length();j++){
            tmp1.append(testSolve[i]->resultData[j][1]);
            tmp2.append(testSolve[i]->resultData[j][2]);
            tmp3.append(testSolve[i]->resultData[j][0]);
        }
        badCdArray.append(tmp2);
        badClArray.append(tmp1);
        badAlphaArray.append(tmp3);
    }

    qDeleteAll(testSolve);
    testSolve.clear();

}
void propellerVLM::smoothDrag() {
    double error = 0.01;
    int len = Ny;
    QVector<int> badValueIndex;


    for (int index = 0; index < vinfArray.length(); index++) {
        QVector<double> tmp1;  // 有效的 Yt 值
        QVector<double> tmp2;  // 有效的拖曳系数

        for (int i = 0; i < len; i++) {
            int G = index * len + i;

            // 比较 spanLiftCoefficient 和 spanLiftArray 的差异
            if (abs(spanLiftCoefficient[index][i] - spanLiftArray[G]) > error) {
                badValueIndex.append(G);  // 记录不合格的索引
                badDragIndex.append(G);
            } else {
                tmp1.append(spanYt[i]);    // 记录有效的 Yt 值
                tmp2.append(spanDragArray[G]); // 记录有效的拖曳系数
            }
        }

        // 对于每个不合格的拖曳系数，进行线性插值
        for (int i = 0; i < badValueIndex.length(); i++) {
            int G = badValueIndex[i];
            int k = G % len;  // 当前索引对应的 Yt 索引
            if (!tmp1.isEmpty() && !tmp2.isEmpty()) {  // 确保有有效数据
                spanDragArray[G] = myMath::linearInterpolation(tmp1, tmp2, spanYt[k]);
            }
        }
        //log
        int value = double(badValueIndex.length()) / len * 100;
        QString txt;
        if(value <= 5){
            txt = "<font color='#388E3C'>状态(" + QString::number(index + 1) + ")组阻力迭代收敛，阻力发散点占总比例" + QString::number(value) + "%</font>";
        }else if(value > 5&&value <= 15){
            txt = "<font color='#FBC02D'>状态(" + QString::number(index + 1) + ")组阻力迭代收敛，阻力发散点占总比例" + QString::number(value) + "</font>%";
        }else{
            txt = "<font color='#C62828'>状态(" + QString::number(index + 1) + ")组阻力迭代发散，阻力发散点占总比例" + QString::number(value) + "</font>%";
        }


        emit emitAnalyseLog(txt);


        // 清空 badValueIndex 以便下一个 index 使用
        badValueIndex.clear();
    }
}
void propellerVLM::smoothDragAgain(){


    for(int i = 0;i<badDragIndex.length();i++){

        xfoilSetting tmp1 = spanInputArray[badDragIndex[i]];
        tmp1.model = 0;
        badSettingArray.append(tmp1);
        badCSTArray.append(spanCstAllArray[badDragIndex[i]]);




    }


    int threadSum = badDragIndex.length();
    int step = threadSum / minThreadNum;
    int remainderTmp = threadSum % minThreadNum;

    QString txt1 = "发现" + QString::number(badDragIndex.length()) + "个迭代发散插值点";


    emit emitAnalyseLog(txt1);
    emit emitAnalyseLog("再次迭代................");

    for(int i = 0;i<step;i++){
        int index = i * minThreadNum;
        startXfoilInThreadB(index,minThreadNum);


        QString txt2 = "迭代第(" + QString::number(i + 1) + ")批翼型";
        emit emitAnalyseLog(txt2);


        int  progressValue = double(i + 1) / step * 35 + 60;
        emit emitProgressValue(progressValue);
    }
    if(remainderTmp > 0){
        int beginIndex = threadSum - remainderTmp;
        startXfoilInThreadB(beginIndex,remainderTmp);

    }

    QString txt2 = "线性拟合................";
    emit emitAnalyseLog(txt2);



    const QVector<double>hisDrag = spanDragArray;
    double dragTmp;
    for(int i = 0;i<badDragIndex.length();i++){
        int ind = badDragIndex[i];
        if(badClArray[i].length() > 6){
            double alpha = mathSolver.interpolateWithLinear(badAlphaArray[i],badClArray[i],badSettingArray[i].designCL);           
            dragTmp = mathSolver.interpolateWithLinear(badCdArray[i],badAlphaArray[i],alpha);
            if(dragTmp < hisDrag[ind])
                spanDragArray[ind] = dragTmp;
            QString txt3 = "升力系数:" + QString::number(badSettingArray[i].designCL) + "   原始阻力系数:" + QString::number(hisDrag[ind]) + "   迭代后阻力系数:" + QString::number(spanDragArray[ind]);
            emit emitAnalyseLog(txt3);
        }
    }
    /*
    for(int i = 0;i<spanDragArray.length();i++){
        if(spanDragArray[i] > 0.1  ||  spanDragArray[i] < 0){
            double cdL = 1.328 / sqrt(reArray[i]);
            double cdT = 0.455 / (std::pow(std::log10(reArray[i]), 2.58) * 1.0914);
            spanDragArray[i] =  cdL * 0.5 + cdT * 0.5;
        }

    }
    */



}

/*
void propellerVLM::smoothDrag(){
    double error = 0.01;
    int len = Ny + 1;
    QVector<int>badValueIndex;
    QVector<QVector<double>>spanIndexTmp;
    QVector<QVector<double>>spanDragTmp;


    for(int index = 0;index<vinfArray.length();index++){
        QVector<double>tmp1;
        QVector<double>tmp2;
        for(int i = 0;i<len;i++){
            int G = index * len + i;
            //qDebug()<<spanLiftCoefficient[index][i] - spanLiftArray[G];
            if(abs(spanLiftCoefficient[index][i] - spanLiftArray[G]) > error ){
                badValueIndex.append(G);
                qDebug()<<spanLiftCoefficient[index][i]<<"   "<<spanLiftArray[G];
            }else{
                tmp1.append(spanYt[i]);
                tmp2.append(spanDragArray[G]);

            }
        }

        spanIndexTmp.append(tmp1);
        spanDragTmp.append(tmp2);
    }

    myMath mySolve;
    for(int i = 0;i<badValueIndex.length();i++){
        int j = badValueIndex[i] / len;
        int k = badValueIndex[i] % len;
        int G = badValueIndex[i];

        spanDragArray[G] = mySolve.linearInterpolation(spanIndexTmp[j],spanDragTmp[j],spanYt[k]);


    }


}
*/
void propellerVLM::generateVLMMesh(){
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
        xptotal.append(xp[i] + 0.75 * (xp2[i] - xp[i]));
        //xFptotal.append(xp[i] + 0.25 * (xp2[i] - xp[i]));

        yp.append((xyB[i].y + xyA[i].y) / 2);
        yp2.append((xyC[i].y + xyD[i].y) / 2);
        yp3.append(xyA[i].y + 0.75 * (xyD[i].y - xyA[i].y));
        yp4.append(xyB[i].y + 0.75 * (xyC[i].y - xyB[i].y));
        yp5.append(xyA[i].y + 0.25 * (xyD[i].y - xyA[i].y));
        yp6.append(xyB[i].y + 0.25 * (xyC[i].y - xyB[i].y));
        yptotal.append(yp[i] + 0.75 * (yp2[i] - yp[i]));
        //yFptotal.append(yp[i] + 0.25 * (yp2[i] - yp[i]));

        zp.append((xyB[i].z + xyA[i].z) / 2);
        zp2.append((xyC[i].z + xyD[i].z) / 2);
        zp3.append(xyA[i].z + 0.75 * (xyD[i].z - xyA[i].z));
        zp4.append(xyB[i].z + 0.75 * (xyC[i].z - xyB[i].z));
        zp5.append(xyA[i].z + 0.25 * (xyD[i].z - xyA[i].z));
        zp6.append(xyB[i].z + 0.25 * (xyC[i].z - xyB[i].z));
        zptotal.append(zp[i] + 0.75 * (zp2[i] - zp[i]));
        //zFptotal.append(zp[i] + 0.25 * (zp2[i] - zp[i]));

        cmedia.append(sqrt(std::pow(xp[i] - xp2[i],2) + std::pow(yp[i] - yp2[i],2)));
    }



    for(int i = 0;i<Nx;i++){
        for(int j = 0;j<Ny;j++){
            int G = i * Ny + j;
            cyp.append((chordLengthW[num - index[j]] - chordLengthW[num - index[j] - 1]) / (spanW[num - index[j]] - spanW[num - index[j] - 1]) * (abs(yptotal[G])- abs(spanW[num - index[j] - 1])) + chordLengthW[num - index[j] - 1]);
        }
    }

    /******************************************/
    for(int i = 0;i<Nx;i++){
        QVector<double>xpTmp;
        QVector<double>ypTmp;
        QVector<double>cypTmp;
        QVector<double>xCurvaturaTmp;
        for(int j = 0;j<Ny;j++){
            int G = i * Ny + j;
            xpTmp.append(xptotal[G]);
            ypTmp.append(yptotal[G]);
            cypTmp.append(cyp[G]);
            xCurvaturaTmp.append((xptotal[G] - abs(yptotal[G]) * psiTan[j]) / cyp[G]);   //not need

        }
        Xptotal.append(xpTmp);
        Yptotal.append(ypTmp);
        Cyp.append(cypTmp);
        xCurvatura.append(xCurvaturaTmp);                                             //not need
    }



    /*********************创建涡格**********************/
    for(int i = 0;i<Nx;i++){
        for(int j = 0;j<Ny;j++){
            int G = i * Ny + j;
            XAtotal.append(xyA[G].x + (xyD[G].x - xyA[G].x) / 4);
            XBtotal.append(xyB[G].x + (xyC[G].x - xyB[G].x) / 4);
            YAtotal.append(xyA[G].y);
            YBtotal.append(xyB[G].y);
            ZAtotal.append(xyA[G].z * 0.75 + xyD[G].z * 0.25);
            ZBtotal.append(xyB[G].z * 0.75 + xyC[G].z * 0.25);
        }
    }










    // YCtotal = YBtotal;
    // YDtotal = YAtotal;


    for(int i = 0;i<Nx - 1;i++){
        for(int j = 0;j<Ny;j++){
            int G = i * Ny + j;
            XCtotal.append(XBtotal[G + Ny]);
            XDtotal.append(XAtotal[G + Ny]);
            YCtotal.append(YBtotal[G + Ny]);
            YDtotal.append(YAtotal[G + Ny]);
            ZCtotal.append(ZBtotal[G + Ny]);
            ZDtotal.append(ZAtotal[G + Ny]);
        }
    }


    for(int i = 0;i<Ny;i++){
        //double x1 = xSalida[i + 1] + 0.25 * abs(xyB[N - Ny + i].x - xyC[N - Ny + i].x);
        //double x2 = xSalida[i] + 0.25 * abs(xyA[N - Ny + i].x - xyD[N - Ny + i].x);
        double x1 = xyC[N - Ny + i].x + 0.25 * abs(xyB[N - Ny + i].x - xyC[N - Ny + i].x);
        double x2 = xyD[N - Ny + i].x + 0.25 * abs(xyA[N - Ny + i].x - xyD[N - Ny + i].x);
        XCtotal.append(x1);
        XDtotal.append(x2);
        double y1 = (xyB[N - Ny + i].y - xyC[N - Ny + i].y) /  (xyB[N - Ny + i].x - xyC[N - Ny + i].x) * (x1 - xyC[N - Ny + i].x) + xyC[N - Ny + i].y;
        double y2 = (xyA[N - Ny + i].y - xyD[N - Ny + i].y) /  (xyA[N - Ny + i].x - xyD[N - Ny + i].x) * (x2 - xyD[N - Ny + i].x) + xyD[N - Ny + i].y;
        YCtotal.append(y1);
        YDtotal.append(y2);
        double z1 = (xyB[N - Ny + i].z - xyC[N - Ny + i].z) /  (xyB[N - Ny + i].x - xyC[N - Ny + i].x) * (x1 - xyC[N - Ny + i].x) + xyC[N - Ny + i].z;
        double z2 = (xyA[N - Ny + i].z - xyD[N - Ny + i].z) /  (xyA[N - Ny + i].x - xyD[N - Ny + i].x) * (x2 - xyD[N - Ny + i].x) + xyD[N - Ny + i].z;
        ZCtotal.append(z1);
        ZDtotal.append(z2);
    }

    //six
    for(int i = 0;i<Nx;i++){
        for(int j = 0;j<Ny;j++){
            int G = i * Ny + j;
            XEtotal.append(xyD[G].x);
            XFtotal.append(xyC[G].x);
            YEtotal.append(xyD[G].y);
            YFtotal.append(xyC[G].y);
            ZEtotal.append(xyD[G].z);
            ZFtotal.append(xyC[G].z);
        }
    }



    /*
    for(int i = 0;i<Ny;i++){
        XCtotal.append(500 * chordLengthW[0]);
        XDtotal.append(500 * chordLengthW[0]);
        XAtotal.append(XDtotal[N - Ny + i]);
        XBtotal.append(XCtotal[N - Ny + i]);
        YAtotal.append(YDtotal[N - Ny + i]);
        YBtotal.append(YCtotal[N - Ny + i]);
        YCtotal.append(YCtotal[N - Ny + i]);
        YDtotal.append(YDtotal[N - Ny + i]);
        ZAtotal.append(ZDtotal[N - Ny + i]);
        ZBtotal.append(ZCtotal[N - Ny + i]);
        ZCtotal.append(ZCtotal[N - Ny + i]);
        ZDtotal.append(ZDtotal[N - Ny + i]);
    }

    for(int i = 0;i<Ny;i++){
        XEtotal.append(XDtotal[N - Ny + i] + chordLengthW[0]);
        XFtotal.append(XCtotal[N - Ny + i] + chordLengthW[0]);
        YEtotal.append(YDtotal[N - Ny + i]);
        YFtotal.append(YCtotal[N - Ny + i]);
        ZEtotal.append(ZDtotal[N - Ny + i]);
        ZFtotal.append(ZCtotal[N - Ny + i]);

    }
    */



    for(int i = 0;i<N;i++){
        xFptotal.append((XAtotal[i] + XBtotal[i]) / 2);
        yFptotal.append((YAtotal[i] + YBtotal[i]) / 2);
        zFptotal.append((ZAtotal[i] + ZBtotal[i]) / 2);
    }



}

void propellerVLM::generateWakeMesh(){
    if (rpm < 1)
        rpm = 1;
    int gridC = 20;

    if(!omegaArray.isEmpty()){
        omegaArray.clear();
        XAtotalArray.clear();YAtotalArray.clear();ZAtotalArray.clear();
        XBtotalArray.clear();YBtotalArray.clear();ZBtotalArray.clear();
        XCtotalArray.clear();YCtotalArray.clear();ZCtotalArray.clear();
        XDtotalArray.clear();YDtotalArray.clear();ZDtotalArray.clear();
        XEtotalArray.clear();YEtotalArray.clear();ZEtotalArray.clear();
        XFtotalArray.clear();YFtotalArray.clear();ZFtotalArray.clear();
    }


    //更新尾迹初始位置
    QVector<point3d>initialCod;//初始位置的坐标
    QVector<double>initialR;//初始位置的旋转半径
    QVector<double>initialA;//初始位置的角度
    for(int i = N - Ny;i<N;i++){
        point3d a(XDtotal[i],YDtotal[i],ZDtotal[i]);
        initialCod.append(a);

    }
    point3d a(XCtotal[N - 1],YCtotal[N - 1],ZCtotal[N - 1]);
    initialCod.append(a);
    for(int i = 0;i<Ny + 1;i++){
        initialR.append(sqrt(initialCod[i].x * initialCod[i].x + initialCod[i].y * initialCod[i].y));
        initialA.append(atan2(initialCod[i].y,initialCod[i].x));
    }


    //更新尾迹点





    for(int i = 0;i<rpmArray.length();i++)
        omegaArray.append(rpmArray[i] / 30 * M_PI);
    //qDebug()<<omegaArray;

    for(int p = 0;p<vArray.length();p++){

        for(int q = 0;q<rpmArray.length();q++){

            double dt = dt = M_PI * 2 / gridC / omegaArray[q];

            QVector<QVector<point3d>>wakeTmp;
            wakeTmp.append(initialCod);

            for(int i = 1;i<wakeStep;i++){
                QVector<point3d>tmp;
                for(int j = 0;j<Ny + 1;j++){

                    double z = initialCod[j].z;

                    double realx = initialR[j] * cos(initialA[j] + omegaArray[q] * dt * i);
                    double realy = initialR[j] * sin(initialA[j] + omegaArray[q] * dt * i);
                    double realz = z + vArray[p] * dt * i;
                    point3d a(realx,realy,realz);

                    tmp.append(a);
                }
                wakeTmp.append(tmp);
            }
            QVector<double>xaTmp = XAtotal;QVector<double>yaTmp = YAtotal;QVector<double>zaTmp = ZAtotal;
            QVector<double>xbTmp = XBtotal;QVector<double>ybTmp = YBtotal;QVector<double>zbTmp = ZBtotal;
            QVector<double>xcTmp = XCtotal;QVector<double>ycTmp = YCtotal;QVector<double>zcTmp = ZCtotal;
            QVector<double>xdTmp = XDtotal;QVector<double>ydTmp = YDtotal;QVector<double>zdTmp = ZDtotal;
            QVector<double>xeTmp = XEtotal;QVector<double>yeTmp = YEtotal;QVector<double>zeTmp = ZEtotal;
            QVector<double>xfTmp = XFtotal;QVector<double>yfTmp = YFtotal;QVector<double>zfTmp = ZFtotal;




            for(int i = 0;i<(wakeStep - 1) / 2;i++){
                for(int j = 0;j<Ny;j++){
                    xaTmp.append(wakeTmp[i * 2][j].x);
                    xbTmp.append(wakeTmp[i * 2][j + 1].x);
                    xeTmp.append(wakeTmp[i * 2 + 1][j].x);
                    xfTmp.append(wakeTmp[i * 2 + 1][j + 1].x);
                    xcTmp.append(wakeTmp[i * 2 + 2][j + 1].x);
                    xdTmp.append(wakeTmp[i * 2 + 2][j].x);

                    yaTmp.append(wakeTmp[i * 2][j].y);
                    ybTmp.append(wakeTmp[i * 2][j + 1].y);
                    yeTmp.append(wakeTmp[i * 2 + 1][j].y);
                    yfTmp.append(wakeTmp[i * 2 + 1][j + 1].y);
                    ycTmp.append(wakeTmp[i * 2 + 2][j + 1].y);
                    ydTmp.append(wakeTmp[i * 2 + 2][j].y);

                    zaTmp.append(wakeTmp[i * 2][j].z);
                    zbTmp.append(wakeTmp[i * 2][j + 1].z);
                    zeTmp.append(wakeTmp[i * 2 + 1][j].z);
                    zfTmp.append(wakeTmp[i * 2 + 1][j + 1].z);
                    zcTmp.append(wakeTmp[i * 2 + 2][j + 1].z);
                    zdTmp.append(wakeTmp[i * 2 + 2][j].z);
                }
            }
            XAtotalArray.append(xaTmp);YAtotalArray.append(yaTmp);ZAtotalArray.append(zaTmp);
            XBtotalArray.append(xbTmp);YBtotalArray.append(ybTmp);ZBtotalArray.append(zbTmp);
            XCtotalArray.append(xcTmp);YCtotalArray.append(ycTmp);ZCtotalArray.append(zcTmp);
            XDtotalArray.append(xdTmp);YDtotalArray.append(ydTmp);ZDtotalArray.append(zdTmp);
            XEtotalArray.append(xeTmp);YEtotalArray.append(yeTmp);ZEtotalArray.append(zeTmp);
            XFtotalArray.append(xfTmp);YFtotalArray.append(yfTmp);ZFtotalArray.append(zfTmp);
        }
    }



    /*
    omega = rpm / 30 * M_PI;
    double dt = M_PI * 2 / gridC / omega;
    //int step = 50;


    QVector<QVector<point3d>>wakeTmp;
    //更新尾迹初始位置
    QVector<point3d>initialCod;//初始位置的坐标
    QVector<double>initialR;//初始位置的旋转半径
    QVector<double>initialA;//初始位置的角度
    for(int i = N - Ny;i<N;i++){
        point3d a(XDtotal[i],YDtotal[i],ZDtotal[i]);
        initialCod.append(a);

    }
    point3d a(XCtotal[N - 1],YCtotal[N - 1],ZCtotal[N - 1]);
    initialCod.append(a);
    for(int i = 0;i<Ny + 1;i++){
        initialR.append(sqrt(initialCod[i].x * initialCod[i].x + initialCod[i].y * initialCod[i].y));
        initialA.append(atan2(initialCod[i].y,initialCod[i].x));
    }



    //更新尾迹点


    wakeTmp.append(initialCod);
    for(int i = 1;i<wakeStep;i++){
        QVector<point3d>tmp;
        for(int j = 0;j<Ny + 1;j++){

            double z = initialCod[j].z;

            double realx = initialR[j] * cos(initialA[j] + omega * dt * i);
            double realy = initialR[j] * sin(initialA[j] + omega * dt * i);
            double realz = z + vinf * dt * i;
            point3d a(realx,realy,realz);

            tmp.append(a);
        }
        wakeTmp.append(tmp);
    }




    for(int i = 0;i<(wakeStep - 1) / 2;i++){
        for(int j = 0;j<Ny;j++){
            XAtotal.append(wakeTmp[i * 2][j].x);
            XBtotal.append(wakeTmp[i * 2][j + 1].x);
            XEtotal.append(wakeTmp[i * 2 + 1][j].x);
            XFtotal.append(wakeTmp[i * 2 + 1][j + 1].x);
            XCtotal.append(wakeTmp[i * 2 + 2][j + 1].x);
            XDtotal.append(wakeTmp[i * 2 + 2][j].x);

            YAtotal.append(wakeTmp[i * 2][j].y);
            YBtotal.append(wakeTmp[i * 2][j + 1].y);
            YEtotal.append(wakeTmp[i * 2 + 1][j].y);
            YFtotal.append(wakeTmp[i * 2 + 1][j + 1].y);
            YCtotal.append(wakeTmp[i * 2 + 2][j + 1].y);
            YDtotal.append(wakeTmp[i * 2 + 2][j].y);

            ZAtotal.append(wakeTmp[i * 2][j].z);
            ZBtotal.append(wakeTmp[i * 2][j + 1].z);
            ZEtotal.append(wakeTmp[i * 2 + 1][j].z);
            ZFtotal.append(wakeTmp[i * 2 + 1][j + 1].z);
            ZCtotal.append(wakeTmp[i * 2 + 2][j + 1].z);
            ZDtotal.append(wakeTmp[i * 2 + 2][j].z);
        }
    }
    */


}
void propellerVLM::mergeMesh(){
    for(int i = 0;i<N + Ny;i++){
        point3d a(XAtotal[i],YAtotal[i],ZAtotal[i]);
        point3d b(XBtotal[i],YBtotal[i],ZBtotal[i]);
        point3d c(XCtotal[i],YCtotal[i],ZCtotal[i]);
        point3d d(XDtotal[i],YDtotal[i],ZDtotal[i]);
        xyTA.append(a);
        xyTB.append(b);
        xyTC.append(c);
        xyTD.append(d);
    }
}
void propellerVLM::generate3DModel(){
    QVector<QVector<point3d>> tmp1;

    for (int i = 0; i < num + 1; i++) {
        int index = Ny; // 初始化index
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
void propellerVLM::generate3DMesh(){

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


    //int len = Nx + (wakeStep - 1) / 2;
    int len = Nx;
    for(int i = 0;i<len;i++){
        for(int j = 0;j<Ny;j++){
            int G = i * Ny + j;
            gridA.append(point3d (XAtotal[G],YAtotal[G],ZAtotal[G]));
            gridB.append(point3d (XBtotal[G],YBtotal[G],ZBtotal[G]));
            gridC.append(point3d (XFtotal[G],YFtotal[G],ZFtotal[G]));
            gridD.append(point3d (XEtotal[G],YEtotal[G],ZEtotal[G]));

            gridA.append(point3d (XEtotal[G],YEtotal[G],ZEtotal[G]));
            gridB.append(point3d (XFtotal[G],YFtotal[G],ZFtotal[G]));
            gridC.append(point3d (XCtotal[G],YCtotal[G],ZCtotal[G]));
            gridD.append(point3d (XDtotal[G],YDtotal[G],ZDtotal[G]));
        }
    }




}

void propellerVLM::computeMeanCamberAirfoil(){
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
void propellerVLM::computeMeanCamberCst(){
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

void propellerVLM::interpolationAirfoil(){


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

        /*
        for(int i = Ny / 2 - 1;i>=0;i--){
            realCst.append(realCst[i]);
            spanCstArray.append(spanCstArray[i]);
            //
            upperYte.append(upperYte[i]);
            lowerYte.append(lowerYte[i]);
        }
        */
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

void propellerVLM::leyBiotSavart3D(const double xr, const double yr, const double zr,
                              const QVector<double> &xP, const QVector<double> &yP, const QVector<double> &zP,
                              const QVector<double> &xQ, const QVector<double> &yQ, const QVector<double> &zQ,
                              QVector<double> &vX, QVector<double> &vY, QVector<double> &vZ) {

    double hl_limit = 1e-7; // 数值稳定性参数，类似MATLAB的hl_limit
    int num = xP.length();  // 涡段的数量


    for (int i = 0; i < num; i++) {
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
/*
void propellerVLM::leyBiotSavart3D(const double xr, const double yr, const double zr,
                              const QVector<double> &xP, const QVector<double> &yP, const QVector<double> &zP,
                              const QVector<double> &xQ, const QVector<double> &yQ, const QVector<double> &zQ,
                              QVector<double> &vX, QVector<double> &vY, QVector<double> &vZ,double r) {

    double hl_limit = 1e-7; // 数值稳定性参数，类似MATLAB的hl_limit
    int num = xP.length();  // 涡段的数量


    for (int i = 0; i < num; i++) {
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
        //bool SW_hl_limit = (Ratio_hl < hl_limit);

        // 计算 F2 = F1 / LF1
        double F2x = F1x / LF1;
        double F2y = F1y / LF1;
        double F2z = F1z / LF1;

        // 计算 L2 = R0 · L1
        double L2 = R0x * L1x + R0y * L1y + R0z * L1z;

        //double tmp = Lrp * Lrp /  sqrt(pow(r,4) + pow(Lrp,4));
        // 计算最终的下洗速度 VDW
        double vdwX = F2x * L2 / (4 * M_PI);
        double vdwY = F2y * L2 / (4 * M_PI);
        double vdwZ = F2z * L2 / (4 * M_PI);

        // 对hl_limit和涡段长度为0的情况进行处理
        if ( SW_l0 || Ratio_hl < hl_limit) {
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
*/
double propellerVLM::computeZ(const int index, const double x, const double c){
    double z = 0;
    for(int i = 1;i<cstNum + 1;i++){
        z = realCst[index][i - 1] * std::pow(x,0.5) * std::pow(1 - x,1) * nchoosek(cstNum - 1,i-1) * std::pow(1 - x,cstNum - i) * std::pow(x,i - 1) + z;
    }


    return z * c;
}
int propellerVLM::nchoosek(const int order,const int index) {

    int result = 1;
    for(int i = 0;i < index;i++) {
        result = result * (order - i) / (i + 1);
    }
    return result;
}
point3d propellerVLM::calculateVector(point3d P1, point3d P2){
    point3d a(P2.x - P1.x, P2.y - P1.y, P2.z - P1.z);
    return normalizeVector(a);
}
point3d propellerVLM::normalizeVector(point3d vec){
    double length = sqrt(vec.x * vec.x + vec.y * vec.y + vec.z * vec.z);
    return point3d(vec.x / length, vec.y / length, vec.z / length);
}
double propellerVLM::computeUpperZ(const int index, const double x, const double c){
    double z = 0;
    for(int i = 1;i<cstNum + 1;i++){
        z = spanCstArray[index][i - 1] * std::pow(x,0.5) * std::pow(1 - x,1) * nchoosek(cstNum - 1,i-1) * std::pow(1 - x,cstNum - i) * std::pow(x,i - 1) + z;
    }


    return (z + upperYte[index] * x) * c;
}
double propellerVLM::computeLowerZ(const int index, const double x, const double c){
    double z = 0;
    for(int i = 1;i<cstNum + 1;i++){
        z = spanCstArray[index][cstNum * 2 - i] * std::pow(x,0.5) * std::pow(1 - x,1) * nchoosek(cstNum - 1,i-1) * std::pow(1 - x,cstNum - i) * std::pow(x,i - 1) + z;
    }
    return (z + lowerYte[index] * x) * c;
}
point3d propellerVLM::rotatePointAroundAxis(const point3d& point, const point3d& axisPoint, const point3d& axisDir, double angle) {
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
double propellerVLM::computeNormalInfluences(const point3d &ci, const point3d &n){
    return  ci.x * n.x + ci.y * n.y + ci.z * n.z;
}
double propellerVLM::dotProduct(const point3d a, const point3d b){
    return a.x * b.x + a.y * b.y + a.z * b.z;
}
point3d propellerVLM::rotateCod(const point3d &x, double angle){


       // 旋转后的新坐标
       point3d rotated;

       // 绕z轴旋转
       rotated.x = x.x * cos(angle) - x.y * sin(angle);
       rotated.y = x.x * sin(angle) + x.y * cos(angle);
       rotated.z = x.z; // z坐标不变

       return rotated;
}
QVector<double> propellerVLM::calculateBendingMoment(const QVector<double>& spanForce, const QVector<double>& yt) {
    QVector<double>tmp;
    int len = yt.length();
    for(int i = 0;i<len;i++){
        double tmp1 = 0;
        for(int j = 0;j<len - i;j++){
            tmp1 += (spanYt[len - i - 1] - yt[j]) * spanForce[j];
        }
        tmp.append(tmp1);
    }
    tmp.append(0);
    std::reverse(tmp.begin(), tmp.end());
    for(int i = len - 1;i>=0;i--)
        tmp.append(tmp[i]);


    return tmp;
}
double propellerVLM::calculateTorque(const QVector<double>& spanForce, const QVector<double>& yt) {
    int len = yt.length();
    double tmp1 = 0;
    for(int j = 0;j<len;j++){
        tmp1 += (-yt[j]) * spanForce[j];
    }
    return tmp1;
}
void propellerVLM::computePressure(){
    QVector<QVector<double>>colors;
    for(int i = 0;i<vinfArray.length();i++){
        QVector<double>tmpColor;
        QVector<double>tmp;
        for(int j = 0;j<N;j++){
            tmp.append(realGammaArray[i][j] * vinfArray[i][j].x);
        }

        double maxT = max(tmp);
        double minT = min(tmp);
        double maxTmp = maxT - minT;
        for(int j = 0;j<N;j++){
            tmpColor.append(1.0 - (-minT + tmp[j]) / maxTmp);
        }
        colors.append(tmpColor);
    }
    pressureColorArray = colors;
}
void propellerVLM::dealResult(){
    //计算有效功率
    QVector<double>pArray;
    int row = vArray.length();
    int col = rpmArray.length();
    for(int i = 0;i<row;i++){
        for(int j = 0;j<col;j++){
            int k = i * col + j;
            double a1 = torqueArray[k] * omegaArray[j];
            pArray.append(a1);
        }
    }
    //计算效率
    QVector<double>nArray;
    for(int i = 0;i<row;i++){
        for(int j = 0;j<col;j++){
            int k = i * col + j;
            double a2 = 0;
            if(thrustArray[k] > 0)
                a2 = thrustArray[k] * -vArray[i] / pArray[k];

            nArray.append(a2);
        }
    }
    //计算前进比
    QVector<double>jArray;
    for(int i = 0;i<row;i++){
        for(int j = 0;j<col;j++){
            double a3 = abs(vArray[i] / spanW[num - 1] / rpmArray[j] * 30);
            jArray.append(a3);
        }
    }


    //计算推力系数
    QVector<double>tArray;
    QVector<double>cpArray;
    QVector<double>cArray;
    for(int i = 0;i<row;i++){
        for(int j = 0;j<col;j++){
            double a4 = 0;
            double a5 = 0;
            double a6 = 0;
            int k = i * col + j;
            if(thrustArray[k] > 0){

                double d = abs(spanW[num - 1]) * 2;
                double n = rpmArray[j] / 60;
                a4 = thrustArray[k] / density / n / n / (pow(d,4));
                a5 = pArray[k] / density / n / n / n / (pow(d,5));
                a6 = a4 * jArray[k] / a5;
            }


            tArray.append(a4);
            cpArray.append(a5);
            cArray.append(a6);
        }
    }


    //计算扭矩系数

    resultArray.append(pArray);
    resultArray.append(nArray);
    resultArray.append(thrustArray);
    resultArray.append(torqueArray);
    resultArray.append(jArray);
    resultArray.append(tArray);
    resultArray.append(cpArray);

    qDebug()<<"功率"<<resultArray[0];
    qDebug()<<"效率"<<resultArray[1];
    qDebug()<<"推力"<<resultArray[2];
    qDebug()<<"扭矩"<<resultArray[3];
    qDebug()<<" 前进比"<<resultArray[4];
    qDebug()<<" 推力系数"<<resultArray[5];
    qDebug()<<"功率系数"<<resultArray[6];
    //qDebug()<<"有效功率"<< pArray;
    //qDebug()<<"效率"<<nArray;
    //qDebug()<<"前进比"<<jArray;
    //qDebug()<<"ct"<<tArray;
    //qDebug()<<"p"<<cpArray;
    //qDebug()<<"推力"<<thrustArray;
    //qDebug()<<"力矩"<<torqueArray;
    //推力
    //力矩

}
point3d propellerVLM::rungeKutta4(const point3d& position,const double deltaTime,const int index){

    point3d k1 = calculateVelocity(position,index);
    point3d position1(position.x + k1.x * (deltaTime / 2),position.y + k1.y * (deltaTime / 2),position.z + k1.z * (deltaTime / 2));
    point3d k2 = calculateVelocity(position1,index);
    point3d position2(position.x + k2.x * (deltaTime / 2),position.y + k2.y * (deltaTime / 2),position.z + k2.z * (deltaTime / 2));
    point3d k3 = calculateVelocity(position2,index);
    point3d position3(position.x + k3.x * deltaTime,position.y + k3.y * deltaTime,position.z + k3.z * deltaTime);
    point3d k4 = calculateVelocity(position3,index);
    double x1 = position.x + (k1.x + k2.x * 2 + k3.x * 2 + k4.x) * (deltaTime / 6);
    double y1 = position.y + (k1.y + k2.y * 2 + k3.y * 2 + k4.y) * (deltaTime / 6);
    double z1 = position.z + (k1.z + k2.z * 2 + k3.z * 2 + k4.z) * (deltaTime / 6);
    return point3d(x1,y1,z1);
}
point3d propellerVLM::calculateVelocity(const point3d& position,const int index){

    int rpmIndex = streamIndex%(rpmArray.length());
    //int rpmIndex = 0;
    // QVector<double>vxAB;QVector<double>vxBC;QVector<double>vxCD;QVector<double>vxDA;
    // QVector<double>vyAB;QVector<double>vyBC;QVector<double>vyCD;QVector<double>vyDA;
    // QVector<double>vzAB;QVector<double>vzBC;QVector<double>vzCD;QVector<double>vzDA;
    QVector<double> vxAB, vyAB, vzAB;
    QVector<double> vxBF, vyBF, vzBF;
    QVector<double> vxFC, vyFC, vzFC;
    QVector<double> vxCD, vyCD, vzCD;
    QVector<double> vxDE, vyDE, vzDE;
    QVector<double> vxEA, vyEA, vzEA;
    leyBiotSavart3D(position.x,position.y,position.z, XAtotalArray[streamIndex], YAtotalArray[streamIndex], ZAtotalArray[streamIndex], XBtotalArray[streamIndex], YBtotalArray[streamIndex], ZBtotalArray[streamIndex], vxAB, vyAB, vzAB);
    leyBiotSavart3D(position.x,position.y,position.z, XBtotalArray[streamIndex], YBtotalArray[streamIndex], ZBtotalArray[streamIndex], XFtotalArray[streamIndex], YFtotalArray[streamIndex], ZFtotalArray[streamIndex], vxBF, vyBF, vzBF);
    leyBiotSavart3D(position.x,position.y,position.z, XFtotalArray[streamIndex], YFtotalArray[streamIndex], ZFtotalArray[streamIndex], XCtotalArray[streamIndex], YCtotalArray[streamIndex], ZCtotalArray[streamIndex], vxFC, vyFC, vzFC);
    leyBiotSavart3D(position.x,position.y,position.z, XCtotalArray[streamIndex], YCtotalArray[streamIndex], ZCtotalArray[streamIndex], XDtotalArray[streamIndex], YDtotalArray[streamIndex], ZDtotalArray[streamIndex], vxCD, vyCD, vzCD);
    leyBiotSavart3D(position.x,position.y,position.z, XDtotalArray[streamIndex], YDtotalArray[streamIndex], ZDtotalArray[streamIndex], XEtotalArray[streamIndex], YEtotalArray[streamIndex], ZEtotalArray[streamIndex], vxDE, vyDE, vzDE);
    leyBiotSavart3D(position.x,position.y,position.z, XEtotalArray[streamIndex], YEtotalArray[streamIndex], ZEtotalArray[streamIndex], XAtotalArray[streamIndex], YAtotalArray[streamIndex], ZAtotalArray[streamIndex], vxEA, vyEA, vzEA);


    // leyBiotSavart3D(position.x,position.y,position.z,XAtotal,YAtotal,ZAtotal,XBtotal,YBtotal,ZBtotal,vxAB,vyAB,vzAB);
    // leyBiotSavart3D(position.x,position.y,position.z,XBtotal,YBtotal,ZBtotal,XCtotal,YCtotal,ZCtotal,vxBC,vyBC,vzBC);
    // leyBiotSavart3D(position.x,position.y,position.z,XCtotal,YCtotal,ZCtotal,XDtotal,YDtotal,ZDtotal,vxCD,vyCD,vzCD);
    // leyBiotSavart3D(position.x,position.y,position.z,XDtotal,YDtotal,ZDtotal,XAtotal,YAtotal,ZAtotal,vxDA,vyDA,vzDA);
    QVector<double>vx;QVector<double>vy;QVector<double>vz;
    for(int i = 0;i<N + Ny * (wakeStep - 1) / 2;i++){
        vx.append(vxAB[i] + vxBF[i] + vxFC[i] + vxCD[i] + vxDE[i] + vxEA[i]);
        vy.append(vyAB[i] + vyBF[i] + vyFC[i] + vyCD[i] + vyDE[i] + vyEA[i]);
        vz.append(vzAB[i] + vzBF[i] + vzFC[i] + vzCD[i] + vzDE[i] + vzEA[i]);

    }

    double vxt = 0;
    double vyt = 0;
    double vzt = 0;

    for(int i = 0;i<N + Ny * (wakeStep - 1) / 2;i++){
        vxt = vxt + vx[i] * matrixGammaArray[streamIndex][i];
        vyt = vyt + vy[i] * matrixGammaArray[streamIndex][i];
        vzt = vzt + vz[i] * matrixGammaArray[streamIndex][i];
    }
    //double vm = vinfArray[streamIndex][index].x;
    double vm = -yt[index] * omegaArray[rpmIndex];


    //return point3d(vxt + vinfArray[streamIndex][index].x,vyt + vinfArray[streamIndex][index].y,vzt + vinfArray[streamIndex][index].z);
    //return point3d(vxt,vyt,vzt + vinfArray[streamIndex][index].z);
    return point3d(vxt + vm * cos(thate),vyt + vm * sin(thate),vzt + vinfArray[streamIndex][index].z);
}
double propellerVLM::max(const QVector<double>&a){
    if (a.isEmpty()) {
        throw std::runtime_error("The QVector is empty");
    }
    return *std::max_element(a.begin(), a.end());
}
double propellerVLM::min(const QVector<double>&a){
    if (a.isEmpty()) {
        throw std::runtime_error("The QVector is empty");
    }
    return *std::min_element(a.begin(), a.end());
}
void propellerVLM::clearAllValue(){


    if(!yt.isEmpty()){
        resultArray.clear();
        thrustArray.clear();
        torqueArray.clear();
        badCSTArray.clear();
        badDragIndex.clear();
        badClArray.clear();
        badCdArray.clear();
        badAlphaArray.clear();
        badSettingArray.clear();
        reArray.clear();
        xyA.clear();xyB.clear();xyC.clear();xyD.clear();
        //contourArray.clear();
        xyTA.clear();xyTB.clear();xyTC.clear();xyTD.clear();
        //totalForce.clear();
        //wingCL.clear();
        //wingCD.clear();
        //wingCM.clear();

        spanForceYt.clear();
        VX.clear();
        VY.clear();
        VZ.clear();
        VM.clear();
        VN.clear();

        yt.clear();
        cy.clear();
        cv.clear();
        xAtaque.clear();
        xSalida.clear();

        index.clear();


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

        xptotal.clear();
        yptotal.clear();
        zptotal.clear();
        xFptotal.clear();
        yFptotal.clear();
        zFptotal.clear();

        XAtotal.clear();
        XBtotal.clear();
        XCtotal.clear();
        XDtotal.clear();
        XEtotal.clear();
        XFtotal.clear();

        YAtotal.clear();
        YBtotal.clear();
        YCtotal.clear();
        YDtotal.clear();
        YEtotal.clear();
        YFtotal.clear();

        ZAtotal.clear();
        ZBtotal.clear();
        ZCtotal.clear();
        ZDtotal.clear();
        ZEtotal.clear();
        ZFtotal.clear();

        cmedia.clear();
        cyp.clear();

        Xptotal.clear();
        Yptotal.clear();
        Cyp.clear();
        xCurvatura.clear();



        vTotal.clear();

        nx.clear();
        ny.clear();
        nz.clear();

        nx2.clear();
        ny2.clear();
        nz2.clear();

        Zn1.clear();
        Yn1.clear();
        Xn1.clear();

        Zn2.clear();
        Yn2.clear();
        Xn2.clear();
        vPQ.clear();  //附着涡长度向量

        vinx.clear();
        viny.clear();
        vinz.clear();
        //vinn.clear();



        Xn.clear();Yn.clear();Zn.clear();



        //areas.clear();
        //hs.clear();

        spanDragArray.clear();
        spanLiftArray.clear();
        //spanDrag.clear();

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
