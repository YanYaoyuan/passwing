#ifndef WINGOPTIMIZATION_H
#define WINGOPTIMIZATION_H

#include <QString>
#include <QVector>

#include "wing/wingdefinition.h"

struct WingGaParameters
{
    double val = 0.000001;                  // CST编码精度
    double variation = 0.05;                // 变异概率
    double selection = 1.4;                 // 轮盘赌指数
    double cross = 0.95;                    // 交叉概率
    int initialEliteNum = 40;               // 初始种群
    int eliteNum = 20;                      // 精英种群
    int step = 10;                          // 迭代步数
    int solutionsNum = 24;                  // 单个翼型的CST参数数量

    double MaxSpan = 0.0;
    double MinSpan = 0.0;
    QVector<double> minSpan;
    QVector<double> maxSpan;
    QVector<double> minChord;
    QVector<double> maxChord;
    QVector<double> minOffsetLength;
    QVector<double> maxOffsetLength;
    QVector<double> minTwisAngle;
    QVector<double> maxTwisAngle;
    QVector<double> minDihedralAngle;
    QVector<double> maxDihedralAngle;
    double spanVal = 0.01;
    double chordVal = 0.01;
    double offsetLengthVal = 0.01;
    double twisAngleVal = 0.01;
    double dihedralAngleVal = 0.01;
    QVector<bool> airfoilChangeArray;
    QVector<bool> spanChangeArray;
    QVector<bool> chordChangeArray;
    QVector<bool> offsetChangeArray;
    QVector<bool> twistChangeArray;
    QVector<bool> dihedChangeArray;

    double cstRadio = 0.5;
};

class wingOptimization
{
public:
    wingOptimization(const WingGaParameters &, const wingDefinition &);
    wingOptimization() = default;

    int bestIndex = -1;
    WingGaParameters valueList;
    wingDefinition initialWingData;
    wingDefinition bestWingData;
    QVector<double> bestSolutionArray;
    double bestSolution = 0.0;
    QVector<wingDefinition> initialWingDataArray;
    QVector<int> sortArray;
    QVector<wingDefinition> wingChromosomeSequenceDec;

    void setModel(const WingGaParameters &, const wingDefinition &);
    QString validationError() const;
    bool initialElite();
    void sortResult(const QVector<double> &);
    void updateChromomeSequenceDec();
    void selectionChromosomeSequenceDec();
    void decToBin();
    void overlappingOperations();
    void variationOperations();
    void binToDec();

private:
    int numN = 0;
    int cstLength = 1;
    int spanLength = 1;
    int chordLength = 1;
    int offsetLength = 1;
    int twistLength = 1;
    int dihedralLength = 1;

    QVector<QVector<double>> maxCstArrayDec;
    QVector<QVector<double>> minCstArrayDec;

    QVector<QVector<QVector<QString>>> cstStringArray;
    QVector<QVector<QString>> spanStringArray;
    QVector<QVector<QString>> chordStringArray;
    QVector<QVector<QString>> offsetStringArray;
    QVector<QVector<QString>> twistStringArray;
    QVector<QVector<QString>> dihedStringArray;

    QVector<wingDefinition> historyWingChromosomeSequenceDec;
    QVector<double> historyResult;
    QVector<double> selectProbability;

    void resetState();
    void buildBounds();
    void getLengthChromsome();
    void generateSelectionProbability();
    void changeWingData(wingDefinition &);

    wingDefinition randomizedWing() const;
    QVector<double> segmentLengths(const wingDefinition &) const;
    QVector<double> spanPositions(const QVector<double> &) const;
    double fullSpan(const wingDefinition &) const;

    QString encodeValue(double value, double lower, double upper,
                        double resolution, int length) const;
    double decodeValue(const QString &encoded, double lower, double upper,
                       double resolution, bool *ok) const;
    int encodedLength(double lower, double upper, double resolution) const;

    void crossMatrix(QVector<QVector<QString>> &, const QVector<bool> &,
                     int geneCount, int bitLength);
    void mutateMatrix(QVector<QVector<QString>> &, const QVector<bool> &,
                      int geneCount, int bitLength);
    void crossCst();
    void mutateCst();
    void swapChromosome(int begin, int length, QString &, QString &);
    void changeChromosome(int position, QString &);

    double randomQuantized(double lower, double upper,
                           double resolution) const;
    int generateRandomInt(int min, int max) const;
};

#endif // WINGOPTIMIZATION_H
