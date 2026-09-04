#include "airfoilsolve.h"
#include<QDebug>
#include <cmath>


airfoilSolve::airfoilSolve() {

    input.Re = 100000;
    input.Ma = 0.0;
    input.minAlpha = 0;
    input.maxAlpha = 10;
    input.nCrit = 9;
    input.alphaStepSize = 1;
    input.xtrTop = 1;
    input.xtrBot = 1;
    input.ReType = 1;
    input.MaType = 1;
    input.model = 0;
}

airfoilSolve::airfoilSolve(const xfoilSetting&value) {

    input = value;
}





void airfoilSolve::refreshParaments(const xfoilSetting& valueSetting){

    input = valueSetting;


}

void airfoilSolve::refreshSimpleParaments(const xfoilSetting& valueSetting,const int ID){

    input = valueSetting;
}



bool airfoilSolve::importAirfoil(const QVector<QVector<double>>& airfoil){

    airfoilData = airfoil;
    if(!airfoilData.isEmpty())
        return true;

    return false;
}


QVector<QVector<QVector<double>>> airfoilSolve::getResult(){

        solver();
        return allData;

}
void  airfoilSolve::computeInterDrag(){

    double x[600],y[600],nx[600],ny[600];
    int n = airfoilData.size();
    QVector<double>tmp;



    for(int i = 0; i < n;i++){
        x[i] = airfoilData[i][0];
        y[i] = airfoilData[i][1];

    }
    XFoil *foil = new XFoil();


    if (!foil->initXFoilGeometry(n, x, y, nx, ny)) {

        qDebug()<<"Initialization error!";
        return;
    }

    if (!foil->initXFoilAnalysis(input.Re, 0, input.Ma, input.nCrit, input.xtrTop, input.xtrBot, input.ReType, input.MaType, true, ss)) {
        qDebug()<<"Initialization error!";

        return ;
    }

    int step = clArray.length();

    for (int i = 0; i < step; i++) {

        m_Iterations = 0;
        foil->setBLInitialized(false);
        foil->lipan = false;


        foil->setClSpec(clArray[i]);
        foil->lalfa = false;

        foil->setQInf(1.0);


        if (!foil->specal()) {

            return;
        }
        foil->lwake = false;
        foil->lvconv = false;


        while (!iterate(foil))
            ;
        if (foil->lvconv) {
            tmp.append(foil->cd);
        } else {
            tmp.append(0.008);
        }
    }
    cdArray = tmp;

    delete foil;



}

fixClResult airfoilSolve::simpleResult(){
    solverOnce();

    return onceData;
}

void airfoilSolve::emitResult(){
    solverOnce();
}
int airfoilSolve::solver(){
    double x[600],y[600],nx[600],ny[600];
    int n = airfoilData.size();




    for(int i = 0; i < n;i++){
        x[i] = airfoilData[i][0];
        y[i] = airfoilData[i][1];

    }
    XFoil *foil = new XFoil();


    if (!foil->initXFoilGeometry(n, x, y, nx, ny)) {

        qDebug()<<"Initialization error!";
        return 1;
    }

    if (!foil->initXFoilAnalysis(input.Re, 0, input.Ma, input.nCrit, input.xtrTop, input.xtrBot, input.ReType, input.MaType, true, ss)) {
         qDebug()<<"Initialization error!";

        return 1;
    }

    int step = static_cast<int>((input.maxAlpha - input.minAlpha) / input.alphaStepSize);

    double alpha;

    progressSignal = 0;
    QVector<QVector<double>>data;
    QVector<QVector<double>>cpxData;
    QVector<QVector<QVector<double>>> allresult;
    QVector<QVector<double>>xblUpper;
    QVector<QVector<double>>xblLower;
    QVector<QVector<double>>xTmpWake;
    QVector<QVector<double>>yTmpWake;



    double tmp1,tmp2;




    for (int i = 0; i < step + 1; i++) {

        alpha = input.minAlpha + static_cast<double>(i) * input.alphaStepSize;
        m_Iterations = 0;
        foil->setBLInitialized(false);
        foil->lipan = false;

        if(!input.model){
            foil->setAlpha(alpha * 3.1415926 / 180);
            foil->lalfa = true;
        }else{

            foil->setClSpec(alpha);
            foil->lalfa = false;
        }



        foil->setQInf(1.0);


        if (!foil->specal()) {

            return 1;
        }
        foil->lwake = false;
        foil->lvconv = false;


        while (!iterate(foil))
            ;
        QVector<double>row;
        QVector<double>cpY;
        QVector<double>upperTmp;
        QVector<double>lowerTmp;
        QVector<double>wakeX;
        QVector<double>wakeY;

        progressSignal = (static_cast<double>(i) + 1) / (step + 1) * 100;
        emit progressUpdat(progressSignal);
        if (foil->lvconv) {
            //qDebug() << "  converged after " << m_Iterations << " iterations";
            if(foil->cd != 0 ){
                tmp1 = foil->cl / foil->cd;
                tmp2 = std::pow(std::abs(foil->cl),1.5) / foil->cd * std::abs(foil->cl) / foil->cl ;
            }else{
                tmp1 = 0;
                tmp2 = 0;

            }
            for(int i = 0;i<airfoilData.size();i++){

                cpY.append(foil->cpi[i]);

            }
            for(int i = 0;i<foil->nbl[1];i++){
                upperTmp.append(foil->dstr[i][1]);
            }
            for(int i = 0;i<foil->nbl[2];i++){
                lowerTmp.append(foil->dstr[i][2]);
            }

            int len = foil->nbl[1] + foil->nbl[2] - airfoilData.size() - 2;

            for(int i = 0;i<len;i++){
                wakeX.append(foil->x[i + airfoilData.size() + 1]);
                wakeY.append(foil->y[i + airfoilData.size() + 1]);
            }


            row.append(foil->alfa / 3.1415926 * 180);
            row.append(foil->cl);
            row.append(foil->cd);
            row.append(tmp1);
            row.append(tmp2);
            row.append(foil->cm);
            row.append(foil->xcp);
            row.append(foil->xoctr[1]);
            row.append(foil->xoctr[2]);







            data.append(row);
            cpxData.append(cpY);
            xblUpper.append(upperTmp);
            xblLower.append(lowerTmp);
            xTmpWake.append(wakeX);
            yTmpWake.append(wakeY);

        } else {

        }

    }





    allresult.append(data);
    allresult.append(cpxData);
    resultData = data;
    cpxResultData = cpxData;
    allData = allresult;
    xblDataUpper = xblUpper;
    xblDataLower = xblLower;
    xWakeArray = xTmpWake;
    yWakeArray = yTmpWake;
    delete foil;
    foil = nullptr;

    return 0;
}

int airfoilSolve::solverOnce(){

    double x[600],y[600],nx[600],ny[600];

    int n = airfoilData.size();
    for(int i = 0; i < n;i++){
        x[i] = airfoilData[i][0];
        y[i] = airfoilData[i][1];

    }
    XFoil *foil = new XFoil();
    if (!foil->initXFoilGeometry(n, x, y, nx, ny)) {

        qDebug()<<"Initialization error!";
        return 1;
    }

    if (!foil->initXFoilAnalysis(input.Re, 0, input.Ma, input.nCrit, input.xtrTop, input.xtrBot, input.ReType, input.MaType, true, ss)) {
        qDebug()<<"Initialization error!";

        return 1;
    }

        double tmp1;
        QVector<double>cpY;
        m_Iterations = 0;
        foil->setBLInitialized(false);
        foil->lipan = false;
        if(input.model){
            foil->setClSpec(input.designCL);
            foil->lalfa = false;
        }
        else{
            foil->setAlpha(input.designALPHA * 3.14159 / 180);
            foil->lalfa = true;
        }


        foil->setQInf(1.0);


        if (!foil->specal()) {

            return 1;
        }
        foil->lwake = false;
        foil->lvconv = false;



        while (!iterate(foil))
            ;


        if (foil->lvconv) {
            //qDebug() << "  converged after " << m_Iterations << " iterations";
            if(foil->cd != 0 ){
                tmp1 = foil->cl / foil->cd;

            }else{
                tmp1 = 0;
            }

            for(int i = 0;i<airfoilData.size();i++){

                cpY.append(foil->cpi[i]);


            }



            onceData.K = tmp1;
            onceData.cD = foil->cd;
            onceData.cM = foil->cm;
            onceData.cL = foil->cl;
            onceData.cpx = cpY;


        } else {

            onceData.cL = 0;
            onceData.cD = 1;
            onceData.K = 0;


        }


        delete foil;

    return 0;

}

void airfoilSolve::solverInThread() {
    solver();
}

bool airfoilSolve::iterate(XFoil *foil) {

    if (!foil->viscal()) {
        foil->lvconv = false;

        qDebug()<< "CpCalc: local speed too large\nCompressibility corrections invalid";
        //局部速度值过大计算失败
        return false;
    }

    while (m_Iterations < input.s_IterLim && !foil->lvconv /*&& !s_bCancel*/) {
        if (foil->ViscousIter()) {
            m_Iterations++;
        } else
            m_Iterations = input.s_IterLim;
    }

    if (!foil->ViscalEnd()) {
        foil->lvconv = false;  // point is unconverged

        foil->setBLInitialized(false);
        foil->lipan = false;
        m_bErrors = true;
        return true;  // to exit loop
    }

    if (m_Iterations >= input.s_IterLim && !foil->lvconv) {
        if (s_bAutoInitBL) {
            foil->setBLInitialized(false);
            foil->lipan = false;
        }
        foil->fcpmin();  // Is it of any use ?
        return true;
    }
    if (!foil->lvconv) {
        m_bErrors = true;
        foil->fcpmin();  // Is it of any use ?
        return false;
    } else {
        // converged at last
        foil->fcpmin();  // Is it of any use ?
        return true;
    }
    return false;

}

airfoilSolve::~airfoilSolve(){


}
