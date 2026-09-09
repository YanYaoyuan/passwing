#include "wingoptimization.h"

#include <QRandomGenerator>
#include <QtGlobal>

#include <algorithm>
#include <cmath>
#include <limits>
#include <numeric>
#include <random>

namespace {

bool validProbability(double value)
{
    return std::isfinite(value) && value >= 0.0 && value <= 1.0;
}

bool validResolution(double value)
{
    return std::isfinite(value) && value > 0.0;
}

bool validRange(double first, double second, double resolution)
{
    if (!std::isfinite(first) || !std::isfinite(second) ||
        !validResolution(resolution))
        return false;
    const double width = std::abs(second - first);
    return width / resolution <= std::numeric_limits<int>::max();
}

} // namespace

wingOptimization::wingOptimization(const WingGaParameters &list,
                                   const wingDefinition &wingData)
{
    setModel(list, wingData);
}

void wingOptimization::setModel(const WingGaParameters &list,
                                const wingDefinition &wingData)
{
    valueList = list;
    initialWingData = wingData;
    numN = initialWingData.chordLengthW.size();
}

QString wingOptimization::validationError() const
{
    if (numN < 2)
        return QStringLiteral("机翼至少需要两个截面");
    if (valueList.solutionsNum <= 0 ||
        (valueList.solutionsNum % 2) != 0)
        return QStringLiteral("CST参数数量必须是正偶数");
    if (valueList.initialEliteNum <= 0 || valueList.eliteNum <= 0 ||
        valueList.initialEliteNum < valueList.eliteNum)
        return QStringLiteral("种群数量设置无效");
    if (valueList.step <= 0)
        return QStringLiteral("迭代步数必须大于0");
    if (!std::isfinite(valueList.selection) || valueList.selection <= 0.0)
        return QStringLiteral("轮盘赌指数必须大于0");
    if (!validProbability(valueList.cross) ||
        !validProbability(valueList.variation))
        return QStringLiteral("交叉和变异概率必须在0到1之间");
    if (!std::isfinite(valueList.cstRadio) || valueList.cstRadio <= 0.0 ||
        valueList.cstRadio >= 1.0)
        return QStringLiteral("CST变化倍率必须大于0且小于1");
    if (!validResolution(valueList.val) ||
        !validResolution(valueList.spanVal) ||
        !validResolution(valueList.chordVal) ||
        !validResolution(valueList.offsetLengthVal) ||
        !validResolution(valueList.twisAngleVal) ||
        !validResolution(valueList.dihedralAngleVal))
        return QStringLiteral("编码精度必须大于0");

    const auto hasStations = [this](qsizetype size) {
        return size >= numN;
    };
    if (!hasStations(initialWingData.spanW.size()) ||
        !hasStations(initialWingData.chordLengthW.size()) ||
        !hasStations(initialWingData.offsetLengthW.size()) ||
        !hasStations(initialWingData.twistAngleW.size()) ||
        !hasStations(initialWingData.dihedralAngleW.size()) ||
        !hasStations(initialWingData.cstArray.size()))
        return QStringLiteral("机翼截面数据不完整");

    if (!hasStations(valueList.minChord.size()) ||
        !hasStations(valueList.maxChord.size()) ||
        !hasStations(valueList.minOffsetLength.size()) ||
        !hasStations(valueList.maxOffsetLength.size()) ||
        !hasStations(valueList.minTwisAngle.size()) ||
        !hasStations(valueList.maxTwisAngle.size()) ||
        !hasStations(valueList.minDihedralAngle.size()) ||
        !hasStations(valueList.maxDihedralAngle.size()) ||
        !hasStations(valueList.airfoilChangeArray.size()) ||
        !hasStations(valueList.chordChangeArray.size()) ||
        !hasStations(valueList.offsetChangeArray.size()) ||
        !hasStations(valueList.twistChangeArray.size()) ||
        !hasStations(valueList.dihedChangeArray.size()) ||
        valueList.spanChangeArray.size() < numN - 1)
        return QStringLiteral("机翼优化范围或开关数量不完整");

    if (!std::isfinite(valueList.MinSpan) ||
        !std::isfinite(valueList.MaxSpan) || valueList.MinSpan < 0.0 ||
        valueList.MaxSpan <= 0.0 || fullSpan(initialWingData) <= 0.0)
        return QStringLiteral("翼展范围无效");

    for (int station = 0; station < numN; ++station) {
        if (initialWingData.cstArray[station].size() !=
            valueList.solutionsNum)
            return QStringLiteral("各截面的CST参数数量不一致");

        for (double cst : initialWingData.cstArray[station]) {
            const double first = cst * (1.0 - valueList.cstRadio);
            const double second = cst * (1.0 + valueList.cstRadio);
            if (!validRange(first, second, valueList.val))
                return QStringLiteral("CST编码范围过大或包含无效值");
        }
        if (!validRange(valueList.minChord[station],
                        valueList.maxChord[station], valueList.chordVal) ||
            !validRange(valueList.minOffsetLength[station],
                        valueList.maxOffsetLength[station],
                        valueList.offsetLengthVal) ||
            !validRange(valueList.minTwisAngle[station],
                        valueList.maxTwisAngle[station],
                        valueList.twisAngleVal) ||
            !validRange(valueList.minDihedralAngle[station],
                        valueList.maxDihedralAngle[station],
                        valueList.dihedralAngleVal))
            return QStringLiteral("几何参数范围过大或包含无效值");
    }

    const QVector<double> spans = segmentLengths(initialWingData);
    const double baseSpan = fullSpan(initialWingData);
    const double minRatio = std::min(valueList.MinSpan, valueList.MaxSpan) /
                            baseSpan;
    const double maxRatio = std::max(valueList.MinSpan, valueList.MaxSpan) /
                            baseSpan;
    for (double span : spans) {
        if (!validRange(span * minRatio, span * maxRatio,
                        valueList.spanVal))
            return QStringLiteral("翼展分段编码范围过大或包含无效值");
    }

    return {};
}

bool wingOptimization::initialElite()
{
    if (!validationError().isEmpty())
        return false;

    resetState();
    buildBounds();
    getLengthChromsome();
    generateSelectionProbability();

    initialWingData.spanLengthW = segmentLengths(initialWingData);
    initialWingData.spanW = spanPositions(initialWingData.spanLengthW);
    initialWingData.airfoilInputType = false;

    initialWingDataArray.reserve(valueList.initialEliteNum);
    initialWingDataArray.append(initialWingData);
    for (int i = 1; i < valueList.initialEliteNum; ++i)
        initialWingDataArray.append(randomizedWing());

    return true;
}

void wingOptimization::sortResult(const QVector<double> &result)
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

    bestSolution = historyResult.first();
    bestSolutionArray.append(bestSolution);
    bestIndex = sortArray.first();
}

void wingOptimization::updateChromomeSequenceDec()
{
    QVector<wingDefinition> candidates = wingChromosomeSequenceDec.isEmpty()
        ? initialWingDataArray
        : wingChromosomeSequenceDec + historyWingChromosomeSequenceDec;
    if (sortArray.size() < valueList.eliteNum)
        return;

    QVector<wingDefinition> updatedDefinitions;
    updatedDefinitions.reserve(valueList.eliteNum);
    for (int i = 0; i < valueList.eliteNum; ++i) {
        const int index = sortArray[i];
        if (index < 0 || index >= candidates.size())
            return;
        updatedDefinitions.append(candidates[index]);
    }
    if (updatedDefinitions.size() != valueList.eliteNum)
        return;

    bestWingData = updatedDefinitions.first();
    wingChromosomeSequenceDec = updatedDefinitions;
    historyWingChromosomeSequenceDec = updatedDefinitions;
}

void wingOptimization::selectionChromosomeSequenceDec()
{
    if (historyWingChromosomeSequenceDec.size() != valueList.eliteNum ||
        wingChromosomeSequenceDec.size() != valueList.eliteNum ||
        selectProbability.size() != valueList.eliteNum + 1)
        return;

    QVector<int> selectedIndices;
    selectedIndices.reserve(valueList.eliteNum);
    for (int i = 0; i < valueList.eliteNum; ++i) {
        const double draw = QRandomGenerator::global()->bounded(1.0);
        const auto upper = std::upper_bound(selectProbability.cbegin(),
                                            selectProbability.cend(), draw);
        const int rawIndex = static_cast<int>(
            std::distance(selectProbability.cbegin(), upper)) - 1;
        selectedIndices.append(qBound(0, rawIndex,
                                      valueList.eliteNum - 1));
    }

    for (int i = 0; i < valueList.eliteNum; ++i)
        wingChromosomeSequenceDec[i] =
            historyWingChromosomeSequenceDec[selectedIndices[i]];

    for (int i = valueList.eliteNum / 2; i < valueList.eliteNum; ++i) {
        if (valueList.variation * 2.0 >
            QRandomGenerator::global()->bounded(1.0))
            changeWingData(wingChromosomeSequenceDec[i]);
    }
    sortArray = selectedIndices;
}

void wingOptimization::generateSelectionProbability()
{
    selectProbability.clear();
    selectProbability.reserve(valueList.eliteNum + 1);
    selectProbability.append(0.0);

    QVector<double> logWeights;
    logWeights.reserve(valueList.eliteNum);
    const double logSelection = std::log(valueList.selection);
    for (int i = 0; i < valueList.eliteNum; ++i)
        logWeights.append((valueList.eliteNum - i - 1) * logSelection);

    const double largestLog =
        *std::max_element(logWeights.cbegin(), logWeights.cend());
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

void wingOptimization::decToBin()
{
    if (wingChromosomeSequenceDec.size() != valueList.eliteNum)
        return;

    cstStringArray.clear();
    spanStringArray.clear();
    chordStringArray.clear();
    offsetStringArray.clear();
    twistStringArray.clear();
    dihedStringArray.clear();

    for (const wingDefinition &wing : wingChromosomeSequenceDec) {
        if (wing.cstArray.size() < numN || wing.spanLengthW.size() < numN - 1 ||
            wing.chordLengthW.size() < numN ||
            wing.offsetLengthW.size() < numN ||
            wing.twistAngleW.size() < numN ||
            wing.dihedralAngleW.size() < numN)
            return;

        QVector<QVector<QString>> encodedCst;
        QVector<QString> encodedSpan;
        QVector<QString> encodedChord;
        QVector<QString> encodedOffset;
        QVector<QString> encodedTwist;
        QVector<QString> encodedDihedral;
        for (int station = 0; station < numN; ++station) {
            QVector<QString> stationCst;
            stationCst.reserve(valueList.solutionsNum);
            for (int coefficient = 0;
                 coefficient < valueList.solutionsNum; ++coefficient) {
                stationCst.append(encodeValue(
                    wing.cstArray[station][coefficient],
                    minCstArrayDec[station][coefficient],
                    maxCstArrayDec[station][coefficient], valueList.val,
                    cstLength));
            }
            encodedCst.append(stationCst);
            encodedChord.append(encodeValue(
                wing.chordLengthW[station], valueList.minChord[station],
                valueList.maxChord[station], valueList.chordVal,
                chordLength));
            encodedOffset.append(encodeValue(
                wing.offsetLengthW[station],
                valueList.minOffsetLength[station],
                valueList.maxOffsetLength[station],
                valueList.offsetLengthVal, offsetLength));
            encodedTwist.append(encodeValue(
                wing.twistAngleW[station], valueList.minTwisAngle[station],
                valueList.maxTwisAngle[station], valueList.twisAngleVal,
                twistLength));
            encodedDihedral.append(encodeValue(
                wing.dihedralAngleW[station],
                valueList.minDihedralAngle[station],
                valueList.maxDihedralAngle[station],
                valueList.dihedralAngleVal, dihedralLength));
        }
        for (int segment = 0; segment < numN - 1; ++segment) {
            encodedSpan.append(encodeValue(
                wing.spanLengthW[segment], valueList.minSpan[segment],
                valueList.maxSpan[segment], valueList.spanVal, spanLength));
        }

        cstStringArray.append(encodedCst);
        spanStringArray.append(encodedSpan);
        chordStringArray.append(encodedChord);
        offsetStringArray.append(encodedOffset);
        twistStringArray.append(encodedTwist);
        dihedStringArray.append(encodedDihedral);
    }
}

void wingOptimization::overlappingOperations()
{
    crossCst();
    crossMatrix(spanStringArray, valueList.spanChangeArray,
                numN - 1, spanLength);
    crossMatrix(chordStringArray, valueList.chordChangeArray,
                numN, chordLength);
    crossMatrix(offsetStringArray, valueList.offsetChangeArray,
                numN, offsetLength);
    crossMatrix(twistStringArray, valueList.twistChangeArray,
                numN, twistLength);
    crossMatrix(dihedStringArray, valueList.dihedChangeArray,
                numN, dihedralLength);
}

void wingOptimization::variationOperations()
{
    mutateCst();
    mutateMatrix(spanStringArray, valueList.spanChangeArray,
                 numN - 1, spanLength);
    mutateMatrix(chordStringArray, valueList.chordChangeArray,
                 numN, chordLength);
    mutateMatrix(offsetStringArray, valueList.offsetChangeArray,
                 numN, offsetLength);
    mutateMatrix(twistStringArray, valueList.twistChangeArray,
                 numN, twistLength);
    mutateMatrix(dihedStringArray, valueList.dihedChangeArray,
                 numN, dihedralLength);
}

void wingOptimization::binToDec()
{
    if (cstStringArray.size() != valueList.eliteNum ||
        spanStringArray.size() != valueList.eliteNum ||
        chordStringArray.size() != valueList.eliteNum ||
        offsetStringArray.size() != valueList.eliteNum ||
        twistStringArray.size() != valueList.eliteNum ||
        dihedStringArray.size() != valueList.eliteNum)
        return;

    QVector<wingDefinition> decodedPopulation;
    decodedPopulation.reserve(valueList.eliteNum);
    for (int individual = 0; individual < valueList.eliteNum; ++individual) {
        wingDefinition wing = initialWingData;
        QVector<double> spans = segmentLengths(initialWingData);
        bool ok = true;

        for (int station = 0; station < numN; ++station) {
            if (valueList.airfoilChangeArray[station]) {
                for (int coefficient = 0;
                     coefficient < valueList.solutionsNum; ++coefficient) {
                    wing.cstArray[station][coefficient] = decodeValue(
                        cstStringArray[individual][station][coefficient],
                        minCstArrayDec[station][coefficient],
                        maxCstArrayDec[station][coefficient], valueList.val,
                        &ok);
                    if (!ok)
                        return;
                }
            }
            if (valueList.chordChangeArray[station]) {
                wing.chordLengthW[station] = decodeValue(
                    chordStringArray[individual][station],
                    valueList.minChord[station],
                    valueList.maxChord[station], valueList.chordVal, &ok);
            }
            if (valueList.offsetChangeArray[station]) {
                wing.offsetLengthW[station] = decodeValue(
                    offsetStringArray[individual][station],
                    valueList.minOffsetLength[station],
                    valueList.maxOffsetLength[station],
                    valueList.offsetLengthVal, &ok);
            }
            if (valueList.twistChangeArray[station]) {
                wing.twistAngleW[station] = decodeValue(
                    twistStringArray[individual][station],
                    valueList.minTwisAngle[station],
                    valueList.maxTwisAngle[station], valueList.twisAngleVal,
                    &ok);
            }
            if (valueList.dihedChangeArray[station]) {
                wing.dihedralAngleW[station] = decodeValue(
                    dihedStringArray[individual][station],
                    valueList.minDihedralAngle[station],
                    valueList.maxDihedralAngle[station],
                    valueList.dihedralAngleVal, &ok);
            }
            if (!ok)
                return;
        }

        for (int segment = 0; segment < numN - 1; ++segment) {
            if (valueList.spanChangeArray[segment]) {
                spans[segment] = decodeValue(
                    spanStringArray[individual][segment],
                    valueList.minSpan[segment], valueList.maxSpan[segment],
                    valueList.spanVal, &ok);
                if (!ok)
                    return;
            }
        }

        wing.spanLengthW = spans;
        wing.spanW = spanPositions(spans);
        wing.airfoilInputType = false;
        decodedPopulation.append(wing);
    }

    wingChromosomeSequenceDec = decodedPopulation;
}

void wingOptimization::resetState()
{
    maxCstArrayDec.clear();
    minCstArrayDec.clear();
    cstStringArray.clear();
    spanStringArray.clear();
    chordStringArray.clear();
    offsetStringArray.clear();
    twistStringArray.clear();
    dihedStringArray.clear();
    initialWingDataArray.clear();
    wingChromosomeSequenceDec.clear();
    historyWingChromosomeSequenceDec.clear();
    historyResult.clear();
    selectProbability.clear();
    sortArray.clear();
    bestSolutionArray.clear();
    bestWingData = {};
    bestSolution = 0.0;
    bestIndex = -1;
}

void wingOptimization::buildBounds()
{
    for (int station = 0; station < numN; ++station) {
        QVector<double> minimums;
        QVector<double> maximums;
        minimums.reserve(valueList.solutionsNum);
        maximums.reserve(valueList.solutionsNum);
        for (double cst : initialWingData.cstArray[station]) {
            const double first = cst * (1.0 - valueList.cstRadio);
            const double second = cst * (1.0 + valueList.cstRadio);
            minimums.append(std::min(first, second));
            maximums.append(std::max(first, second));
        }
        minCstArrayDec.append(minimums);
        maxCstArrayDec.append(maximums);
    }

    const QVector<double> spans = segmentLengths(initialWingData);
    const double baseSpan = fullSpan(initialWingData);
    const double minRatio = std::min(valueList.MinSpan, valueList.MaxSpan) /
                            baseSpan;
    const double maxRatio = std::max(valueList.MinSpan, valueList.MaxSpan) /
                            baseSpan;
    valueList.minSpan.clear();
    valueList.maxSpan.clear();
    for (double span : spans) {
        valueList.minSpan.append(span * minRatio);
        valueList.maxSpan.append(span * maxRatio);
    }
}

void wingOptimization::getLengthChromsome()
{
    cstLength = 1;
    for (int station = 0; station < numN; ++station) {
        for (int coefficient = 0;
             coefficient < valueList.solutionsNum; ++coefficient) {
            cstLength = std::max(cstLength, encodedLength(
                minCstArrayDec[station][coefficient],
                maxCstArrayDec[station][coefficient], valueList.val));
        }
    }

    spanLength = chordLength = offsetLength = twistLength =
        dihedralLength = 1;
    for (int segment = 0; segment < numN - 1; ++segment) {
        spanLength = std::max(spanLength, encodedLength(
            valueList.minSpan[segment], valueList.maxSpan[segment],
            valueList.spanVal));
    }
    for (int station = 0; station < numN; ++station) {
        chordLength = std::max(chordLength, encodedLength(
            valueList.minChord[station], valueList.maxChord[station],
            valueList.chordVal));
        offsetLength = std::max(offsetLength, encodedLength(
            valueList.minOffsetLength[station],
            valueList.maxOffsetLength[station],
            valueList.offsetLengthVal));
        twistLength = std::max(twistLength, encodedLength(
            valueList.minTwisAngle[station],
            valueList.maxTwisAngle[station], valueList.twisAngleVal));
        dihedralLength = std::max(dihedralLength, encodedLength(
            valueList.minDihedralAngle[station],
            valueList.maxDihedralAngle[station],
            valueList.dihedralAngleVal));
    }
}

wingDefinition wingOptimization::randomizedWing() const
{
    wingDefinition wing = initialWingData;
    for (int station = 0; station < numN; ++station) {
        if (valueList.airfoilChangeArray[station]) {
            for (int coefficient = 0;
                 coefficient < valueList.solutionsNum; ++coefficient) {
                wing.cstArray[station][coefficient] = randomQuantized(
                    minCstArrayDec[station][coefficient],
                    maxCstArrayDec[station][coefficient], valueList.val);
            }
        }
        if (valueList.chordChangeArray[station]) {
            wing.chordLengthW[station] = randomQuantized(
                valueList.minChord[station], valueList.maxChord[station],
                valueList.chordVal);
        }
        if (valueList.offsetChangeArray[station]) {
            wing.offsetLengthW[station] = randomQuantized(
                valueList.minOffsetLength[station],
                valueList.maxOffsetLength[station],
                valueList.offsetLengthVal);
        }
        if (valueList.twistChangeArray[station]) {
            wing.twistAngleW[station] = randomQuantized(
                valueList.minTwisAngle[station],
                valueList.maxTwisAngle[station], valueList.twisAngleVal);
        }
        if (valueList.dihedChangeArray[station]) {
            wing.dihedralAngleW[station] = randomQuantized(
                valueList.minDihedralAngle[station],
                valueList.maxDihedralAngle[station],
                valueList.dihedralAngleVal);
        }
    }

    QVector<double> spans = segmentLengths(initialWingData);
    for (int segment = 0; segment < numN - 1; ++segment) {
        if (valueList.spanChangeArray[segment]) {
            spans[segment] = randomQuantized(
                valueList.minSpan[segment], valueList.maxSpan[segment],
                valueList.spanVal);
        }
    }
    wing.spanLengthW = spans;
    wing.spanW = spanPositions(spans);
    wing.airfoilInputType = false;
    return wing;
}

void wingOptimization::changeWingData(wingDefinition &wingData)
{
    wingData = randomizedWing();
}

QVector<double> wingOptimization::segmentLengths(
    const wingDefinition &wing) const
{
    QVector<double> spans;
    spans.reserve(std::max(0, numN - 1));
    for (int i = 0; i < numN - 1; ++i)
        spans.append(std::abs(wing.spanW[i + 1] - wing.spanW[i]));
    return spans;
}

QVector<double> wingOptimization::spanPositions(
    const QVector<double> &spans) const
{
    QVector<double> positions;
    positions.reserve(spans.size() + 1);
    double position = 0.0;
    positions.append(position);
    for (double span : spans) {
        position += span;
        positions.append(position);
    }
    return positions;
}

double wingOptimization::fullSpan(const wingDefinition &wing) const
{
    if (wing.spanW.size() < 2)
        return 0.0;
    const double halfOrFull = std::abs(wing.spanW.last() - wing.spanW.first());
    return wing.isSymmetry ? halfOrFull * 2.0 : halfOrFull;
}

QString wingOptimization::encodeValue(double value, double lower, double upper,
                                      double resolution, int length) const
{
    if (lower > upper)
        std::swap(lower, upper);
    value = std::clamp(value, lower, upper);
    const int maxIndex = static_cast<int>(
        std::ceil((upper - lower) / resolution));
    const int index = qBound(0, static_cast<int>(
        std::llround((value - lower) / resolution)), maxIndex);
    return QString::number(index, 2).rightJustified(
        std::max(1, length), QLatin1Char('0'));
}

double wingOptimization::decodeValue(const QString &encoded, double lower,
                                     double upper, double resolution,
                                     bool *ok) const
{
    if (lower > upper)
        std::swap(lower, upper);
    const int index = encoded.toInt(ok, 2);
    if (!*ok)
        return lower;
    return std::clamp(lower + static_cast<double>(index) * resolution,
                      lower, upper);
}

int wingOptimization::encodedLength(double lower, double upper,
                                    double resolution) const
{
    if (lower > upper)
        std::swap(lower, upper);
    const int maxIndex = static_cast<int>(
        std::ceil((upper - lower) / resolution));
    return std::max(1,
                    static_cast<int>(QString::number(maxIndex, 2).size()));
}

void wingOptimization::crossMatrix(QVector<QVector<QString>> &matrix,
                                   const QVector<bool> &changeMask,
                                   int geneCount, int bitLength)
{
    const int pairs = valueList.eliteNum / 2;
    if (pairs <= 0 || geneCount <= 0 || bitLength <= 0 ||
        matrix.size() < pairs * 2 || changeMask.size() < geneCount)
        return;

    for (int pair = 0; pair < pairs; ++pair) {
        if (QRandomGenerator::global()->bounded(1.0) >= valueList.cross)
            continue;
        const int geneBegin = QRandomGenerator::global()->bounded(geneCount);
        const int bitBegin = QRandomGenerator::global()->bounded(bitLength);
        for (int gene = geneBegin; gene < geneCount; ++gene) {
            if (changeMask[gene]) {
                swapChromosome(bitBegin, bitLength - bitBegin,
                               matrix[2 * pair][gene],
                               matrix[2 * pair + 1][gene]);
            }
        }
    }
}

void wingOptimization::mutateMatrix(QVector<QVector<QString>> &matrix,
                                    const QVector<bool> &changeMask,
                                    int geneCount, int bitLength)
{
    if (geneCount <= 0 || bitLength <= 0 ||
        matrix.size() < valueList.eliteNum ||
        changeMask.size() < geneCount)
        return;
    for (int individual = 0; individual < valueList.eliteNum; ++individual) {
        for (int gene = 0; gene < geneCount; ++gene) {
            if (changeMask[gene] &&
                QRandomGenerator::global()->bounded(1.0) <
                    valueList.variation) {
                const int bit = QRandomGenerator::global()->bounded(bitLength);
                changeChromosome(bit, matrix[individual][gene]);
            }
        }
    }
}

void wingOptimization::crossCst()
{
    const int pairs = valueList.eliteNum / 2;
    const int halfCst = valueList.solutionsNum / 2;
    if (pairs <= 0 || halfCst <= 0 || cstLength <= 0 ||
        cstStringArray.size() < pairs * 2)
        return;

    for (int pair = 0; pair < pairs; ++pair) {
        if (QRandomGenerator::global()->bounded(1.0) >= valueList.cross)
            continue;
        const int stationBegin = QRandomGenerator::global()->bounded(numN);
        const int coefficientBegin =
            QRandomGenerator::global()->bounded(halfCst);
        const int bitBegin = QRandomGenerator::global()->bounded(cstLength);
        for (int station = stationBegin; station < numN; ++station) {
            if (!valueList.airfoilChangeArray[station])
                continue;
            for (int coefficient = coefficientBegin;
                 coefficient < halfCst; ++coefficient) {
                swapChromosome(
                    bitBegin, cstLength - bitBegin,
                    cstStringArray[2 * pair][station][coefficient],
                    cstStringArray[2 * pair + 1][station][coefficient]);
                swapChromosome(
                    bitBegin, cstLength - bitBegin,
                    cstStringArray[2 * pair][station][coefficient + halfCst],
                    cstStringArray[2 * pair + 1][station]
                                  [coefficient + halfCst]);
            }
        }
    }
}

void wingOptimization::mutateCst()
{
    if (cstStringArray.size() < valueList.eliteNum || cstLength <= 0)
        return;
    for (int individual = 0; individual < valueList.eliteNum; ++individual) {
        for (int station = 0; station < numN; ++station) {
            if (valueList.airfoilChangeArray[station] &&
                QRandomGenerator::global()->bounded(1.0) <
                    valueList.variation) {
                const int coefficient = QRandomGenerator::global()->bounded(
                    valueList.solutionsNum);
                const int bit = QRandomGenerator::global()->bounded(cstLength);
                changeChromosome(
                    bit, cstStringArray[individual][station][coefficient]);
            }
        }
    }
}

void wingOptimization::swapChromosome(int begin, int length,
                                      QString &first, QString &second)
{
    if (begin < 0 || length <= 0 || begin + length > first.size() ||
        begin + length > second.size())
        return;
    const QString firstBits = first.mid(begin, length);
    const QString secondBits = second.mid(begin, length);
    first.replace(begin, length, secondBits);
    second.replace(begin, length, firstBits);
}

void wingOptimization::changeChromosome(int position, QString &value)
{
    if (position < 0 || position >= value.size())
        return;
    value[position] = value[position] == QLatin1Char('1')
        ? QLatin1Char('0') : QLatin1Char('1');
}

double wingOptimization::randomQuantized(double lower, double upper,
                                         double resolution) const
{
    if (lower > upper)
        std::swap(lower, upper);
    const int maxIndex = static_cast<int>(
        std::ceil((upper - lower) / resolution));
    const int index = generateRandomInt(0, maxIndex);
    return std::clamp(lower + static_cast<double>(index) * resolution,
                      lower, upper);
}

int wingOptimization::generateRandomInt(int min, int max) const
{
    if (min > max)
        std::swap(min, max);
    std::uniform_int_distribution<int> distribution(min, max);
    return distribution(*QRandomGenerator::global());
}
