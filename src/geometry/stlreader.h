#ifndef STLREADER_H
#define STLREADER_H

#include <QWidget>
#include <QVTKOpenGLNativeWidget.h>
#include <vtkSmartPointer.h>
#include <vtkSTLReader.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkActor.h>
#include <vtkAxesActor.h>
#include <vtkOrientationMarkerWidget.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkCamera.h>
#include <vtkProperty.h>
class STLReader : public QWidget
{
    Q_OBJECT

public:
    STLReader(QWidget* parent = nullptr);
    void setModelName(const QString);

private:
    QVTKOpenGLNativeWidget* vtkWidget;
    vtkSmartPointer<vtkRenderer> renderer;
    vtkSmartPointer<vtkGenericOpenGLRenderWindow> renderWindow;
    vtkSmartPointer<vtkSTLReader>reader;
    vtkSmartPointer<vtkPolyDataMapper> mapper;
    vtkSmartPointer<vtkActor> actor;
    vtkSmartPointer<vtkInteractorStyleTrackballCamera> style;
    vtkSmartPointer<vtkAxesActor> axes;
    vtkSmartPointer<vtkCamera> camera;


    vtkSmartPointer<vtkOrientationMarkerWidget>orientationMarker;
};

#endif // STLREADER_H
