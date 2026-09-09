#ifndef PLTREADER_H
#define PLTREADER_H

#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <QStringList>
#include <unordered_map>

#include <vtkSmartPointer.h>
#include <vtkPoints.h>
#include <vtkFloatArray.h>
#include <vtkUnstructuredGrid.h>
#include <vtkIdList.h>
#include <vtkPointData.h>
#include <vtkMultiBlockDataSet.h>
#include <vtkCellType.h>
#include <ctime>
#include <vtkHexahedron.h>
#include <vtkArrayCalculator.h>
#include <QString>
struct ZoneInfo {
    std::string title;
    int nodes = 0;
    int elements = 0;
    std::string cellType;
    std::string datType;
    double solutionTime = 0.0;
    int strandId = 0;
};
struct CellInfo {
    int vtkType = 0;           // VTK 单元类型
    int pointsPerCell = 0;     // 每个单元节点数
};
class pltReader
{
private:
    std::string fileName;

    // Helper functions:

    void pointsReader(int pointId, const std::string& line, int varNum,
                      std::vector<vtkSmartPointer<vtkFloatArray>>& zoneData,
                      vtkPoints* thePoints);
    void readCells(std::ifstream& file, ZoneInfo z,
                              vtkSmartPointer<vtkUnstructuredGrid> ug);

    bool readVarCount(std::ifstream& file, int varNum, std::vector<float>& result);

    CellInfo getCellInfo(const std::string& cellType);
public:
    pltReader(const std::string& fileName) : fileName(fileName) {}
    vtkSmartPointer<vtkMultiBlockDataSet> readTecplot();
    QStringList getValueName(){return valueNameArray;}

private:
    ZoneInfo parseZone(const std::string& line);
    QStringList valueNameArray;



     // ---------------- 新拆分的小函数 ----------------
     std::string readTitle(std::ifstream& file);
     std::vector<std::string> readVariables(std::ifstream& file);
     ZoneInfo readZone(std::ifstream& file, std::string& firstDataLine);
     void readPointData(std::ifstream& file, const ZoneInfo& z,
                           const std::vector<std::string>& varName,
                           vtkSmartPointer<vtkPoints> thePoints,
                           std::vector<vtkSmartPointer<vtkFloatArray>>& zoneData,
                           const std::string& firstLine);
     void readBlockPacking(
             std::ifstream& file,
             const ZoneInfo& z,
             const std::vector<std::string>& varName,
             vtkPoints* thePoints,
             std::vector<vtkSmartPointer<vtkFloatArray>>& zoneData,
             const std::string& firstLine);
     vtkSmartPointer<vtkUnstructuredGrid> readCells(std::ifstream& file,
                                                       const ZoneInfo& z,
                                                       const std::vector<vtkSmartPointer<vtkFloatArray>>& zoneData,
                                                       vtkSmartPointer<vtkPoints> thePoints);


};

#endif // PLTREADER_H
