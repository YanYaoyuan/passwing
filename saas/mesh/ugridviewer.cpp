#include "saas/mesh/ugridviewer.h"
#include <QDir>

#include <vtkUnstructuredGrid.h>

ugridViewer::ugridViewer(QVTKOpenGLNativeWidget* widget, QObject* parent)
    : QObject(parent)
    , m_widget(widget)
{
    // 渲染窗口
    m_renderWindow = vtkSmartPointer<vtkGenericOpenGLRenderWindow>::New();
    m_widget->setRenderWindow(m_renderWindow);

    // 渲染器
    m_renderer = vtkSmartPointer<vtkRenderer>::New();
    m_renderWindow->AddRenderer(m_renderer);
    m_renderer->SetBackground(0.2, 0.3, 0.4);

    // Mapper + Actor
    m_mapper = vtkSmartPointer<vtkDataSetMapper>::New();
    m_actor = vtkSmartPointer<vtkActor>::New();
    m_actor->SetMapper(m_mapper);
    m_renderer->AddActor(m_actor);

    // ✅ 给 Mapper 设置一个空网格，避免“0 connections”报错
    vtkSmartPointer<vtkUnstructuredGrid> emptyGrid =
        vtkSmartPointer<vtkUnstructuredGrid>::New();
    m_mapper->SetInputData(emptyGrid);

    // 可以安全渲染空场景
    m_renderWindow->Render();
}



bool ugridViewer::loadUgridFromDir(const QString& runDir, const QString& fileName)
{
    QString fullPath;

    if (!fileName.isEmpty()) {
        fullPath = QDir(runDir).filePath(fileName);
    } else {
        QDir dir(runDir);
        QStringList vtkFiles = dir.entryList(QStringList() << "*.vtk", QDir::Files);
        if (vtkFiles.isEmpty()) {

            return false;
        }
        fullPath = dir.filePath(vtkFiles.first());
    }

    QFileInfo checkFile(fullPath);
    if (!checkFile.exists() || !checkFile.isFile()) {

        return false;
    }

    vtkSmartPointer<vtkUnstructuredGridReader> reader =
        vtkSmartPointer<vtkUnstructuredGridReader>::New();
    reader->SetFileName(fullPath.toStdString().c_str());

    // VTK 9.3 不支持 CanReadFile，直接 Update()
    reader->Update();

    vtkUnstructuredGrid* grid = reader->GetOutput();
    if (!grid || grid->GetNumberOfPoints() == 0) {

        return false;
    }

    m_mapper->SetInputData(grid);
    m_renderer->ResetCamera();
    m_renderWindow->Render();


    return true;
}


void ugridViewer::setWireframe(bool enable)
{
    if (enable)
        m_actor->GetProperty()->SetRepresentationToWireframe();
    else
        m_actor->GetProperty()->SetRepresentationToSurface();

    m_renderWindow->Render();
}

