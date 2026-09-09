target_sources(passwing PRIVATE
    airfoilplot.cpp
    airfoilplot.h
    hoverbutton.h
    mychartview.cpp
    mychartview.h
    myfile.cpp
    myfile.h
    mymath.cpp
    mymath.h
    qcustomplot.cpp
    qcustomplot.h
    structDefinition.h
    vtkfontutils.cpp
    vtkfontutils.h
)

# OTHER/ contains an archived QCustomPlot copy and is intentionally not built.

# QCustomPlot is an imported third-party implementation. Keep its historical
# warning debt local so /W4 remains useful for PassWing-owned sources.
if(MSVC)
    set_source_files_properties("${CMAKE_CURRENT_SOURCE_DIR}/qcustomplot.cpp"
        TARGET_DIRECTORY passwing
        PROPERTIES
        COMPILE_FLAGS "/wd4456 /wd4458 /wd4702 /wd4996"
    )
endif()
