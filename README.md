# ✈️ PassWing

> 🎉 **PassWing 于 2026 年 9 月 9 日正式开源！**
> 感谢每一位关注飞行器设计、空气动力学与科学计算的朋友，欢迎一起探索、使用和改进 PassWing。🚀

PassWing 是一套基于 Qt/C++ 的飞行器气动设计与分析桌面软件，覆盖翼型、机翼、
飞机和螺旋桨等设计对象，并提供数据可视化、网格/流场查看及其他辅助功能。

项目现已采用现代 CMake 构建，支持 Windows 和 Linux，兼容 Qt 5/Qt 6，代码标准为
C++17。

## ✨ 主要功能

- 🪽 翼型导入、设计、分析、优化及 XFoil 计算
- 📐 机翼定义、VLM 分析和优化
- ✈️ 飞机布局、气动显示及稳定性分析
- 🌀 螺旋桨设计、BEMT/VLM 分析和结果展示
- 📊 基于 VTK 的三维模型、网格及流场可视化
- 💾 HDF5 工程数据读写
- ☁️ 云计算功能预告页面

## 🧰 技术栈

- C++17
- CMake 3.21+
- Qt 5.14+ 或 Qt 6（Widgets、Charts、OpenGL 等）
- VTK 9
- HDF5
- Eigen 3.3+
- Ninja（推荐）

## 🚀 快速开始

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

#### 不安装开发环境，直接运行

如果只需要使用 PassWing，不需要在目标电脑上编译。进入仓库的
[GitHub Actions](https://github.com/YanYaoyuan/passwing/actions)，打开最近一次成功的
`build` 流水线，在页面底部下载 `PassWing-Windows-x64` 成果物。完整解压 ZIP 后运行
`PassWing.exe`，不要只复制一个 EXE。成果物包含 Qt 插件、MinGW/VTK/HDF5 运行库，
以及程序需要的 `setting`、`resoure`、`theoreticalFramework`、`libaries`、`help` 和
`Profili.mdb`（仓库中存在时）。

#### 全新电脑从零编译（推荐：MSYS2 MinGW64）

这种方式不要求预先安装 Visual Studio、Qt、VTK、HDF5 或 Eigen；它们都由 MSYS2
统一安装。建议使用 64 位 Windows 10/11，并为工具链和依赖预留至少 10 GB 空间。

1. 从 [MSYS2 官网](https://www.msys2.org/) 安装 MSYS2。
2. 先打开 `MSYS2 MSYS` 终端更新基础系统：

   ```bash
   pacman -Syu
   ```

   如果终端提示关闭窗口，关闭后重新打开 `MSYS2 MSYS`，再次执行 `pacman -Syu`，
   直到没有待更新的软件包。

3. 从开始菜单打开 **MSYS2 MinGW x64** 终端。后续命令必须在这个终端执行，不能使用
   普通 CMD、PowerShell、`MSYS2 MSYS` 或其他编译器终端。
4. 一次性安装编译器、CMake、Qt、VTK、HDF5、Eigen 和其余依赖：

   ```bash
   pacman -S --needed \
     git \
     mingw-w64-x86_64-gcc \
     mingw-w64-x86_64-cmake \
     mingw-w64-x86_64-ninja \
     mingw-w64-x86_64-qt6-base \
     mingw-w64-x86_64-qt6-charts \
     mingw-w64-x86_64-qt6-tools \
     mingw-w64-x86_64-eigen3 \
     mingw-w64-x86_64-exprtk \
     mingw-w64-x86_64-fast_float \
     mingw-w64-x86_64-hdf5 \
     mingw-w64-x86_64-nlohmann-json \
     mingw-w64-x86_64-utf8cpp \
     mingw-w64-x86_64-vtk
   ```

5. 下载代码并进入项目目录：

   ```bash
   git clone https://github.com/YanYaoyuan/passwing.git
   cd passwing
   ```

   如果下载的是源码 ZIP，也可以解压后进入该目录。Windows 的 `D:\work\passwing`
   在 MSYS2 中写作 `/d/work/passwing`。

6. 确认当前使用的是 MinGW64 工具链：

   ```bash
   which gcc g++ cmake ninja
   gcc --version
   cmake --version
   ```

   `which` 输出应以 `/mingw64/bin/` 开头。

7. 配置并编译 Release 版本：

   ```bash
   CC=gcc CXX=g++ cmake --preset release \
     -DPASSWING_ENABLE_WEBENGINE=OFF
   cmake --build --preset release --parallel 2
   ```

   `--parallel 2` 可以降低 VTK/Qt 项目编译时的内存压力；内存充足时可以提高数字。
   编译结果位于 `out/build/release/PassWing.exe`。在开发环境中可直接启动：

   ```bash
   ./out/build/release/PassWing.exe
   ```

8. 如需生成可复制到其他 Windows 电脑的完整 ZIP 包，继续执行：

   ```bash
   package_root=out/package/PassWing-Windows-x64
   rm -rf "$package_root"
   mkdir -p "$package_root"
   cp out/build/release/PassWing.exe "$package_root/PassWing.exe"

   for item in setting resoure theoreticalFramework libaries help Profili.mdb; do
     if [ -e "$item" ]; then
       cp -a "$item" "$package_root/"
     fi
   done

   cp -f /mingw64/bin/*.dll "$package_root/"
   windeployqt6 --release --compiler-runtime --no-translations \
     --dir "$package_root" "$package_root/PassWing.exe"

   cd out/package
   cmake -E tar cf PassWing-Windows-x64.zip --format=zip -- PassWing-Windows-x64
   ```

   最终文件为 `out/package/PassWing-Windows-x64.zip`。应在另一台没有 MSYS2/Qt 的电脑
   上完整解压后测试，至少确认 `PassWing.exe` 能启动且 `platforms/qwindows.dll` 存在。

如已安装 Qt WebEngine，可去掉 `-DPASSWING_ENABLE_WEBENGINE=OFF`；未安装时程序使用
`QTextBrowser` 显示本地 HTML。不要混用 MinGW 与 MSVC 编译的 Qt、VTK 或 HDF5 库。

常见问题：

- `No CMAKE_C_COMPILER could be found`：终端选错；请使用 **MSYS2 MinGW x64**。
- 找不到 Qt、VTK、HDF5 或 Eigen：先完成 `pacman -Syu`，再确认 `cmake` 来自
  `/mingw64/bin/cmake`。
- 双击 EXE 提示缺少 DLL 或 Qt platform plugin：不要单独复制构建目录中的 EXE，
  应使用第 8 步生成的完整目录或下载 GitHub Actions 成果物。
- 曾用其他生成器配置过同一目录：删除 `out/build/release` 后重新执行第 7 步。

#### 已有 MSVC 依赖环境

仓库根目录的 `build-windows.bat` 用于已经安装 Visual Studio C++、MSVC 版 Qt、VTK、
HDF5 和 Eigen 的电脑。它会完成 x64 Release 编译、运行库部署和 ZIP 打包。依赖位置与
脚本默认值不同时，在 PowerShell 中明确传入路径：

```powershell
.\scripts\build-windows.ps1 `
  -QtRoot "C:\path\to\Qt" `
  -VtkRoot "C:\path\to\VTK" `
  -Hdf5Root "C:\path\to\HDF5" `
  -EigenRoot "C:\path\to\Eigen3"
```

Windows GitHub Actions 使用临时 `windows-2022` 执行环境，每次从头安装上述 MinGW64
工具链和依赖，然后配置、编译并打包。因此 CI 成功是“没有预装项目依赖的 Windows
环境”可重复构建的主要验收标准；它不等同于在一台刚安装完成的实体 Windows 电脑上
进行人工测试。

## 🛠️ 开发

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

## 🗂️ 目录结构

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

## ⚠️ 运行注意事项

- 部分模型库和算例依赖外部数据文件；若未随仓库提供，相应功能会提示文件缺失，
  但不影响主程序启动。
- Windows 下不要混用 MSVC 与 MinGW 编译的库。

## ✅ 构建验证

持续集成会在以下环境执行 Release 构建：

- Ubuntu 22.04 + Qt 5
- Windows Server 2022 + MSYS2 MinGW64 + Qt 6

## 🤝 参与贡献

欢迎通过 Issue 分享建议或反馈问题，也欢迎提交 Pull Request 一起完善 PassWing。无论是
代码、文档、测试还是使用体验方面的贡献，都非常珍贵。💙

## 📜 开源许可证

PassWing 采用 **GNU General Public License v3.0 or later（GPL-3.0-or-later）** 开源。
你可以在该许可证条款下使用、研究、修改和分发本项目；完整条款见 [LICENSE](LICENSE)。
