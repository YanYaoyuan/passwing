#include "saas/resultClass/flowviewer.h"
#include<QVBoxLayout>
#include <vtkAutoInit.h>
VTK_MODULE_INIT(vtkRenderingOpenGL2);
VTK_MODULE_INIT(vtkInteractionStyle);
#include <vtkDataSet.h>
#include <vtkPolyData.h>
#include <vtkDataSetMapper.h>
#include <vtkProperty.h>
#include <vtkNamedColors.h>
#include <vtkScalarBarActor.h>
#include <vtkLookupTable.h>
#include <vtkUnstructuredGrid.h>
#include <vtkPointData.h>
#include <vtkNew.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkTextProperty.h>
#include <vtkArrayCalculator.h>
#include <iostream>
#include <QApplication>
#include <QFile>
#include <QDebug>


// ------------------------ TecplotWidget implementation ------------------------
flowViewer::flowViewer(QWidget *parent)
    : QVTKOpenGLNativeWidget(parent)
{
    // 创建渲染器和渲染窗口
    m_renderer = vtkSmartPointer<vtkRenderer>::New();
    m_renderWindow = vtkSmartPointer<vtkGenericOpenGLRenderWindow>::New();
    m_renderWindow->AddRenderer(m_renderer);

    initLighting();

    airfoilActor = nullptr;
    meshActor = nullptr;



    currentMode = ShowAirfoil;



    m_cpActor = vtkSmartPointer<vtkActor>::New();
    m_cpScalarBar = vtkSmartPointer<vtkScalarBarActor>::New();
    m_lut = vtkSmartPointer<vtkLookupTable>::New();
    m_cloudMapper = vtkSmartPointer<vtkDataSetMapper>::New();
    m_cpScalarBar->SetLookupTable(m_lut);   // ★ 必须绑定
    m_lut->SetHueRange(0.667, 0.0);
    m_cpScalarBar->SetLabelFormat("%.3f");

    m_cpActor->SetMapper(m_cloudMapper);

    // 绑定到 QVTKWidget
    this->setRenderWindow(m_renderWindow);
    m_interactor = m_renderWindow->GetInteractor();

    // ===== 创建坐标轴 =====
    m_axesActor = vtkSmartPointer<vtkAxesActor>::New();
    m_axesActor->SetTotalLength(1.0, 1.0, 1.0);
    // ===== 创建坐标轴控件（显示在角落的小坐标系窗口） =====
    m_axesWidget = vtkSmartPointer<vtkOrientationMarkerWidget>::New();
    m_axesWidget->SetOrientationMarker(m_axesActor);
    m_axesWidget->SetInteractor(m_renderWindow->GetInteractor());
    m_axesWidget->SetEnabled(1);          // 默认显示
    m_axesWidget->SetInteractive(0);      // 不允许鼠标拖动（更专业）
    m_axesWidget->SetViewport(0.85, 0.0, 1.0, 0.15);


    //setBackground("LightSteelBlue", "White");



    // 设置交互样式
    auto style = vtkSmartPointer<vtkInteractorStyleTrackballCamera>::New();
    m_interactor->SetInteractorStyle(style);
    m_interactor->Initialize();


}


flowViewer::~flowViewer()
{

}
// flowViewer.cpp
// flowViewer.cpp
void flowViewer::initLighting()
{
    // 关闭默认灯光，否则影响效果
    m_renderer->AutomaticLightCreationOff();

    // ===== Ambient Light（环境光） =====
    m_ambientLight = vtkSmartPointer<vtkLight>::New();
    m_ambientLight->SetLightTypeToSceneLight();
    m_ambientLight->SetAmbientColor(1.0, 1.0, 1.0);
    m_ambientLight->SetIntensity(0.4);     // 默认环境光强度
    m_renderer->AddLight(m_ambientLight);

    // ===== Directional Light （平行光）=====
    m_directionalLight = vtkSmartPointer<vtkLight>::New();
    m_directionalLight->SetLightTypeToHeadlight(); // 跟随相机更自然
    m_directionalLight->SetColor(1.0, 1.0, 1.0);
    m_directionalLight->SetIntensity(0.8);         // 默认主光源
    m_renderer->AddLight(m_directionalLight);
}


vtkSmartPointer<vtkActor> flowViewer::createBasicActor(vtkDataSet* ds)
{
    auto mapper = vtkSmartPointer<vtkDataSetMapper>::New();
    mapper->SetInputData(ds);
    auto actor = vtkSmartPointer<vtkActor>::New();
    actor->SetMapper(mapper);
    return actor;
}

void flowViewer::setInputData(vtkSmartPointer<vtkMultiBlockDataSet> inputData)
{
    if (!inputData) return;

    // 删除旧 Actor（可选，如果你使用成员变量可以不删）
    if (m_basicActor)
        m_renderer->RemoveActor(m_basicActor);

    // ShallowCopy MultiBlock
    m_multiBlock = vtkSmartPointer<vtkMultiBlockDataSet>::New();
    m_multiBlock->ShallowCopy(inputData);

    vtkUnstructuredGrid* ugOrig =
        vtkUnstructuredGrid::SafeDownCast(m_multiBlock->GetBlock(0));
    if (!ugOrig) return;

    // ⚡ 创建新的网格对象，保证 Mapper 刷新
    auto ug = vtkSmartPointer<vtkUnstructuredGrid>::New();
    ug->ShallowCopy(ugOrig);

    // 计算速度
    auto calc = vtkSmartPointer<vtkArrayCalculator>::New();
    calc->SetInputData(ug);
    calc->AddScalarArrayName("u");
    calc->AddScalarArrayName("v");
    calc->AddScalarArrayName("w");
    calc->SetResultArrayName("velocity");
    calc->SetFunction("u*iHat + v*jHat + w*kHat");
    calc->Update();

    m_unstructuredGrid = vtkSmartPointer<vtkUnstructuredGrid>::New();
    m_unstructuredGrid->ShallowCopy(calc->GetUnstructuredGridOutput());

    // 更新 Actor Mapper（成员变量）
    m_cloudMapper->SetInputData(m_unstructuredGrid);
    m_cloudMapper->Update();

    auto surfaceProp = m_cpActor->GetProperty();
    // 稍微暗一点的灰，有更好对比度
    surfaceProp->SetColor(0.75, 0.75, 0.75);

    // 光照参数
    surfaceProp->SetAmbient(0.2);        // 环境光低一点，让阴影有层次
    surfaceProp->SetDiffuse(0.7);        // 漫反射强一点，增强立体感
    surfaceProp->SetSpecular(0.3);       // 明显高光，轮廓更亮
    surfaceProp->SetSpecularPower(20);   // 高光集中（越高越尖锐）


    if (!m_renderer->HasViewProp(m_cpActor))
        m_renderer->AddActor(m_cpActor);

    // 可见性
    //m_cpActor->VisibilityOn();
    //showScalarCloud(0);
    //setMeshDisplayMode(1);


    this->renderWindow()->Render();
}
void flowViewer::setMeshDisplayMode(int mode)
{
    // mode: 0 = 只表面
    //       1 = 表面 + 线框
    //       2 = 只线框

    if (!m_unstructuredGrid || !m_renderer) return;

    // 确保 Mapper 已经绑定网格
    m_cloudMapper->SetInputData(m_unstructuredGrid);
    m_cloudMapper->ScalarVisibilityOff(); // 只显示网格，不显示云图
    m_cloudMapper->Update();

    // ======== 表面 Actor ========
    if (!m_surfaceActor) {
        m_surfaceActor = vtkSmartPointer<vtkActor>::New();
        m_surfaceActor->SetMapper(m_cloudMapper);
        m_renderer->AddActor(m_surfaceActor);
    }

    auto surfaceProp = m_surfaceActor->GetProperty();
    // 稍微暗一点的灰，有更好对比度
    surfaceProp->SetColor(0.75, 0.75, 0.75);

    // 光照参数
    surfaceProp->SetAmbient(0.2);        // 环境光低一点，让阴影有层次
    surfaceProp->SetDiffuse(0.7);        // 漫反射强一点，增强立体感
    surfaceProp->SetSpecular(0.3);       // 明显高光，轮廓更亮
    surfaceProp->SetSpecularPower(20);   // 高光集中（越高越尖锐）



    // ======== 线框 Actor ========
    if (!m_wireActor) {
        m_wireActor = vtkSmartPointer<vtkActor>::New();
        m_wireActor->SetMapper(m_cloudMapper);
        m_renderer->AddActor(m_wireActor);
    }

    auto wireProp = m_wireActor->GetProperty();
    wireProp->SetRepresentationToWireframe();
    wireProp->SetColor(0.0, 0.0, 0.0);        // 黑色线框
    wireProp->SetLineWidth(1.0);
    wireProp->LightingOff();                  // 线框不受光照影响，颜色均匀

    // ======== 根据模式设置可见性 ========
    switch (mode)
    {
        case 0: // 只表面
            m_surfaceActor->VisibilityOn();
            m_wireActor->VisibilityOff();
            break;

        case 1: // 表面 + 线框
            m_surfaceActor->VisibilityOn();
            m_wireActor->VisibilityOn();
            break;

        case 2: // 只线框
            m_surfaceActor->VisibilityOff();
            m_wireActor->VisibilityOn();
            break;

        default:
            m_surfaceActor->VisibilityOn();
            m_wireActor->VisibilityOn();
            break;
    }

    m_renderer->GetRenderWindow()->Render();
}








void flowViewer::setBackgroundColor(const QColor &color)
{
    m_renderer->SetBackground(color.redF(), color.greenF(), color.blueF());
    this->renderWindow()->Render();
}

QColor flowViewer::getBackgroundColor() const
{
    double *c = m_renderer->GetBackground();
    return QColor::fromRgbF(c[0], c[1], c[2]);
}



void flowViewer::showScalarCloud(int dataIndex)
{
    // 获取网格
    vtkUnstructuredGrid* ug =
        vtkUnstructuredGrid::SafeDownCast(m_multiBlock->GetBlock(0));
    if (!ug) {
        std::cerr << "UnstructuredGrid is null!\n";
        return;
    }

    vtkPointData* pd = ug->GetPointData();
    if (!pd) {
        std::cerr << "PointData is null!\n";
        return;
    }

    // 检查索引是否正确
    if (dataIndex < 0 || dataIndex >= pd->GetNumberOfArrays()) {
        std::cerr << "Invalid data index: " << dataIndex << "\n";
        return;
    }

    // 通过索引获取数组
    vtkDataArray* arr = pd->GetArray(dataIndex);
    if (!arr) {
        std::cerr << "Data array at index " << dataIndex << " is null!\n";
        return;
    }

    std::string arrName = arr->GetName();
    if (arrName.empty()) arrName = "Variable";

    // 范围
    double range[2];
    arr->GetRange(range);

    historyValueRange.min = range[0];
    historyValueRange.max = range[1];

    // 激活 scalars
    pd->SetActiveScalars(arrName.c_str());

    // 配置 LUT
    m_lut->SetNumberOfTableValues(256);

    m_lut->SetTableRange(range);
    m_lut->Build();

    // Mapper
    //m_cloudMapper = vtkSmartPointer<vtkDataSetMapper>::New();
    m_cloudMapper->SetInputData(ug);
    m_cloudMapper->SetLookupTable(m_lut);
    m_cloudMapper->SetScalarRange(range);
    m_cloudMapper->ScalarVisibilityOn();
    //m_cloudMapper->Update();   // 这一句非常关键

    // Actor

    if (!m_renderer->HasViewProp(m_cpActor))
        m_renderer->AddActor(m_cpActor);

    // ScalarBar
    if (m_cpScalarBar) {
        m_cpScalarBar->SetLookupTable(m_lut);
        m_cpScalarBar->SetTitle(arrName.c_str());
        m_cpScalarBar->SetPosition(0.9, 0.2);
        m_cpScalarBar->SetPosition2(0.04, 0.6);
        m_cpScalarBar->SetNumberOfLabels(historyValueRange.nums);

        if (!m_renderer->HasViewProp(m_cpScalarBar))
            m_renderer->AddActor2D(m_cpScalarBar);

    }

    m_renderer->GetRenderWindow()->Render();
}




void flowViewer::setBackground( const std::string& color1 = "White",
                   const std::string& color2 = "")
{
    if (!m_renderer) return;

    vtkSmartPointer<vtkNamedColors> colors = vtkSmartPointer<vtkNamedColors>::New();

    if (color2.empty())
    {
        // 单色背景
        m_renderer->SetBackground(colors->GetColor3d(color1).GetData());
    }
    else
    {
        // 渐变背景
        m_renderer->GradientBackgroundOn();
        m_renderer->SetBackground(colors->GetColor3d(color1).GetData());
        m_renderer->SetBackground2(colors->GetColor3d(color2).GetData());
    }

    m_renderer->GetRenderWindow()->Render();
}
void flowViewer::switchColorMap(int index)
{


    switch(index)
    {
        case 0: m_lut = makeJet(); break;
        case 1: m_lut = makeRainbow(); break;
        case 2: m_lut = makeHot(); break;
        case 3: m_lut = makeCoolWarm(); break;
        case 4: m_lut = makeGreys(); break;
        default: return;
    }

    // 更新 actor
    if (m_cpActor && m_cpActor->GetMapper())
    {
        m_cpActor->GetMapper()->SetLookupTable(m_lut);
        m_cpActor->GetMapper()->Update();       // 确保 mapper 更新
    }

    // 更新 scalar bar
    if (m_cpScalarBar)
        m_cpScalarBar->SetLookupTable(m_lut);

    if (m_renderer)
    {
        m_renderer->GetRenderWindow()->Render();  // RenderWindow 刷新，而非单 renderer
    }
}
void flowViewer::setCpRange(double minVal, double maxVal,int numLabels)
{
    if (!m_cpActor || !m_cpActor->GetMapper() || !m_lut) return;

    // 更新 Mapper
    auto mapper = vtkDataSetMapper::SafeDownCast(m_cpActor->GetMapper());
    if (mapper) {
        mapper->SetScalarRange(minVal, maxVal);
    }

    // 更新 LUT
    m_lut->SetTableRange(minVal, maxVal);
    m_lut->Build();

    // 更新 ScalarBar (LUT 已经绑定了 m_lut)
    if (m_cpScalarBar) {
        m_cpScalarBar->SetLookupTable(m_lut); // 确保同步
        m_cpScalarBar->SetNumberOfLabels(numLabels);
        historyValueRange.nums = numLabels;
    }

    valueRange.min = minVal;
    valueRange.max = maxVal;

    m_renderer->GetRenderWindow()->Render();
}



vtkSmartPointer<vtkLookupTable> flowViewer::makeJet()
{
    auto lut = vtkSmartPointer<vtkLookupTable>::New();
    lut->SetNumberOfTableValues(256);
    lut->SetHueRange(0.667, 0.0);  // Tecplot-like
    lut->SetSaturationRange(1.0, 1.0);
    lut->SetValueRange(1.0, 1.0);
    lut->Build();
    return lut;
}
vtkSmartPointer<vtkLookupTable> flowViewer::makeRainbow()
{
    auto lut = vtkSmartPointer<vtkLookupTable>::New();
    lut->SetNumberOfTableValues(256);
    lut->SetHueRange(0.8, -0.2);
    lut->SetSaturationRange(1.0, 1.0);
    lut->SetValueRange(1.0, 1.0);
    lut->Build();
    return lut;
}
vtkSmartPointer<vtkLookupTable> flowViewer::makeHot()
{
    auto lut = vtkSmartPointer<vtkLookupTable>::New();
    lut->SetNumberOfTableValues(256);

    for (int i = 0; i < 256; i++)
    {
        double t = i / 255.0;
        double r = std::min(1.0, 3 * t);
        double g = std::min(1.0, 3 * (t - 1.0/3.0));
        double b = std::min(1.0, 3 * (t - 2.0/3.0));

        lut->SetTableValue(i, r, std::max(0.0, g), std::max(0.0, b), 1.0);
    }
    lut->Build();
    return lut;
}
vtkSmartPointer<vtkLookupTable> flowViewer::makeCoolWarm()
{
    auto lut = vtkSmartPointer<vtkLookupTable>::New();
    lut->SetNumberOfTableValues(256);

    for (int i = 0; i < 256; i++)
    {
        double t = i / 255.0;
        lut->SetTableValue(i, t, t, 1.0 - t, 1.0);
    }
    lut->Build();
    return lut;
}
vtkSmartPointer<vtkLookupTable> flowViewer::makeGreys()
{
    auto lut = vtkSmartPointer<vtkLookupTable>::New();
    lut->SetNumberOfTableValues(256);

    for (int i = 0; i < 256; i++)
    {
        double t = i / 255.0;
        lut->SetTableValue(i, t, t, t, 1.0);
    }
    lut->Build();
    return lut;
}
// --- 示例槽函数 ---
void flowViewer::leftView()
{
    if (!m_renderer) return;
    auto cam = m_renderer->GetActiveCamera();
    cam->SetPosition(-1,0,0);
    cam->SetFocalPoint(0,0,0);
    cam->SetViewUp(0,0,1);
    //m_renderer->Render();
    m_renderer->ResetCamera();
    m_renderer->ResetCameraClippingRange();
    m_renderer->GetRenderWindow()->Render();
    QApplication::processEvents();


}

void flowViewer::topView()
{
    if (!m_renderer) return;
    auto cam = m_renderer->GetActiveCamera();
    cam->SetPosition(0,1,0);
    cam->SetFocalPoint(0,0,0);
    cam->SetViewUp(0,0,1);

    //m_renderer->Render();
    m_renderer->ResetCamera();
    m_renderer->ResetCameraClippingRange();
    m_renderer->GetRenderWindow()->Render();
    QApplication::processEvents();

}

void flowViewer::frontView()
{
    if (!m_renderer) return;
    auto cam = m_renderer->GetActiveCamera();
    cam->SetPosition(0,0,1);
    cam->SetFocalPoint(0,0,0);
    cam->SetViewUp(0,1,0);
    //m_renderer->Render();
    m_renderer->ResetCamera();
    m_renderer->ResetCameraClippingRange();
    m_renderer->GetRenderWindow()->Render();
    QApplication::processEvents();

}

void flowViewer::bestView()
{
    if (!m_renderer) return;

    vtkCamera* camera = m_renderer->GetActiveCamera();

    // ========== 等轴测视图关键设置 ==========
    camera->ParallelProjectionOn(); // 正交投影（必须，否则有透视）
    camera->SetPosition(1, -1, 1);   // 相机位置（等轴测经典视角）
    camera->SetFocalPoint(0, 0, 0); // 焦点在场景中心（可根据你的数据调整）
    camera->SetViewUp(-1, 1, 0);    // 视图上方方向（保证等轴测方向正确）

    // ========== 自动适配场景范围 ==========
    m_renderer->ResetCamera(); // 适配所有内容到视口（保持等轴测视角的同时缩放）

    // ========== 可选：调整正交缩放比例 ==========
    // camera->SetParallelScale(camera->GetParallelScale() * 1.1); // 轻微放大/缩小

    // ========== 刷新渲染 ==========
    m_renderer->GetRenderWindow()->Render();
    QApplication::processEvents();
}

void flowViewer::toggleScaleBar()
{
    //if (!m_scalarBar) return;
    //m_scalarBar->SetVisibility(!m_scalarBar->GetVisibility());
    //m_renderer->GetRenderWindow()->Render();
}
void flowViewer::showAxes(bool visible)
{
    if (!m_axesWidget) return;

    m_axesWidget->SetEnabled(visible ? 1 : 0);
    m_renderWindow->Render();
}
void flowViewer::setBackgroundStyle(int style)
{
    switch (style)
    {
    case 0: // 自定义白色主题搭配
        // 浅灰蓝渐变 → 白色，突出白色控件
        setBackground("LightSteelBlue", "White");
        m_cpScalarBar->GetTitleTextProperty()->SetColor(0.0,0.0,0.0);
        m_cpScalarBar->GetLabelTextProperty()->SetColor(0.0,0.0,0.0);
        break;

    case 1:
        // 渐变：浅蓝 → 白（最清晰）
        setBackground("LightSteelBlue", "White");
        break;

    case 2:
        // 渐变：天空蓝 → 白
        setBackground("SkyBlue", "White");
        break;

    case 3:
        // 渐变：CornflowerBlue → LightBlue
        setBackground("CornflowerBlue", "LightBlue");
        break;

    case 4:
        // 渐变：DarkSlateGray → Black（暗黑风）
        setBackground("DarkSlateGray", "Black");
        break;

    case 5:
        // 渐变：SteelBlue → LightSteelBlue（工程柔蓝）
        setBackground("SteelBlue", "LightSteelBlue");
        break;

    case 6:
        // 单色：白色（截图最干净）
        setBackground("White");
        break;

    default:
        // 默认：浅蓝 → 白
        setBackground("LightSteelBlue", "White");
        break;
    }
}
void flowViewer::toggleAxesWidget()
{
    if (!m_axesWidget) return;

    // 自动切换显示状态
    bool currentlyVisible = m_axesWidget->GetEnabled() != 0;
    m_axesWidget->SetEnabled(!currentlyVisible);

    // 刷新渲染
    if (m_renderer && m_renderer->GetRenderWindow())
        m_renderer->GetRenderWindow()->Render();
}
void flowViewer::toggleScalarBar()
{
    if (!m_cpScalarBar) return;

    // 当前可见性
    bool currentlyVisible = m_cpScalarBar->GetVisibility() != 0;

    // 切换显示/隐藏
    m_cpScalarBar->SetVisibility(!currentlyVisible);

    // 刷新渲染
    if (m_renderer && m_renderer->GetRenderWindow())
        m_renderer->GetRenderWindow()->Render();
}
void flowViewer::setLightStrength(double ambient, double directional)
{
    if (m_ambientLight)
        m_ambientLight->SetIntensity(ambient);

    if (m_directionalLight)
        m_directionalLight->SetIntensity(directional);

    m_renderer->GetRenderWindow()->Render();
}
void flowViewer::setDirectionalLightDirection(double x, double y, double z)
{
    if (!m_directionalLight) return;

    m_directionalLight->SetFocalPoint(0, 0, 0);
    m_directionalLight->SetPosition(-x, -y, -z);

    m_renderer->GetRenderWindow()->Render();
}
void flowViewer::setLightColor(double r, double g, double b)
{
    if (m_directionalLight)
        m_directionalLight->SetColor(r, g, b);

    m_renderer->GetRenderWindow()->Render();
}

void flowViewer::setAirfoil(const QVector<QVector<double>>& airfoil)
{

    if (!airfoilActor)
    {
        airfoilActor = createAirfoilActor();
        m_renderer->AddActor(airfoilActor);
    }


    updateAirfoilData(airfoil);






    // 3️⃣ 显示控制（关键）
    airfoilActor->SetVisibility(true);

    // 👉 如果有 mesh
    if (meshActor)
        meshActor->SetVisibility(false);

    // 4️⃣ 刷新
    m_renderer->ResetCamera();
    m_renderer->GetRenderWindow()->Render();
}
void flowViewer::updateAirfoilData(const QVector<QVector<double>>& airfoil)
{
    if (!airfoilActor) {
        qDebug() << "❌ airfoilActor is null";
        return;
    }

    auto mapper = airfoilActor->GetMapper();
    if (!mapper) {
        qDebug() << "❌ mapper is null";
        return;
    }

    auto polyData = vtkPolyData::SafeDownCast(mapper->GetInput());
    if (!polyData) {
        qDebug() << "❌ polyData is null";
        return;
    }

    // 👉 关键：lines 也要保证存在
    if (!polyData->GetLines())
    {
        auto cells = vtkSmartPointer<vtkCellArray>::New();
        polyData->SetLines(cells);
    }

    auto points = vtkSmartPointer<vtkPoints>::New();

    for (int i = 0; i < airfoil.size(); ++i)
    {
        double x = airfoil[i][0];
        double y = airfoil[i][1];
        double z = 0.0;


        points->InsertNextPoint(x, y, z);

    }

    polyData->SetPoints(points);

    // 🔥 同步更新 polyline（非常关键）
    auto polyLine = vtkSmartPointer<vtkPolyLine>::New();
    polyLine->GetPointIds()->SetNumberOfIds(airfoil.size());

    for (int i = 0; i < airfoil.size(); ++i)
    {
        polyLine->GetPointIds()->SetId(i, i);
    }

    auto cells = vtkSmartPointer<vtkCellArray>::New();
    cells->InsertNextCell(polyLine);

    polyData->SetLines(cells);

    polyData->Modified();
}
vtkSmartPointer<vtkActor> flowViewer::createAirfoilActor()
{
    qDebug() << "🔥 createAirfoilActor CALLED";

    auto points = vtkSmartPointer<vtkPoints>::New();

    auto cells = vtkSmartPointer<vtkCellArray>::New();

    auto polyData = vtkSmartPointer<vtkPolyData>::New();
    polyData->SetPoints(points);
    polyData->SetLines(cells);

    auto mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
    mapper->SetInputData(polyData);

    auto actor = vtkSmartPointer<vtkActor>::New();
    actor->SetMapper(mapper);

    qDebug() << "mapper inside create =" << mapper;
    qDebug() << "mapper inside actor =" << actor->GetMapper();

    actor->GetProperty()->SetColor(1,0,0);
    actor->GetProperty()->SetLineWidth(2);

    return actor;
}












void flowViewer::loadFlattenedGridAuto(const QString& filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        meshLogStatus("file not open!");
        //qDebug() << "❌ 文件打开失败";
        return;
    }


    QTextStream in(&file);

    // =========================
    // 1️⃣ 读取 header
    // =========================
    int block = 0;
    int nx = 0, ny = 0, nz = 0;

    in >> block;
    in >> nx >> ny >> nz;

    int N = nx * ny * nz;
    int meshNum = (nx - 1) * (ny - 1) * (nz - 1);

    meshLogStatus("Block:" + QString::number(block));
    //meshLogStatus("Dimensions:" + QString::number(block));
    meshLogStatus("Total points:" + QString::number(N));
    meshLogStatus("total number of cells:" + QString::number(meshNum));

    //qDebug() << "Block:" << block;
    //qDebug() << "Dimensions:" << nx << ny << nz;
    //qDebug() << "Total points:" << N;

    // =========================
    // 2️⃣ 读取全部数据
    // =========================
    std::vector<double> data;
    double val;

    while (!in.atEnd())
    {
        in >> val;
        data.push_back(val);
    }

    file.close();

    if ((int)data.size() < 3 * N)
    {
        qDebug() << "❌ 数据不足，期望:" << 3 * N << "实际:" << data.size();
        return;
    }

    // =========================
    // 3️⃣ 拆分 XYZ
    // =========================
    std::vector<double> X(data.begin(), data.begin() + N);
    std::vector<double> Y(data.begin() + N, data.begin() + 2 * N);
    std::vector<double> Z(data.begin() + 2 * N, data.begin() + 3 * N);

    // =========================
    // 4️⃣ 构建 points
    // =========================
    vtkSmartPointer<vtkPoints> points =
        vtkSmartPointer<vtkPoints>::New();

    points->SetNumberOfPoints(N);

    for (int i = 0; i < N; ++i)
    {
        points->SetPoint(i, X[i], Y[i], Z[i]);
    }

    // =========================
    // 5️⃣ structured grid
    // =========================
    vtkSmartPointer<vtkStructuredGrid> grid =
        vtkSmartPointer<vtkStructuredGrid>::New();

    grid->SetDimensions(nx, ny, nz);
    grid->SetPoints(points);

    // =========================
    // 6️⃣ surface
    // =========================
    vtkSmartPointer<vtkGeometryFilter> geo =
        vtkSmartPointer<vtkGeometryFilter>::New();

    geo->SetInputData(grid);

    // =========================
    // 7️⃣ mapper
    // =========================
    vtkSmartPointer<vtkPolyDataMapper> mapper =
        vtkSmartPointer<vtkPolyDataMapper>::New();

    mapper->SetInputConnection(geo->GetOutputPort());

    // =========================
    // 8️⃣ actor
    // =========================
    vtkSmartPointer<vtkActor> actor =
        vtkSmartPointer<vtkActor>::New();

    actor->SetMapper(mapper);

    // ==========================
    // ✔ 面 + 线框混合显示
    // ==========================
    actor->GetProperty()->SetRepresentationToSurface();
    actor->GetProperty()->EdgeVisibilityOn();
    actor->GetProperty()->SetEdgeColor(0.0, 0.0, 0.0);
    actor->GetProperty()->SetLineWidth(0.8);
    actor->GetProperty()->SetOpacity(1.0);
    actor->GetProperty()->SetLighting(true);

    // =========================
    // 9️⃣ render
    // =========================
    m_renderer->RemoveAllViewProps();
    m_renderer->AddActor(actor);


    m_renderer->ResetCamera();

    auto cam = m_renderer->GetActiveCamera();
    cam->Azimuth(45);
    cam->Elevation(30);
    cam->Zoom(1.5);

    m_renderer->ResetCameraClippingRange();
    m_renderer->GetRenderWindow()->Render();
}

void flowViewer::switchToAirfoil()
{
    if (currentMode == ShowAirfoil)
        return;

    airfoilActor->SetVisibility(true);
    meshActor->SetVisibility(false);

    currentMode = ShowAirfoil;

    m_renderer->ResetCamera();
    m_renderer->GetRenderWindow()->Render();
}
void flowViewer::switchToMesh()
{
    if (currentMode == ShowMesh)
        return;

    airfoilActor->SetVisibility(false);
    meshActor->SetVisibility(true);

    currentMode = ShowMesh;

    m_renderer->ResetCamera();
    m_renderer->GetRenderWindow()->Render();
}
