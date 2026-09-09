# PassWing

PassWing 是一套基于 Qt/C++ 的飞行器气动设计与分析桌面软件，覆盖翼型、机翼、
飞机和螺旋桨等设计对象，并提供数据可视化、网格和流场查看功能。

项目现已采用现代 CMake 构建，支持 Windows 和 Linux，兼容 Qt 5/Qt 6，代码标准为
C++17。

## 主要功能

- 翼型导入、设计、分析、优化及 XFoil 计算
- 机翼定义、VLM 分析和优化
- 飞机布局、气动显示及稳定性分析
- 螺旋桨设计、BEMT/VLM 分析和结果展示
- 基于 VTK 的三维模型、网格及流场可视化
- HDF5 工程数据读写
- 云计算功能预告页面

## 技术栈

- C++17
- CMake 3.21+
- Qt 5.14+ 或 Qt 6（Widgets、Charts、OpenGL 等）
- VTK 9
- HDF5
- Eigen 3.3+
- Ninja（推荐）

## 快速开始

首先安装对应平台的依赖，完整说明见 [BUILDING.md](BUILDING.md)。

### Linux

Ubuntu 22.04 可安装以下依赖：

```bash
sudo apt-get update
sudo apt-get install build-essential cmake ninja-build qtbase5-dev \
  libqt5charts5-dev libqt5opengl5-dev qttools5-dev \
  libeigen3-dev libhdf5-dev libvtk9-dev libvtk9-qt-dev
```

配置、编译并启动：

```bash
cmake --preset release -DPASSWING_ENABLE_WEBENGINE=OFF
cmake --build --preset release --parallel
./out/build/release/PassWing
```

### Windows

推荐使用 MSYS2 MinGW64，并确保所有依赖均来自同一套 MinGW64 工具链：

```bash
pacman -S --needed mingw-w64-x86_64-gcc mingw-w64-x86_64-cmake \
  mingw-w64-x86_64-ninja \
  mingw-w64-x86_64-qt6-base mingw-w64-x86_64-qt6-charts \
  mingw-w64-x86_64-qt6-tools mingw-w64-x86_64-eigen3 \
  mingw-w64-x86_64-exprtk mingw-w64-x86_64-fast_float \
  mingw-w64-x86_64-hdf5 mingw-w64-x86_64-nlohmann-json \
  mingw-w64-x86_64-utf8cpp mingw-w64-x86_64-vtk
```

在 MSYS2 MinGW64 终端中执行：

```bash
cmake --preset release -DPASSWING_ENABLE_WEBENGINE=OFF
cmake --build --preset release --parallel
./out/build/release/PassWing.exe
```

如已安装 Qt WebEngine，可去掉 `-DPASSWING_ENABLE_WEBENGINE=OFF`。未安装时程序会使用
`QTextBrowser` 作为 HTML 页面回退方案。

已有 MSVC 依赖环境时，也可以直接双击仓库根目录的 `build-windows.bat`。脚本会使用
Visual Studio x64 Release 模式完成配置、编译和运行库部署，输出目录为
`out/package/PassWing-Windows-x64`，同时生成同名 ZIP 包。本机依赖路径与默认值不同时，
可在 PowerShell 中覆盖参数：

```powershell
.\scripts\build-windows.ps1 `
  -QtRoot "C:\path\to\Qt" `
  -VtkRoot "C:\path\to\VTK" `
  -Hdf5Root "C:\path\to\HDF5" `
  -EigenRoot "C:\path\to\Eigen3"
```

GitHub Actions 的 Linux 和 Windows 任务也会生成完整的可分发包，并分别上传
`PassWing-Linux-x64` 和 `PassWing-Windows-x64` 成果物。包内包含程序、Qt 插件、
VTK/HDF5 等运行库，以及 `setting`、`resoure`、`theoreticalFramework`、
`libaries`、`help` 和 `Profili.mdb`（仓库中存在时）。

## 开发

Debug 构建：

```bash
cmake --preset debug -DPASSWING_ENABLE_WEBENGINE=OFF
cmake --build --preset debug --parallel
```

仓库提供以下工程化配置：

- `.clang-format`：统一 C++ 排版
- `.clang-tidy`：静态检查与现代化建议
- `.editorconfig`：统一编辑器基础格式
- `.github/workflows/build.yml`：Windows/Linux 持续集成构建

如需将编译器警告视为错误，可在配置时增加：

```bash
-DPASSWING_WARNINGS_AS_ERRORS=ON
```

## 目录结构

```text
src/
  app/                  程序入口与主窗口
  airfoil/              翼型设计、分析与优化
  wing/                 机翼设计、VLM 与优化
  aircraft/             飞机设计、稳定性与整机分析
  propeller/            螺旋桨设计、BEMT 与 VLM
  common/               公共数据、数学、文件和绘图设施
  widgets/              可复用 Qt 控件
  geometry/             几何模型及 VTK 加载
  dynamics/             动力学模型
  chat/                 对话界面
  help/                 桌面帮助集成
cmake/                  依赖发现和编译选项
resoure/                Qt 资源文件（保留历史拼写）
libaries/               翼型与螺旋桨数据（保留历史拼写）
setting/                运行配置
help/                   HTML 帮助及媒体素材
theoreticalFramework/   理论说明文档
```

模块边界、依赖方向和新增代码约定见
[docs/ARCHITECTURE.md](docs/ARCHITECTURE.md)。

## 运行注意事项

- 部分模型库和算例依赖外部数据文件；若未随仓库提供，相应功能会提示文件缺失，
  但不影响主程序启动。
- Windows 下不要混用 MSVC 与 MinGW 编译的库。

## 构建验证

持续集成会在以下环境执行 Release 构建：

- Ubuntu 22.04 + Qt 5
- Windows Server 2022 + MSYS2 MinGW64 + Qt 6
