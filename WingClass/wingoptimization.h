#ifndef WINGOPTIMIZATION_H
#define WINGOPTIMIZATION_H
#include <QVector>
#include "WingClass/wingdefinition.h"
#include "AirfoilClass/airfoildesign.h"
struct WingGaParameters
{
    double val;                                                             //精度
    double variation;                                                       //变异概率
    double selection;                                                       //轮盘赌指数
    double cross;                                                           //交叉概率
    int initialEliteNum;                                                    //初始化种群
    int eliteNum;                                                           //精英种群
    int step;                                                               //迭代步数
    int solutionsNum;

    double MaxSpan;
    double MinSpan;
    QVector<double>minSpan;
    QVector<double>maxSpan;
    QVector<double>minChord;
    QVector<double>maxChord;
    QVector<double>minOffsetLength;
    QVector<double>maxOffsetLength;
    QVector<double>minTwisAngle;
    QVector<double>maxTwisAngle;
    QVector<double>minDihedralAngle;
    QVector<double>maxDihedralAngle;
    double spanVal;
    double chordVal;
    double offsetLengthVal;
    double twisAngleVal;
    double dihedralAngleVal;
    QVector<bool>airfoilChangeArray;
    QVector<bool>spanChangeArray;
    QVector<bool>chordChangeArray;
    QVector<bool>offsetChangeArray;
    QVector<bool>twistChangeArray;
    QVector<bool>dihedChangeArray;
    // bool airfoilChange = false;
    // bool spanChange = false;
    // bool chordChange = true;
    // bool offsetChange = true;
    // bool twistChange = true;
    // bool dihedChange = false;





    double cstRadio;                                                        //cst变化倍率
};

class wingOptimization
{
public:
    wingOptimization(const WingGaParameters&,const wingDefinition&);
    wingOptimization();

    int bestIndex;
    WingGaParameters valueList;
    wingDefinition initialWingData;
    wingDefinition bestWingData;
    QVector<double>bestSolutionArray;
    double bestSolution;
    QVector<wingDefinition>initialWingDataArray;
    QVector<int>sortArray;                                               //排序后原索引
    QVector<wingDefinition>wingChromosomeSequenceDec;                       //十进制cst参数
    void setModel(const WingGaParameters&,const wingDefinition&);
    void initialElite();
    void sortResult(const QVector<double>&);
    void updateChromomeSequenceDec();
    void selectionChromosomeSequenceDec();
    void decToBin();
    void overlappingOperations();
    void variationOperations();
    void binToDec();



private:

    int numN;
    QVector<QVector<double>>spanArray;
    QVector<QVector<double>>chordArray;
    QVector<QVector<double>>offsetLengthArray;
    QVector<QVector<double>>twistAngleArray;
    QVector<QVector<double>>dihedralAngleArray;
    QVector<QVector<QVector<double>>>cstArray;

    QVector<QVector<int>>symbolCSTArray;                       //cst参数符号
    QVector<QVector<int>>symbolOffsetArray;
    QVector<QVector<int>>symbolTwistArray;
    QVector<QVector<int>>symbolDihedArray;



    QVector<QVector<QVector<QString>>>cstStringArray;
    QVector<QVector<QString>>spanStringArray;
    QVector<QVector<QString>>chordStringArray;
    QVector<QVector<QString>>offsetStringArray;
    QVector<QVector<QString>>twistStringArray;
    QVector<QVector<QString>>dihedStringArray;

    QString maxBin[6];
    QString minBin[6];






    QVector<wingDefinition>historyWingChromosomeSequenceDec;

    int cstLength;
    int spanLength;
    int chordLength;
    int offsetLength;
    int twistLength;
    int dihedralLength;

    QVector<double>allSpan;
    QVector<double>historyResult;

    void getLengthChromsome();
    void generateSelectionProbability();
    void swapChromosome(const int,const int,QString&,QString&);          //染色体交换操作
    void changeChromosome(const int,QString&);                           //染色体变异操作
    void checkValue();
\
    void changeWingData(wingDefinition&);

    //airfoil
    void airfoilDecToBin();
    void airfoilOverlappingOperations();
    void airfoilVariationOperations();
    void airfoilBinToDec();
    //span
    void spanDecToBin();
    void spanOverlappingOperations();
    void spanVariationOperations();
    void spanBinToDec();
    //chord
    void chordDecToBin();
    void chordOverlappingOperations();
    void chordVariationOperations();
    void chordBinToDec();
    //offset
    void offsetDecToBin();
    void offsetOverlappingOperations();
    void offsetVariationOperations();
    void offsetBinToDec();
    //twist
    void twistDecToBin();
    void twistOverlappingOperations();
    void twistVariationOperations();
    void twistBinToDec();
    //dihed
    void dihedDecToBin();
    void dihedOverlappingOperations();
    void dihedVariationOperations();
    void dihedBinToDec();







    QVector<QVector<double>>maxCstArrayDec;//翼型最大cst参数
    QVector<QVector<double>>minCstArrayDec;//翼型最小cst参数

                                               //排序后原索引
    QVector<double>selectProbability;                                    //选中概率

public:







private:
    void restArray();
    double generateRandomDouble(double, double);
    int generateRandomInt(int min, int max);
    double max(const QVector<double>&);
    double min(const QVector<double>&);
    QString decToBinAlone(int,const int);
    template<class T>
    void swap(T&a, T&b){
        T temp = a;
        a = b;
        b = temp;
    }
    int getSign(const double);


};

#endif // WINGOPTIMIZATION_H
