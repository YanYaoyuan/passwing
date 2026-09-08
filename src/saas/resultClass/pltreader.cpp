#include "saas/resultClass/pltreader.h"

// 读取节点数据
void pltReader::pointsReader(int pointId, const std::string& line, int varNum,
                             std::vector<vtkSmartPointer<vtkFloatArray>>& zoneData,
                             vtkPoints* thePoints)
{
    std::istringstream iss(line);
    std::string theValue;
    float x = 0, y = 0, z = 0;

    for (int i = 0; i < varNum; i++)
    {
        if (!(iss >> theValue)) {
            std::cerr << "Error reading variable " << i << " at point " << pointId << " (not enough values in line)" << std::endl;
            break;
        }

        float tmp = 0.0f;
        try {
            tmp = std::stof(theValue);
        } catch (const std::exception& e) {
            std::cerr << "Error converting value '" << theValue
                      << "' to float at variable " << i
                      << ", point " << pointId
                      << ": " << e.what() << std::endl;
            tmp = 0.0f; // 可以选择默认值
        }

        // 确保 zoneData[i] 已经分配了足够大小
        if (pointId >= zoneData[i]->GetNumberOfTuples()) {
            zoneData[i]->SetNumberOfTuples(pointId + 1);
        }

        zoneData[i]->SetValue(pointId, tmp);

        switch (i)
        {
            case 0: x = tmp; break;
            case 1: y = tmp; break;
            case 2: z = tmp; break;
            default: break;
        }
    }

    thePoints->InsertNextPoint(x, y, z);

    // 前 5 个点打印
    if (pointId < 5 && pointId >0) {
        std::cout << "Point " << pointId << ": (" << x << "," << y << "," << z << ") ";
        std::cout << "Variables: ";
        for (int i = 0; i < varNum; i++)
            std::cout << zoneData[i]->GetValue(pointId) << " ";
        std::cout << std::endl;
    }
}


// 读取单元连接
void pltReader::readCells(std::ifstream& file, ZoneInfo z,
                          vtkSmartPointer<vtkUnstructuredGrid> ug)
{
    std::string line;
    int elements = z.elements;
    int printed = 0;

    for (int i = 0; i < elements; i++)
    {
        if (!std::getline(file, line)) {
            std::cerr << "Unexpected EOF reading cells in zone " << z.title << std::endl;
            break;
        }

        auto ids = vtkSmartPointer<vtkIdList>::New();
        std::istringstream iss(line);
        std::string tmp;
        std::vector<int> realcell;
        while (iss >> tmp)
        {
            int theID = std::stoi(tmp) - 1; // TECplot 索引从 1 开始
            realcell.push_back(theID);
            ids->InsertNextId(theID);
        }

        // 根据节点数决定 cell 类型
        switch (realcell.size())
        {
        case 3: ug->InsertNextCell(VTK_TRIANGLE, ids); break;
        case 4: ug->InsertNextCell(VTK_TETRA, ids); break;
        case 5: ug->InsertNextCell(VTK_PYRAMID, ids); break;
        case 6: ug->InsertNextCell(VTK_WEDGE, ids); break;
        case 8: ug->InsertNextCell(VTK_HEXAHEDRON, ids); break;
        default: break;
        }

        // 前 5 个单元打印
        if (printed < 5) {
            std::cout << "Cell " << printed << ": ";
            for (auto id : realcell) std::cout << (id+1) << " ";
            std::cout << std::endl;
            printed++;
        }
    }
}


void pltReader::readBlockPacking(std::ifstream& file,
                                 const ZoneInfo& z,
                                 const std::vector<std::string>& varName,
                                 vtkPoints* thePoints,
                                 std::vector<vtkSmartPointer<vtkFloatArray>>& zoneData,
                                 const std::string& firstLine)
{
    const int N = z.nodes;
    const int V = static_cast<int>(varName.size());

    // 初始化 zoneData
    zoneData.clear();
    for (int v = 0; v < V; ++v) {
        auto arr = vtkSmartPointer<vtkFloatArray>::New();
        arr->SetName(varName[v].c_str());
        arr->SetNumberOfTuples(N);
        zoneData.push_back(arr);
    }



    std::vector<std::vector<double>> varData(V, std::vector<double>(N));

    std::istringstream firstIss(firstLine);



    double val;
    for (int v = 0; v < V; ++v) {
        int count = 0;
        std::string line;
        if(v == 0 && count ==0){
            while (firstIss >> val)
                varData[v][count++] = val;
        }


        while (count < N && std::getline(file, line)) {
            std::istringstream iss(line);



            while (iss >> val) {
                varData[v][count++] = val;
                if (count == N)
                    break;   // ✅ 一够 N，立刻停
            }
        }

        if (count < N) {
            std::cerr << "Warning: variable " << v
                      << " only read " << count << "/" << N << " values\n";
        }
    }



    // 按点调用 pointsReader 赋值
    for (int i = 0; i < N; ++i) {
        std::ostringstream oss;
        for (int v = 0; v < V; ++v)
            oss << varData[v][i] << (v + 1 < V ? ' ' : '\0');

        pointsReader(i, oss.str(), V, zoneData, thePoints);
    }

    std::cout << "BLOCK read: " << N << " points, " << V << " variables." << std::endl;
}



vtkSmartPointer<vtkMultiBlockDataSet> pltReader::readTecplot() {
    if(!valueNameArray.isEmpty())
        valueNameArray.clear();

    std::ifstream file(fileName);
    if (!file.is_open()) {
        std::cerr << "Cannot open file: " << fileName << std::endl;
        throw std::runtime_error("Cannot open file");
    }
    std::cout << "Opened file: " << fileName << std::endl;

    std::string title = readTitle(file);
    std::vector<std::string> varName = readVariables(file);

    std::string firstLine;
    ZoneInfo z = readZone(file, firstLine);

    auto thePoints = vtkSmartPointer<vtkPoints>::New();
    std::vector<vtkSmartPointer<vtkFloatArray>> zoneData;
    if (z.datType == "POINT") {
        readPointData(file, z, varName, thePoints, zoneData, firstLine);
    }
    else if (z.datType == "BLOCK") {
        readBlockPacking(file, z, varName, thePoints, zoneData,firstLine);
    }
    else {
        throw std::runtime_error("Unsupported DATAPACKING");
    }
    vtkSmartPointer<vtkUnstructuredGrid> ug = readCells(file, z, zoneData, thePoints);

    auto multiBlock = vtkSmartPointer<vtkMultiBlockDataSet>::New();
    multiBlock->SetBlock(0, ug);

    std::cout << "Finished reading file." << std::endl;
    return multiBlock;
}



ZoneInfo pltReader::parseZone(const std::string& line) {
    ZoneInfo z;

    // 先把 = 和 , 替换为空格，方便读取
    std::string tmp = line;
    std::replace(tmp.begin(), tmp.end(), '=', ' ');
    std::replace(tmp.begin(), tmp.end(), ',', ' ');
    std::replace(tmp.begin(), tmp.end(), '"', ' ');

    std::istringstream iss(tmp);
    std::string token;

    while (iss >> token) {
        std::string key = token;
        std::transform(key.begin(), key.end(), key.begin(), ::tolower);

        if (key == "t") {
            // title 可能包含空格，用 getline 读取剩余直到下一个关键字
            std::string value;
            if (iss >> value)
                z.title = value; // 可以在外面再做 trim 或替换反斜杠
        }
        else if (key == "strandid") {
            iss >> z.strandId;
        }
        else if (key == "solutiontime") {
            iss >> z.solutionTime;
        }
        else if (key == "nodes" || key == "n") {
            iss >> z.nodes;
        }
        else if (key == "elements" || key == "e") {
            iss >> z.elements;
        }
        else if (key == "zonetype" || key == "et") {
            iss >> z.cellType;
        }
        else if (key == "datapacking") {

            iss >> z.datType; // 这里保存 POINT 或 BLOCK
        }
    }

    return z;
}


bool pltReader::readVarCount(std::ifstream& file, int varNum, std::vector<float>& result)
{
    result.clear();
    result.reserve(varNum);

    while ((int)result.size() < varNum)
    {
        std::string line;
        if (!std::getline(file, line))
            return false;

        if (line.empty())
            continue;

        std::istringstream iss(line);
        float v;

        while (iss >> v)
            result.push_back(v);

        // 如果这一行读不够，会自动继续下一行补
    }

    return true;
}
std::string pltReader::readTitle(std::ifstream& file) {
    std::string line;
    while (std::getline(file, line)) {
        if (line.find("TITLE") != std::string::npos || line.find("title") != std::string::npos) {
            size_t pos1 = line.find("\"");
            size_t pos2 = line.find("\"", pos1 + 1);
            if (pos1 != std::string::npos && pos2 != std::string::npos) {
                std::string title = line.substr(pos1 + 1, pos2 - pos1 - 1);
                std::cout << "File TITLE: " << title << std::endl;
                return title;
            }
        }
    }
    return "";
}

CellInfo pltReader::getCellInfo(const std::string& cellType){
    CellInfo info;
        std::string type = cellType;
        // 小写化方便比较
        std::transform(type.begin(), type.end(), type.begin(), ::tolower);

        if (type == "febrick" || type == "brick") {
            info.vtkType = VTK_HEXAHEDRON;
            info.pointsPerCell = 8;
        } else if (type == "fequadrilateral" || type == "quad") {
            info.vtkType = VTK_QUAD;
            info.pointsPerCell = 4;
        } else if (type == "fetriangle" || type == "triangle") {
            info.vtkType = VTK_TRIANGLE;
            info.pointsPerCell = 3;
        } else if (type == "fetetrahedron" || type == "tetra") {
            info.vtkType = VTK_TETRA;
            info.pointsPerCell = 4;
        } else if (type == "fepyramid" || type == "pyramid") {
            info.vtkType = VTK_PYRAMID;
            info.pointsPerCell = 5;
        } else if (type == "fewedge" || type == "wedge") {
            info.vtkType = VTK_WEDGE;
            info.pointsPerCell = 6;
        } else {
            std::cerr << "Warning: unknown cell type \"" << cellType
                      << "\", defaulting to VTK_HEXAHEDRON with 8 points" << std::endl;
            info.vtkType = VTK_HEXAHEDRON;
            info.pointsPerCell = 8;
        }

        return info;
}
std::vector<std::string> pltReader::readVariables(std::ifstream& file) {
    std::vector<std::string> varName;
    std::string line;
    while (std::getline(file, line)) {
        if (line.find("VARIABLES") != std::string::npos || line.find("variables") != std::string::npos) {
            size_t pos = line.find("=");
            if (pos != std::string::npos) {
                std::string rest = line.substr(pos + 1);
                rest.erase(0, rest.find_first_not_of(" \t\r\n"));

                if (!rest.empty() && rest.front() == '"') {
                    // 多行带引号
                    do {
                        size_t pos1 = rest.find("\"");
                        size_t pos2 = rest.find("\"", pos1 + 1);
                        while (pos1 != std::string::npos && pos2 != std::string::npos) {
                            varName.push_back(rest.substr(pos1 + 1, pos2 - pos1 - 1));
                            pos1 = rest.find("\"", pos2 + 1);
                            pos2 = rest.find("\"", pos1 + 1);
                        }
                        if (!std::getline(file, rest)) break;
                        rest.erase(0, rest.find_first_not_of(" \t\r\n"));
                    } while (!rest.empty() && rest.front() == '"');
                } else {
                    // 单行无引号
                    std::istringstream iss(rest);
                    std::string token;
                    while (iss >> token) varName.push_back(token);
                }
                break;
            }
        }
    }

    for (const auto& s : varName)
        valueNameArray << QString::fromStdString(s);

    return varName;
}

ZoneInfo pltReader::readZone(std::ifstream& file, std::string& firstDataLine) {
    std::string line;
    while (std::getline(file, line)) {
        if (line.empty()) continue;
        if (line.find("ZONE") != std::string::npos || line.find("zone") != std::string::npos) {
            std::string zoneBlock = line;
            while (std::getline(file, line)) {
                std::string tmp = line;
                tmp.erase(0, tmp.find_first_not_of(" \t\r\n"));
                if (tmp.empty()) continue;
                if ((tmp.front() >= '0' && tmp.front() <= '9') || tmp.front() == '-' || tmp.front() == '+') {
                    firstDataLine = line; // 保存第一行节点数据
                    break;
                }
                zoneBlock += " " + tmp;
            }

            ZoneInfo z = parseZone(zoneBlock);
            std::cout << "Zone Title: " << z.title
                      << ", Nodes: " << z.nodes
                      << ", Elements: " << z.elements
                      << ", CellType: " << z.cellType
                      << ", DataPacking: " << z.datType << std::endl;
            return z;
        }
    }
    return ZoneInfo{};
}

void pltReader::readPointData(std::ifstream& file, const ZoneInfo& z,
                              const std::vector<std::string>& varName,
                              vtkSmartPointer<vtkPoints> thePoints,
                              std::vector<vtkSmartPointer<vtkFloatArray>>& zoneData,
                              const std::string& firstLine)
{
    zoneData.clear();
    for (size_t i = 0; i < varName.size(); i++) {
        auto arr = vtkSmartPointer<vtkFloatArray>::New();
        arr->SetNumberOfTuples(z.nodes);
        arr->SetName(varName[i].c_str());
        zoneData.push_back(arr);
    }

    std::vector<std::string> leftover;
    std::istringstream firstIss(firstLine);
    std::string tok;
    while (firstIss >> tok) leftover.push_back(tok);

    for (int pointId = 0; pointId < z.nodes; ++pointId) {
        std::vector<std::string> tokens(leftover);
        leftover.clear();

        while (tokens.size() < varName.size()) {
            std::string newline;
            if (!std::getline(file, newline)) break;
            std::istringstream iss(newline);
            std::string t;
            while (iss >> t) tokens.push_back(t);
        }

        if (tokens.size() < varName.size()) {
            std::cerr << "EOF: not enough values for point " << pointId << '\n';
            break;
        }

        std::ostringstream oss;
        for (int i = 0; i < int(varName.size()); ++i) {
            oss << tokens[i] << (i + 1 < int(varName.size()) ? ' ' : '\0');
        }
        leftover.assign(tokens.begin() + varName.size(), tokens.end());

        pointsReader(pointId, oss.str(), int(varName.size()), zoneData, thePoints);
    }
}

vtkSmartPointer<vtkUnstructuredGrid> pltReader::readCells(std::ifstream& file,
                                                          const ZoneInfo& z,
                                                          const std::vector<vtkSmartPointer<vtkFloatArray>>& zoneData,
                                                          vtkSmartPointer<vtkPoints> thePoints)
{
    CellInfo info = getCellInfo(z.cellType);
    int expectedPointsPerCell = info.pointsPerCell;
    int vtkCellType = info.vtkType;

    std::vector<vtkIdType> pts;
    pts.reserve(expectedPointsPerCell);

    auto ug = vtkSmartPointer<vtkUnstructuredGrid>::New();
    ug->SetPoints(thePoints);
    for (auto& arr : zoneData)
        ug->GetPointData()->AddArray(arr);

    int cellsRead = 0;
    std::string line;
    while (cellsRead < z.elements && std::getline(file, line)) {
        if (line.empty()) continue;
        std::istringstream iss(line);
        vtkIdType idx;
        while (iss >> idx) {
            pts.push_back(idx - 1);
            if ((int)pts.size() == expectedPointsPerCell) {
                ug->InsertNextCell(vtkCellType, expectedPointsPerCell, pts.data());
                pts.clear();
                ++cellsRead;
                if (cellsRead >= z.elements) break;
            }
        }
    }

    if (!pts.empty())
        std::cerr << "Warning: leftover points not forming a full cell!" << std::endl;

    std::cout << "Finished reading zone: " << z.title << ", Nodes: " << z.nodes
              << ", Cells: " << z.elements << std::endl;
    std::cout << "Read " << cellsRead << " cells." << std::endl;

    return ug;
}

