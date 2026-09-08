#ifndef GLWIDGET_H
#define GLWIDGET_H

#include <QWidget>
#include <QVTKOpenGLNativeWidget.h>
#include <vtkCubeSource.h>
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkGenericOpenGLRenderWindow.h>
#include <vtkPoints.h>
#include <vtkCellArray.h>
#include <vtkLine.h> // 添加此头文件
#include <vtkTransform.h> // 添加此头文件
#include <vtkTransformPolyDataFilter.h> // 添加此头文件
#include <vtkKochanekSpline.h>
#include <vtkParametricSpline.h>
#include <vtkParametricFunctionSource.h>
#include <vtkProperty.h>
#include <vtkRuledSurfaceFilter.h>
#include <vtkAppendPolyData.h>
#include <vtkSplineFilter.h>
#include <vtkPolyDataNormals.h> // 添加此头文件
#include <vtkTriangleFilter.h> // 添加此头文件
#include <vtkLinearExtrusionFilter.h>
#include <vtkLight.h>
#include <vtkLightKit.h>
#include <vtkSurfaceReconstructionFilter.h>
#include <vtkAxesActor.h>
#include <vtkOrientationMarkerWidget.h>
#include <vtkQuad.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkCamera.h>
#include <vtkTextActor.h>
#include <vtkTextProperty.h>
#include <vtkBorderRepresentation.h>
#include <vtkSphereSource.h>
#include <vtkTransformFilter.h>
#include <vtkCellData.h>

class glWidget
{

public:
    glWidget();

};

#endif // GLWIDGET_H
