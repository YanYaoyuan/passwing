#include "cfddatareader.h"
#include <QDebug>
#include <vector>

using namespace H5;

cfdDataReader::cfdDataReader() : file(nullptr) {}

// 打开 HDF5 文件
bool cfdDataReader::openFile(const QString &filePath)
{

    try {
        std::string path = filePath.toStdString();

        file = std::make_unique<H5::H5File>(
            path,
            H5F_ACC_RDONLY
        );

        qDebug() << "Successfully opened HDF5 file:" << filePath;
        return true;
    }
    catch (H5::FileIException &e) {
        qDebug() << "File open error:" << e.getCDetailMsg();
    }
    catch (H5::Exception &e) {
        qDebug() << "HDF5 error:" << e.getCDetailMsg();
    }
    catch (std::exception &e) {
        qDebug() << "Std exception:" << e.what();
    }

    return false;
}


// ------------------ 读取二维几何参数 ------------------
QVector<QVector<double>> cfdDataReader::readCST()
{
    if(!file) return {};
    QVector<QVector<double>> data;
    try {
        DataSet ds = file->openDataSet("/shape/cst");
        DataSpace space = ds.getSpace();
        hsize_t dims[2], maxdims[2];
        int rank = space.getSimpleExtentDims(dims, maxdims);
        if(rank != 2) return {};

        std::vector<double> buffer(dims[0]*dims[1]);
        ds.read(buffer.data(), PredType::NATIVE_DOUBLE);

        data.resize(dims[0]);
        for(hsize_t i=0;i<dims[0];++i){
            data[i].resize(dims[1]);
            for(hsize_t j=0;j<dims[1];++j)
                data[i][j] = buffer[i*dims[1]+j];
        }
    } catch(H5::Exception &e) {
        qDebug() << "Failed to read CST:" << e.getCDetailMsg();
    }
    return data;
}

QVector<QVector<double>> cfdDataReader::readBezier()
{
    if(!file) return {};
    QVector<QVector<double>> data;
    try {
        DataSet ds = file->openDataSet("/shape/bezier");
        DataSpace space = ds.getSpace();
        hsize_t dims[2], maxdims[2];
        int rank = space.getSimpleExtentDims(dims,maxdims);
        if(rank!=2) return {};

        std::vector<double> buffer(dims[0]*dims[1]);
        ds.read(buffer.data(), PredType::NATIVE_DOUBLE);

        data.resize(dims[0]);
        for(hsize_t i=0;i<dims[0];++i){
            data[i].resize(dims[1]);
            for(hsize_t j=0;j<dims[1];++j)
                data[i][j] = buffer[i*dims[1]+j];
        }
    } catch(H5::Exception &e) {
        qDebug() << "Failed to read Bezier:" << e.getCDetailMsg();
    }
    return data;
}

QVector<QVector<double>> cfdDataReader::readGrassmann()
{
    if(!file) return {};
    QVector<QVector<double>> data;
    try {
        DataSet ds = file->openDataSet("/shape/grassmann");
        DataSpace space = ds.getSpace();
        hsize_t dims[2], maxdims[2];
        int rank = space.getSimpleExtentDims(dims,maxdims);
        if(rank!=2) return {};

        std::vector<double> buffer(dims[0]*dims[1]);
        ds.read(buffer.data(), PredType::NATIVE_DOUBLE);

        data.resize(dims[0]);
        for(hsize_t i=0;i<dims[0];++i){
            data[i].resize(dims[1]);
            for(hsize_t j=0;j<dims[1];++j)
                data[i][j] = buffer[i*dims[1]+j];
        }
    } catch(H5::Exception &e) {
        qDebug() << "Failed to read Grassmann:" << e.getCDetailMsg();
    }
    return data;
}

QVector<QVector<QVector<double>>> cfdDataReader::readLandmarks()
{
    if(!file) return {};
    QVector<QVector<QVector<double>>> data;
    try{
        DataSet ds = file->openDataSet("/shape/landmarks");
        DataSpace space = ds.getSpace();
        hsize_t dims[3], maxdims[3];
        int rank = space.getSimpleExtentDims(dims,maxdims);
        if(rank!=3) return {};

        std::vector<double> buffer(dims[0]*dims[1]*dims[2]);
        ds.read(buffer.data(), PredType::NATIVE_DOUBLE);

        data.resize(dims[0]);
        for(hsize_t i=0;i<dims[0];++i){
            data[i].resize(dims[1]);
            for(hsize_t j=0;j<dims[1];++j){
                data[i][j].resize(dims[2]);
                for(hsize_t k=0;k<dims[2];++k)
                    data[i][j][k] = buffer[i*dims[1]*dims[2]+j*dims[2]+k];
            }
        }
    } catch(H5::Exception &e){
        qDebug() << "Failed to read Landmarks:" << e.getCDetailMsg();
    }
    return data;
}

// ------------------ 读取气动系数 ------------------
QVector<double> cfdDataReader::read1DData(const QString &datasetPath)
{
    if(!file) return {};
    QVector<double> data;
    try {
        DataSet ds = file->openDataSet(datasetPath.toStdString());
        DataSpace space = ds.getSpace();
        hsize_t dims[1], maxdims[1];
        int rank = space.getSimpleExtentDims(dims,maxdims);
        if(rank!=1) return {};

        std::vector<double> buffer(dims[0]);
        ds.read(buffer.data(), PredType::NATIVE_DOUBLE);

        data.resize(dims[0]);
        for(hsize_t i=0;i<dims[0];++i)
            data[i] = buffer[i];
    } catch(H5::Exception &e){
        qDebug() << "Failed to read 1D dataset:" << datasetPath
                 << "Error:" << e.getCDetailMsg();
    }
    return data;
}

QVector<double> cfdDataReader::readCL(const QString &alphaGroup) { return read1DData(alphaGroup+"/C_l"); }
QVector<double> cfdDataReader::readCD(const QString &alphaGroup) { return read1DData(alphaGroup+"/C_d"); }
QVector<double> cfdDataReader::readCM(const QString &alphaGroup) { return read1DData(alphaGroup+"/C_m"); }

// ------------------ 读取流场 ------------------
FlowField cfdDataReader::readFlowField(const QString &alphaGroup, int airfoilIndex)
{
    FlowField ff;
    if(!file) return ff;

    QString idx = QString("%1").arg(airfoilIndex, 4, 10, QChar('0'));
    QString groupPath = alphaGroup + "/flow_field/" + idx;

    try{
        Group grp = file->openGroup(groupPath.toStdString());

        auto readArray = [&](const QString &name, QVector<double> &vec){
            DataSet ds = grp.openDataSet(name.toStdString());
            DataSpace space = ds.getSpace();
            hsize_t dims[1], maxdims[1];
            int rank = space.getSimpleExtentDims(dims,maxdims);
            if(rank!=1) return;
            vec.resize(dims[0]);
            ds.read(vec.data(), PredType::NATIVE_DOUBLE);
        };

        readArray("x", ff.x);
        readArray("y", ff.y);
        readArray("rho", ff.rho);
        readArray("rho_u", ff.rho_u);
        readArray("rho_v", ff.rho_v);
        readArray("e", ff.e);
        readArray("omega", ff.omega);

    } catch(H5::Exception &e){
        qDebug() << "Failed to read FlowField:" << groupPath
                 << "Error:" << e.getCDetailMsg();
    }

    return ff;
}



