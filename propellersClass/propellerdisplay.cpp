#include "propellerdisplay.h"

#include <QDir>
#include <QFile>
#include <QSplitter>
#include <QGraphicsView>
#include <QFileDialog>
#include <QMessageBox>
#include <QtMath>
#include <QTextStream>
#include <QDebug>
#include <QApplication>
#include <QElapsedTimer>
#include <iostream>

#if defined(_MSC_VER) && (_MSC_VER >= 1600)
# pragma execution_character_set("utf-8")
#endif

propellerDisplay::propellerDisplay(QWidget *parent)
    : QWidget(parent){

    propDefineWidget = new QWidget();

    initialPropChartMenu();
    initialSteup();
    initialPropLibaries();
    initialAxis();
    initialPropDesignerModel();

    initialPropShowModel();
    buildPropDefineDialog();//螺旋桨定义框
    initialPropMessage();
    initialPropResultWidget();
    initialPropDesignWidget();
    initialPropAnalyseWidget();
    initialPropDefineWidget();
    initialView();
    initialPropSettingDialog();
    initialStreamDialog();
    initialAnalyseLogDialog();
    initialProgressDialog();
    connect(propLibary->addPropButton,&QPushButton::clicked,this,&propellerDisplay::addPropData);


    propDesign = new propellerDesign();

    bemtSolver = new propellerBemt();
    updateView();


    QObject::connect(resultChartViewA,&MyChartView::mousePositionChanged,this,&propellerDisplay::updateMousePositionRA);
    QObject::connect(resultChartViewA,&MyChartView::mousePressBegin,this,&propellerDisplay::getMousePressBeginPositionR);
    QObject::connect(resultChartViewA,&MyChartView::mouseReleaseEnd,this,&propellerDisplay::getMouseReleaseEndPositionR);
    QObject::connect(resultChartViewB,&MyChartView::mousePositionChanged,this,&propellerDisplay::updateMousePositionRB);
    QObject::connect(resultChartViewB,&MyChartView::mousePressBegin,this,&propellerDisplay::getMousePressBeginPositionR);
    QObject::connect(resultChartViewB,&MyChartView::mouseReleaseEnd,this,&propellerDisplay::getMouseReleaseEndPositionR);
    QObject::connect(resultChartViewC,&MyChartView::mousePositionChanged,this,&propellerDisplay::updateMousePositionRC);
    QObject::connect(resultChartViewC,&MyChartView::mousePressBegin,this,&propellerDisplay::getMousePressBeginPositionR);
    QObject::connect(resultChartViewC,&MyChartView::mouseReleaseEnd,this,&propellerDisplay::getMouseReleaseEndPositionR);
    QObject::connect(resultChartViewD,&MyChartView::mousePositionChanged,this,&propellerDisplay::updateMousePositionRD);
    QObject::connect(resultChartViewD,&MyChartView::mousePressBegin,this,&propellerDisplay::getMousePressBeginPositionR);
    QObject::connect(resultChartViewD,&MyChartView::mouseReleaseEnd,this,&propellerDisplay::getMouseReleaseEndPositionR);

    QObject::connect(chartViewB,&MyChartView::mousePositionChanged,this,&propellerDisplay::updateMousePositionTwist);
    QObject::connect(chartViewB,&MyChartView::mousePressBegin,this,&propellerDisplay::getMousePressBeginPosition);
    QObject::connect(chartViewB,&MyChartView::mouseReleaseEnd,this,&propellerDisplay::getMouseReleaseEndPosition);

    QObject::connect(chartViewC,&MyChartView::mousePositionChanged,this,&propellerDisplay::updateMousePositionChord);
    QObject::connect(chartViewC,&MyChartView::mousePressBegin,this,&propellerDisplay::getMousePressBeginPosition);
    QObject::connect(chartViewC,&MyChartView::mouseReleaseEnd,this,&propellerDisplay::getMouseReleaseEndPosition);

    QObject::connect(resultChartViewA,&MyChartView::mouseRightLeftPress,this,&propellerDisplay::showChartAMenu);
    QObject::connect(resultChartViewB,&MyChartView::mouseRightLeftPress,this,&propellerDisplay::showChartBMenu);
    QObject::connect(resultChartViewC,&MyChartView::mouseRightLeftPress,this,&propellerDisplay::showChartCMenu);
    QObject::connect(resultChartViewD,&MyChartView::mouseRightLeftPress,this,&propellerDisplay::showChartDMenu);
    QObject::connect(resultView,&MyChartView::mouseRightLeftPress,this,&propellerDisplay::showChartMenu);
    connect(propDesign,&propellerDesign::emitMessage,this,&propellerDisplay::updateMessage);
    connect(propDesign,&propellerDesign::emitProgressValue,this,&propellerDisplay::changeProgressValue);


}
void propellerDisplay::initialPropAnalyseWidget(){

    // 创建子控件
    propAnalyseWidget = new QWidget();
    propDisplayWidgetE = new QVTKOpenGLNativeWidget();
    propTreeWidget = new QTreeWidget();
    //结果视图
    resultChart = new QChart();
    resultView = new MyChartView(resultChart,propChartMenu);
    resultView->setRenderHint(QPainter::Antialiasing);
    resultAxisX = new QValueAxis();
    resultAxisY = new QValueAxis();

    autoSeries = new QLineSeries;





    //resultChart->legend()->setVisible(true);

    resultChart->addAxis(resultAxisX,Qt::AlignBottom);
    resultChart->addAxis(resultAxisY,Qt::AlignLeft);
    resultChart->addSeries(autoSeries);
    autoSeries->attachAxis(resultAxisX);
    autoSeries->attachAxis(resultAxisY);
    resultView->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    resultView->hide();

    removeSeriesLegendItem(resultChart,autoSeries);
    QPen pen;
    pen.setStyle(Qt::DashLine);
    pen.setColor(Qt::red);
    pen.setWidth(2);
    autoSeries->setPen(pen);



    // 设置布局管理器
    //designMainVLayout = new QVBoxLayout(wingDesignWidget);
    listHboxLayout = new QHBoxLayout();
    listVboxLayout = new QVBoxLayout();
    viewChangeHLayout = new QHBoxLayout();
    propWidgetVLayout = new QVBoxLayout();

    // 设置树形控件
    propTreeWidget->setColumnCount(1); // 设置列
    propTreeWidget->setHeaderLabel(tr("螺旋桨")); // 设置标题
    propTreeWidget->setMaximumWidth(300);

    // 创建按钮
    changeModelViewButton = new QPushButton("3D视图");
    changeResultViewButton = new QPushButton("结果视图");
    propStateCombox = new QComboBox();
    propResultTypeCombox = new QComboBox();

    changeResultViewButton->setFixedWidth(100);
    changeModelViewButton->setFixedWidth(100);

    // 创建标签
    propMessageTextLabel = new QLabel();
    propMessageTextLabel->setFixedHeight(30);

    propResultTypeCombox->addItem("推力-速度 曲线");
    propResultTypeCombox->addItem("推力-转速 曲线");
    propResultTypeCombox->addItem("效率曲线");
    propResultTypeCombox->addItem("推力系数曲线");
    propResultTypeCombox->addItem("功率系数曲线");
    propResultTypeCombox->addItem("dT/dr");
    propResultTypeCombox->addItem("dQ/dr");



    propResultTypeCombox->setFixedWidth(120);
    propStateCombox->setFixedWidth(120);
    propStateCombox->setEnabled(false);
    propResultTypeCombox->setEnabled(false);
    changeModelViewButton->setEnabled(false);

    // 添加控件到水平布局中
    viewChangeHLayout->addWidget(changeModelViewButton);
    viewChangeHLayout->addWidget(changeResultViewButton);
    viewChangeHLayout->addWidget(propStateCombox);
    viewChangeHLayout->addWidget(propResultTypeCombox);
    viewChangeHLayout->addWidget(propMessageTextLabel);

    propWidgetVLayout->addLayout(viewChangeHLayout);
    propWidgetVLayout->addWidget(propDisplayWidgetE);
    propWidgetVLayout->addWidget(resultView);
    // 初始化布局

    listVboxLayout->addWidget(propTreeWidget);
    listHboxLayout->addLayout(listVboxLayout);
    listHboxLayout->addLayout(propWidgetVLayout);


    propAnalyseWidget->setLayout(listHboxLayout);

    connect(propTreeWidget,&QTreeWidget::itemClicked, this, &propellerDisplay::changePropDisplay);
    connect(propTreeWidget,&QTreeWidget::itemClicked, this, &propellerDisplay::changePropData);
    connect(propTreeWidget,&QTreeWidget::itemClicked, this, &propellerDisplay::showPropSettingVinfDialog);
    connect(propTreeWidget,&QTreeWidget::itemClicked, this, &propellerDisplay::showPropStreamLineView);
    connect(propTreeWidget,&QTreeWidget::itemClicked, this, &propellerDisplay::showPropPressureContourView);
    connect(changeModelViewButton,&QPushButton::clicked,this,&propellerDisplay::changeUIForPropDesign);
    connect(changeResultViewButton,&QPushButton::clicked,this,&propellerDisplay::changeUIForPropResult);
    connect(propResultTypeCombox, QOverload<int>::of(&QComboBox::activated), this, &propellerDisplay::drawPropResult);
    connect(propStateCombox, QOverload<int>::of(&QComboBox::activated), this, &propellerDisplay::updateViewText);
}
void propellerDisplay::initialPropDesignerModel(){
    propDefineHLayout = new QHBoxLayout(propDefineWidget);

    propDefineWidget->setMinimumWidth(1400);
    propDefineWidget->setMinimumHeight(900);
    // 创建QScrollArea，但不将它的父类设置为propDefineWidget，避免父子控件层次不合理。
    scrolArea = new QScrollArea();
    scrolArea->setWidgetResizable(true);  // 允许QScrollArea根据内容自动调整
    scrolArea->setMinimumHeight(300);     // 设置滚动区域的最小高度，便于测试

    // propDefineBox将作为QScrollArea的子控件
    propDefineBox = new QGroupBox();


    // propDefineBox布局
    propDefineGLayout = new QGridLayout(propDefineBox);

    chordLengthLabel = new QLabel("弦长(mm)", propDefineBox);
    spanLabel = new QLabel("翼展(mm)", propDefineBox);
    twistAngleLabel = new QLabel("扭转角(°)", propDefineBox);
    offsetLengthLabel = new QLabel("偏移(mm)", propDefineBox);
    nameLabel = new QLabel("翼型名称", propDefineBox);
    xGridNumLabel = new QLabel("X网格数", propDefineBox);
    yGridNumLabel = new QLabel("Y网格数", propDefineBox);

    // 将所有标签添加到GridLayout中
    propDefineGLayout->addWidget(spanLabel, 0, 1);
    propDefineGLayout->addWidget(chordLengthLabel, 0, 2);
    propDefineGLayout->addWidget(offsetLengthLabel, 0, 3);
    propDefineGLayout->addWidget(twistAngleLabel, 0, 4);
    propDefineGLayout->addWidget(xGridNumLabel, 0, 5);
    propDefineGLayout->addWidget(yGridNumLabel, 0, 6);
    propDefineGLayout->addWidget(nameLabel, 0, 7);

    // 设置QScrollArea显示propDefineBox
    scrolArea->setWidget(propDefineBox);

    // propDisplayWidgetB的设置
    propDisplayWidgetB = new QVTKOpenGLNativeWidget();
    propDisplayWidgetB->setMinimumHeight(600);

    // 创建右侧消息框
    propMessageBox = new QGroupBox(propDefineWidget);
    propMessageLayout = new QGridLayout(propMessageBox);
    propMessageBox->setFixedWidth(300);  // 固定宽度

    // 左侧布局：QScrollArea + propDisplayWidgetB
    propDefineVLayout = new QVBoxLayout();
    propDefineVLayout->addWidget(scrolArea);            // 将QScrollArea添加到左侧布局
    propDefineVLayout->addWidget(propDisplayWidgetB);   // 将显示区域添加到左侧布局

    // 总体布局：左侧布局 + 右侧消息框
    propDefineHLayout->addLayout(propDefineVLayout);
    propDefineHLayout->addWidget(propMessageBox);

    // 设置总布局
    propDefineWidget->setLayout(propDefineHLayout);


}
void propellerDisplay::initialPropDefineWidget(){
    rendererE = vtkSmartPointer<vtkRenderer>::New();
    renwinE = vtkSmartPointer<vtkGenericOpenGLRenderWindow>::New();
    renwinE->AddRenderer(rendererE);
    propDisplayWidgetE->setRenderWindow(renwinE);
    //
    sphereSource = vtkSmartPointer<vtkSphereSource>::New();
    sphereSource->SetRadius(0.005);
    sphereSource->SetThetaResolution(50);
    sphereSource->SetPhiResolution(50);
    //
    pointsE = vtkSmartPointer<vtkPoints>::New();
    pointsS = vtkSmartPointer<vtkPoints>::New();
    pointsC = vtkSmartPointer<vtkPoints>::New();

    linesE = vtkSmartPointer<vtkCellArray>::New();
    linesS = vtkSmartPointer<vtkCellArray>::New();
    linesC = vtkSmartPointer<vtkCellArray>::New();

    profilePolyDataE = vtkSmartPointer<vtkPolyData>::New();
    profilePolyDataE->SetPoints(pointsE);
    profilePolyDataE->SetPolys(linesE);

    profilePolyDataS = vtkSmartPointer<vtkPolyData>::New();
    profilePolyDataS->SetPoints(pointsS);
    profilePolyDataS->SetLines(linesS);

    profilePolyDataC = vtkSmartPointer<vtkPolyData>::New();
    profilePolyDataC->SetPoints(pointsC);
    profilePolyDataC->SetPolys(linesC);

    mapperE = vtkSmartPointer<vtkPolyDataMapper>::New();
    //mapperB->SetInputConnection(appendFilterB->GetOutputPort());
    mapperE->SetInputData(profilePolyDataE);
    actorE = vtkSmartPointer<vtkActor>::New();
    actorE->SetMapper(mapperE);

    mapperS = vtkSmartPointer<vtkPolyDataMapper>::New();
    mapperS->SetInputData(profilePolyDataS);
    actorS = vtkSmartPointer<vtkActor>::New();
    actorS->GetProperty()->SetColor(0.8, 0.6, 1.0); // 红色
    actorS->SetMapper(mapperS);

    mapperC = vtkSmartPointer<vtkPolyDataMapper>::New();
    mapperC->SetInputData(profilePolyDataC);
    actorC = vtkSmartPointer<vtkActor>::New();
    actorC->SetMapper(mapperC);

    mapperD = vtkSmartPointer<vtkPolyDataMapper>::New();
    //mapperB->SetInputConnection(appendFilterB->GetOutputPort());
    mapperD->SetInputConnection(sphereSource->GetOutputPort());;
    actorD = vtkSmartPointer<vtkActor>::New();
    actorD->SetMapper(mapperD);
    actorD->GetProperty()->SetColor(1.0, 0.0, 0.0);




    // 初始化文本演员
    textActorA = vtkSmartPointer<vtkTextActor>::New();
    textActorA->SetInput("");
    textActorA->SetPosition(10, 10); // 设置文本位置为左下角
    textActorA->GetTextProperty()->SetJustificationToLeft();
    textActorA->GetTextProperty()->SetVerticalJustificationToBottom();

    QString path1 = QDir::currentPath() + "/resoure/language/chinese.ttf";
    //QString path1 = ":/language/chinese.ttf";
    std::string path2 = path1.toUtf8().constData();


    textProperty = vtkSmartPointer<vtkTextProperty>::New();
    textProperty->SetFontFamily(VTK_FONT_FILE);
    textProperty->SetFontFile(path2.c_str()); // 设置支持中文的字体文件
    textProperty->SetFontSize(18); // 初始字体大小
    textProperty->SetColor(1.0, 1.0, 1.0); // 白色文本
    textActorA->SetTextProperty(textProperty);

    // 初始化颜色数组
    colorE = vtkSmartPointer<vtkUnsignedCharArray>::New();
    colorE->SetNumberOfComponents(3); // 设置为 RGB
    colorE->SetName("Colors");
    profilePolyDataE->GetCellData()->SetScalars(colorE);

    colors = vtkSmartPointer<vtkUnsignedCharArray>::New();
    colors->SetNumberOfComponents(3); // 设置为 RGB
    colors->SetName("Colors");
    profilePolyDataC->GetCellData()->SetScalars(colors);

    actorE->VisibilityOff();
    actorS->VisibilityOff();
    actorC->VisibilityOff();
    actorD->VisibilityOn();
    // 将 Actor 添加到渲染器中
    rendererE->AddActor(actorE);
    rendererE->AddActor(actorS);
    rendererE->AddActor(actorC);
    rendererE->AddActor(actorD);
    rendererE->AddActor(textActorA);






    // 设置背景颜色
    rendererE->SetBackground(0.1, 0.2, 0.4); // 深蓝色背景

    // 获取 QVTKOpenGLNativeWidget 提供的交互器
    vtkRenderWindowInteractor* interactor = this->propDisplayWidgetE->interactor();

    // 创建并设置交互样式
    vtkSmartPointer<vtkInteractorStyleTrackballCamera> style = vtkSmartPointer<vtkInteractorStyleTrackballCamera>::New();
    interactor->SetInteractorStyle(style);

    // 创建坐标轴
    vtkSmartPointer<vtkAxesActor> axes = vtkSmartPointer<vtkAxesActor>::New();
    axes->SetTotalLength(2.0, 2.0, 2.0); // 设置坐标轴的长度
    axes->SetShaftType(0);
    axes->SetCylinderRadius(0.05);

    // 创建 OrientationMarkerWidget
    orientationMarkerE = vtkSmartPointer<vtkOrientationMarkerWidget>::New();
    orientationMarkerE->SetOrientationMarker(axes);
    orientationMarkerE->SetInteractor(interactor);
    orientationMarkerE->SetViewport(0.8, 0.0, 1.0, 0.2); // 设置坐标轴的位置和大小
    orientationMarkerE->SetEnabled(1);
    orientationMarkerE->InteractiveOff(); // 禁止交互

    // 渲染并启动交互
    renwinE->Render();





}
void propellerDisplay::initialPropMessage(){
    propRealDiameterLabel = new QLabel(propDefineWidget);
    propRealMeshNumLabel = new QLabel(propDefineWidget);
    propSoildityLabel = new QLabel(propDefineWidget);
    areaLabel = new QLabel(propDefineWidget);
    propNumLabel = new QLabel(propDefineWidget);
    propMeshVTypeLabel = new QLabel(propDefineWidget);
    propMeshUTypeLabel = new QLabel(propDefineWidget);
    propYteTypeLabel = new QLabel(propDefineWidget);
    propYteValueLabel = new QLabel(propDefineWidget);
    propNameLabel = new QLabel(propDefineWidget);
    propSaveButton = new QPushButton(propDefineWidget);



    propRealDiameterTextLabel = new QLabel(propDefineWidget);
    propRealMeshNumTextLabel = new QLabel(propDefineWidget);
    propSoildityTextLabel = new QLabel(propDefineWidget);
    areaTextLabel = new QLabel(propDefineWidget);
    propNumEdit = new QLineEdit(propDefineWidget);
    propMeshVTypeCombobox = new QComboBox(propDefineWidget);
    propMeshUTypeCombobox = new QComboBox(propDefineWidget);
    propYteTypeCombobox = new QComboBox(propDefineWidget);
    propYteValueEdit = new QLineEdit(propDefineWidget);
    propNameEdit = new QLineEdit(propDefineWidget);
    propOutputButton = new QPushButton(propDefineWidget);






    propRealDiameterLabel->setText("直径(寸):   ");
    propRealMeshNumLabel->setText("网格数量:   ");
    propSoildityLabel->setText("实度:   ");

    areaLabel->setText("翼面积");
    propNumLabel->setText("桨叶数目");
    propNumEdit->setText("2");
    propMeshVTypeLabel->setText("弦向网格分布方式");
    propMeshUTypeLabel->setText("展向网格分布方式");

    propYteTypeLabel->setText("尾缘分布方式");
    propYteValueLabel->setText("尾缘厚度(mm)");
    propNameLabel->setText("螺旋桨名称");
    propNameEdit->setText("螺旋桨");
    propSaveButton->setText("保存");
    propOutputButton->setText("输出Catia文件");

    propMeshVTypeCombobox->addItem("平均分布");
    propMeshVTypeCombobox->addItem("余弦分布");

    propMeshUTypeCombobox->addItem("平均分布");
    propMeshUTypeCombobox->addItem("比例分布A");

    propYteValueEdit->setText("0");

    propYteTypeCombobox->addItem("等厚度分布");
    propYteTypeCombobox->addItem("比例分布");

    verticaSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

    propMessageLayout->addWidget(propRealDiameterLabel,0,0,1,1);
    propMessageLayout->addWidget(propSoildityLabel,1,0,1,1);
    propMessageLayout->addWidget(propRealMeshNumLabel,3,0,1,1);

    propMessageLayout->addWidget(areaLabel,2,0,1,1);
    propMessageLayout->addItem(verticaSpacer,4,0,1,1);
    propMessageLayout->addWidget(propNumLabel,5,0,1,1);
    propMessageLayout->addWidget(propMeshVTypeLabel,6,0,1,1);
    propMessageLayout->addWidget(propMeshUTypeLabel,7,0,1,1);
    propMessageLayout->addWidget(propYteTypeLabel,8,0,1,1);
    propMessageLayout->addWidget(propYteValueLabel,9,0,1,1);
    propMessageLayout->addWidget(propNameLabel,10,0,1,1);
    propMessageLayout->addWidget(propSaveButton,11,0,1,1);

    propMessageLayout->addWidget(propRealDiameterTextLabel,0,1,1,1);
    propMessageLayout->addWidget(propSoildityTextLabel,1,1,1,1);
    propMessageLayout->addWidget(propRealMeshNumTextLabel,3,1,1,1);
    propMessageLayout->addWidget(areaTextLabel,2,1,1,1);
    propMessageLayout->addWidget(propNumEdit,5,1,1,1);
    propMessageLayout->addWidget(propMeshVTypeCombobox,6,1,1,1);
    propMessageLayout->addWidget(propMeshUTypeCombobox,7,1,1,1);
    propMessageLayout->addWidget(propYteTypeCombobox,8,1,1,1);
    propMessageLayout->addWidget(propYteValueEdit,9,1,1,1);
    propMessageLayout->addWidget(propNameEdit,10,1,1,1);
    propMessageLayout->addWidget(propOutputButton,11,1,1,1);



    connect(propMeshVTypeCombobox,QOverload<const int>::of(&QComboBox::activated),this,&propellerDisplay::updatePropData);
    connect(propMeshUTypeCombobox,QOverload<const int>::of(&QComboBox::activated),this,&propellerDisplay::updatePropData);
    connect(propYteTypeCombobox,QOverload<const int>::of(&QComboBox::activated),this,&propellerDisplay::updatePropData);
    connect(propYteValueEdit,&QLineEdit::editingFinished,this,&propellerDisplay::updatePropData);
    connect(propNumEdit,&QLineEdit::editingFinished,this,&propellerDisplay::updatePropData);

    connect(propSaveButton,&QPushButton::clicked,this,&propellerDisplay::savePropellerData);
    connect(propOutputButton,&QPushButton::clicked,this,&propellerDisplay::outputAnalysePropData);


}
void propellerDisplay::initialStreamDialog(){
    streamSettingDialog = new QDialog();
    QGridLayout *layout = new QGridLayout(streamSettingDialog);
    QPushButton *saveButton = new QPushButton(streamSettingDialog);
    QPushButton *cancelButton = new QPushButton(streamSettingDialog);
    for(int i = 0;i<4;i++){
        QLabel *label = new QLabel(streamSettingDialog);
        QLineEdit *edit = new QLineEdit(streamSettingDialog);
        streamLabelArray[i] = label;
        streamEditArray[i] = edit;
        layout->addWidget(label,i,0,1,1);
        layout->addWidget(edit,i,1,1,1);
    }
    layout->addWidget(saveButton,4,0,1,1);
    layout->addWidget(cancelButton,4,1,1,1);
    streamLabelArray[0]->setText("流线起点X坐标");
    streamLabelArray[1]->setText("流线高度");
    streamLabelArray[2]->setText("计算时间");
    streamLabelArray[3]->setText("计算步长");
    saveButton->setText("确认");
    cancelButton->setText("取消");

    streamEditArray[0]->setText("0");
    streamEditArray[1]->setText("0");
    streamEditArray[2]->setText("0.05");
    streamEditArray[3]->setText("0.0005");

    connect(saveButton,&QPushButton::clicked,this,&propellerDisplay::showPropStreamLine);
    connect(cancelButton,&QPushButton::clicked,streamSettingDialog,&QDialog::hide);

}
void propellerDisplay::initialPropChartMenu(){
    propChartMenu = new QMenu();
    QAction *rAction1 = new QAction("推力-速度  曲线",propChartMenu);
    QAction *rAction2 = new QAction("推力-转速  曲线",propChartMenu);
    QAction *rAction3 = new QAction("效率曲线",propChartMenu);
    QAction *rAction4 = new QAction("推力系数曲线",propChartMenu);
    QAction *rAction5 = new QAction("功率系数曲线",propChartMenu);
    QAction *rAction6 = new QAction("数据导出",propChartMenu);
    QAction *rAction7 = new QAction("添加点",propChartMenu);
    QAction *rAction8 = new QAction("删除点",propChartMenu);

    rAction1->setProperty("actions",0);
    rAction2->setProperty("actions",1);
    rAction3->setProperty("actions",2);
    rAction4->setProperty("actions",3);
    rAction5->setProperty("actions",4);
    rAction6->setProperty("actions",5);
    rAction7->setProperty("actions",6);
    rAction8->setProperty("actions",7);


    connect(rAction1,&QAction::triggered,this,&propellerDisplay::changePropResultType);
    connect(rAction2,&QAction::triggered,this,&propellerDisplay::changePropResultType);
    connect(rAction3,&QAction::triggered,this,&propellerDisplay::changePropResultType);
    connect(rAction4,&QAction::triggered,this,&propellerDisplay::changePropResultType);
    connect(rAction5,&QAction::triggered,this,&propellerDisplay::changePropResultType);
    connect(rAction6,&QAction::triggered,this,&propellerDisplay::changePropResultType);
    connect(rAction7,&QAction::triggered,this,&propellerDisplay::addPropDataPoint);
    connect(rAction8,&QAction::triggered,this,&propellerDisplay::clearPropDataPoint);


    rAction1->setCheckable(true);
    rAction2->setCheckable(true);
    rAction3->setCheckable(true);
    rAction4->setCheckable(true);
    rAction5->setCheckable(true);
    rAction6->setCheckable(true);
    rAction7->setCheckable(true);
    rAction8->setCheckable(true);

    propChartMenu->addAction(rAction1);
    propChartMenu->addAction(rAction2);
    propChartMenu->addAction(rAction3);
    propChartMenu->addAction(rAction4);
    propChartMenu->addAction(rAction5);
    propChartMenu->addAction(rAction6);
    propChartMenu->addAction(rAction7);
    propChartMenu->addAction(rAction8);

    propActionArray.append(rAction1);
    propActionArray.append(rAction2);
    propActionArray.append(rAction3);
    propActionArray.append(rAction4);
    propActionArray.append(rAction5);
    propActionArray.append(rAction6);
    propActionArray.append(rAction7);
    propActionArray.append(rAction8);



}
void propellerDisplay::changePropResultType(){
    QAction *action = qobject_cast<QAction*>(sender());
    int index = action->property("actions").toInt();
    if (action) {
        // 取消所有其他动作的选中状态
        for (QAction *act : rChartMenu->actions()) {
            act->setChecked(false);
        }
    }

    action->setChecked(true);
    if(index <= 4 ){
        propChartTypeIndex = index;
        drawPropResult(index);
    }else{
        exportChartData(resultChart);
    }

}
void propellerDisplay::buildPropDesignerModel(){
    removeWidget();//删除上一次的widget
    SpanNumB = spanNumEditB->text().toInt();

    if(SpanNumB < 5)
        SpanNumB = 5;
    for(int i = 1;i<SpanNumB + 1;i++){
        QLabel *label = new QLabel(propDefineWidget);
        QLineEdit *edit1 = new QLineEdit(propDefineWidget);
        QLineEdit *edit2 = new QLineEdit(propDefineWidget);
        QLineEdit *edit3 = new QLineEdit(propDefineWidget);
        QLineEdit *edit4 = new QLineEdit(propDefineWidget);
        QLineEdit *edit5 = new QLineEdit(propDefineWidget);
        QLineEdit *edit6 = new QLineEdit(propDefineWidget);
        QComboBox *combox = new QComboBox(propDefineWidget);

        QString names = "截面(" + QString::number(i) + ")";
        label->setText(names);
        if(!airfoilArray.isEmpty() && !propAirfoilNameArray.isEmpty()){
            for(int j = 0;j<propAirfoilNameArray.length();j++){
                combox->addItem(propAirfoilNameArray[j]);
            }
        }else{
            combox->addItem("平板");

        }
        combox->setCurrentIndex(0);

        propDefineGLayout->addWidget(label,i,0,1,1);
        propDefineGLayout->addWidget(edit1,i,1,1,1);
        propDefineGLayout->addWidget(edit2,i,2,1,1);
        propDefineGLayout->addWidget(edit3,i,3,1,1);
        propDefineGLayout->addWidget(edit4,i,4,1,1);
        propDefineGLayout->addWidget(edit5,i,5,1,1);
        propDefineGLayout->addWidget(edit6,i,6,1,1);

        propDefineGLayout->addWidget(combox,i,7,1,1);
        profileLabel.append(label);
        profileSpanEdit.append(edit1);
        profilechordLengthEdit.append(edit2);
        profileOffsetLengthEdit.append(edit3);
        profileTwistAngleEdit.append(edit4);
        profileXGridEdit.append(edit5);
        profileYGridEdit.append(edit6);
        profileAirfoilChoiceCombobox.append(combox);

        connect(edit1,&QLineEdit::editingFinished,this,&propellerDisplay::updatePropData);
        connect(edit2,&QLineEdit::editingFinished,this,&propellerDisplay::updatePropData);
        connect(edit3,&QLineEdit::editingFinished,this,&propellerDisplay::updatePropData);
        connect(edit4,&QLineEdit::editingFinished,this,&propellerDisplay::updatePropData);
        connect(edit5,&QLineEdit::editingFinished,this,&propellerDisplay::updatePropData);
        connect(edit6,&QLineEdit::editingFinished,this,&propellerDisplay::updatePropData);
        connect(combox,QOverload<const int>::of(&QComboBox::activated),this,&propellerDisplay::updatePropData);



    }
    int maxWidth = 40 + SpanNumB * 30;
    propDefineBox->setFixedHeight(maxWidth);
}
void propellerDisplay::updatePropEdit(const int index){
    removeWidget();
    updatePropDesignerModel();
    updatePropData();

    for(int i = 0;i<propArray[index].chordLengthW.length();i++){
        double spanVal = propArray[index].spanW[i] * 1000;
        double chordVal = propArray[index].chordLengthW[i] * 1000;
        double offsetVal = propArray[index].offsetLengthW[i] * 1000;
        double twistVal = propArray[index].twistAngleW[i];
        //double dihedVal = propArray[index].dihedralAngleW[i];
        int xVal = propArray[index].gridV[i];
        int yVal = propArray[index].gridU[i];

        profileAirfoilChoiceCombobox[i]->blockSignals(true);
        int indexTmp = getAirfoilIndex(propArray[index].airfoilNameArray[i]);
        profileAirfoilChoiceCombobox[i]->setCurrentIndex(indexTmp);
        profileAirfoilChoiceCombobox[i]->blockSignals(false);

        profileLabel[i]->setText("截面(" + QString::number(i + 1) + ")");
        profileSpanEdit[i]->setText(QString::number(spanVal));
        profilechordLengthEdit[i]->setText(QString::number(chordVal));
        profileOffsetLengthEdit[i]->setText(QString::number(offsetVal));
        profileTwistAngleEdit[i]->setText(QString::number(twistVal));
        //profileDihedralAngleEdit[i]->setText(QString::number(dihedVal));
        profileXGridEdit[i]->setText(QString::number(xVal));
        profileYGridEdit[i]->setText(QString::number(yVal));
        propNumEdit->setText(QString::number(propArray[index].num));
    }



}
void propellerDisplay::initialPropShowModel(){
     // 如果 propDisplayWidgetB 是指针类型

    // 初始化 VTK 相关对象
    rendererB = vtkSmartPointer<vtkRenderer>::New();
    renwinB = vtkSmartPointer<vtkGenericOpenGLRenderWindow>::New();
    pointsB = vtkSmartPointer<vtkPoints>::New();
    linesB = vtkSmartPointer<vtkCellArray>::New();
    profilePolyDataB = vtkSmartPointer<vtkPolyData>::New();
    appendFilterB = vtkSmartPointer<vtkAppendPolyData>::New();
    mapperB = vtkSmartPointer<vtkPolyDataMapper>::New();
    actorB = vtkSmartPointer<vtkActor>::New();
    lightKitB = vtkSmartPointer<vtkLightKit>::New();
    //orientationMarkerB = vtkSmartPointer<vtkOrientationMarkerWidget>::New();


    propDisplayWidgetB->setRenderWindow(renwinB);
    renwinB->AddRenderer(rendererB);

    profilePolyDataB->SetPoints(pointsB);
    profilePolyDataB->SetLines(linesB);

    mapperB->SetInputData(profilePolyDataB);


    actorB->SetMapper(mapperB);

    rendererB->AddActor(actorB);


    // 设置背景颜色
    rendererB->SetBackground(0.1, 0.2, 0.4); // 深蓝色背景

    lightKitB = vtkSmartPointer<vtkLightKit>::New();
    lightKitB->AddLightsToRenderer(rendererB);
    renwinB->Render();




    //propDefineHLayout->addLayout(propDefineVLayout);
}
void propellerDisplay::initialPropSettingDialog(){
    propSettingDialog = new QDialog();
    propSettingVLayout = new QVBoxLayout(propSettingDialog);
    propSettingLayout = new QGridLayout();
    propSettingHLayoutA = new QHBoxLayout();
    propSettingHLayoutB = new QHBoxLayout();


    heightLabel = new QLabel(propSettingDialog);
    heightEdit = new QLineEdit(propSettingDialog);
    wakeStepLabel = new QLabel(propSettingDialog);
    wakeStepEdit = new QLineEdit(propSettingDialog);
    matrixLabel = new QLabel(propSettingDialog);
    symbolLabel = new QLabel(propSettingDialog);
    rowsEdit = new QLineEdit(propSettingDialog);
    columnsEdit = new QLineEdit(propSettingDialog);
    updateSettingButton = new QPushButton(propSettingDialog);

    propSolverModelLabel = new QLabel(propSettingDialog);
    propSolverModelCombox = new QComboBox(propSettingDialog);
    propTipLabel = new QLabel(propSettingDialog);
    propTipCheckbox = new QCheckBox(propSettingDialog);

    saveSettingButton = new QPushButton(propSettingDialog);
    cancelSettingButton = new QPushButton(propSettingDialog);


    heightLabel->setText("高度(米)");
    heightEdit->setText("0");
    wakeStepLabel->setText("尾涡迭代步数");
    wakeStepEdit->setText("100");



    matrixLabel->setText("求解矩阵");
    symbolLabel->setText("×");
    rowsEdit->setText("3");
    columnsEdit->setText("3");

    propSolverModelLabel->setText("求解器");
    propSolverModelCombox->addItem("动量叶素法");
    propSolverModelCombox->addItem("涡格法");
    propTipLabel->setText("翼尖涡修正");
    propTipCheckbox->setChecked(false);

    updateSettingButton->setText("刷新矩阵");
    updateSettingButton->setEnabled(false);
    saveSettingButton->setText("保存");
    cancelSettingButton->setText("取消");

    propSettingHLayoutA->addWidget(heightLabel);
    propSettingHLayoutA->addWidget(heightEdit);
    propSettingHLayoutA->addWidget(wakeStepLabel);
    propSettingHLayoutA->addWidget(wakeStepEdit);
    propSettingHLayoutA->addWidget(matrixLabel);
    propSettingHLayoutA->addWidget(rowsEdit);
    propSettingHLayoutA->addWidget(symbolLabel);
    propSettingHLayoutA->addWidget(columnsEdit);
    propSettingHLayoutA->addWidget(updateSettingButton);

    propSettingHLayoutB->addWidget(saveSettingButton);
    propSettingHLayoutB->addWidget(cancelSettingButton);

    propSettingLayout->addWidget(propSolverModelLabel,0,0,1,1);
    propSettingLayout->addWidget(propSolverModelCombox,0,1,1,1);
    propSettingLayout->addWidget(propTipLabel,1,0,1,1);
    propSettingLayout->addWidget(propTipCheckbox,1,1,1,1);

    for(int i = 1;i<4;i++){
        int tmp2 = i * 2000;
        int tmp1 = i * 5;
        QLabel *labelA = new QLabel(propSettingDialog);
        QLabel *labelB = new QLabel(propSettingDialog);
        QLineEdit *editA = new QLineEdit(propSettingDialog);
        QLineEdit *editB = new QLineEdit(propSettingDialog);
        labelA->setText("速度(" + QString::number(i) + ")");
        labelB->setText("转速(" + QString::number(i) + ")");
        editA->setText(QString::number(tmp1,'f',2));
        editB->setText(QString::number(tmp2,'f',2));
        velocityLabelArray.append(labelA);
        velocityEditArray.append(editA);
        rpmLabelArray.append(labelB);
        rpmEditArray.append(editB);
        propSettingLayout->addWidget(labelA,i + 1,0,1,1);
        propSettingLayout->addWidget(editA,i + 1,1,1,1);
        propSettingLayout->addWidget(labelB,0,i + 1,1,1);
        propSettingLayout->addWidget(editB,1,i + 1,1,1);
        connect(editA,&QLineEdit::editingFinished,this,&propellerDisplay::updateLabelState);
        connect(editB,&QLineEdit::editingFinished,this,&propellerDisplay::updateLabelState);

        for(int j = 1;j<4;j++){
            int tmp3 = j * 2000;
            QLabel *label = new QLabel(propSettingDialog);
            label->setText("v = " + QString::number(tmp1,'f',2) + ",rpm = " + QString::number(tmp3,'f',2));
            propStateLabelArray.append(label);
            propSettingLayout->addWidget(label,i + 1,j + 1,1,1);
        }
    }

    propSettingVLayout->addLayout(propSettingHLayoutA);
    propSettingVLayout->addLayout(propSettingLayout);
    propSettingVLayout->addLayout(propSettingHLayoutB);
    propSettingDialog->setLayout(propSettingVLayout);


    connect(rowsEdit,&QLineEdit::editingFinished,this,&propellerDisplay::updateButtonState);
    connect(columnsEdit,&QLineEdit::editingFinished,this,&propellerDisplay::updateButtonState);
    connect(updateSettingButton,&QPushButton::clicked,this,&propellerDisplay::updatePropSettingState);
    connect(saveSettingButton,&QPushButton::clicked,this,&propellerDisplay::getPropSetting);

    connect(propSolverModelCombox,QOverload<const int>::of(&QComboBox::activated),this,&propellerDisplay::updateVTipCheckbox);

}
void propellerDisplay::initialAnalyseLogDialog(){
    propLogDialog = new QDialog();
    propLogVLayout = new QVBoxLayout();
    propLogTextEdit = new QTextEdit(propLogDialog);
    propLogBar = new QProgressBar(propLogDialog);

    QHBoxLayout *layout = new QHBoxLayout();
    QPushButton *button = new QPushButton(propLogDialog);
    button->setText("关闭");
    button->setFixedWidth(100);
    propLogDialog->setMinimumWidth(1400);
    propLogDialog->setMinimumHeight(800);
    layout->addWidget(button);
    propLogVLayout->addWidget(propLogTextEdit);
    propLogVLayout->addWidget(propLogBar);
    propLogVLayout->addLayout(layout);
    propLogDialog->setLayout(propLogVLayout);
    connect(button,&QPushButton::clicked,propLogDialog,&QDialog::hide);
}
void propellerDisplay::updatePropSettingState(){
    for(QLabel *label:propStateLabelArray){
        propSettingLayout->removeWidget(label);
        delete label;
    }

    for(QLabel *label:velocityLabelArray){
        propSettingLayout->removeWidget(label);
        delete label;
    }

    for(QLabel *label:rpmLabelArray){
        propSettingLayout->removeWidget(label);
        delete label;
    }

    for(QLineEdit *edit:velocityEditArray){
        propSettingLayout->removeWidget(edit);
        delete edit;
    }

    for(QLineEdit *edit:rpmEditArray){
        propSettingLayout->removeWidget(edit);
        delete edit;
    }

    propStateLabelArray.clear();
    velocityEditArray.clear();
    rpmEditArray.clear();
    velocityLabelArray.clear();
    rpmLabelArray.clear();

    bool ok1,ok2;
    int m = rowsEdit->text().toInt(&ok1);
    int n = columnsEdit->text().toInt(&ok2);
    if(!ok1 || m < 1)
        m = 1;
    if(!ok2 || n < 1)
        n = 1;

    for(int i = 1;i<m + 1;i++){

        int tmp1 = i * 5;
        QLabel *labelA = new QLabel(propSettingDialog);

        QLineEdit *editA = new QLineEdit(propSettingDialog);

        labelA->setText("速度(" + QString::number(i) + ")");

        editA->setText(QString::number(tmp1,'f',2));

        velocityLabelArray.append(labelA);
        velocityEditArray.append(editA);

        propSettingLayout->addWidget(labelA,i + 1,0,1,1);
        propSettingLayout->addWidget(editA,i + 1,1,1,1);

        connect(editA,&QLineEdit::editingFinished,this,&propellerDisplay::updateLabelState);
        for(int j = 1;j<n + 1;j++){
            int tmp3 = j * 1000;
            QLabel *label = new QLabel(propSettingDialog);
            label->setText("v = " + QString::number(tmp1) + ",rpm = " + QString::number(tmp3));
            propStateLabelArray.append(label);
            if(i == 1){
                QLineEdit *editB = new QLineEdit(propSettingDialog);
                QLabel *labelB = new QLabel(propSettingDialog);
                labelB->setText("转速(" + QString::number(j) + ")");
                editB->setText(QString::number(tmp3,'f',0));
                rpmLabelArray.append(labelB);
                rpmEditArray.append(editB);
                propSettingLayout->addWidget(labelB,0,j + 1,1,1);
                propSettingLayout->addWidget(editB,1,j + 1,1,1);
                connect(editB,&QLineEdit::editingFinished,this,&propellerDisplay::updateLabelState);
            }
            propSettingLayout->addWidget(label,i + 1,j + 1,1,1);
        }
    }

    updateSettingButton->setEnabled(false);
    //propSettingLayout->update(); // 或者 propSettingLayout->invalidate();
    //update();


}
void propellerDisplay::updateButtonState(){
    updateSettingButton->setEnabled(true);
}
void propellerDisplay::removeWidget(){



    for (int i = profileSpanEdit.length() - 1; i >= 0; --i) {

        propDefineGLayout->removeWidget(profileLabel[i]);
        delete profileLabel[i];
        profileLabel.remove(i);


        propDefineGLayout->removeWidget(profileSpanEdit[i]);
        delete profileSpanEdit[i];
        profileSpanEdit.remove(i);

        propDefineGLayout->removeWidget(profilechordLengthEdit[i]);
        delete profilechordLengthEdit[i];
        profilechordLengthEdit.remove(i);

        propDefineGLayout->removeWidget(profileOffsetLengthEdit[i]);
        delete profileOffsetLengthEdit[i];
        profileOffsetLengthEdit.remove(i);


        propDefineGLayout->removeWidget(profileTwistAngleEdit[i]);
        delete profileTwistAngleEdit[i];
        profileTwistAngleEdit.remove(i);




        propDefineGLayout->removeWidget(profileXGridEdit[i]);
        delete profileXGridEdit[i];
        profileXGridEdit.remove(i);

        propDefineGLayout->removeWidget(profileYGridEdit[i]);
        delete profileYGridEdit[i];
        profileYGridEdit.remove(i);

        propDefineGLayout->removeWidget(profileAirfoilChoiceCombobox[i]);
        delete profileAirfoilChoiceCombobox[i];
        profileAirfoilChoiceCombobox.remove(i);

    }
}
void propellerDisplay::updateInput(){
    QLineEdit *edit = qobject_cast<QLineEdit*>(sender());
    int tmp = edit->property("input").toInt();

    double actualRadius = propDiameterEditB->text().toDouble() / 2;
    // 假设 propellerRadius 为螺旋桨的半径，单位为米
    double propellerRadius = 0.7 * actualRadius;  // 0.7 倍桨叶半径

    switch (tmp) {
        case 0: {
            // 修改 propDiameterEditA (直径) -> 更新 propDiameterEditB
            bool ok;
            double D = edit->text().toDouble(&ok);  // 获取直径（英寸）
            if(ok) {
                double diameterInMeters = D * 0.0254;  // 将直径从英寸转换为米
                propDiameterEditB->setText(QString::number(diameterInMeters,'f',3));

                // 这里我们不再根据直径来更新螺距，只通过扭转角更新螺距
            }
            break;
        }
        case 1: {
            // 修改 propDiameterEditB (直径) -> 更新 propDiameterEditA
            bool ok1;
            double D1 = edit->text().toDouble(&ok1);  // 获取直径（米）
            if(ok1) {
                double diameterInInches = D1 / 0.0254;  // 将直径从米转换为英寸
                propDiameterEditA->setText(QString::number(diameterInInches,'f',3));

                // 这里也不再根据直径来更新螺距，只通过扭转角更新螺距
            }
            break;
        }
        case 2: {
            // 修改 propPitchEditA (螺距) -> 更新 propPitchEditB (角度)
            bool ok2;
            double pitch = edit->text().toDouble(&ok2) * 0.0254;  // 获取螺距
            if(ok2) {
                // 通过几何公式计算 0.7 半径处的扭转角
                double twistAngle = atan(pitch / (2 * M_PI * propellerRadius)) * 180.0 / M_PI;
                propPitchEditB->setText(QString::number(twistAngle,'f',3));  // 更新扭转角
            }
            break;
        }
        case 3: {
            // 修改 propPitchEditB (扭转角) -> 更新 propPitchEditA (螺距)
            bool ok3;
            double twistAngle = edit->text().toDouble(&ok3);  // 获取扭转角
            if(ok3) {
                // 使用公式根据扭转角计算螺距
                double pitch = 2 * M_PI * propellerRadius * tan(twistAngle * M_PI / 180.0) / 0.0254;
                propPitchEditA->setText(QString::number(pitch,'f',3));  // 更新螺距
            }
            break;
        }
        default:
            break;
    }
}
void propellerDisplay::hidePressureContourView(){
    actorC->VisibilityOff();
    //actorB->VisibilityOn();
    // 移除旧的演员
    vtkSmartPointer<vtkActorCollection> actors = rendererE->GetActors();
    actors->InitTraversal();
    vtkActor* actor = nullptr;
    while ((actor = actors->GetNextActor())) {
        if (!actor->GetPickable()) {
            rendererE->RemoveActor(actor);

        }
        //rendererE->RemoveActor(actor);
    }

    showPropActor();
    propDisplayWidgetE->renderWindow()->Render();
}
void propellerDisplay::hideStreamLineView(){
    VLMSolverArray[propChoiceIndex]->isShowStreamLine = false;
    actorS->VisibilityOff();
    propDisplayWidgetE->renderWindow()->Render();
}
void propellerDisplay::generatePropModelA(){
    //判断输入是否有效
    bool ok1,ok2,ok3,ok4,ok5;
    propDiameter = propDiameterEditB->text().toDouble(&ok1);
    propPitch = propPitchEditB->text().toDouble(&ok2);
    xGridNum = XGridNumEdit->text().toInt(&ok3);
    yGridNum = YGridNumEdit->text().toInt(&ok4);
    propRh = propRhEdit->text().toDouble(&ok5);


    if(!ok1 || propDiameter <= 0)
        propDiameter = 0.6;
    if(!ok2 || propPitch <= 0)
        propPitch = 20;
    if(!ok3 || xGridNum <= 0)
        xGridNum = 10;
    if(!ok4 || yGridNum <= 0)
        yGridNum = 2;
    if(!ok5 || propRh <= 0)
        propRh = 0.01;

    //生成基础螺旋桨
    for(int i = 0;i<SpanNumB;i++){
        double x = double(i) / (SpanNumB - 1);
        double span = (propRh + i * ((propDiameter / 2 - propRh) / (SpanNumB - 1))) * 1000;
        double chord = propDiameter * 0.08 * 1000;
        double offset = 0;
        double twist = -2.646 * x * x * x + 21.69 * x * x - 39.6 * x + 45 + (propPitch - 27);
        profileSpanEdit[i]->setText(QString::number(span));
        profilechordLengthEdit[i]->setText(QString::number(chord));
        profileOffsetLengthEdit[i]->setText(QString::number(offset));
        profileTwistAngleEdit[i]->setText(QString::number(twist));
        profileXGridEdit[i]->setText(QString::number(xGridNum));
        profileYGridEdit[i]->setText(QString::number(yGridNum));
    }


}
void propellerDisplay::generatePropModelB(){
    bool ok1,ok2;
    xGridNum = XGridNumEdit->text().toInt(&ok1);
    yGridNum = YGridNumEdit->text().toInt(&ok2);

    if(!ok1 || xGridNum <= 0)
        xGridNum = 10;
    if(!ok2 || yGridNum <= 0)
        yGridNum = 2;

    QVector<QVector<QVector<double>>>airfoilTmp;//总翼型
    //判断翼型分布方式
    int ind1 = propListCombox->currentIndex();
    double ratioTmp = 1.0;
    QVector<int>airfoilIndex = findMatchingIds(propAirfoilNameArray,propLibary->propAirfoilNameArray[ind1]);

    airfoilList = propLibary->propAirfoilNameArray[ind1];
    QString waringA = propLibary->propAirfoilNameArray[ind1].join(",");

    QString textTmp;
    if(propLibary->propAirfoilNameArray[ind1].length() >= 4){
        for(int i = 1;i<3;i++){
            textTmp = textTmp + propLibary->propAirfoilNameArray[ind1].at(i) + ",";
        }
        QString valueT = propLibary->propAirfoilNameArray[ind1].at(3);
        bool ok;
        double ratioT = valueT.toDouble(&ok);
        if(ok || ratioT > 0)
            ratioTmp = ratioT;

    }else{
        textTmp = waringA.mid(8);
    }



    QString waringText = "请导入" + textTmp +  "翼型";


    if(airfoilIndex.isEmpty()){
        isAPCOK = false;
        QMessageBox::information(nullptr,"警告",waringText);
        return;
    }else if(airfoilIndex.length() == 1){
        int ind = airfoilIndex[0];
        for(int i = 0;i<SpanNumB;i++){
            airfoilTmp.append(airfoilArray[ind]);
        }

    }else if(airfoilIndex.length() == 2){
        int indA = airfoilIndex[0];
        int indB = airfoilIndex[1];
        QVector<QVector<double>>airfoilA = airfoilArray[indA];
        QVector<QVector<double>>airfoilB = airfoilArray[indB];
        int num = 12;
        for(int i = 0;i<SpanNumB;i++){
            double ratio = sin(M_PI / 2 * double(i) / (SpanNumB - 1));
            if(ratio <= ratioTmp){
                double ratios = ratio / ratioTmp;
                airfoilDesign designA(num);
                airfoilDesign designB(num);
                airfoilDesign designC(num);
                designA.buildBenrnstein(airfoilA);
                designB.buildBenrnstein(airfoilB);
                double blendingYte1 = (designA.yTe1 + designB.yTe1) / 2;
                double blendingYte2 = (designA.yTe2 + designB.yTe2) / 2;
                QVector<double>cstTmp;
                for(int j = 0;j<num * 2;j++){
                    cstTmp.append(designA.cstParameter[j] * (1 - ratios) + designB.cstParameter[j] * ratios);
                }
                //qDebug()<<cstTmp;
                designC.blendingAirfoil(cstTmp,blendingYte1,blendingYte2);
                airfoilTmp.append(designC.newAirfoilData);
            }else{
                airfoilTmp.append(airfoilB);
            }

        }
    }


    APCAirfoilArray = airfoilTmp;
    myMath mathSolver;

    //
    if(ind1 < propLibary->propGeoArray.length()){

        QVector<QVector<double>>propTmp = propLibary->propGeoArray[ind1];
        propRh = propTmp[0][0] * 0.0254;
        int ind2 = propTmp.length() - 1;
        double propRhTmp = propTmp[0][0];
        double diameter = propTmp[ind2][0];
        QVector<double>a1;
        QVector<double>a2;
        QVector<double>a3;
        QVector<double>a4;
        for(int i = 0;i<propTmp.length();i++){
            a1.append(propTmp[i][0]);
            a2.append(propTmp[i][1]);
            a3.append(propTmp[i][2]);
            a4.append(propTmp[i][3]);
        }

        for(int i = 0;i<SpanNumB;i++){
            double span1 = propRhTmp + (diameter - propRhTmp) * sin(M_PI / 2 * double(i) / (SpanNumB - 1));

            //double chord = propLibary->threePointInterpolation(propTmp,span1,1) * 25.4;
            //double offset = -propLibary->threePointInterpolation(propTmp,span1,2) * 25.4;
            //double twist = propLibary->threePointInterpolation(propTmp,span1,3);

            double chord = mathSolver.interpolateWithLinear(a2,a1,span1) * 25.4;
            double offset = -mathSolver.interpolateWithLinear(a3,a1,span1) * 25.4;
            double twist = mathSolver.interpolateWithLinear(a4,a1,span1);

            double span = span1 * 25.4;

            profileSpanEdit[i]->setText(QString::number(span));
            profilechordLengthEdit[i]->setText(QString::number(chord));
            profileOffsetLengthEdit[i]->setText(QString::number(offset));
            profileTwistAngleEdit[i]->setText(QString::number(twist));
            profileXGridEdit[i]->setText(QString::number(xGridNum));
            profileYGridEdit[i]->setText(QString::number(yGridNum));

        }
    }

}
void propellerDisplay::showPropDefineDialog(){
    propDefineDialog->show();
}
void propellerDisplay::buildPropDefineDialog(){
    propDefineDialog = new QDialog();
    propDefineGLayoutB = new QGridLayout(propDefineDialog);
    QHBoxLayout *hLayout = new QHBoxLayout();

    propDiameterLabel = new QLabel(propDefineDialog);
    propDiameterLabelA = new QLabel(propDefineDialog);
    propDiameterLabelB = new QLabel(propDefineDialog);
    propDiameterEditA = new QLineEdit(propDefineDialog);
    propDiameterEditB = new QLineEdit(propDefineDialog);

    propPitchLabel = new QLabel(propDefineDialog);
    propPitchLabelA = new QLabel(propDefineDialog);
    propPitchLabelB = new QLabel(propDefineDialog);
    propPitchEditA = new QLineEdit(propDefineDialog);
    propPitchEditB = new QLineEdit(propDefineDialog);

    XGridNumLabel = new QLabel(propDefineDialog);
    XGridNumEdit = new QLineEdit(propDefineDialog);
    YGridNumLabel = new QLabel(propDefineDialog);
    YGridNumEdit = new QLineEdit(propDefineDialog);

    propRhLabel = new QLabel(propDefineDialog);
    propRhEdit = new QLineEdit(propDefineDialog);

    spanNumLabelB = new QLabel(propDefineDialog);
    spanNumEditB = new QLineEdit(propDefineDialog);

    propInputModelLabel = new QLabel(propDefineDialog);
    propInputModelCheckbox = new QCheckBox(propDefineDialog);
    propListCombox = new QComboBox(propDefineDialog);


    saveButton = new QPushButton(propDefineDialog);
    cancelButton = new QPushButton(propDefineDialog);

    propDiameterLabel->setText("直径:");
    propDiameterLabelA->setText("(英寸)");
    propDiameterLabelB->setText("(米)");
    propDiameterEditA->setText("10");
    propDiameterEditB->setText("0.254");

    propPitchLabel->setText("螺距:");
    propPitchLabelA->setText("(英寸)");
    propPitchLabelB->setText("(°)");
    propPitchEditA->setText("5");
    propPitchEditB->setText("0.127");

    spanNumLabelB->setText("分段数");
    spanNumEditB->setText("10");

    XGridNumLabel->setText("弦向网格数");
    YGridNumLabel->setText("展向网格数(每段)");
    XGridNumEdit->setText("10");
    YGridNumEdit->setText("1");
    propRhLabel->setText("桨毂半径(米)");
    propRhEdit->setText("0.01");
    propInputModelLabel->setText("使用螺旋桨模板");
    propInputModelCheckbox->setChecked(false);

    propListCombox->setEnabled(false);
    if(!propLibary->propGeoNameArray.isEmpty())
    for(int i = 0;i<propLibary->propGeoNameArray.length();i++)
        propListCombox->addItem(propLibary->propGeoNameArray[i]);

    saveButton->setText("保存");
    cancelButton->setText("取消");

    propDiameterEditA->setProperty("input",QVariant(0));
    propDiameterEditB->setProperty("input",QVariant(1));
    propPitchEditA->setProperty("input",QVariant(2));
    propPitchEditB->setProperty("input",QVariant(3));


    propDefineGLayoutB->addWidget(propDiameterLabel,0,0,1,1);
    propDefineGLayoutB->addWidget(propDiameterEditA,0,1,1,1);
    propDefineGLayoutB->addWidget(propDiameterLabelA,0,2,1,1);
    propDefineGLayoutB->addWidget(propDiameterEditB,0,3,1,1);
    propDefineGLayoutB->addWidget(propDiameterLabelB,0,4,1,1);

    propDefineGLayoutB->addWidget(propPitchLabel,1,0,1,1);
    propDefineGLayoutB->addWidget(propPitchEditA,1,1,1,1);
    propDefineGLayoutB->addWidget(propPitchLabelA,1,2,1,1);
    propDefineGLayoutB->addWidget(propPitchEditB,1,3,1,1);
    propDefineGLayoutB->addWidget(propPitchLabelB,1,4,1,1);



    propDefineGLayoutB->addWidget(spanNumLabelB,2,0,1,1);
    propDefineGLayoutB->addWidget(spanNumEditB,2,1,1,4);
    propDefineGLayoutB->addWidget(XGridNumLabel,3,0,1,1);
    propDefineGLayoutB->addWidget(XGridNumEdit,3,1,1,4);
    propDefineGLayoutB->addWidget(YGridNumLabel,4,0,1,1);
    propDefineGLayoutB->addWidget(YGridNumEdit,4,1,1,4);

    propDefineGLayoutB->addWidget(propRhLabel,5,0,1,1);
    propDefineGLayoutB->addWidget(propRhEdit,5,1,1,4);

    propDefineGLayoutB->addWidget(propInputModelLabel,6,0,1,1);
    propDefineGLayoutB->addWidget(propInputModelCheckbox,6,1,1,1);
    propDefineGLayoutB->addWidget(propListCombox,6,2,1,3);


    hLayout->addWidget(saveButton);
    hLayout->addWidget(cancelButton);

    propDefineGLayoutB->addLayout(hLayout,7,0,1,5);
    connect(saveButton,&QPushButton::clicked,this,&propellerDisplay::showPropDefineWidget);

    connect(propDiameterEditA,&QLineEdit::editingFinished,this,&propellerDisplay::updateInput);
    connect(propDiameterEditB,&QLineEdit::editingFinished,this,&propellerDisplay::updateInput);
    connect(propPitchEditA,&QLineEdit::editingFinished,this,&propellerDisplay::updateInput);
    connect(propPitchEditB,&QLineEdit::editingFinished,this,&propellerDisplay::updateInput);

    connect(propInputModelCheckbox,&QCheckBox::clicked,this,&propellerDisplay::updateInputState);
    //connect(saveButton,&QPushButton::clicked,this,&propellerDisplay::generateProp);
    //connect(cancelButton,&QPushButton::clicked,this,&propellerDisplay::generateProp);


}

void propellerDisplay::updatePropDesignerModel(){


    for(int i = 0;i<SpanNumB;i++){


        QLabel *labelA = new QLabel(propDefineWidget);
        QLineEdit *editA1 = new QLineEdit(propDefineWidget);
        QLineEdit *editA2 = new QLineEdit(propDefineWidget);
        QLineEdit *editA3 = new QLineEdit(propDefineWidget);
        QLineEdit *editA4 = new QLineEdit(propDefineWidget);
        QLineEdit *editA6 = new QLineEdit(propDefineWidget);
        QLineEdit *editA7 = new QLineEdit(propDefineWidget);
        QComboBox *comboboxA = new QComboBox(propDefineWidget);

        if(!airfoilArray.isEmpty() && !propAirfoilNameArray.isEmpty()){
            for(int j = 0;j<propAirfoilNameArray.length();j++){
                comboboxA->addItem(propAirfoilNameArray[j]);
            }
        }else{
            for(int j = 0;j<propAirfoilNameArray.length();j++){
                comboboxA->addItem("平板");
            }
        }
        comboboxA->setCurrentIndex(0);


        propDefineGLayout->addWidget(labelA,i + 1,0,1,1);
        propDefineGLayout->addWidget(editA1,i + 1,1,1,1);
        propDefineGLayout->addWidget(editA2,i + 1,2,1,1);
        propDefineGLayout->addWidget(editA3,i + 1,3,1,1);
        propDefineGLayout->addWidget(editA4,i + 1,4,1,1);

        propDefineGLayout->addWidget(editA6,i + 1,5,1,1);
        propDefineGLayout->addWidget(editA7,i + 1,6,1,1);
        propDefineGLayout->addWidget(comboboxA,i + 1,7,1,1);


        profileLabel.append(labelA);
        profileSpanEdit.append(editA1);
        profilechordLengthEdit.append(editA2);
        profileOffsetLengthEdit.append(editA3);
        profileTwistAngleEdit.append(editA4);
        profileXGridEdit.append(editA6);
        profileYGridEdit.append(editA7);
        profileAirfoilChoiceCombobox.append(comboboxA);

        connect(editA1,&QLineEdit::editingFinished,this,&propellerDisplay::updatePropData);
        connect(editA2,&QLineEdit::editingFinished,this,&propellerDisplay::updatePropData);
        connect(editA3,&QLineEdit::editingFinished,this,&propellerDisplay::updatePropData);
        connect(editA4,&QLineEdit::editingFinished,this,&propellerDisplay::updatePropData);
        //connect(editA5,&QLineEdit::editingFinished,this,&propellerDisplay::updatePropData);
        connect(editA6,&QLineEdit::editingFinished,this,&propellerDisplay::updatePropData);
        connect(editA7,&QLineEdit::editingFinished,this,&propellerDisplay::updatePropData);
        connect(comboboxA,QOverload<const int>::of(&QComboBox::activated),this,&propellerDisplay::updatePropData);


    }

    int maxWidth = 40 + SpanNumB * 30;
    propDefineBox->setFixedHeight(maxWidth);


}
void propellerDisplay::updateDefineView(const wingDefinition &propData){
    propellerVLM ned;
    ned.initialModel(propData);

    if(!ned.gridA.isEmpty()){
        pointsB->Reset();
        linesB->Reset();
        for(int i = 0;i<ned.meshNum * 2;i++){
            pointsB->InsertNextPoint(ned.meshA[i].x, ned.meshA[i].y, ned.meshA[i].z);
            pointsB->InsertNextPoint(ned.meshB[i].x, ned.meshB[i].y, ned.meshB[i].z);
            pointsB->InsertNextPoint(ned.meshC[i].x, ned.meshC[i].y, ned.meshC[i].z);
            pointsB->InsertNextPoint(ned.meshD[i].x, ned.meshD[i].y, ned.meshD[i].z);
        }

        for (vtkIdType i = 0; i < pointsB->GetNumberOfPoints() - 3; i += 4) {
            vtkSmartPointer<vtkLine> line1 = vtkSmartPointer<vtkLine>::New();
            line1->GetPointIds()->SetId(0, i);
            line1->GetPointIds()->SetId(1, i + 1);
            vtkSmartPointer<vtkLine> line2 = vtkSmartPointer<vtkLine>::New();
            line2->GetPointIds()->SetId(0, i + 1);
            line2->GetPointIds()->SetId(1, i + 2);
            vtkSmartPointer<vtkLine> line3 = vtkSmartPointer<vtkLine>::New();
            line3->GetPointIds()->SetId(0, i + 2);
            line3->GetPointIds()->SetId(1, i + 3);
            vtkSmartPointer<vtkLine> line4 = vtkSmartPointer<vtkLine>::New();
            line4->GetPointIds()->SetId(0, i + 3);
            line4->GetPointIds()->SetId(1, i);
            linesB->InsertNextCell(line1);
            linesB->InsertNextCell(line2);
            linesB->InsertNextCell(line3);
            linesB->InsertNextCell(line4);
    }

        propDisplayWidgetB->renderWindow()->Render();
    }
}
void propellerDisplay::getPropSetting(){
    propSettingDialog->hide();
    QVector<double>a;
    QVector<double>b;
    bool ok1,ok2;
    double h = heightEdit->text().toDouble(&ok1);
    int step = wakeStepEdit->text().toInt(&ok2);
    if(!ok1)
        h = 0;
    if(!ok2 || step < 10)
        step = 10;
    for(int i = 0;i<rpmEditArray.length();i++){
        bool ok;
        double tmp = rpmEditArray[i]->text().toDouble(&ok);
        if(!ok || tmp <=0)
            tmp = 1000 * (i + 1);
        a.append(tmp);
    }
    for(int i = 0;i<velocityEditArray.length();i++){
        bool ok;
        double tmp = velocityEditArray[i]->text().toDouble(&ok);
        if(!ok || tmp <0)
            tmp = 5 * (i + 1);
        b.append(tmp);
    }
    VLMSettingArray[propChoiceIndex].rpmArray = a;
    VLMSettingArray[propChoiceIndex].vinfArray = b;
    VLMSettingArray[propChoiceIndex].height = h;
    VLMSettingArray[propChoiceIndex].wakeStep = step;

}
wingDefinition propellerDisplay::getPropDefine(){
    int gridvType = propMeshVTypeCombobox->currentIndex();
    double griduType = double(propMeshUTypeCombobox->currentIndex()) / 20.0 + 1.0;
    bool ok1,ok2;
    int propNum = propNumEdit->text().toInt(&ok1);
    double yteTmp = propYteValueEdit->text().toDouble(&ok2);
    if(!ok1 || propNum < 1)
        propNum = 1;
    if(!ok2 || yteTmp < 0)
        yteTmp = 0;
    QVector<double>a;
    QVector<double>b;
    QVector<double>c;
    QVector<double>d;
    QVector<double>e;

    QVector<int>xnum;
    QVector<int>ynum;

    for(int i = 0;i<profileSpanEdit.length();i++){
        double tmp = profileSpanEdit[i]->text().toDouble() / 1000;
        a.append(tmp);
    }
    for(int i = 0;i<profilechordLengthEdit.length();i++){
        double tmp = profilechordLengthEdit[i]->text().toDouble() / 1000;
        b.append(tmp);
    }
    for(int i = 0;i<profileOffsetLengthEdit.length();i++){
        double tmp = profileOffsetLengthEdit[i]->text().toDouble() / 1000;
        c.append(tmp);
    }
    for(int i = 0;i<profileTwistAngleEdit.length();i++){
        double tmp = profileTwistAngleEdit[i]->text().toDouble();
        d.append(tmp);
    }
    for(int i = 0;i<profileTwistAngleEdit.length();i++){
        e.append(0.0);
    }

    for(int i = 0;i<profileXGridEdit.length();i++){
        int tmp = profileXGridEdit[i]->text().toInt();
        xnum.append(tmp);
    }
    for(int i = 0;i<profileYGridEdit.length();i++){
        int tmp = profileYGridEdit[i]->text().toInt();
        ynum.append(tmp);
    }

    wingDefinition newProp(a,b,c,d,e,xnum,ynum);
    newProp.yteType = propYteTypeCombobox->currentIndex();

    newProp.ctYte = yteTmp / 1000;
    newProp.vMeshType = gridvType;
    newProp.uMeshType = griduType;
    newProp.num = propNum;


    if(!isFormAPC){
        airfoilList.clear();
        if(!airfoilArray.isEmpty()&&!propAirfoilNameArray.isEmpty()){


            newProp.cstNum = cstNumArray[profileAirfoilChoiceCombobox[0]->currentIndex()];//参数化数目是第一个翼型的阶数;

            for(int i = 0;i<profileAirfoilChoiceCombobox.length();i++){
                int index = profileAirfoilChoiceCombobox[i]->currentIndex();
                //newWing.airfoilMeanCamberCST.append(VLMCSTArray[index]);
                newProp.airfoilArray.append(airfoilArray[index]);
                newProp.oldAirfoilArray.append(airfoilArray[index]);
                newProp.airfoilNameArray.append(propAirfoilNameArray[index]);

                    //这里定义翼型
                airfoilList.append(propAirfoilNameArray[index]);

            }



            newProp.airfoilInputType = true;
        }else{

            for(int i = 0;i<profileAirfoilChoiceCombobox.length();i++){

                //newWing.airfoilMeanCamberCST.append(VLMCSTArray[index]);
                newProp.airfoilNameArray.append("无");
                    //这里定义翼型
            }
        }



    }else{
        for(int i = 0;i<profileAirfoilChoiceCombobox.length();i++){
            //int index = profileAirfoilChoiceCombobox[i]->currentIndex();
            //newWing.airfoilMeanCamberCST.append(VLMCSTArray[index]);
            newProp.airfoilArray.append(APCAirfoilArray[i]);
            //newProp.oldAirfoilArray.append(airfoilArray[index]);
            newProp.airfoilNameArray.append(propAirfoilNameArray[0]);
                //这里定义翼型
        }


    }


    newProp.name = propNameEdit->text();
    return newProp;


}
void propellerDisplay::updateVTipCheckbox(){
    int state = propSolverModelCombox->currentIndex();
    if(!state){
        propTipLabel->show();
        propTipCheckbox->show();
    }else{
        propTipLabel->hide();
        propTipCheckbox->hide();
    }
}
void propellerDisplay::updateInputState(){
    bool state = propInputModelCheckbox->isChecked();
    if(state){
        propDiameterEditA->setEnabled(false);
        propDiameterEditB->setEnabled(false);
        propPitchEditA->setEnabled(false);
        propPitchEditB->setEnabled(false);
        propListCombox->setEnabled(true);
    }else{
        propDiameterEditA->setEnabled(true);
        propDiameterEditB->setEnabled(true);
        propPitchEditA->setEnabled(true);
        propPitchEditB->setEnabled(true);
        propListCombox->setEnabled(false);
    }

}
void propellerDisplay::updatePropData(){

    wingDefinition newProp = getPropDefine();
    newProp.computeWingMessage();
    double area = newProp.Area() * newProp.num;
    double span = newProp.Span() / 2;
    double a1 = area / (span * span * M_PI);
    double a2 = span / 0.0254 * 2;
    int a3 = newProp.MeshNum() * newProp.num ;

    propRealDiameterTextLabel->setText(QString::number(a2));
    propSoildityTextLabel->setText(QString::number(a1,'f',2));
    propRealMeshNumTextLabel->setText(QString::number(a3));
    areaTextLabel->setText(QString::number(area,'f',2));


    updateDefineView(newProp);

}
void propellerDisplay::showPropDefineWidget(){
    bool state = propInputModelCheckbox->isChecked();
    if(airfoilArray.isEmpty()){
        QMessageBox::information(nullptr,"警告","未导入翼型");
    }else{

        isModify = false;
        propDefineDialog->hide();
        buildPropDesignerModel();
        if(!state){
            isFormAPC = false;
            isAPCOK = true;
            generatePropModelA();
        }
        else{
            isFormAPC = true;
            generatePropModelB();
        }


        if(isAPCOK){
            updatePropData();
            propOutputButton->setEnabled(false);
            propDefineWidget->show();
        }
        isAPCOK = true;

    }
}
void propellerDisplay::startAnalyseProp(){
    bool modelType = propSolverModelCombox->currentIndex();
    bool tipType = propTipCheckbox->isChecked();



    if(!modelType){


        bemtSolver->readInterDrag(propAirfoilListArray[propChoiceIndex]);


        //bemtSolver->readInterDrag(propAirfoilListArray[propChoiceIndex]);
        bemtSolver->setVTip(tipType);
        bemtSolver->initialAnalyse(propArray[propChoiceIndex],VLMSettingArray[propChoiceIndex]);

        // 创建计时器实例
        //QElapsedTimer timer;

        // 启动计时器
        //timer.start();

        bemtSolver->solver();

        //qint64 elapsed = timer.elapsed(); // 获取已过去的时间（毫秒）
        //qDebug() << "bemt耗时:" << elapsed << "毫秒";


        propResultArray[propChoiceIndex] = bemtSolver->resultArray;
        propResultTypeArray[propChoiceIndex] = false;
        hideResultNode(propTreeItemArray[propChoiceIndex],"后处理");

    }else{
        propLogDialog->show();
        QApplication::processEvents(); // 确保界面更新
        propellerVLM *solvers = VLMSolverArray[propChoiceIndex];
        solvers->initialAnalyse(propArray[propChoiceIndex],VLMSettingArray[propChoiceIndex]);

        // 创建计时器实例
        //QElapsedTimer timer;

        // 启动计时器
       // timer.start();
        solvers->solver();

        //qint64 elapsed = timer.elapsed(); // 获取已过去的时间（毫秒）
        //qDebug() << "VLM耗时:" << elapsed << "毫秒";


        propResultArray[propChoiceIndex] = solvers->resultArray;
        propResultTypeArray[propChoiceIndex] = true;
        showResultNode(propTreeItemArray[propChoiceIndex],"后处理");
        solvers = nullptr;
    }
    //


    propStateCombox->setEnabled(true);
    int row = VLMSettingArray[propChoiceIndex].vinfArray.length();
    int column = VLMSettingArray[propChoiceIndex].rpmArray.length();
    propStateCombox->setEnabled(true);
    propStateCombox->blockSignals(true);
    propStateCombox->clear();
    for(int i = 0;i<row;i++){
        for(int j = 0;j<column;j++){
            int k = i * column + j + 1;
            QString text = "状态" + QString::number(k);
            propStateCombox->addItem(text);
        }
    }
    propStateCombox->blockSignals(false);
    updateGLTextA(resultToString());
    drawPropResult(0);//   
    //resultChart->createDefaultAxes();
    updateAxes(resultChart,resultSeriesArray);





}

void propellerDisplay::startSolve(){
    if(airfoilArray.isEmpty()){
        QMessageBox::information(this,"警告","未导入翼型");
        return;
    }
    propDesign->propR = propDesignEditArray[0]->text().toDouble();
    propDesign->propRh = propDesignEditArray[1]->text().toDouble();
    propDesign->propNum = propDesignEditArray[2]->text().toInt();
    propDesign->propRpm = propDesignEditArray[3]->text().toDouble();
    propDesign->propThrust = propDesignEditArray[4]->text().toDouble();
    propDesign->propVinf = propDesignEditArray[5]->text().toDouble();
    propDesign->propHeight = propDesignEditArray[6]->text().toDouble();
    propDesign->propNb = propDesignEditArray[7]->text().toInt();
    propDesign->xfoilStep = propDesignEditArray[8]->text().toInt();
    propDesign->threadNum = propDesignEditArray[9]->text().toInt();


    int index = propDesignAirfoilCombox->currentIndex();
    propDesign->cstNum = cstNumArray[index];
    propDesign->importAirfoil(airfoilArray[index]);

    propDesign->startAnalyse();
    updateView();
    chordAxisX->setRange(0,functionMath.maxV(propDesign->damiterArray) * 1.2);
    chordAxisY->setRange(0,functionMath.maxV(propDesign->oldPropChord) * 1.5);
    shapeAxisX->setRange(0,functionMath.maxV(propDesign->damiterArray) * 1.2);
    shapeAxisY->setRange(-functionMath.maxV(propDesign->xSalida) * 2,functionMath.maxV(propDesign->xSalida) * 2);
    twistAxisX->setRange(0,functionMath.maxV(propDesign->damiterArray) * 1.2);
    drawPropTwistOnce();
    drawPropChordOnce();

    initialTwistTable();
    initialChordTable();
}

void propellerDisplay::updateAirfoilArray(const QVector<QVector<QVector<double>>>&airfoilArrayTmp,const QVector<QString>&nameArray,const QVector<int>&cstNumArrayTmp){

    if(!airfoilArrayTmp.isEmpty() && !nameArray.isEmpty()){




        propDesignAirfoilCombox->blockSignals(true);
        propDesignAirfoilCombox->clear();
        airfoilArray = airfoilArrayTmp;
        cstNumArray = cstNumArrayTmp;
        propAirfoilNameArray = nameArray;
        for(int i = 0;i<nameArray.length();i++)
            propDesignAirfoilCombox->addItem(nameArray[i]);
        propDesignAirfoilCombox->blockSignals(false);
    }
}
void propellerDisplay::initialView() {
    // 创建渲染窗口和渲染器
    renwin = vtkSmartPointer<vtkGenericOpenGLRenderWindow>::New();
    propDisplayWidget->setRenderWindow(renwin);
    renderer = vtkSmartPointer<vtkRenderer>::New();
    renwin->AddRenderer(renderer);



    // 创建点和线
    points = vtkSmartPointer<vtkPoints>::New();
    lines = vtkSmartPointer<vtkCellArray>::New();
    profilePolyData = vtkSmartPointer<vtkPolyData>::New();
    profilePolyData->SetPoints(points);
    profilePolyData->SetPolys(lines);
    //profilePolyData->SetLines(lines);

    // 创建多边形数据的追加过滤器
    // appendFilter = vtkSmartPointer<vtkAppendPolyData>::New();
    // appendFilter->AddInputData(profilePolyData);
    // appendFilter->Update();

    // 创建映射器和演员
    mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
    //mapper->SetInputConnection(appendFilter->GetOutputPort());
    mapper->SetInputData(profilePolyData);
    actor = vtkSmartPointer<vtkActor>::New();
    actor->SetMapper(mapper);
    actor->VisibilityOff();
    renderer->AddActor(actor);

    // 初始化颜色数组
    color = vtkSmartPointer<vtkUnsignedCharArray>::New();
    color->SetNumberOfComponents(3); // 设置为 RGB
    color->SetName("Colors");
    profilePolyData->GetCellData()->SetScalars(color);

    // 设置背景颜色
    renderer->SetBackground(0.1, 0.2, 0.4); // 深蓝色背景

    // 创建并添加灯光
    lightKit = vtkSmartPointer<vtkLightKit>::New();
    lightKit->AddLightsToRenderer(renderer);

    // 获取 QVTKOpenGLNativeWidget 提供的交互器
    vtkRenderWindowInteractor* interactor = this->propDisplayWidget->interactor();

    // 创建并设置交互样式
    vtkSmartPointer<vtkInteractorStyleTrackballCamera> style = vtkSmartPointer<vtkInteractorStyleTrackballCamera>::New();
    interactor->SetInteractorStyle(style);

    // 创建坐标轴
    vtkSmartPointer<vtkAxesActor> axes = vtkSmartPointer<vtkAxesActor>::New();
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

    // 确保渲染窗口和交互器已正确设置
    renwin->Render();
}

void propellerDisplay::updatePropDefineWidget(const wingDefinition& propData){
    propellerVLM ned;

    ned.initialModel(propData);

    showPropActor();
    //actorE->VisibilityOn();
    if(!ned.gridA.isEmpty()){
        pointsE->Reset();
        linesE->Reset();
        colorE->Reset();
        /*
        for(int i = 0;i<ned.gridA.length();i++){
            pointsE->InsertNextPoint(ned.gridA[i].x, ned.gridA[i].y, ned.gridA[i].z);
            pointsE->InsertNextPoint(ned.gridB[i].x, ned.gridB[i].y, ned.gridB[i].z);
            pointsE->InsertNextPoint(ned.gridC[i].x, ned.gridC[i].y, ned.gridC[i].z);
            pointsE->InsertNextPoint(ned.gridD[i].x, ned.gridD[i].y, ned.gridD[i].z);

        }
        */
        for(int i = 0;i<ned.meshNum * 2;i++){
            pointsE->InsertNextPoint(ned.meshA[i].x, ned.meshA[i].y, ned.meshA[i].z);
            pointsE->InsertNextPoint(ned.meshB[i].x, ned.meshB[i].y, ned.meshB[i].z);
            pointsE->InsertNextPoint(ned.meshC[i].x, ned.meshC[i].y, ned.meshC[i].z);
            pointsE->InsertNextPoint(ned.meshD[i].x, ned.meshD[i].y, ned.meshD[i].z);

        }

        unsigned char white[3] = {200, 200, 200};
        for (vtkIdType i = 0; i < pointsE->GetNumberOfPoints() - 3; i += 4) {

            vtkSmartPointer<vtkQuad> quad = vtkSmartPointer<vtkQuad>::New();
            quad->GetPointIds()->SetId(0, i);
            quad->GetPointIds()->SetId(1, i + 1);
            quad->GetPointIds()->SetId(2, i + 2);
            quad->GetPointIds()->SetId(3, i + 3);
            linesE->InsertNextCell(quad);                // 添加颜色

            colorE->InsertNextTypedTuple(white);

        }

        profilePolyDataE->SetPoints(pointsE);
        profilePolyDataE->SetPolys(linesE);
        profilePolyDataE->GetCellData()->SetScalars(colorE);

        // 移除旧的演员
        vtkSmartPointer<vtkActorCollection> actors = rendererE->GetActors();
        actors->InitTraversal();
        vtkActor* actor = nullptr;
        while ((actor = actors->GetNextActor())) {
            if (!actor->GetPickable()) {
                rendererE->RemoveActor(actor);

            }
            //rendererE->RemoveActor(actor);
        }



        // 创建新的映射器和演员
        vtkSmartPointer<vtkPolyDataMapper> mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
        mapper->SetInputData(profilePolyDataE);

        vtkSmartPointer<vtkActor> newActor = vtkSmartPointer<vtkActor>::New();
        newActor->SetMapper(mapper);
        rendererE->AddActor(newActor);


        vtkSmartPointer<vtkProperty> property = vtkSmartPointer<vtkProperty>::New();
        property->SetColor(1.0, 0.0, 0.0); // 可选：设置颜色或其他属性
        newActor->SetProperty(property);
        newActor->PickableOff();

        // 创建和添加旋转阵列
        for(int i = 1;i<propData.num;i++){
            double rotated = double(i) / propData.num * 360;
            CreateRotatedArray(rendererE,profilePolyDataE, rotated);
        }


        //rendererE->Render();
        propDisplayWidgetE->renderWindow()->Render();

    }

}
void propellerDisplay::updateView() {
    actor->VisibilityOn();
    points->Reset();
    lines->Reset();
    color->Reset();

    if (!propDesign->xyALower.isEmpty()) {
        for (int i = 0; i < propDesign->meshNum; i++) {
            points->InsertNextPoint(propDesign->xyAUpper[i].x, propDesign->xyAUpper[i].y, propDesign->xyAUpper[i].z);
            points->InsertNextPoint(propDesign->xyBUpper[i].x, propDesign->xyBUpper[i].y, propDesign->xyBUpper[i].z);
            points->InsertNextPoint(propDesign->xyCUpper[i].x, propDesign->xyCUpper[i].y, propDesign->xyCUpper[i].z);
            points->InsertNextPoint(propDesign->xyDUpper[i].x, propDesign->xyDUpper[i].y, propDesign->xyDUpper[i].z);
        }
        for (int i = 0; i < propDesign->meshNum; i++) {
            points->InsertNextPoint(propDesign->xyALower[i].x, propDesign->xyALower[i].y, propDesign->xyALower[i].z);
            points->InsertNextPoint(propDesign->xyBLower[i].x, propDesign->xyBLower[i].y, propDesign->xyBLower[i].z);
            points->InsertNextPoint(propDesign->xyCLower[i].x, propDesign->xyCLower[i].y, propDesign->xyCLower[i].z);
            points->InsertNextPoint(propDesign->xyDLower[i].x, propDesign->xyDLower[i].y, propDesign->xyDLower[i].z);
        }

        // 添加面
        unsigned char white[3] = {200, 200, 200};
        for (vtkIdType i = 0; i < points->GetNumberOfPoints() - 3; i += 4) {
            vtkSmartPointer<vtkQuad> quad = vtkSmartPointer<vtkQuad>::New();
            quad->GetPointIds()->SetId(0, i);
            quad->GetPointIds()->SetId(1, i + 1);
            quad->GetPointIds()->SetId(2, i + 2);
            quad->GetPointIds()->SetId(3, i + 3);
            lines->InsertNextCell(quad);

            // 添加颜色
            color->InsertNextTypedTuple(white);
        }

        profilePolyData->SetPoints(points);
        profilePolyData->SetPolys(lines);
        profilePolyData->GetCellData()->SetScalars(color);

        // 移除旧的演员
        vtkSmartPointer<vtkActorCollection> actors = renderer->GetActors();
        actors->InitTraversal();
        vtkActor* actor = nullptr;
        while ((actor = actors->GetNextActor())) {            
            renderer->RemoveActor(actor);
        }



        // 创建新的映射器和演员
        vtkSmartPointer<vtkPolyDataMapper> mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
        mapper->SetInputData(profilePolyData);

        vtkSmartPointer<vtkActor> newActor = vtkSmartPointer<vtkActor>::New();
        newActor->SetMapper(mapper);
        renderer->AddActor(newActor);



        // 创建和添加旋转阵列
        for(int i = 1;i<propDesign->propNum;i++){
            double rotated = double(i) / propDesign->propNum * 360;
            CreateRotatedArray(renderer,profilePolyData, rotated);
        }

        // 渲染窗口
        renwin->Render();
    }
}


void propellerDisplay::CreateRotatedArray(vtkSmartPointer<vtkRenderer> ren, vtkSmartPointer<vtkPolyData> inputPolyData, double angle) {
    // 检查输入数据是否有效
    if (!inputPolyData || inputPolyData->GetNumberOfPoints() == 0) {
        std::cerr << "Error: inputPolyData is empty or invalid!" << std::endl;
        return;
    }

    // 创建变换对象并进行旋转
    vtkSmartPointer<vtkTransform> transform = vtkSmartPointer<vtkTransform>::New();
    transform->RotateZ(angle);

    // 应用变换到PolyData
    vtkSmartPointer<vtkTransformPolyDataFilter> transformFilter = vtkSmartPointer<vtkTransformPolyDataFilter>::New();
    transformFilter->SetInputData(inputPolyData);
    transformFilter->SetTransform(transform);
    transformFilter->Update();

    vtkSmartPointer<vtkPolyData> transformedData = transformFilter->GetOutput();

    // 创建映射器和演员
    vtkSmartPointer<vtkPolyDataMapper> mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
    mapper->SetInputData(transformedData);

    vtkSmartPointer<vtkActor> actor = vtkSmartPointer<vtkActor>::New();
    actor->SetMapper(mapper);

    vtkSmartPointer<vtkProperty> property = vtkSmartPointer<vtkProperty>::New();
    property->SetColor(1.0, 0.0, 0.0); // 可选：设置颜色或其他属性
    actor->SetProperty(property);
    actor->PickableOff();

    // 将演员添加到渲染器
    ren->AddActor(actor);

    // 如果需要，可以在这里清除旧的演员或进行其他处理
}

void propellerDisplay::initialSteup(){
    initialRChartMenu();
    loadStyleSheet();
    loadIcon();
    axisXName[0] = "速度(m/s)"; axisYName[0] = "推力(N)"; titleName[0] = "推力曲线A";
    axisXName[1] = "转速(r/min)"; axisYName[1] = "推力(N)"; titleName[1] = "推力曲线B";
    axisXName[2] = "前进比(J)"; axisYName[2] = "效率(n)"; titleName[2] = "效率曲线A";
    axisXName[3] = "速度(m/s)"; axisYName[3] = "效率(n)"; titleName[3] = "效率曲线B";
    axisXName[4] = "前进比(J)"; axisYName[4] = "推力系数(Ct)"; titleName[4] = "推力系数曲线A";
    axisXName[5] = "速度(m/s)"; axisYName[5] = "推力系数(Ct)"; titleName[5] = "推力系数曲线B";
    axisXName[6] = "速度(m/s)"; axisYName[6] = "功率(w)"; titleName[6] = "功率曲线";
    axisXName[7] = "速度(m/s)"; axisYName[7] = "力矩(N/m)"; titleName[7] = "力矩曲线";
    axisIndex[0][0] = 0,axisIndex[0][1] = 10;
    axisIndex[1][0] = 0,axisIndex[1][1] = 10;
    axisIndex[2][0] = 1,axisIndex[2][1] = 2;
    axisIndex[3][0] = 0,axisIndex[3][1] = 2;
    axisIndex[4][0] = 1,axisIndex[4][1] = 3;
    axisIndex[5][0] = 0,axisIndex[5][1] = 3;
    axisIndex[6][0] = 0,axisIndex[6][1] = 8;
    axisIndex[7][0] = 0,axisIndex[7][1] = 9;
    xAxisIndexA = axisIndex[0][0], yAxisIndexA = axisIndex[0][1];
    xAxisIndexB = axisIndex[3][0], yAxisIndexB = axisIndex[3][1];
    xAxisIndexC = axisIndex[6][0], yAxisIndexC = axisIndex[6][1];
    xAxisIndexD = axisIndex[7][0], yAxisIndexD = axisIndex[7][1];
    chartTypeIndexArray[0] = 0;
    chartTypeIndexArray[1] = 3;
    chartTypeIndexArray[2] = 6;
    chartTypeIndexArray[3] = 7;

    QVector<double>a{2000,4000,6000};
    QVector<double>b{5,10,15};

    initialVLMSetting.rpmArray = a;
    initialVLMSetting.vinfArray = b;
    initialVLMSetting.height = 0;
    initialVLMSetting.wakeStep = 100;

}
void propellerDisplay::initialText(const QVector<QString>&Text){




}
void propellerDisplay::loadIcon(){

    checkIcon = QDir::currentPath() + "/resoure/images/check.png";
    removeIcon = QDir::currentPath() + "/resoure/images/remove.png";

}
void propellerDisplay::loadStyleSheet(){
    QString path1 = QDir::currentPath() + "/resoure/airfoilQss/button.qss";
    QFile styleFile1(path1);
    if (styleFile1.open(QFile::ReadOnly | QFile::Text)) {
        QTextStream stream(&styleFile1);
        buttonStyle = stream.readAll();
        styleFile1.close();

    } else {
        qDebug() << "Failed to open stylesheet file!";
    }

    QString path2 = QDir::currentPath() + "/resoure/airfoilQss/progressBar.qss";
    QFile styleFile2(path2);
    if (styleFile2.open(QFile::ReadOnly | QFile::Text)) {
        QTextStream stream(&styleFile2);
        progressBarStyle = stream.readAll();
        styleFile2.close();

    } else {
        qDebug() << "Failed to open stylesheet file!";
    }

    QString path3 = QDir::currentPath() + "/resoure/airfoilQss/groupBox.qss";
    QFile styleFile3(path3);
    if (styleFile3.open(QFile::ReadOnly | QFile::Text)) {
        QTextStream stream(&styleFile3);
        groupBoxStyle = stream.readAll();
        styleFile3.close();

    } else {
        qDebug() << "Failed to open stylesheet file!";
    }



}
void propellerDisplay::initialPropLibaries(){
    propLibary = new propellerLibary();
}
void propellerDisplay::initialPropResultWidget(){
    propResultDisplayWidget = new QWidget();
    colorDialog = new QColorDialog();
    resultHLayout = new QHBoxLayout(propResultDisplayWidget);
    resultVlayout = new QVBoxLayout();
    propListVLayout = new QVBoxLayout();
    propListBox = new QGroupBox("螺旋桨列表");
    propListGLayout = new QGridLayout(propListBox);
    itemSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);
    initialResultChart();


    QSplitter *vSplitter = new QSplitter(Qt::Vertical);

    // 创建水平的 QSplitter 用于左上和右上分割
    QSplitter *hSplitterTop = new QSplitter(Qt::Horizontal);
    hSplitterTop->addWidget(resultChartViewA);
    hSplitterTop->addWidget(resultChartViewB);

    // 创建水平的 QSplitter 用于左下和右下分割
    QSplitter *hSplitterBottom = new QSplitter(Qt::Horizontal);
    hSplitterBottom->addWidget(resultChartViewC);
    hSplitterBottom->addWidget(resultChartViewD);

    // 将上部和下部分割器添加到垂直分割器
    vSplitter->addWidget(hSplitterTop);
    vSplitter->addWidget(hSplitterBottom);
    QLabel *spaceLabel = new QLabel(propResultDisplayWidget);
    resultVlayout->addWidget(vSplitter);

    propListVLayout->addWidget(propListBox);
    propListGLayout->addWidget(spaceLabel,0,0,1,4);
    resultHLayout->addLayout(propListVLayout);
    resultHLayout->addLayout(resultVlayout);
    //propResultDisplayWidget->setLayout(resultHLayout);
    propListBox->setMaximumWidth(350);





}
void propellerDisplay::initialResultChart(){
    resultChartA = new QChart();
    resultChartB = new QChart();
    resultChartC = new QChart();
    resultChartD = new QChart();

    toolTipA = new Callout(resultChartA);
    toolTipB = new Callout(resultChartB);
    toolTipC = new Callout(resultChartC);
    toolTipD = new Callout(resultChartD);




    resultAxisXA = new QValueAxis; resultAxisYA = new QValueAxis;
    resultAxisXB = new QValueAxis; resultAxisYB = new QValueAxis;
    resultAxisXC = new QValueAxis; resultAxisYC = new QValueAxis;
    resultAxisXD = new QValueAxis; resultAxisYD = new QValueAxis;







    resultAxisXA->setRange(0,axisRange[0]);
    resultAxisYA->setRange(0,axisRange[10]);
    resultAxisXB->setRange(0,axisRange[1]);
    resultAxisYB->setRange(0,axisRange[2]);
    resultAxisXC->setRange(0,axisRange[0]);
    resultAxisYC->setRange(0,axisRange[9]);
    resultAxisXD->setRange(0,axisRange[0]);
    resultAxisYD->setRange(0,axisRange[8]);
    resultChartA->addAxis(resultAxisXA,Qt::AlignBottom);
    resultChartA->addAxis(resultAxisYA,Qt::AlignLeft);
    resultChartB->addAxis(resultAxisXB,Qt::AlignBottom);
    resultChartB->addAxis(resultAxisYB,Qt::AlignLeft);
    resultChartC->addAxis(resultAxisXC,Qt::AlignBottom);
    resultChartC->addAxis(resultAxisYC,Qt::AlignLeft);
    resultChartD->addAxis(resultAxisXD,Qt::AlignBottom);
    resultChartD->addAxis(resultAxisYD,Qt::AlignLeft);
    resultChartViewA = new MyChartView(resultChartA,rChartMenu);
    resultChartViewB = new MyChartView(resultChartB,rChartMenu);
    resultChartViewC = new MyChartView(resultChartC,rChartMenu);
    resultChartViewD = new MyChartView(resultChartD,rChartMenu);
    resultChartViewA->setRenderHint(QPainter::Antialiasing);
    resultChartViewB->setRenderHint(QPainter::Antialiasing);
    resultChartViewC->setRenderHint(QPainter::Antialiasing);
    resultChartViewD->setRenderHint(QPainter::Antialiasing);
    resultChartViewA->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    resultChartViewB->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    resultChartViewC->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    resultChartViewD->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    resultAxisXA->setTitleText(axisXName[0]);
    resultAxisYA->setTitleText(axisYName[0]);
    resultAxisXB->setTitleText(axisXName[3]);
    resultAxisYB->setTitleText(axisYName[3]);
    resultAxisXC->setTitleText(axisXName[6]);
    resultAxisYC->setTitleText(axisYName[6]);
    resultAxisXD->setTitleText(axisXName[7]);
    resultAxisYD->setTitleText(axisYName[7]);
    resultChartA->setTitle(titleName[0]);
    resultChartB->setTitle(titleName[3]);
    resultChartC->setTitle(titleName[6]);
    resultChartD->setTitle(titleName[7]);

    resultChartA->legend()->setVisible(false);
    resultChartB->legend()->setVisible(false);
    resultChartC->legend()->setVisible(false);
    resultChartD->legend()->setVisible(false);

    autoSeriesA = new QLineSeries;
    autoSeriesB = new QLineSeries;
    autoSeriesC = new QLineSeries;
    autoSeriesD = new QLineSeries;
    resultChartA->addSeries(autoSeriesA);
    resultChartB->addSeries(autoSeriesB);
    resultChartC->addSeries(autoSeriesC);
    resultChartD->addSeries(autoSeriesD);
    autoSeriesA->attachAxis(resultAxisXA);
    autoSeriesA->attachAxis(resultAxisYA);
    autoSeriesB->attachAxis(resultAxisXB);
    autoSeriesB->attachAxis(resultAxisYB);
    autoSeriesC->attachAxis(resultAxisXC);
    autoSeriesC->attachAxis(resultAxisYC);
    autoSeriesD->attachAxis(resultAxisXD);
    autoSeriesD->attachAxis(resultAxisYD);

    QPen pen;
    pen.setStyle(Qt::DashLine);
    pen.setColor(Qt::red);
    pen.setWidth(2);
    autoSeriesA->setPen(pen);
    autoSeriesB->setPen(pen);
    autoSeriesC->setPen(pen);
    autoSeriesD->setPen(pen);



}

void propellerDisplay::initialAxis(){
    axisRange[0] = 50; // Mph
    axisRange[1] = 1.5;// J
    axisRange[2] = 1.0;// Pe
    axisRange[3] = 1.0;// Ct
    axisRange[4] = 1.0;// Cp
    axisRange[5] = 1.0;// Pwr(Hp)
    axisRange[6] = 1.0;// Torque
    axisRange[7] = 2.0;// Thrust
    axisRange[8] = 150.0;// Pwr(W)
    axisRange[9] = 1.0;// Torque(N/m)
    axisRange[10] = 20.0;// Thrust(N)
    axisRange[11] = 5.0;// Thr/Pwr
    axisRange[12] = 1.0;// Mach
    axisRange[13] = 100000.0;// Reyn
    axisRange[14] = 1.0;// Fom
}
void propellerDisplay::initialRChartMenu(){
    rChartMenu = new QMenu();
    QAction *rAction1 = new QAction("速度-推力曲线",rChartMenu);
    QAction *rAction2 = new QAction("转速-推力曲线",rChartMenu);
    QAction *rAction3 = new QAction("前进比-效率曲线",rChartMenu);
    QAction *rAction4 = new QAction("速度-效率曲线",rChartMenu);
    QAction *rAction5 = new QAction("前进比-推力系数曲线",rChartMenu);
    QAction *rAction6 = new QAction("速度-推力系数曲线",rChartMenu);
    QAction *rAction7 = new QAction("速度-功率曲线",rChartMenu);
    QAction *rAction8 = new QAction("速度-力矩曲线",rChartMenu);
    QAction *rAction9 = new QAction("保存数据",rChartMenu);
    QAction *rAction10 = new QAction("添加点",rChartMenu);
    QAction *rAction11 = new QAction("移除点",rChartMenu);


    rAction1->setProperty("actions",0);
    rAction2->setProperty("actions",1);
    rAction3->setProperty("actions",2);
    rAction4->setProperty("actions",3);
    rAction5->setProperty("actions",4);
    rAction6->setProperty("actions",5);
    rAction7->setProperty("actions",6);
    rAction8->setProperty("actions",7);
    rAction9->setProperty("actions",8);
    connect(rAction1,&QAction::triggered,this,&propellerDisplay::changeResultType);
    connect(rAction2,&QAction::triggered,this,&propellerDisplay::changeResultType);
    connect(rAction3,&QAction::triggered,this,&propellerDisplay::changeResultType);
    connect(rAction4,&QAction::triggered,this,&propellerDisplay::changeResultType);
    connect(rAction5,&QAction::triggered,this,&propellerDisplay::changeResultType);
    connect(rAction6,&QAction::triggered,this,&propellerDisplay::changeResultType);
    connect(rAction7,&QAction::triggered,this,&propellerDisplay::changeResultType);
    connect(rAction8,&QAction::triggered,this,&propellerDisplay::changeResultType);
    connect(rAction9,&QAction::triggered,this,&propellerDisplay::changeResultType);
    connect(rAction10,&QAction::triggered,this,&propellerDisplay::addDataPoint);
    connect(rAction11,&QAction::triggered,this,&propellerDisplay::clearDataPoint);

    rAction1->setCheckable(true);
    //rAction2->setCheckable(true);
    rAction2->setEnabled(false);
    rAction3->setCheckable(true);
    rAction4->setCheckable(true);
    rAction5->setCheckable(true);
    rAction6->setCheckable(true);
    rAction7->setCheckable(true);
    rAction8->setCheckable(true);
    rAction9->setCheckable(true);


    rChartMenu->addAction(rAction1);
    rChartMenu->addAction(rAction2);
    rChartMenu->addAction(rAction3);
    rChartMenu->addAction(rAction4);
    rChartMenu->addAction(rAction5);
    rChartMenu->addAction(rAction6);
    rChartMenu->addAction(rAction7);
    rChartMenu->addAction(rAction8);
    rChartMenu->addAction(rAction9);
    rChartMenu->addAction(rAction10);
    rChartMenu->addAction(rAction11);

    actionArray.append(rAction1);
    actionArray.append(rAction2);
    actionArray.append(rAction3);
    actionArray.append(rAction4);
    actionArray.append(rAction5);
    actionArray.append(rAction6);
    actionArray.append(rAction7);
    actionArray.append(rAction8);
    actionArray.append(rAction9);
    actionArray.append(rAction10);
    actionArray.append(rAction11);

}
void propellerDisplay::initialPropDesignWidget(){
    propDesignWidget = new QWidget();
    propDisplayWidget = new QVTKOpenGLNativeWidget();
    propDesignHLayout = new QHBoxLayout();
    propDesignVLayoutA = new QVBoxLayout();
    propDesignVLayoutB = new QVBoxLayout();

    propDesignSettingBox = new QGroupBox();
    propDesignResultBox = new QGroupBox();
    propDesignGLayout = new QGridLayout(propDesignSettingBox);
    propDesignGMessageLayout = new QGridLayout(propDesignResultBox);

    twistControlButton = new hoverButton();
    chordControlButton = new hoverButton();
    savePropDataButton = new hoverButton();
    changeViewButtonA = new hoverButton();
    changeViewButtonB = new hoverButton();
    changeViewButtonC = new hoverButton();





    propDesignMessageTextEdit = new QTextEdit();

    for(int i = 0;i<10;i++){
        QLabel *label = new QLabel();
        QLineEdit *edit = new QLineEdit();
        propDesignLabelArray[i] = label;
        propDesignEditArray[i] = edit;

    }
    QLabel *label1 = new QLabel();
    QLabel *label2 = new QLabel();
    QLineEdit *edit1 = new QLineEdit();

    propDesignLabelArray[10] = label1;
    propDesignLabelArray[11] = label2;
    propDesignEditArray[10] = edit1;

    yteValueLabel = new QLabel();
    yteTypeLabel = new QLabel();
    yteValueEdit = new QLineEdit();
    yteTypeCombobox = new QComboBox();

    propDesignAirfoilCombox = new QComboBox();
    for(int i = 0;i<5;i++){
        propDesignGLayout->addWidget(propDesignLabelArray[i * 2],i,0,1,1);
        propDesignGLayout->addWidget(propDesignEditArray[i * 2],i,1,1,1);
        propDesignGLayout->addWidget(propDesignLabelArray[i * 2+ 1],i,2,1,1);
        propDesignGLayout->addWidget(propDesignEditArray[i * 2 + 1],i,3,1,1);
    }
    propDesignGLayout->addWidget(propDesignLabelArray[10],5,0,1,1);
    propDesignGLayout->addWidget(propDesignEditArray[10],5,1,1,1);
    propDesignGLayout->addWidget(propDesignLabelArray[11],5,2,1,1);
    propDesignGLayout->addWidget(propDesignAirfoilCombox,5,3,1,1);

    propDesignGLayout->addWidget(yteValueLabel,6,0,1,1);
    propDesignGLayout->addWidget(yteValueEdit,6,1,1,1);
    propDesignGLayout->addWidget(yteTypeLabel,6,2,1,1);
    propDesignGLayout->addWidget(yteTypeCombobox,6,3,1,1);


    propDesignGMessageLayout->addWidget(twistControlButton,0,0,1,1);
    propDesignGMessageLayout->addWidget(chordControlButton,0,1,1,1);
    propDesignGMessageLayout->addWidget(savePropDataButton,0,2,1,1);
    propDesignGMessageLayout->addWidget(changeViewButtonA,1,0,1,1);
    propDesignGMessageLayout->addWidget(changeViewButtonB,1,1,1,1);
    propDesignGMessageLayout->addWidget(changeViewButtonC,1,2,1,1);


    propDesignLabelArray[0]->setText("螺旋桨半径（m）");
    propDesignLabelArray[1]->setText("桨毂半径（m）");
    propDesignLabelArray[2]->setText("桨叶数目");
    propDesignLabelArray[3]->setText("转速（RPM）");
    propDesignLabelArray[4]->setText("推力（N）");
    propDesignLabelArray[5]->setText("速度（m/s）");
    propDesignLabelArray[6]->setText("高度（m）");
    propDesignLabelArray[7]->setText("截面数");
    propDesignLabelArray[8]->setText("迭代次数");
    propDesignLabelArray[9]->setText("线程数量");
    propDesignLabelArray[10]->setText("X网格数");
    propDesignLabelArray[11]->setText("翼型");

    yteTypeLabel->setText("厚度分布方式");
    yteValueLabel->setText("尾缘厚度(mm)");
    yteTypeCombobox->addItem("等厚度分布");
    yteTypeCombobox->addItem("比例分布");
    yteValueEdit->setText("0");

    propDesignEditArray[0]->setText("0.7");
    propDesignEditArray[1]->setText("0.06");
    propDesignEditArray[2]->setText("2");
    propDesignEditArray[3]->setText("2100");
    propDesignEditArray[4]->setText("370");
    propDesignEditArray[5]->setText("25");
    propDesignEditArray[6]->setText("1000");
    propDesignEditArray[7]->setText("10");
    propDesignEditArray[8]->setText("100");
    propDesignEditArray[9]->setText("30");
    propDesignEditArray[10]->setText("20");

    propDesignSettingBox->setTitle("参数设置");

    twistControlButton->setText("扭转角调整");
    chordControlButton->setText("外型调整");
    savePropDataButton->setText("输出Catia文件");
    changeViewButtonA->setText("俯视图");
    changeViewButtonB->setText("左视图");
    changeViewButtonC->setText("正视图");
    twistControlButton->setFixedWidth(100);
    chordControlButton->setFixedWidth(100);
    savePropDataButton->setFixedWidth(100);
    changeViewButtonA->setFixedWidth(100);
    changeViewButtonB->setFixedWidth(100);
    changeViewButtonC->setFixedWidth(100);

    propDesignSettingBox->setFixedSize(400,500);
    propDesignMessageTextEdit->setFixedWidth(400);
    propDesignResultBox->setFixedWidth(400);
    propDesignVLayoutA->addWidget(propDesignSettingBox);
    propDesignVLayoutA->addWidget(propDesignResultBox);
    propDesignVLayoutA->addWidget(propDesignMessageTextEdit);

    propDesignVLayoutB->addWidget(propDisplayWidget);


    propDesignHLayout->addLayout(propDesignVLayoutA);
    propDesignHLayout->addLayout(propDesignVLayoutB);
    propDesignWidget->setLayout(propDesignHLayout);
    initialDialogA();
    initialDialogB();

    connect(twistControlButton,&QPushButton::clicked,this,&propellerDisplay::showPropTwistDialog);
    connect(chordControlButton,&QPushButton::clicked,this,&propellerDisplay::showPropChordDialog);
    connect(savePropDataButton,&QPushButton::clicked,this,&propellerDisplay::outputPropData);
    connect(changeViewButtonA,&QPushButton::clicked,this,&propellerDisplay::switchTopView);
    connect(changeViewButtonB,&QPushButton::clicked,this,&propellerDisplay::switchMainView);
    connect(changeViewButtonC,&QPushButton::clicked,this,&propellerDisplay::switchLeftView);
    connect(edit1,&QLineEdit::editingFinished,this,&propellerDisplay::changePropXMesh);
    connect(yteValueEdit,&QLineEdit::editingFinished,this,&propellerDisplay::changePropXMesh);
    connect(yteTypeCombobox, QOverload<int>::of(&QComboBox::activated),this,&propellerDisplay::changePropXMesh);

}
void propellerDisplay::initialDialogA(){

    chartADialog = new QDialog();
    QVBoxLayout *vLayout = new QVBoxLayout();
    QHBoxLayout *hLayoutA = new QHBoxLayout();
    QHBoxLayout *hLayoutB = new QHBoxLayout();
    chartA = new QChart();
    chartC = new QChart();
    chartViewA = new MyChartView(chartA);
    chartViewC = new MyChartView(chartC);
    chartViewA->setRenderHint(QPainter::Antialiasing);
    chartViewC->setRenderHint(QPainter::Antialiasing);

    chordSeries = new QLineSeries;
    optChordSeries = new QLineSeries;

    xAtaqueSeries = new QLineSeries;
    xSalidaSeries = new QLineSeries;

    xAtaqueScatterSeries = new QScatterSeries;
    xSalidaScatterSeries = new QScatterSeries;
    xAtaqueScatterSeriesB = new QScatterSeries;
    xSalidaScatterSeriesB = new QScatterSeries;

    xAtaqueScatterSeries->setMarkerSize(20);
    xAtaqueScatterSeriesB->setMarkerSize(12);
    xAtaqueScatterSeries->setColor(Qt::red);
    xAtaqueScatterSeriesB->setColor(Qt::white);

    xSalidaScatterSeries->setMarkerSize(20);
    xSalidaScatterSeriesB->setMarkerSize(12);
    xSalidaScatterSeries->setColor(Qt::red);
    xSalidaScatterSeriesB->setColor(Qt::white);






    chordAxisX = new QValueAxis;
    chordAxisY = new QValueAxis;
    shapeAxisX = new QValueAxis;
    shapeAxisY = new QValueAxis;

    chordSpinBox = new QSpinBox();

    QLabel *orderLabel = new QLabel();
    QPushButton *restButton = new QPushButton();
    QPushButton *saveButton = new QPushButton();
    chordTable = new QTableWidget(4,10,chartADialog);
    chordTable->setFixedHeight(200);





    chartADialog->resize(1250,700);

    orderLabel->setText("光顺阶数");
    chordSpinBox->setMinimum(1);
    chordSpinBox->setMaximum(6);
    chordSpinBox->setValue(3);
    restButton->setText("还原");
    saveButton->setText("导出数据");



    chartA->addSeries(chordSeries);
    chartA->addSeries(optChordSeries);
    chartA->addAxis(chordAxisX,Qt::AlignBottom);
    chartA->addAxis(chordAxisY,Qt::AlignLeft);

    chartC->addSeries(xAtaqueSeries);
    chartC->addSeries(xSalidaSeries);
    chartC->addSeries(xAtaqueScatterSeries);
    chartC->addSeries(xSalidaScatterSeries);
    chartC->addSeries(xAtaqueScatterSeriesB);
    chartC->addSeries(xSalidaScatterSeriesB);
    chartC->addAxis(shapeAxisX,Qt::AlignBottom);
    chartC->addAxis(shapeAxisY,Qt::AlignLeft);

    chordSeries->attachAxis(chordAxisX);
    chordSeries->attachAxis(chordAxisY);
    optChordSeries->attachAxis(chordAxisX);
    optChordSeries->attachAxis(chordAxisY);

    xAtaqueSeries->attachAxis(shapeAxisX);
    xAtaqueSeries->attachAxis(shapeAxisY);
    xSalidaSeries->attachAxis(shapeAxisX);
    xSalidaSeries->attachAxis(shapeAxisY);
    xAtaqueScatterSeries->attachAxis(shapeAxisX);
    xAtaqueScatterSeries->attachAxis(shapeAxisY);
    xSalidaScatterSeries->attachAxis(shapeAxisX);
    xSalidaScatterSeries->attachAxis(shapeAxisY);
    xAtaqueScatterSeriesB->attachAxis(shapeAxisX);
    xAtaqueScatterSeriesB->attachAxis(shapeAxisY);
    xSalidaScatterSeriesB->attachAxis(shapeAxisX);
    xSalidaScatterSeriesB->attachAxis(shapeAxisY);

    chordSeries->setName("原始弦长分布");
    optChordSeries->setName("优化弦长分布");
    xAtaqueSeries->setName("前缘轮廓");
    xSalidaSeries->setName("后缘轮廓");


    chordAxisX->setRange(0,1);
    chordAxisY->setRange(-0.2,0.2);
    shapeAxisX->setRange(0,1);
    shapeAxisY->setRange(-0.2,0.2);

    QLegend *legend = chartC->legend();

    if (legend) {
        // 直接访问图例项，知道每个系列的图例项位置
        QList<QLegendMarker*> markers = legend->markers();
        for(int i = 0;i<2;i++){
            markers.at(i)->setVisible(true);
        }
        for(int i = 2;i<markers.size();i++){
            markers.at(i)->setVisible(false);
        }
    }

    // chartA->legend()->setVisible(false);
    // chartC->legend()->setVisible(false);
    hLayoutA->addWidget(chartViewA);
    hLayoutA->addWidget(chartViewC);

    hLayoutB->addWidget(orderLabel);
    hLayoutB->addWidget(chordSpinBox);
    hLayoutB->addWidget(restButton);
    hLayoutB->addWidget(saveButton);

    vLayout->addLayout(hLayoutA);

    vLayout->addWidget(chordTable);
    vLayout->addLayout(hLayoutB);



    chartADialog->setLayout(vLayout);
    connect(restButton,&QPushButton::clicked,this,&propellerDisplay::resetPropChord);

    connect(chordTable, &QTableWidget::itemChanged, this, &propellerDisplay::onChordItemChanged);
    connect(saveButton,&QPushButton::clicked,this,&propellerDisplay::exportToChordTXT);


}
void propellerDisplay::initialDialogB(){
    chartBDialog = new QDialog();
    QVBoxLayout *vLayout = new QVBoxLayout();
    QHBoxLayout *hLayout = new QHBoxLayout();
    chartB = new QChart();   
    chartViewB = new MyChartView(chartB);
    chartViewB->setRenderHint(QPainter::Antialiasing);
    twistSeries = new QLineSeries;
    optTwistSeries = new QLineSeries;
    optTwistScatterSeries = new QScatterSeries;
    optTwistScatterSeriesB = new QScatterSeries;
    twistAxisX = new QValueAxis;
    twistAxisY = new QValueAxis;
    twistSpinBox = new QSpinBox();
    QLabel *orderLabel = new QLabel();

    QPushButton *restButton = new QPushButton();
    QPushButton *saveButton = new QPushButton();
    twistTable = new QTableWidget(2,10);
    twistTable->setFixedHeight(120);

    optTwistScatterSeries->setMarkerSize(20);
    optTwistScatterSeriesB->setMarkerSize(12);
    optTwistScatterSeries->setColor(Qt::red);
    optTwistScatterSeriesB->setColor(Qt::white);


    chartBDialog->resize(1250,700);

    orderLabel->setText("光顺阶数");
    twistSpinBox->setMinimum(1);
    twistSpinBox->setMaximum(6);
    twistSpinBox->setValue(3);
    restButton->setText("还原");
    saveButton->setText("导出数据");



    chartB->addSeries(twistSeries);
    chartB->addSeries(optTwistSeries);
    chartB->addSeries(optTwistScatterSeries);
    chartB->addSeries(optTwistScatterSeriesB);
    chartB->addAxis(twistAxisX,Qt::AlignBottom);
    chartB->addAxis(twistAxisY,Qt::AlignLeft);
    twistSeries->attachAxis(twistAxisX);
    twistSeries->attachAxis(twistAxisY);
    optTwistSeries->attachAxis(twistAxisX);
    optTwistSeries->attachAxis(twistAxisY);
    optTwistScatterSeries->attachAxis(twistAxisX);
    optTwistScatterSeries->attachAxis(twistAxisY);
    optTwistScatterSeriesB->attachAxis(twistAxisX);
    optTwistScatterSeriesB->attachAxis(twistAxisY);
    twistAxisX->setRange(0,1);
    twistAxisY->setRange(0,90);
    twistSeries->setName("原始扭转角分布");
    optTwistSeries->setName("优化扭转角分布");

    QLegend *legend = chartB->legend();

    if (legend) {
        // 直接访问图例项，知道每个系列的图例项位置
        QList<QLegendMarker*> markers = legend->markers();
        for(int i = 0;i<2;i++){
            markers.at(i)->setVisible(true);
        }
        markers.at(2)->setVisible(false);
        markers.at(3)->setVisible(false);

    }



    //chartB->legend()->setVisible(false);
    hLayout->addWidget(orderLabel);
    hLayout->addWidget(twistSpinBox);
    hLayout->addWidget(restButton);
    hLayout->addWidget(saveButton);


    vLayout->addWidget(chartViewB);
    vLayout->addWidget(twistTable);
    vLayout->addLayout(hLayout);


    chartBDialog->setLayout(vLayout);
    connect(restButton,&QPushButton::clicked,this,&propellerDisplay::resetPropTwist);
    connect(twistTable, &QTableWidget::itemChanged, this, &propellerDisplay::onTwistItemChanged);
    connect(saveButton,&QPushButton::clicked,this,&propellerDisplay::exportToTwistTXT);


}
void propellerDisplay::drawPropTwistOnce(){
    if(twistSeries->count() > 0){
        twistSeries->clear();
        optTwistSeries->clear();
        optTwistScatterSeries->clear();
        optTwistScatterSeriesB->clear();

    }
    if(!propDesign->oldPropTwist.isEmpty()){
        for(int i = 0;i<propDesign->oldPropTwist.length();i++){
            twistSeries->append(propDesign->damiterArray[i],propDesign->oldPropTwist[i]);
        }
        QPen pen1;
        pen1.setStyle(Qt::DotLine);
        pen1.setColor(Qt::black);
        pen1.setWidth(2);
        twistSeries->setPen(pen1);
        for(int i = 0;i<propDesign->propTwist.length();i++){
            optTwistSeries->append(propDesign->damiterArray[i],propDesign->propTwist[i]);
        }
        for(int i = 0;i<propDesign->oldPropTwist.length();i++){
            optTwistScatterSeries->append(propDesign->damiterArray[i],propDesign->propTwist[i]);
            optTwistScatterSeriesB->append(propDesign->damiterArray[i],propDesign->propTwist[i]);
        }
    }

}
void propellerDisplay::drawPropTwist(){
    if(optTwistSeries->count() > 0){
        optTwistSeries->clear();
        optTwistScatterSeries->clear();
        optTwistScatterSeriesB->clear();
    }
    if(!propDesign->propTwist.isEmpty()){
        for(int i = 0;i<propDesign->propTwist.length();i++){
            optTwistSeries->append(propDesign->damiterArray[i],propDesign->propTwist[i]);
        }
        for(int i = 0;i<propDesign->oldPropTwist.length();i++){
            optTwistScatterSeries->append(propDesign->damiterArray[i],propDesign->propTwist[i]);
            optTwistScatterSeriesB->append(propDesign->damiterArray[i],propDesign->propTwist[i]);
        }
    }
}
void propellerDisplay::drawPropChordOnce(){
    if(chordSeries->count() > 0){
        chordSeries->clear();
        optChordSeries->clear();
        xAtaqueSeries->clear();
        xSalidaSeries->clear();
        xAtaqueScatterSeries->clear();
        xSalidaScatterSeries->clear();
        xAtaqueScatterSeriesB->clear();
        xSalidaScatterSeriesB->clear();
    }
    if(!propDesign->oldPropChord.isEmpty()){
        for(int i = 0;i<propDesign->oldPropChord.length();i++){
            chordSeries->append(propDesign->damiterArray[i],propDesign->oldPropChord[i]);
        }
        QPen pen1;
        pen1.setStyle(Qt::DotLine);
        pen1.setColor(Qt::black);
        pen1.setWidth(2);
        chordSeries->setPen(pen1);
        for(int i = 0;i<propDesign->oldPropChord.length();i++){
            optChordSeries->append(propDesign->damiterArray[i],propDesign->propChord[i]);
        }
        for(int i = 0;i<propDesign->oldPropChord.length();i++){
            xAtaqueSeries->append(propDesign->damiterArray[i],propDesign->xAtaque[i]);
        }
        for(int i = 0;i<propDesign->oldPropChord.length();i++){
            xSalidaSeries->append(propDesign->damiterArray[i],propDesign->xSalida[i]);
        }
        for(int i = 0;i<propDesign->oldPropChord.length();i++){
            xAtaqueScatterSeries->append(propDesign->damiterArray[i],propDesign->xAtaque[i]);
            xAtaqueScatterSeriesB->append(propDesign->damiterArray[i],propDesign->xAtaque[i]);
        }
        for(int i = 0;i<propDesign->oldPropChord.length();i++){
            xSalidaScatterSeries->append(propDesign->damiterArray[i],propDesign->xSalida[i]);
            xSalidaScatterSeriesB->append(propDesign->damiterArray[i],propDesign->xSalida[i]);
        }
    }
}
void propellerDisplay::drawPropChord(){
    if(optChordSeries->count() > 0){
        optChordSeries->clear();
        xAtaqueSeries->clear();
        xSalidaSeries->clear();
        xAtaqueScatterSeries->clear();
        xSalidaScatterSeries->clear();
        xAtaqueScatterSeriesB->clear();
        xSalidaScatterSeriesB->clear();
    }
    if(!propDesign->propChord.isEmpty()){

        for(int i = 0;i<propDesign->oldPropChord.length();i++){
            optChordSeries->append(propDesign->damiterArray[i],propDesign->propChord[i]);
        }
        for(int i = 0;i<propDesign->oldPropChord.length();i++){
            xAtaqueSeries->append(propDesign->damiterArray[i],propDesign->xAtaque[i]);
        }
        for(int i = 0;i<propDesign->oldPropChord.length();i++){
            xSalidaSeries->append(propDesign->damiterArray[i],propDesign->xSalida[i]);
        }
        for(int i = 0;i<propDesign->oldPropChord.length();i++){
            xAtaqueScatterSeries->append(propDesign->damiterArray[i],propDesign->xAtaque[i]);
            xAtaqueScatterSeriesB->append(propDesign->damiterArray[i],propDesign->xAtaque[i]);
        }
        for(int i = 0;i<propDesign->oldPropChord.length();i++){
            xSalidaScatterSeries->append(propDesign->damiterArray[i],propDesign->xSalida[i]);
            xSalidaScatterSeriesB->append(propDesign->damiterArray[i],propDesign->xSalida[i]);
        }
    }
}
void propellerDisplay::drawOldPropTwist(){
    if(optTwistSeries->count() > 0){
        optTwistSeries->clear();
        optTwistScatterSeries->clear();
        optTwistScatterSeriesB->clear();
    }
    if(!propDesign->propTwist.isEmpty()){
        for(int i = 0;i<propDesign->propTwist.length();i++){
            optTwistSeries->append(propDesign->damiterArray[i],propDesign->oldPropTwist[i]);
        }
        for(int i = 0;i<propDesign->oldPropTwist.length();i++){
            optTwistScatterSeries->append(propDesign->damiterArray[i],propDesign->oldPropTwist[i]);
            optTwistScatterSeriesB->append(propDesign->damiterArray[i],propDesign->oldPropTwist[i]);
        }
    }
}
void propellerDisplay::drawOldPropChord(){
    if(optChordSeries->count() > 0){
        optChordSeries->clear();
        xAtaqueSeries->clear();
        xSalidaSeries->clear();
        xAtaqueScatterSeries->clear();
        xSalidaScatterSeries->clear();
        xAtaqueScatterSeriesB->clear();
        xSalidaScatterSeriesB->clear();
    }
    if(!propDesign->propChord.isEmpty()){

        for(int i = 0;i<propDesign->oldPropChord.length();i++){
            optChordSeries->append(propDesign->damiterArray[i],propDesign->oldPropChord[i]);
        }
        for(int i = 0;i<propDesign->oldPropChord.length();i++){
            xAtaqueSeries->append(propDesign->damiterArray[i],propDesign->oldXAtaque[i]);
        }
        for(int i = 0;i<propDesign->oldPropChord.length();i++){
            xSalidaSeries->append(propDesign->damiterArray[i],propDesign->oldXSalida[i]);
        }
        for(int i = 0;i<propDesign->oldPropChord.length();i++){
            xAtaqueScatterSeries->append(propDesign->damiterArray[i],propDesign->oldXAtaque[i]);
            xAtaqueScatterSeriesB->append(propDesign->damiterArray[i],propDesign->oldXAtaque[i]);
        }
        for(int i = 0;i<propDesign->oldPropChord.length();i++){
            xSalidaScatterSeries->append(propDesign->damiterArray[i],propDesign->oldXSalida[i]);
            xSalidaScatterSeriesB->append(propDesign->damiterArray[i],propDesign->oldXSalida[i]);
        }
    }

}
void propellerDisplay::switchTopView(){
    vtkSmartPointer<vtkCamera> camera = renderer->GetActiveCamera();

    // 设置相机位置（从上方俯视）
    camera->SetPosition(0.0, 0.0, 5.0); // 从 Z 轴正方向俯视

    // 设置相机焦点
    camera->SetFocalPoint(0.0, 0.0, 0.0); // 观察点在原点

    // 设置视图上方向
    camera->SetViewUp(0.0, 1.0, 0.0); // Y 轴向上

    // 更新相机设置
    renderer->ResetCameraClippingRange();
    renwin->Render(); // 重新渲染以应用新的视图
}
void propellerDisplay::switchMainView(){
    vtkSmartPointer<vtkCamera> camera = renderer->GetActiveCamera();

    // 设置相机位置（从上方俯视）
    camera->SetPosition(5.0, 0, 0.0); // 从 轴正方向俯视

    // 设置相机焦点
    camera->SetFocalPoint(0.0, 0.0, 0.0); // 观察点在原点

    // 设置视图上方向
    camera->SetViewUp(0.0, 0.0, 1.0); //  轴向上

    // 更新相机设置
    renderer->ResetCameraClippingRange();
    renwin->Render(); // 重新渲染以应用新的视图
}
void propellerDisplay::switchLeftView(){
    vtkSmartPointer<vtkCamera> camera = renderer->GetActiveCamera();

    // 设置相机位置（从上方俯视）
    camera->SetPosition(0.0, 5, 0.0); // 从 轴正方向俯视

    // 设置相机焦点
    camera->SetFocalPoint(0.0, 0.0, 0.0); // 观察点在原点

    // 设置视图上方向
    camera->SetViewUp(0.0, 0.0, 1.0); //  轴向上

    // 更新相机设置
    renderer->ResetCameraClippingRange();
    renwin->Render(); // 重新渲染以应用新的视图
}
void propellerDisplay::changeUIForPropResult(){
    propResultTypeCombox->setEnabled(true);
    changeModelViewButton->setEnabled(true);
    changeResultViewButton->setEnabled(false);
    propDisplayWidgetE->hide();
    resultView->show();

}
void propellerDisplay::changeUIForPropDesign(){
    propResultTypeCombox->setEnabled(false);
    changeModelViewButton->setEnabled(false);
    changeResultViewButton->setEnabled(true);
    propDisplayWidgetE->show();
    resultView->hide();
}
void propellerDisplay::changeProgressValue(const int value){
    emit emitPropellerProgressBarValue(value);
}
void propellerDisplay::changeRpm(const int rpm){
    QSpinBox *spinbox = qobject_cast<QSpinBox*>(sender());
    int index = spinbox->property("rpm").toInt();
    choiceIndex = index;
    rpmArray[index] = rpm;

    drawResultChartA();
    drawResultChartB();
    drawResultChartC();
    drawResultChartD();
}
void propellerDisplay::changePropTwist(const double s){

    int order = twistSpinBox->value();
    propDesign->propTwist[controlTwistIndex] = s;
    propDesign->smoothPropTwist(order);
    changeTwistTableValue(1,propDesign->propTwist);
    drawPropTwist();
    propDesign->updateGeometry();
    updateView();

}
void propellerDisplay::changePropChord(const double s){
    int index = controlChordIndex;
    int order = chordSpinBox->value();
    if(controlChordIndex <= propDesign->propNb - 1){

        propDesign->xAtaque[index] = s;
        propDesign->smoothPropXAtaque(order);
        changeChordTableValue(2,propDesign->xAtaque);

    }else{
        index = controlChordIndex - propDesign->propNb;
        propDesign->xSalida[index] = s;
        propDesign->smoothPropXSalida(order);
        changeChordTableValue(3,propDesign->xSalida);
    }
    changeChordTableValue(1,propDesign->propChord);
    drawPropChord();
    propDesign->updateGeometry();
    updateView();
}
void propellerDisplay::changePropXMesh(){
    if(!propDesign->damiterArray.isEmpty()){
        int num = propDesignEditArray[10]->text().toInt();
        double yte = yteValueEdit->text().toDouble() / 1000;

        if(num <= 1)
            num = 10;
        propDesign->meshX = num;
        propDesign->yteType = yteTypeCombobox->currentIndex();
        propDesign->ctYte = yte;
        propDesign->computeCst();
        propDesign->updateGeometry();
        updateView();
    }

}
void propellerDisplay::resetPropTwist(){

    propDesign->propTwist = propDesign->oldPropTwist;


    drawOldPropTwist();
    changeTwistTableValue(1,propDesign->propTwist);
    propDesign->updateGeometry();
    updateView();
}
void propellerDisplay::resetPropChord(){
    drawOldPropChord();
    propDesign->xSalida = propDesign->oldXSalida;
    propDesign->xAtaque = propDesign->oldXAtaque;
    changeChordTableValue(1,propDesign->propChord);
    changeChordTableValue(2,propDesign->xAtaque);
    changeChordTableValue(3,propDesign->xSalida);


    propDesign->updateGeometry();
    updateView();
}
void propellerDisplay::changePropDisplay(QTreeWidgetItem* item, int column){
    if (!item->parent()) { // 判断是否是子节点
        int index = item->data(0, Qt::UserRole).toInt();
        propChoiceIndex = index;
        //hidePressureContourView();//关闭云图
        //updateGLTextA(wingDataToString(wingDataArray[index]));

        //


        //刷新模型视图
        updatePropDefineWidget(propArray[index]);
        //刷新结果视图  刷新状态视图
        if(VLMSolverArray[index]->isAnalyse){
            drawPropResult(0);//刷新结果
            int row = VLMSettingArray[index].vinfArray.length();
            int column = VLMSettingArray[index].rpmArray.length();
            propStateCombox->setEnabled(true);
            propStateCombox->blockSignals(true);
            propStateCombox->clear();
            for(int i = 0;i<row;i++){
                for(int j = 0;j<column;j++){
                    int k = i * column + j + 1;
                    QString text = "状态" + QString::number(k);
                    propStateCombox->addItem(text);
                }
            }
            propStateCombox->blockSignals(false);
            updateGLTextA(resultToString());
        }else{
            propStateCombox->setEnabled(false);
        }
    }
}

void propellerDisplay::changePropData(QTreeWidgetItem* item, int column){
    if (item->parent()) { // 判断是否是子节点



        QString txt = item->data(0,Qt::UserRole).toString();
        QString txtTmp = txt.mid(0,10);
        QString tmp1 = txt.mid(10);
        QString tmp = "childShape";

        if(tmp == txtTmp &&  tmp1.toInt() == propChoiceIndex){
            propOutputButton->setEnabled(true);

            isModify = true;
            updatePropEdit(propChoiceIndex);
            propOutputButton->setEnabled(true);
            propDefineWidget->show();
            updateDefineView(propArray[propChoiceIndex]);


            //defineWing->updatewingDefineWidget(wingDataArray[wingChoiceIndex]);
        }
    }

}

void propellerDisplay::showPropPressureContourView(QTreeWidgetItem* item, int column){
    if (item->checkState(column) == Qt::Checked) { // 判断是否是子节点



        QString txt = item->data(0,Qt::UserRole).toString();
        QString txtTmp = txt.mid(0,13);
        QString tmp1 = txt.mid(13);
        QString tmp = "childPressure";
        if(tmp == txtTmp &&  tmp1.toInt() == propChoiceIndex){
            //zeroLiftDragChoiceDialog->show();
            //defineWing->updatewingDefineWidget(wingDataArray[wingChoiceIndex]);
            //defineWing->updateStreamLineView();
            //qDebug()<<"PrssureContour"<<wingChoiceIndex;
            updatePressureContourView(VLMSolverArray[propChoiceIndex]);

        }
    }else{

        QString txt = item->data(0,Qt::UserRole).toString();
        QString txtTmp = txt.mid(0,13);
        QString tmp1 = txt.mid(13);
        QString tmp = "childPressure";
        if(tmp == txtTmp &&  tmp1.toInt() == propChoiceIndex){
            //zeroLiftDragChoiceDialog->show();
            //defineWing->updatewingDefineWidget(wingDataArray[wingChoiceIndex]);
            //qDebug()<<"NoPrssureContour"<<wingChoiceIndex;
            hidePressureContourView();
        }
    }

}
void propellerDisplay::showPropStreamLineView(QTreeWidgetItem* item, int column){
    if (item->checkState(column) == Qt::Checked) { // 判断是否是子节点
        QString txt = item->data(0,Qt::UserRole).toString();
        QString txtTmp = txt.mid(0,15);
        QString tmp1 = txt.mid(15);
        QString tmp = "childStreamLine";
        if(tmp == txtTmp &&  tmp1.toInt() == propChoiceIndex){
            streamSettingDialog->show();
        }
    }else{
        QString txt = item->data(0,Qt::UserRole).toString();
        QString txtTmp = txt.mid(0,15);
        QString tmp1 = txt.mid(15);
        QString tmp = "childStreamLine";
        if(tmp == txtTmp &&  tmp1.toInt() == propChoiceIndex){
            hideStreamLineView();
        }
    }
}
void propellerDisplay::changeTwistTableValue(const int row, const QVector<double> &array){
    int colCount = twistTable->columnCount();
    for(int i = 0;i<colCount;i++){
         QTableWidgetItem *item = twistTable->item(row, i);
         item->setText(QString::number(array[i],'f',1));
    }

}
void propellerDisplay::changeChordTableValue(const int row, const QVector<double> &array){

    int colCount = chordTable->columnCount();
    for(int i = 0;i<colCount;i++){
         QTableWidgetItem *item = chordTable->item(row, i);
         item->setText(QString::number(array[i],'f',4));
    }
}
bool propellerDisplay::returnPropNumTwist(const QPointF& position){

    bool isinside = false;
    double err = 0.02;
    int length = propDesign->propNb;

    double x = (position.x() - propDesign->propRh) / (propDesign->propR - propDesign->propRh);
    int index = (x + err) * (length - 1);

    if(position.x()>propDesign->damiterArray[index] - err&&position.x()<propDesign->damiterArray[index] + err){
        isinside = true;
        controlTwistIndex = index;

    }
    else{
        isinside = false;
    }
    return isinside;
}
bool propellerDisplay::returnPropNumChord(const QPointF& position){
    bool isinside = false;
    double err = 0.02;
    int length = propDesign->propNb;
    double x = (position.x() - propDesign->propRh) / (propDesign->propR - propDesign->propRh);
    int index = (x + err) * (length - 1);

    //qDebug()<<propDesign->damiterArray[index]<<" "<<position.x();
    if(position.x()>propDesign->damiterArray[index] - err&&position.x()<propDesign->damiterArray[index] + err){
        double err1 = abs(position.y() - propDesign->xAtaque[index]);
        double err2 = abs(position.y() - propDesign->xSalida[index]);
        if(err1 < err2){
            controlChordIndex = index;
        }else{
            controlChordIndex = index + propDesign->propNb;
        }

        isinside = true;

    }
    else{
        isinside = false;
    }

    return isinside;
}
void propellerDisplay::exportToTwistTXT() {
    // 使用QFileDialog选择文件路径和文件名
    QString fileName = QFileDialog::getSaveFileName(this, "Save Table Data", "", "Text Files (*.txt);;All Files (*)");

    if (fileName.isEmpty()) {
        return; // 用户取消了保存对话框
    }

    QFile file(fileName);

    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::warning(this, "Export Error", "Failed to open file for writing.");
        return;
    }

    QTextStream out(&file);

    int rowCount = twistTable->rowCount();
    int colCount = twistTable->columnCount();

    // 确保表头项存在
    if (rowCount > 0) {
        // 输出列标题（如果需要，可以取消注释下面的代码）
        // out << "Row Header\t"; // 如果需要添加列标题，可以加上这一行

        // 遍历所有列
        for (int col = 0; col < colCount; ++col) {
            if (col < colCount - 1) {
                out << "Column " << col + 1 << "\t"; // 默认列标题
            } else {
                out << "Column " << col + 1; // 最后一列不加制表符
            }
        }
        out << "\n"; // 换行
    }

    // 遍历所有行
    for (int row = 0; row < rowCount; ++row) {
        // 输出行标题
        QTableWidgetItem *rowHeaderItem = twistTable->verticalHeaderItem(row);
        if (rowHeaderItem) {
            out << rowHeaderItem->text() << "\t";
        } else {
            out << "Row " << row + 1 << "\t"; // 默认行标题
        }

        // 遍历所有列
        for (int col = 0; col < colCount; ++col) {
            QTableWidgetItem *item = twistTable->item(row, col);
            if (item) {
                out << item->text();
            } else {
                out << ""; // 如果单元格项为空，则输出空值
            }
            if (col < colCount - 1) {
                out << "\t"; // 使用制表符分隔列
            }
        }
        out << "\n"; // 换行
    }

    file.close();
    QMessageBox::information(this, "提示", "保存成功");
}


void propellerDisplay::exportToChordTXT() {
    // 使用QFileDialog选择文件路径和文件名
    QString fileName = QFileDialog::getSaveFileName(this, "Save Table Data", "", "Text Files (*.txt);;All Files (*)");

    if (fileName.isEmpty()) {
        return; // 用户取消了保存对话框
    }

    QFile file(fileName);

    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::warning(this, "Export Error", "Failed to open file for writing.");
        return;
    }

    QTextStream out(&file);

    int rowCount = chordTable->rowCount();
    int colCount = chordTable->columnCount();

    // 确保表头项存在
    if (rowCount > 0) {
        // 输出列标题（如果需要，可以取消注释下面的代码）
        // out << "Row Header\t"; // 如果需要添加列标题，可以加上这一行

        // 遍历所有列
        for (int col = 0; col < colCount; ++col) {
            if (col < colCount - 1) {
                out << "Column " << col + 1 << "\t"; // 默认列标题
            } else {
                out << "Column " << col + 1; // 最后一列不加制表符
            }
        }
        out << "\n"; // 换行
    }

    // 遍历所有行
    for (int row = 0; row < rowCount; ++row) {
        // 输出行标题
        QTableWidgetItem *rowHeaderItem = chordTable->verticalHeaderItem(row);
        if (rowHeaderItem) {
            out << rowHeaderItem->text() << "\t";
        } else {
            out << "Row " << row + 1 << "\t"; // 默认行标题
        }

        // 遍历所有列
        for (int col = 0; col < colCount; ++col) {
            QTableWidgetItem *item = chordTable->item(row, col);
            if (item) {
                out << item->text();
            } else {
                out << ""; // 如果单元格项为空，则输出空值
            }
            if (col < colCount - 1) {
                out << "\t"; // 使用制表符分隔列
            }
        }
        out << "\n"; // 换行
    }

    file.close();
    QMessageBox::information(this, "提示", "保存成功");
}

void propellerDisplay::outputAnalysePropData(){
    propellerVLM *solver = new propellerVLM();

    solver->initialModel(getPropDefine());
    solver->generate3DModel();
    if(solver ->model.isEmpty()){
        QMessageBox::information(this,"警告","模型未创建");
        return;
    }

    QString fileName = QFileDialog::getSaveFileName(nullptr, "Save Data", "", "CSV Files (*.csv)");
    if (!fileName.isEmpty()) {
        QFile file(fileName);
        if (file.open(QIODevice::WriteOnly)) {
            QTextStream stream(&file);
            stream<<"StartLoft\n";

            int len = solver->chordLengthW.length();

            for(int i = 0;i<len;i++){
                stream<<"StartCurve\n";
                QVector<point3d>outData = solver->model[i];
                for(int j = 0;j<outData.length();j++){
                    stream<<outData[j].x * 1000<<","<<outData[j].y * 1000<<","<<outData[j].z * 1000<<"\n";
                }
                stream<<"EndCurve\n";
            }
            stream<<"EndLoft\n";
            stream<<"End";
            file.close();
        }
    }
    delete solver;
    solver = nullptr;
    QMessageBox::information(this, "信息", "保存成功");


}
void propellerDisplay::outputPropData(){
    if(propDesign->model.isEmpty()){
        QMessageBox::information(this,"警告","模型未创建");
        return;
    }

    QString fileName = QFileDialog::getSaveFileName(nullptr, "Save Data", "", "CSV Files (*.csv)");
    if (!fileName.isEmpty()) {
        QFile file(fileName);
        if (file.open(QIODevice::WriteOnly)) {
            QTextStream stream(&file);
            stream<<"StartLoft\n";

            int len = propDesign->propNb;

            for(int i = 0;i<len;i++){
                stream<<"StartCurve\n";
                QVector<point3d>outData = propDesign->model[i];
                for(int j = 0;j<outData.length();j++){
                    stream<<outData[j].x * 1000<<","<<outData[j].y * 1000<<","<<outData[j].z * 1000<<"\n";
                }
                stream<<"EndCurve\n";
            }
            stream<<"EndLoft\n";
            stream<<"End";
            file.close();
        }
    }

    QMessageBox::information(this, "信息", "保存成功");

}
void propellerDisplay::updateMousePositionTwist(const QPointF &localPoint){

    double realy = localPoint.y();

    if(isPropMousePressed) {
        if(returnPropNumTwist(localPoint)){
            changePropTwist(realy);
        }
    }
}
void propellerDisplay::updateMousePositionChord(const QPointF &localPoint){

    double realy = localPoint.y();

    if(isPropMousePressed) {
        if(returnPropNumChord(localPoint)){
            changePropChord(realy);
        }
    }
}
void propellerDisplay::getMousePressBeginPosition(const QPointF &beginPoint){

    //mouseStartPosition = beginPoint;
    isPropMousePressed = true;
}

void propellerDisplay::getMouseReleaseEndPosition(const QPointF &endPoint){


    isPropMousePressed = false;
    // if(chose >= 1)
    //     updateResultChart();
}


void propellerDisplay::initialTwistTable() {
    // 获取列数
    int colNum = propDesign->propTwist.length();
    int colWidth = 1100 / colNum;

    // 清除当前表格数据和项
    for (int row = 0; row < twistTable->rowCount(); ++row) {
        for (int col = 0; col < twistTable->columnCount(); ++col) {
            QTableWidgetItem *item = twistTable->takeItem(row, col);
            delete item; // 手动删除项以释放内存
        }
    }
    twistTable->clear();  // 清除表格中的所有项
    twistTable->setRowCount(2);  // 重置行数
    twistTable->setColumnCount(colNum);  // 重置列数

    // 设置列数
    if (colNum > 0) {
        twistTable->setColumnCount(colNum);
    }

    if (!propDesign->propTwist.isEmpty()) {
        // 设置行标题
        twistTable->setVerticalHeaderItem(0, new QTableWidgetItem("原始扭转角(°)"));
        twistTable->setVerticalHeaderItem(1, new QTableWidgetItem("优化扭转角(°)"));

        // 设置单元格项
        for (int i = 0; i < propDesign->oldPropTwist.length(); ++i) {
            QTableWidgetItem *item = new QTableWidgetItem(QString::number(propDesign->oldPropTwist[i]));
            item->setFlags(item->flags() & ~Qt::ItemIsEditable); // 禁用编辑
            item->setBackground(QBrush(QColor("#E0E0E0"))); // 浅灰色背景
            item->setForeground(QBrush(QColor("#606060"))); // 深灰色文字

            twistTable->setItem(0, i, item);
        }

        for (int i = 0; i < propDesign->propTwist.length(); ++i) {
            QTableWidgetItem *item = new QTableWidgetItem(QString::number(propDesign->propTwist[i]));
            item->setBackground(QBrush(QColor("#F9F9F9"))); // 浅灰色背景
            item->setForeground(QBrush(QColor("#003366"))); // 深蓝色文字

            twistTable->setItem(1, i, item);
        }

        // 设置列宽
        for (int i = 0; i < propDesign->propTwist.length(); ++i) {
            twistTable->setColumnWidth(i, colWidth);
        }
    }
}

void propellerDisplay::initialChordTable() {
    int colNum = propDesign->propChord.length();
    int colWidth = 1200 / colNum;

    // 清除当前表格数据和项
    for (int row = 0; row < chordTable->rowCount(); ++row) {
        for (int col = 0; col < chordTable->columnCount(); ++col) {
            QTableWidgetItem *item = chordTable->takeItem(row, col);
            delete item; // 手动删除项以释放内存
        }
    }
    chordTable->clear();


    chordTable->setRowCount(4);  // 重置行数
    chordTable->setColumnCount(colNum);  // 重置列数

    // 设置列数
    if (colNum > 0) {
        chordTable->setColumnCount(colNum);
    }

    if (!propDesign->propChord.isEmpty()) {
        // 设置行标题
        chordTable->setVerticalHeaderItem(0, new QTableWidgetItem("原始弦长(m)"));
        chordTable->setVerticalHeaderItem(1, new QTableWidgetItem("优化弦长(m)"));
        chordTable->setVerticalHeaderItem(2, new QTableWidgetItem("前缘坐标(m)"));
        chordTable->setVerticalHeaderItem(3, new QTableWidgetItem("后缘坐标(m)"));

        // 设置数据项
        for (int i = 0; i < propDesign->oldPropChord.length(); ++i) {
            QTableWidgetItem *item = new QTableWidgetItem(QString::number(propDesign->oldPropChord[i]));
            item->setFlags(item->flags() & ~Qt::ItemIsEditable); // 禁用编辑
            item->setBackground(QBrush(QColor("#E0E0E0"))); // 浅灰色背景
            item->setForeground(QBrush(QColor("#606060"))); // 深灰色文字
            chordTable->setItem(0, i, item);
        }

        for (int i = 0; i < propDesign->propChord.length(); ++i) {
            QTableWidgetItem *item = new QTableWidgetItem(QString::number(propDesign->propChord[i]));
            item->setFlags(item->flags() & ~Qt::ItemIsEditable); // 禁用编辑
            item->setBackground(QBrush(QColor("#E0E0E0"))); // 浅灰色背景
            item->setForeground(QBrush(QColor("#606060"))); // 深灰色文字
            chordTable->setItem(1, i, item);
        }

        for (int i = 0; i < propDesign->xAtaque.length(); ++i) {
            QTableWidgetItem *item = new QTableWidgetItem(QString::number(propDesign->xAtaque[i]));
            item->setBackground(QBrush(QColor("#F9F9F9"))); // 浅灰色背景
            item->setForeground(QBrush(QColor("#003366"))); // 深蓝色文字
            chordTable->setItem(2, i, item);
        }

        for (int i = 0; i < propDesign->xSalida.length(); ++i) {
            QTableWidgetItem *item = new QTableWidgetItem(QString::number(propDesign->xSalida[i]));
            item->setBackground(QBrush(QColor("#F9F9F9"))); // 浅灰色背景
            item->setForeground(QBrush(QColor("#003366"))); // 深蓝色文字
            chordTable->setItem(3, i, item);
        }

        // 设置列宽
        for (int i = 0; i < colNum; ++i) {
            chordTable->setColumnWidth(i, colWidth);
        }
    }
}

void propellerDisplay::onTwistItemChanged(QTableWidgetItem *item){

    int col = item->column();
    double tmp = item->text().toDouble();
    propDesign->propTwist[col] = tmp;
    drawPropTwist();


}
void propellerDisplay::onChordItemChanged(QTableWidgetItem *item){
    int row = item->row();
    int col = item->column();
    double tmp = item->text().toDouble();
    if(row == 2){
        propDesign->xAtaque[col] = tmp;

    }else if(row == 3){
        propDesign->xSalida[col] = tmp;
    }
    drawPropChord();


}
void propellerDisplay::showPropSettingVinfDialog(QTreeWidgetItem* item, int column){

    if (item->parent()) { // 判断是否是子节点

        QString txt = item->data(0,Qt::UserRole).toString();
        QString txtTmp = txt.mid(0,9);
        QString tmp1 = txt.mid(9);
        QString tmp = "childVinf";


        if(tmp == txtTmp &&  tmp1.toInt() == propChoiceIndex){
            //wingMinAlphaEdit->setText(QString::number(wingSettingArray[wingChoiceIndex].minAlpha));
            //wingMaxAlphaEdit->setText(QString::number(wingSettingArray[wingChoiceIndex].maxAlpha));
            //wingStepAlphaEdit->setText(QString::number(wingSettingArray[wingChoiceIndex].stepAlpha));
            //wingVelocityEdit->setText(QString::number(wingSettingArray[wingChoiceIndex].vinf));


            propSettingDialog->show();

        }

    }
}

void propellerDisplay::showResultNode(QTreeWidgetItem *root, const QString &textToFind) {
    for (int i = 0; i < root->childCount(); ++i) {
        QTreeWidgetItem *child = root->child(i);
        if (child->text(0) == textToFind) {
            child->setHidden(false); // 显示匹配的节点
            return;
        } else {
            showResultNode(child, textToFind); // 递归遍历子节点
        }
    }
}
void propellerDisplay::hideResultNode(QTreeWidgetItem *root, const QString &textToFind) {
    for (int i = 0; i < root->childCount(); ++i) {
        QTreeWidgetItem *child = root->child(i);
        if (child->text(0) == textToFind) {
            child->setHidden(true); // 显示匹配的节点
            return;
        } else {
            hideResultNode(child, textToFind); // 递归遍历子节点
        }
    }
}
void propellerDisplay::savePropellerData(){
    propDefineWidget->hide();
    wingDefinition newPropData = getPropDefine();

    if(!isModify){

        propellerVLM *solver = new propellerVLM;
        VLMSolverArray.append(solver);
        QString path1 = ":/images/propellers.png";
        QIcon icon1;
        icon1.addPixmap(QPixmap(path1),QIcon::Normal,QIcon::On);

        QString path2 = ":/images/edit.png";
        QIcon icon2 ;
        icon2.addPixmap(QPixmap(path2),QIcon::Normal,QIcon::On);

        QTreeWidgetItem *fItem = new QTreeWidgetItem(propTreeWidget,QStringList(QString(newPropData.name)));
        fItem->setData(0,Qt::UserRole, QVariant(QString::number(propellerIndex)));
        fItem->setIcon(0,icon1);

        QTreeWidgetItem *fItemGeometry = new QTreeWidgetItem(fItem,QStringList("几何模型"));
        fItemGeometry->setData(0,Qt::UserRole,"fItemGeometry");

        QTreeWidgetItem *childShape = new QTreeWidgetItem(fItemGeometry,QStringList("平面形状"));
        childShape->setData(0, Qt::UserRole, QVariant("childShape" + QString::number(propellerIndex)));
        childShape->setIcon(0,icon2);

        //QTreeWidgetItem *childtwist = new QTreeWidgetItem(fItemGeometry,QStringList("扭转角"));
        //childtwist->setData(0, Qt::UserRole, QVariant("childtwist" + QString::number(propellerIndex)));
        //childtwist->setIcon(0,icon2);

        QTreeWidgetItem *fItemAnalyse = new QTreeWidgetItem(fItem,QStringList("求解设置"));
        fItemAnalyse->setData(0, Qt::UserRole, QVariant("childSolver" + QString::number(propellerIndex)));
        //fItemAnalyse->setData(0, Qt::UserRole, "fItemAnalyse");

        QTreeWidgetItem *childVinf = new QTreeWidgetItem(fItemAnalyse,QStringList("工况设置"));
        childVinf->setData(0, Qt::UserRole, QVariant("childVinf" + QString::number(propellerIndex)));
        childVinf->setIcon(0,icon2);

        //QTreeWidgetItem *childWake = new QTreeWidgetItem(fItemAnalyse,QStringList("尾迹设置"));
        //childWake->setData(0, Qt::UserRole, QVariant("childWake" + QString::number(propellerIndex)));
        //childWake->setIcon(0,icon2);

        QTreeWidgetItem *fItemResultA = new QTreeWidgetItem(fItem,QStringList("后处理"));
        QTreeWidgetItem *childPrssure = new QTreeWidgetItem(fItemResultA,QStringList("压力"));
        childPrssure->setData(0, Qt::UserRole, QVariant("childPressure" + QString::number(propellerIndex)));
        childPrssure->setCheckState(0,Qt::Unchecked);
        QTreeWidgetItem *childStreamLine = new QTreeWidgetItem(fItemResultA,QStringList("流线"));
        childStreamLine->setData(0, Qt::UserRole, QVariant("childStreamLine" + QString::number(propellerIndex)));
        childStreamLine->setCheckState(0,Qt::Unchecked);
        //QTreeWidgetItem *fItemResultB = new QTreeWidgetItem(fItem,QStringList("结果"));
        //fItemResultB->setData(0, Qt::UserRole, QVariant("childSeriesType"));

        fItemResultA->setHidden(true);
        //fItemResultB->setHidden(true);
        propTreeItemArray.append(fItem);

        propArray.append(newPropData);
        VLMSettingArray.append(initialVLMSetting);
        propChoiceIndex = propellerIndex;
        QVector<QVector<double>>resultTmp;
        bool resultType = false;
        propResultArray.append(resultTmp);
        propResultTypeArray.append(resultType);
        propAirfoilListArray.append(airfoilList);


        propellerIndex++;
        connect(solver,&propellerVLM::emitAnalyseLog,this,&propellerDisplay::updateLog);
        connect(solver,&propellerVLM::emitProgressValue,this,&propellerDisplay::updateBar);
        connect(solver, &propellerVLM::progressUpdated, this, &propellerDisplay::updateProgress);
        connect(solver, &propellerVLM::workFinished, this, &propellerDisplay::onCalculationFinished);
    }else{
        propArray[propChoiceIndex] = newPropData;
    }
    updatePropDefineWidget(newPropData);
}
void propellerDisplay::addPropellerData(wingDefinition &newPropData){
    propellerVLM *solver = new propellerVLM;
    VLMSolverArray.append(solver);
    QString path1 = ":/images/propellers.png";
    QIcon icon1;
    icon1.addPixmap(QPixmap(path1),QIcon::Normal,QIcon::On);

    QString path2 = ":/images/edit.png";
    QIcon icon2 ;
    icon2.addPixmap(QPixmap(path2),QIcon::Normal,QIcon::On);

    QTreeWidgetItem *fItem = new QTreeWidgetItem(propTreeWidget,QStringList(QString(newPropData.name)));
    fItem->setData(0,Qt::UserRole, QVariant(QString::number(propellerIndex)));
    fItem->setIcon(0,icon1);

    QTreeWidgetItem *fItemGeometry = new QTreeWidgetItem(fItem,QStringList("几何模型"));
    fItemGeometry->setData(0,Qt::UserRole,"fItemGeometry");

    QTreeWidgetItem *childShape = new QTreeWidgetItem(fItemGeometry,QStringList("平面形状"));
    childShape->setData(0, Qt::UserRole, QVariant("childShape" + QString::number(propellerIndex)));
    childShape->setIcon(0,icon2);

    //QTreeWidgetItem *childtwist = new QTreeWidgetItem(fItemGeometry,QStringList("扭转角"));
    //childtwist->setData(0, Qt::UserRole, QVariant("childtwist" + QString::number(propellerIndex)));
    //childtwist->setIcon(0,icon2);

    QTreeWidgetItem *fItemAnalyse = new QTreeWidgetItem(fItem,QStringList("求解设置"));
    fItemAnalyse->setData(0, Qt::UserRole, QVariant("childSolver" + QString::number(propellerIndex)));
    //fItemAnalyse->setData(0, Qt::UserRole, "fItemAnalyse");

    QTreeWidgetItem *childVinf = new QTreeWidgetItem(fItemAnalyse,QStringList("工况设置"));
    childVinf->setData(0, Qt::UserRole, QVariant("childVinf" + QString::number(propellerIndex)));
    childVinf->setIcon(0,icon2);

    //QTreeWidgetItem *childWake = new QTreeWidgetItem(fItemAnalyse,QStringList("尾迹设置"));
    //childWake->setData(0, Qt::UserRole, QVariant("childWake" + QString::number(propellerIndex)));
    //childWake->setIcon(0,icon2);

    QTreeWidgetItem *fItemResultA = new QTreeWidgetItem(fItem,QStringList("后处理"));
    QTreeWidgetItem *childPrssure = new QTreeWidgetItem(fItemResultA,QStringList("压力"));
    childPrssure->setData(0, Qt::UserRole, QVariant("childPressure" + QString::number(propellerIndex)));
    childPrssure->setCheckState(0,Qt::Unchecked);
    QTreeWidgetItem *childStreamLine = new QTreeWidgetItem(fItemResultA,QStringList("流线"));
    childStreamLine->setData(0, Qt::UserRole, QVariant("childStreamLine" + QString::number(propellerIndex)));
    childStreamLine->setCheckState(0,Qt::Unchecked);
    //QTreeWidgetItem *fItemResultB = new QTreeWidgetItem(fItem,QStringList("结果"));
    //fItemResultB->setData(0, Qt::UserRole, QVariant("childSeriesType"));

    fItemResultA->setHidden(true);
    //fItemResultB->setHidden(true);
    propTreeItemArray.append(fItem);

    propArray.append(newPropData);
    VLMSettingArray.append(initialVLMSetting);
    propChoiceIndex = propellerIndex;
    QVector<QVector<double>>resultTmp;
    bool resultType = false;
    propResultArray.append(resultTmp);
    propResultTypeArray.append(resultType);
    propAirfoilListArray.append(airfoilList);


    propellerIndex++;
    connect(solver,&propellerVLM::emitAnalyseLog,this,&propellerDisplay::updateLog);
    connect(solver,&propellerVLM::emitProgressValue,this,&propellerDisplay::updateBar);
    connect(solver, &propellerVLM::progressUpdated, this, &propellerDisplay::updateProgress);
    connect(solver, &propellerVLM::workFinished, this, &propellerDisplay::onCalculationFinished);
    updatePropDefineWidget(newPropData);
}
void propellerDisplay::updatePropList(QVector<wingDefinition>&newPropData){
    propellerIndex = 0;
    if(!propArray.isEmpty()){
        for(int i = 0;i<resultSeriesArray.length();i++){
            resultChart->removeSeries(resultSeriesArray[i]);
        }
        for (QTreeWidgetItem* item : propTreeItemArray) {
             delete item; // 删除指针指向的 QTreeWidgetItem 对象
        }
        for(propellerVLM* solver:VLMSolverArray){
            delete solver;
        }
        for(int i = 0;i<resultSeriesArray.length();i++){
            delete resultSeriesArray[i];
        }
        propTreeItemArray.clear();
        resultSeriesArray.clear();
        VLMSolverArray.clear();
        propArray.clear();


    }
    for(int i = 0;i<newPropData.length();i++){
        newPropData[i].airfoilInputType = true;
        newPropData[i].uMeshType = 1;
        newPropData[i].vMeshType = 1;
        SpanNumB = newPropData[i].chordLengthW.length();
        for(int j = 0;j<newPropData[i].chordLengthW.length();j++){
            int index = getAirfoilIndex(newPropData[i].airfoilNameArray[j]);
            newPropData[i].airfoilArray.append(airfoilArray[index]);
            newPropData[i].oldAirfoilArray.append(airfoilArray[index]);


        }
    }



    for(int i = 0;i<newPropData.length();i++){

        airfoilList = newPropData[i].airfoilNameArray.toList();
        addPropellerData(newPropData[i]);

    }


}
void propellerDisplay::showPropStreamLine(){
    streamSettingDialog->hide();
    int ind = propStateCombox->currentIndex();

    //int index = wingAlphaCombox->currentIndex();
    double a1 = streamEditArray[0]->text().toDouble();
    double a2 = streamEditArray[1]->text().toDouble();
    double a3 = streamEditArray[2]->text().toDouble();
    double a4 = streamEditArray[3]->text().toDouble();

    VLMSolverArray[propChoiceIndex]->isShowStreamLine = true;
    if(ind >= 0){
        startCalculation();

        VLMSolverArray[propChoiceIndex]->computeStreamLine(ind,a1,a2,a3,a4);
        updateStreamLineView(VLMSolverArray[propChoiceIndex]);
    }


}
/*
void propellerDisplay::updateStreamLineView(const propellerVLM *ned){
    pointsS->Reset();
    linesS->Reset();
    actorS->VisibilityOn();

    // 用于记录所有点的全局索引
    vtkIdType globalPointIndex = 0;

    // 遍历每条线段
    for (int i = 0; i < ned->streamLine.length(); i++) {
        const QVector<point3d>& linePoints = ned->streamLine[i];

        // 用于记录当前线段的局部点索引
        vtkSmartPointer<vtkIdList> pointIds = vtkSmartPointer<vtkIdList>::New();

        // 遍历线段中的每个点并插入到 pointsS 中
        for (int j = 0; j < linePoints.size(); j++) {
            pointsS->InsertNextPoint(linePoints[j].x, linePoints[j].y, linePoints[j].z);
            pointIds->InsertNextId(globalPointIndex++);

        }

        // 创建 vtkLine 并插入到 linesS 中
        for (int j = 0; j < pointIds->GetNumberOfIds() - 1; j++) {
            vtkSmartPointer<vtkLine> line = vtkSmartPointer<vtkLine>::New();
            line->GetPointIds()->SetId(0, pointIds->GetId(j));
            line->GetPointIds()->SetId(1, pointIds->GetId(j + 1));
            linesS->InsertNextCell(line);
        }
    }





    propDisplayWidgetE->renderWindow()->Render();
    //renwinB->Render();
}
*/

void propellerDisplay::updatePressureContourView(const propellerVLM *ned){


    //int index = wingAlphaCombox->currentIndex();
    int index = propStateCombox->currentIndex();;
    actorC->VisibilityOn();
    hidePropActor();
    //actorE->VisibilityOff();
    pointsC->Reset();
    linesC->Reset();
    colors->Reset();



    for (int i = 0; i < ned->meshNum; i++) {
        pointsC->InsertNextPoint(ned->xyA[i].x, ned->xyA[i].y, ned->xyA[i].z);
        pointsC->InsertNextPoint(ned->xyB[i].x, ned->xyB[i].y, ned->xyB[i].z);
        pointsC->InsertNextPoint(ned->xyC[i].x, ned->xyC[i].y, ned->xyC[i].z);
        pointsC->InsertNextPoint(ned->xyD[i].x, ned->xyD[i].y, ned->xyD[i].z);



    }

    for (vtkIdType i = 0; i < pointsC->GetNumberOfPoints() - 3; i += 4) {
        vtkSmartPointer<vtkQuad> quad = vtkSmartPointer<vtkQuad>::New();
        quad->GetPointIds()->SetId(0, i);
        quad->GetPointIds()->SetId(1, i + 1);
        quad->GetPointIds()->SetId(2, i + 2);
        quad->GetPointIds()->SetId(3, i + 3);
        linesC->InsertNextCell(quad);
    }

    // Set colors for each cell
    int blue[3] = {0, 0, 255};    // Blue
    int yellow[3] = {255, 255, 0}; // Yellow
    int red[3] = {255, 0, 0};     // Red

    // Ensure colors array is allocated and set correct number of components
    colors->SetNumberOfComponents(3);
    colors->SetNumberOfTuples(profilePolyDataC->GetNumberOfCells());

    for (vtkIdType i = 0; i < profilePolyDataC->GetNumberOfCells(); i++) {
        unsigned char rgb[3];
        double value = ned->pressureColorArray[index][i];


        // Clamp value to ensure it is within [0, 1]
        value = std::clamp(value, 0.0, 1.0);

        if (value <= 0.5) {
            // Interpolate between blue and yellow
            double ratio = value / 0.5;
            rgb[0] = static_cast<unsigned char>((1 - ratio) * blue[0] + ratio * yellow[0]);
            rgb[1] = static_cast<unsigned char>((1 - ratio) * blue[1] + ratio * yellow[1]);
            rgb[2] = static_cast<unsigned char>((1 - ratio) * blue[2] + ratio * yellow[2]);
        } else {
            // Interpolate between yellow and red
            double ratio = (value - 0.5) / 0.5;
            rgb[0] = static_cast<unsigned char>((1 - ratio) * yellow[0] + ratio * red[0]);
            rgb[1] = static_cast<unsigned char>((1 - ratio) * yellow[1] + ratio * red[1]);
            rgb[2] = static_cast<unsigned char>((1 - ratio) * yellow[2] + ratio * red[2]);
        }

        colors->InsertTypedTuple(i, rgb);
    }

    // 移除旧的演员
    vtkSmartPointer<vtkActorCollection> actors = rendererE->GetActors();
    actors->InitTraversal();
    vtkActor* actor = nullptr;
    while ((actor = actors->GetNextActor())) {
        if (!actor->GetPickable()) {
            rendererE->RemoveActor(actor);

        }
        //rendererE->RemoveActor(actor);
    }

    // 创建新的映射器和演员
    vtkSmartPointer<vtkPolyDataMapper> mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
    mapper->SetInputData(profilePolyDataC);

    vtkSmartPointer<vtkActor> newActor = vtkSmartPointer<vtkActor>::New();
    newActor->SetMapper(mapper);
    rendererE->AddActor(newActor);


    vtkSmartPointer<vtkProperty> property = vtkSmartPointer<vtkProperty>::New();
    property->SetColor(1.0, 0.0, 0.0); // 可选：设置颜色或其他属性
    newActor->SetProperty(property);
    newActor->PickableOff();

    int n = ned->propNum;
    // 创建和添加旋转阵列
    for(int i = 1;i<n;i++){
        double rotated = double(i) / n * 360;
        CreateRotatedArray(rendererE,profilePolyDataC, rotated);
    }














    propDisplayWidgetE->renderWindow()->Render();


}

void propellerDisplay::updateStreamLineView(const propellerVLM *ned) {
    pointsS->Reset();
    linesS->Reset();
    actorS->VisibilityOn();
    int n = ned->propNum;
    vtkIdType globalPointIndex = 0;

    // 遍历每条线段
    for (int i = 0; i < ned->streamLine.length(); i++) {
        const QVector<point3d>& linePoints = ned->streamLine[i];

        // 用于记录当前线段的局部点索引
        vtkSmartPointer<vtkIdList> pointIds = vtkSmartPointer<vtkIdList>::New();

        // 遍历线段中的每个点并插入到 pointsS 中
        for (int j = 0; j < linePoints.size(); j++) {
            pointsS->InsertNextPoint(linePoints[j].x, linePoints[j].y, linePoints[j].z);
            pointIds->InsertNextId(globalPointIndex++);
        }

        // 创建原始线段
        for (int j = 0; j < pointIds->GetNumberOfIds() - 1; j++) {
            vtkSmartPointer<vtkLine> line = vtkSmartPointer<vtkLine>::New();
            line->GetPointIds()->SetId(0, pointIds->GetId(j));
            line->GetPointIds()->SetId(1, pointIds->GetId(j + 1));
            linesS->InsertNextCell(line);
        }

        // 旋转复制
        for (int k = 1; k <= n; k++) {
            double angle = k * (360.0 / n); // 计算每次旋转的角度

            vtkSmartPointer<vtkTransform> transform = vtkSmartPointer<vtkTransform>::New();
            transform->RotateZ(angle); // 绕Z轴旋转

            // 新的点索引列表
            vtkSmartPointer<vtkIdList> newPointIds = vtkSmartPointer<vtkIdList>::New();

            // 对每个点应用变换并插入新的点
            for (int j = 0; j < linePoints.size(); j++) {
                double point[3] = { linePoints[j].x, linePoints[j].y, linePoints[j].z };
                transform->TransformPoint(point, point); // 变换点
                pointsS->InsertNextPoint(point[0], point[1], point[2]);
                newPointIds->InsertNextId(globalPointIndex++);
            }

            // 创建复制的线段
            for (int j = 0; j < newPointIds->GetNumberOfIds() - 1; j++) {
                vtkSmartPointer<vtkLine> line = vtkSmartPointer<vtkLine>::New();
                line->GetPointIds()->SetId(0, newPointIds->GetId(j));
                line->GetPointIds()->SetId(1, newPointIds->GetId(j + 1));
                linesS->InsertNextCell(line);
            }
        }
    }

    propDisplayWidgetE->renderWindow()->Render();
}

void propellerDisplay::showPropTwistDialog(){

    chartBDialog->show();
}
void propellerDisplay::showPropChordDialog(){

    chartADialog->show();
}
void propellerDisplay::showChartCoordA(const QPointF& point,bool state){
    if(state){
        QString xText = axisXName[chartTypeIndexArray[0]] + ": " + QString::number(point.x(),'f',2) + "\n";
        QString yText = axisYName[chartTypeIndexArray[0]] + ": " + QString::number(point.y(),'f',2);
        toolTipA->setText(xText + yText);
        toolTipA->setAnchor(point);
        toolTipA->setZValue(11);
        toolTipA->updateGeometry();
        toolTipA->show();

    }else{
        toolTipA->hide();
    }
}
void propellerDisplay::showChartCoordB(const QPointF& point,bool state){
    if(state){
        QString xText = axisXName[chartTypeIndexArray[1]] + ": " + QString::number(point.x(),'f',2) + "\n";
        QString yText = axisYName[chartTypeIndexArray[1]] + ": " + QString::number(point.y(),'f',2);
        toolTipB->setText(xText + yText);
        toolTipB->setAnchor(point);
        toolTipB->setZValue(11);
        toolTipB->updateGeometry();
        toolTipB->show();

    }else{
        toolTipB->hide();
    }
}
void propellerDisplay::showChartCoordC(const QPointF& point,bool state){
    if(state){
        QString xText = axisXName[chartTypeIndexArray[2]] + ": " + QString::number(point.x(),'f',2) + "\n";
        QString yText = axisYName[chartTypeIndexArray[2]] + ": " + QString::number(point.y(),'f',2);
        toolTipC->setText(xText + yText);
        toolTipC->setAnchor(point);
        toolTipC->setZValue(11);
        toolTipC->updateGeometry();
        toolTipC->show();

    }else{
        toolTipC->hide();
    }
}
void propellerDisplay::showChartCoordD(const QPointF& point,bool state){
    if(state){
        QString xText = axisXName[chartTypeIndexArray[3]] + ": " + QString::number(point.x(),'f',2) + "\n";
        QString yText = axisYName[chartTypeIndexArray[3]] + ": " + QString::number(point.y(),'f',2);
        toolTipD->setText(xText + yText);
        toolTipD->setAnchor(point);
        toolTipD->setZValue(11);
        toolTipD->updateGeometry();
        toolTipD->show();

    }else{
        toolTipD->hide();
    }
}
void propellerDisplay::showChartAMenu(const QPoint &pos){

    chartIndex = 0;
    for (QAction *act : rChartMenu->actions()) {
        act->setChecked(false);
    }
    actionArray[chartTypeIndexArray[0]]->setChecked(true);



    rChartMenu->exec(QCursor::pos());


}
void propellerDisplay::showChartBMenu(const QPoint &pos){

    chartIndex = 1;
    for (QAction *act : rChartMenu->actions()) {
        act->setChecked(false);
    }
    actionArray[chartTypeIndexArray[1]]->setChecked(true);

    rChartMenu->exec(QCursor::pos());


}
void propellerDisplay::showChartCMenu(const QPoint &pos){

    chartIndex = 2;
    for (QAction *act : rChartMenu->actions()) {
        act->setChecked(false);
    }
    actionArray[chartTypeIndexArray[2]]->setChecked(true);

    rChartMenu->exec(QCursor::pos());


}
void propellerDisplay::showChartDMenu(const QPoint &pos){

    chartIndex = 3;
    for (QAction *act : rChartMenu->actions()) {
        act->setChecked(false);
    }
    actionArray[chartTypeIndexArray[3]]->setChecked(true);

    rChartMenu->exec(QCursor::pos());

}
void propellerDisplay::showChartMenu(const QPoint &pos){


    for (QAction *act : propChartMenu->actions()) {
        act->setChecked(false);
    }
    propActionArray[propChartTypeIndex]->setChecked(true);

    propChartMenu->exec(QCursor::pos());

}

void propellerDisplay::addItemsToResultB() {

    QTreeWidgetItem *fItem = propTreeItemArray[propChoiceIndex];

    // 查找特定子项
    QTreeWidgetItem *targetSubItem = nullptr;
    for (int i = 0; i < fItem->childCount(); ++i) {
        if (fItem->child(i)->text(0) == "结果") {  // 替换为你的查找条件
            targetSubItem = fItem->child(i);
            break;
        }
    }

    if (targetSubItem) {
        // 在找到的子项下添加 fItemResultB

        QStringList checkboxItems;
        const QVector<double>a = VLMSettingArray[propChoiceIndex].vinfArray;
        const QVector<double>b = VLMSettingArray[propChoiceIndex].rpmArray;
        for(int i = 0;i<a.length();i++){

            for(int j = 0;j<b.length();j++){
                QString txt = "v = " + QString::number(a[i]) + ",rpm = " + QString::number(b[j]);
                checkboxItems.append(txt);
            }
        }
        addCheckboxItems(targetSubItem, checkboxItems);
     }

}
void propellerDisplay::addCheckboxItems(QTreeWidgetItem *parentItem, const QStringList &itemNames) {

    for(int i = 0;i<itemNames.length();i++){
        QTreeWidgetItem *checkItem = new QTreeWidgetItem(parentItem);
        checkItem->setText(0, itemNames[i]);
        checkItem->setData(0, Qt::UserRole, QVariant("childResult" + QString::number(i)));
        checkItem->setCheckState(0, Qt::Unchecked);  // 设置复选框为未选中状态
        checkItem->setBackground(0, QBrush(QColor(0, 255, 0)));
        parentItem->addChild(checkItem);
    }
}
void propellerDisplay::removeItemsToResultB() {

    QTreeWidgetItem *fItem = propTreeItemArray[propChoiceIndex];

    // 查找特定子项
    QTreeWidgetItem *targetSubItem = nullptr;
    for (int i = 0; i < fItem->childCount(); ++i) {
        if (fItem->child(i)->text(0) == "结果") {  // 替换为你的查找条件
            targetSubItem = fItem->child(i);
            break;
        }
    }

    if (targetSubItem) {
        // 在找到的子项下添加 fItemResultB
        QStringList checkboxItems;
        const QVector<double>a = VLMSettingArray[propChoiceIndex].vinfArray;
        const QVector<double>b = VLMSettingArray[propChoiceIndex].rpmArray;
        for(int i = 0;i<a.length();i++){

            for(int j = 0;j<b.length();j++){
                QString txt = "v = " + QString::number(a[i]) + ",rpm = " + QString::number(b[j]);
                checkboxItems.append(txt);
            }
        }



        removeCheckboxItems(targetSubItem, checkboxItems);
     }

}
void propellerDisplay::removeCheckboxItems(QTreeWidgetItem *parentItem, const QStringList &itemNames) {
    for (int i = parentItem->childCount() - 1; i >= 0; --i) {
        QTreeWidgetItem *childItem = parentItem->child(i);
        if (itemNames.contains(childItem->text(0))) {
            delete parentItem->takeChild(i);  // 删除匹配的子项
        }
    }
}

void propellerDisplay::addDataPoint(){
    switch (chartIndex) {
    case 0:
        addDataPointA();
        break;
    case 1:
        addDataPointB();
        break;
    case 2:
        addDataPointC();
        break;
    case 3:
        addDataPointD();
        break;
    default:
        break;
    }
}
void propellerDisplay::clearDataPoint(){
    switch (chartIndex) {
    case 0:
        clearDataPointA();
        break;
    case 1:
        clearDataPointB();
        break;
    case 2:
        clearDataPointC();
        break;
    case 3:
        clearDataPointD();
        break;
    default:
        break;
    }

}
void propellerDisplay::changeResultType(){
    QAction *action = qobject_cast<QAction*>(sender());
    int index = action->property("actions").toInt();
    if (action) {
        // 取消所有其他动作的选中状态
        for (QAction *act : rChartMenu->actions()) {
            act->setChecked(false);
        }
    }

        // 设置当前动作为选中状态
        action->setChecked(true);


    switch (chartIndex) {
    case 0:

        if(index == 8){
            exportChartData(resultChartA);
            QMessageBox::information(this,"信息提示","保存成功");
        }else{
            clearDataPointA();
            xAxisIndexA = axisIndex[index][0], yAxisIndexA = axisIndex[index][1];
            resultAxisXA->setRange(0,axisRange[xAxisIndexA]);
            resultAxisYA->setRange(0,axisRange[yAxisIndexA]);
            resultAxisXA->setTitleText(axisXName[index]);
            resultAxisYA->setTitleText(axisYName[index]);
            resultChartA->setTitle(titleName[index]);
            chartTypeIndexArray[0] = index;
            drawResultChartA();


        }

        break;
    case 1:
        if(index == 8){
            exportChartData(resultChartB);
            QMessageBox::information(this,"信息提示","保存成功");
        }
        else{
            clearDataPointB();
            xAxisIndexB = axisIndex[index][0], yAxisIndexB = axisIndex[index][1];
            resultAxisXB->setRange(0,axisRange[xAxisIndexB]);
            resultAxisYB->setRange(0,axisRange[yAxisIndexB]);
            resultAxisXB->setTitleText(axisXName[index]);
            resultAxisYB->setTitleText(axisYName[index]);
            resultChartB->setTitle(titleName[index]);
            chartTypeIndexArray[1] = index;
            drawResultChartB();

        }


        break;
    case 2:
        if(index == 8){
            exportChartData(resultChartC);
            QMessageBox::information(this,"信息提示","保存成功");
        }else{
            clearDataPointC();
            xAxisIndexC = axisIndex[index][0], yAxisIndexC = axisIndex[index][1];
            resultAxisXC->setRange(0,axisRange[xAxisIndexC]);
            resultAxisYC->setRange(0,axisRange[yAxisIndexC]);
            resultAxisXC->setTitleText(axisXName[index]);
            resultAxisYC->setTitleText(axisYName[index]);
            resultChartC->setTitle(titleName[index]);
            chartTypeIndexArray[2] = index;
            drawResultChartC();

        }

        break;
    case 3:
        if(index == 8){
            exportChartData(resultChartD);
            QMessageBox::information(this,"信息提示","保存成功");
        }else{
            clearDataPointD();
            xAxisIndexD = axisIndex[index][0], yAxisIndexD = axisIndex[index][1];
            resultAxisXD->setRange(0,axisRange[xAxisIndexD]);
            resultAxisYD->setRange(0,axisRange[yAxisIndexD]);
            resultAxisXD->setTitleText(axisXName[index]);
            resultAxisYD->setTitleText(axisYName[index]);
            resultChartD->setTitle(titleName[index]);
            chartTypeIndexArray[3] = index;
            drawResultChartD();

        }

        break;
    default:
        break;
    }


}
void propellerDisplay::readPropData(){
    //QString propName = propLibary->propNameA[propLibary->choiceIndex];
    QString propName = propLibary->choicePropName;
    QString name = QDir::currentPath() + "/libaries/propeller/apc/data/PERFILES2/" + propName + ".dat";
    QFile file(name);

    if(file.open(QIODevice::ReadOnly|QIODevice::Text)){

        QVector<propRPMData>propData;
        QTextStream stream(&file);
        //去掉数据文件介绍
        for(int i = 0;i<19;i++){
            stream.readLine();
        }
        QStringList values ;


        while(!stream.atEnd()){
            propRPMData tmp;
            values = stream.readLine().split("    ",Qt::SkipEmptyParts);
            tmp.RPM = values.at(1).toInt();
            stream.readLine();
            stream.readLine();
            stream.readLine();
            values = stream.readLine().split("   ",Qt::SkipEmptyParts);
            while(values.size() != 0){
                QVector<double>tmpArray;
                if(values.size() == 15){
                    for(int i = 0;i<values.size();i++)
                        tmpArray.append(values.at(i).toDouble());
                    tmp.propData.append(tmpArray);
                }
                values = stream.readLine().split("   ",Qt::SkipEmptyParts);                
            }
            for(int i = 0;i<tmp.propData.length();i++)
                tmp.propData[i][0] *= 0.44704;
            stream.readLine();
            stream.readLine();
            propData.append(tmp);

        }

        propDataArray.append(propData);
        nameArray.append(propName);
    }
}
void propellerDisplay::addPropData(){
    propLibary->propLibaryWidget->hide();
    readPropData();
    rpmArray.append(3000);

    QPushButton *button1 = new QPushButton(propResultDisplayWidget);
    hoverButton *button2 = new hoverButton(propResultDisplayWidget);
    hoverButton *button3 = new hoverButton(propResultDisplayWidget);
    QLineEdit *lineEdit = new QLineEdit(propResultDisplayWidget);
    QLabel *label1 = new QLabel(propResultDisplayWidget);
    QLabel *label2 = new QLabel(propResultDisplayWidget);
    QLabel *label3 = new QLabel(propResultDisplayWidget);
    QLabel *label4 = new QLabel(propResultDisplayWidget);
    QLabel *label5 = new QLabel(propResultDisplayWidget);
    QLabel *label6 = new QLabel(propResultDisplayWidget);
    QFrame *line = new QFrame();

    QSpinBox *spinbox = new QSpinBox(propResultDisplayWidget);




    int r = rand() % 255;
    int b = rand() % 255;
    int g = rand() % 255;
    QColor color(r, b, g);

    propColorArray.append(color);
    QString colorStyle = QString("background-color: %1;").arg(color.name());
    label1->setText("转速");
    label2->setText("速度(m/s):");
    label3->setText("推力：");
    label4->setText("功率：");
    label5->setText("拉力系数：");
    label6->setText("效率：");

    button1->setProperty("prop", propIndex);
    button2->setProperty("color", propIndex);
    button3->setProperty("remove", propIndex);
    spinbox->setProperty("rpm", propIndex);
    lineEdit->setProperty("velocity", propIndex);
    line->setFrameShape(QFrame::HLine); // 设置为水平分割线
    line->setFrameShadow(QFrame::Sunken); // 设置阴影样式

    spinbox->setMinimum(1);
    spinbox->setMaximum(99999);
    spinbox->setValue(3000);

    button1->setFixedHeight(40);
    button1->setText(nameArray[propIndex]);
    button2->setFixedHeight(40);
    button3->setButtonIcon(removeIcon, QSize(40, 40));
    spinbox->setMinimumWidth(80);

    button1->setObjectName("childButton");
    button2->setObjectName("colorButton");
    button3->setObjectName("iconButton");

    button2->setStyleSheet(colorStyle);
    spinbox->setMinimumHeight(30);
    lineEdit->setText("5");

    buttonArray.append(button1);
    colorButtonArray.append(button2);
    removeButtonArray.append(button3);
    labelArrayA.append(label1);
    labelArrayB.append(label2);
    labelArrayC.append(label3);
    labelArrayD.append(label4);
    labelArrayE.append(label5);
    labelArrayF.append(label6);
    lineEditArray.append(lineEdit);
    spinBoxArray.append(spinbox);
    lineArray.append(line);

    if (propIndex > 0) {
        propListGLayout->removeItem(itemSpacer);
    }

    int baseRow = 6 * propIndex;
    propListGLayout->addWidget(button1, baseRow + 1, 0, 1, 2);
    propListGLayout->addWidget(button2, baseRow + 1, 2, 1, 1);
    propListGLayout->addWidget(button3, baseRow + 1, 3, 1, 1);
    propListGLayout->addWidget(label1, baseRow + 2, 0, 1, 1);
    propListGLayout->addWidget(spinbox, baseRow + 2, 1, 1, 1);
    propListGLayout->addWidget(label2, baseRow + 2, 2, 1, 1);
    propListGLayout->addWidget(lineEdit, baseRow + 2, 3, 1, 1);
    propListGLayout->addWidget(label3, baseRow + 3, 0, 1, 2);
    propListGLayout->addWidget(label4, baseRow + 3, 2, 1, 2);
    propListGLayout->addWidget(label5, baseRow + 4, 0, 1, 2);
    propListGLayout->addWidget(label6, baseRow + 4, 2, 1, 2);
    propListGLayout->addWidget(line, baseRow + 5, 0, 1, 4);

    // 重新添加 QSpacerItem
    propListGLayout->addItem(itemSpacer, baseRow + 6, 0, 1, 4);



    connect(button1,&QPushButton::clicked,this,&propellerDisplay::choicePropButton);
    connect(button2,&QPushButton::clicked,this,&propellerDisplay::showColorDialog);
    connect(button3,&QPushButton::clicked,this,&propellerDisplay::deleteProp);
    connect(spinbox,QOverload<int>::of(&QSpinBox::valueChanged),this,&propellerDisplay::changeRpm);
    connect(lineEdit,&QLineEdit::textChanged,this,&propellerDisplay::drawResultChartA);
    connect(lineEdit,&QLineEdit::textChanged,this,&propellerDisplay::drawResultChartB);
    connect(lineEdit,&QLineEdit::textChanged,this,&propellerDisplay::drawResultChartC);
    connect(lineEdit,&QLineEdit::textChanged,this,&propellerDisplay::drawResultChartD);




    QLineSeries *series1 = new QLineSeries;
    QLineSeries *series2 = new QLineSeries;
    QLineSeries *series3 = new QLineSeries;
    QLineSeries *series4 = new QLineSeries;
    resultChartA->addSeries(series1);
    resultChartB->addSeries(series2);
    resultChartC->addSeries(series3);
    resultChartD->addSeries(series4);
    series1->attachAxis(resultAxisXA);
    series1->attachAxis(resultAxisYA);
    series2->attachAxis(resultAxisXB);
    series2->attachAxis(resultAxisYB);
    series3->attachAxis(resultAxisXC);
    series3->attachAxis(resultAxisYC);
    series4->attachAxis(resultAxisXD);
    series4->attachAxis(resultAxisYD);
    seriesA.append(series1);
    seriesB.append(series2);
    seriesC.append(series3);
    seriesD.append(series4);




    choiceIndex = propIndex;

    connect(series1,&QLineSeries::hovered,this,&propellerDisplay::showChartCoordA);
    connect(series2,&QLineSeries::hovered,this,&propellerDisplay::showChartCoordB);
    connect(series3,&QLineSeries::hovered,this,&propellerDisplay::showChartCoordC);
    connect(series4,&QLineSeries::hovered,this,&propellerDisplay::showChartCoordD);
    drawResultChartA();
    drawResultChartB();
    drawResultChartC();
    drawResultChartD();


    propIndex++;
}
void propellerDisplay::deleteProp(){
    QPushButton *button = static_cast<QPushButton*>(sender());
    int index = button->property("remove").toInt();
    button = nullptr;
    removeProp(index);
}
void propellerDisplay::removeProp(const int index){
    delete seriesA[index];
    delete seriesB[index];
    delete seriesC[index];
    delete seriesD[index];
    seriesA.remove(index);
    seriesB.remove(index);
    seriesC.remove(index);
    seriesD.remove(index);

    propDataArray.remove(index);
    propColorArray.remove(index);
    nameArray.remove(index);
    rpmArray.remove(index);

    if (index < buttonArray.length()) {
        QPushButton* buttonToRemove = buttonArray.at(index);
        propListGLayout->removeWidget(buttonToRemove);
        buttonToRemove->deleteLater();
        buttonArray.removeAt(index);
    }

    if (index < colorButtonArray.length()) {
        QPushButton* buttonToRemove = colorButtonArray.at(index);
        propListGLayout->removeWidget(buttonToRemove);
        buttonToRemove->deleteLater();
        colorButtonArray.removeAt(index);
    }

    if (index < removeButtonArray.length()) {
        QPushButton* buttonToRemove = removeButtonArray.at(index);
        propListGLayout->removeWidget(buttonToRemove);
        buttonToRemove->deleteLater();
        removeButtonArray.removeAt(index);
    }

    if (index < labelArrayA.length()) {
        QLabel* spinboxToRemove = labelArrayA.at(index);
        propListGLayout->removeWidget(spinboxToRemove);
        spinboxToRemove->deleteLater();
        labelArrayA.removeAt(index);
    }
    if (index < spinBoxArray.length()) {
        QSpinBox* spinboxToRemove = spinBoxArray.at(index);
        propListGLayout->removeWidget(spinboxToRemove);
        spinboxToRemove->deleteLater();
        spinBoxArray.removeAt(index);
    }

    if (index < labelArrayB.length()) {
        QLabel* spinboxToRemove = labelArrayB.at(index);
        propListGLayout->removeWidget(spinboxToRemove);
        spinboxToRemove->deleteLater();
        labelArrayB.removeAt(index);
    }
    if (index < lineEditArray.length()) {
        QLineEdit* spinboxToRemove = lineEditArray.at(index);
        propListGLayout->removeWidget(spinboxToRemove);
        spinboxToRemove->deleteLater();
        lineEditArray.removeAt(index);
    }

    if (index < labelArrayC.length()) {
        QLabel* spinboxToRemove = labelArrayC.at(index);
        propListGLayout->removeWidget(spinboxToRemove);
        spinboxToRemove->deleteLater();
        labelArrayC.removeAt(index);
    }


    if (index < labelArrayD.length()) {
        QLabel* spinboxToRemove = labelArrayD.at(index);
        propListGLayout->removeWidget(spinboxToRemove);
        spinboxToRemove->deleteLater();
        labelArrayD.removeAt(index);
    }

    if (index < labelArrayE.length()) {
        QLabel* spinboxToRemove = labelArrayE.at(index);
        propListGLayout->removeWidget(spinboxToRemove);
        spinboxToRemove->deleteLater();
        labelArrayE.removeAt(index);
    }
    if (index < labelArrayF.length()) {
        QLabel* spinboxToRemove = labelArrayF.at(index);
        propListGLayout->removeWidget(spinboxToRemove);
        spinboxToRemove->deleteLater();
        labelArrayF.removeAt(index);
    }
    if (index < lineArray.length()) {
        QFrame* spinboxToRemove = lineArray.at(index);
        propListGLayout->removeWidget(spinboxToRemove);
        spinboxToRemove->deleteLater();
        lineArray.removeAt(index);
    }

    // if (index < vSpacerArray.length()) {
    //     QSpacerItem* laeblToRemove = vSpacerArray.at(index);
    //     propListGLayout->removeItem(laeblToRemove);
    //     delete laeblToRemove;
    //     vSpacerArray.removeAt(index);
    // }

    for (int i = 0; i < buttonArray.size(); ++i) {
        propListGLayout->addWidget(buttonArray.at(i), 5 * i + 1, 0,1,2); // 第一列
        propListGLayout->addWidget(colorButtonArray.at(i), 5 * i + 1, 2,1,1); // 第二列
        propListGLayout->addWidget(removeButtonArray.at(i), 5 * i + 1, 3,1,1); // 第二列
        propListGLayout->addWidget(labelArrayA.at(i), 5 * i + 2, 0,1,1); // 第二列
        propListGLayout->addWidget(spinBoxArray.at(i), 5 * i + 2, 1,1,1); // 第二列
        propListGLayout->addWidget(labelArrayB.at(i), 5 * i + 2, 2,1,1); // 第二列
        propListGLayout->addWidget(lineEditArray.at(i), 5 * i + 2, 3,1,1); // 第二列
        propListGLayout->addWidget(labelArrayC.at(i), 5 * i + 3, 0,1,2); // 第二列
        propListGLayout->addWidget(labelArrayD.at(i), 5 * i + 3, 2,1,2); // 第二列
        propListGLayout->addWidget(labelArrayE.at(i), 5 * i + 4, 0,1,2); // 第二列
        propListGLayout->addWidget(labelArrayF.at(i), 5 * i + 4, 2,1,2); // 第二列
        propListGLayout->addWidget(lineArray.at(i), 5 * i + 5, 0,1,4); // 第二列
        //propListGLayout->addItem(vSpacerArray.at(i), 5 * i + 4, 0,1,4); // 第二列
        buttonArray.at(i)->setProperty("prop",i);
        colorButtonArray.at(i)->setProperty("color",i);
        removeButtonArray.at(i)->setProperty("remove",i);
        spinBoxArray.at(i)->setProperty("rpm",i);
        lineEditArray.at(i)->setProperty("velocity",i);
    }

    propIndex--;






}
void propellerDisplay::showColorDialog(){
    QPushButton *button = qobject_cast<QPushButton *>(sender());
    int index = button->property("color").toInt();
    choiceIndex = index;
    if (button) {
        connect(colorDialog,&QColorDialog::colorSelected,this,&propellerDisplay::changePropColor);
        colorDialog->show();
    }
}
void propellerDisplay::changePropColor(const QColor& color){
    propColorArray[choiceIndex] = color;
    QString colorStyle = QString("background-color: %1;").arg(color.name());
    colorButtonArray[choiceIndex]->setStyleSheet(colorStyle);
    drawResultChartA();
    drawResultChartB();
    drawResultChartC();
    drawResultChartD();

}
double propellerDisplay::getFixVelocityTmp(const double v,const QVector<double>&x,const QVector<double>&data){
    double tmp = 0;
    double minv = functionMath.minV(x);
    double maxv = functionMath.maxV(x);
    if(v>=minv&&v<=maxv){

        tmp = myMath::linearInterpolation(x,data,v);
        return tmp;
    }
    else{
        return 0;
    }


}
QVector<QVector<double>> propellerDisplay::getInterpResult(const int index ,const int rpm,const int index1,const int index2){
    QVector<QVector<double>>speeds;
    QVector<QVector<double>>thrusts;
    QVector<double>xCod;
    QVector<double>allRpm;
    for(int i = 0;i<propDataArray[index].length();i++){
        allRpm.append(propDataArray[index][i].RPM);
    }
    if(rpm > functionMath.maxV(allRpm)){
        //rpm = functionMath.maxV(allRpm);
        rpmArray[choiceIndex] = functionMath.maxV(allRpm);
    }


    int tmpIndex = functionMath.minIndex(allRpm,rpmArray[choiceIndex]);
    if(tmpIndex > allRpm.length() - 2)
        tmpIndex = allRpm.length() - 2;
    if(tmpIndex < 1)
        tmpIndex = 1;

    QVector<double>lenV;
    for(int i = tmpIndex - 1;i<tmpIndex + 2;i++){
        lenV.append(propDataArray[index][i].propData.length());
    }
    int length = functionMath.minV(lenV);





    for(int i = tmpIndex - 1;i<tmpIndex + 2;i++){
        QVector<double>speedTmp;
        QVector<double>thrustTmp;
        for(int j = 0;j<length;j++){
            speedTmp.append(propDataArray[index][i].propData[j][index1]);
            thrustTmp.append(propDataArray[index][i].propData[j][index2]);
        }
        speeds.append(speedTmp);
        thrusts.append(thrustTmp);
    }




    QVector<double>rpms{allRpm[tmpIndex - 1],allRpm[tmpIndex],allRpm[tmpIndex + 1]};

    double drpm = rpm;

    double k = 1 - (drpm - rpms[0]) / 2000;
    for(int i = 0;i<length;i++){
        double yn1 = speeds[0][1] * i;
        double yn2 = speeds[2][1] * i;
        xCod.append(yn1 * k + yn2 * (1 - k));
    }


    QVector<QVector<double>>result;
    result.append(xCod);
    //result.append(speeds[1]);
    result.append(functionMath.getInterpolateThrust(thrusts,rpms,drpm));

    return result;
}
void propellerDisplay::choicePropButton(){
    QPushButton *button = qobject_cast<QPushButton*>(sender());
    int index = button->property("prop").toInt();
    choiceIndex = index;
    setPorpButtonEnabled(index);
}
void propellerDisplay::setPorpButtonEnabled(const int index){
    for(int i = 0;i < buttonArray.length();i++){
        if(i != index){
            buttonArray[i]->setEnabled(true);
            removeButtonArray[i]->setEnabled(false);
        }
    }

    buttonArray[index]->setEnabled(false);
    removeButtonArray[index]->setEnabled(true);

}
QVector<QVector<double>> propellerDisplay::getResult(const int index ,const int rpm,const int index1,const int index2){
    QVector<QVector<double>>tmp;
    if(index2 != 2)
         tmp = getInterpResult(index,rpm,index1,index2);
    else{
        QVector<QVector<double>>tmp1 = getInterpResult(index,rpm,0,10);
        QVector<QVector<double>>tmp2 = getInterpResult(index,rpm,0,8);
        QVector<double>tmpA;
        QVector<double>tmpB;



        for(int i = 0;i<tmp1[0].length();i++){
            tmp[1][i] = tmp1[0][i] * tmp1[1][i] / tmp2[1][i];
        }
    }
    return tmp;
}
void propellerDisplay::drawResultChartA(){
    if(!propDataArray.isEmpty()){
        QVector<QVector<double>>result = getInterpResult(choiceIndex,rpmArray[choiceIndex],xAxisIndexA,yAxisIndexA);
        //QVector<QVector<double>>result = getResult(choiceIndex,rpmArray[choiceIndex],xAxisIndexA,yAxisIndexA);

        if(seriesA[choiceIndex]->count() > 0)
            seriesA[choiceIndex]->clear();
        for(int i = 0;i<result[0].length();i++){
            seriesA[choiceIndex]->append(result[0][i],result[1][i]);
        }
        QPen pen;
        pen.setWidth(2);
        pen.setColor(propColorArray[choiceIndex]);
        seriesA[choiceIndex]->setPen(pen);


        if(functionMath.maxV(result[0]) > resultAxisXA->max() || functionMath.maxV(result[0]) < resultAxisXA->max() / 2)
            resultAxisXA->setRange(0,functionMath.maxV(result[0]) * 1.2);
        if(functionMath.maxV(result[1]) > resultAxisYA->max() || functionMath.maxV(result[1]) < resultAxisYA->max() / 3)
            resultAxisYA->setRange(0,functionMath.maxV(result[1]) * 1.2);

        //updateAxes(resultChartA,seriesA);
        bool ok;
        double value = lineEditArray[choiceIndex]->text().toDouble(&ok);
        if(ok){
            double tmp = getFixVelocityTmp(value,result[0],result[1]);
            labelArrayC[choiceIndex]->setText(axisYName[chartTypeIndexArray[0]] + ":  " + QString::number(tmp,'f',3));
        }



    }
}
void propellerDisplay::drawResultChartB(){
    if(!propDataArray.isEmpty()){

        QVector<QVector<double>>result = getInterpResult(choiceIndex,rpmArray[choiceIndex],xAxisIndexB,yAxisIndexB);
        if(seriesB[choiceIndex]->count() > 0)
            seriesB[choiceIndex]->clear();
        for(int i = 0;i<result[0].length();i++){
            seriesB[choiceIndex]->append(result[0][i],result[1][i]);
        }

        QPen pen;
        pen.setWidth(2);
        pen.setColor(propColorArray[choiceIndex]);
        seriesB[choiceIndex]->setPen(pen);





        if(functionMath.maxV(result[0]) > resultAxisXB->max() || functionMath.maxV(result[0]) < resultAxisXB->max() / 2)
            resultAxisXB->setRange(0,functionMath.maxV(result[0]) * 1.2);
        if(functionMath.maxV(result[1]) > resultAxisYB->max() || functionMath.maxV(result[1]) < resultAxisYB->max() / 3)
            resultAxisYB->setRange(0,functionMath.maxV(result[1]) * 1.2);
        //updateAxes(resultChartB,seriesB);
        bool ok;
        double value = lineEditArray[choiceIndex]->text().toDouble(&ok);
        if(ok){
            double tmp = getFixVelocityTmp(value,result[0],result[1]);
            labelArrayD[choiceIndex]->setText(axisYName[chartTypeIndexArray[1]] + ":  " + QString::number(tmp,'f',3));
        }

    }
}
void propellerDisplay::drawResultChartC(){
    if(!propDataArray.isEmpty()){


        QVector<QVector<double>>result = getInterpResult(choiceIndex,rpmArray[choiceIndex],xAxisIndexC,yAxisIndexC);
        if(seriesC[choiceIndex]->count() > 0)
            seriesC[choiceIndex]->clear();
        for(int i = 0;i<result[0].length();i++){
            seriesC[choiceIndex]->append(result[0][i],result[1][i]);
        }
        QPen pen;
        pen.setWidth(2);
        pen.setColor(propColorArray[choiceIndex]);
        seriesC[choiceIndex]->setPen(pen);

        if(functionMath.maxV(result[0]) > resultAxisXC->max() || functionMath.maxV(result[0]) < resultAxisXC->max() / 2)
            resultAxisXC->setRange(0,functionMath.maxV(result[0]) * 1.2);
        if(functionMath.maxV(result[1]) > resultAxisYC->max() || functionMath.maxV(result[1]) < resultAxisYC->max() / 3)
            resultAxisYC->setRange(0,functionMath.maxV(result[1]) * 1.2);

       //updateAxes(resultChartC,seriesC);
        bool ok;
        double value = lineEditArray[choiceIndex]->text().toDouble(&ok);
        if(ok){
            double tmp = getFixVelocityTmp(value,result[0],result[1]);
            labelArrayE[choiceIndex]->setText(axisYName[chartTypeIndexArray[2]] + ":  " + QString::number(tmp,'f',3));
        }


    }
}

void propellerDisplay::drawResultChartD(){
    if(!propDataArray.isEmpty()){
        QVector<QVector<double>>result = getInterpResult(choiceIndex,rpmArray[choiceIndex],xAxisIndexD,yAxisIndexD);
        if(seriesD[choiceIndex]->count() > 0)
            seriesD[choiceIndex]->clear();
        for(int i = 0;i<result[0].length();i++){
            seriesD[choiceIndex]->append(result[0][i],result[1][i]);
        }
        QPen pen;
        pen.setWidth(2);
        pen.setColor(propColorArray[choiceIndex]);
        seriesD[choiceIndex]->setPen(pen);

        if(functionMath.maxV(result[0]) > resultAxisXD->max() || functionMath.maxV(result[0]) < resultAxisXD->max() / 2)
            resultAxisXD->setRange(0,functionMath.maxV(result[0]) * 1.2);
        if(functionMath.maxV(result[1]) > resultAxisYD->max() || functionMath.maxV(result[1]) < resultAxisYD->max() / 3)
            resultAxisYD->setRange(0,functionMath.maxV(result[1]) * 1.2);

        //updateAxes(resultChartD,seriesD);
        bool ok;
        double value = lineEditArray[choiceIndex]->text().toDouble(&ok);
        if(ok){
            double tmp = getFixVelocityTmp(value,result[0],result[1]);
            labelArrayF[choiceIndex]->setText(axisYName[chartTypeIndexArray[3]] + ":  " + QString::number(tmp,'f',3));
        }


    }
}
void propellerDisplay::exportChartData(QChart *chart) {
    QString fileName = QFileDialog::getSaveFileName(nullptr, "Save Data", "", "Text Files (*.txt)");
    if (!fileName.isEmpty()) {
        QFile file(fileName);
        if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QTextStream stream(&file);

            // 获取坐标轴
            auto axisX = chart->axes(Qt::Horizontal).at(0);
            auto axisY = chart->axes(Qt::Vertical).at(0);

            // 获取坐标轴的标签
            QString xLabel = axisX->titleText();
            QString yLabel = axisY->titleText();

            // 遍历图表中的所有系列
            const auto seriesList = chart->series();
            for (const QAbstractSeries *series : seriesList) {
                const QLineSeries *lineSeries = qobject_cast<const QLineSeries *>(series);
                if (lineSeries) {
                    // 写入系列名称作为标头
                    stream << lineSeries->name() << "\n";
                    // 写入表头（使用坐标轴标签）
                    stream << xLabel << "\t" << yLabel << "\n";
                    // 写入数据点
                    for (const QPointF &point : lineSeries->points()) {
                        stream << point.x() << "\t" << point.y() << "\n";
                    }
                    // 不同系列之间用空行分隔
                    stream << "\n";
                }
            }
            file.close();

        } else {

        }
    }
    QMessageBox::information(nullptr,"提示","保存成功");
}

void propellerDisplay::showPropLibary(){
    propLibary->propLibaryWidget->show();
}
void propellerDisplay::updateMessage(const QString text){
    propDesignMessageTextEdit->append(text);
}
void propellerDisplay::updateMousePositionRA(const QPointF &localPoint){

    if(isMousePressed&&!propDataArray.isEmpty()) {


        double realy = localPoint.y();
        if(realy - lastYA >= 0){
            rpmArray[choiceIndex] +=  50;
        }else{
            rpmArray[choiceIndex] -=  50;
        }
        if(rpmArray[choiceIndex] > 50000)
            rpmArray[choiceIndex] = 3000;


        spinBoxArray[choiceIndex]->blockSignals(true);
        spinBoxArray[choiceIndex]->setValue(rpmArray[choiceIndex]);
        spinBoxArray[choiceIndex]->blockSignals(false);

        lastYA = realy;
        drawResultChartA();
        if(bothChart){
            drawResultChartB();
            drawResultChartC();
            drawResultChartD();
        }
    }

    // chartCoordXA->setText(QString("X: %1").arg(localPoint.x()));

    // chartCoordYA->setText(QString("Y: %1").arg(localPoint.y()));

}
void propellerDisplay::updateMousePositionRB(const QPointF &localPoint){

    if(isMousePressed&&!propDataArray.isEmpty()) {


        double realy = localPoint.y();
        if(realy - lastYB >= 0){
            rpmArray[choiceIndex] +=  50;
        }else{
            rpmArray[choiceIndex] -=  50;
        }
        if(rpmArray[choiceIndex] > 50000)
            rpmArray[choiceIndex] = 3000;

        spinBoxArray[choiceIndex]->blockSignals(true);
        spinBoxArray[choiceIndex]->setValue(rpmArray[choiceIndex]);
        spinBoxArray[choiceIndex]->blockSignals(false);
        lastYB = realy;
        drawResultChartB();
        if(bothChart){

            drawResultChartA();
            drawResultChartC();
            drawResultChartD();
        }
    }

    // chartCoordXB->setText(QString("X: %1").arg(localPoint.x()));

    // chartCoordYB->setText(QString("Y: %1").arg(localPoint.y()));
}
void propellerDisplay::updateMousePositionRC(const QPointF &localPoint){

    if(isMousePressed&&!propDataArray.isEmpty()) {

        //QPoint viewPoint = resultChartViewC->mapFromScene(localPoint);
        //double dy = viewPoint.y();
        double realy = localPoint.y();
        if(realy - lastYC >= 0){
            rpmArray[choiceIndex] +=  50;
        }else{
            rpmArray[choiceIndex] -=  50;
        }
        if(rpmArray[choiceIndex] > 50000)
            rpmArray[choiceIndex] = 3000;

        spinBoxArray[choiceIndex]->blockSignals(true);
        spinBoxArray[choiceIndex]->setValue(rpmArray[choiceIndex]);
        spinBoxArray[choiceIndex]->blockSignals(false);
        lastYC = realy;
        drawResultChartC();
        if(bothChart){

            drawResultChartA();
            drawResultChartB();
            drawResultChartD();
        }
    }

    // chartCoordXC->setText(QString("X: %1").arg(localPoint.x()));

    // chartCoordYC->setText(QString("Y: %1").arg(localPoint.y()));
}
void propellerDisplay::updateMousePositionRD(const QPointF &localPoint){

    if(isMousePressed&&!propDataArray.isEmpty()) {

        //QPoint viewPoint = resultChartViewA->mapFromScene(localPoint);
        //double dy = viewPoint.y();
        double realy = localPoint.y();
        if(realy - lastYD >= 0){
            rpmArray[choiceIndex] +=  50;
        }else{
            rpmArray[choiceIndex] -=  50;
        }
        if(rpmArray[choiceIndex] > 50000)
            rpmArray[choiceIndex] = 3000;

        spinBoxArray[choiceIndex]->blockSignals(true);
        spinBoxArray[choiceIndex]->setValue(rpmArray[choiceIndex]);
        spinBoxArray[choiceIndex]->blockSignals(false);
        lastYD = realy;
        drawResultChartD();
        if(bothChart){

            drawResultChartA();
            drawResultChartB();
            drawResultChartC();
        }
    }

    // chartCoordXD->setText(QString("X: %1").arg(localPoint.x()));

    // chartCoordYD->setText(QString("Y: %1").arg(localPoint.y()));
}
void propellerDisplay::getMousePressBeginPositionR(const QPointF &beginPoint){

    //mouseStartPosition = beginPoint;

    isMousePressed = true;
}

void propellerDisplay::getMouseReleaseEndPositionR(const QPointF &endPoint){


    isMousePressed = false;
    // if(chose >= 1)
    //     updateResultChart();
}
propellerDisplay::~propellerDisplay(){

}
void propellerDisplay::addDataPointA()
{
    DataPointDialog dialog(this);
    if (dialog.exec() == QDialog::Accepted) {
        QVector<QPair<double, double>> points = dialog.getPoints();

        for (const auto &point : points) {
            double x = point.first;
            double y = point.second;

            // Add the new data point to both series
            autoSeriesA->append(x, y);


            // Adjust axis ranges if necessary for Chart A
            if (x > resultAxisXA->max()) {
                resultAxisXA->setMax(x);
            }
            if (x < resultAxisXA->min()) {
                resultAxisXA->setMin(x);
            }
            if (y > resultAxisYA->max()) {
                resultAxisYA->setMax(y);
            }
            if (y < resultAxisYA->min()) {
                resultAxisYA->setMin(y);
            }
        }
    }
}
void propellerDisplay::addDataPointB()
{
    DataPointDialog dialog(this);
    if (dialog.exec() == QDialog::Accepted) {
        QVector<QPair<double, double>> points = dialog.getPoints();

        for (const auto &point : points) {
            double x = point.first;
            double y = point.second;

            // Add the new data point to both series
            autoSeriesB->append(x, y);


            // Adjust axis ranges if necessary for Chart A
            if (x > resultAxisXB->max()) {
                resultAxisXB->setMax(x);
            }
            if (x < resultAxisXB->min()) {
                resultAxisXB->setMin(x);
            }
            if (y > resultAxisYB->max()) {
                resultAxisYB->setMax(y);
            }
            if (y < resultAxisYB->min()) {
                resultAxisYB->setMin(y);
            }
        }
    }
}
void propellerDisplay::addDataPointC()
{
    DataPointDialog dialog(this);
    if (dialog.exec() == QDialog::Accepted) {
        QVector<QPair<double, double>> points = dialog.getPoints();

        for (const auto &point : points) {
            double x = point.first;
            double y = point.second;

            // Add the new data point to both series
            autoSeriesC->append(x, y);


            // Adjust axis ranges if necessary for Chart A
            if (x > resultAxisXC->max()) {
                resultAxisXC->setMax(x);
            }
            if (x < resultAxisXC->min()) {
                resultAxisXC->setMin(x);
            }
            if (y > resultAxisYC->max()) {
                resultAxisYC->setMax(y);
            }
            if (y < resultAxisYC->min()) {
                resultAxisYC->setMin(y);
            }
        }
    }
}
void propellerDisplay::addDataPointD()
{
    DataPointDialog dialog(this);
    if (dialog.exec() == QDialog::Accepted) {
        QVector<QPair<double, double>> points = dialog.getPoints();

        for (const auto &point : points) {
            double x = point.first;
            double y = point.second;

            // Add the new data point to both series
            autoSeriesD->append(x, y);


            // Adjust axis ranges if necessary for Chart A
            if (x > resultAxisXD->max()) {
                resultAxisXD->setMax(x);
            }
            if (x < resultAxisXD->min()) {
                resultAxisXD->setMin(x);
            }
            if (y > resultAxisYD->max()) {
                resultAxisYD->setMax(y);
            }
            if (y < resultAxisYD->min()) {
                resultAxisYD->setMin(y);
            }
        }
    }
}
void propellerDisplay::clearDataPointA()
{
    // Clear all data points from the autoSeriesA and autoSeriesB
    autoSeriesA->clear();
    //autoSeriesB->clear();
    updateAxes(resultChartA,seriesA);

}
void propellerDisplay::clearDataPointB()
{
    // Clear all data points from the autoSeriesA and autoSeriesB
    autoSeriesB->clear();
    //autoSeriesB->clear();
    updateAxes(resultChartB,seriesB);

}
void propellerDisplay::clearDataPointC()
{
    // Clear all data points from the autoSeriesA and autoSeriesB
    autoSeriesC->clear();
    //autoSeriesB->clear();
    updateAxes(resultChartC,seriesC);

}
void propellerDisplay::clearDataPointD()
{
    // Clear all data points from the autoSeriesA and autoSeriesB
    autoSeriesD->clear();
    //autoSeriesB->clear();
    updateAxes(resultChartD,seriesD);

}
void propellerDisplay::updateAxes(QChart *chart, const QVector<QLineSeries*> &seriesList) {
    if (seriesList.isEmpty()) return;

    // 初始化数据范围，设置为一个极端值以便于后续比较
    qreal minX = std::numeric_limits<qreal>::max();
    qreal maxX = std::numeric_limits<qreal>::lowest();
    qreal minY = std::numeric_limits<qreal>::max();
    qreal maxY = std::numeric_limits<qreal>::lowest();

    // 遍历所有曲线数据，计算总体范围
    for (QLineSeries *series : seriesList) {
        if (series->points().isEmpty()) continue; // 如果当前系列没有点，则跳过

        for (const QPointF &point : series->points()) {
            if (point.x() < minX) minX = point.x();
            if (point.x() > maxX) maxX = point.x();
            if (point.y() < minY) minY = point.y();
            if (point.y() > maxY) maxY = point.y();
        }
    }

    // 检查是否成功找到数据点
    if (minX == std::numeric_limits<qreal>::max() ||
        maxX == std::numeric_limits<qreal>::lowest() ||
        minY == std::numeric_limits<qreal>::max() ||
        maxY == std::numeric_limits<qreal>::lowest()) {
        return; // 没有有效的数据点，直接返回
    }

    // 为坐标轴范围添加额外的空间
    qreal xMargin = (maxX - minX) * 0.1;  // 添加10%的额外空间
    qreal yMargin = (maxY - minY) * 0.1;  // 添加10%的额外空间

    minX -= xMargin;
    maxX += xMargin;
    minY -= yMargin;
    maxY += yMargin;

    // 设置新的坐标轴范围
    QList<QAbstractAxis*> axesX = chart->axes(Qt::Horizontal);
    QList<QAbstractAxis*> axesY = chart->axes(Qt::Vertical);

    if (!axesX.isEmpty() && !axesY.isEmpty()) {
        QValueAxis *axisX = qobject_cast<QValueAxis*>(axesX.first());
        QValueAxis *axisY = qobject_cast<QValueAxis*>(axesY.first());

        if (axisX && axisY) {
            axisX->setRange(minX, maxX);
            axisY->setRange(minY, maxY);
        }
    }
}
int propellerDisplay::getAirfoilIndex(const QString tmp){
    int index = 0;
    for(int i = 0;i<propAirfoilNameArray.length();i++){
        if(propAirfoilNameArray[i] == tmp){
            return i;
        }
    }
    return index;
}
void propellerDisplay::hidePropActor(){

    vtkSmartPointer<vtkActorCollection> actors = rendererE->GetActors();
    actors->InitTraversal();
    vtkActor* actor = nullptr;
    while ((actor = actors->GetNextActor())) {
        if (!actor->GetPickable()) {
            actor->VisibilityOff();

        }
        //rendererE->RemoveActor(actor);
    }

}
void propellerDisplay::showPropActor(){
    vtkSmartPointer<vtkActorCollection> actors = rendererE->GetActors();
    actors->InitTraversal();
    vtkActor* actor = nullptr;
    while ((actor = actors->GetNextActor())) {
        if (!actor->GetPickable()) {
            actor->VisibilityOn();

        }
        //rendererE->RemoveActor(actor);
    }

}
void propellerDisplay::drawPropResult(int index) {
    // 如果 resultSeriesArray 不为空，先清除已有的 series
    if (!resultSeriesArray.isEmpty()) {
        clearChartView(resultChart, resultSeriesArray);
    }

    // 获取速度数组和转速数组的长度
    const int row = VLMSettingArray[propChoiceIndex].vinfArray.length();
    const int columns = VLMSettingArray[propChoiceIndex].rpmArray.length();

    // 获取需要绘制的 y 轴数据


    // 根据 index 判断需要绘制的内容
    if (index == 0) {
        // 获取 x 轴数据（速度数组）
        const QVector<double>& yArray = propResultArray[propChoiceIndex][2];
        const QVector<double>& xArray = VLMSettingArray[propChoiceIndex].vinfArray;

        // 遍历每个转速 (columns) 并生成对应的曲线
        for (int i = 0; i < columns; ++i) {
            QLineSeries* series = new QLineSeries;

            // 将每个点 (x, y) 添加到曲线中
            for (int j = 0; j < row; ++j) {
                int k = j * columns + i;
                series->append(xArray[j], yArray[k]);
            }


            int vTmp = VLMSettingArray[propChoiceIndex].rpmArray[i];
            QString txt = "转速:" + QString::number(vTmp) + "rpm";
            series->setName(txt);
            // 将曲线添加到 resultChart，并与轴连接
            resultChart->addSeries(series);
            series->attachAxis(resultAxisX);
            series->attachAxis(resultAxisY);

            // 将 series 存储在 resultSeriesArray 中，以便之后释放
            resultSeriesArray.append(series);
        }
        resultAxisX->setTitleText("速度(m/s)");
        resultAxisY->setTitleText("推力(N)");
        resultChart->setTitle("推力-速度 曲线");


        updateAxes(resultChart,resultSeriesArray);
    }else if(index == 1){
        // 获取 x 轴数据（速度数组）
        const QVector<double>& yArray = propResultArray[propChoiceIndex][2];
        const QVector<double>& xArray = VLMSettingArray[propChoiceIndex].rpmArray;

        // 遍历每个转速 (columns) 并生成对应的曲线
        for (int i = 0; i < row; ++i) {
            QLineSeries* series = new QLineSeries;

            // 将每个点 (x, y) 添加到曲线中
            for (int j = 0; j < columns; ++j) {
                int k = i * columns + j;
                series->append(xArray[j], yArray[k]);
            }


            int vTmp = VLMSettingArray[propChoiceIndex].vinfArray[i];
            QString txt = "速度:" + QString::number(vTmp) + "m/s";
            series->setName(txt);
            // 将曲线添加到 resultChart，并与轴连接
            resultChart->addSeries(series);
            series->attachAxis(resultAxisX);
            series->attachAxis(resultAxisY);

            // 将 series 存储在 resultSeriesArray 中，以便之后释放
            resultSeriesArray.append(series);
        }
        resultAxisX->setTitleText("转速(rpm)");
        resultAxisY->setTitleText("推力(N)");
        resultChart->setTitle("推力-转速 曲线");


        updateAxes(resultChart,resultSeriesArray);


    }else if(index == 2){
        // 获取 x 轴数据（速度数组）
        const QVector<double>& yArray = propResultArray[propChoiceIndex][1];
        const QVector<double>& xArray = propResultArray[propChoiceIndex][4];

        // 遍历每个转速 (columns) 并生成对应的曲线
        for (int i = 0; i < columns; ++i) {
            QLineSeries* series = new QLineSeries;

            // 将每个点 (x, y) 添加到曲线中
            for (int j = 0; j < row; ++j) {
                int k = j * columns + i;
                series->append(xArray[k], yArray[k]);
            }


            int vTmp = VLMSettingArray[propChoiceIndex].rpmArray[i];
            QString txt = "转速:" + QString::number(vTmp) + "rpm";
            series->setName(txt);
            // 将曲线添加到 resultChart，并与轴连接
            resultChart->addSeries(series);
            series->attachAxis(resultAxisX);
            series->attachAxis(resultAxisY);

            // 将 series 存储在 resultSeriesArray 中，以便之后释放
            resultSeriesArray.append(series);
        }
        resultAxisX->setTitleText("前进比");
        resultAxisY->setTitleText("效率");
        resultChart->setTitle("效率曲线");


        updateAxes(resultChart,resultSeriesArray);


    }else if(index == 3){
        // 获取 x 轴数据（速度数组）
        const QVector<double>& yArray = propResultArray[propChoiceIndex][5];
        const QVector<double>& xArray = propResultArray[propChoiceIndex][4];

        // 遍历每个转速 (columns) 并生成对应的曲线
        for (int i = 0; i < columns; ++i) {
            QLineSeries* series = new QLineSeries;

            // 将每个点 (x, y) 添加到曲线中
            for (int j = 0; j < row; ++j) {
                int k = j * columns + i;
                series->append(xArray[k], yArray[k]);
            }


            int vTmp = VLMSettingArray[propChoiceIndex].rpmArray[i];
            QString txt = "转速:" + QString::number(vTmp) + "rpm";
            series->setName(txt);

            // 将曲线添加到 resultChart，并与轴连接
            resultChart->addSeries(series);
            series->attachAxis(resultAxisX);
            series->attachAxis(resultAxisY);

            // 将 series 存储在 resultSeriesArray 中，以便之后释放
            resultSeriesArray.append(series);
        }

        resultAxisX->setTitleText("前进比");
        resultAxisY->setTitleText("推力系数");
        resultChart->setTitle("推力系数曲线");


        updateAxes(resultChart,resultSeriesArray);
    }else if(index == 4){
        // 获取 x 轴数据（速度数组）
        const QVector<double>& yArray = propResultArray[propChoiceIndex][6];
        const QVector<double>& xArray = propResultArray[propChoiceIndex][4];

        // 遍历每个转速 (columns) 并生成对应的曲线
        for (int i = 0; i < columns; ++i) {
            QLineSeries* series = new QLineSeries;

            // 将每个点 (x, y) 添加到曲线中
            for (int j = 0; j < row; ++j) {
                int k = j * columns + i;
                series->append(xArray[k], yArray[k]);
            }


            int vTmp = VLMSettingArray[propChoiceIndex].rpmArray[i];
            QString txt = "转速:" + QString::number(vTmp) + "rpm";
            series->setName(txt);

            // 将曲线添加到 resultChart，并与轴连接
            resultChart->addSeries(series);
            series->attachAxis(resultAxisX);
            series->attachAxis(resultAxisY);

            // 将 series 存储在 resultSeriesArray 中，以便之后释放
            resultSeriesArray.append(series);
        }

        resultAxisX->setTitleText("前进比");
        resultAxisY->setTitleText("功率系数系数");
        resultChart->setTitle("功率系数曲线");


        updateAxes(resultChart,resultSeriesArray);
    }else if(index == 5 && !propResultTypeArray[propChoiceIndex]){
        const QVector<double>& yArray = bemtSolver->dTArray[propStateIndex];
        const QVector<double>& xArray = bemtSolver->radiusArray;
        QLineSeries* series = new QLineSeries;
        for(int i = 0;i<xArray.length();i++){
            series->append(xArray[i], yArray[i]);
        }
        resultSeriesArray.append(series);
        resultChart->addSeries(series);
        series->attachAxis(resultAxisX);
        series->attachAxis(resultAxisY);
        int tmp = propStateIndex%columns;
        int vTmp = VLMSettingArray[propChoiceIndex].rpmArray[tmp];
        QString txt = "转速:" + QString::number(vTmp) + "rpm";
        series->setName(txt);
        resultAxisX->setTitleText("r");
        resultAxisY->setTitleText("dT");
        resultChart->setTitle("dT/dr");
        updateAxes(resultChart,resultSeriesArray);



    }else if(index == 6 && !propResultTypeArray[propChoiceIndex]){
        const QVector<double>& yArray = bemtSolver->dQArray[propStateIndex];
        const QVector<double>& xArray = bemtSolver->radiusArray;
        QLineSeries* series = new QLineSeries;
        for(int i = 0;i<xArray.length();i++){
            series->append(xArray[i], yArray[i]);
        }
        resultSeriesArray.append(series);
        resultChart->addSeries(series);
        series->attachAxis(resultAxisX);
        series->attachAxis(resultAxisY);
        int tmp = propStateIndex%columns;
        int vTmp = VLMSettingArray[propChoiceIndex].rpmArray[tmp];
        QString txt = "转速:" + QString::number(vTmp) + "rpm";
        series->setName(txt);
        resultAxisX->setTitleText("r");
        resultAxisY->setTitleText("dQ");
        resultChart->setTitle("dQ/dr");
        updateAxes(resultChart,resultSeriesArray);
    }
}

void propellerDisplay::clearChartView(QChart* chart, QVector<QLineSeries*>& resultSeriesArray) {
    // Step 1: 遍历所有的 QLineSeries 对象
    for (QLineSeries* series : resultSeriesArray) {
        // Step 2: 从 QChart 中移除 series
        chart->removeSeries(series);

        // Step 3: 删除 series 对象以释放内存
        delete series;
    }

    // Step 4: 清空 QVector 数组
    resultSeriesArray.clear();
}
void propellerDisplay::updateLog(const QString txt){
    propLogTextEdit->append(txt);
    propLogTextEdit->append("\n");
}
void propellerDisplay::updateBar(const int value){
    propLogBar->setValue(value);
}
void propellerDisplay::updateLabelState(){
    for(int i = 0;i<velocityEditArray.length();i++){
        QString vTmp = velocityEditArray[i]->text();
        for(int j = 0;j<rpmEditArray.length();j++){
            int k = i * rpmEditArray.length() + j;
            QString rpmTmp = rpmEditArray[j]->text();
            propStateLabelArray[k]->setText("v = " + vTmp + ",rpm = " + rpmTmp);
        }
    }
}
void propellerDisplay::updateGLTextA(QString text){

    std::string tmp = text.toStdString();
    textActorA->SetInput(tmp.c_str());
    textActorA->Modified();
    propDisplayWidgetE->renderWindow()->Render();


}
QVector<int> propellerDisplay::findMatchingIds(const QVector<QString>& airfoilNames, const QStringList& searchNames) {
    QVector<int> matchingIds;

    // 遍历 QStringList 中的每个字符串
    for (const QString& searchName : searchNames) {
        // 归一化搜索名称，去除空格并转换为小写
        QString normalizedSearchName = searchName.simplified().toLower();
        normalizedSearchName.remove(' ');

        // 遍历 QVector<QString> 中的每个元素
        for (int i = 0; i < airfoilNames.size(); ++i) {
            // 归一化气动外形名称，去除空格并转换为小写
            QString normalizedAirfoilName = airfoilNames[i].simplified().toLower();
            normalizedAirfoilName.remove(' ');
            //qDebug()<<normalizedAirfoilName <<" "<<normalizedSearchName;

            // 比较归一化后的字符串

            if (normalizedAirfoilName == normalizedSearchName) {
                matchingIds.append(i);  // 将匹配的索引添加到结果列表中
                break;  // 如果找到匹配项，可以跳出内层循环（可选）
            }
        }
    }

    return matchingIds;  // 返回所有匹配的索引
}
QString propellerDisplay::resultToString(){
    const QVector<QVector<double>> a = propResultArray[propChoiceIndex];
    int ind = propStateCombox->currentIndex();
    if(ind < 0)
        ind = 0;
    int row = VLMSettingArray[propChoiceIndex].vinfArray.length();
    int columns = VLMSettingArray[propChoiceIndex].rpmArray.length();
    int n = row * columns;
    int rRow = 0;
    int rColumns = 0;
    if(ind>=0 &&ind <= n - 1){
        rRow = ind / columns;
        rColumns = ind % columns;
    }
    QString text;
    QString tmp1 = propArray[propChoiceIndex].name + "    状态(" + QString::number(ind + 1) + ")";
    QString tmp2 = "\n转速(rpm): " + QString::number(VLMSettingArray[propChoiceIndex].rpmArray[rColumns]);
    QString tmp3 = "\n速度(m/s): " + QString::number(VLMSettingArray[propChoiceIndex].vinfArray[rRow]);
    QString tmp4 = "\n前进比: " + QString::number(a[4][ind],'f',2);
    QString tmp5 = "\n推力(N): " + QString::number(a[2][ind],'f',2);
    QString tmp6 = "\n扭矩(N/m): " + QString::number(a[3][ind],'f',2);
    QString tmp7 = "\n功率(w): " + QString::number(a[0][ind],'f',2);
    QString tmp8 = "\n效率: " + QString::number(a[1][ind],'f',2);
    text = tmp1 + tmp2 + tmp3 + tmp4 + tmp5 + tmp6 + tmp7 + tmp8;
    return text;
}
void propellerDisplay::updateViewText(){
    updateGLTextA(resultToString());

    int ind = propStateCombox->currentIndex();
    int index = propResultTypeCombox->currentIndex();
    propStateIndex = ind;
    if(index == 5 || index == 6)
        drawPropResult(index);

    if(propResultTypeArray[propChoiceIndex]){
        updatePressureContourView(VLMSolverArray[propChoiceIndex]);


        if(ind >= 0 && VLMSolverArray[propChoiceIndex]->isShowStreamLine){



            //int index = wingAlphaCombox->currentIndex();
            double a1 = streamEditArray[0]->text().toDouble();
            double a2 = streamEditArray[1]->text().toDouble();
            double a3 = streamEditArray[2]->text().toDouble();
            double a4 = streamEditArray[3]->text().toDouble();
            startCalculation();

            VLMSolverArray[propChoiceIndex]->computeStreamLine(ind,a1,a2,a3,a4);
            updateStreamLineView(VLMSolverArray[propChoiceIndex]);
        }
    }

    QApplication::processEvents(); // 确保界面更新

}
void propellerDisplay::startCalculation() {

    dialogBar->setValue(0);
    progressDialog->show(); // 显示对话框


}
void propellerDisplay::updateProgress(int value) {

    dialogBar->setValue(value);
    QApplication::processEvents(); // 确保界面更新


}
void propellerDisplay::onCalculationFinished() {

    progressDialog->close();
}
void propellerDisplay::initialProgressDialog(){


    // 初始化进度对话框
    progressDialog = new QDialog();
    QVBoxLayout *layout= new QVBoxLayout();
    dialogBar = new QProgressBar(progressDialog);
    layout->addWidget(dialogBar);
    progressDialog->setLayout(layout);

}

void propellerDisplay::addPropDataPoint()
{
    DataPointDialog dialog(this);
    if (dialog.exec() == QDialog::Accepted) {
        QVector<QPair<double, double>> points = dialog.getPoints();

        for (const auto &point : points) {
            double x = point.first;
            double y = point.second;

            // Add the new data point to both series
            autoSeriesA->append(x, y);


            // Adjust axis ranges if necessary for Chart A
            if (x > resultAxisX->max()) {
                resultAxisX->setMax(x);
            }
            if (x < resultAxisX->min()) {
                resultAxisX->setMin(x);
            }
            if (y > resultAxisY->max()) {
                resultAxisY->setMax(y);
            }
            if (y < resultAxisY->min()) {
                resultAxisY->setMin(y);
            }
        }
    }
}
void propellerDisplay::clearPropDataPoint()
{
    // Clear all data points from the autoSeriesA and autoSeriesB
    autoSeries->clear();
    //autoSeriesB->clear();
    updateAxes(resultChart,resultSeriesArray);

}
void propellerDisplay::removeSeriesLegendItem(QChart *chart, QLineSeries *series) {
    // Get all legend markers for the given series
    QList<QLegendMarker *> markers = chart->legend()->markers(series);

    // Iterate through all legend markers
    for (QLegendMarker *marker : markers) {
        // Hide the legend marker
        marker->setVisible(false);
    }
}
