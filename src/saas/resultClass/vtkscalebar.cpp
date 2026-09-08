#include "saas/resultClass/vtkscalebar.h"
#include <vtkCamera.h>
#include <vtkCubeAxesActor2D.h>

vtkScaleBar::vtkScaleBar(vtkRenderer* renderer)
{
    m_renderer = renderer;

    // 创建文字属性
    m_textProp = vtkSmartPointer<vtkTextProperty>::New();
    m_textProp->SetColor(1,1,0); // 默认黄色
    m_textProp->ShadowOn();
    m_textProp->SetFontSize(12);

    // 创建 CubeAxesActor2D
    m_axes = vtkSmartPointer<vtkCubeAxesActor2D>::New();
    m_axes->SetCamera(m_renderer->GetActiveCamera());
    m_axes->SetLabelFormat("%6.4g");
    m_axes->SetFlyModeToOuterEdges();
    m_axes->SetFontFactor(1.0);
    m_axes->SetAxisTitleTextProperty(m_textProp);
    m_axes->SetAxisLabelTextProperty(m_textProp);

    // 只显示 X 轴
    m_axes->SetXAxisVisibility(true);
    m_axes->SetYAxisVisibility(false);
    m_axes->SetZAxisVisibility(false);

    // 默认不显示网格线
    m_axes->SetXAxisVisibility(true);
    m_axes->SetYAxisVisibility(false);  // 可以保留，不会报错
    m_axes->SetZAxisVisibility(false);  // 可以保留，不会报错

    // 添加到渲染器
    m_renderer->AddViewProp(m_axes);
}

void vtkScaleBar::setColor(double r, double g, double b)
{
    m_textProp->SetColor(r,g,b);
}

void vtkScaleBar::setFontSize(int size)
{
    m_textProp->SetFontSize(size);
}



// 设置数据源
void vtkScaleBar::setInputData(vtkDataSet* data)
{
    m_axes->SetInputData(data);
}
