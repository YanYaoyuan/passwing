#include "airfoiloptimization.h"
#include <QRandomGenerator>

#include <QDebug>
airfoilOptimization::airfoilOptimization(const GaParameters&value,const QVector<double>&CST) {

    valueList = value;
    cstArray = CST;
}

void airfoilOptimization::initialElite(){
    double max, min, cst;
    int symbol;
    QVector<int>newSymbol;
     int cstVal = static_cast<int>(1 / valueList.val);

    for(int i = 0; i < valueList.solutionsNum; i++){
        max = cstArray[i] * (1 + valueList.cstRadio);
        min = cstArray[i] * (1 - valueList.cstRadio);
        MaxCstArrayDec.append(max);
        MinCstArrayDec.append(min);

        if(cstArray[i] != 0)
            symbol = cstArray[i] / abs(cstArray[i]);
        else
            symbol = 1;
        newSymbol.append(symbol);
    }



    symbolArray = newSymbol;
    int cstMax, cstMin;

    for(int i = 0; i < valueList.initialEliteNum; i++){
        QVector<double>temp1;
        for(int j = 0;j < valueList.solutionsNum; j++){
            cstMax = abs(static_cast<int>(MaxCstArrayDec[j] * cstVal));
            cstMin = abs(static_cast<int>(MinCstArrayDec[j] * cstVal));
            //cst = static_cast<double>((QRandomGenerator::global()->generateDouble()*(cstMax - cstMin) + cstMin) * valueList.val) * newSymbol[j];
            cst = double(generateRandomInt(cstMin,cstMax) * valueList.val * newSymbol[j]);
            temp1.append(cst);
        }

        initialCst.append(temp1);

    }


    generateSelectionProbability();
    getLengthChromsome();
}

void airfoilOptimization::sortResult(const QVector<double>&result){
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
    for(int i = 0; i<valueList.eliteNum;i++){
        T.append(P[i]);
    }

    historyResult = T;
    bestSolution.append(P[0]);
    bestIndex = sortArray[0];

    double maxP = max(bestSolution);
    historySolution.append(maxP);
}


void airfoilOptimization::selectionChromosomeSequenceDec(){

    QVector<double>randA;        //轮盘赌决定概率
    QVector<int>randS;           //轮盘赌选择索引
    double tmp;
    for(int i = 0; i < valueList.eliteNum; i++){
        tmp = QRandomGenerator::global()->bounded(1.0);
        randA.append(tmp);
    }

    int c;
    for(int i = 0; i < valueList.eliteNum; i++){
        bool ok = true;
        int b = 0;
        while(ok){
            if(randA[i] >= selectProbability[b] && randA[i] < selectProbability[b + 1])
                ok = false;
            b = b + 1;
        }
        c = b - 1;
        randS.append(c);

    }
    for(int i = 0;i<valueList.eliteNum;i++){
        chromosomeSequenceDec[i] = historyChromosomeSequenceDec[randS[i]];
    }
    for(int i = valueList.eliteNum / 2;i<valueList.eliteNum;i++){
        if(valueList.variation * 2 > QRandomGenerator::global()->bounded(1.0)){
            changeCst(chromosomeSequenceDec[i]);
        }
    }
    sortArray = randS;

}


void airfoilOptimization::updateChromosomeSequenceDec(){

    QVector<QVector<double>>newChromosomeSequenceDec;

    QVector<double>tmp;
    int index;
    int len = valueList.eliteNum;



    if(chromosomeSequenceDec.isEmpty()){
        for(int i = 0; i < len; i++){
            index = sortArray[i];
            tmp = initialCst[index];
            newChromosomeSequenceDec.append(tmp);


        }

    }
    else{
        QVector<QVector<double>>allChromosomeSequenceDec;
        allChromosomeSequenceDec = chromosomeSequenceDec;
        for(int i = 0; i<len;i++ ){
            QVector<double>tmp;
            for(int j = 0; j< valueList.solutionsNum;j++)
                tmp.append(historyChromosomeSequenceDec[i][j]);
            allChromosomeSequenceDec.append(tmp);
        }


        for(int i = 0; i < len; i++){
            index = sortArray[i];
            tmp = allChromosomeSequenceDec[index];
            newChromosomeSequenceDec.append(tmp);
    }

    }


    bestCST = newChromosomeSequenceDec[0];
    chromosomeSequenceDec = newChromosomeSequenceDec;
    historyChromosomeSequenceDec = newChromosomeSequenceDec;


}

void airfoilOptimization::decToBin(){

    int cstVal = static_cast<int>(1 / valueList.val);
    int tmp;
    QString stringTmp;

    QVector<QVector<QString>>newChromosomeSequenceBin;
    checkValue();
    for(int i = 0; i < valueList.eliteNum; i++){
        QVector<QString>newString;
        for(int j = 0; j < valueList.solutionsNum; j++){
            tmp = chromosomeSequenceDec[i][j] * cstVal * symbolArray[j];
            stringTmp = QString::number(tmp,2);

            newString.append(stringTmp);

        }
        newChromosomeSequenceBin.append(newString);
    }

    chromosomeSequenceBin = newChromosomeSequenceBin;
}

void airfoilOptimization::binToDec(){
    int len = sortArray.length();
    int tmp;
    QString stringTmp;
    bool ok;
    double newTemp = 0;
    QVector<QVector<double>>newChromosomeSequenceDec;
    for(int i = 0; i < len;i++){

        QVector<double>newDouble;
        for(int j = 0; j < valueList.solutionsNum;j++){
            stringTmp = chromosomeSequenceBin[i][j];
            tmp = stringTmp.toInt(&ok,2);
            if(ok)
                newTemp = static_cast<double>(tmp) * valueList.val * symbolArray[j];

            newDouble.append(newTemp);

        }
        newChromosomeSequenceDec.append(newDouble);
    }
    chromosomeSequenceDec = newChromosomeSequenceDec;

}



void airfoilOptimization::overlappingOperations(){
    QVector<int>randBeginCst;
    QVector<int>randLengthCst;
    QVector<int>randBegin;
    QVector<int>randLength;
    int step = valueList.eliteNum / 2;
    int beginA, beginB;
    for(int i = 0; i < step; i++){
        beginA = QRandomGenerator::global()->bounded(valueList.solutionsNum / 2 - 1);
        randBeginCst.append(beginA);
        randLengthCst.append(valueList.solutionsNum / 2 - beginA);
        beginB = QRandomGenerator::global()->bounded(lengthChromsome - 1);
        randBegin.append(beginB);
        randLength.append(lengthChromsome - beginB);

    }

    double randA;

    for(int i = 0; i < step; i++){
        randA = QRandomGenerator::global()->bounded(1.0);
        if(randA < valueList.cross){
            int begin,length;

            if(randLengthCst[i] != 0){
                begin = randBeginCst[i];
                length = randLengthCst[i] + begin;
                for(int j = begin; j < length; j++){
                    swapChromosome(randBegin[i],randLength[i],chromosomeSequenceBin[2 * i][j],chromosomeSequenceBin[2 * i + 1][j]);
                    swapChromosome(randBegin[i],randLength[i],chromosomeSequenceBin[2 * i][j + valueList.solutionsNum / 2],chromosomeSequenceBin[2 * i + 1][j + valueList.solutionsNum / 2]);
                }
           }
        }
    }
}

void airfoilOptimization::variationOperations(){

    double randA;
    int randLocation,randBegin;
    for(int i = 0; i < valueList.eliteNum; i++){
        randA = QRandomGenerator::global()->bounded(1.0);
        if(valueList.variation > randA){                         
            randLocation = QRandomGenerator::global()->bounded(valueList.solutionsNum - 1);
            randBegin = QRandomGenerator::global()->bounded(lengthChromsome - 1);
            changeChromosome(randBegin,chromosomeSequenceBin[i][randLocation]);
        }
    }
}
void airfoilOptimization::swapChromosome(const int begin,const int len,QString&a,QString&b){

    QString tmpA = a.mid(begin,len);
    QString tmpB = b.mid(begin,len);
    a.replace(begin,len,tmpB);
    b.replace(begin,len,tmpA);
}

void airfoilOptimization::changeChromosome(const int begin,QString&a){
    QString val = a.mid(begin,1);
    if(val == "1")
        a.replace(begin,1,"0");
    else
        a.replace(begin,1,"1");

}
void airfoilOptimization::checkValue(){
    for(int i = 0; i < valueList.eliteNum; i++){
        for(int j = 0; j < valueList.solutionsNum / 2;j++){
            if(chromosomeSequenceDec[i][j] > MaxCstArrayDec[j]){
                chromosomeSequenceDec[i][j] = MaxCstArrayDec[j];
            }else if(chromosomeSequenceDec[i][j] < MinCstArrayDec[j]){
                chromosomeSequenceDec[i][j] = MinCstArrayDec[j];
            }
        }
        for(int j = valueList.solutionsNum / 2; j < valueList.solutionsNum;j++){
            if(chromosomeSequenceDec[i][j] < MaxCstArrayDec[j]){
                chromosomeSequenceDec[i][j] = MaxCstArrayDec[j];
            }else if(chromosomeSequenceDec[i][j] > MinCstArrayDec[j]){
                chromosomeSequenceDec[i][j] = MinCstArrayDec[j];
            }
        }

    }

}
void airfoilOptimization::changeCst(QVector<double>&cst){
    double cstTmp;
    int cstMax, cstMin;
    int cstVal = static_cast<int>(1 / valueList.val);
    QVector<double>temp1;
    for(int j = 0;j < valueList.solutionsNum; j++){
        cstMax = abs(static_cast<int>(MaxCstArrayDec[j] * cstVal));
        cstMin = abs(static_cast<int>(MinCstArrayDec[j] * cstVal));
        //cst = static_cast<double>((QRandomGenerator::global()->generateDouble()*(cstMax - cstMin) + cstMin) * valueList.val) * newSymbol[j];
        cstTmp = double(generateRandomInt(cstMin,cstMax) * valueList.val * symbolArray[j]);
        temp1.append(cstTmp);
    }
    cst = temp1;

}
double airfoilOptimization::max(const QVector<double>&array){



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

double airfoilOptimization::min(const QVector<double>&array){


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



void airfoilOptimization::getLengthChromsome(){
    int cstVal = static_cast<int>(1 / valueList.val);
    int maxValue;
    int max1 = static_cast<int>(max(cstArray) * (1 + valueList.cstRadio) * cstVal);
    int max2 = static_cast<int>(min(cstArray) * -(1 + valueList.cstRadio) * cstVal);


    maxValue = max1;
    if(max1 < max2)
        maxValue = max2;

    QString bin = QString::number(maxValue,2);
    int len = bin.length();
    lengthChromsome = len;

}

void airfoilOptimization::generateSelectionProbability(){
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
int airfoilOptimization::generateRandomInt(int min, int max) {
    // 使用QRandomGenerator生成[min, max]范围内的随机整数
    return QRandomGenerator::global()->bounded(min, max + 1);
}
