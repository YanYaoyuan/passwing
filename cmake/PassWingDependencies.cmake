include_guard(GLOBAL)

# Dependencies are discovered through CMAKE_PREFIX_PATH or the CMake package
# registry. Machine-specific paths belong in CMakeUserPresets.json.
find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core)
set(PASSWING_QT_PACKAGE "Qt${QT_VERSION_MAJOR}")

find_package(${PASSWING_QT_PACKAGE} REQUIRED COMPONENTS
    Charts
    Concurrent
    Core
    Help
    OpenGL
    PrintSupport
    Sql
    Widgets
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

set(PASSWING_EIGEN_DIR "${PROJECT_SOURCE_DIR}/3rdLib/Eigen3/include/eigen3")
if(EXISTS "${PASSWING_EIGEN_DIR}/Eigen/Core")
    add_library(passwing_eigen INTERFACE)
    target_include_directories(passwing_eigen INTERFACE "${PASSWING_EIGEN_DIR}")
else()
    # Eigen's config package only reports compatibility within the requested
    # major version.  Omitting a version here allows both Eigen 3.x and the
    # current MSYS2 Eigen 5.x package; PassWing only uses APIs shared by both.
    find_package(Eigen3 REQUIRED NO_MODULE)
    add_library(passwing_eigen ALIAS Eigen3::Eigen)
endif()

add_library(passwing_dependencies INTERFACE)
add_library(PassWing::Dependencies ALIAS passwing_dependencies)

target_link_libraries(passwing_dependencies INTERFACE
    ${PASSWING_QT_PACKAGE}::Charts
    ${PASSWING_QT_PACKAGE}::Concurrent
    ${PASSWING_QT_PACKAGE}::Core
    ${PASSWING_QT_PACKAGE}::Help
    ${PASSWING_QT_PACKAGE}::OpenGL
    ${PASSWING_QT_PACKAGE}::PrintSupport
    ${PASSWING_QT_PACKAGE}::Sql
    ${PASSWING_QT_PACKAGE}::Widgets
    HDF5::HDF5
    passwing_eigen
    ${VTK_LIBRARIES}
)

if(PASSWING_ENABLE_WEBENGINE)
    target_link_libraries(passwing_dependencies INTERFACE
        ${PASSWING_QT_PACKAGE}::WebEngineWidgets
    )
    target_compile_definitions(passwing_dependencies INTERFACE
        PASSWING_HAS_WEBENGINE=1
    )
endif()
