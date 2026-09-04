#include "propellerbemt.h"
#include <qDebug>
#if defined(_MSC_VER) && (_MSC_VER >= 1600)
# pragma execution_character_set("utf-8")
#endif
propellerBemt::propellerBemt()
{
    myAirfoilData = new myFile();

    //myAirfoilData->readInterData("CLARK Y");
    //airfoilResultArrayA = myAirfoilData->interDataArray;

    //test();
    //computeForce();


}
/*
void propellerBemt::readInterDrag(QString name){
    myAirfoilData->readInterData(name);
    airfoilResultArrayA = myAirfoilData->interDataArray;
    if(!airfoilResultArrayB.isEmpty())
        airfoilResultArrayB.clear();
    isMoreAirfoil = false;
}
void propellerBemt::readInterDrag(QString name1,QString name2,double value){
    myAirfoilData->readInterData(name1);
    airfoilResultArrayA = myAirfoilData->interDataArray;
    myAirfoilData->readInterData(name2);
    airfoilResultArrayB = myAirfoilData->interDataArray;
    isMoreAirfoil = true;
    airfoilRatio = value;

}
*/
void propellerBemt::readInterDrag(QStringList list){
    if(!airfoilResultArray.isEmpty())
        airfoilResultArray.clear();


    QString txt = list.at(0);

    txt.remove(' ');
    if(txt == "airfoil"){
        if(list.length() >= 3){
            myAirfoilData->readInterData(list.at(1));
            airfoilResultArrayA = myAirfoilData->interData;
            myAirfoilData->readInterData(list.at(2));
            airfoilResultArrayB = myAirfoilData->interData;

            if(list.length() >= 4){
                QString valueTxtx = list.at(3);
                airfoilRatio = valueTxtx.toDouble();
            }
            isMoreAirfoil = true;

        }else{

            myAirfoilData->readInterData(list.at(1));

            airfoilResultArrayA = myAirfoilData->interData;
            isMoreAirfoil = false;

        }
        isModel = false;

    }else{
        for(int i = 0;i<list.length();i++){
            myAirfoilData->readInterData(list.at(i));
            airfoilResultArray.append(myAirfoilData->interData);
        }
        isModel = true;
        isMoreAirfoil = true;
    }
}
void propellerBemt::test(){
    rpmArray.append(2100);
    omegaArray.append(2100 / 30 * M_PI);
    for(int i = 0;i<60;i++)
        vinfArray.append(i + 1);
    for(int i = 0;i<11;i++)
        chordArray.append(0.12);
    for(int i = 0;i<11;i++)
        spanArray.append(0.08 + 0.72 / 10 * double(i));
    for(int i = 0;i<11;i++)
        twistArray.append(atan(1 / (2 * M_PI * spanArray[i])));


}
void propellerBemt::solver(){
    computeForce();
}
void propellerBemt::initialAnalyse(const wingDefinition&propData,const propVLMSetting&b){

    if(!spanArray.isEmpty()){
        spanArray.clear();
        chordArray.clear();
        twistArray.clear();
        omegaArray.clear();
        radiusArray.clear();
        spanW.clear();
        resultArray.clear();
        dTArray.clear();
        dQArray.clear();
    }

    rpmArray = b.rpmArray;
    vinfArray = b.vinfArray;
    height = b.height;
    propNum = propData.num;

    R = propData.spanW.last();
    for(int i = 0;i<propData.spanW.length() - 1;i++)
        spanArray.append(abs(propData.spanW[i] + propData.spanW[i + 1]) / 2);
    for(int i = 0;i<propData.spanW.length() - 1;i++)
        radiusArray.append(abs(propData.spanW[i] + propData.spanW[i + 1]) / 2);


    for(int i = 0;i<spanArray.length() - 1;i++){
        spanW.append(spanArray[i + 1] - spanArray[i]);
    }
    for(int i = 0;i<propData.chordLengthW.length() - 1;i++)
        chordArray.append((propData.chordLengthW[i] + propData.chordLengthW[i + 1]) / 2);

    //for(int i = 0;i<propData.chordLengthW.length() - 1;i++)
        //chordArray.append(propData.chordLengthW[i + 1]);


    for(int i = 0;i<propData.twistAngleW.length() - 1;i++)
        twistArray.append((propData.twistAngleW[i] + propData.twistAngleW[i + 1]) / 360.0 * M_PI);

    //for(int i = 0;i<propData.twistAngleW.length() - 1;i++)
        //twistArray.append (propData.twistAngleW[i + 1] / 360.0 * M_PI);
    for(int i = 0;i<rpmArray.length();i++)
        omegaArray.append(rpmArray[i] / 30.0 * M_PI);

    density = mathSolver.calculateAirDensity(height);
    temperature = mathSolver.calculateTemperature(height);
    viscosity = mathSolver.calculateAirViscosity(temperature);




}
void propellerBemt::computeForce(){



    double dia = R * 2;
    int row = vinfArray.length();
    int columns = rpmArray.length();
    QVector<double>tmp1;
    QVector<double>tmp2;
    QVector<double>tmp3;
    QVector<double>tmp4;
    QVector<double>tmp5;
    QVector<double>tmp6;
    QVector<double>tmp7;
    double p;
    for(int i = 0;i<row;i++){
        for(int j = 0;j<columns;j++){
            QVector<double>dT;
            QVector<double>dQ;
            double thrust = 0.0;
            double torque = 0.0;
            for(int k = 0;k<spanW.length();k++){
                a = 0.1;
                b = 0.01;
                double tmp1 = 0;
                double tmp2 = 0;
                double v = 0.0;
                if(isModel){
                    airfoilResultArrayA = airfoilResultArray[k];
                    airfoilResultArrayB = airfoilResultArray[k + 1];
                }


                iterCompute(i,j,k,&tmp1,&tmp2,v);

                thrust += tmp1 * spanW[k];
                torque += tmp2 * spanW[k];

                dT.append(tmp1 * spanW[k] /  propNum);
                dQ.append( tmp2 * spanW[k] /  propNum / radiusArray[k]);
                //thrust += tmp1 * 0.072;
                //torque += tmp2 * 0.072;

            }
            dTArray.append(dT);
            dQArray.append(dQ);
            if(thrust > 0){

                double tmp = density * rpmArray[j] / 3600 * rpmArray[j] * pow(dia,4);
                tmp1.append(thrust);
                tmp2.append(torque);
                p = torque * omegaArray[j];
                tmp3.append(p);
                tmp4.append(thrust * vinfArray[i] / p);
                tmp5.append(vinfArray[i] / R / rpmArray[j] * 30);
                tmp6.append(thrust / tmp);
                tmp7.append(torque / tmp / dia);
            }else{
                tmp1.append(0);
                tmp2.append(0);
                tmp3.append(0);
                tmp4.append(0);
                tmp5.append(vinfArray[i] / R / rpmArray[j] * 30);
                tmp6.append(0);
                tmp7.append(0);
            }





        }
    }

    resultArray.append(tmp3);
    resultArray.append(tmp4);
    resultArray.append(tmp1);
    resultArray.append(tmp2);
    resultArray.append(tmp5);
    resultArray.append(tmp6);
    resultArray.append(tmp7);



}

void propellerBemt::iterCompute(int index1,int index2,int index3,double *tForce,double*mForce,double&v){
    bool isOk = false;
    int step = 0;
    double error = 0.00001;
    double v0,v2,phi,alpha,cl,cd,p;
    int re;
    double dtDr,dqDr;
    double tmp1,tmp2;
    double tmpa,tmpb;
    double ratio;
    //double val = airfoilResultArrayA[1].Re - airfoilResultArrayA[0].Re;
    double vTip;

    while(!isOk){
        v0 = vinfArray[index1] * (1 + a);
        v2 = omegaArray[index2] * radiusArray[index3] * (1 - b);
        v = sqrt(v0 * v0 + v2 * v2);
        phi = atan2(v0,v2);
        alpha = twistArray[index3] - phi;

        //Ma = v / mathSolver.getSoundSpeed(temperature);
        re = int(chordArray[index3] * v / viscosity);





        //获取升阻力系数
        int reIndex = getReIndex(re);

        bool ok1 = (alpha * 180 / M_PI >=mathSolver.minV(airfoilResultArrayA[reIndex].alphaData));
        bool ok2 = (alpha * 180 / M_PI <=mathSolver.maxV(airfoilResultArrayA[reIndex].alphaData));
        if(ok1 && ok2){
            if(!isMoreAirfoil){
                cl = mathSolver.interpolateWithLinear(airfoilResultArrayA[reIndex].clData,airfoilResultArrayA[reIndex].alphaData,alpha * 180.0 / M_PI);
                cd = mathSolver.interpolateWithLinear(airfoilResultArrayA[reIndex].cdData,airfoilResultArrayA[reIndex].alphaData,alpha * 180.0 / M_PI);
            }else{
                ratio = sin(M_PI / 2 * double(index3) / spanW.length());

                if(ratio <= airfoilRatio){
                    double ratios = ratio / airfoilRatio;
                    double cl1 = mathSolver.interpolateWithLinear(airfoilResultArrayA[reIndex].clData,airfoilResultArrayA[reIndex].alphaData,alpha * 180.0 / M_PI);
                    double cl2 = mathSolver.interpolateWithLinear(airfoilResultArrayB[reIndex].clData,airfoilResultArrayB[reIndex].alphaData,alpha * 180.0 / M_PI);
                    double cd1 = mathSolver.interpolateWithLinear(airfoilResultArrayA[reIndex].cdData,airfoilResultArrayA[reIndex].alphaData,alpha * 180.0 / M_PI);
                    double cd2 = mathSolver.interpolateWithLinear(airfoilResultArrayB[reIndex].cdData,airfoilResultArrayB[reIndex].alphaData,alpha * 180.0 / M_PI);
                    if(isModel){
                        cl = cl1 * 0.5 + cl2 * 0.5;
                        cd = cd1 * 0.5 + cd2 * 0.5;
                    }else{
                        cl = cl1 * (1 - ratios) + cl2 * ratios;
                        cd = cd1 * (1 - ratios) + cd2 * ratios;
                    }
                }else{
                    cl = mathSolver.interpolateWithLinear(airfoilResultArrayB[reIndex].clData,airfoilResultArrayB[reIndex].alphaData,alpha * 180.0 / M_PI);
                    cd = mathSolver.interpolateWithLinear(airfoilResultArrayB[reIndex].cdData,airfoilResultArrayB[reIndex].alphaData,alpha * 180.0 / M_PI);
                }

            }


        }else{
            if(!isMoreAirfoil){
                cl = getAeroA(reIndex,re,alpha).Cl;
                cd = getAeroA(reIndex,re,alpha).Cd;

            }else{
                ratio = sin(M_PI / 2 * double(index3) / spanW.length());
                if(ratio <= airfoilRatio){
                    double ratios = ratio / airfoilRatio;
                    double cl1 = getAeroA(reIndex,re,alpha).Cl;
                    double cl2 = getAeroB(reIndex,re,alpha).Cl;
                    double cd1 = getAeroA(reIndex,re,alpha).Cd;
                    double cd2 = getAeroB(reIndex,re,alpha).Cd;
                    if(isModel){
                        cl = cl1 * 0.5 + cl2 * 0.5;
                        cd = cd1 * 0.5 + cd2 * 0.5;
                    }else{
                        cl = cl1 * (1 - ratios) + cl2 * ratios;
                        cd = cd1 * (1 - ratios) + cd2 * ratios;
                    }

                }else{
                   cl = getAeroB(reIndex,re,alpha).Cl;
                   cd = getAeroB(reIndex,re,alpha).Cd;
                }
            }

        }

        if(!isVTip)
            vTip = 1;
        else
            vTip = 2 / M_PI * acos(exp(-(propNum / 2) * (1 - radiusArray[index3] / R) / (spanArray[index3] / R) / sin(phi)));
        p = 0.5 * density * v * v * chordArray[index3] * propNum;
        dtDr = p * (cos(phi) * cl - sin(phi) * cd) * vTip;
        dqDr = p * radiusArray[index3] * (cd * cos(phi) + cl * sin(phi)) * vTip;
        tmp1 = dtDr / (4 * M_PI * radiusArray[index3] * density * vinfArray[index1] * vinfArray[index1] * (1 + a));
        tmp2 = dqDr / (4 * M_PI * pow(radiusArray[index3],3) * density * vinfArray[index1] * (1 + a) * omegaArray[index2]);
        tmpa = 0.5 * (a + tmp1);
        tmpb = 0.5 * (b + tmp2);

        if(abs(tmpa - a)<error && abs(tmpb - b) < error){
            isOk = true;

        }

        a = tmpa;
        b = tmpb;
        step++;

        if(step > iterStep){
            isOk = true;
        }
    }

    *tForce = dtDr;
    *mForce = dqDr;
}
AeroCoeff propellerBemt::getAeroA(int reIndex,double re,double alpha){
    double val = airfoilResultArrayA[1].Re - airfoilResultArrayA[0].Re;

    int maxIndex1 = mathSolver.getMaxIndex(airfoilResultArrayA[reIndex].clData);
    double stallAlpha1 = airfoilResultArrayA[reIndex].alphaData[maxIndex1];

    double stallCl1 = airfoilResultArrayA[reIndex].clData[maxIndex1] * clRatio;
    double stallCd1 = airfoilResultArrayA[reIndex].cdData[maxIndex1];

    double maxCd1 = mathSolver.maxV(airfoilResultArrayA[reIndex].cdData);

    int maxIndex2 = mathSolver.getMaxIndex(airfoilResultArrayA[reIndex + 1].clData);
    double stallAlpha2 = airfoilResultArrayA[reIndex + 1].alphaData[maxIndex2];

    double stallCd2 = airfoilResultArrayA[reIndex + 1].cdData[maxIndex2];
    double stallCl2 = airfoilResultArrayA[reIndex + 1].clData[maxIndex2] * clRatio;

    double maxCd2 = mathSolver.maxV(airfoilResultArrayA[reIndex + 1].cdData);

    double ratio = (re - airfoilResultArrayA[reIndex].Re) / val;

    double stallAlpha = ((1 - ratio) * stallAlpha1 + ratio * stallAlpha2) * M_PI / 180.0;

    double stallCd = (1 - ratio) * stallCd1 + ratio * stallCd2;
    double stallCl = (1 - ratio) * stallCl1 + ratio * stallCl2;

    double maxCd = (1 - ratio) * maxCd1 + ratio * maxCd2;

    AeroCoeff aero  = viternaModel(alpha,stallAlpha,stallCl,stallCd, maxCd);
    return aero;
    //return getMaxCl(alpha,stallAlpha,maxCd,stallCl);

}

AeroCoeff propellerBemt::getAeroB(int reIndex,double re,double alpha){
    double val = airfoilResultArrayB[1].Re - airfoilResultArrayB[0].Re;
    int maxIndex1 = mathSolver.getMaxIndex(airfoilResultArrayB[reIndex].clData);
    double stallAlpha1 = airfoilResultArrayB[reIndex].alphaData[maxIndex1];
    double stallCl1 = airfoilResultArrayB[reIndex].clData[maxIndex1] * clRatio;
    double stallCd1 = airfoilResultArrayB[reIndex].cdData[maxIndex1];

    double maxCd1 = mathSolver.maxV(airfoilResultArrayB[reIndex].cdData);

    int maxIndex2 = mathSolver.getMaxIndex(airfoilResultArrayB[reIndex + 1].clData);
    double stallAlpha2 = airfoilResultArrayB[reIndex + 1].alphaData[maxIndex2];
    double stallCd2 = airfoilResultArrayB[reIndex + 1].cdData[maxIndex2];
    double stallCl2 = airfoilResultArrayB[reIndex + 1].clData[maxIndex2] * clRatio;

    double maxCd2 = mathSolver.maxV(airfoilResultArrayB[reIndex + 1].cdData);

    double ratio = (re - airfoilResultArrayB[reIndex].Re) / val;

    double stallAlpha = ((1 - ratio) * stallAlpha1 + ratio * stallAlpha2) * M_PI / 180.0;
    double stallCd = (1 - ratio) * stallCd1 + ratio * stallCd2;
    double stallCl = (1 - ratio) * stallCl1 + ratio * stallCl2;

    double maxCd = (1 - ratio) * maxCd1 + ratio * maxCd2;

    //return getMaxCl(alpha,stallAlpha,maxCd,stallCl);

    AeroCoeff aero  = viternaModel(alpha,stallAlpha,stallCl,stallCd, maxCd);
    return aero;

}

int propellerBemt::getReIndex(int value){
    int len = airfoilResultArrayA.length();
    if (len == 0) return -1; // 处理空数组的情况

    // 边界条件
    if (value <= airfoilResultArrayA[0].Re) {
        return 0; // 返回第一个元素的索引
    }
    if (value >= airfoilResultArrayA[len - 2].Re) {
        return len - 2; // 返回最后第二个元素的索引
    }

    // 使用二分查找
    int low = 0;
    int high = len - 1;

    while (low < high) {
        int mid = (low + high) / 2;
        if (airfoilResultArrayA[mid].Re < value) {
            low = mid + 1;
        } else {
            high = mid;
        }
    }

    // low 现在指向第一个大于或等于 value 的索引
    return low;
}

void propellerBemt::setVTip(bool ok){
    isVTip = ok;
}
AeroCoeff propellerBemt::viternaModel(
    double alpha,
    double stallAlpha,
    double stallCl,
    double stallCd,
    double CdMax,
    const ViternaParam& p)
{
    const double eps = 1e-6;
    alpha = std::clamp(alpha, -M_PI + eps, M_PI - eps);

    double sa = sin(alpha);
    double ca = cos(alpha);
    if (fabs(sa) < eps) sa = (sa > 0 ? eps : -eps);

    /* ===============================
       1. 深失速阻力上限（关键）
       =============================== */
    // ★ 不信 XFOIL 的 maxCd，强制平板级别
    double CdMaxEff = std::max(p.k_cd * CdMax, 1.8);

    /* ===============================
       2. Viterna 连续性参数
       =============================== */
    double A = (stallCl - CdMaxEff * sin(stallAlpha) * cos(stallAlpha))
             * sin(stallAlpha)
             / (cos(stallAlpha) * cos(stallAlpha));

    double B = (stallCd - CdMaxEff * sin(stallAlpha) * sin(stallAlpha))
             / cos(stallAlpha);

    /* ===============================
       3. 失速后模型（重点修改）
       =============================== */

    // ★ 平板升力项：有限展长 + 旋转 → 衰减
    const double k_fp = 0.6;   // 推荐 0.55–0.65
    double Cl_fp = k_fp * 0.5 * CdMaxEff * sin(2.0 * alpha);

    // ★ A 项只在 stall 附近起作用，防止中段 Cl 被抬高
    const double alpha_fp = 25.0 * M_PI / 180.0;
    double decay = exp(-pow(alpha / alpha_fp, 2.0));

    double Cl_A = p.k_cl2 * decay * A * (ca * ca / sa);

    double Cl_post = Cl_fp + Cl_A;

    // ★ 阻力：平板主导 + 连续修正
    double Cd_post = CdMaxEff * sa * sa + B * ca;

    /* ===============================
       4. 失速前模型（保守线性）
       =============================== */
    double Cl_pre = p.k_slope * (stallCl / stallAlpha) * alpha;
    double Cd_pre = stallCd * pow(alpha / stallAlpha, 2.0);

    /* ===============================
       5. 平滑切换
       =============================== */
    double w = 0.5 * (1.0 + tanh((fabs(alpha) - stallAlpha) / p.delta_stall));

    AeroCoeff out;
    out.Cl = (1.0 - w) * Cl_pre + w * Cl_post;
    out.Cd = (1.0 - w) * Cd_pre + w * Cd_post;

    /* ===============================
       6. 最终物理下限（非常重要）
       =============================== */
    // ★ 深失速 Cd 不得过小
    double Cd_min = 1.8 * sa * sa;
    out.Cd = std::max(out.Cd, Cd_min);
    out.Cl = std::min(out.Cl,2.0);

    return out;
}

