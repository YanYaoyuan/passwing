target_sources(passwing PRIVATE
    cfd/cfddisplay.cpp
    cfd/cfddisplay.h
    funClass/structcfddefinition.cpp
    funClass/structcfddefinition.h
    funClass/structwidget.cpp
    funClass/structwidget.h
    login/loginwidget.cpp
    login/loginwidget.h
    manager/jobmanager.cpp
    manager/jobmanager.h
    mesh/ugridviewer.cpp
    mesh/ugridviewer.h
    publicClass/fullwidgetdelegate.cpp
    publicClass/fullwidgetdelegate.h
    publicClass/fun3drun.cpp
    publicClass/fun3drun.h
    publicClass/myfile.cpp
    publicClass/myfile.h
    resultClass/colormapmanager.cpp
    resultClass/colormapmanager.h
    resultClass/flowviewer.cpp
    resultClass/flowviewer.h
    resultClass/pltreader.cpp
    resultClass/pltreader.h
    resultClass/vtkscalebar.cpp
    resultClass/vtkscalebar.h
)

# The remaining publicClass sources are retained for planned features. The
# alternate QCustomPlot implementation must not be linked with common/.
