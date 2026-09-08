cmake_minimum_required(VERSION 3.16)

project(PassWing VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

# Discover dependencies through CMAKE_PREFIX_PATH; never hard-code a
# contributor's local installation paths in this file.
find_package(Qt5 5.14 REQUIRED COMPONENTS
    Core Widgets Network Sql Help OpenGL Charts WebEngineWidgets
)
find_package(HDF5 REQUIRED COMPONENTS C CXX)
find_package(VTK REQUIRED)

# Prefer the vendored Eigen headers, but also support a system installation.
set(PASSWING_EIGEN_DIR "${CMAKE_CURRENT_SOURCE_DIR}/3rdLib/Eigen3/include/eigen3")
if(EXISTS "${PASSWING_EIGEN_DIR}/Eigen/Core")
    add_library(passwing_eigen INTERFACE)
    target_include_directories(passwing_eigen INTERFACE "${PASSWING_EIGEN_DIR}")
else()
    find_package(Eigen3 3.3 REQUIRED NO_MODULE)
    add_library(passwing_eigen ALIAS Eigen3::Eigen)
endif()

set(PASSWING_SOURCES
    main.cpp
    # Header-only QObject classes must be listed so CMAKE_AUTOMOC sees them.
    PublicClass/mychartview.h
    PublicClass/hoverbutton.h
    publicWidgetClass/customscrollArea.h
    publicWidgetClass/imagebutton.h
    AirfoilClass/airfoildesign.h
    AirfoilClass/airfoildesign.cpp
    AirfoilClass/airfoilsolve.h
    AirfoilClass/airfoilsolve.cpp
    AirfoilClass/airfoiloptimization.h
    AirfoilClass/airfoiloptimization.cpp
    AirfoilClass/airfoillibary.h
    AirfoilClass/airfoillibary.cpp

    AirfoilClass/airfoilanalyse.h
    AirfoilClass/airfoilanalyse.cpp
    AirfoilClass/airfoilexplorer.h
    AirfoilClass/airfoilexplorer.cpp
    AirfoilClass/airfoiloutput.h
    AirfoilClass/airfoiloutput.cpp
    AirfoilClass/airfoilmesh.h
    AirfoilClass/airfoilmesh.cpp
    AirfoilClass/cfddatareader.h
    AirfoilClass/cfddatareader.cpp
    AirfoilClass/XFoil.h
    AirfoilClass/XFoil.cpp
    AirfoilClass/airfoildisplay.h
    AirfoilClass/airfoildisplay.cpp
    WingClass/wingdisplay.h
    WingClass/wingdisplay.cpp
    WingClass/wingvlm.h
    WingClass/wingvlm.cpp
    WingClass/wingoptimization.h
    WingClass/wingoptimization.cpp
    PublicClass/mymath.h
    PublicClass/mymath.cpp
    PublicClass/qcustomplot.h
    PublicClass/qcustomplot.cpp
    publicWidgetClass/rangeslider.h
    publicWidgetClass/rangeslider.cpp
    publicWidgetClass/glwidget.h
    publicWidgetClass/glwidget.cpp
    publicWidgetClass/switchbutton.h
    publicWidgetClass/switchbutton.cpp
    publicWidgetClass/callout.h
    publicWidgetClass/callout.cpp
    publicWidgetClass/datapointdialog.h
    publicWidgetClass/datapointdialog.cpp
    propellersClass/propellerlibary.h
    propellersClass/propellerlibary.cpp
    propellersClass/propellerdisplay.h
    propellersClass/propellerdisplay.cpp
    propellersClass/propellerdesign.h
    propellersClass/propellerdesign.cpp
    propellersClass/propellervlm.h
    propellersClass/propellervlm.cpp
    propellersClass/propellerbemt.h
    propellersClass/propellerbemt.cpp
    help/htmlviewer.h
    help/htmlviewer.cpp
    geometryClass/stlreader.h
    geometryClass/stlreader.cpp
    geometryClass/airplaneLibrary.h
    geometryClass/airplaneLibrary.cpp
    publicClass/myfile.h
    publicClass/myfile.cpp
    AirplaneClass/airplanedisplay.h
    AirplaneClass/airplanedisplay.cpp
    AirplaneClass/airplanedesign.h
    AirplaneClass/airplanedesign.cpp
    AirplaneClass/airplanevlm.h
    AirplaneClass/airplanevlm.cpp
    AirplaneClass/airplaneStability.h
    AirplaneClass/airplaneStability.cpp
    dynamicModelClass/torsionalSpring.h
    dynamicModelClass/torsionalSpring.cpp
    chatClass/chatwidget.h
    chatClass/chatwidget.cpp
    mainwindow.h
    mainwindow.cpp
    saas/cfd/cfddisplay.h
    saas/cfd/cfddisplay.cpp
    saas/funClass/structwidget.h
    saas/funClass/structwidget.cpp
    saas/funClass/structcfddefinition.h
    saas/funClass/structcfddefinition.cpp
    saas/publicClass/fullwidgetdelegate.h
    saas/publicClass/fullwidgetdelegate.cpp
    saas/publicClass/myfile.h
    saas/publicClass/myfile.cpp
    saas/publicClass/fun3drun.h
    saas/publicClass/fun3drun.cpp
    saas/resultClass/colormapmanager.h
    saas/resultClass/colormapmanager.cpp
    saas/resultClass/flowviewer.h
    saas/resultClass/flowviewer.cpp
    saas/resultClass/pltreader.h
    saas/resultClass/pltreader.cpp
    saas/resultClass/vtkscalebar.h
    saas/resultClass/vtkscalebar.cpp
    saas/mesh/ugridviewer.h
    saas/mesh/ugridviewer.cpp
    saas/login/loginwidget.h
    saas/login/loginwidget.cpp
    saas/manager/jobmanager.h
    saas/manager/jobmanager.cpp
    resoure/passwing.qrc
)

if(WIN32)
    list(APPEND PASSWING_SOURCES win.rc)
endif()

add_executable(passwing ${PASSWING_SOURCES})

target_include_directories(passwing PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}"
    ${HDF5_INCLUDE_DIRS}
)

target_link_libraries(passwing PRIVATE
    Qt5::Core
    Qt5::Widgets
    Qt5::Network
    Qt5::Sql
    Qt5::Help
    Qt5::OpenGL
    Qt5::Charts
    Qt5::WebEngineWidgets
    passwing_eigen
    ${HDF5_CXX_LIBRARIES}
    ${HDF5_LIBRARIES}
    ${VTK_LIBRARIES}
)

if(COMMAND vtk_module_autoinit)
    vtk_module_autoinit(TARGETS passwing MODULES ${VTK_LIBRARIES})
endif()

set_target_properties(passwing PROPERTIES
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
    MACOSX_BUNDLE_BUNDLE_VERSION "${PROJECT_VERSION}"
    MACOSX_BUNDLE_SHORT_VERSION_STRING "${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}"
)

include(GNUInstallDirs)
install(TARGETS passwing
    BUNDLE DESTINATION .
    LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}"
    RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}"
)

# Examples:
# Windows (your current installation):
#   cmake -S . -B build -G "Visual Studio 16 2019" -A x64
#     -DCMAKE_PREFIX_PATH="C:/Qt/Qt5.14.2/5.14.2/msvc2017_64"
#     -DVTK_DIR="C:/VTK/lib/cmake/vtk-9.3"
#     -DHDF5_ROOT="C:/dev"
# Linux/macOS (dependencies installed in standard package-manager prefixes):
#   cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
# Build:
#   cmake --build build --config Release --parallel
