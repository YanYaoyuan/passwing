# Building PassWing

PassWing uses CMake 3.21+, C++17, and Qt 5.14+ or Qt 6. The source tree contains no
developer-specific SDK paths. Provide dependency prefixes through
`CMAKE_PREFIX_PATH`, a package-manager toolchain, or an untracked
`CMakeUserPresets.json`.

## Dependencies

- Qt 5/6: Charts, Concurrent, Core, Help, Network, OpenGL, PrintSupport, SQL,
  and Widgets. WebEngineWidgets is optional.
- VTK 9 with Qt support
- HDF5 with C and C++ support
- Eigen 3.3+
- Ninja (recommended)

Dependency architecture must match the compiler and application architecture.
In particular, MSVC libraries cannot be linked into a MinGW build.

## Linux

On Ubuntu 22.04, install the toolchain and development packages:

```bash
sudo apt-get install build-essential cmake ninja-build qtbase5-dev \
  libqt5charts5-dev libqt5opengl5-dev qttools5-dev qtwebengine5-dev \
  libeigen3-dev libhdf5-dev libvtk9-dev libvtk9-qt-dev
```

Then configure and build:

```bash
cmake --preset release
cmake --build --preset release --parallel
```

## Windows

Use one consistent dependency stack. With MSYS2/MinGW64, install:

```bash
pacman -S --needed mingw-w64-x86_64-cmake mingw-w64-x86_64-ninja \
  mingw-w64-x86_64-qt6-base mingw-w64-x86_64-qt6-charts \
  mingw-w64-x86_64-qt6-tools mingw-w64-x86_64-eigen3 \
  mingw-w64-x86_64-hdf5 mingw-w64-x86_64-vtk
```

Run the same preset commands from an MSYS2 MinGW64 shell. For an MSVC build,
install MSVC-compatible builds of all dependencies and configure from a
Developer Command Prompt:

```powershell
cmake -S . -B out/build/msvc -G Ninja `
  -DCMAKE_BUILD_TYPE=Release `
  -DCMAKE_PREFIX_PATH="C:/path/to/qt;C:/path/to/dependencies"
cmake --build out/build/msvc --parallel
```

WebEngine can be omitted for lightweight or headless builds by configuring
with `-DPASSWING_ENABLE_WEBENGINE=OFF`; local HTML then uses `QTextBrowser`.

The local FUN3D runner expects `mpiexec` on `PATH`. It looks for
`nodet_mpi.exe` on Windows and `nodet_mpi` on Linux.

## Code quality

Format first-party C++ files with `clang-format` using the checked-in
`.clang-format`. The `.clang-tidy` configuration enables bug, portability,
performance, and selected modernization checks while excluding vendored
QCustomPlot and XFoil code from header diagnostics.

Enable strict warning enforcement once the existing warning backlog is clean:

```bash
cmake --preset debug -DPASSWING_WARNINGS_AS_ERRORS=ON
```
