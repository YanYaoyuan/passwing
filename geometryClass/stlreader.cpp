#include "stlreader.h"
#include <QVBoxLayout>
#include <QDir>
#include "vtkGenericOpenGLRenderWindow.h"
STLReader::STLReader(QWidget* parent)
    : QWidget(parent)
{
    // Create a VTK widget
        vtkWidget = new QVTKOpenGLNativeWidget(this);

        // Create a VTK renderer
        renderer = vtkSmartPointer<vtkRenderer>::New();

        // Create a VTK render window and set the renderer
        renderWindow = vtkSmartPointer<vtkGenericOpenGLRenderWindow>::New();
        renderWindow->AddRenderer(renderer);

        // Set the VTK render window to the VTK widget
        vtkWidget->setRenderWindow(renderWindow);

        // Create a VTK STL reader
        reader = vtkSmartPointer<vtkSTLReader>::New();

        // Create a mapper and actor
        mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
        actor = vtkSmartPointer<vtkActor>::New();
        actor->SetMapper(mapper);

        // 获取 QVTKOpenGLNativeWidget 提供的交互器
        vtkRenderWindowInteractor* interactor = vtkWidget->renderWindow()->GetInteractor();

        // 创建并设置交互样式
        style = vtkSmartPointer<vtkInteractorStyleTrackballCamera>::New();
        interactor->SetInteractorStyle(style);

        // 创建坐标轴
        axes = vtkSmartPointer<vtkAxesActor>::New();
        axes->SetTotalLength(2.0, 2.0, 2.0); // 设置坐标轴的长度
        axes->SetShaftType(0);
        axes->SetCylinderRadius(0.05);

        // 创建 OrientationMarkerWidget
        orientationMarker = vtkSmartPointer<vtkOrientationMarkerWidget>::New();
        orientationMarker->SetOrientationMarker(axes);
        orientationMarker->SetInteractor(interactor);
        orientationMarker->SetViewport(0.8, 0.0, 1.0, 0.2); // 设置坐标轴的位置和大小
        orientationMarker->SetEnabled(1);
        orientationMarker->InteractiveOff(); // 禁止交互

        // 设置视图
        camera = renderer->GetActiveCamera();
        camera->SetPosition(0.0, 0.0, 1000.0); // 从 Z 轴正方向俯视
        camera->SetFocalPoint(0.0, 0.0, 0.0); // 观察点在原点
        camera->SetViewUp(0.0, 1.0, 0.0); // Y 轴向上
        renderer->ResetCameraClippingRange();



        // Add the actor to the renderer
        renderer->AddActor(actor);
        renderer->SetBackground(0.941, 0.941, 0.941); // Background color dark blue
        actor->GetProperty()->SetColor(0.6, 0.8, 1.0); // RGB for deep blue
        renderer->ResetCamera();

        // Layout to place the vtkWidget within the STLReader widget
        QVBoxLayout* layout = new QVBoxLayout(this);
        layout->addWidget(vtkWidget);
        setLayout(layout);
}
void STLReader::setModelName(const QString name){
    QString fileName = name;
    std::string fileName1 = fileName.toStdString();
    reader->SetFileName(fileName1.c_str());
    reader->Update();
    // 更新 mapper 的输入
    mapper->SetInputConnection(reader->GetOutputPort());
    // Update the renderer's view
    renderer->ResetCamera(); // Reset camera to fit the new data
    renderer->ResetCameraClippingRange(); // Adjust the clipping range

    // 更新渲染视图
    vtkWidget->renderWindow()->Render();

}
