cmake_minimum_required(VERSION 3.21)

project(PassWing VERSION 0.1.0 LANGUAGES C CXX)

include(GNUInstallDirs)

option(PASSWING_WARNINGS_AS_ERRORS "Treat compiler warnings as errors" OFF)
option(PASSWING_ENABLE_WEBENGINE "Enable the embedded HTML aircraft view" ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

# Resolve dependencies through CMAKE_PREFIX_PATH or the CMake package registry.
# Developer-specific paths belong in the untracked CMakeUserPresets.json.
find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core)
set(PASSWING_QT_PACKAGE "Qt${QT_VERSION_MAJOR}")

find_package(${PASSWING_QT_PACKAGE} REQUIRED COMPONENTS
    Charts Concurrent Core Help Network OpenGL PrintSupport Sql Widgets
)
if(PASSWING_ENABLE_WEBENGINE)
    find_package(${PASSWING_QT_PACKAGE} QUIET COMPONENTS WebEngineWidgets)
    if(NOT TARGET ${PASSWING_QT_PACKAGE}::WebEngineWidgets)
        message(WARNING "Qt WebEngine was not found; using the QTextBrowser fallback")
        set(PASSWING_ENABLE_WEBENGINE OFF)
    endif()
endif()
find_package(HDF5 REQUIRED COMPONENTS C CXX)
if(UNIX AND NOT APPLE)
    # Debian's VTK package exports GLVND targets from its imported modules.
    set(OpenGL_GL_PREFERENCE GLVND)
    find_package(OpenGL REQUIRED COMPONENTS OpenGL GLX)
endif()
find_package(VTK 9.0 REQUIRED COMPONENTS
    CommonColor
    CommonComputationalGeometry
    CommonCore
    CommonDataModel
    CommonMath
    CommonTransforms
    FiltersCore
    FiltersGeneral
    FiltersGeometry
    FiltersModeling
    FiltersSources
    GUISupportQt
    ImagingHybrid
    InteractionStyle
    InteractionWidgets
    IOGeometry
    IOLegacy
    IOParallel
    RenderingAnnotation
    RenderingCore
    RenderingOpenGL2
)

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
    mainwindow.cpp
    mainwindow.h

    AirfoilClass/XFoil.cpp
    AirfoilClass/XFoil.h
    AirfoilClass/airfoilanalyse.cpp
    AirfoilClass/airfoilanalyse.h
    AirfoilClass/airfoilcompute.h
    AirfoilClass/airfoildesign.cpp
    AirfoilClass/airfoildesign.h
    AirfoilClass/airfoildisplay.cpp
    AirfoilClass/airfoildisplay.h
    AirfoilClass/airfoilexplorer.cpp
    AirfoilClass/airfoilexplorer.h
    AirfoilClass/airfoillibary.cpp
    AirfoilClass/airfoillibary.h
    AirfoilClass/airfoilmesh.cpp
    AirfoilClass/airfoilmesh.h
    AirfoilClass/airfoiloptimization.cpp
    AirfoilClass/airfoiloptimization.h
    AirfoilClass/airfoiloutput.cpp
    AirfoilClass/airfoiloutput.h
    AirfoilClass/airfoilsolve.cpp
    AirfoilClass/airfoilsolve.h
    AirfoilClass/cfddatareader.cpp
    AirfoilClass/cfddatareader.h
    AirfoilClass/xfoil_params.h

    AirPlaneClass/airplanedesign.cpp
    AirPlaneClass/airplanedesign.h
    AirPlaneClass/airplanedisplay.cpp
    AirPlaneClass/airplanedisplay.h
    AirPlaneClass/airplanestability.cpp
    AirPlaneClass/airplanestability.h
    AirPlaneClass/airplanevlm.cpp
    AirPlaneClass/airplanevlm.h

    WingClass/wingdefinition.h
    WingClass/wingdisplay.cpp
    WingClass/wingdisplay.h
    WingClass/wingoptimization.cpp
    WingClass/wingoptimization.h
    WingClass/wingvlm.cpp
    WingClass/wingvlm.h

    propellersClass/propellerbemt.cpp
    propellersClass/propellerbemt.h
    propellersClass/propellerdesign.cpp
    propellersClass/propellerdesign.h
    propellersClass/propellerdisplay.cpp
    propellersClass/propellerdisplay.h
    propellersClass/propellerlibary.cpp
    propellersClass/propellerlibary.h
    propellersClass/propellervlm.cpp
    propellersClass/propellervlm.h

    PublicClass/hoverbutton.h
    PublicClass/mychartview.cpp
    PublicClass/mychartview.h
    PublicClass/myfile.cpp
    PublicClass/myfile.h
    PublicClass/mymath.cpp
    PublicClass/mymath.h
    PublicClass/qcustomplot.cpp
    PublicClass/qcustomplot.h
    PublicClass/structDefinition.h

    publicWidgetClass/callout.cpp
    publicWidgetClass/callout.h
    publicWidgetClass/customscrollarea.h
    publicWidgetClass/datapointdialog.cpp
    publicWidgetClass/datapointdialog.h
    publicWidgetClass/glwidget.cpp
    publicWidgetClass/glwidget.h
    publicWidgetClass/imagebutton.h
    publicWidgetClass/rangeslider.cpp
    publicWidgetClass/rangeslider.h
    publicWidgetClass/switchbutton.cpp
    publicWidgetClass/switchbutton.h

    geometryClass/airplanelibrary.cpp
    geometryClass/airplanelibrary.h
    geometryClass/stlreader.cpp
    geometryClass/stlreader.h
    dynamicModelClass/torsionalspring.cpp
    dynamicModelClass/torsionalspring.h
    chatClass/chatwidget.cpp
    chatClass/chatwidget.h

    saas/cfd/cfddisplay.cpp
    saas/cfd/cfddisplay.h
    saas/funClass/structcfddefinition.cpp
    saas/funClass/structcfddefinition.h
    saas/funClass/structwidget.cpp
    saas/funClass/structwidget.h
    saas/login/loginwidget.cpp
    saas/login/loginwidget.h
    saas/manager/jobmanager.cpp
    saas/manager/jobmanager.h
    saas/mesh/ugridviewer.cpp
    saas/mesh/ugridviewer.h
    saas/publicClass/fullwidgetdelegate.cpp
    saas/publicClass/fullwidgetdelegate.h
    saas/publicClass/fun3drun.cpp
    saas/publicClass/fun3drun.h
    saas/publicClass/myfile.cpp
    saas/publicClass/myfile.h
    saas/resultClass/colormapmanager.cpp
    saas/resultClass/colormapmanager.h
    saas/resultClass/flowviewer.cpp
    saas/resultClass/flowviewer.h
    saas/resultClass/pltreader.cpp
    saas/resultClass/pltreader.h
    saas/resultClass/vtkscalebar.cpp
    saas/resultClass/vtkscalebar.h

    resoure/passwing.qrc
)

if(WIN32)
    list(APPEND PASSWING_SOURCES win.rc)
endif()

add_executable(passwing ${PASSWING_SOURCES})
target_include_directories(passwing PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}")

target_link_libraries(passwing PRIVATE
    ${PASSWING_QT_PACKAGE}::Charts
    ${PASSWING_QT_PACKAGE}::Concurrent
    ${PASSWING_QT_PACKAGE}::Core
    ${PASSWING_QT_PACKAGE}::Help
    ${PASSWING_QT_PACKAGE}::Network
    ${PASSWING_QT_PACKAGE}::OpenGL
    ${PASSWING_QT_PACKAGE}::PrintSupport
    ${PASSWING_QT_PACKAGE}::Sql
    ${PASSWING_QT_PACKAGE}::Widgets
    HDF5::HDF5
    passwing_eigen
    ${VTK_LIBRARIES}
)

if(PASSWING_ENABLE_WEBENGINE)
    target_link_libraries(passwing PRIVATE ${PASSWING_QT_PACKAGE}::WebEngineWidgets)
    target_compile_definitions(passwing PRIVATE PASSWING_HAS_WEBENGINE=1)
endif()

if(MSVC)
    target_compile_definitions(passwing PRIVATE _USE_MATH_DEFINES)
    target_compile_options(passwing PRIVATE /permissive- /utf-8 /W4)
    if(PASSWING_WARNINGS_AS_ERRORS)
        target_compile_options(passwing PRIVATE /WX)
    endif()
else()
    target_compile_options(passwing PRIVATE -Wall -Wextra -Wpedantic)
    if(PASSWING_WARNINGS_AS_ERRORS)
        target_compile_options(passwing PRIVATE -Werror)
    endif()
endif()

if(COMMAND vtk_module_autoinit)
    vtk_module_autoinit(TARGETS passwing MODULES ${VTK_LIBRARIES})
endif()

set_target_properties(passwing PROPERTIES
    OUTPUT_NAME PassWing
    WIN32_EXECUTABLE TRUE
)

install(TARGETS passwing
    RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}"
    BUNDLE DESTINATION .
)
install(DIRECTORY theoreticalFramework
    DESTINATION "${CMAKE_INSTALL_DATADIR}/PassWing"
)
