#include "AirfoilClass/airfoilexplorer.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QScrollArea>
#include <QLabel>
#include <QtConcurrent/QtConcurrent>
#include "PublicClass/mymath.h"
#if defined(_MSC_VER) && (_MSC_VER >= 1600)
# pragma execution_character_set("utf-8")
#endif

airfoilExplorer::airfoilExplorer(QWidget *parent)
    : QWidget(parent)
{
    //创建一个失速模型
    double cl[25] = {
       -0.4116, -0.2934, -0.1741, -0.0542, 0.0655,
        0.1846, 0.3034, 0.4200, 0.5355, 0.6498,
        0.7630, 0.8742, 0.9836, 1.0934, 1.2026,
        1.3050, 1.4061, 1.5021, 1.5896, 1.6692,
        1.7327, 1.7764, 1.7775, 1.7296, 1.6479
    };

    // 攻角 alpha 对应
    double alpha[25];
    for (int i = 0; i < 25; ++i) {
        alpha[i] = -4.0 + i; // -4 到 20，每度 1°
    }

    for (int i = 0; i < 25; ++i) {
        StallDataPoint pt;
        pt.alpha = alpha[i];
        pt.Cl = cl[i];
        currentCurve.points.append(pt);
    }

    setupUI();
    connect(this,
            &airfoilExplorer::requestAppendText,
            this,
            &airfoilExplorer::appendColoredText,
            Qt::QueuedConnection);   // 🔴 必须 Queued

    connect(this,
            &airfoilExplorer::requestSelectionArray,
            this,
            &airfoilExplorer::updateListView,
            Qt::QueuedConnection);   // 🔴 必须 Queued


    //QFile file(":/qss/airfoilExplorer/explorer.qss");
    //file.open(QFile::ReadOnly);
    //this->setStyleSheet(file.readAll());
}

void airfoilExplorer::setupUI()
{
    // 主水平布局
    QHBoxLayout *mainLayout = new QHBoxLayout(this);

    /* ================= 左侧面板 ================= */
    QScrollArea *leftScroll = new QScrollArea;
    leftScroll->setWidgetResizable(true);

    QWidget *leftPanel = new QWidget;
    QVBoxLayout *leftLayout = new QVBoxLayout(leftPanel);


    // 现有左栏模块
    leftLayout->addWidget(createConditionGroup());
    leftLayout->addWidget(createAirfoilLibraryGroup());
    // 新增：多 Cl 设计 GroupBox
    leftLayout->addWidget(createDesignClGroup());

    leftLayout->addWidget(createSoftConstraintGroup());



    // Run 按钮
    runButton = new QPushButton("开始");
    leftLayout->addWidget(runButton);

    leftLayout->addStretch(); // 推到顶部
    leftPanel->setLayout(leftLayout);
    leftScroll->setWidget(leftPanel);


    /* ================= 中间面板 ================= */
    QWidget *rightPanel = createRightPanel();

    /* ================= 右侧面板 ================= */
    QWidget *centerPanel = new QWidget();
    QVBoxLayout *centerLayout = new QVBoxLayout(centerPanel);
    logWidget = new QPlainTextEdit();
    gressBar = new QProgressBar;
    centerLayout->addWidget(createCenterPanel());
    centerLayout->addWidget(logWidget);
    centerLayout->addWidget(gressBar);

    centerLayout->setContentsMargins(0, 0, 0, 0);
    centerLayout->setSpacing(0);


    /* ================= 主布局 ================= */
    mainLayout->addWidget(leftScroll,   1); // 左栏滚动
    mainLayout->addWidget(centerPanel,  3);
    mainLayout->addWidget(rightPanel, 1);





    leftScroll->setMinimumWidth(500);
    rightPanel->setMaximumWidth(400);
    logWidget->setFixedHeight(350);


    setLayout(mainLayout);

    /* ================= 初始化默认 Cl ================= */
    addDesignCl(0.4, -0.1,0.4);
    addDesignCl(0.6, -0.15,0.4);
    addDesignCl(0.8, -0.18,0.2);

    gressBar->setVisible(false);
    connect(runButton,&QPushButton::clicked,this,&airfoilExplorer::startSelection);
    connect(addClButton, &QPushButton::clicked, [=]() {
        addDesignCl(0.6,-0.1,0.2); // 默认添加 0.6
    });
}
void airfoilExplorer::drawAirfoil(const QItemSelection &selected,const QItemSelection &deselected){

    QModelIndexList indexes = selected.indexes();

    int index1 = indexes.at(0).row();
    int index2 = indexes.at(0).row();
    if(!selectionArray.isEmpty())
        index2 = selectionArray[index1];


    plotAirfoil(airfoilLib[index2],Qt::red);
    if(gressBarStep == 2){
        highlightCurve(index1);
        highlightGraphPoint(index1);
    }
}

/* ================= Left Panel ================= */



QGroupBox* airfoilExplorer::createConditionGroup()
{
    QGroupBox *group = new QGroupBox("工况");
    QVBoxLayout *layout = new QVBoxLayout(group);

    reSpin = new QDoubleSpinBox;
    reSpin->setRange(5000, 1e7);
    reSpin->setDecimals(0);
    reSpin->setSingleStep(10000);
    reSpin->setValue(200000);

    maSpin = new QDoubleSpinBox;
    maSpin->setRange(0,1);
    maSpin->setDecimals(3);
    maSpin->setSingleStep(0.01);
    maSpin->setValue(0.0);

    iterSpin = new QSpinBox;
    iterSpin->setRange(50,500);
    iterSpin->setSingleStep(1);
    iterSpin->setValue(100);

    stepSpin = new QDoubleSpinBox;
    stepSpin->setRange(0.05,5);
    stepSpin->setDecimals(2);
    stepSpin->setSingleStep(0.05);
    stepSpin->setValue(0.25);

    threadSpin = new QSpinBox;
    threadSpin->setRange(1,20);
    threadSpin->setSingleStep(1);
    threadSpin->setValue(10);

    xfoilSolver = new QCheckBox("XFoil (低雷诺数)");
    xfoilSolver->setChecked(true);

    windAISolver = new QCheckBox("windAIBench (高雷诺数)");
    windAISolver->setEnabled(false);

    layout->addWidget(new QLabel("Reynolds Number"));
    layout->addWidget(reSpin);
    layout->addWidget(new QLabel("Ma"));
    layout->addWidget(maSpin);
    layout->addWidget(new QLabel("Xfoil最大迭代数"));
    layout->addWidget(iterSpin);
    layout->addWidget(new QLabel("迎角步进"));
    layout->addWidget(stepSpin);
    layout->addWidget(new QLabel("线程数"));
    layout->addWidget(threadSpin);
    layout->addWidget(xfoilSolver);
    layout->addWidget(windAISolver);

    return group;
}
QGroupBox* airfoilExplorer::createAirfoilLibraryGroup()
{
    QGroupBox *group = new QGroupBox("翼型库");
    group->setStyleSheet("QGroupBox { border: 2px solid #27ae60; }");

    QVBoxLayout *layout = new QVBoxLayout(group);

    // 三个库
    profiliCheck = new QCheckBox("profili");
    profiliCheck->setChecked(true);

    uiucCheck = new QCheckBox("UIUC");
    uiucCheck->setChecked(true);

    windAICheck = new QCheckBox("windAIBench");
    windAICheck->setChecked(true);

    layout->addWidget(profiliCheck);
    layout->addWidget(uiucCheck);
    layout->addWidget(windAICheck);


    //layout->addStretch(); // 推到顶部

    return group;
}

QGroupBox* airfoilExplorer::createSoftConstraintGroup()
{
    QGroupBox *group = new QGroupBox("宽约束");
    group->setStyleSheet("QGroupBox { border: 2px solid #2980b9; }");

    QVBoxLayout *layout = new QVBoxLayout(group);

    /* ----------- 厚度 & Cl_max ----------- */

    tcMinSpin = new QDoubleSpinBox;
    tcMinSpin->setRange(0.01, 0.30);
    tcMinSpin->setValue(0.08);
    tcMinSpin->setDecimals(3);

    tcMaxSpin = new QDoubleSpinBox;
    tcMaxSpin->setRange(0.01, 0.30);
    tcMaxSpin->setValue(0.15);
    tcMaxSpin->setDecimals(3);

    clMaxSpin = new QDoubleSpinBox;
    clMaxSpin->setRange(0.0, 3.0);
    clMaxSpin->setValue(1.2);
    clMaxSpin->setDecimals(3);

    camberMaxSpin = new QDoubleSpinBox;
    camberMaxSpin->setRange(0.01, 1.00);
    camberMaxSpin->setValue(0.10);
    camberMaxSpin->setDecimals(3);

    camberMinSpin = new QDoubleSpinBox;
    camberMinSpin->setRange(0.01, 1.00);
    camberMinSpin->setValue(0.02);
    camberMinSpin->setDecimals(3);

    layout->addWidget(new QLabel("最小厚度"));
    layout->addWidget(tcMinSpin);
    layout->addWidget(new QLabel("最大厚度"));
    layout->addWidget(tcMaxSpin);
    layout->addWidget(new QLabel("最小弯度"));
    layout->addWidget(camberMinSpin);
    layout->addWidget(new QLabel("最大弯度"));
    layout->addWidget(camberMaxSpin);
    layout->addWidget(new QLabel("Cl_max ≥"));
    layout->addWidget(clMaxSpin);

    /* ----------- 失速斜率控制 ----------- */

    enableStallSlopeCheck = new QCheckBox;
    stallSlopeSlider = new QSlider(Qt::Horizontal);
    stallSlopeSlider->setRange(0, 100);
    stallSlopeSlider->setValue(1);
    stallSlopeSlider->setEnabled(false);

    stallSlopeLabel = new QLabel("0");
    stallSlopeLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);

    QGridLayout *stallSlopeLayout = new QGridLayout();
    stallSlopeLayout->setHorizontalSpacing(6);
    stallSlopeLayout->setVerticalSpacing(4);

    stallSlopeLayout->addWidget(new QLabel("失速后斜率控制"), 0, 0);
    stallSlopeLayout->addWidget(enableStallSlopeCheck,         0, 1, Qt::AlignLeft);
    stallSlopeLayout->addWidget(stallSlopeSlider,              1, 0, 1, 2);
    stallSlopeLayout->addWidget(stallSlopeLabel,               1, 2);

    layout->addLayout(stallSlopeLayout);

    /* ----------- 失速平台长度控制 ----------- */

    enableStallPlateauWidthCheck = new QCheckBox;
    stallPlateauWidthSlider = new QSlider(Qt::Horizontal);
    stallPlateauWidthSlider->setRange(0, 100);
    stallPlateauWidthSlider->setValue(1);
    stallPlateauWidthSlider->setEnabled(false);

    plateauWidthLabel = new QLabel("1");
    plateauWidthLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);

    QGridLayout *stallPlateauLayout = new QGridLayout();
    stallPlateauLayout->setHorizontalSpacing(6);
    stallPlateauLayout->setVerticalSpacing(4);

    stallPlateauLayout->addWidget(new QLabel("失速平台长度控制"), 0, 0);
    stallPlateauLayout->addWidget(enableStallPlateauWidthCheck,  0, 1, Qt::AlignLeft);
    stallPlateauLayout->addWidget(stallPlateauWidthSlider,       1, 0, 1, 2);
    stallPlateauLayout->addWidget(plateauWidthLabel,             1, 2);

    layout->addLayout(stallPlateauLayout);

    /* ----------- 交互 ----------- */

    connect(enableStallSlopeCheck, &QCheckBox::toggled,
            this, [this](bool on){
        stallSlopeSlider->setEnabled(on);
        rightTabs->setCurrentIndex(1); // 切到第 4 个 tab (plotWidget)
        updateStallMetric();
    });

    connect(enableStallPlateauWidthCheck, &QCheckBox::toggled,
            this, [this](bool on){
        stallPlateauWidthSlider->setEnabled(on);
        rightTabs->setCurrentIndex(1); // 切到第 4 个 tab (plotWidget)
        updateStallMetric();
    });

    connect(stallSlopeSlider, &QSlider::valueChanged,
            this, [this](int v){
        // v: 0~100 → slope: -0.00 ~ -1.00
        postStallSlope = -v / 100.0;

        stallSlopeLabel->setText(
            QString::number(postStallSlope, 'f', 2)
        );

        updateStallMetric();
    });

    connect(stallPlateauWidthSlider, &QSlider::valueChanged,
            this, [this](int v){
        // v: 0~100 → Δα: 0.0 ~ 10.0
        stallPlateauWidth = double(v / 10.0);

        plateauWidthLabel->setText(
            QString::number(stallPlateauWidth, 'f', 1)
        );

        updateStallMetric();
    });





    return group;
}




/* ================= Center Panel ================= */

QWidget* airfoilExplorer::createRightPanel()
{
    QWidget *panel = new QWidget;
    QVBoxLayout *layout = new QVBoxLayout(panel);

    QLabel *label = new QLabel("翼型列表");
    airfoilList = new QListView;
    listModel = new QStringListModel();
    airfoilList->setModel(listModel);
    //connect(airfoilList->selectionModel(),&QItemSelectionModel::selectionChanged,this,&airfoilLibary::drawAirfoil);



    layout->addWidget(label);
    layout->addWidget(airfoilList);
    connect(airfoilList->selectionModel(),&QItemSelectionModel::selectionChanged,this,&airfoilExplorer::drawAirfoil);

    return panel;
}

/* ================= Right Panel ================= */

QWidget* airfoilExplorer::createCenterPanel()
{
    QWidget *panel = new QWidget;
    QVBoxLayout *layout = new QVBoxLayout(panel);

    rightTabs = new QTabWidget;

    geometryPlot = new QCustomPlot();
    stallPlot = new QCustomPlot();
    resultPlot = new QCustomPlot();
    airfoilCurve = new QCPCurve(geometryPlot->xAxis,geometryPlot->yAxis);



    geometryPlot->xAxis->setRange(-0.2, 1.2);
    geometryPlot->yAxis->setRange(-0.7, 0.7);

    stallPlot->xAxis->setRange(-10, 25);
    stallPlot->yAxis->setRange(-1, 2);
    plotWidget = createFourPlotWidget();



    geometryPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);
    stallPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);
    resultPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);

    rightTabs->addTab(geometryPlot,tr("翼型"));
    rightTabs->addTab(stallPlot,    tr("失速模型"));
    rightTabs->addTab(resultPlot,       tr("结果曲线"));
    rightTabs->addTab(plotWidget, tr("解集"));

    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);



    layout->addWidget(rightTabs);
    resultPlot->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(resultPlot, &QCustomPlot::customContextMenuRequested,
            this, &airfoilExplorer::showResultPlotContextMenu);

    return panel;
}
QWidget* airfoilExplorer::createFourPlotWidget()
{
    QWidget* container = new QWidget();

    QGridLayout* grid = new QGridLayout(container);
    grid->setSpacing(4);          // plot 之间的间距
    grid->setContentsMargins(2,2,2,2);

    plotTL = new QCustomPlot(container);
    plotTR = new QCustomPlot(container);
    plotBL = new QCustomPlot(container);
    plotBR = new QCustomPlot(container);

    // Top-Left
    plotTL->xAxis->setLabel("厚度");
    plotTL->yAxis->setLabel("目标值");

    // Top-Right
    plotTR->xAxis->setLabel("力矩系数");
    plotTR->yAxis->setLabel("目标值");

    // Bottom-Left
    plotBL->xAxis->setLabel("最大升力系数");
    plotBL->yAxis->setLabel("目标值");

    // Bottom-Right
    plotBR->xAxis->setLabel("升力线斜率");
    plotBR->yAxis->setLabel("目标值");


    plotTL->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);
    plotTR->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);
    plotBL->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);
    plotBR->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);

    grid->addWidget(plotTL, 0, 0);
    grid->addWidget(plotTR, 0, 1);
    grid->addWidget(plotBL, 1, 0);
    grid->addWidget(plotBR, 1, 1);

    // 行列等比例拉伸（非常重要）
    grid->setRowStretch(0, 1);
    grid->setRowStretch(1, 1);
    grid->setColumnStretch(0, 1);
    grid->setColumnStretch(1, 1);

    connect(plotTL, &QCustomPlot::selectionChangedByUser,
            this, &airfoilExplorer::onAnyPlotSelectionChanged);
    connect(plotTR, &QCustomPlot::selectionChangedByUser,
            this, &airfoilExplorer::onAnyPlotSelectionChanged);
    connect(plotBL, &QCustomPlot::selectionChangedByUser,
            this, &airfoilExplorer::onAnyPlotSelectionChanged);
    connect(plotBR, &QCustomPlot::selectionChangedByUser,
            this, &airfoilExplorer::onAnyPlotSelectionChanged);


    return container;
}

QGroupBox* airfoilExplorer::createDesignClGroup()
{
    QGroupBox *group = new QGroupBox("设计点");
    QVBoxLayout *layout = new QVBoxLayout(group);

    designClList = new QListWidget;
    designClList->setSpacing(4);

    addClButton = new QPushButton("+ 添加设计点");

    layout->addWidget(designClList);
    layout->addWidget(addClButton);

    return group;
}
QWidget* airfoilExplorer::createClItem(double clValue, double cmValue, double weight)
{
    QWidget *itemWidget = new QWidget;
    QHBoxLayout *layout = new QHBoxLayout(itemWidget);
    layout->setContentsMargins(4, 2, 4, 2);
    layout->setSpacing(6);

    // --- Cl ---
    QLabel *clLabel = new QLabel("Cl =");
    QDoubleSpinBox *clSpin = new QDoubleSpinBox;
    clSpin->setRange(0.0, 2.0);
    clSpin->setSingleStep(0.05);
    clSpin->setDecimals(3);
    clSpin->setValue(clValue);

    // --- Cm ---
    QLabel *cmLabel = new QLabel("Cm =");
    QDoubleSpinBox *cmSpin = new QDoubleSpinBox;
    cmSpin->setRange(-1.0, 1.0);
    cmSpin->setDecimals(3);
    cmSpin->setSingleStep(0.02);
    cmSpin->setValue(cmValue);

    // --- 权重 ---
    QLabel *wLabel = new QLabel("w =");
    QDoubleSpinBox *wSpin = new QDoubleSpinBox;
    wSpin->setRange(0.0, 100.0);
    wSpin->setDecimals(2);
    wSpin->setValue(weight);




    // --- 删除按钮 ---
    QPushButton *removeBtn = new QPushButton("x");
    removeBtn->setFixedWidth(20);

    layout->addWidget(clLabel);
    layout->addWidget(clSpin);
    layout->addWidget(cmLabel);
    layout->addWidget(cmSpin);
    layout->addWidget(wLabel);
    layout->addWidget(wSpin);

    layout->addStretch();
    layout->addWidget(removeBtn);

    // 删除行为（安全版）
    connect(removeBtn, &QPushButton::clicked, this, [=]() {
        for (int i = 0; i < designClList->count(); ++i) {
            if (designClList->itemWidget(designClList->item(i)) == itemWidget) {
                delete designClList->takeItem(i);
                break;
            }
        }
    });

    return itemWidget;
}


void airfoilExplorer::importAirfoilName(const QStringList &a,const QStringList&b,const QStringList&c ){
    profiliName = a;
    UIUCName = b;
    windAIBenchName = c;
    QStringList nameArray;
    nameArray.append(a);
    nameArray.append(b);
    nameArray.append(c);

    listModel->setStringList(nameArray);
}
void airfoilExplorer::importAirfoilParameters(const QVector<AirfoilParameters> &a, const QVector<AirfoilParameters> &b, const QVector<AirfoilParameters> &c){
    int len1 = profiliName.length();
    int len2 = UIUCName.length();
    int len3 = windAIBenchName.length();





    for(int i = 0;i<len1;i++){
        airfoilLib[i].maxThickness = a[i].MaxThickness;
        airfoilLib[i].maxCamber = a[i].MaxCamber;
    }

    for(int i = 0;i<len2;i++){
        airfoilLib[i + len1].maxThickness = b[i].MaxThickness;
        airfoilLib[i + len1].maxCamber = b[i].MaxCamber;
    }

    for(int i = 0;i<len3;i++){
        airfoilLib[i + len1 + len2].maxThickness = c[i].MaxThickness;
        airfoilLib[i + len1 + len2].maxCamber = c[i].MaxCamber;
    }







}
void airfoilExplorer::importAirfoilData(
    const QVector<QVector<double>> (&profili)[1000],
    const QVector<QVector<double>> (&UIUC)[1600],
    const QVector<QVector<double>> (&windAI)[2000])
{
    profiliAirfoil = profili;
    UIUCAirfoil = UIUC;
    windAIBenchAirfoil = windAI;
}
void airfoilExplorer::initialAirfoilLib(){
    int len1 = profiliName.length();
    int len2 = UIUCName.length();
    int len3 = windAIBenchName.length();
    for(int i = 0;i<len1;i++){
        AirfoilLib tmp;
        tmp.name = profiliName.at(i);
        tmp.id = i;
        tmp.data = &profiliAirfoil[i];
        tmp.maxThickness = 0;
        airfoilLib.append(tmp);
    }
    for(int i = 0;i<len2;i++){
        AirfoilLib tmp;
        tmp.name = UIUCName.at(i);
        tmp.id = i + len1;
        tmp.data = &UIUCAirfoil[i];
        tmp.maxThickness = 0;
        airfoilLib.append(tmp);
    }
    for(int i = 0;i<len3;i++){
        AirfoilLib tmp;
        tmp.name = windAIBenchName.at(i);
        tmp.id = i + len1 + len2;
        tmp.data = &windAIBenchAirfoil[i];
        tmp.maxThickness = 0;
        airfoilLib.append(tmp);
    }


}
void airfoilExplorer::initialListView(){
    QStringList nameArray;
    nameArray.append(profiliName);
    nameArray.append(UIUCName);
    nameArray.append(windAIBenchName);
    listModel->setStringList(nameArray);

}
void airfoilExplorer::updateResultPlot(const int index){
    switch (index) {
    case 0:
        for(int i = 0;i<resultCurveArray.length();i++){
            int index1 = selectionArray[i];
            plotResultCurve(resultCurveArray[i],airfoilLib[index1].alphaArray,airfoilLib[index1].clArray);
        }
        break;
    case 1:
        for(int i = 0;i<resultCurveArray.length();i++){
            int index1 = selectionArray[i];
            plotResultCurve(resultCurveArray[i],airfoilLib[index1].alphaArray,airfoilLib[index1].cdArray);
        }
        break;
    case 2:
        for(int i = 0;i<resultCurveArray.length();i++){
            int index1 = selectionArray[i];
            plotResultCurve(resultCurveArray[i],airfoilLib[index1].alphaArray,airfoilLib[index1].cmArray);
        }
        break;
    case 3:
        for(int i = 0;i<resultCurveArray.length();i++){
            int index1 = selectionArray[i];
            plotResultCurve(resultCurveArray[i],airfoilLib[index1].alphaArray,airfoilLib[index1].kArray);
        }
        break;
    case 4:
        for(int i = 0;i<resultCurveArray.length();i++){
            int index1 = selectionArray[i];
            plotResultCurve(resultCurveArray[i],airfoilLib[index1].alphaArray,airfoilLib[index1].pArray);
        }
        break;
    case 5:
        for(int i = 0;i<resultCurveArray.length();i++){
            int index1 = selectionArray[i];
            plotResultCurve(resultCurveArray[i],airfoilLib[index1].cdArray,airfoilLib[index1].clArray);
        }
        break;

    default:
        break;

    }

    resultPlot->rescaleAxes();
    resultPlot->replot();

}
void airfoilExplorer::startSelection(){


    runButton->setEnabled(false);
    gressBarStep = 0;
    gressBar->setVisible(true);
    gressBar->setValue(0);
    selectionArray.clear();

    // --- 1. 清空 resultCurveArray 的数据 ---
    for (QCPCurve* curve : resultCurveArray)
    {
        if (curve)
        {
            curve->data()->clear();                   // 清空所有曲线数据
            //curve->clearSelection();                  // 清除选中状态
            curve->setPen(QPen(Qt::black, 1.0, Qt::DashLine)); // 可选：重置样式为淡色
        }
    }

    // --- 2. 清空四个 graph 的数据 ---
    for (QCPGraph* g : {graphTL, graphTR, graphBL, graphBR})
    {
        if (g)
        {
            g->data()->clear();       // 清空 graph 数据
            //g->clearSelection();      // 清除选中状态
        }
    }

    // --- 3. 刷新绘图 ---
    if(plotTL) plotTL->replot(QCustomPlot::rpQueuedReplot);
    if(plotTR) plotTR->replot(QCustomPlot::rpQueuedReplot);
    if(plotBL) plotBL->replot(QCustomPlot::rpQueuedReplot);
    if(plotBR) plotBR->replot(QCustomPlot::rpQueuedReplot);
    if(resultPlot) resultPlot->replot(QCustomPlot::rpQueuedReplot);







    initialListView();


    initialXfoilSetting();

    checkThicknessConstraint();


    auto *watcher = new QFutureWatcher<void>(this);

    connect(watcher, &QFutureWatcher<void>::finished,
            this, [this, watcher]() {


        sortByWeightedScore();
        for(int i = 0;i<selectionArray.length();i++){
            QCPCurve *curve = new QCPCurve(resultPlot->xAxis,resultPlot->yAxis);
            resultCurveArray.append(curve);
            int index = selectionArray[i];
            plotResultCurve(curve,airfoilLib[index].alphaArray,airfoilLib[index].clArray);
        }
        resultPlot->xAxis->setLabel("alpha");
        resultPlot->yAxis->setLabel("Cl");
        resultPlot->rescaleAxes();
        resultPlot->replot();
        runButton->setEnabled(true);

        drawThicknessAndFx();
        drawCmAndFx();
        drawClMaxAndFx();
        drawClSlopeAndFx();

        rightTabs->setCurrentIndex(3); // 切到第 4 个 tab (plotWidget)


        watcher->deleteLater();
    });


    QFuture<void>future = QtConcurrent::run([&](){
        checkMomentConstraint();
        checkClMaxConstraint();


    });
    watcher->setFuture(future);

    //

}
void airfoilExplorer::initialXfoilSetting(){

    double weight = 0.0;
    int iterStep = iterSpin->value();
    double stepSize = stepSpin->value();
    threadNum = threadSpin->value();
    if(iterStep<20 || iterStep > 300)
        iterStep = 100;
    if(stepSize < 0.05 || stepSize > 5)
        stepSize = 0.25;
    if(threadNum < 1)
        threadNum = 1;

    for(int i = 0;i<getDesignPoints().numDesignPoints;i++){
        setting[i].Re = reSpin->value();
        setting[i].Ma = maSpin->value();
        setting[i].designCL = getDesignPoints().cl[i];
        setting[i].model = 1;
        setting[i].nCrit = 9;
        setting[i].minAlpha = 0;
        setting[i].maxAlpha = 15;
        setting[i].alphaStepSize = stepSize;
        setting[i].xtrTop = 1.0;
        setting[i].xtrBot = 1.0;
        setting[i].s_IterLim = iterStep;
        weight  =  weight + getDesignPoints().weight[i];
    }
    //归一化
    designLib = getDesignPoints();
    for(int i = 0;i<getDesignPoints().numDesignPoints;i++){
        designLib.weight[i] = getDesignPoints().weight[i] / weight;
    }



}
void airfoilExplorer::checkThicknessConstraint(){
    QVector<int>choiceArray;
    double min = tcMinSpin->value();
    double max = tcMaxSpin->value();
    double minCamber = camberMinSpin->value();
    double maxCamber = camberMaxSpin->value();

    int len1 = profiliName.length();
    int len2 = UIUCName.length();
    int len3 = windAIBenchName.length();
    if (min > max) {
        std::swap(min, max);
    }

    if (minCamber > maxCamber) {
        std::swap(minCamber, maxCamber);
    }

    bool useProfili = profiliCheck->isChecked();
    bool useUIUC = uiucCheck->isChecked();
    bool useWindAI = windAICheck->isChecked();

    int start = 0;

    // Profili
    if (useProfili) {
        for (int i = 0; i < len1; i++) {
            double t = airfoilLib[i].maxThickness;
            double c = airfoilLib[i].maxCamber;
            bool ok1 = t >= min && t <= max;
            bool ok2 = c >= minCamber && c <= maxCamber;
            if (ok1 && ok2)
                choiceArray.append(airfoilLib[i].id);
        }
    }
    start += len1;

    // UIUC
    if (useUIUC) {
        for (int i = start; i < start + len2; i++) {
            double t = airfoilLib[i].maxThickness;
            double c = airfoilLib[i].maxCamber;
            bool ok1 = t >= min && t <= max;
            bool ok2 = c >= minCamber && c <= maxCamber;
            if (ok1 && ok2)
                choiceArray.append(airfoilLib[i].id);
        }
    }
    start += len2;

    // WindAI
    if (useWindAI) {
        for (int i = start; i < start + len3; i++) {
            double t = airfoilLib[i].maxThickness;
            double c = airfoilLib[i].maxCamber;
            bool ok1 = t >= min && t <= max;
            bool ok2 = c >= minCamber && c <= maxCamber;
            if (ok1 && ok2)
                choiceArray.append(airfoilLib[i].id);
        }
    }

    selectionArray = choiceArray;
    QString text = "筛选出" + QString::number(selectionArray.length()) + "个翼型";
    appendColoredText(text,Qt::green,0);
    updateListView(selectionArray);

    sumValue = selectionArray.length() * (5 + designLib.numDesignPoints);






}
void airfoilExplorer::checkMomentConstraint(){
    //对厚度筛选过的翼型进行多点气动计算
    int threadSum = selectionArray.length();
    int step = threadSum / threadNum;
    int remainderTmp = threadSum % threadNum;


    for(int i = 0;i<designLib.numDesignPoints;i++){
        for(int j = 0;j<step;j++){
            int index = j * threadNum;
            startXfoilInThread(index,threadNum,i);

        }

        if(remainderTmp > 0){
            int beginIndex = threadSum - remainderTmp;
            startXfoilInThread(beginIndex,remainderTmp,i);
        }
    }

        //筛选力矩满足条件的
    QVector<int>validIndices;
    for(int i = 0;i<selectionArray.length();i++){
        int ind = selectionArray[i];
        bool allOk = true; // 假设当前 ind 都满足
        for(int j = 0; j < designLib.numDesignPoints; j++)
        {
           // 判断当前设计点的 Cm 是否满足条件
            if(airfoilLib[ind].cm[j] < designLib.cm[j])
            {
                allOk = false; // 有一个不满足，标记为 false
                break;         // 不用再检查其他点
            }
        }

        if(allOk)
        {
            validIndices.append(ind);
        }

    }
    selectionArray = validIndices;
    emit requestSelectionArray(validIndices);






}
void airfoilExplorer::checkClMaxConstraint(){
    //对力矩筛选过的翼型进行多点气动计算

    int threadSum = selectionArray.length();
    int step = threadSum / threadNum;
    int remainderTmp = threadSum % threadNum;
    setting[0].model = 0;//进行多迎角分析
    for(int i = 0;i<step;i++){
        int index = i * threadNum;
        startXfoilInThread(index,threadNum);
        if(i == 0)
            gressBarStep = 1;

    }

    if(remainderTmp > 0){
        int beginIndex = threadSum - remainderTmp;
        startXfoilInThread(beginIndex,remainderTmp);
    }
    //筛选最大升力约束满足条件的
    QVector<int>validIndices;
    //

    for(int i = 0;i<selectionArray.length();i++){
        int index = selectionArray[i];
        bool ok = checkClArrayAgainstPostStallSlope(airfoilLib[index].alphaArray,airfoilLib[index].clArray);
        if(ok)
            validIndices.append(index);

    }

    selectionArray = validIndices;

    emit requestSelectionArray(validIndices);







}
void airfoilExplorer::sortByWeightedScore(){


    //按照评分进行排序

    gressBarStep = 2;
    gressBar->setVisible(false);

    QVector<double>scoreArray;
    for(int i = 0;i<selectionArray.length();i++){
        int index = selectionArray[i];
        scoreArray.append(computeWeightedInverseCdScore(airfoilLib[index]));
    }



    sortScoresWithIds(scoreArray,selectionArray);
    for(int i = 0;i<selectionArray.length();i++){
        int index = selectionArray[i];
        QString text = "翼型: " + airfoilLib[index].name + "     目标值: "
                + QString::number(scoreArray[i]);
        emit requestAppendText(text,Qt::green,100);
    }

    emit requestSelectionArray(selectionArray);




}
void airfoilExplorer::startXfoilInThread(const int index,const int num,const int k){
    QVector<QFuture<void>>futures;

    QFutureSynchronizer<void> sync;
    QVector<airfoilSolve*>testSolve;




    for(int i = 0;i<num;i++){
        int ind = selectionArray[index + i];
        airfoilSolve *solvers = new airfoilSolve();
        testSolve.append(solvers);
        solvers->importAirfoil(*airfoilLib[ind].data);
        solvers->refreshParaments(setting[k]);
        QFuture<void>future = QtConcurrent::run([solvers](){
            solvers->emitResult();
        });
        futures.append(future);
    }



    for (auto &f : futures) {
        sync.addFuture(f);
    }

    sync.waitForFinished();   // ✅ 等“所有线程”一起完成
    for(int i = 0;i<num;i++){
        int ind = selectionArray[index + i];
        airfoilLib[ind].cl[k] = testSolve[i]->onceData.cL;
        airfoilLib[ind].cd[k] = testSolve[i]->onceData.cD;
        airfoilLib[ind].cm[k] = testSolve[i]->onceData.cM;

        QString text;
        QColor color;
        if(testSolve[i]->onceData.cL <= 1e-5){
            color = Qt::red;
            text = "翼型: " + airfoilLib[ind].name + "      无法收敛";
            airfoilLib[ind].badSolutions[k] = false;
        }else{

            color = Qt::black;
            text = "翼型: " + airfoilLib[ind].name + "      Cl: " + QString::number(testSolve[i]->onceData.cL) + "      Cd = "
                    + QString::number(testSolve[i]->onceData.cD) + "      Cm: " + QString::number(testSolve[i]->onceData.cM);
            airfoilLib[ind].badSolutions[k] = true;
        }

        int value = k * selectionArray.length() + index + i;
        emit requestAppendText(text, color,value);

    }


    qDeleteAll(testSolve);
    testSolve.clear();
}
void airfoilExplorer::startXfoilInThread(const int index, const int num){
    QVector<QFuture<void>>futures;

    QFutureSynchronizer<void> sync;
    QVector<airfoilSolve*>testSolve;




    for(int i = 0;i<num;i++){
        int ind = selectionArray[index + i];
        airfoilSolve *solvers = new airfoilSolve();
        testSolve.append(solvers);
        solvers->importAirfoil(*airfoilLib[ind].data);
        solvers->refreshParaments(setting[0]);
        QFuture<void>future = QtConcurrent::run([solvers](){
            solvers->solver();
        });
        futures.append(future);
    }


    for (auto &f : futures) {
        sync.addFuture(f);
    }

    sync.waitForFinished();   // ✅ 等“所有线程”一起完成
    myMath math;
    for(int i = 0;i<num;i++){
        int ind = selectionArray[index + i];
        //升力系数赋值
        for(int j = 0;j<testSolve[i]->resultData.length();j++){
            airfoilLib[ind].alphaArray.append(testSolve[i]->resultData[j][0]);
            airfoilLib[ind].clArray.append(testSolve[i]->resultData[j][1]);
            airfoilLib[ind].cdArray.append(testSolve[i]->resultData[j][2]);
            airfoilLib[ind].cmArray.append(testSolve[i]->resultData[j][5]);
            airfoilLib[ind].kArray.append(testSolve[i]->resultData[j][3]);
            airfoilLib[ind].pArray.append(testSolve[i]->resultData[j][4]);
        }
        int alphaMaxIndex = math.getMaxIndex(airfoilLib[ind].clArray);
        QString text = "翼型: " + airfoilLib[ind].name + "      失速迎角: " + QString::number(airfoilLib[ind].alphaArray[alphaMaxIndex]) +
                "      最大升力系数: " + QString::number(airfoilLib[ind].clArray[alphaMaxIndex]);

        int value = index + i;
        emit requestAppendText(text, Qt::black,value);

    }



    qDeleteAll(testSolve);
    testSolve.clear();
}
// airfoilExplorer.h 或 airfoilExplorer.cpp 中
void airfoilExplorer::plotAirfoil(const AirfoilLib& lib, const QColor& color)
{
    if (!lib.data) return;

    const QVector<QVector<double>>& airfoil = *(lib.data);

    // 2. 创建 QCPCurve（替代 QCPGraph）


    airfoilCurve->data()->clear();
    // 3. 准备数据（保持你的数据读取逻辑）
    QVector<double> t, x, y;  // t 是参数化变量（索引）
    int index = 0;
    for (const auto &pt : airfoil) {
        t.append(index++);    // 用索引作为参数
        x.append(pt[0]);      // 原始x坐标
        y.append(pt[1]);      // 原始y坐标
    }

    // 4. 设置曲线数据（参数t，x坐标，y坐标）
    airfoilCurve->setData(t, x, y);

    // 5. 设置曲线样式（连续线）
    airfoilCurve->setLineStyle(QCPCurve::lsLine);
    airfoilCurve->setPen(QPen(color, 2));  // 设置线条颜色和宽度（可选）

    // 7. 重新绘制
    geometryPlot->replot();

}
void airfoilExplorer::showResultPlotContextMenu(const QPoint &pos)
{
    QMenu menu(this);

    QAction *actCl   = menu.addAction("升力系数曲线 (Cl)");
    QAction *actCd   = menu.addAction("阻力系数曲线 (Cd)");
    QAction *actCm   = menu.addAction("力矩系数曲线 (Cm)");
    QAction *actClCd = menu.addAction("升阻比曲线 (Cl/Cd)");
    QAction *actCp   = menu.addAction("功率因子曲线 (p)");

    menu.addSeparator();

    QAction *actPolar = menu.addAction("极曲线 (Cl–Cd)");

    QAction *selected = menu.exec(resultPlot->mapToGlobal(pos));
    if (!selected) return;

    if (selected == actCl){
        resultPlot->xAxis->setLabel("alpha");
        resultPlot->yAxis->setLabel("Cl");
        updateResultPlot(0);

    }

    else if (selected == actCd){
        resultPlot->xAxis->setLabel("alpha");
        resultPlot->yAxis->setLabel("Cd");
        updateResultPlot(1);

    }

    else if (selected == actCm){
        resultPlot->xAxis->setLabel("alpha");
        resultPlot->yAxis->setLabel("Cm");
        updateResultPlot(2);

    }

    else if (selected == actClCd){
        resultPlot->xAxis->setLabel("alpha");
        resultPlot->yAxis->setLabel("升阻比");
        updateResultPlot(3);

    }

    else if (selected == actCp){
        resultPlot->xAxis->setLabel("alpha");
        resultPlot->yAxis->setLabel("功率因子");
        updateResultPlot(4);

    }

    else if (selected == actPolar){
        resultPlot->xAxis->setLabel("Cd");
        resultPlot->yAxis->setLabel("Cl");
        updateResultPlot(5);

    }

}

void airfoilExplorer::plotResultCurve(QCPCurve*curve,const QVector<double>&xArray,
                                      const QVector<double>&yArray){
    if(xArray.isEmpty())
        return;

    QVector<double>t;
    for(int i = 0;i<xArray.length();i++)
        t.append(i);
    curve->setData(t,xArray,yArray);

    QColor fadedColor = Qt::black;
    fadedColor.setAlpha(80); // 半透明
    curve->setPen(QPen(fadedColor, 1.0));


}


void airfoilExplorer::addDesignCl(double cl, double cm, double weight)
{
    QListWidgetItem *item = new QListWidgetItem(designClList);
    QWidget *widget = createClItem(cl, cm, weight);

    item->setSizeHint(widget->sizeHint());
    designClList->addItem(item);
    designClList->setItemWidget(item, widget);
}
AirfoilLib airfoilExplorer::getDesignPoints()
{
    AirfoilLib lib;
    lib.numDesignPoints = designClList->count();
    for(int i = 0; i < designClList->count(); ++i){
        QWidget *w = designClList->itemWidget(designClList->item(i));
        auto spins = w->findChildren<QDoubleSpinBox*>();

        lib.cl[i]     = spins[0]->value();
        lib.cm[i]     = spins[1]->value();
        lib.weight[i] = spins[2]->value();

    }
    return lib;
}


void airfoilExplorer::setEqualAspectPlot(QCustomPlot* plot, double xMin, double xMax)
{
    if(!plot) return;

    // 设置 X 轴范围
    plot->xAxis->setRange(xMin, xMax);

    // 获取绘图区宽高
    double w = plot->axisRect()->width();
    double h = plot->axisRect()->height();
    if(h == 0) return; // 防止除0

    // 根据宽高比自动计算 Y 轴范围，保持 X 单位 = Y 单位
    double xRange = xMax - xMin;
    double yRange = xRange * h / w; // 等比例
    double yMid = 0.0; // Y 中心，可修改

    plot->yAxis->setRange(yMid - yRange/2, yMid + yRange/2);

    // 保证拖拽/缩放时也等比例
    plot->xAxis->setScaleRatio(plot->yAxis, 1.0);
}
void airfoilExplorer::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);

    // 保持绘图区等比例
    setEqualAspectPlot(geometryPlot, -0.2, 1.2);

}
/*
 * α ≤ α_stall                  : 原始曲线
α_stall < α ≤ α_stall + W    : Cl = Cl_max (平台)
α > α_stall + W              : Cl = Cl_max + slope * (α - α_stall - W)
                                slope < 0
  */
void airfoilExplorer::drawStallCurve(QCustomPlot *plot,
                                     const StallCurve &curve)
{
    plot->addGraph();
    QCPGraph *g = plot->graph();

    QVector<double> alpha, cl;
    for (const auto &p : curve.points) {
        alpha << p.alpha;
        cl    << p.Cl;
    }

    g->setData(alpha, cl);
    g->setPen(QPen(QColor(120,120,120), 4, Qt::DashLine));
    g->setName("Original Cl-α");

    plot->xAxis->setLabel("Alpha (deg)");
    plot->yAxis->setLabel("Cl");
}


void airfoilExplorer::drawPostStallSlope(QCustomPlot *plot, const StallCurve &curve, double slope)
{
    // 找到失速点（Cl 最大值）
    int stallIndex = 0;
    double ClMax = -1e9;
    for(int i=0; i<curve.points.size(); i++){
        if(curve.points[i].Cl > ClMax){
            ClMax = curve.points[i].Cl;
            stallIndex = i;
        }
    }

    double alphaStall = curve.points[stallIndex].alpha;
    double ClStall = ClMax;

    // 计算失速后斜率线
    QVector<double> alphaPost, ClPost;
    for(int i=stallIndex; i<curve.points.size(); i++){
        double da = curve.points[i].alpha - alphaStall;
        alphaPost.append(curve.points[i].alpha);
        ClPost.append(ClStall + slope * da);
    }

    plot->addGraph();
    plot->graph()->setData(alphaPost, ClPost);
    plot->graph()->setPen(QPen(Qt::red, 2, Qt::DashLine));
    plot->replot();
}
void airfoilExplorer::drawStallPlateau(QCustomPlot *plot, const StallCurve &curve, double plateauWidth)
{
    int stallIndex = 0;
    double ClMax = -1e9;
    for(int i=0; i<curve.points.size(); i++){
        if(curve.points[i].Cl > ClMax){
            ClMax = curve.points[i].Cl;
            stallIndex = i;
        }
    }

    double alphaStall = curve.points[stallIndex].alpha;
    double ClStall = ClMax;

    // 平台起止点
    double alphaStart = alphaStall;
    double alphaEnd   = alphaStall + plateauWidth;

    plot->addGraph();
    plot->graph()->setData(QVector<double>{alphaStart, alphaEnd},
                           QVector<double>{ClStall, ClStall});
    plot->graph()->setPen(QPen(Qt::blue, 2, Qt::DashLine));
    plot->replot();
}

void airfoilExplorer::updateStallMetric()
{
    if(!stallPlot) return;

    stallPlot->clearGraphs();

    // ---------------- 1️⃣ 绘制原始曲线 ----------------
    drawStallCurve(stallPlot, currentCurve);

    // ---------------- 2️⃣ 找到失速点 ----------------
    int stallIndex = 0;
    double ClMax = -1e9;
    for(int i=0; i<currentCurve.points.size(); i++){
        if(currentCurve.points[i].Cl > ClMax){
            ClMax = currentCurve.points[i].Cl;
            stallIndex = i;
        }
    }
    double alphaStall = currentCurve.points[stallIndex].alpha;
    double ClStall    = ClMax;

    // ---------------- 3️⃣ slider 参数 ----------------
    double slope = -stallSlopeSlider->value() / 100.0;       // 斜率值
    double redLineLength = stallPlateauWidthSlider->value() / 10.0; // 红线长度由 slider 控制

    // ---------------- 4️⃣ 绘制红色斜率线 ----------------
    if(enableStallSlopeCheck->isChecked()){
        double slopeStartAlpha = alphaStall;
        double slopeEndAlpha   = alphaStall + redLineLength; // 长度 slider 控制
        double slopeStartCl    = ClStall;
        double slopeEndCl      = slopeStartCl + slope * (slopeEndAlpha - slopeStartAlpha);

        stallPlot->addGraph();
        stallPlot->graph()->setData(
            QVector<double>{slopeStartAlpha, slopeEndAlpha},
            QVector<double>{slopeStartCl, slopeEndCl}
        );
        stallPlot->graph()->setPen(QPen(QColor(255,80,80), 2, Qt::DashLine));
        stallPlot->graph()->setName("Post-stall slope");
    }

    // ---------------- 5️⃣ 调整显示 ----------------

    stallPlot->replot();
}
bool airfoilExplorer::checkClArrayAgainstPostStallSlope(
    const QVector<double> &alphaArray,   // 与 clArray 对应的攻角
    const QVector<double> &clArray)
{
    if(alphaArray.size() != clArray.size()) return false;

    // ---------------- 找到失速点 ----------------
    int stallIndex = 0;
    double ClMax = -1e9;
    for(int i=0; i<clArray.length(); i++){
        if(clArray[i] > ClMax){
            ClMax = clArray[i];
            stallIndex = i;
        }
    }
    double alphaStall = alphaArray[stallIndex];
    double ClStall    = ClMax;
    if(ClMax < clMaxSpin->value())
        return false;

    // ---------------- 斜率与长度 ----------------
    double slope = -stallSlopeSlider->value() / 100.0;           // 斜率
    double redLineLength = stallPlateauWidthSlider->value() / 10.0; // 红线长度
    double alphaEnd = alphaStall + redLineLength;

    // ---------------- 检查每个点 ----------------
    for(int i=0; i<alphaArray.size(); i++){
        double a = alphaArray[i];
        double cl = clArray[i];

        // 只检查失速点之后，且不超过红线长度
        if(a >= alphaStall && a <= alphaEnd){
            double allowedCl = ClStall + slope * (a - alphaStall);
            if(cl > allowedCl){
                return false; // 不满足标准
            }
        }
    }

    return true; // 全部符合标准
}

void airfoilExplorer::appendColoredText(const QString& text,
                       const QColor& color,const int value)
{


    QTextCursor cursor = logWidget->textCursor();
    cursor.movePosition(QTextCursor::End);

    QTextCharFormat format;
    format.setForeground(color);

    cursor.insertText(text, format);
    cursor.insertBlock();   // 换行（可删）
    int realValue;
    if(gressBarStep == 0){
        realValue = static_cast<int>(double(value) / double(sumValue) * 100.0);
        gressBar->setValue(realValue);
    }else if(gressBarStep == 1){
        double tmp = double(designLib.numDesignPoints) / double(5 + designLib.numDesignPoints);
        realValue = static_cast<int>((tmp + (1.0 - tmp) * (double(value) / double(selectionArray.length()))) * 100.0);
        gressBar->setValue(realValue);
    }else{

    }

    logWidget->setTextCursor(cursor);
}
double airfoilExplorer::computeWeightedInverseCdScore(AirfoilLib lib)
{
    double score = 0.0;
    double wsum  = 0.0;

    for(int i = 0; i < designLib.numDesignPoints; ++i) {
        // 防止除零，直接用条件判断
        double cd_safe = 1;
        if(lib.badSolutions[i]){
           cd_safe = lib.cd[i];
        }else{
           double designAlpha = myMath::alphaFromClLinearFit(lib.alphaArray,lib.clArray,designLib.cl[i],-4,4);

           cd_safe = myMath::linearInterpolation(lib.alphaArray,lib.cdArray,designAlpha);

        }

        if(cd_safe < 1e-6) cd_safe = 1e-6;

        double s = 1.0 / cd_safe;   // 倒数评分
        score += designLib.weight[i] * s;
        wsum  += designLib.weight[i];
    }

    // 返回加权平均
    if(wsum > 0.0)
        return score / wsum;
    else
        return 0.0;
}
void airfoilExplorer::sortScoresWithIds(QVector<double>& score, QVector<int>& selection)
{
    int n = score.size();
    if(n != selection.size()) return;  // 安全检查

    // 简单选择排序（降序）
    for(int i = 0; i < n-1; ++i) {
        int maxIdx = i;
        for(int j = i+1; j < n; ++j) {
            if(score[j] > score[maxIdx])
                maxIdx = j;
        }
        if(maxIdx != i) {
            // 交换分数
            double tmpScore = score[i];
            score[i] = score[maxIdx];
            score[maxIdx] = tmpScore;

            // 同步交换 ID
            int tmpId = selection[i];
            selection[i] = selection[maxIdx];
            selection[maxIdx] = tmpId;
        }
    }
}
void airfoilExplorer::updateListView(const QVector<int>& choiceArray){
    QStringList list;
    for(int i = 0;i<choiceArray.length();i++){
        int index = choiceArray[i];
        list.append(airfoilLib[index].name);
    }
    listModel->setStringList(list);
}
void airfoilExplorer::drawThicknessAndFx(){
    QVector<double>x;
    QVector<double>y;
    for(int i = 0;i<selectionArray.length();i++){
        int index = selectionArray[i];
        x.append(airfoilLib[index].maxThickness);
        y.append(computeWeightedInverseCdScore(airfoilLib[index]));

    }
    //drawPoint(plotTL,QColor(0, 114, 189),x,y,false);
    graphTL = drawPoint(plotTL,QColor(0, 114, 189),x,y);


    plotTL->rescaleAxes();
    plotTL->replot();

}
void airfoilExplorer::drawCmAndFx(){
    QVector<double>x;
    QVector<double>y;
    for(int i = 0;i<selectionArray.length();i++){
        int index = selectionArray[i];
        x.append(airfoilLib[index].cm[0]);
        y.append(computeWeightedInverseCdScore(airfoilLib[index]));

    }
    //drawPoint(plotTR,QColor(217, 83, 25),x,y,false);
    graphTR = drawPoint(plotTR,QColor(217, 83, 25),x,y);

    plotTR->rescaleAxes();
    plotTR->replot();

}
void airfoilExplorer::drawClMaxAndFx(){
    QVector<double>x;
    QVector<double>y;
    myMath mathSolver;
    for(int i = 0;i<selectionArray.length();i++){
        int index = selectionArray[i];
        x.append(mathSolver.maxV(airfoilLib[index].clArray));
        y.append(computeWeightedInverseCdScore(airfoilLib[index]));

    }
    //drawPoint(plotBL,QColor(0, 158, 115),x,y,false);
    graphBL = drawPoint(plotBL,QColor(0, 158, 115),x,y);

    plotBL->rescaleAxes();
    plotBL->replot();

}
void airfoilExplorer::drawClSlopeAndFx(){
    QVector<double>x;
    QVector<double>y;

    for(int i = 0;i<selectionArray.length();i++){
        int index = selectionArray[i];
        x.append(computeLiftSlope(airfoilLib[index].alphaArray,airfoilLib[index].clArray));
        y.append(computeWeightedInverseCdScore(airfoilLib[index]));

    }
    //drawPoint(plotBR,QColor(126, 47, 142),x,y,false);
    graphBR = drawPoint(plotBR,QColor(126, 47, 142),x,y);
    plotBR->rescaleAxes();
    plotBR->replot();

}




double airfoilExplorer::computeLiftSlope(
    const QVector<double>& alpha,
    const QVector<double>& cl)
{
    int n = alpha.size();
    if (n != cl.size() || n < 3)
        return 0.0;

    // ---------- 计算局部斜率 ----------
    QVector<double> slope(n - 1);
    for (int i = 0; i < n - 1; ++i) {
        double dAlpha = alpha[i + 1] - alpha[i];
        if (std::abs(dAlpha) < 1e-12)
            slope[i] = 0.0;
        else
            slope[i] = (cl[i + 1] - cl[i]) / dAlpha;
    }

    // ---------- 找最稳定斜率区间 ----------
    double kRef = slope[0];
    int i0 = 0, i1 = 1;

    for (int i = 1; i < slope.size(); ++i) {
        if (std::abs(slope[i] - kRef) < 0.15 * std::abs(kRef)) {
            i1 = i + 1;
        } else {
            break;
        }
    }

    if (i1 - i0 < 2)
        return 0.0;

    // ---------- 最小二乘拟合 Cl = k*α + b ----------
    double Sx = 0, Sy = 0, Sxx = 0, Sxy = 0;
    int m = i1 - i0 + 1;

    for (int i = i0; i <= i1; ++i) {
        Sx  += alpha[i];
        Sy  += cl[i];
        Sxx += alpha[i] * alpha[i];
        Sxy += alpha[i] * cl[i];
    }

    double denom = m * Sxx - Sx * Sx;
    if (std::abs(denom) < 1e-12)
        return 0.0;

    double k = (m * Sxy - Sx * Sy) / denom;

    return k;   // 单位：Cl / deg
}
QCPGraph* airfoilExplorer::drawPoint(QCustomPlot *plot,
                                     const QColor &color,
                                     const QVector<double> &x,
                                     const QVector<double> &y)
{
    QCPGraph *graph = plot->addGraph();



    graph->setLineStyle(QCPGraph::lsNone);
    graph->setSelectable(QCP::stSingleData);



    // 普通点：空心圈
    QPen pen(color);
    pen.setWidthF(1.5);

    QCPScatterStyle normal(
        QCPScatterStyle::ssCircle,
        pen,
        Qt::NoBrush,
        8
    );
    graph->setScatterStyle(normal);

    // 选中态：同色放大加粗
    QPen selPen(color);
    selPen.setWidthF(2.5);

    QCPSelectionDecorator *decorator = graph->selectionDecorator();
    decorator->setScatterStyle(
        QCPScatterStyle(
            QCPScatterStyle::ssCircle,
            selPen,
            Qt::NoBrush,
            16
        )
    );
    decorator->setUsedScatterProperties(
        QCPScatterStyle::spPen | QCPScatterStyle::spSize
    );

    graph->setData(x, y,true);


    plot->setInteractions(QCP::iSelectPlottables |
                          QCP::iRangeDrag |
                          QCP::iRangeZoom);

    return graph;
}
void airfoilExplorer::onAnyPlotSelectionChanged()
{
    if (m_syncingSelection)
        return;

    QCustomPlot *srcPlot = qobject_cast<QCustomPlot*>(sender());
    if (!srcPlot)
        return;

    QCPGraph *srcGraph = nullptr;
    for (QCPAbstractPlottable *p : srcPlot->selectedPlottables()) {
        srcGraph = qobject_cast<QCPGraph*>(p);
        if (srcGraph)
            break;
    }
    if (!srcGraph)
        return;

    const QCPDataSelection sel = srcGraph->selection();
    if (sel.dataRangeCount() != 1)
        return;

    const QCPDataRange range = sel.dataRange(0);
    if (range.size() != 1)
        return;

    m_syncingSelection = true;
    syncSelection(range.begin());
    m_syncingSelection = false;


}



void airfoilExplorer::syncSelection(int dataIndex)
{
    if (dataIndex < 0 || dataIndex >= selectionArray.size())
        return;

    for (QCPGraph *g : {graphTL, graphTR, graphBL, graphBR}) {
        if (!g) continue;
        g->setSelection(QCPDataSelection(
            QCPDataRange(dataIndex, dataIndex + 1)
        ));
    }

    plotTL->replot(QCustomPlot::rpQueuedReplot);
    plotTR->replot(QCustomPlot::rpQueuedReplot);
    plotBL->replot(QCustomPlot::rpQueuedReplot);
    plotBR->replot(QCustomPlot::rpQueuedReplot);

    currentSelectedAirfoil = selectionArray[dataIndex];

    selectAirfoilRow(dataIndex);
}

void airfoilExplorer::selectAirfoilRow(int row)
{
    if (!airfoilList || !listModel) return;
    if (row < 0 || row >= listModel->rowCount()) return;

    QModelIndex index = listModel->index(row, 0);

    QItemSelectionModel *sel = airfoilList->selectionModel();
    sel->setCurrentIndex(
        index,
        QItemSelectionModel::ClearAndSelect | QItemSelectionModel::Rows
    );

    airfoilList->scrollTo(index, QListView::PositionAtCenter);
}

void airfoilExplorer::highlightCurve(int selectedIndex)
{
    if (resultCurveArray.isEmpty()) return;  // 假设你存放所有 QCPCurve 的 QVector

    for (int i = 0; i < resultCurveArray.size(); ++i)
    {
        QCPCurve* curve = resultCurveArray[i];
        if (!curve) continue;

        if (i == selectedIndex)
        {
            // 选中曲线：黑色 + 粗
            curve->setPen(QPen(Qt::black, 2.5));
        }
        else
        {
            // 未选中曲线：淡色 + 细
            QColor fadedColor = curve->pen().color();
            fadedColor.setAlpha(80); // 半透明
            curve->setPen(QPen(fadedColor, 1.0));
        }
    }

    resultPlot->replot(QCustomPlot::rpQueuedReplot);
}

// 清空单个 graph 的函数
void airfoilExplorer::clearGraph(QCustomPlot* plot, QCPGraph*& graphPtr)
{
    if (graphPtr)
    {
        plot->removeGraph(graphPtr); // 从 plot 中移除
        delete graphPtr;             // 删除 graph 对象
        graphPtr = nullptr;          // 指针置空
    }
}

void airfoilExplorer::highlightGraphPoint(int dataIndex)
{
    if (!graphTL || !graphTR || !graphBL || !graphBR) return;

    for (QCPGraph* g : {graphTL, graphTR, graphBL, graphBR})
    {
        if (!g) continue;

        // 创建数据选择对象，只选择一个点
        QCPDataSelection sel(QCPDataRange(dataIndex, dataIndex + 1));
        g->setSelection(sel);  // 选中该点
    }

    // 强制刷新 plot
    plotTL->replot(QCustomPlot::rpQueuedReplot);
    plotTR->replot(QCustomPlot::rpQueuedReplot);
    plotBL->replot(QCustomPlot::rpQueuedReplot);
    plotBR->replot(QCustomPlot::rpQueuedReplot);
}
