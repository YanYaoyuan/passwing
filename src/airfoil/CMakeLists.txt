target_sources(passwing PRIVATE
    XFoil.cpp
    XFoil.h
    airfoilanalyse.cpp
    airfoilanalyse.h
    airfoilcompute.h
    airfoildesign.cpp
    airfoildesign.h
    airfoildisplay.cpp
    airfoildisplay.h
    airfoilexplorer.cpp
    airfoilexplorer.h
    airfoillibary.cpp
    airfoillibary.h
    airfoilmesh.cpp
    airfoilmesh.h
    airfoiloptimization.cpp
    airfoiloptimization.h
    airfoiloutput.cpp
    airfoiloutput.h
    airfoilsolve.cpp
    airfoilsolve.h
    cfddatareader.cpp
    cfddatareader.h
    xfoil_params.h
)

# XFoil is a direct legacy solver port whose original short mathematical names
# intentionally overlap class state. Limit those diagnostics to this unit.
if(MSVC)
    set_source_files_properties("${CMAKE_CURRENT_SOURCE_DIR}/XFoil.cpp"
        TARGET_DIRECTORY passwing
        PROPERTIES
        COMPILE_FLAGS "/wd4100 /wd4456 /wd4457 /wd4458"
    )
endif()
