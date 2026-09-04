#include "wingoptimization.h"
#include <QRandomGenerator>

wingOptimization::wingOptimization(const WingGaParameters& list,const wingDefinition& wingData) {
    valueList = list;
    initialWingData = wingData;
    numN = list.maxChord.length();

}
wingOptimization::wingOptimization() {


}
void wingOptimization::setModel(const WingGaParameters& list,const wingDefinition& wingData){
    valueList = list;
    initialWingData = wingData;
    numN = list.maxChord.length();

}
void wingOptimization::initialElite(){

    restArray();
    double max, min;
    //int symbol;
    QVector<int>newSymbolTmp;

    int cstVal = int(1.0 / valueList.val);
    double minRatio = valueList.MinSpan / initialWingData.Span();
    double maxRatio = valueList.MaxSpan / initialWingData.Span();



    for(int i = 0;i<numN;i++){
        QVector<double>maxTmp;
        QVector<double>minTmp;
        QVector<double>cstArrayTmp = initialWingData.cstArray[i];
        for(int j = 0;j<valueList.solutionsNum;j++){
            max = cstArrayTmp[j] * (1 + valueList.cstRadio);
            min = cstArrayTmp[j] * (1 - valueList.cstRadio);
            maxTmp.append(max);
            minTmp.append(min);

            // if(cstArrayTmp[j] != 0)
            //     symbol = cstArrayTmp[j] / abs(cstArrayTmp[j]);
            // else
            //     symbol = 1;
            newSymbolTmp.append(getSign(cstArrayTmp[j]));

        }
        symbolCSTArray.append(newSymbolTmp);
        maxCstArrayDec.append(maxTmp);
        minCstArrayDec.append(minTmp);
    }


    for(int i = 0;i<numN - 1;i++){
        double spanTmp = (initialWingData.spanW[i + 1] - initialWingData.spanW[i]);
        allSpan.append(spanTmp);
        valueList.maxSpan.append(spanTmp * maxRatio);
        valueList.minSpan.append(spanTmp * minRatio);
    }
    initialWingData.spanLengthW = allSpan;







    int cstMax, cstMin;
    double cst,span,chord,offsetLength,twisAngle,dihedralAngle;
    for(int i = 0;i<valueList.initialEliteNum;i++){
        wingDefinition defineTmp;

        defineTmp.gridU = initialWingData.gridU;
        defineTmp.gridV = initialWingData.gridV;
        defineTmp.vMeshType = initialWingData.vMeshType;
        defineTmp.uMeshType = initialWingData.uMeshType;
        defineTmp.realChord = initialWingData.realChord;
        defineTmp.airfoilInputType = false;
        QVector<int>symbol1;
        QVector<int>symbol2;
        QVector<int>symbol3;

        //生成cst随机变量
        for(int j = 0;j<numN;j++){
            if(valueList.airfoilChangeArray[j]){
                QVector<double>cstTmp;
                for(int k = 0;k<valueList.solutionsNum;k++){
                    cstMax = abs(static_cast<int>(maxCstArrayDec[j][k] * cstVal));
                    cstMin = abs(static_cast<int>(minCstArrayDec[j][k] * cstVal));
                //cst = static_cast<double>((QRandomGenerator::global()->generateDouble()*(cstMax - cstMin) + cstMin) * valueList.val) * symbolArray[j][k];
                    cst = double(generateRandomInt(cstMin,cstMax) * valueList.val * symbolCSTArray[j][k]);
                    cstTmp.append(cst);
                }
                defineTmp.cstArray.append(cstTmp);
            }else{
                defineTmp.cstArray.append(initialWingData.cstArray[j]);
            }




           // span = generateRandomInt(int(initialWingData.spanW[j] * minRatio / valueList.spanVal),int(initialWingData.spanW[j] * maxRatio / valueList.spanVal)) * valueList.spanVal;
            if(valueList.chordChangeArray[j]){
                chord = generateRandomInt(int(valueList.minChord[j] / valueList.chordVal),int(valueList.maxChord[j] / valueList.chordVal)) * valueList.chordVal;
                defineTmp.chordLengthW.append(chord);
            }else{
                defineTmp.chordLengthW.append(initialWingData.chordLengthW[j]);
            }

            if(valueList.offsetChangeArray[j]){
                offsetLength= generateRandomInt(int(valueList.minOffsetLength[j] / valueList.offsetLengthVal),int(valueList.maxOffsetLength[j] / valueList.offsetLengthVal)) * valueList.offsetLengthVal;
                defineTmp.offsetLengthW.append(offsetLength);
                // if(offsetLength != 0)
                //     symbol1.append(offsetLength / abs(offsetLength));
                // else
                //     symbol1.append(1);
                symbol1.append(getSign(offsetLength));
            }else{
                defineTmp.offsetLengthW.append(initialWingData.offsetLengthW[j]);
                // if(initialWingData.offsetLengthW[j] != 0)
                //     symbol1.append(initialWingData.offsetLengthW[j] / abs(initialWingData.offsetLengthW[j]));
                // else
                //     symbol1.append(1);
                symbol1.append(getSign(initialWingData.offsetLengthW[j]));

            }


            if(valueList.twistChangeArray[j]){
                twisAngle = generateRandomInt(int(valueList.minTwisAngle[j] / valueList.twisAngleVal),int(valueList.maxTwisAngle[j] / valueList.twisAngleVal)) * valueList.twisAngleVal;
                defineTmp.twistAngleW.append(twisAngle);
                // if(twisAngle != 0)
                //     symbol2.append(twisAngle / abs(twisAngle));
                // else
                //     symbol2.append(1);
                symbol2.append(getSign(twisAngle));
            }else{
                defineTmp.twistAngleW.append(initialWingData.twistAngleW[j]);
                // if(initialWingData.twistAngleW[j] != 0)
                //     symbol2.append(initialWingData.twistAngleW[j] / abs(initialWingData.twistAngleW[j]));
                // else
                //     symbol2.append(1);
                symbol2.append(getSign(initialWingData.twistAngleW[j]));
            }


            if(valueList.dihedChangeArray[j]){
                dihedralAngle = generateRandomInt(int(valueList.minDihedralAngle[j] / valueList.dihedralAngleVal),int(valueList.maxDihedralAngle[j] / valueList.dihedralAngleVal)) * valueList.dihedralAngleVal;
                defineTmp.dihedralAngleW.append(dihedralAngle);
                // if(dihedralAngle != 0)
                //     symbol3.append(dihedralAngle / abs(dihedralAngle));
                // else
                //     symbol3.append(1);
                symbol3.append(getSign(dihedralAngle));
            }else{
                defineTmp.dihedralAngleW.append(initialWingData.dihedralAngleW[j]);
                // if(initialWingData.dihedralAngleW[j] != 0)
                //     symbol3.append(initialWingData.dihedralAngleW[j] / abs(initialWingData.dihedralAngleW[j]));
                // else
                //     symbol3.append(1);
                symbol3.append(getSign(initialWingData.dihedralAngleW[j]));
            }


        }


        symbolOffsetArray.append(symbol1);

        symbolTwistArray.append(symbol2);

        symbolDihedArray.append(symbol3);
        //机翼只需n-1参数

        for(int k = 0;k<numN - 1;k++){
            if(valueList.spanChangeArray[k]){
                span = generateRandomInt(int(allSpan[k] * minRatio / valueList.spanVal),int(allSpan[k] * maxRatio / valueList.spanVal)) * valueList.spanVal;
                defineTmp.spanLengthW.append(span);

            }else{
                defineTmp.spanLengthW.append(initialWingData.spanLengthW[k]);
                //defineTmp.spanW.append(initialWingData.spanW[k]);
            }
        }

        QVector<double>spanTmp;
        spanTmp.append(0);
        spanTmp.append(defineTmp.spanLengthW[0]);
        for(int j = 0;j<numN - 2;j++)
            spanTmp.append(defineTmp.spanLengthW[j] + defineTmp.spanLengthW[j + 1]);
        defineTmp.spanW = spanTmp;


        // if(valueList.spanChangeArray[j]){
        //     for(int k = 0;k<numN - 1;k++){
        //         span = generateRandomInt(int(allSpan[k] * minRatio / valueList.spanVal),int(allSpan[k] * maxRatio / valueList.spanVal)) * valueList.spanVal;
        //         defineTmp.spanLengthW.append(span);
        //     }

        //     QVector<double>spanTmp;
        //     spanTmp.append(0);

        //     spanTmp.append(defineTmp.spanLengthW[0]);

        //     for(int j = 0;j<numN - 2;j++)
        //         spanTmp.append(defineTmp.spanLengthW[j] + defineTmp.spanLengthW[j + 1]);
        //     defineTmp.spanW = spanTmp;
        // }else{
        //     defineTmp.spanLengthW = initialWingData.spanLengthW;
        //     defineTmp.spanW = initialWingData.spanW;
        // }

        initialWingDataArray.append(defineTmp);
    }


    generateSelectionProbability();
    getLengthChromsome();

}

void wingOptimization::sortResult(const QVector<double>&result){
    QVector<double>P = result;
    QVector<double>T;

    int len;
    if(result.length() == valueList.initialEliteNum){

        len = valueList.initialEliteNum;

    }
    else{
        for(int i = 0; i<historyResult.length();i++)
            P.append(historyResult[i]);

        len = valueList.eliteNum * 2;
    }




    if(!sortArray.isEmpty())
        QVector<int>().swap(sortArray);


    for(int i = 0; i < len; i++)
        sortArray.append(i);
    for(int i = 0; i < len; i++){
        for(int j = 0; j < len - i - 1; j++){
            if(P[j] < P[j + 1]){
                double temp = P[j];
                P[j] = P[j + 1];
                P[j + 1] = temp;

                int ind = sortArray[j];
                sortArray[j] = sortArray[j + 1];
                sortArray[j + 1] = ind;
            }
        }
    }


    // 去掉 sortArray 中重复的索引
    // QSet<int> usedIndices;
    // for (int i = 0; i < len; ++i) {
    //     if (!usedIndices.contains(sortArray[i])) {
    //         usedIndices.insert(sortArray[i]);
    //     } else {
    //         // 找到重复的索引，用后面的索引填充
    //         int newIndex = len - 1;
    //         while (usedIndices.contains(newIndex)) {
    //             --newIndex;
    //         }
    //         sortArray[i] = newIndex;
    //         usedIndices.insert(newIndex);
    //     }
    // }


    for(int i = 0; i<valueList.eliteNum;i++){
        T.append(P[i]);
    }


    historyResult = T;
    bestSolutionArray.append(P[0]);
    bestSolution = P[0];
    bestIndex = sortArray[0];
    // bestIndex = sortArray[0];


    // double maxP = max(bestSolution);
    // historySolution.append(maxP);
}
void wingOptimization::updateChromomeSequenceDec() {
    QVector<wingDefinition> updatedDefinitions;
    int len = valueList.eliteNum;

    if (wingChromosomeSequenceDec.isEmpty()) {
        for (int i = 0; i < len; ++i) {
            int index = sortArray[i];
            updatedDefinitions.append(initialWingDataArray[index]);
        }
    } else {
        QVector<wingDefinition> combinedDefinitions = wingChromosomeSequenceDec;
        combinedDefinitions.append(historyWingChromosomeSequenceDec);

        for (int i = 0; i < len; ++i) {
            int index = sortArray[i];
            if (index >= 0 && index < combinedDefinitions.size()) {
                updatedDefinitions.append(combinedDefinitions[index]);
            } else {
                //qDebug() << "Error: Index out of range" << index;
            }
        }

    }



    bestWingData = updatedDefinitions[0];
    wingChromosomeSequenceDec = updatedDefinitions;
    historyWingChromosomeSequenceDec = updatedDefinitions;


}

void wingOptimization::selectionChromosomeSequenceDec() {
    QVector<double> randA;  // 轮盘赌决定概率
    QVector<int> randS;     // 轮盘赌选择索引

    // 生成随机概率
    for (int i = 0; i < valueList.eliteNum; i++) {
        double tmp = QRandomGenerator::global()->bounded(1.0);
        randA.append(tmp);
    }


    // 执行轮盘赌选择
    for (int i = 0; i < valueList.eliteNum; i++) {
        double randValue = randA[i];
        int selectedIdx = -1;

        for (int j = 0; j < selectProbability.size() - 1; j++) {
            if (randValue >= selectProbability[j] && randValue < selectProbability[j + 1]) {
                selectedIdx = j;
                break;
            }
        }

        if (selectedIdx != -1) {
            randS.append(selectedIdx);
        } else {
            // 如果未找到有效索引，可以考虑是否需要处理异常情况
            //qDebug() << "Error: No valid selection found for probability" << randValue;
        }
    }






    for(int i = 0;i<valueList.eliteNum;i++){
        wingChromosomeSequenceDec[i] = historyWingChromosomeSequenceDec[randS[i]];
    }

    for(int i = valueList.eliteNum / 2;i<valueList.eliteNum;i++){
        if(valueList.variation * 2 > QRandomGenerator::global()->bounded(1.0)){
            changeWingData(wingChromosomeSequenceDec[i]);
        }

    }

    sortArray = randS;
}
void wingOptimization::generateSelectionProbability(){
    selectProbability.append(0);
    double tmp = 0;

    if(valueList.selection == 1){
        for(int i = 0; i < valueList.eliteNum; i++){
            tmp = static_cast<double>((i + 1)) / valueList.eliteNum;
            selectProbability.append(tmp);
        }
    }
    else {
        for(int i = 0; i < valueList.eliteNum; i++){
            tmp = (1 - valueList.selection) / (1 - pow(valueList.selection,valueList.eliteNum)) * pow(valueList.selection,(valueList.eliteNum - i - 1)) + tmp;
            selectProbability.append(tmp);
        }

    }

}
void wingOptimization::decToBin(){
    //if(valueList.airfoilChange)
        //airfoilDecToBin();
    //if(valueList.spanChange)
        spanDecToBin();
    //if(valueList.chordChange)
        chordDecToBin();
    //if(valueList.offsetChange)
        offsetDecToBin();
    //if(valueList.twistChange)
        twistDecToBin();
    //if(valueList.dihedChange)
        dihedDecToBin();
}

void wingOptimization::overlappingOperations(){
    //if(valueList.airfoilChange)
        //airfoilOverlappingOperations();
    //if(valueList.spanChange)
        spanOverlappingOperations();
    //if(valueList.chordChange)
        chordOverlappingOperations();
    //if(valueList.offsetChange)
        offsetOverlappingOperations();
    //if(valueList.twistChange)
        twistOverlappingOperations();
    //if(valueList.dihedChange)
        dihedOverlappingOperations();

}
void wingOptimization::variationOperations(){
    //if(valueList.airfoilChange)
    //airfoilVariationOperations();

    //if(valueList.spanChange)
    spanVariationOperations();

    //if(valueList.chordChange)
    chordVariationOperations();

    //if(valueList.offsetChange)
    offsetVariationOperations();

    //if(valueList.twistChange)
    twistVariationOperations();

    //if(valueList.dihedChange)
    dihedVariationOperations();



}
void wingOptimization::binToDec(){
    //if(valueList.airfoilChange)
        //airfoilBinToDec();
    //if(valueList.spanChange)
        spanBinToDec();
    //if(valueList.chordChange)
        chordBinToDec();
    //if(valueList.offsetChange)
        offsetBinToDec();
    //if(valueList.twistChange)
        twistBinToDec();
    //if(valueList.dihedChange)
        dihedBinToDec();

    checkValue();

    QVector<wingDefinition>defineTmp;
    int len = valueList.eliteNum;;
    for(int i = 0;i<len;i++){
        wingDefinition tmp;
        QVector<double>spanTmp;
        tmp.gridU = initialWingData.gridU;
        tmp.gridV = initialWingData.gridV;
        tmp.vMeshType = initialWingData.vMeshType;
        tmp.uMeshType = initialWingData.uMeshType;
        tmp.realChord = initialWingData.realChord;
        tmp.airfoilInputType = false;//导入cst参数而不是翼型标志
        //tmp.cstArray = cstArray[i];
        tmp.cstArray = initialWingData.cstArray;

        tmp.spanLengthW = spanArray[i];
        spanTmp.append(0);
        spanTmp.append(spanArray[i][0]);
        for(int j = 0;j<numN - 2;j++)
            spanTmp.append(spanArray[i][j] + spanArray[i][j + 1]);
        tmp.spanW = spanTmp;

        tmp.chordLengthW = chordArray[i];
        tmp.offsetLengthW = offsetLengthArray[i];
        tmp.twistAngleW = twistAngleArray[i];
        tmp.dihedralAngleW = dihedralAngleArray[i];



        // //cst
        // if(valueList.airfoilChangeArray[i])
        //     tmp.cstArray = cstArray[i];
        // else
        //     tmp.cstArray = initialWingData.cstArray;

        // //span
        // if(valueList.spanChangeArray[i]){
        //     spanTmp.append(0);
        //     spanTmp.append(spanArray[i][0]);
        //     for(int j = 0;j<numN - 2;j++)
        //         spanTmp.append(spanArray[i][j] + spanArray[i][j + 1]);
        //     tmp.spanW = spanTmp;
        //     tmp.spanLengthW = spanArray[i];
        // }else{
        //     tmp.spanW = initialWingData.spanW;
        //     tmp.spanLengthW = initialWingData.spanLengthW;
        // }
        // //chord
        // if(valueList.chordChangeArray[i])
        //     tmp.chordLengthW = chordArray[i];
        // else
        //     tmp.chordLengthW = initialWingData.chordLengthW;
        // //offset
        // if(valueList.offsetChangeArray[i])
        //     tmp.offsetLengthW = offsetLengthArray[i];
        // else
        //     tmp.offsetLengthW = initialWingData.offsetLengthW;
        // //twist
        // if(valueList.twistChangeArray[i])
        //     tmp.twistAngleW = twistAngleArray[i];
        // else
        //     tmp.twistAngleW = initialWingData.twistAngleW;
        // //dihed
        // if(valueList.dihedChangeArray[i])
        //     tmp.dihedralAngleW = dihedralAngleArray[i];
        // else
        //     tmp.dihedralAngleW = initialWingData.dihedralAngleW;

        defineTmp.append(tmp);


    }

    wingChromosomeSequenceDec = defineTmp;

}

void wingOptimization::airfoilDecToBin(){
    int cstVal = static_cast<int>(1.0 / valueList.val);
    QVector<QVector<QVector<QString>>>cstString;
    int tmp1;
    for(int i = 0;i<valueList.eliteNum;i++){
        QVector<QVector<QString>>tmpArray1;
        for(int j = 0;j<numN;j++){
            QVector<QString>tmps;
            for(int k = 0;k<valueList.solutionsNum;k++){
                tmp1 = wingChromosomeSequenceDec[i].cstArray[j][k] * cstVal * symbolCSTArray[j][k];
                tmps.append(decToBinAlone(tmp1,cstLength));
            }
            tmpArray1.append(tmps);
        }
        cstString.append(tmpArray1);
    }
    cstStringArray = cstString;

}
void wingOptimization::airfoilOverlappingOperations(){
    QVector<int>randBeginAirfoil;
    QVector<int>randLengthAirfoil;
    QVector<int>randBeginCst;
    QVector<int>randLengthCst;
    QVector<int>randBeginVal1;
    QVector<int>randLengthVal1;
    int step = valueList.eliteNum / 2;
    int beginA0,beginA1, beginB1;


    for(int i = 0;i<step;i++){
        beginA0 = QRandomGenerator::global()->bounded(numN - 1);
        randBeginAirfoil.append(beginA0);
        randLengthAirfoil.append(numN - beginA0);
        beginA1 = QRandomGenerator::global()->bounded(valueList.solutionsNum / 2 - 1);
        randBeginCst.append(beginA1);
        randLengthCst.append(valueList.solutionsNum / 2 - beginA1);
        beginB1 = QRandomGenerator::global()->bounded(cstLength - 1);
        randBeginVal1.append(beginB1);
        randLengthVal1.append(cstLength - beginB1);
    }

    double randA1;
    for(int i = 0;i<step;i++){
        randA1 = QRandomGenerator::global()->bounded(1.0);
        if(randA1 < valueList.cross){
            int begin,len;
            for(int j = randBeginAirfoil[i];j<randBeginAirfoil[i] + randLengthAirfoil[i];j++){
                if(randLengthCst[i] != 0 && valueList.airfoilChangeArray[j]){
                    begin = randBeginCst[i];
                    len = randLengthCst[i] + begin;
                    for(int k = begin; k < len; k++){
                        swapChromosome(randBeginVal1[i],randLengthVal1[i],cstStringArray[2 * i][j][k],cstStringArray[2 * i + 1][j][k]);
                        swapChromosome(randBeginVal1[i],randLengthVal1[i],cstStringArray[2 * i][j][k + valueList.solutionsNum / 2],cstStringArray[2 * i + 1][j][k + valueList.solutionsNum / 2]);
                    }
                }
            }
        }
    }

}
void wingOptimization::airfoilVariationOperations(){
    int step = valueList.eliteNum;
    int randLocation1,randBegin1;
    double randA1;
    for(int i = 0;i<step;i++){
        for(int j = 0;j<numN;j++){
            if(valueList.airfoilChangeArray[j]){
                randA1 = QRandomGenerator::global()->bounded(1.0);
                if(valueList.variation > randA1){
                    randLocation1 = QRandomGenerator::global()->bounded(valueList.solutionsNum - 1);
                    randBegin1 = QRandomGenerator::global()->bounded(cstLength - 1);
                    changeChromosome(randBegin1,cstStringArray[i][j][randLocation1]);
                }
            }
        }
    }
}
void wingOptimization::airfoilBinToDec(){
    int len = valueList.eliteNum;
    int tmp1;
    double newTmp1 = 0;
    QString stringTmp1;
    bool ok;
    QVector<QVector<QVector<double>>>cstDecArray;
    for(int i = 0;i<len;i++){
        QVector<QVector<double>>cstDec;
        for(int j = 0;j<numN;j++){
            QVector<double>cstTmp;
            for(int k = 0;k<valueList.solutionsNum;k++){
                stringTmp1 = cstStringArray[i][j][k];
                tmp1 = stringTmp1.toInt(&ok,2);
                if(ok)
                    newTmp1 = static_cast<double>(tmp1) * valueList.val * symbolCSTArray[j][k];
                cstTmp.append(newTmp1);
            }
            cstDec.append(cstTmp);
        }
        cstDecArray.append(cstDec);
    }
    cstArray = cstDecArray;
}



void wingOptimization::spanDecToBin(){
    int Val2 = static_cast<int>(1.0 / valueList.spanVal);
    QVector<QVector<QString>>spanString;
    int tmp2;
    for(int i = 0;i<valueList.eliteNum;i++){
        QVector<QString>tmpArray2;
        for(int l = 0;l<numN - 1;l++){
            tmp2 = wingChromosomeSequenceDec[i].spanLengthW[l] * Val2;
            tmpArray2.append(decToBinAlone(tmp2,spanLength));
        }
        spanString.append(tmpArray2);
    }
    spanStringArray = spanString;
}
void wingOptimization::spanOverlappingOperations(){
    QVector<int>randBeginSpan;
    QVector<int>randLengthSpan;
    QVector<int>randBeginVal2;
    QVector<int>randLengthVal2;
    int step = valueList.eliteNum / 2;
    int beginA2, beginB2;

    for(int i = 0;i<step;i++){
        beginA2 = QRandomGenerator::global()->bounded(numN - 2);
        randBeginSpan.append(beginA2);
        randLengthSpan.append(numN - 1 - beginA2);
        beginB2 = QRandomGenerator::global()->bounded(spanLength - 1);
        randBeginVal2.append(beginB2);
        randLengthVal2.append(spanLength - beginB2);
    }
    double randA2;
    for(int i = 0;i<step;i++){
        randA2 = QRandomGenerator::global()->bounded(1.0);
        if(randA2 < valueList.cross){
            int begin,len;

            if(randLengthSpan[i] != 0){
                begin = randBeginSpan[i];
                len = randLengthSpan[i] + begin;
                for(int j = begin; j < len; j++){
                    if(valueList.spanChangeArray[j])
                        swapChromosome(randBeginVal2[i],randLengthVal2[i],spanStringArray[2 * i][j],spanStringArray[2 * i + 1][j]);

                }
            }
        }
    }

}
void wingOptimization::spanVariationOperations(){
    int step = valueList.eliteNum;
    //int randLocation2,randBegin2;
    int randBegin2;
    double randA2;
    for(int i = 0;i<step;i++){
        for(int j = 0;j<numN - 1;j++){
            if(valueList.spanChangeArray[0]){
                randA2 = QRandomGenerator::global()->bounded(1.0);
                if(valueList.variation > randA2){
                    //randLocation2 = QRandomGenerator::global()->bounded(numN - 2);
                    randBegin2 = QRandomGenerator::global()->bounded(spanLength - 1);
                    changeChromosome(randBegin2,spanStringArray[i][j]);
                }
            }
        }
    }

}
void wingOptimization::spanBinToDec(){
    int len = sortArray.length();
    int tmp2;
    double newTmp2 = 0;
    QString stringTmp2;
    bool ok;
    QVector<QVector<double>>spanDecArray;
    for(int i = 0;i<len;i++){
        QVector<double>spanDec;
        for(int j = 0;j<numN - 1;j++){
            stringTmp2 = spanStringArray[i][j];
            tmp2 = stringTmp2.toInt(&ok,2);
            if(ok)
                newTmp2 = static_cast<double>(tmp2) * valueList.spanVal;
            spanDec.append(newTmp2);
        }
        spanDecArray.append(spanDec);
    }
    spanArray = spanDecArray;

}



void wingOptimization::chordDecToBin(){
    int Val3 = static_cast<int>(1.0 / valueList.chordVal);
    QVector<QVector<QString>>chordString;
    int tmp3;
    for(int i = 0;i<valueList.eliteNum;i++){
        QVector<QString>tmpArray3;
        for(int j = 0;j<numN;j++){
            tmp3 = wingChromosomeSequenceDec[i].chordLengthW[j] * Val3;
            tmpArray3.append(decToBinAlone(tmp3,chordLength));
        }
        chordString.append(tmpArray3);
    }
    chordStringArray = chordString;


}
void wingOptimization::chordOverlappingOperations(){
    QVector<int>randBeginChord;
    QVector<int>randLengthChord;
    QVector<int>randBeginVal3;
    QVector<int>randLengthVal3;
    int step = valueList.eliteNum / 2;
    int beginA3, beginB3;
    for(int i = 0;i<step;i++){
        beginA3 = QRandomGenerator::global()->bounded(numN - 1);
        randBeginChord.append(beginA3);
        randLengthChord.append(numN  - beginA3);
        beginB3 = QRandomGenerator::global()->bounded(chordLength - 1);
        randBeginVal3.append(beginB3);
        randLengthVal3.append(chordLength - beginB3);
    }
    double randA3;
    for(int i = 0;i<step;i++){
        randA3 = QRandomGenerator::global()->bounded(1.0);
        if(randA3 < valueList.cross){
            int begin,len;

            if(randLengthChord[i] != 0){
                begin = randBeginChord[i];
                len = randLengthChord[i] + begin;
                for(int j = begin; j < len; j++){
                    if(valueList.chordChangeArray[j]){
                        int k = QRandomGenerator::global()->bounded(numN - 1);
                        swapChromosome(randBeginVal3[i],randLengthVal3[i],chordStringArray[2 * i][j],chordStringArray[2 * i + 1][k]);
                    }
                }
            }
        }
    }
}
void wingOptimization::chordVariationOperations(){
    int step = valueList.eliteNum;
    //int randLocation3,randBegin3;
    int randBegin3;
    double randA3;
    for(int i = 0;i<step;i++){
        for(int j = 0;j<numN;j++){
            if(valueList.chordChangeArray[j]){
                randA3 = QRandomGenerator::global()->bounded(1.0);
                if(valueList.variation > randA3){
                    //randLocation3 = QRandomGenerator::global()->bounded(numN - 1);
                    randBegin3 = QRandomGenerator::global()->bounded(chordLength - 1);
                    changeChromosome(randBegin3,chordStringArray[i][j]);
                }
            }

        }

    }

}
void wingOptimization::chordBinToDec(){
    int len = sortArray.length();
    int tmp3;
    double newTmp3 = 0;
    QString stringTmp3;
    bool ok;
    QVector<QVector<double>>chordDecArray;
    for(int i = 0;i<len;i++){
        QVector<double>chordDec;
        for(int j = 0;j<numN;j++){
            stringTmp3 = chordStringArray[i][j];
            tmp3 = stringTmp3.toInt(&ok,2);
            if(ok)
                newTmp3 = static_cast<double>(tmp3) * valueList.chordVal;
            chordDec.append(newTmp3);
        }
        chordDecArray.append(chordDec);
    }
    chordArray = chordDecArray;
}



void wingOptimization::offsetDecToBin(){
    int Val4 = static_cast<int>(1.0 / valueList.offsetLengthVal);
    QVector<QVector<QString>>offsetString;
    int tmp4;
    for(int i = 0;i<valueList.eliteNum;i++){
        QVector<QString>tmpArray4;
        for(int j = 0;j<numN;j++){
            tmp4 = wingChromosomeSequenceDec[i].offsetLengthW[j] * Val4 * symbolOffsetArray[i][j];
            tmpArray4.append(decToBinAlone(tmp4,offsetLength));
        }
        offsetString.append(tmpArray4);
    }
    offsetStringArray = offsetString;
}
void wingOptimization::offsetOverlappingOperations(){
    QVector<int>randBeginOffset;
    QVector<int>randLengthOffset;
    QVector<int>randBeginVal4;
    QVector<int>randLengthVal4;
    int step = valueList.eliteNum / 2;
    int beginA4, beginB4;
    for(int i = 0;i<step;i++){
        beginA4 = QRandomGenerator::global()->bounded(numN - 1);
        randBeginOffset.append(beginA4);
        randLengthOffset.append(numN - beginA4);
        beginB4 = QRandomGenerator::global()->bounded(offsetLength - 1);
        randBeginVal4.append(beginB4);
        randLengthVal4.append(offsetLength - beginB4);
    }
    double randA4;
    for(int i = 0;i<step;i++){
        randA4 = QRandomGenerator::global()->bounded(1.0);
        if(randA4 < valueList.cross){
            int begin,len;

            if(randLengthOffset[i] != 0){
                begin = randBeginOffset[i];
                len = randLengthOffset[i] + begin;
                for(int j = begin; j < len; j++){
                    if(valueList.offsetChangeArray[j])
                    //int k = QRandomGenerator::global()->bounded(numN - 1);
                    swapChromosome(randBeginVal4[i],randLengthVal4[i],offsetStringArray[2 * i][j],offsetStringArray[2 * i + 1][j]);


                }
            }
        }
    }
}
void wingOptimization::offsetVariationOperations(){
    int step = valueList.eliteNum;
    //int randLocation4,randBegin4;
    int randBegin4;
    double randA4;
    for(int i = 0;i<step;i++){
        for(int j = 0;j<numN;j++){
            if(valueList.offsetChangeArray[j]){
                randA4 = QRandomGenerator::global()->bounded(1.0);
                if(valueList.variation > randA4){
                    //randLocation4 = QRandomGenerator::global()->bounded(numN - 1);
                    randBegin4 = QRandomGenerator::global()->bounded(offsetLength - 1);
                    changeChromosome(randBegin4,offsetStringArray[i][j]);
                }

            }
        }

    }
}
void wingOptimization::offsetBinToDec(){
    int len = sortArray.length();
    int tmp4;
    double newTmp4 = 0;
    QString stringTmp4;
    bool ok;
    QVector<QVector<double>>offsetDecArray;
    for(int i = 0;i<len;i++){
        QVector<double>offsetDec;
        for(int j = 0;j<numN;j++){
            stringTmp4 = offsetStringArray[i][j];
            tmp4 = stringTmp4.toInt(&ok,2);
            if(ok)
                newTmp4 = static_cast<double>(tmp4) * valueList.offsetLengthVal * symbolOffsetArray[i][j];
            offsetDec.append(newTmp4);
        }
        offsetDecArray.append(offsetDec);
    }
    offsetLengthArray = offsetDecArray;
}



void wingOptimization::twistDecToBin(){
    int Val5 = static_cast<int>(1.0 / valueList.twisAngleVal);
    QVector<QVector<QString>>twisString;
    int tmp5;
    for(int i = 0;i<valueList.eliteNum;i++){
        QVector<QString>tmpArray5;
        for(int j = 0;j<numN;j++){
            tmp5 = wingChromosomeSequenceDec[i].twistAngleW[j] * Val5 * symbolTwistArray[i][j];
            tmpArray5.append(decToBinAlone(tmp5,twistLength));
        }
        twisString.append(tmpArray5);
    }
    twistStringArray = twisString;
}
void wingOptimization::twistOverlappingOperations(){
    QVector<int>randBeginTwist;
    QVector<int>randLengthTwist;
    QVector<int>randBeginVal5;
    QVector<int>randLengthVal5;
    int step = valueList.eliteNum / 2;
    int beginA5, beginB5;
    for(int i = 0;i<step;i++){
        beginA5 = QRandomGenerator::global()->bounded(numN - 1);
        randBeginTwist.append(beginA5);
        randLengthTwist.append(numN - beginA5);
        beginB5 = QRandomGenerator::global()->bounded(twistLength - 1);
        randBeginVal5.append(beginB5);
        randLengthVal5.append(twistLength - beginB5);
    }
    double randA5;
    for(int i = 0;i<step;i++){
        randA5 = QRandomGenerator::global()->bounded(1.0);
        if(randA5 < valueList.cross){
            int begin,len;

            if(randLengthTwist[i] != 0){
                begin = randBeginTwist[i];
                len = randLengthTwist[i] + begin;
                for(int j = begin; j < len; j++){
                    if(valueList.twistChangeArray[j])
                    //int k = QRandomGenerator::global()->bounded(numN - 1);
                    swapChromosome(randBeginVal5[i],randLengthVal5[i],twistStringArray[2 * i][j],twistStringArray[2 * i + 1][j]);

                }
            }
        }
    }
}
void wingOptimization::twistVariationOperations(){
    int step = valueList.eliteNum;
    //int randLocation5,randBegin5;
    int randBegin5;
    double randA5;
    for(int i = 0;i<step;i++){
        for(int j = 0;j<numN;j++){
            if(valueList.twistChangeArray[j]){
                randA5 = QRandomGenerator::global()->bounded(1.0);
                if(valueList.variation > randA5){
                    //randLocation5 = QRandomGenerator::global()->bounded(numN - 1);
                    randBegin5 = QRandomGenerator::global()->bounded(twistLength - 1);
                    changeChromosome(randBegin5,twistStringArray[i][j]);
                }
            }
        }

    }
}
void wingOptimization::twistBinToDec(){
    int len = sortArray.length();
    int tmp5;
    double newTmp5 = 0;
    QString stringTmp5;
    bool ok;
    QVector<QVector<double>>twistDecArray;
    for(int i = 0;i<len;i++){
        QVector<double>twistDec;
        for(int j = 0;j<numN;j++){
            stringTmp5 = twistStringArray[i][j];
            tmp5 = stringTmp5.toInt(&ok,2);
            if(ok)
                newTmp5 = static_cast<double>(tmp5) * valueList.twisAngleVal * symbolTwistArray[i][j];
            twistDec.append(newTmp5);
        }
        twistDecArray.append(twistDec);
    }
    twistAngleArray = twistDecArray;
}


void wingOptimization::dihedDecToBin(){
    int Val6 = static_cast<int>(1.0 / valueList.dihedralAngleVal);
    QVector<QVector<QString>>dihedString;
    int tmp6;
    for(int i = 0;i<valueList.eliteNum;i++){
        QVector<QString>tmpArray6;
        for(int j = 0;j<numN;j++){
            tmp6 = wingChromosomeSequenceDec[i].dihedralAngleW[j] * Val6 * symbolDihedArray[i][j];
            tmpArray6.append(decToBinAlone(tmp6,dihedralLength));
        }
        dihedString.append(tmpArray6);
    }
    dihedStringArray = dihedString;
}
void wingOptimization::dihedOverlappingOperations(){
    QVector<int>randBeginDihed;
    QVector<int>randLengthDihed;
    QVector<int>randBeginVal6;
    QVector<int>randLengthVal6;
    int step = valueList.eliteNum / 2;
    int beginA6, beginB6;
    for(int i = 0;i<step;i++){
        beginA6 = QRandomGenerator::global()->bounded(numN - 1);
        randBeginDihed.append(beginA6);
        randLengthDihed.append(numN - beginA6);
        beginB6 = QRandomGenerator::global()->bounded(dihedralLength - 1);
        randBeginVal6.append(beginB6);
        randLengthVal6.append(dihedralLength - beginB6);
    }
    double randA6;
    for(int i = 0;i<step;i++){
        randA6 = QRandomGenerator::global()->bounded(1.0);
        if(randA6 < valueList.cross){
            int begin,len;

            if(randLengthDihed[i] != 0){
                begin = randBeginDihed[i];
                len = randLengthDihed[i] + begin;
                for(int j = begin; j < len; j++){
                    if(valueList.dihedChangeArray[j])
                        swapChromosome(randBeginVal6[i],randLengthVal6[i],dihedStringArray[2 * i][j],dihedStringArray[2 * i + 1][j]);

                }
            }
        }
    }
}
void wingOptimization::dihedVariationOperations(){
    int step = valueList.eliteNum;
    //int randLocation6,randBegin6;
    int randBegin6;
    double randA6;
    for(int i = 0;i<step;i++){
        for(int j = 0;j<numN;j++){
            if(valueList.dihedChangeArray[j]){
                randA6 = QRandomGenerator::global()->bounded(1.0);
                if(valueList.variation > randA6){
                    //randLocation6 = QRandomGenerator::global()->bounded(numN - 1);
                    randBegin6 = QRandomGenerator::global()->bounded(dihedralLength - 1);
                    changeChromosome(randBegin6,dihedStringArray[i][j]);
                }
            }
        }
    }
}
void wingOptimization::dihedBinToDec(){
    int len = sortArray.length();
    int tmp6;
    double newTmp6 = 0;
    QString stringTmp6;
    bool ok;
    QVector<QVector<double>>dihedDecArray;
    for(int i = 0;i<len;i++){
        QVector<double>dihedDec;
        for(int j = 0;j<numN;j++){
            stringTmp6 = dihedStringArray[i][j];
            tmp6 = stringTmp6.toInt(&ok,2);
            if(ok)
                newTmp6 = static_cast<double>(tmp6) * valueList.dihedralAngleVal * symbolDihedArray[i][j];
            dihedDec.append(newTmp6);
        }
        dihedDecArray.append(dihedDec);
    }
    dihedralAngleArray = dihedDecArray;
}



void wingOptimization::checkValue(){
    int len = sortArray.length();
    for(int i = 0;i<len;i++){
        for(int j = 0;j<numN;j++){
            if(valueList.airfoilChangeArray[j]){
                for(int k = 0;k<valueList.solutionsNum;k++){
                    if(cstArray[i][j][k] > maxCstArrayDec[j][k]){
                        cstArray[i][j][k] = maxCstArrayDec[j][k];
                        cstStringArray[i][j][k] = maxBin[0];
                    }else if(cstArray[i][j][k] < minCstArrayDec[j][k]){
                        cstArray[i][j][k] = minCstArrayDec[j][k];
                        cstStringArray[i][j][k] = minBin[0];
                    }
                }
            }
            if(valueList.spanChangeArray[j]){
                if(j < numN - 1){
                    if(spanArray[i][j] > valueList.maxSpan[j]){
                        spanArray[i][j] = valueList.maxSpan[j];
                        spanStringArray[i][j] = maxBin[1];
                    }else if(spanArray[i][j] < valueList.minSpan[j]){
                        spanArray[i][j] = valueList.minSpan[j];
                        spanStringArray[i][j] = minBin[1];
                    }
                }
            }

            if(valueList.chordChangeArray[j]){
                if(chordArray[i][j] > valueList.maxChord[j]){
                    chordArray[i][j] = valueList.maxChord[j];
                    chordStringArray[i][j] = maxBin[2];
                }else if(chordArray[i][j] < valueList.minChord[j]){
                    chordArray[i][j] = valueList.minChord[j];
                    chordStringArray[i][j] = minBin[2];
                }
            }
            if(valueList.offsetChangeArray[j]){
                if(offsetLengthArray[i][j] > valueList.maxOffsetLength[j]){
                    offsetLengthArray[i][j] = valueList.maxOffsetLength[j];
                    offsetStringArray[i][j] = maxBin[3];
                }else if(offsetLengthArray[i][j] < valueList.minOffsetLength[j]){
                    offsetLengthArray[i][j] = valueList.minOffsetLength[j];
                    offsetStringArray[i][j] = minBin[3];
                }
            }


            if(valueList.twistChangeArray[j]){
                if(twistAngleArray[i][j] > valueList.maxTwisAngle[j]){
                    twistAngleArray[i][j] = valueList.maxTwisAngle[j];
                    twistStringArray[i][j] = maxBin[4];
                }else if(twistAngleArray[i][j] < valueList.minTwisAngle[j]){
                    twistAngleArray[i][j] = valueList.minTwisAngle[j];
                    twistStringArray[i][j] = minBin[4];
                }
            }

            if(valueList.dihedChangeArray[j]){
                if(dihedralAngleArray[i][j] > valueList.maxDihedralAngle[j]){
                    dihedralAngleArray[i][j] = valueList.maxDihedralAngle[j];
                    dihedStringArray[i][j] = maxBin[5];
                }else if(dihedralAngleArray[i][j] < valueList.minDihedralAngle[j]){
                    dihedralAngleArray[i][j] = valueList.minDihedralAngle[j];
                    dihedStringArray[i][j] = minBin[5];
                }
            }


        }
    }
}
void wingOptimization::changeWingData(wingDefinition& wingData){



    int cstMax, cstMin;
    double cst,span,chord,offsetLength,twisAngle,dihedralAngle;
    QVector<QVector<double>>cstArrayA;
    QVector<double>chordArrayA;
    QVector<double>twistArrayA;
    QVector<double>offsetArrayA;
    QVector<double>dihedArrayA;
    QVector<double>spanArrayA;
    QVector<double>spanTmp;

    int cstVal = int(1.0 / valueList.val);
    double minRatio = valueList.MinSpan / initialWingData.Span();
    double maxRatio = valueList.MaxSpan / initialWingData.Span();
    for(int j = 0;j<numN;j++){

        if(valueList.airfoilChangeArray[j]){
            QVector<double>cstTmp;
            for(int k = 0;k<valueList.solutionsNum;k++){
                cstMax = abs(static_cast<int>(maxCstArrayDec[j][k] * cstVal));
                cstMin = abs(static_cast<int>(minCstArrayDec[j][k] * cstVal));
                cst = double(generateRandomInt(cstMin,cstMax) * valueList.val * symbolCSTArray[j][k]);
                cstTmp.append(cst);
            }
            cstArrayA.append(cstTmp);
        }else{
            cstArrayA.append(initialWingData.cstArray[j]);
        }



        if(valueList.chordChangeArray[j]){
            chord = generateRandomInt(int(valueList.minChord[j] / valueList.chordVal),int(valueList.maxChord[j] / valueList.chordVal)) * valueList.chordVal;
            chordArrayA.append(chord);
        }else{
            chordArrayA.append(initialWingData.chordLengthW[j]);
        }


        if(valueList.offsetChangeArray[j]){
            offsetLength= generateRandomInt(int(valueList.minOffsetLength[j] / valueList.offsetLengthVal),int(valueList.maxOffsetLength[j] / valueList.offsetLengthVal)) * valueList.offsetLengthVal;
            offsetArrayA.append(offsetLength);
        }else{
            offsetArrayA.append(initialWingData.offsetLengthW[j]);
        }


        if(valueList.twistChangeArray[j]){
            twisAngle = generateRandomInt(int(valueList.minTwisAngle[j] / valueList.twisAngleVal),int(valueList.maxTwisAngle[j] / valueList.twisAngleVal)) * valueList.twisAngleVal;
            twistArrayA.append(twisAngle);
        }else{
            twistArrayA.append(initialWingData.twistAngleW[j]);
        }


        if(valueList.dihedChangeArray[j]){
            dihedralAngle = generateRandomInt(int(valueList.minDihedralAngle[j] / valueList.dihedralAngleVal),int(valueList.maxDihedralAngle[j] / valueList.dihedralAngleVal)) * valueList.dihedralAngleVal;
            dihedArrayA.append(dihedralAngle);
        }else{
            dihedArrayA.append(initialWingData.dihedralAngleW[j]);
        }


    }

    for(int k = 0;k<numN - 1;k++){
        if(valueList.spanChangeArray[0]){
            span = generateRandomInt(int(allSpan[k] * minRatio / valueList.spanVal),int(allSpan[k] * maxRatio / valueList.spanVal)) * valueList.spanVal;
            spanArrayA.append(span);
        }
        else{
            spanArrayA.append(initialWingData.spanLengthW[k]);
        }
    }


    spanTmp.append(0);
    spanTmp.append(spanArrayA[0]);
    for(int j = 0;j<numN - 2;j++)
        spanTmp.append(spanArrayA[j] + spanArrayA[j + 1]);






    wingData.cstArray = cstArrayA;
    wingData.chordLengthW = chordArrayA;
    wingData.offsetLengthW = offsetArrayA;
    wingData.twistAngleW = twistArrayA;
    wingData.dihedralAngleW = dihedArrayA;
    wingData.spanLengthW = spanArrayA;
    wingData.spanW = spanTmp;



}
void wingOptimization::swapChromosome(const int begin,const int len,QString&a,QString&b){
    QString tmpA = a.mid(begin,len);
    QString tmpB = b.mid(begin,len);
    a.replace(begin,len,tmpB);
    b.replace(begin,len,tmpA);
}
void wingOptimization::changeChromosome(const int begin,QString&a){
    QString val = a.mid(begin,1);
    if(val == "1")
        a.replace(begin,1,"0");
    else
        a.replace(begin,1,"1");

}
void wingOptimization::getLengthChromsome(){
    int cstVal1 = static_cast<int>(1.0 / valueList.val * max(allSpan) * (1 + valueList.cstRadio));
    int spanVal1 = static_cast<int>(1.0 / valueList.spanVal * max(valueList.maxSpan) / 2);
    int chordVal1 = static_cast<int>(1.0 / valueList.chordVal * max(valueList.maxChord));
    int offsetVal1 = static_cast<int>(1.0 / valueList.offsetLengthVal * max(valueList.maxOffsetLength));
    int twistVal1 = static_cast<int>(1.0 / valueList.twisAngleVal * max(valueList.maxTwisAngle));
    int dihedVal1 = static_cast<int>(1.0 / valueList.dihedralAngleVal * max(valueList.maxDihedralAngle));


    int cstVal2 = static_cast<int>(1.0 / valueList.val * min(allSpan) * (1 - valueList.cstRadio));
    int spanVal2 = static_cast<int>(1.0 / valueList.spanVal * min(valueList.minSpan) / 2);
    int chordVal2 = static_cast<int>(1.0 / valueList.chordVal * min(valueList.minChord));
    int offsetVal2 = static_cast<int>(1.0 / valueList.offsetLengthVal * min(valueList.minOffsetLength));
    int twistVal2 = static_cast<int>(1.0 / valueList.twisAngleVal * min(valueList.minTwisAngle));
    int dihedVal2 = static_cast<int>(1.0 / valueList.dihedralAngleVal * min(valueList.minDihedralAngle));



    QString a1 = QString::number(cstVal1,2);   QString b1 = QString::number(cstVal2,2);
    QString a2 = QString::number(spanVal1,2);  QString b2 = QString::number(spanVal2,2);
    QString a3 = QString::number(chordVal1,2); QString b3 = QString::number(chordVal2,2);
    QString a4 = QString::number(offsetVal1,2);QString b4 = QString::number(offsetVal2,2);
    QString a5 = QString::number(twistVal1,2); QString b5 = QString::number(twistVal2,2);
    QString a6 = QString::number(dihedVal1,2); QString b6 = QString::number(dihedVal2,2);

    maxBin[0] = a1;minBin[0] = b1;
    maxBin[1] = a2;minBin[1] = b2;
    maxBin[2] = a3;minBin[2] = b3;
    maxBin[3] = a4;minBin[3] = b4;
    maxBin[4] = a5;minBin[4] = b5;
    maxBin[5] = a6;minBin[5] = b6;


    cstLength = a1.length();
    spanLength = a2.length();
    chordLength = a3.length();
    offsetLength = a4.length();
    twistLength = a5.length();
    dihedralLength = a6.length();
}

QString wingOptimization::decToBinAlone(int val, const int len) {
    if(val<0)
        return "0";
    QString valString = QString::number(val, 2); // 明确使用 QString::number

    while (valString.length() < len)
        valString.insert(0, "0");
    return valString;
}

void wingOptimization::restArray(){


    if(!initialWingDataArray.isEmpty())
        initialWingDataArray.clear();



    if(!maxCstArrayDec.isEmpty())
        maxCstArrayDec.clear();
    if(!minCstArrayDec.isEmpty())
        minCstArrayDec.clear();


    if(!symbolCSTArray.isEmpty())
        symbolCSTArray.clear();
    if(!symbolOffsetArray.isEmpty())
        symbolOffsetArray.clear();
    if(!symbolTwistArray.isEmpty())
        symbolTwistArray.clear();
    if(!symbolDihedArray.isEmpty())
        symbolDihedArray.clear();


    if(!spanArray.isEmpty())
        spanArray.clear();
    if(!chordArray.isEmpty())
        chordArray.clear();
    if(!offsetLengthArray.isEmpty())
        offsetLengthArray.clear();
    if(!twistAngleArray.isEmpty())
        twistAngleArray.clear();
    if(!dihedralAngleArray.isEmpty())
        dihedralAngleArray.clear();
    if(!cstArray.isEmpty())
        cstArray.clear();
    if(!allSpan.isEmpty())
        allSpan.clear();

    if(!selectProbability.isEmpty())
        selectProbability.clear();


}
double wingOptimization::generateRandomDouble(double a, double b) {
    // 生成 [0.0, 1.0) 之间的随机数
    double randomValue = QRandomGenerator::global()->generateDouble();

    // 调整范围到 [a, b)
    return a + randomValue * (b - a);
}
int wingOptimization::generateRandomInt(int min, int max) {
    // 使用QRandomGenerator生成[min, max]范围内的随机整数
    return QRandomGenerator::global()->bounded(min, max + 1);
}
double wingOptimization::max(const QVector<double>&array){



    double maxValue = array[0];
    if(array.length() <= 1)
        return maxValue;

    for(int i = 1;i<array.size();i++){
        if(array[i] > maxValue){
            maxValue = array[i];

        }
    }


    return maxValue;
}

double wingOptimization::min(const QVector<double>&array){


    double minValue = array[0];
    if(array.length() <= 1)
        return minValue;

    for(int i = 1;i<array.size();i++){
        if(array[i] < minValue){
            minValue = array[i];

        }
    }

    return minValue;
}
int wingOptimization::getSign(const double value){

    if(value < 0)
        return -1;
    else
        return 1;
}
