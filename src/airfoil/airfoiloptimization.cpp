#include "airfoiloptimization.h"
#include <QRandomGenerator>

#include <QtGlobal>

#include <algorithm>
#include <cmath>
#include <limits>
#include <numeric>
#include <random>
airfoilOptimization::airfoilOptimization(const GaParameters &value,
                                         const QVector<double> &CST)
    : cstArray(CST), valueList(value)
{
}

QString airfoilOptimization::validationError() const
{
    if (valueList.solutionsNum <= 0 || valueList.solutionsNum != cstArray.size())
        return QStringLiteral("CST参数数量与优化变量数量不一致");
    if ((valueList.solutionsNum % 2) != 0)
        return QStringLiteral("CST参数数量必须为偶数");
    if (valueList.initialEliteNum <= 0 || valueList.eliteNum <= 0)
        return QStringLiteral("种群数量必须大于0");
    if (valueList.initialEliteNum < valueList.eliteNum)
        return QStringLiteral("初始种群数量不能小于精英种群数量");
    if (valueList.step <= 0)
        return QStringLiteral("迭代步数必须大于0");
    if (!std::isfinite(valueList.val) || valueList.val <= 0.0 ||
        valueList.val > 1.0 ||
        (1.0 / valueList.val) > std::numeric_limits<int>::max())
        return QStringLiteral("数值精度必须在有效范围内");
    if (!std::isfinite(valueList.cstRadio) ||
        valueList.cstRadio <= 0.0 || valueList.cstRadio >= 1.0)
        return QStringLiteral("变量上限必须大于0且小于1");
    if (!std::isfinite(valueList.selection) || valueList.selection <= 0.0)
        return QStringLiteral("轮盘赌指数必须大于0");
    if (!std::isfinite(valueList.cross) ||
        valueList.cross < 0.0 || valueList.cross > 1.0)
        return QStringLiteral("交叉概率必须在0到1之间");
    if (!std::isfinite(valueList.variation) ||
        valueList.variation < 0.0 || valueList.variation > 1.0)
        return QStringLiteral("变异概率必须在0到1之间");

    const double scale = 1.0 / valueList.val;
    for (double value : cstArray) {
        if (!std::isfinite(value))
            return QStringLiteral("CST参数包含无效数值");
        const double largestMagnitude =
            std::abs(value) * (1.0 + valueList.cstRadio) * scale;
        if (largestMagnitude > std::numeric_limits<int>::max())
            return QStringLiteral("CST参数超出当前编码范围");
    }

    return {};
}

bool airfoilOptimization::initialElite()
{
    if (!validationError().isEmpty())
        return false;

    MaxCstArrayDec.clear();
    MinCstArrayDec.clear();
    symbolArray.clear();
    initialCst.clear();
    chromosomeSequenceBin.clear();
    chromosomeSequenceDec.clear();
    sortArray.clear();
    selectProbability.clear();
    bestSolution.clear();
    bestCST.clear();
    historySolution.clear();
    historyResult.clear();
    historyChromosomeSequenceDec.clear();
    bestIndex = -1;

    const int cstVal = static_cast<int>(1.0 / valueList.val);
    for (int i = 0; i < valueList.solutionsNum; ++i) {
        const double boundA = cstArray[i] * (1.0 + valueList.cstRadio);
        const double boundB = cstArray[i] * (1.0 - valueList.cstRadio);
        MaxCstArrayDec.append(std::max(boundA, boundB));
        MinCstArrayDec.append(std::min(boundA, boundB));
        symbolArray.append(cstArray[i] < 0.0 ? -1 : 1);
    }

    initialCst.reserve(valueList.initialEliteNum);
    initialCst.append(cstArray);
    for (int i = 1; i < valueList.initialEliteNum; ++i) {
        QVector<double> chromosome;
        chromosome.reserve(valueList.solutionsNum);
        for (int j = 0; j < valueList.solutionsNum; ++j) {
            const int scaledA =
                std::abs(static_cast<int>(MaxCstArrayDec[j] * cstVal));
            const int scaledB =
                std::abs(static_cast<int>(MinCstArrayDec[j] * cstVal));
            const double cst = static_cast<double>(
                generateRandomInt(scaledA, scaledB)) *
                valueList.val * symbolArray[j];
            chromosome.append(cst);
        }
        initialCst.append(chromosome);
    }

    generateSelectionProbability();
    getLengthChromsome();
    return true;
}

void airfoilOptimization::sortResult(const QVector<double> &result)
{
    QVector<double> combinedResult = result;
    if (!historyResult.isEmpty())
        combinedResult += historyResult;

    if (combinedResult.size() < valueList.eliteNum)
        return;

    const double invalidFitness = std::numeric_limits<double>::lowest();
    for (double &fitness : combinedResult) {
        if (!std::isfinite(fitness))
            fitness = invalidFitness;
    }

    sortArray.resize(combinedResult.size());
    std::iota(sortArray.begin(), sortArray.end(), 0);
    std::stable_sort(sortArray.begin(), sortArray.end(),
                     [&combinedResult](int left, int right) {
                         return combinedResult[left] > combinedResult[right];
                     });

    historyResult.clear();
    historyResult.reserve(valueList.eliteNum);
    for (int i = 0; i < valueList.eliteNum; ++i)
        historyResult.append(combinedResult[sortArray[i]]);

    const double generationBest = historyResult.first();
    bestSolution.append(generationBest);
    bestIndex = sortArray.first();
    historySolution.append(max(bestSolution));
}


void airfoilOptimization::selectionChromosomeSequenceDec()
{
    if (historyChromosomeSequenceDec.size() < valueList.eliteNum ||
        chromosomeSequenceDec.size() < valueList.eliteNum ||
        selectProbability.size() != valueList.eliteNum + 1)
        return;

    QVector<int> selectedIndices;
    selectedIndices.reserve(valueList.eliteNum);
    for (int i = 0; i < valueList.eliteNum; ++i) {
        const double draw = QRandomGenerator::global()->bounded(1.0);
        const auto upper = std::upper_bound(selectProbability.cbegin(),
                                            selectProbability.cend(), draw);
        const int rawIndex =
            static_cast<int>(std::distance(selectProbability.cbegin(), upper)) - 1;
        selectedIndices.append(qBound(0, rawIndex,valueList.eliteNum - 1));
    }

    for (int i = 0; i < valueList.eliteNum; ++i)
        chromosomeSequenceDec[i] =
            historyChromosomeSequenceDec[selectedIndices[i]];

    for (int i = valueList.eliteNum / 2; i < valueList.eliteNum; ++i) {
        if (valueList.variation * 2.0 >
            QRandomGenerator::global()->bounded(1.0))
            changeCst(chromosomeSequenceDec[i]);
    }
    sortArray = selectedIndices;
}


void airfoilOptimization::updateChromosomeSequenceDec(){
    QVector<QVector<double>> candidates = chromosomeSequenceDec.isEmpty()
        ? initialCst
        : chromosomeSequenceDec + historyChromosomeSequenceDec;
    if (sortArray.size() < valueList.eliteNum)
        return;

    QVector<QVector<double>> newChromosomeSequenceDec;
    newChromosomeSequenceDec.reserve(valueList.eliteNum);
    for (int i = 0; i < valueList.eliteNum; ++i) {
        const int index = sortArray[i];
        if (index < 0 || index >= candidates.size())
            return;
        const QVector<double> &candidate = candidates[index];
        if (candidate.size() != valueList.solutionsNum)
            return;
        newChromosomeSequenceDec.append(candidate);
    }

    if (newChromosomeSequenceDec.isEmpty())
        return;

    bestCST = newChromosomeSequenceDec.first();
    chromosomeSequenceDec = newChromosomeSequenceDec;
    historyChromosomeSequenceDec = newChromosomeSequenceDec;
}

void airfoilOptimization::decToBin()
{
    if (chromosomeSequenceDec.size() < valueList.eliteNum ||
        symbolArray.size() != valueList.solutionsNum ||
        lengthChromsome <= 0)
        return;

    const int cstVal = static_cast<int>(1.0 / valueList.val);
    QVector<QVector<QString>> newChromosomeSequenceBin;
    newChromosomeSequenceBin.reserve(valueList.eliteNum);
    checkValue();
    for (int i = 0; i < valueList.eliteNum; ++i) {
        if (chromosomeSequenceDec[i].size() != valueList.solutionsNum)
            return;

        QVector<QString> newString;
        newString.reserve(valueList.solutionsNum);
        for (int j = 0; j < valueList.solutionsNum; ++j) {
            const int magnitude = std::max(
                0, static_cast<int>(chromosomeSequenceDec[i][j] *
                                    cstVal * symbolArray[j]));
            newString.append(QString::number(magnitude, 2)
                                 .rightJustified(lengthChromsome,
                                                 QLatin1Char('0')));
        }
        newChromosomeSequenceBin.append(newString);
    }

    chromosomeSequenceBin = newChromosomeSequenceBin;
}

void airfoilOptimization::binToDec()
{
    if (symbolArray.size() != valueList.solutionsNum)
        return;

    QVector<QVector<double>> newChromosomeSequenceDec;
    newChromosomeSequenceDec.reserve(chromosomeSequenceBin.size());
    for (const QVector<QString> &encodedChromosome : chromosomeSequenceBin) {
        if (encodedChromosome.size() != valueList.solutionsNum)
            return;

        QVector<double> newDouble;
        newDouble.reserve(valueList.solutionsNum);
        for (int j = 0; j < valueList.solutionsNum; ++j) {
            bool ok = false;
            const int magnitude = encodedChromosome[j].toInt(&ok, 2);
            if (!ok)
                return;
            newDouble.append(static_cast<double>(magnitude) * valueList.val *
                             symbolArray[j]);
        }
        newChromosomeSequenceDec.append(newDouble);
    }
    chromosomeSequenceDec = newChromosomeSequenceDec;
    checkValue();
}



void airfoilOptimization::overlappingOperations()
{
    const int halfSolutions = valueList.solutionsNum / 2;
    const int step = valueList.eliteNum / 2;
    if (step <= 0 || halfSolutions <= 0 || lengthChromsome <= 0 ||
        chromosomeSequenceBin.size() < step * 2)
        return;

    QVector<int> randBeginCst;
    QVector<int> randLengthCst;
    QVector<int> randBegin;
    QVector<int> randLength;
    for (int i = 0; i < step; ++i) {
        const int beginA = QRandomGenerator::global()->bounded(halfSolutions);
        randBeginCst.append(beginA);
        randLengthCst.append(halfSolutions - beginA);
        const int beginB = QRandomGenerator::global()->bounded(lengthChromsome);
        randBegin.append(beginB);
        randLength.append(lengthChromsome - beginB);
    }

    for (int i = 0; i < step; ++i) {
        if (QRandomGenerator::global()->bounded(1.0) < valueList.cross) {
            const int end = randBeginCst[i] + randLengthCst[i];
            for (int j = randBeginCst[i]; j < end; ++j) {
                swapChromosome(randBegin[i], randLength[i],
                               chromosomeSequenceBin[2 * i][j],
                               chromosomeSequenceBin[2 * i + 1][j]);
                swapChromosome(randBegin[i], randLength[i],
                               chromosomeSequenceBin[2 * i][j + halfSolutions],
                               chromosomeSequenceBin[2 * i + 1][j + halfSolutions]);
            }
        }
    }
}

void airfoilOptimization::variationOperations()
{
    if (valueList.solutionsNum <= 0 || lengthChromsome <= 0 ||
        chromosomeSequenceBin.size() < valueList.eliteNum)
        return;

    for (int i = 0; i < valueList.eliteNum; ++i) {
        if (valueList.variation > QRandomGenerator::global()->bounded(1.0)) {
            const int location =
                QRandomGenerator::global()->bounded(valueList.solutionsNum);
            const int bit = QRandomGenerator::global()->bounded(lengthChromsome);
            if (chromosomeSequenceBin[i].size() != valueList.solutionsNum)
                return;
            changeChromosome(bit, chromosomeSequenceBin[i][location]);
        }
    }
}
void airfoilOptimization::swapChromosome(const int begin, const int len,
                                         QString &a, QString &b)
{
    if (begin < 0 || len <= 0 || begin + len > a.size() ||
        begin + len > b.size())
        return;

    const QString tmpA = a.mid(begin, len);
    const QString tmpB = b.mid(begin, len);
    a.replace(begin, len, tmpB);
    b.replace(begin, len, tmpA);
}

void airfoilOptimization::changeChromosome(const int begin, QString &a)
{
    if (begin < 0 || begin >= a.size())
        return;

    a[begin] = a[begin] == QLatin1Char('1') ? QLatin1Char('0')
                                            : QLatin1Char('1');
}
void airfoilOptimization::checkValue()
{
    if (MaxCstArrayDec.size() != valueList.solutionsNum ||
        MinCstArrayDec.size() != valueList.solutionsNum)
        return;

    for (QVector<double> &chromosome : chromosomeSequenceDec) {
        if (chromosome.size() != valueList.solutionsNum)
            return;
        for (int j = 0; j < valueList.solutionsNum; ++j) {
            chromosome[j] = qBound(MinCstArrayDec[j], chromosome[j],
                                   MaxCstArrayDec[j]);
        }
    }
}
void airfoilOptimization::changeCst(QVector<double> &cst)
{
    if (MaxCstArrayDec.size() != valueList.solutionsNum ||
        MinCstArrayDec.size() != valueList.solutionsNum ||
        symbolArray.size() != valueList.solutionsNum)
        return;

    const int cstVal = static_cast<int>(1.0 / valueList.val);
    QVector<double> randomizedCst;
    randomizedCst.reserve(valueList.solutionsNum);
    for (int j = 0; j < valueList.solutionsNum; ++j) {
        const int scaledA =
            std::abs(static_cast<int>(MaxCstArrayDec[j] * cstVal));
        const int scaledB =
            std::abs(static_cast<int>(MinCstArrayDec[j] * cstVal));
        randomizedCst.append(static_cast<double>(
            generateRandomInt(scaledA, scaledB)) * valueList.val *
            symbolArray[j]);
    }
    cst = randomizedCst;
}
double airfoilOptimization::max(const QVector<double> &array)
{
    if (array.isEmpty())
        return 0.0;
    return *std::max_element(array.cbegin(), array.cend());
}

double airfoilOptimization::min(const QVector<double> &array)
{
    if (array.isEmpty())
        return 0.0;
    return *std::min_element(array.cbegin(), array.cend());
}

void airfoilOptimization::getLengthChromsome()
{
    const double scale = 1.0 / valueList.val;
    double largestMagnitude = 0.0;
    for (int i = 0; i < MaxCstArrayDec.size(); ++i) {
        largestMagnitude = std::max(largestMagnitude,
                                    std::abs(MaxCstArrayDec[i]) * scale);
        largestMagnitude = std::max(largestMagnitude,
                                    std::abs(MinCstArrayDec[i]) * scale);
    }

    const int encodedMaximum = static_cast<int>(largestMagnitude);
    lengthChromsome = std::max(1, QString::number(encodedMaximum, 2).size());
}

void airfoilOptimization::generateSelectionProbability()
{
    selectProbability.clear();
    selectProbability.reserve(valueList.eliteNum + 1);
    selectProbability.append(0.0);

    QVector<double> logWeights;
    logWeights.reserve(valueList.eliteNum);
    const double logSelection = std::log(valueList.selection);
    for (int i = 0; i < valueList.eliteNum; ++i)
        logWeights.append((valueList.eliteNum - i - 1) * logSelection);

    const double largestLog = max(logWeights);
    QVector<double> weights;
    weights.reserve(valueList.eliteNum);
    double totalWeight = 0.0;
    for (double logWeight : logWeights) {
        const double weight = std::exp(logWeight - largestLog);
        weights.append(weight);
        totalWeight += weight;
    }

    double cumulativeProbability = 0.0;
    for (double weight : weights) {
        cumulativeProbability += weight / totalWeight;
        selectProbability.append(cumulativeProbability);
    }
    selectProbability.last() = 1.0;
}

int airfoilOptimization::generateRandomInt(int min, int max)
{
    if (min > max)
        std::swap(min, max);
    std::uniform_int_distribution<int> distribution(min, max);
    return distribution(*QRandomGenerator::global());
}
