#ifndef VTKSCALEBAR_H
#define VTKSCALEBAR_H

#include <vtkSmartPointer.h>
#include <vtkRenderer.h>
#include <vtkCubeAxesActor2D.h>
#include <vtkTextProperty.h>
#include <vtkPolyData.h>

class vtkScaleBar
{
public:
    explicit vtkScaleBar(vtkRenderer* renderer);
    ~vtkScaleBar() = default;

    // 设置文字颜色
    void setColor(double r, double g, double b);

    // 设置字体大小
    void setFontSize(int size);



    // 设置数据源（你的网格/点云）
    void setInputData(vtkDataSet* data);

private:
    vtkRenderer* m_renderer = nullptr;
    vtkSmartPointer<vtkCubeAxesActor2D> m_axes;
    vtkSmartPointer<vtkTextProperty> m_textProp;
};

#endif








