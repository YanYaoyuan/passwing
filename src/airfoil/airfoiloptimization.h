#ifndef AIRFOILOPTIMIZATION_H
#define AIRFOILOPTIMIZATION_H
#include <QString>
#include <QVector>
struct GaParameters
{
    double val = 0.00001;                                                   //精度
    double variation = 0.05;                                                //变异概率
    double selection = 1.4;                                                 //轮盘赌指数
    double cross = 0.95;                                                     //交叉概率
    int initialEliteNum = 20;                                                //初始化种群
    int eliteNum = 20;                                                       //精英种群
    int step = 10;                                                           //迭代步数
    int solutionsNum = 0;                                                    //CST参数数量

    double cstRadio = 0.5;                                                   //cst变化倍率
};

class airfoilOptimization
{
public:
    airfoilOptimization(const GaParameters&,const QVector<double>&);

private:



    QVector<QVector<QString>>chromosomeSequenceBin;                      //二进制cst参数
    QVector<int>symbolArray;                                             //cst参数符号

    QVector<double>MaxCstArrayDec;
    QVector<double>MinCstArrayDec;
    // QVector<QString>MaxCstArrayBin;
    // QVector<QString>MinCstArrayBin;

    QVector<double>cstArray;                                             //原cst参数
    QVector<double>oldResultArray;
    QVector<double>newResultArray;
    QVector<int>sortArray;                                               //排序后原索引
    QVector<double>selectProbability;                                    //选中概率

public:
    QVector<QVector<double>>initialCst;                                  //初始cst种群
    QVector<double>bestSolution;
    QVector<double>bestCST;
    int bestIndex = -1;
    QVector<double>historySolution;
    QVector<double>historyResult;
    QVector<QVector<double>>historyChromosomeSequenceDec;
    GaParameters valueList;
    int lengthChromsome = 1;                                                //染色体长度
    QVector<QVector<double>>chromosomeSequenceDec;                       //十进制cst参数






    void sortResult(const QVector<double>&);    //对结果进行排序
    void updateChromosomeSequenceDec();         //更新排序后CST十进制的值
    void selectionChromosomeSequenceDec();      //轮盘赌后CST十进制的值
    void overlappingOperations();               //交叉操作
    void variationOperations();                 //变异操作
    void changeCst(QVector<double>&);           //



    //once

    bool initialElite();                        //初始化CST参数
    QString validationError() const;
    void getLengthChromsome();                 //获取最大染色体长度
    void generateSelectionProbability();      //生成选中概率区间（轮盘赌）



    //fuction
    double max(const QVector<double>&);
    double min(const QVector<double>&);
    void binToDec();
    void decToBin();
    void swapChromosome(const int,const int,QString&,QString&);          //染色体交换操作
    void changeChromosome(const int,QString&);                           //染色体变异操作
    void checkValue();



private:
    int generateRandomInt(int min, int max);
    template<class T>
    void swap(T&a, T&b){
        T temp = a;
        a = b;
        b = temp;
    }

    //void initial



};

#endif // AIRFOILOPTIMIZATION_H
