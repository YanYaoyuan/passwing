

#include "airfoildesign.h"

#include <cmath>

#include <Eigen/Sparse>
#include <Eigen/Dense>
#include <QFile>

#include <QDebug>
#include <QThread>
#include <QTime>


#include <Eigen/IterativeLinearSolvers>



#define Exp 2.7182818284590
#define M_PI 3.141592653589

airfoilDesign::airfoilDesign(const int n)

{
    cstNum = n;
}
airfoilDesign::airfoilDesign(){

}


void airfoilDesign::splitAirfoilData(const QVector<QVector<double>> &airfoildata) {
    int length = airfoildata.size();
    if(length < 2) return;

    // 1️⃣ 找最小 x（前缘），浮点安全
    double minx = airfoildata[0][0];
    int j = 0;
    for(int i = 1; i < length; i++){
        if(airfoildata[i][0] < minx){
            minx = airfoildata[i][0];
            j = i;
        }

    }

    QVector<double>tmp = airfoildata[j];


    // 2️⃣ 分割上表面和下表面
    QVector<QVector<double>> temporaryAirfoilUpper;
    QVector<QVector<double>> temporaryAirfoilLower;



    int i = 0;
    yTe1 = airfoildata[0][1];
    yTe2 = airfoildata[length - 1][1];

    // 上表面：前缘之前的点
    while(i < length && airfoildata[i][0] > minx + 1e-12){
        temporaryAirfoilUpper.append(airfoildata[i]);
        i++;
    }
    if(i < length) temporaryAirfoilUpper.append(airfoildata[i]); // 包括前缘点
    upperData = temporaryAirfoilUpper;

    // 下表面：前缘之后的点
    for(int j = i + 1; j < length; j++){
        temporaryAirfoilLower.append(airfoildata[j]);
    }


    lowerData = temporaryAirfoilLower;
    lowerData.insert(0, tmp);


    // 3️⃣ 去掉 x < 0 的点
    auto removeNegX = [](QVector<QVector<double>>& data){
        QVector<QVector<double>> tmp;
        for(auto &pt : data){
            if(pt[0] >= 0.0) tmp.append(pt);
        }
        data = tmp;
    };
    removeNegX(upperData);
    removeNegX(lowerData);

    // 4️⃣ 去掉相邻重复 x 点（浮点安全）
    auto removeDuplicateX = [](QVector<QVector<double>>& data){
        if(data.size() < 2) return;
        QVector<QVector<double>> tmp;
        tmp.append(data[0]);
        for(int i = 1; i < data.size(); i++){
            if(qAbs(data[i][0] - data[i-1][0]) > 1e-12)
                tmp.append(data[i]);
        }
        data = tmp;
    };
    removeDuplicateX(upperData);
    removeDuplicateX(lowerData);

    // 5️⃣ 合并成 newAirfoilData
    newAirfoilData.clear();
    newAirfoilData.append(upperData);
    newAirfoilData.append(temporaryAirfoilLower);




}


void airfoilDesign::buildBenrnstein(const QVector<QVector<double>> &airfoildata) {


    splitAirfoilData(airfoildata);
    int lengthUp = upperData.size();
    int lengthLo = lowerData.size();


    QVector<double> temporaryCstParameter;
    Eigen::MatrixXd matrixSupper;
    Eigen::MatrixXd matrixSlower;

    Eigen::VectorXd Yupper(lengthUp);
    Eigen::VectorXd Ylower(lengthLo);






    matrixSupper.resize(lengthUp,cstNum);
    matrixSlower.resize(lengthLo,cstNum);
    Yupper.resize(lengthUp);
    Ylower.resize(lengthLo);

    double  up, lo;
    for(int i = 0; i < lengthUp ;i++) {
        for(int j = 1; j < cstNum + 1 ;j++){
            up = std::pow(upperData[i][0],0.5) * std::pow(1 - upperData[i][0],1) * nchoosek(cstNum - 1,j-1) * std::pow(1 - upperData[i][0],cstNum - j) * std::pow(upperData[i][0],j - 1);
            matrixSupper.coeffRef(i,j-1) = up;
        }
        Yupper(i)= upperData[i][1] - upperData[i][0] * yTe1;

    }

    for(int i = 0; i < lengthLo;i++) {
        for(int j = 1; j < cstNum + 1 ;j++) {
            lo = std::pow(lowerData[i][0],0.5) * std::pow(1 - lowerData[i][0],1) * nchoosek(cstNum - 1,j-1) * std::pow(1 - lowerData[i][0],cstNum - j) * std::pow(lowerData[i][0],j - 1);

            matrixSlower.coeffRef(i,j-1) = lo;
        }
        //lowerMatrix.append(loMatrix);
        Ylower(i) = lowerData[i][1] - lowerData[i][0] * yTe2;
        //<<"pp";
    }

    //Eigen::VectorXd cstUpper = matrixSupper.colPivHouseholderQr().solve(Yupper);
    Eigen::VectorXd cstUpper = (matrixSupper.transpose() * matrixSupper).inverse() * matrixSupper.transpose() * Yupper;
    Eigen::VectorXd cstLower = (matrixSlower.transpose() * matrixSlower).inverse() * matrixSlower.transpose() * Ylower;

    //Eigen::VectorXd cstLower = matrixSlower.colPivHouseholderQr().solve(Ylower);

    //Eigen::Vector3cd R = matrixSlower.colPivHouseholderQr().solve(Ylower);
    //Eigen::VectorXcd R = (matrixSlower.transpose() * matrixSlower).inverse() * matrixSlower.transpose() * Ylower;
    //Eigen::VectorXd R = cstLower.real();
    double a1 = cstUpper[0];
    double a2 = cstLower[0];
    if(!std::isnan(a1) && !std::isnan(a2))
        buildCSTSuccess = true;
    else
        buildCSTSuccess = false;


    for(int i = 0; i < cstNum;i++){
        temporaryCstParameter.append(cstUpper(i));
    }
    for(int i = 0; i < cstNum;i++){
        temporaryCstParameter.append(cstLower(cstNum - i -1));

    }



    cstParameter = temporaryCstParameter;

    //computeThickness(airfoildata);



}
void airfoilDesign::buildAirfoilMeanCamberBenrnstein(const QVector<QVector<double>>&airfoil){

    buildBenrnstein(airfoil);
    QVector<double>cstTmp;
    for(int i = 0;i<cstNum;i++){
        cstTmp.append((cstParameter[i] + cstParameter[cstNum * 2  - i - 1]) / 2 );
    }
    meanCstParameter = cstTmp;



}

void airfoilDesign::buildAirfoilCurve(const QVector<double>& cst){
    int lengthUp = upperData.size();
    int lengthLo = lowerData.size();

    double upY = 0;
    double loY = 0;

    QVector<QVector<double>> airfoilUpper;
    QVector<QVector<double>> airfoilLower;
    QVector<QVector<double>> airfoilSum;


    for(int i = 0; i < lengthUp; i++){
        for(int j = 1; j < cstNum + 1; j++){
            upY = cst[j - 1] * std::pow(upperData[i][0],0.5) * std::pow(1 - upperData[i][0],1) * nchoosek(cstNum - 1,j-1) * std::pow(1 - upperData[i][0],cstNum - j) * std::pow(upperData[i][0],j - 1) + upY;
        }


        QVector<double> row;
        row.append(upperData[i][0]);
        row.append(upY + upperData[i][0] * yTe1);
        airfoilUpper.append(row);
        upY = 0;
    }

    for(int i = 0; i < lengthLo; i++){
        for(int j = 1; j < cstNum + 1; j++){
            loY = cst[ cstNum * 2 - j] * std::pow(lowerData[i][0],0.5) * std::pow(1 - lowerData[i][0],1) * nchoosek(cstNum - 1,j-1) * std::pow(1 - lowerData[i][0],cstNum - j) * std::pow(lowerData[i][0],j - 1) + loY;
        }
        QVector<double> row;
        row.append(lowerData[i][0]);
        row.append(loY + lowerData[i][0] * yTe2);
        airfoilLower.append(row);
        loY = 0;
    }
    airfoilLower.removeAt(0);

    airfoilSum.append(airfoilUpper);
    airfoilSum.append(airfoilLower);
    //<<airfoilLower;

    newAirfoilData = airfoilSum;

}

void airfoilDesign::buildAirfoilCurve(const QVector<double>& cst,const QVector<double>&airfoilX){
    int lengthUp = airfoilX.length();
    int lengthLo = lengthUp - 1;
    QVector<double>airfoilX1;
    QVector<double>airfoilX2;

    for(int i = 0;i<lengthUp;i++){
        airfoilX1.append(airfoilX[lengthUp - i - 1]);
    }
    for(int i = 1;i<lengthUp;i++){
        airfoilX2.append(airfoilX[i]);
    }

    double upY = 0;
    double loY = 0;

    QVector<QVector<double>> airfoilUpper;
    QVector<QVector<double>> airfoilLower;
    QVector<QVector<double>> airfoilSum;

    for(int i = 0; i < lengthUp; i++){
        for(int j = 1; j < cstNum + 1; j++){
            upY = cst[j - 1] * std::pow(airfoilX1[i],0.5) * std::pow(1 - airfoilX1[i],1) * nchoosek(cstNum - 1,j-1) * std::pow(1 - airfoilX1[i],cstNum - j) * std::pow(airfoilX1[i],j - 1) + upY;
        }


        QVector<double> row;
        row.append(airfoilX1[i]);
        row.append(upY + airfoilX1[i] * yTe1);
        airfoilUpper.append(row);
        upY = 0;
    }

    for(int i = 0; i < lengthLo; i++){
        for(int j = 1; j < cstNum + 1; j++){
            loY = cst[ cstNum * 2 - j] * std::pow(airfoilX2[i],0.5) * std::pow(1 - airfoilX2[i],1) * nchoosek(cstNum - 1,j-1) * std::pow(1 - airfoilX2[i],cstNum - j) * std::pow(airfoilX2[i],j - 1) + loY;
        }
        QVector<double> row;
        row.append(airfoilX2[i]);
        row.append(loY + airfoilX2[i] * yTe2);
        airfoilLower.append(row);
        loY = 0;
    }

    //airfoilLower.removeAt(0);

    airfoilSum.append(airfoilUpper);
    airfoilSum.append(airfoilLower);


    newAirfoilData = airfoilSum;

}



void airfoilDesign::deflectedAirfoil(const QVector<QVector<double>> &airfoildata){


    splitAirfoilData(airfoildata);

    double upY = 0;
    double loY = 0;
    double originX = rotationAxis ,originY;
    double angleInRadians = flapAngle / 180 * M_PI;

    QVector<QVector<double>> upAirfoil;
    QVector<QVector<double>> loAirfoil;
    QVector<QVector<double>> totolAirfoil;


    upAirfoil = upperData;
    loAirfoil = lowerData;

    for(int j = 1; j < cstNum + 1; j++){
        upY = cstParameter[j - 1] * std::pow(originX,0.5) * std::pow(1 - originX,1) * nchoosek(cstNum - 1,j-1) * std::pow(1 - originX,cstNum - j) * std::pow(originX,j - 1) + upY + originX * yTe1;
    }
    for(int j = 1; j < cstNum + 1; j++){
        loY = cstParameter[cstNum * 2 - j] * std::pow(originX,0.5) * std::pow(1 - originX,1) * nchoosek(cstNum - 1,j-1) * std::pow(1 - originX,cstNum - j) * std::pow(originX,j - 1) + loY + originX * yTe2;
    }

    originY = (upY + loY) / 2;
    int uplength = 0;

    int lowerSize = lowerData.size();



    double upperOriginX = originX;
    double lowerOriginX = originX;
    while(upperOriginX < upperData[uplength][0]){

        uplength++;
    }
    int lolength = 0;
    while(lowerOriginX < lowerData[lowerSize - lolength - 1][0]){

        lolength++;
    }



    double rx,ry;
    double cosA = cos(angleInRadians);
    double sinA = sin(angleInRadians);
    for(int i = 0;i<uplength;i++){
        rx = (upperData[i][0] - originX) * cosA - (upperData[i][1] - originY) * sinA;
        ry = (upperData[i][0] - originX) * sinA + (upperData[i][1] - originY) * cosA;
        rx = rx + originX;
        ry = ry + originY;

        upAirfoil[i][0] = rx;
        upAirfoil[i][1] = ry;

    }



    for(int i = 0;i<lolength;i++){
        rx = (lowerData[lowerSize - i - 1][0] - originX) * cosA - (lowerData[lowerSize - i - 1][1] - originY) * sinA;
        ry = (lowerData[lowerSize - i - 1][0] - originX) * sinA + (lowerData[lowerSize - i - 1][1] - originY) * cosA;

        rx = rx + originX;
        ry = ry + originY;
        loAirfoil[lowerSize - i - 1][0] = rx;
        loAirfoil[lowerSize - i - 1][1] = ry;

    }


    //判断偏转上弧面还是下弧面
    int deleteindex = 0,deletenum = 0 ;

    if(angleInRadians >= 0){




        for(int i = 0;i<uplength;i++){

            if(upAirfoil[i][0] >= upAirfoil[uplength - 1][0] && upAirfoil[i][0] <=upperData[uplength - 1][0]){
                deletenum++;
                deleteindex = i;

         }

        }

        for(int i = 0;i<deletenum ;i++)
            upAirfoil.remove(deleteindex + 1 - i);


    }
    else{
        for(int i = 0;i<lolength;i++){

            if(loAirfoil[lowerSize - i - 1][0] >= loAirfoil[lowerSize - lolength][0] && loAirfoil[lowerSize - i - 1][0] <=lowerData[lowerSize - lolength][0]){
                deletenum++;
                deleteindex = i;

            }

        }

        for(int i = 0;i<deletenum + 1;i++)
            loAirfoil.remove(lowerSize - deleteindex - i + 1 );
    }



    totolAirfoil.append(upAirfoil);
    totolAirfoil.append(loAirfoil);





    flapAirfoilData = totolAirfoil;

}
void airfoilDesign::computeParameters(const QVector<double>&cstParameter){

    int num = 100;

    QVector<double> airfoilX;
    QVector<double> airfoilAllX;
    QVector<double> airfoilAllY;
    QVector<double> allThickness;



    if(buildCSTSuccess){

        for(int i = 0;i < num + 1;i++){

            airfoilX.append(0.5 - cos(static_cast<double>(i) * M_PI / num ) * 0.5);
            airfoilAllX.append(0.5 - cos(static_cast<double>(num - i) * M_PI / num ) * 0.5);
        }
        for(int i = 1;i < num + 1;i++){

            airfoilAllX.append(0.5 - cos(static_cast<double>(i) * M_PI / num ) * 0.5);
        }


        double upY = 0;
        double loY = 0;

        QVector<double> airfoilUpper;
        QVector<double> airfoilLower;
        QVector<double> midarcy;






        for(int i = 0; i < num + 1; i++){
            for(int j = 1; j < cstNum + 1; j++){
                upY = cstParameter[j - 1] * pow(airfoilX[i],0.5) * pow(1 - airfoilX[i],1) * nchoosek(cstNum - 1,j-1) * pow(1 - airfoilX[i],cstNum - j) * pow(airfoilX[i],j - 1) + upY;
            }

            airfoilUpper.append(upY + airfoilX[i] * yTe1);


            upY = 0;

        }

        for(int i = 0; i<num + 1;i++)
            airfoilAllY.append(airfoilUpper[num - i]);

        for(int i = 0; i < num + 1; i++){
            for(int j = 1; j < cstNum + 1; j++){
                loY = cstParameter[ cstNum * 2 - j] * pow(airfoilX[i],0.5) * pow(1 - airfoilX[i],1) * nchoosek(cstNum - 1,j-1) * pow(1 - airfoilX[i],cstNum - j) * pow(airfoilX[i],j - 1) + loY;
            }

            airfoilLower.append(loY + airfoilX[i] * yTe2);
            if(i != 0)
                airfoilAllY.append(loY + airfoilX[i] * yTe2);
            loY = 0;

        }





        for(int i = 0;i < num + 1;i++){
            allThickness.append(airfoilUpper[i] - airfoilLower[i]);

        }

        for(int i = 0;i<num + 1;i++){
            midarcy.append((airfoilUpper[i] + airfoilLower[i]) / 2);
        }





        minRadius = curvatureRadius(airfoilAllX[num - 1],airfoilAllX[num],airfoilAllX[num + 1],airfoilAllY[num - 1],airfoilAllY[num],airfoilAllY[num + 1]);

        double angleUp = computeTrailingAngle(airfoilX[num - 1],airfoilX[num],airfoilUpper[num - 1],airfoilUpper[num]) * 180 / M_PI;
        double angleLo = computeTrailingAngle(airfoilX[num - 1],airfoilX[num],airfoilLower[num - 1],airfoilLower[num]) * 180 / M_PI;


        int index1 = maxQVector(allThickness);
        int index3 = maxQVector(midarcy);


        maxThickness = allThickness[index1];
        locationThickness = airfoilX[index1];







        trailingAngle = angleLo - angleUp;
        maxCamber = midarcy[index3];
        locationCamber = airfoilX[index3];

    }
    else{
        computeSimpleParameters(newAirfoilData);
    }

}
void airfoilDesign::computeOtherParameters(const QVector<QVector<double>> &airfoil){

    int num = 100;

    QVector<double> airfoilX;
    QVector<double> airfoilAllX;
    QVector<double> airfoilAllY;
    QVector<double> allThickness;

    buildBenrnstein(airfoil);

    if(buildCSTSuccess){

        for(int i = 0;i < num + 1;i++){

            airfoilX.append(0.5 - cos(static_cast<double>(i) * M_PI / num ) * 0.5);
            airfoilAllX.append(0.5 - cos(static_cast<double>(num - i) * M_PI / num ) * 0.5);
        }
        for(int i = 1;i < num + 1;i++){

            airfoilAllX.append(0.5 - cos(static_cast<double>(i) * M_PI / num ) * 0.5);
        }


        double upY = 0;
        double loY = 0;

        QVector<double> airfoilUpper;
        QVector<double> airfoilLower;
        QVector<double> midarcy;






        for(int i = 0; i < num + 1; i++){
            for(int j = 1; j < cstNum + 1; j++){
                upY = cstParameter[j - 1] * pow(airfoilX[i],0.5) * pow(1 - airfoilX[i],1) * nchoosek(cstNum - 1,j-1) * pow(1 - airfoilX[i],cstNum - j) * pow(airfoilX[i],j - 1) + upY;
            }

            airfoilUpper.append(upY + airfoilX[i] * yTe1);


            upY = 0;

        }

        for(int i = 0; i<num + 1;i++)
            airfoilAllY.append(airfoilUpper[num - i]);

        for(int i = 0; i < num + 1; i++){
            for(int j = 1; j < cstNum + 1; j++){
                loY = cstParameter[ cstNum * 2 - j] * pow(airfoilX[i],0.5) * pow(1 - airfoilX[i],1) * nchoosek(cstNum - 1,j-1) * pow(1 - airfoilX[i],cstNum - j) * pow(airfoilX[i],j - 1) + loY;
            }

            airfoilLower.append(loY + airfoilX[i] * yTe2);
            if(i != 0)
                airfoilAllY.append(loY + airfoilX[i] * yTe2);
            loY = 0;

    }





        for(int i = 0;i < num + 1;i++){
            allThickness.append(airfoilUpper[i] - airfoilLower[i]);

        }

        for(int i = 0;i<num + 1;i++){
            midarcy.append((airfoilUpper[i] + airfoilLower[i]) / 2);
        }





        minRadius = curvatureRadius(airfoilAllX[num - 1],airfoilAllX[num],airfoilAllX[num + 1],airfoilAllY[num - 1],airfoilAllY[num],airfoilAllY[num + 1]);

        double angleUp = computeTrailingAngle(airfoilX[num - 1],airfoilX[num],airfoilUpper[num - 1],airfoilUpper[num]) * 180 / M_PI;
        double angleLo = computeTrailingAngle(airfoilX[num - 1],airfoilX[num],airfoilLower[num - 1],airfoilLower[num]) * 180 / M_PI;


        int index1 = maxQVector(allThickness);
        int index3 = maxQVector(midarcy);


        maxThickness = allThickness[index1];
        locationThickness = airfoilX[index1];






        trailingAngle = angleLo - angleUp;
        maxCamber = midarcy[index3];
        locationCamber = airfoilX[index3];
        }
    else{
        computeSimpleParameters(airfoil);
        }

}


void airfoilDesign::computeSimpleParameters(const QVector<QVector<double>> &airfoil){
    //QVector<double> airfoilX;
    QVector<double> allThickness;
    QVector<double> midarcy;

    splitAirfoilData(airfoil);
    int length = 0;
    if(lowerData.length() < upperData.length())
        length = lowerData.length();
    else
        length = upperData.length();

    for(int i = 0; i<length - 1;i++){
        allThickness.append(upperData[i][1] - lowerData[length - i - 1][1]);

    }


    for(int i = 0; i<length - 1;i++){
        midarcy.append((upperData[i][1] + lowerData[length - i - 1][1]) / 2);

    }

    minRadius = curvatureRadius(upperData[upperData.length() - 3][0],upperData[upperData.length() - 1][0],lowerData[1][0],upperData[upperData.length() - 3][1],upperData[upperData.length() - 1][1],lowerData[1][1]);

    double angleUp = computeTrailingAngle(airfoil[1][0],airfoil[0][0],airfoil[1][1],airfoil[0][1]) * 180 / M_PI;
    double angleLo = computeTrailingAngle(airfoil[airfoil.length() - 2][0],airfoil[airfoil.length() - 1][0],airfoil[airfoil.length() - 2][1],airfoil[airfoil.length() - 1][1]) * 180 / M_PI;
    trailingAngle = angleLo - angleUp;

    int index1 = maxQVector(allThickness);
    int index2 = maxQVector(midarcy);

    maxThickness = allThickness[index1];
    maxCamber = midarcy[index2];

    locationThickness = upperData[index1][0];
    locationCamber = upperData[index2][0];

}
void airfoilDesign::blendingAirfoil(const QVector<double>&cst,const double yt1,const double yt2){

    yTe1 = yt1;
    yTe2 = yt2;
    int num = 150;
    QVector<double>xData;

    for(int i = 0;i < num;i++){
        xData.append(0.5 - cos(static_cast<double>(i) * M_PI / (num - 1) ) * 0.5);
    }
    buildAirfoilCurve(cst,xData);

}
double airfoilDesign::curvatureRadius(const double x1,const double x2,const double x3, const double y1,const double y2, const double y3){

    /*
    1  -a  a*a       a1          x1
    1   0  0     *   a2   =      x2
    1   b  b*b       a3          x3





                                      */
    double a = distance(x1,x2,y1,y2);
    double b = distance(x2,x3,y2,y3);
    Eigen::MatrixXd m1;
    Eigen::MatrixXd m2;
    Eigen::VectorXd x;
    Eigen::VectorXd y;
    m1.resize(3,3);
    m2.resize(3,3);
    x.resize(3);
    y.resize(3);

    for(int i = 0; i < 3; i++){
        m1.coeffRef(i,0) = 1;

    }
    m1.coeffRef(0,1) = -a;
    m1.coeffRef(1,1) = 0;
    m1.coeffRef(2,1) = b;
    m1.coeffRef(0,2) = a * a;
    m1.coeffRef(1,2) = 0;
    m1.coeffRef(2,2) = b * b;

    m2 = m1;

    x.coeffRef(0) = x1;
    x.coeffRef(1) = x2;
    x.coeffRef(2) = x3;
    y.coeffRef(0) = y1;
    y.coeffRef(1) = y2;
    y.coeffRef(2) = y3;

    Eigen::VectorXd A = m1.lu().solve(x);
    Eigen::VectorXd B = m2.lu().solve(y);


    double a2 = A[1];
    double a3 = A[2];

    double b2 = B[1];
    double b3 = B[2];



    double r =  abs(pow((a2 * a2 + b2 * b2 ),1.5) /  (2 * (a3 * b2 - a2 * b3)));
    return r;

}
double airfoilDesign::distance(const double x1,const double x2,const double y1, const double y2){

    double s = sqrt(pow((x1 - x2),2) + pow((y1 - y2),2));

    return s;
}
double airfoilDesign::computeTrailingAngle(const double x1,const double x2,const double y1,const double y2){
    return atan((y1 - y2) / (x1 - x2));
}
int airfoilDesign::maxQVector(const QVector<double>& data){
    double maxValue = data[0];
    int maxIndex = 0;
    for(int i = 1;i<data.size();i++){
        if(data[i] > maxValue){
            maxValue = data[i];
            maxIndex = i;
        }
    }


    return maxIndex;

}
int airfoilDesign::minQVector(const QVector<double>& data){
    double minValue = data[0];
    int minIndex = 0;
    for(int i = 1;i<data.size();i++){
        if(data[i] < minValue){
            minValue = data[i];
            minIndex = i;
        }
    }


    return minIndex;

}

int airfoilDesign::nchoosek(const int order,const int index) {

    int result = 1;
    for(int i = 0;i < index;i++) {
        result = result * (order - i) / (i + 1);
    }

    return result;

}

void airfoilDesign::getIntersectionPosition(const QVector<double>& position){


    //计算两条直线交点
    double x1,x2,x3,x4,y1,y2,y3,y4;


    x1 = position[0],x2 =position[2],x3 = position[4],x4 = position[6];
    y1 = position[1],y2 =position[3],y3 = position[5],y4 = position[7];




    double m1 = (y2 - y1) / (x2 - x1);
    double b1 = y1 - m1 * x1;
    double m2 = (y4 - y3) / (x4 - x3);
    double b2 = y3 - m2 * x3;
    double x = (b2 - b1) / (m1 - m2);
    double y = m1 * x + b1;

    intersectionX = x;
    intersectionY = y;


}
void airfoilDesign::buildHicksHenne(const QVector<double>&c){
    int num = cstNum;

    int len1 = upperData.length();
    int len2 = lowerData.length();
    QVector<double>ek;
    QVector<QVector<double>>dY1;
    QVector<QVector<double>>dY2;



    double k;
    for(int i = 0; i < num - 2;i++){
        k = static_cast<double>(i + 1) / num;
        ek.append(log(0.5) / (log(k)));
    }
    QVector<double>tmp1;
    QVector<double>tmp2;
    for(int i = 0;i < len1;i++)
        tmp1.append(dy1(upperData[i][0],c[0]));
    for(int i = 0;i < len2;i++)
        tmp2.append(dy1(lowerData[i][0],c[num]));
    dY1.append(tmp1);
    dY2.append(tmp2);




    for(int i = 1;i < num - 1;i++){
        tmp1.clear();
        for(int j = 0;j<len1;j++){
            tmp1.append(dy2(upperData[j][0],ek[i - 1],c[i]));
        }
        dY1.append(tmp1);
    }
    for(int i = 1;i < num - 1;i++){
        tmp2.clear();
        for(int j = 0;j<len2;j++){
            tmp2.append(dy2(lowerData[j][0],ek[i - 1],c[num + i]));
        }
        dY2.append(tmp2);
    }
    tmp1.clear();
    tmp2.clear();




    for(int i = 0;i < len1;i++)
        tmp1.append(dy3(upperData[i][0],c[num - 1]));
    for(int i = 0;i < len2;i++)
        tmp2.append(dy3(lowerData[i][0],c[num * 2 - 1]));
    dY1.append(tmp1);
    dY2.append(tmp2);



    for(int i = 0;i < len1;i++){
        for(int j = 0; j < num;j++){
            upperData[i][1] = upperData[i][1] + dY1[j][i];
        }
    }

    for(int i = 0;i < len2;i++){
        for(int j = 0; j < num;j++){
            lowerData[i][1] = lowerData[i][1] - dY2[j][i];
        }
    }



    QVector<QVector<double>> airfoilSum;
    airfoilSum.append(upperData);
    airfoilSum.append(lowerData);
    newAirfoilData = airfoilSum;


}
double airfoilDesign::getUpperY(double x)const{
    // 1. 检查数据有效性
        if(upperData.isEmpty()) {

            return 0.0;
        }

        // 2. 判断数据顺序
        bool isDescending = (upperData.first()[0] > upperData.last()[0]);

        // 3. 边界检查（兼容升序/降序）
        if((isDescending && x >= upperData.first()[0]) ||
           (!isDescending && x <= upperData.first()[0])) {
            return upperData.first()[1];
        }
        if((isDescending && x <= upperData.last()[0]) ||
           (!isDescending && x >= upperData.last()[0])) {
            return upperData.last()[1];
        }

        // 4. 二分查找（兼容逆序）
        int low = 0;
        int high = upperData.size() - 1;
        while(low <= high) {
            int mid = low + (high - low)/2;
            if((isDescending && upperData[mid][0] > x) ||
               (!isDescending && upperData[mid][0] < x)) {
                low = mid + 1;
            } else {
                high = mid - 1;
            }
        }

        // 5. 安全插值
        const auto& p0 = upperData[low-1];
        const auto& p1 = upperData[low];

        double deltaX = p1[0] - p0[0];
        if(qFuzzyIsNull(deltaX)) { // 处理除零
            return (p0[1] + p1[1])/2.0;
        }

        double t = (x - p0[0])/deltaX;
        return p0[1] + t*(p1[1] - p0[1]);

}
double airfoilDesign::getLowerY(double x)const{
    // 1. 检查数据有效性
        if(lowerData.isEmpty()) {

            return 0.0;
        }

        // 2. 判断数据顺序
        bool isDescending = (lowerData.first()[0] > lowerData.last()[0]);

        // 3. 边界检查（兼容升序/降序）
        if((isDescending && x >= lowerData.first()[0]) ||
           (!isDescending && x <= lowerData.first()[0])) {
            return lowerData.first()[1];
        }
        if((isDescending && x <= lowerData.last()[0]) ||
           (!isDescending && x >= lowerData.last()[0])) {
            return lowerData.last()[1];
        }

        // 4. 二分查找（兼容逆序）
        int low = 0;
        int high = lowerData.size() - 1;
        while(low <= high) {
            int mid = low + (high - low)/2;
            if((isDescending && lowerData[mid][0] > x) ||
               (!isDescending && lowerData[mid][0] < x)) {
                low = mid + 1;
            } else {
                high = mid - 1;
            }
        }

        // 5. 安全插值
        const auto& p0 = lowerData[low-1];
        const auto& p1 = lowerData[low];

        double deltaX = p1[0] - p0[0];
        if(qFuzzyIsNull(deltaX)) { // 处理除零
            return (p0[1] + p1[1])/2.0;
        }

        double t = (x - p0[0])/deltaX;
        return p0[1] + t*(p1[1] - p0[1]);
}
void airfoilDesign::buildHicksHenne(const QVector<double>&c,const double location){
    int num = 3;

    int len1 = upperData.length();
    int len2 = lowerData.length();

    QVector<QVector<double>>dY1;
    QVector<QVector<double>>dY2;

    double ek = log(0.5) / (log(location));


    QVector<double>tmp1;
    QVector<double>tmp2;
    for(int i = 0;i < len1;i++)
        tmp1.append(dy1(upperData[i][0],c[0]));
    for(int i = 0;i < len2;i++)
        tmp2.append(dy1(lowerData[i][0],c[num]));
    dY1.append(tmp1);
    dY2.append(tmp2);


    tmp1.clear();
    for(int j = 0;j<len1;j++){
        tmp1.append(dy2(upperData[j][0],ek,c[1]));
    }
    dY1.append(tmp1);


    tmp2.clear();
    for(int j = 0;j<len2;j++){
        tmp2.append(dy2(lowerData[j][0],ek,c[num + 1]));
    }
    dY2.append(tmp2);

    tmp1.clear();
    tmp2.clear();

    for(int i = 0;i < len1;i++)
        tmp1.append(dy3(upperData[i][0],c[num - 1]));
    for(int i = 0;i < len2;i++)
        tmp2.append(dy3(lowerData[i][0],c[num * 2 - 1]));
    dY1.append(tmp1);
    dY2.append(tmp2);

    for(int i = 0;i < len1;i++){
        for(int j = 0; j < num;j++){
            upperData[i][1] = upperData[i][1] + dY1[j][i];
        }
    }

    for(int i = 0;i < len2;i++){
        for(int j = 0; j < num;j++){
            lowerData[i][1] = lowerData[i][1] - dY2[j][i];
        }
    }



    QVector<QVector<double>> airfoilSum;
    airfoilSum.append(upperData);
    airfoilSum.append(lowerData);
    newAirfoilData = airfoilSum;
}


double airfoilDesign::dy1(const double x,const double c){
    return pow(x,0.25) * (1 - x) * pow(Exp, - 20 * x) * c;
}
double airfoilDesign::dy2(const double x,const double ek,const double c){
    return pow(sin(M_PI *pow(x,ek)),3) * c;
}
double airfoilDesign::dy3(const double x,const double c){
    return 8 * x * (1 - x) * pow(Exp , -10 * (1 - x)) * c;
}
 /*************************************************StructDesign******************************************************/


