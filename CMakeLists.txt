cmake_minimum_required(VERSION 3.10)
project(passwing VERSION 0.1 LANGUAGES CXX)

set(QT_DIR "C:/Qt/Qt5.14.2/5.14.2/msvc2017_64/lib/cmake/Qt5")
set(CMAKE_PREFIX_PATH "C:/Qt/Qt5.14.2/5.14.2/msvc2017_64")
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# HDF5 预编译库
set(HDF5_ROOT "C:/dev")

# 手动添加 include
include_directories(${HDF5_ROOT}/include)

# 手动添加库文件
link_directories(${HDF5_ROOT}/lib)






# VTK configuration
find_package(VTK REQUIRED)
include(${VTK_USE_FILE})
#find_package(QT5WebEngine REQUIRED)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/3rdLib/Eigen3/include/eigen3)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

link_directories(${VTK_LIBRARY_DIR})

#find_package(Qt5 REQUIRED COMPONENTS Widgets OpenGL OpenGLExtensions Charts Core Sql Help)
find_package(Qt5 REQUIRED COMPONENTS Widgets Core Sql Help OpenGL  Charts WebEngineWidgets)
set(PROJECT_SOURCES
    main.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/AirfoilClass/airfoildesign.h
    ${CMAKE_CURRENT_SOURCE_DIR}/AirfoilClass/airfoildesign.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/AirfoilClass/airfoilsolve.h
    ${CMAKE_CURRENT_SOURCE_DIR}/AirfoilClass/airfoilsolve.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/AirfoilClass/airfoiloptimization.h
    ${CMAKE_CURRENT_SOURCE_DIR}/AirfoilClass/airfoiloptimization.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/AirfoilClass/airfoillibary.h
    ${CMAKE_CURRENT_SOURCE_DIR}/AirfoilClass/airfoillibary.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/AirfoilClass/airfoilanalyse.h
    ${CMAKE_CURRENT_SOURCE_DIR}/AirfoilClass/airfoilanalyse.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/AirfoilClass/airfoilexplorer.h
    ${CMAKE_CURRENT_SOURCE_DIR}/AirfoilClass/airfoilexplorer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/AirfoilClass/airfoiloutput.h
    ${CMAKE_CURRENT_SOURCE_DIR}/AirfoilClass/airfoiloutput.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/AirfoilClass/airfoilmesh.h
    ${CMAKE_CURRENT_SOURCE_DIR}/AirfoilClass/airfoilmesh.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/AirfoilClass/cfddatareader.h
    ${CMAKE_CURRENT_SOURCE_DIR}/AirfoilClass/cfddatareader.cpp




    ${CMAKE_CURRENT_SOURCE_DIR}/WingClass/wingdisplay.h
    ${CMAKE_CURRENT_SOURCE_DIR}/WingClass/wingdisplay.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/AirfoilClass/xfoil_params.h
    ${CMAKE_CURRENT_SOURCE_DIR}/AirfoilClass/XFoil.h
    ${CMAKE_CURRENT_SOURCE_DIR}/AirfoilClass/XFoil.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/PublicClass/mychartview.h
    ${CMAKE_CURRENT_SOURCE_DIR}/WingClass/wingdefinition.h
    ${CMAKE_CURRENT_SOURCE_DIR}/WingClass/wingvlm.h
    ${CMAKE_CURRENT_SOURCE_DIR}/WingClass/wingvlm.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/AirfoilClass/airfoildisplay.h
    ${CMAKE_CURRENT_SOURCE_DIR}/AirfoilClass/airfoildisplay.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/mainwindow.h
    ${CMAKE_CURRENT_SOURCE_DIR}/mainwindow.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/chatClass/chatwidget.h
    ${CMAKE_CURRENT_SOURCE_DIR}/chatClass/chatwidget.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/propellersClass/propellerlibary.h
    ${CMAKE_CURRENT_SOURCE_DIR}/propellersClass/propellerlibary.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/PublicClass/structDefinition.h
    ${CMAKE_CURRENT_SOURCE_DIR}/propellersClass/propellerdisplay.h
    ${CMAKE_CURRENT_SOURCE_DIR}/propellersClass/propellerdisplay.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/PublicClass/mymath.h
    ${CMAKE_CURRENT_SOURCE_DIR}/PublicClass/mymath.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/PublicClass/qcustomplot.h
    ${CMAKE_CURRENT_SOURCE_DIR}/PublicClass/qcustomplot.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/PublicClass/hoverbutton.h
    ${CMAKE_CURRENT_SOURCE_DIR}/WingClass/wingoptimization.h
    ${CMAKE_CURRENT_SOURCE_DIR}/WingClass/wingoptimization.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/publicWidgetClass/rangeslider.h
    ${CMAKE_CURRENT_SOURCE_DIR}/publicWidgetClass/rangeslider.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/propellersClass/propellerdesign.h
    ${CMAKE_CURRENT_SOURCE_DIR}/propellersClass/propellerdesign.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/propellersClass/propellervlm.h
    ${CMAKE_CURRENT_SOURCE_DIR}/propellersClass/propellervlm.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/propellersClass/propellerbemt.h
    ${CMAKE_CURRENT_SOURCE_DIR}/propellersClass/propellerbemt.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/publicWidgetClass/glwidget.h
    ${CMAKE_CURRENT_SOURCE_DIR}/publicWidgetClass/glwidget.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/help/htmlviewer.h
    ${CMAKE_CURRENT_SOURCE_DIR}/help/htmlviewer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/publicWidgetClass/switchbutton.h
    ${CMAKE_CURRENT_SOURCE_DIR}/publicWidgetClass/switchbutton.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/publicWidgetClass/callout.h
    ${CMAKE_CURRENT_SOURCE_DIR}/publicWidgetClass/callout.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/publicWidgetClass/customscrollArea.h
    ${CMAKE_CURRENT_SOURCE_DIR}/geometryClass/stlreader.h
    ${CMAKE_CURRENT_SOURCE_DIR}/geometryClass/stlreader.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/geometryClass/airplaneLibrary.h
    ${CMAKE_CURRENT_SOURCE_DIR}/geometryClass/airplaneLibrary.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/resoure/passwing.qrc
    ${CMAKE_CURRENT_SOURCE_DIR}/win.rc
    ${CMAKE_CURRENT_SOURCE_DIR}/publicWidgetClass/imagebutton.h

    ${CMAKE_CURRENT_SOURCE_DIR}/publicWidgetClass/datapointdialog.h
    ${CMAKE_CURRENT_SOURCE_DIR}/publicWidgetClass/datapointdialog.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/publicClass/myfile.h
    ${CMAKE_CURRENT_SOURCE_DIR}/publicClass/myfile.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/AirplaneClass/airplanedisplay.h
    ${CMAKE_CURRENT_SOURCE_DIR}/AirplaneClass/airplanedisplay.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/AirplaneClass/airplanedesign.h
    ${CMAKE_CURRENT_SOURCE_DIR}/AirplaneClass/airplanedesign.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/AirplaneClass/airplanevlm.h
    ${CMAKE_CURRENT_SOURCE_DIR}/AirplaneClass/airplanevlm.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/AirplaneClass/airplaneStability.h
    ${CMAKE_CURRENT_SOURCE_DIR}/AirplaneClass/airplaneStability.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/dynamicModelClass/torsionalSpring.h
    ${CMAKE_CURRENT_SOURCE_DIR}/dynamicModelClass/torsionalSpring.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/saas/cfd/cfddisplay.h
    ${CMAKE_CURRENT_SOURCE_DIR}/saas/cfd/cfddisplay.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/saas/funClass/structwidget.h
    ${CMAKE_CURRENT_SOURCE_DIR}/saas/funClass/structwidget.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/saas/funClass/structcfddefinition.h
    ${CMAKE_CURRENT_SOURCE_DIR}/saas/funClass/structcfddefinition.cpp


    ${CMAKE_CURRENT_SOURCE_DIR}/saas/publicClass/fullwidgetdelegate.h
    ${CMAKE_CURRENT_SOURCE_DIR}/saas/publicClass/fullwidgetdelegate.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/saas/publicClass/myfile.h
    ${CMAKE_CURRENT_SOURCE_DIR}/saas/publicClass/myfile.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/saas/publicClass/fun3drun.h
    ${CMAKE_CURRENT_SOURCE_DIR}/saas/publicClass/fun3drun.cpp


    ${CMAKE_CURRENT_SOURCE_DIR}/saas/resultClass/colormapmanager.h
    ${CMAKE_CURRENT_SOURCE_DIR}/saas/resultClass/colormapmanager.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/saas/resultClass/flowviewer.h
    ${CMAKE_CURRENT_SOURCE_DIR}/saas/resultClass/flowviewer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/saas/resultClass/pltreader.h
    ${CMAKE_CURRENT_SOURCE_DIR}/saas/resultClass/pltreader.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/saas/resultClass/vtkscalebar.h
    ${CMAKE_CURRENT_SOURCE_DIR}/saas/resultClass/vtkscalebar.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/saas/mesh/ugridviewer.h
    ${CMAKE_CURRENT_SOURCE_DIR}/saas/mesh/ugridviewer.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/saas/login/loginwidget.h
    ${CMAKE_CURRENT_SOURCE_DIR}/saas/login/loginwidget.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/saas/manager/jobmanager.h
    ${CMAKE_CURRENT_SOURCE_DIR}/saas/manager/jobmanager.cpp


)

add_executable(passwing ${PROJECT_SOURCES})

target_link_libraries(passwing PRIVATE
    Qt5::Widgets
    Qt5::Core
    Qt5::Sql
    Qt5::Help
    Qt5::OpenGL
    ${VTK_LIBRARIES}
    Qt5::Charts
    Qt5::WebEngineWidgets
    ${HDF5_ROOT}/lib/hdf5.lib
    ${HDF5_ROOT}/lib/hdf5_cpp.lib
)


set_target_properties(passwing PROPERTIES
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
)

include(GNUInstallDirs)
install(TARGETS passwing
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
