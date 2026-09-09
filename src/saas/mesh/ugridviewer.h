#ifndef UGRIDVIEWER_H
#define UGRIDVIEWER_H

#include <QObject>
#include <QVTKOpenGLNativeWidget.h>
#include <vtkProperty.h>           // 必须包含完整定义
#include <vtkSmartPointer.h>
#include <vtkRenderer.h>
#include <vtkGenericOpenGLRenderWindow.h>
#include <vtkUnstructuredGridReader.h>
#include <vtkUnstructuredGrid.h>          // 必须包含完整定义
#include <vtkDataSet.h>                   // 必须包含完整定义
#include <vtkDataSetMapper.h>
#include <vtkActor.h>

class ugridViewer : public QObject
{
    Q_OBJECT

public:
    explicit ugridViewer(QVTKOpenGLNativeWidget* widget, QObject* parent = nullptr);

    // 加载 ugrid (.vtk / .vtu Legacy 格式) 网格
    bool loadUgridFromDir(const QString& runDir,const QString&);

    // 设置显示模式：线框或面
    void setWireframe(bool enable);

private:
    QVTKOpenGLNativeWidget* m_widget;

    vtkSmartPointer<vtkGenericOpenGLRenderWindow> m_renderWindow;
    vtkSmartPointer<vtkRenderer> m_renderer;

    vtkSmartPointer<vtkActor> m_actor;              // Actor 复用
    vtkSmartPointer<vtkDataSetMapper> m_mapper;     // Mapper 复用
};

#endif // UGRIDVIEWER_H


