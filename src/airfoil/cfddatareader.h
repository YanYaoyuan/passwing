#ifndef CFDDATAREADER_H
#define CFDDATAREADER_H

#include <QString>
#include <QVector>
#include <memory>

// 引入完整 HDF5 C++ API
#include <H5Cpp.h>

struct FlowField {
    QVector<double> x;
    QVector<double> y;
    QVector<double> rho;
    QVector<double> rho_u;
    QVector<double> rho_v;
    QVector<double> e;
    QVector<double> omega;
};

class cfdDataReader
{
public:
    cfdDataReader();
    ~cfdDataReader() = default;

    bool openFile(const QString &filePath);

    // ------------------ 几何参数 ------------------
    QVector<QVector<double>> readCST();
    QVector<QVector<double>> readBezier();
    QVector<QVector<double>> readGrassmann();
    QVector<QVector<QVector<double>>> readLandmarks();

    // ------------------ 气动系数 ------------------
    QVector<double> readCL(const QString &alphaGroup);
    QVector<double> readCD(const QString &alphaGroup);
    QVector<double> readCM(const QString &alphaGroup);

    // ------------------ 流场数据 ------------------
    FlowField readFlowField(const QString &alphaGroup, int airfoilIndex);

private:
    QVector<double> read1DData(const QString &datasetPath);
    std::unique_ptr<H5::H5File> file;  // HDF5 文件句柄
};

#endif // CFDDATAREADER_H



