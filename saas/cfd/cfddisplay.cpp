#include "cfddisplay.h"
#include "saas/publicClass/myfile.h"
#include "saas/resultClass/pltreader.h"
#if defined(_MSC_VER) && (_MSC_VER >= 1600)
# pragma execution_character_set("utf-8")
#endif

cfdDisplay::cfdDisplay(QWidget *parent)
    : QWidget(parent){


    // 统一设置 UTF-8 编码



    //centralWidget = new QWidget();
    //applyGlobalStyle();


    //initialMenu();
    initialVTKWidget();



    initialTreeWidget();

    //initialToolButton();


    initialCFDWidget();
    initialStackWidget();
    initialCustomPlot();
    initialFloatingToolbar();

    initialTreeAction();







    login = new LoginWidget();


    funSetupWidget->switchToBox(funSetupWidget->globalSettingsBox);


    applyQtPalette(0);

    QString pathA = ":/icons/item/meshA.png";
    iconMeshA.addPixmap(QPixmap(pathA),QIcon::Normal,QIcon::On);
    QString pathB = ":/icons/item/meshB.png";
    iconMeshB.addPixmap(QPixmap(pathB),QIcon::Normal,QIcon::On);



    //structDefinition a;
    //a.rawGridData = funSetupWidget->getRawGridData();
    //interfaceEdit->appendPlainText(a.getRawGridText().join("\n"));

    QComboBox *combo1 = funSetupWidget->cloudPlotUI.colorCombo;
    QComboBox *combo2 = funSetupWidget->cloudPlotUI.variableCombo;
    QPushButton *button1 = funSetupWidget->cloudPlotUI.applyButton;
    QPushButton *button2 = funSetupWidget->airfoilInfoUI.selectBtn;
    QPushButton *button3 = funSetupWidget->gridUI.genMeshButton;
    connect(combo1, QOverload<int>::of(&QComboBox::currentIndexChanged),
            resultViewer,&flowViewer::switchColorMap);
    connect(combo2,
            QOverload<int>::of(&QComboBox::currentIndexChanged),
            this,
            [=](int index)
            {
                resultViewer->showScalarCloud(index);  // 云图显示
                funSetupWidget->setCloudPlotEdits(resultViewer->historyValueRange.min,
                                                  resultViewer->historyValueRange.max,resultViewer->historyValueRange.nums);

            });

    connect(button1,&QPushButton::clicked,this,&cfdDisplay::setCloudPlotLevel);
    connect(button2,&QPushButton::clicked,this,&cfdDisplay::selectAirfoil);
    connect(button3,&QPushButton::clicked,this,&cfdDisplay::GenerateMesh);
    connect(login,&LoginWidget::meshDownloadFinish,this,&cfdDisplay::displayAirfoilMesh);
    connect(login,&LoginWidget::jobLogStatus,this,&cfdDisplay::appendOutput);
    connect(resultViewer,&flowViewer::meshLogStatus,this,&cfdDisplay::appendOutput);

    //resultViewer->loadFlattenedGridAuto("C:/Users/13568/Desktop/build-PassWing-VS2019-Release/airfoil.p3d");

}
void cfdDisplay::applyQtPalette(int style)
{

    // --- 1. 同步 VTK 背景 ---
    if(resultViewer)
        resultViewer->setBackgroundStyle(style);


    // --- 2. Qt Palette + QSS ---
    QPalette p;
    QString treeQss;
    QString plaintextQss;
    QString toolbuttonQss;

    switch(style)
    {
    case 0: // 纯白
        p.setColor(QPalette::Window, QColor("#FFFFFF"));
        p.setColor(QPalette::Base, QColor("#FFFFFF"));
        p.setColor(QPalette::Button, QColor("#F0F0F0"));
        p.setColor(QPalette::Text, Qt::black);
        treeQss = loadQssFile(":/styles/treewidget/treewidget_white.qss");
        plaintextQss = loadQssFile(":/styles/plaintextedit/plaintextedit_white.qss");
        toolbuttonQss = loadQssFile(":/styles/toolbutton/toolbutton_white.qss");
        break;
    case 1: // 浅蓝
        p.setColor(QPalette::Window, QColor("#E3F2FD"));
        p.setColor(QPalette::Base, QColor("#FFFFFF"));
        p.setColor(QPalette::Button, QColor("#BBDEFB"));
        p.setColor(QPalette::Text, Qt::black);
        treeQss = loadQssFile(":/styles/treewidget/treewidget_light.qss");
        plaintextQss = loadQssFile(":/styles/plaintextedit/plaintextedit_light.qss");
        toolbuttonQss = loadQssFile(":/styles/toolbutton/toolbutton_light.qss");
        break;
    case 2: // Fluent 深灰
        p.setColor(QPalette::Window, QColor("#2F3542"));   // 主窗口背景
        p.setColor(QPalette::Base, QColor("#2B2F38"));     // 文本输入框 / TreeWidget 背景稍浅，提升对比
        p.setColor(QPalette::Button, QColor("#3C4352"));   // 按钮背景
        p.setColor(QPalette::Text, QColor("#E8ECF1"));     // 文字亮白，提高可读性
        treeQss = loadQssFile(":/styles/treewidget/treewidget_fluent.qss");
        plaintextQss = loadQssFile(":/styles/plaintextedit/plaintextedit_fluent.qss");
        toolbuttonQss = loadQssFile(":/styles/toolbutton/toolbutton_fluent.qss");
        break;


    case 3: // 深蓝
        p.setColor(QPalette::Window, QColor("#1E3A5F"));   // 主窗口背景
        p.setColor(QPalette::Base, QColor("#1E3A5F"));     // 文本背景同主色，避免白底 + 白字冲突
        p.setColor(QPalette::Button, QColor("#294E75"));   // 按钮背景
        p.setColor(QPalette::Text, QColor("#FFFFFF"));     // 白字可读
        treeQss = loadQssFile(":/styles/treewidget/treewidget_darkblue.qss");
        plaintextQss = loadQssFile(":/styles/plaintextedit/plaintextedit_darkblue.qss");
        toolbuttonQss = loadQssFile(":/styles/toolbutton/toolbutton_darkblue.qss");
        break;

    default: // 默认浅蓝
        p.setColor(QPalette::Window, QColor("#E3F2FD"));
        p.setColor(QPalette::Base, QColor("#FFFFFF"));
        p.setColor(QPalette::Button, QColor("#BBDEFB"));
        p.setColor(QPalette::Text, Qt::black);
        treeQss = loadQssFile(":/styles/treewidget/treewidget_light.qss");
        plaintextQss = loadQssFile(":/styles/plaintextedit/plaintextedit_light.qss");
        break;
    }

    //qApp->setPalette(p);
    this->setPalette(p);
    this->setAutoFillBackground(true);
    if(cfdTreeWidget) cfdTreeWidget->setStyleSheet(treeQss);
    if(interfaceEdit) interfaceEdit->setStyleSheet(plaintextQss);
    //if(addProjectToolButton) addProjectToolButton->setStyleSheet(toolbuttonQss);
    //if(importProjectToolButton) importProjectToolButton->setStyleSheet(toolbuttonQss);
    //if(saveProjectToolButton) saveProjectToolButton->setStyleSheet(toolbuttonQss);

}

/*
void cfdDisplay::initialMenu(){
    menuBar = new QMenuBar(this);
    fileMenu = new QMenu(tr("文件"));
    settingMenu = new QMenu(tr("设置"));
    aboutMenu = new QMenu(tr("更多"));

    QMenu* subMenu1 = new QMenu(tr("语言"), settingMenu);
    QAction* zhAction = subMenu1->addAction("中文");
    QAction* enAction = subMenu1->addAction("English");

    QMenu* subMenu2 = new QMenu(tr("风格"), settingMenu);
    QAction* styleAction1 = subMenu2->addAction(tr("风格一"));
    QAction* styleAction2 = subMenu2->addAction(tr("风格二"));
    QAction* styleAction3 = subMenu2->addAction(tr("风格三"));
    QAction* styleAction4 = subMenu2->addAction(tr("风格四"));


    settingMenu->addMenu(subMenu1);
    settingMenu->addMenu(subMenu2);


    menuBar->addMenu(fileMenu);
    menuBar->addMenu(settingMenu);
    menuBar->addMenu(aboutMenu);
    setMenuBar(menuBar);

    connect(zhAction, &QAction::triggered, this, [=]() {
        QSettings settings("SmartCombatFlow", "Config");
        settings.setValue("Language", "zh_CN");
        QMessageBox::information(this, "提示", "语言将在重启后生效。");
    });

    connect(enAction, &QAction::triggered, this, [=]() {
        QSettings settings("SmartCombatFlow", "Config");
        settings.setValue("Language", "en_US");
        QMessageBox::information(this, "Notice", "Language will take effect after restart.");
    });

    connect(styleAction1, &QAction::triggered, this, [this]() { setTheme(0); });
    connect(styleAction2, &QAction::triggered, this, [this]() { setTheme(1); });
    connect(styleAction3, &QAction::triggered, this, [this]() { setTheme(2); });
    connect(styleAction4, &QAction::triggered, this, [this]() { setTheme(3); });





}
*/
void cfdDisplay::initialCFDWidget()
{
    // ====== 0. 创建中央容器 ======
    mainLayout = new QVBoxLayout(this);

    // ====== 1. 中间工作区（QSplitter） ======
    listWidget = new QWidget();
    plotContainer = new QWidget();
    //setupWidget = new QWidget();
    listLayout = new QVBoxLayout(listWidget);
    interfaceEdit = new QPlainTextEdit();

    interfaceEdit->setUndoRedoEnabled(false);
    interfaceEdit->setWordWrapMode(QTextOption::NoWrap);
    interfaceEdit->setFont(QFont("Consolas", 15));
    interfaceEdit->setPlainText(">>> ");
    promptPosition = interfaceEdit->toPlainText().size();
    interfaceEdit->installEventFilter(this);
    interfaceEdit->setMaximumHeight(350);




    funSetupWidget = new structWidget();
    listWidget->setMinimumWidth(350);



    // ========== 内容堆叠区 ==========
    QStackedWidget* stacked = new QStackedWidget(listWidget);
    stacked->addWidget(funSetupWidget);



    listLayout->addWidget(stacked);
    listLayout->setContentsMargins(0, 0, 0, 0);
    listLayout->setSpacing(0);

    // ====== 2. 构建分割器结构 ======
    //viewSplitter = new QSplitter(Qt::Horizontal,plotContainer);
    QSplitter *splitterA = new QSplitter(Qt::Horizontal, this);
    QSplitter *splitterB = new QSplitter(Qt::Horizontal, splitterA);
    QSplitter *splitterC = new QSplitter(Qt::Vertical, splitterA);






    splitterB->addWidget(cfdTreeWidget);
    splitterB->addWidget(listWidget);

    splitterA->addWidget(splitterB);
    splitterA->addWidget(splitterC);

    splitterC->addWidget(plotContainer);
    //splitterC->addWidget(viewSplitter);
    splitterC->addWidget(interfaceEdit);

    // ====== 3. 设置 Splitter 样式与参数 ======
    splitterA->setHandleWidth(6);
    splitterA->setChildrenCollapsible(false);
    splitterA->setStretchFactor(1, 1);
    splitterA->setSizes({250, 600});


    splitterB->setHandleWidth(6);
    splitterB->setChildrenCollapsible(false);
    splitterB->setSizes({250, 600});


    // ====== 4. 底部状态区 ======

    progressBar = new QProgressBar();
    progressBar->setRange(0, 100);
    progressBar->setTextVisible(true);
    stopButton = new QPushButton(tr("停止"));

    QHBoxLayout *hLayoutB = new QHBoxLayout();
    hLayoutB->addWidget(progressBar);
    hLayoutB->addWidget(stopButton);

    // ====== 5. 总体布局组装 ======
    //mainLayout->addLayout(toolButtonLayout);
    mainLayout->addWidget(splitterA);
    mainLayout->addLayout(hLayoutB);

    // ====== 6. 应用于 QMainWindow ======
    //this->setCentralWidget(centralWidget);

    connect(this, &cfdDisplay::commandEntered, this, [=](const QString &cmd) {

        handleCommand(cmd);
        promptPosition = interfaceEdit->toPlainText().size();
    });
    connect(funSetupWidget, &structWidget::widgetTriggered, this, [=](const QString &cmd) {

        handleCommand(cmd);
        promptPosition = interfaceEdit->toPlainText().size();
        inputArray[projectChoiceIndex] = funSetupWidget->settingData;
    });
    connect(funSetupWidget->startAnalyseButton,&QPushButton::clicked,this,&cfdDisplay::startAnalyse);

    progressBar->setVisible(false);
    stopButton->setVisible(false);
    connect(stopButton,&QPushButton::clicked,this,&cfdDisplay::onStopProjectClicked);

}

/*
void cfdDisplay::initialToolButton(){
    // ====== 1. 顶部按钮区域 ======
    importProjectToolButton = new QToolButton(this);
    addProjectToolButton = new QToolButton(this);
    saveProjectToolButton   = new QToolButton(this);

    importProjectToolButton->setEnabled(false);
    saveProjectToolButton->setEnabled(false);
    importProjectToolButton->setObjectName("toolButton");
    addProjectToolButton->setObjectName("toolButton");
    saveProjectToolButton->setObjectName("toolButton");
    toolButtonLayout = new QHBoxLayout();



    importProjectToolButton->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    addProjectToolButton->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    saveProjectToolButton->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);





    importProjectToolButton->setIcon(QIcon(":/icons/button/import.svg"));
    importProjectToolButton->setText(tr("Import"));
    importProjectToolButton->setIconSize(QSize(40, 40));  // 图标大小

    addProjectToolButton->setIcon(QIcon(":/icons/button/add.svg"));
    addProjectToolButton->setText(tr("Add"));
    addProjectToolButton->setIconSize(QSize(40, 40));  // 图标大小

    saveProjectToolButton->setIcon(QIcon(":/icons/button/save.svg"));
    saveProjectToolButton->setText(tr("Save"));
    saveProjectToolButton->setIconSize(QSize(40, 40));  // 图标大小

    importProjectToolButton->setFixedSize(80,80);
    addProjectToolButton->setFixedSize(80,80);
    saveProjectToolButton->setFixedSize(80,80);
    toolButtonLayout->addWidget(importProjectToolButton);
    toolButtonLayout->addWidget(addProjectToolButton);
    toolButtonLayout->addWidget(saveProjectToolButton);


    toolButtonLayout->addStretch();                  // 把空白推到右边

    toolButtonLayout->setAlignment(Qt::AlignLeft);   // 整体靠左
    toolButtonLayout->setContentsMargins(0, 0, 0, 0); // 去除外边距
    toolButtonLayout->setSpacing(10);

    connect(addProjectToolButton,&QToolButton::clicked,this,&cfdDisplay::onAddProjectClicked);
    connect(importProjectToolButton,&QToolButton::clicked,this,&cfdDisplay::importPltData);
}
*/
void cfdDisplay::initialTreeWidget(){
    cfdTreeWidget = new QTreeWidget();
    //cfdTreeWidget->setItemDelegate(new fullWidgetDelegate(cfdTreeWidget));

    cfdTreeWidget->setContextMenuPolicy(Qt::CustomContextMenu);
    cfdTreeWidget->setColumnCount(1);
    cfdTreeWidget->setHeaderHidden(true);

    // ===== 顶部搜索框 =====
    //QTreeWidgetItem *inputItem = new QTreeWidgetItem(cfdTreeWidget);
    //cfdTreeWidget->addTopLevelItem(inputItem);
    //inputItem->setFlags(inputItem->flags() & ~Qt::ItemIsSelectable);

    //searchEdit = new QLineEdit;
    //searchEdit->setObjectName("SearchBox");
    //searchEdit->setFixedHeight(60);
    //searchEdit->setFixedWidth(400);

    // ===== 空状态下的“添加项目”按钮 =====
    addProjectButton = new QPushButton("+", cfdTreeWidget);
    addProjectButton->setObjectName("emptyAddButton");
    addProjectButton->setFixedSize(200, 200);


    //searchEdit->setPlaceholderText("🔍 搜索...");
    //cfdTreeWidget->setItemWidget(inputItem, 0, searchEdit);

    cfdTreeWidget->setMinimumWidth(250);
    cfdTreeWidget->header()->hide();



    // 加载 QSS

    //QFile qss1(":/styles/treeWidget.qss");
    //QFile qss2(":/styles/inputBox.qss");
    QFile qss3(":/styles/emptyAddButton.qss");



    //if (qss1.open(QFile::ReadOnly)) {
        //QString style = QString::fromUtf8(qss1.readAll());
        //cfdTreeWidget->setStyleSheet(style);
    //}

    //if (qss2.open(QFile::ReadOnly)) {
        //QString style = QString::fromUtf8(qss2.readAll());
        //searchEdit->setStyleSheet(style);
    //}
    if (qss3.open(QFile::ReadOnly)) {
        QString style = QString::fromUtf8(qss3.readAll());
        addProjectButton->setStyleSheet(style);
    }





    // 居中放置
    QVBoxLayout *treeLayout = new QVBoxLayout(cfdTreeWidget);
    treeLayout->addStretch();
    treeLayout->addWidget(addProjectButton, 0, Qt::AlignCenter);
    treeLayout->addStretch();
    treeLayout->setContentsMargins(0, 0, 0, 0);
    connect(addProjectButton,&QPushButton::clicked,this,&cfdDisplay::onAddProjectClicked);

    connect(cfdTreeWidget, &QTreeWidget::customContextMenuRequested,
            this, &cfdDisplay::onTreeWidgetContextMenu);

    connect(cfdTreeWidget, &QTreeWidget::itemClicked, this, &cfdDisplay::changeProjectDisplay);
    connect(cfdTreeWidget, &QTreeWidget::itemClicked, this, &cfdDisplay::showFlowModelSetup);
    connect(cfdTreeWidget, &QTreeWidget::itemClicked, this, &cfdDisplay::showVolumeOutputSetting);
    connect(cfdTreeWidget, &QTreeWidget::itemClicked, this, &cfdDisplay::showBoundaryOutputSetting);
    connect(cfdTreeWidget, &QTreeWidget::itemClicked, this, &cfdDisplay::showReferenceSetup);
    connect(cfdTreeWidget, &QTreeWidget::itemClicked, this, &cfdDisplay::showEquationSetup);
    connect(cfdTreeWidget, &QTreeWidget::itemClicked, this, &cfdDisplay::showForceMomentSetup);
    connect(cfdTreeWidget, &QTreeWidget::itemClicked, this, &cfdDisplay::showRawGridSetup);
    connect(cfdTreeWidget, &QTreeWidget::itemClicked, this, &cfdDisplay::showInviscidFluxSetup);
    //connect(cfdTreeWidget, &QTreeWidget::itemClicked, this, &cfdDisplay::showSpalartSetup);
    connect(cfdTreeWidget, &QTreeWidget::itemClicked, this, &cfdDisplay::showCodeRunControlSetup);
    connect(cfdTreeWidget, &QTreeWidget::itemClicked, this, &cfdDisplay::showNonlinearSolverSetup);
    connect(cfdTreeWidget, &QTreeWidget::itemClicked, this, &cfdDisplay::showLinearSolverSetup);
    connect(cfdTreeWidget, &QTreeWidget::itemClicked, this, &cfdDisplay::showCloudPlotSetup);
    connect(cfdTreeWidget, &QTreeWidget::itemClicked, this, &cfdDisplay::showAirfoil);


}
void cfdDisplay::initialStackWidget(){
    stackedPlot = new QStackedWidget();
    stackedPlot->addWidget(resultViewer);
}
void cfdDisplay::initialCustomPlot()
{

    // 📐 主布局

    stackedLayout = new QHBoxLayout(plotContainer);
    QVBoxLayout *vLayout = new QVBoxLayout();
    // 🧭 初始化两个 plot
    plotViewA = new QCustomPlot();
    plotViewB = new QCustomPlot();

    setupResidualPlot(plotViewA);
    setupLiftDragPlot(plotViewB);

    // 🧱 使用 QStackedWidget 来切换

    stackedPlot->addWidget(plotViewA);
    stackedPlot->addWidget(plotViewB);

    // 🧭 创建切换按钮
    QPushButton *btnResidual = new QPushButton();
    QPushButton *btnLiftDrag = new QPushButton();
    QPushButton *btn3D = new QPushButton();
    QFile qss1(":/styles/changeListButton.qss");

    if (qss1.open(QFile::ReadOnly)) {
        QString style = QString::fromUtf8(qss1.readAll());
        btn3D->setStyleSheet(style);
        btnResidual->setStyleSheet(style);
        btnLiftDrag->setStyleSheet(style);

    }

    btn3D->setText(tr("3D"));
    btnResidual->setText(tr("残差"));
    btnLiftDrag->setText(tr("气动系数"));

    btn3D->setCheckable(true);
    btnResidual->setCheckable(true);
    btnLiftDrag->setCheckable(true);

    btn3D->setChecked(true);

    // 按钮组控制切换
    QButtonGroup *group = new QButtonGroup(this);
    group->addButton(btn3D, 0);
    group->addButton(btnResidual, 1);
    group->addButton(btnLiftDrag, 2);

    connect(group, QOverload<int>::of(&QButtonGroup::buttonClicked),
            this, [=](int id) {
                stackedPlot->setCurrentIndex(id);

                if (id == 0) {
                    toolbarContainer->show();
                } else {
                    toolbarContainer->hide();
                }
            });


    // 🎨 工具条布局
    QHBoxLayout *btnLayout = new QHBoxLayout();
    //btnLayout->addStretch();
    btnLayout->addWidget(btn3D);
    btnLayout->addWidget(btnResidual);
    btnLayout->addWidget(btnLiftDrag);
    btnLayout->addStretch();
    btnLayout->setContentsMargins(0, 0, 0, 0);
    btnLayout->setSpacing(0);



    vLayout->addLayout(btnLayout);
    vLayout->addWidget(stackedPlot);
    stackedLayout->addLayout(vLayout);
    //vLayout->setContentsMargins(10, 8, 10, 8);

}
void cfdDisplay::setupResidualPlot(QCustomPlot *plot)
{
    // 🎨 基本外观
    plot->setBackground(Qt::white);
    plot->axisRect()->setBackground(Qt::white);
    plot->axisRect()->setupFullAxesBox(true);
    plot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);

    // 坐标轴样式
    QPen axisPen(QColor("#1E3A5F"));
    axisPen.setWidthF(2.0);
    for (auto *axis : plot->axisRect()->axes()) {
        axis->setBasePen(axisPen);
        axis->setTickPen(axisPen);
        axis->grid()->setVisible(false);
        //axis->grid()->setPen(QPen(QColor("#E0E0E0"))); // ⚪ 淡灰网格
        axis->setTickLabelFont(QFont("Microsoft YaHei", 9));
        axis->setLabelFont(QFont("Microsoft YaHei", 10, QFont::Bold));
    }

    // 📉 X轴
    plot->xAxis->setLabel("Iteration");

    // 📈 Y轴：对数坐标
    plot->yAxis->setLabel("Residual");
    plot->yAxis->setScaleType(QCPAxis::stLogarithmic);
    QSharedPointer<QCPAxisTickerLog> logTicker(new QCPAxisTickerLog);
    logTicker->setLogBase(10);
    logTicker->setSubTickCount(0);
    plot->yAxis->setTicker(logTicker);
    plot->yAxis->setNumberFormat("eb");
    plot->yAxis->setNumberPrecision(0);
    plot->yAxis->setRange(1e-10, 1e1);

    // 图例
    plot->legend->setVisible(true);
    plot->legend->setFont(QFont("Microsoft YaHei", 9));
    plot->legend->setBrush(QColor(255, 255, 255, 230));
    plot->legend->setBorderPen(QPen(QColor(210, 210, 210)));

    // =========================
    // 📊 各个残差曲线
    // =========================

    // 🩵 Density 系列
    densityRMSGraph = plot->addGraph();
    densityRMSGraph->setName("density_RMS");
    densityRMSGraph->setPen(QPen(QColor("#1E90FF"), 1.8));

    densityMAXGraph = plot->addGraph();
    densityMAXGraph->setName("density_MAX");
    densityMAXGraph->setPen(QPen(QColor("#4682B4"), 1.6, Qt::DashLine));

    densityXGraph = plot->addGraph();
    densityXGraph->setName("density_X");
    densityXGraph->setPen(QPen(QColor("#6CA6CD"), 1.5, Qt::DotLine));

    densityYGraph = plot->addGraph();
    densityYGraph->setName("density_Y");
    densityYGraph->setPen(QPen(QColor("#87CEEB"), 1.5, Qt::DashDotLine));

    densityZGraph = plot->addGraph();
    densityZGraph->setName("density_Z");
    densityZGraph->setPen(QPen(QColor("#B0E0E6"), 1.5, Qt::DashDotDotLine));

    // 💗 Turbulence 系列
    turbRMSGraph = plot->addGraph();
    turbRMSGraph->setName("turb_RMS");
    turbRMSGraph->setPen(QPen(QColor("#E91E63"), 1.8));

    turbMAXGraph = plot->addGraph();
    turbMAXGraph->setName("turb_MAX");
    turbMAXGraph->setPen(QPen(QColor("#C71585"), 1.6, Qt::DashLine));

    turbXGraph = plot->addGraph();
    turbXGraph->setName("turb_X");
    turbXGraph->setPen(QPen(QColor("#DB7093"), 1.5, Qt::DotLine));

    turbYGraph = plot->addGraph();
    turbYGraph->setName("turb_Y");
    turbYGraph->setPen(QPen(QColor("#F08080"), 1.5, Qt::DashDotLine));

    turbZGraph = plot->addGraph();
    turbZGraph->setName("turb_Z");
    turbZGraph->setPen(QPen(QColor("#FFC0CB"), 1.5, Qt::DashDotDotLine));
}



void cfdDisplay::setupLiftDragPlot(QCustomPlot *plot)
{
    // 🎨 背景
    plot->setBackground(Qt::white);
    plot->axisRect()->setBackground(Qt::white);
    plot->axisRect()->setupFullAxesBox(true);
    plot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);

    // 🧭 坐标轴样式
    QPen axisPen(QColor("#1E3A5F"));
    axisPen.setWidthF(2.0);
    for (auto *axis : plot->axisRect()->axes()) {
        axis->setBasePen(axisPen);
        axis->setTickPen(axisPen);
        axis->setTickLabelFont(QFont("Microsoft YaHei", 9));
        axis->setLabelFont(QFont("Microsoft YaHei", 10, QFont::Bold));
        axis->grid()->setVisible(false);
    }

    // 📉 启用右轴
    plot->yAxis2->setVisible(true);
    plot->yAxis2->setTickLabels(true);
    plot->yAxis2->setTicks(true);

    // 🧮 坐标轴标签
    plot->xAxis->setLabel("Iteration");
    plot->yAxis->setLabel("𝐶𝐿");
    plot->yAxis2->setLabel("𝐶𝐷");

    // 🧾 坐标轴颜色与曲线同步
    QPen liftColor(QColor("#E91E63")); // 粉色
    QPen dragColor(QColor("#2196F3")); // 蓝色
    liftColor.setWidth(2);
    dragColor.setWidth(2);
    plot->yAxis->setLabelColor(liftColor.color());
    plot->yAxis->setTickLabelColor(liftColor.color());
    plot->yAxis2->setLabelColor(dragColor.color());
    plot->yAxis2->setTickLabelColor(dragColor.color());

    // 🔢 范围（可根据数据动态调整）
    plot->yAxis->setRange(-1, 5);
    plot->yAxis2->setRange(0, 2);

    // 🧩 图例
    plot->legend->setVisible(true);
    plot->legend->setFont(QFont("Microsoft YaHei", 9));
    plot->legend->setBrush(QColor(255, 255, 255, 230));
    plot->legend->setBorderPen(QPen(QColor(200, 200, 200)));

    // 🟥 Lift 曲线（左轴）
    liftGraph = plot->addGraph(plot->xAxis, plot->yAxis);
    liftGraph->setName("𝐶𝐿");
    liftGraph->setPen(liftColor);
    liftGraph->setLineStyle(QCPGraph::lsLine);
    liftGraph->setScatterStyle(QCPScatterStyle::ssNone);

    // 🟦 Drag 曲线（右轴）
    dragGraph = plot->addGraph(plot->xAxis, plot->yAxis2);
    dragGraph->setName("𝐶D");
    dragGraph->setPen(dragColor);
    dragGraph->setLineStyle(QCPGraph::lsLine);
    dragGraph->setScatterStyle(QCPScatterStyle::ssNone);

    // 🧱 边距调整，防止右侧刻度被遮挡
    plot->plotLayout()->setMargins(QMargins(8, 5, 25, 8));
}




void cfdDisplay::initialVTKWidget(){


     //viewer = new ugridViewer(glWidget);
     resultViewer = new flowViewer();
     //数据的读入

     //pltReader reader(R"(C:\Users\13568\Desktop\FUN3D\fun3d_part1_tec_volume.dat)");




}
void cfdDisplay::initialTreeAction(){
    // === 初始化右键菜单 ===
    treeContextMenu = new QMenu();
    treeContextMenu->setObjectName("treeContextMenu");  // 可在 QSS 里美化

    copyAction = new QAction(QIcon(":/icons/item/copy.svg"), tr("复制项目"));
    renameAction = new QAction(QIcon(":/icons/item/rename.svg"), tr("重命名项目"));
    deleteAction = new QAction(QIcon(":/icons/item/delete.svg"), tr("删除项目"));


    // 添加到菜单
    treeContextMenu->addAction(copyAction);
    treeContextMenu->addAction(deleteAction);
    treeContextMenu->addSeparator();
    treeContextMenu->addAction(renameAction);

}
void cfdDisplay::initialFloatingToolbar()
{

    toolbarContainer = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(toolbarContainer);

    // === 新增：透明 spacer，把整个工具栏往下压 ===
    QWidget* spacer = new QWidget();
    spacer->setFixedHeight(30); // 工具栏整体下移多少，可调
    spacer->setStyleSheet("background: transparent;");
    spacer->setAttribute(Qt::WA_TransparentForMouseEvents, true);
    layout->addWidget(spacer);
    // === 新增结束 ===

    layout->setContentsMargins(0,5,0,5);
    layout->setSpacing(10);

    QFrame *line1 = new QFrame();
    line1->setFrameShape(QFrame::HLine);    // 设置为水平线
    line1->setFrameShadow(QFrame::Sunken);  // 设置阴影效果（可选）
    line1->setStyleSheet("background-color: #cccccc; height: 1px;"); // 自定义样式
    layout->addWidget(line1);

    btnLeft = new QPushButton();
    btnLeft->setToolTip("Left View");
    layout->addWidget(btnLeft);

    btnTop = new QPushButton();
    btnTop->setToolTip("Top View");
    layout->addWidget(btnTop);

    btnFront = new QPushButton();
    btnFront->setToolTip("Front View");
    layout->addWidget(btnFront);

    btnBest = new QPushButton();
    btnBest->setToolTip("Best View");
    layout->addWidget(btnBest);

    // 创建横线（QFrame实现）
    QFrame *line2 = new QFrame();
    line2->setFrameShape(QFrame::HLine);    // 设置为水平线
    line2->setFrameShadow(QFrame::Sunken);  // 设置阴影效果（可选）
    line2->setStyleSheet("background-color: #cccccc; height: 1px;"); // 自定义样式
    layout->addWidget(line2);

    btnColorBar = new QPushButton();
    btnColorBar->setToolTip(tr("Color Map"));
    layout->addWidget(btnColorBar);

    btnAxes = new QPushButton();
    btnAxes->setToolTip(tr("Axes"));
    layout->addWidget(btnAxes);

    btnColorBar->setCheckable(true);    // 允许切换
    btnColorBar->setChecked(true);      // 默认显示 ColorBar
    btnAxes->setCheckable(true);    // 允许切换
    btnAxes->setChecked(true);      // 默认显示 ColorBar



    QFrame *line3 = new QFrame();
    line3->setFrameShape(QFrame::HLine);    // 设置为水平线
    line3->setFrameShadow(QFrame::Sunken);  // 设置阴影效果（可选）
    line3->setStyleSheet("background-color: #cccccc; height: 1px;"); // 自定义样式
    layout->addWidget(line3);


    btnSurface = new QPushButton();
    btnSurface->setToolTip(tr("Surface"));
    layout->addWidget(btnSurface);

    btnMesh = new QPushButton();
    btnMesh->setToolTip(tr("Mesh"));
    layout->addWidget(btnMesh);



    btnMesh->setCheckable(true);
    btnSurface->setCheckable(true);
    btnSurface->setChecked(true);     // 默认选中

    QButtonGroup* meshGroup = new QButtonGroup(this);
    meshGroup->setExclusive(true);       // 互斥
    meshGroup->addButton(btnMesh, 0);    // id 0 = 网格
    meshGroup->addButton(btnSurface, 1); // id 1 = 表面




    btnLeft->setIcon(QIcon(":/icons/vtkviewer/xy.png"));
    btnTop->setIcon(QIcon(":/icons/vtkviewer/yz.png"));
    btnFront->setIcon(QIcon(":/icons/vtkviewer/zx.png"));
    btnBest->setIcon(QIcon(":/icons/vtkviewer/xyz.png"));
    btnColorBar->setIcon(QIcon(":/icons/vtkviewer/barShow.png"));
    btnAxes->setIcon(QIcon(":/icons/vtkviewer/axesShow.png"));
    btnMesh->setIcon(QIcon(":/icons/vtkviewer/mesh.png"));
    btnSurface->setIcon(QIcon(":/icons/vtkviewer/surface.png"));


    QSize size1(45,45);
    QSize size2(35,35);
    btnLeft->setFixedSize(size1);
    btnTop->setFixedSize(size1);
    btnFront->setFixedSize(size1);
    btnBest->setFixedSize(size1);
    btnColorBar->setFixedSize(size1);
    btnAxes->setFixedSize(size1);
    btnMesh->setFixedSize(size1);
    btnSurface->setFixedSize(size1);

    btnLeft->setIconSize(size2);
    btnTop->setIconSize(size2);
    btnFront->setIconSize(size2);
    btnBest->setIconSize(size2);
    btnColorBar->setIconSize(size2);
    btnAxes->setIconSize(size2);
    btnMesh->setIconSize(size2);
    btnSurface->setIconSize(size2);

    layout->addStretch();

    stackedLayout->addWidget(toolbarContainer);

    connect(btnLeft, &QToolButton::clicked, resultViewer, &flowViewer::leftView);
    connect(btnTop, &QToolButton::clicked, resultViewer, &flowViewer::topView);
    connect(btnFront, &QToolButton::clicked, resultViewer, &flowViewer::frontView);
    connect(btnBest, &QToolButton::clicked, resultViewer, &flowViewer::bestView);
    //connect(btnColorBar, &QToolButton::clicked, resultViewer, &flowViewer::toggleScalarBar);
    //connect(btnAxes, &QToolButton::clicked, resultViewer, &flowViewer::toggleAxesWidget);

    connect(btnColorBar, &QToolButton::clicked, this, [=](bool checked){
        if (checked) {
            // 显示 ColorBar
            btnColorBar->setIcon(QIcon(":/icons/vtkviewer/barShow.png"));
            resultViewer->toggleScalarBar();
        } else {
            // 隐藏 ColorBar
            btnColorBar->setIcon(QIcon(":/icons/vtkviewer/barHide.png"));
            resultViewer->toggleScalarBar();
        }

    });
    connect(btnAxes, &QToolButton::clicked, this, [=](bool checked){
        if (checked) {
            // 显示 ColorBar
            btnAxes->setIcon(QIcon(":/icons/vtkviewer/axesShow.png"));
            resultViewer->toggleAxesWidget();
        } else {
            // 隐藏 ColorBar
            btnAxes->setIcon(QIcon(":/icons/vtkviewer/axesHide.png"));
            resultViewer->toggleAxesWidget();
        }

    });

    connect(meshGroup, QOverload<int>::of(&QButtonGroup::buttonClicked),
            this, [=](int id){

        // ----------------------------
        // 保证互斥按钮永不取消选中
        // ----------------------------
        meshGroup->button(id)->setChecked(true);

        switch (id)
        {
            case 0:
                resultViewer->setMeshDisplayMode(2); // wireframe
                break;
            case 1:
                resultViewer->setMeshDisplayMode(0); // surface
                break;
        }
    });


}

void cfdDisplay::addProject(const QString name,const QString runDir){
    if (!funSetupWidget->globalBox->isEnabled())
        funSetupWidget->globalBox->setEnabled(true);
    QString path1 = ":/icons/item/project.svg";
    QIcon icon1 ;
    icon1.addPixmap(QPixmap(path1),QIcon::Normal,QIcon::On);

    QString path2 = ":/icons/item/mesh.png";
    QIcon icon2 ;
    icon2.addPixmap(QPixmap(path2),QIcon::Normal,QIcon::On);

    QString path3 = ":/icons/item/value.png";
    QIcon icon3 ;
    icon3.addPixmap(QPixmap(path3),QIcon::Normal,QIcon::On);

    QString path4 = ":/icons/item/ref.png";
    QIcon icon4 ;
    icon4.addPixmap(QPixmap(path4),QIcon::Normal,QIcon::On);

    QString path5 = ":/icons/item/volume.png";
    QIcon icon5 ;
    icon5.addPixmap(QPixmap(path5),QIcon::Normal,QIcon::On);

    QString path6 = ":/icons/item/bOutput.png";
    QIcon icon6 ;
    icon6.addPixmap(QPixmap(path6),QIcon::Normal,QIcon::On);

    QString path7 = ":/icons/item/control.png";
    QIcon icon7 ;
    icon7.addPixmap(QPixmap(path7),QIcon::Normal,QIcon::On);

    QString path8 = ":/icons/item/nonSolver.png";
    QIcon icon8 ;
    icon8.addPixmap(QPixmap(path8),QIcon::Normal,QIcon::On);

    QString path9 = ":/icons/item/solver.png";
    QIcon icon9 ;
    icon9.addPixmap(QPixmap(path9),QIcon::Normal,QIcon::On);

    QString path10 = ":/icons/item/run.png";
    QIcon icon10 ;
    icon10.addPixmap(QPixmap(path10),QIcon::Normal,QIcon::On);

    QString path11 = ":/icons/item/turbulent.png";
    QIcon icon11 ;
    icon11.addPixmap(QPixmap(path11),QIcon::Normal,QIcon::On);

    QString path12 = ":/icons/item/inviscid.png";
    QIcon icon12 ;
    icon12.addPixmap(QPixmap(path12),QIcon::Normal,QIcon::On);

    QString path13 = ":/icons/item/contour.png";
    QIcon icon13 ;
    icon13.addPixmap(QPixmap(path13),QIcon::Normal,QIcon::On);

    QString path14 = ":/icons/item/contourSlice.png";
    QIcon icon14 ;
    icon14.addPixmap(QPixmap(path14),QIcon::Normal,QIcon::On);

    QString path15 = ":/icons/item/streamLine.png";
    QIcon icon15 ;
    icon15.addPixmap(QPixmap(path15),QIcon::Normal,QIcon::On);

    QString path16 = ":/icons/item/vorticity.png";
    QIcon icon16 ;
    icon16.addPixmap(QPixmap(path16),QIcon::Normal,QIcon::On);

    QString path17 = ":/icons/item/airfoil.png";
    QIcon icon17 ;
    icon17.addPixmap(QPixmap(path17),QIcon::Normal,QIcon::On);


    QString path18 = ":/icons/item/wing.png";
    QIcon icon18 ;
    icon18.addPixmap(QPixmap(path18),QIcon::Normal,QIcon::On);


    QString path19 = ":/icons/item/airplane.png";
    QIcon icon19 ;
    icon19.addPixmap(QPixmap(path19),QIcon::Normal,QIcon::On);


    QString path20 = ":/icons/item/propeller.png";
    QIcon icon20 ;
    icon20.addPixmap(QPixmap(path20),QIcon::Normal,QIcon::On);




    //项目名
    QTreeWidgetItem *proItem = new QTreeWidgetItem(cfdTreeWidget,QStringList(QString(name)));
    proItem->setData(0,Qt::UserRole,QVariant(QString::number(projectIndex)));
    proItem->setIcon(0,icon1);
    //A设置
    QTreeWidgetItem *geomteryItem = new QTreeWidgetItem(proItem,QStringList(tr("模型设置")));
    geomteryItem->setData(0,Qt::UserRole,"geoSetting");

    //B1翼型
    QTreeWidgetItem *airfoilItem = new QTreeWidgetItem(geomteryItem,QStringList(tr("翼型")));
    airfoilItem->setData(0,Qt::UserRole,QVariant("childAirfoil" + QString::number(projectIndex)));
    airfoilItem->setIcon(0,icon17);
    for(int i = 0;i<AirfoilNameArray.length();i++){
        QTreeWidgetItem *childAirfoilTmp = new QTreeWidgetItem(airfoilItem,QStringList(AirfoilNameArray[i]));
        childAirfoilTmp->setData(0,Qt::UserRole,QVariant("airfoil" + QString::number(i)));
        //childAirfoilTmp->setIcon(0,icon1);
    }
    airfoilRoots.append(airfoilItem);
    airfoilChoiceArray.append(0);


    //B2机翼
    QTreeWidgetItem *wingItem = new QTreeWidgetItem(geomteryItem,QStringList(tr("机翼")));
    wingItem->setData(0,Qt::UserRole,"wing");
    wingItem->setIcon(0,icon18);
    //B3飞机
    QTreeWidgetItem *airplaneItem = new QTreeWidgetItem(geomteryItem,QStringList(tr("飞机")));
    airplaneItem->setData(0,Qt::UserRole,"airplane");
    airplaneItem->setIcon(0,icon19);
    //B4螺旋桨
    QTreeWidgetItem *propellerItem = new QTreeWidgetItem(geomteryItem,QStringList(tr("螺旋桨")));
    propellerItem->setData(0,Qt::UserRole,"propeller");
    propellerItem->setIcon(0,icon20);
    //B5整机


    //
    QTreeWidgetItem *settingItem = new QTreeWidgetItem(proItem,QStringList(tr("网格设置")));
    settingItem->setData(0,Qt::UserRole,"setting");
    //B1网格信息
    QTreeWidgetItem *rawGridItem = new QTreeWidgetItem(settingItem,QStringList(tr("网格生成")));
    rawGridItem->setData(0,Qt::UserRole,QVariant("rawGrid" + QString::number(projectIndex)));
    rawGridItem->setIcon(0,iconMeshA);

    meshRoots.append(rawGridItem);
    //B2参考
    QTreeWidgetItem *forceMomentItem = new QTreeWidgetItem(settingItem,QStringList(tr("参考值")));
    forceMomentItem->setData(0,Qt::UserRole,QVariant("forceMoment" + QString::number(projectIndex)));
    forceMomentItem->setIcon(0,icon3);
    //B3输出
    QTreeWidgetItem *volumeOutputItem = new QTreeWidgetItem(settingItem,QStringList(tr("体积输出变量")));
    volumeOutputItem->setData(0,Qt::UserRole,QVariant("volumeOutput" + QString::number(projectIndex)));
    volumeOutputItem->setIcon(0,icon5);
    //B4输出
    QTreeWidgetItem *boundaryOutputItem = new QTreeWidgetItem(settingItem,QStringList(tr("边界输出变量")));
    boundaryOutputItem->setData(0,Qt::UserRole,QVariant("boundaryOutput" + QString::number(projectIndex)));
    boundaryOutputItem->setIcon(0,icon6);
    //A求解设置
    QTreeWidgetItem *solutionItem = new QTreeWidgetItem(proItem,QStringList(tr("求解设置")));
    solutionItem->setData(0,Qt::UserRole,"solution");
    //B1控制方程
    QTreeWidgetItem *eqnItem = new QTreeWidgetItem(solutionItem,QStringList(tr("控制方程")));
    eqnItem->setData(0,Qt::UserRole,QVariant("eqn" + QString::number(projectIndex)));
    eqnItem->setIcon(0,icon7);
    //B2参考物理属性
    QTreeWidgetItem *refItem = new QTreeWidgetItem(solutionItem,QStringList(tr("参考物理属性")));
    refItem->setData(0,Qt::UserRole,QVariant("ref" + QString::number(projectIndex)));
    refItem->setIcon(0,icon4);
    //B3无粘通量方法
    QTreeWidgetItem *fluxItem = new QTreeWidgetItem(solutionItem,QStringList(tr("无粘通量方法")));
    fluxItem->setData(0,Qt::UserRole,QVariant("flux" + QString::number(projectIndex)));
    fluxItem->setIcon(0,icon12);
    //B4湍流扩散模型
    QTreeWidgetItem *flowModelItem = new QTreeWidgetItem(solutionItem,QStringList(tr("湍流扩散模型")));
    flowModelItem->setData(0,Qt::UserRole,QVariant("flowModel" + QString::number(projectIndex)));
    flowModelItem->setIcon(0,icon11);
    //B5spalart
    // QTreeWidgetItem *spalartItem = new QTreeWidgetItem(solutionItem,QStringList("spalart"));
    //spalartItem->setData(0,Qt::UserRole,QVariant("spalart" + QString::number(projectIndex)));
    //B6非线性求解器参数
    QTreeWidgetItem *nonlinearSolveItem = new QTreeWidgetItem(solutionItem,QStringList(tr("非线性求解器参数")));
    nonlinearSolveItem->setData(0,Qt::UserRole,QVariant("nonlinearSolve" + QString::number(projectIndex)));
    nonlinearSolveItem->setIcon(0,icon8);
    //B7非线性求解器参数
    QTreeWidgetItem *linearSolveItem = new QTreeWidgetItem(solutionItem,QStringList(tr("线性求解器参数")));
    linearSolveItem->setData(0,Qt::UserRole,QVariant("linearSolve" + QString::number(projectIndex)));
    linearSolveItem->setIcon(0,icon9);
    //B8运行控制
    QTreeWidgetItem *codeRunItem = new QTreeWidgetItem(solutionItem,QStringList(tr("开始计算")));
    codeRunItem->setData(0,Qt::UserRole,QVariant("codeRun" + QString::number(projectIndex)));
    codeRunItem->setIcon(0,icon10);



    //A结果与后处理
    QTreeWidgetItem *resultItem = new QTreeWidgetItem(proItem,QStringList(tr("结果与后处理")));
    resultItem->setData(0,Qt::UserRole,"result");
    QTreeWidgetItem *cloudPlotItem = new QTreeWidgetItem(resultItem,QStringList(tr("云图")));
    cloudPlotItem->setData(0,Qt::UserRole,QVariant("cloudPlot" + QString::number(projectIndex)));
    cloudPlotItem->setIcon(0,icon13);
    /*
    QTreeWidgetItem *contourSliceItem = new QTreeWidgetItem(resultItem,QStringList(tr("切片图")));
    contourSliceItem->setData(0,Qt::UserRole,"contourSlice");
    contourSliceItem->setIcon(0,icon14);
    QTreeWidgetItem *streamLineItem = new QTreeWidgetItem(resultItem,QStringList(tr("流线图")));
    streamLineItem->setData(0,Qt::UserRole,"streamLine");
    streamLineItem->setIcon(0,icon15);
    QTreeWidgetItem *VorticityItem = new QTreeWidgetItem(resultItem,QStringList(tr("涡量图")));
    VorticityItem->setData(0,Qt::UserRole,"vorticity");
    VorticityItem->setIcon(0,icon16);
    */


    proItemArray.append(proItem);
    //增加一个参数
    structCFDDefinition a;
    inputArray.append(a);
    dirArray.append(runDir);
    nameArray.append(name);
    //增加一个求解器
    fun3DRun* solver = new fun3DRun;
    solverArray.append(solver);




    projectChoiceIndex = projectIndex;
    projectIndex += 1;


    updateAddButtonState();


}

void cfdDisplay::onAddProjectClicked()
{
    // === 1️⃣ 输入项目名称 ===
    bool ok;
    QString projectName = QInputDialog::getText(
        this,
        tr("新建项目"),
        tr("请输入项目名称："),
        QLineEdit::Normal,
        "",
        &ok
    );

    if (!ok) return; // 用户取消

    projectName = projectName.trimmed();
    if (projectName.isEmpty()) {
        QMessageBox::warning(this, tr("输入无效"), tr("项目名不能为空！"));
        return;
    }

    // === 2️⃣ 选择项目运行目录 ===
    QString runDir = QFileDialog::getExistingDirectory(
        this,
        tr("选择项目运行目录"),
        QDir::homePath(),
        QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks
    );

    if (runDir.isEmpty()&&QFile::exists(runDir + "/nodet_mpi.exe")) {
        QMessageBox::warning(this,
                             tr("缺少可执行文件"),
                             tr("在指定的目录中未检测到求解器，请检查路径是否正确。"));
        return;
    }

    // === 3️⃣ 添加项目（这里可存储路径属性）===
    addProject(projectName,runDir);



    // === 4️⃣ 美化提示 ===
    QMessageBox msgBox(this);
    msgBox.setIcon(QMessageBox::Information);
    msgBox.setWindowTitle(tr("项目创建成功"));
    msgBox.setText(QString("✅ 项目 “<b>%1</b>” 已创建！").arg(projectName));
    msgBox.setInformativeText(QString("运行目录：<br><code>%1</code>").arg(runDir));
    msgBox.setStandardButtons(QMessageBox::Ok);
    msgBox.exec();

}

void cfdDisplay::updateAddButtonState(){
    if(!proItemArray.isEmpty()){
        addProjectButton->hide();
    }else{
        addProjectButton->show();
    }

}
void cfdDisplay::onTreeWidgetContextMenu(const QPoint &pos)
{


    QTreeWidgetItem *item = cfdTreeWidget->itemAt(pos);
    if (!item || item->parent() != nullptr)
      return;  // 只允许根节点



    // 记录当前节点

    cfdTreeWidget->setCurrentItem(item);


    // 弹出菜单（非阻塞）
    QPoint globalPos = cfdTreeWidget->viewport()->mapToGlobal(pos);
    treeContextMenu->popup(globalPos);

}
QString cfdDisplay::loadQssFile(const QString &path)
{
    QFile file(path);
    if (!file.open(QFile::ReadOnly | QFile::Text))
        return "";
    return QString::fromUtf8(file.readAll());
}
void cfdDisplay::applyGlobalStyle()
{
    QString style;
    style += loadQssFile(":/styles/fun3DWidget/label.qss");
    style += "\n" + loadQssFile(":/styles/fun3DWidget/qGroup.qss");
    style += "\n" + loadQssFile(":/styles/fun3DWidget/qScrollBar.qss");
    style += "\n" + loadQssFile(":/styles/fun3DWidget/widget.qss");
    style += "\n" + loadQssFile(":/styles/combobox.qss");
    style += "\n" + loadQssFile(":/styles/spinbox.qss");
    style += "\n" + loadQssFile(":/styles/checkbox.qss");
    style += "\n" + loadQssFile(":/styles/radiobox.qss");
    style += "\n" + loadQssFile(":/styles/lineedit.qss");
    style += "\n" + loadQssFile(":/styles/button.qss");
    style += "\n" + loadQssFile(":/styles/progressbar.qss");
    style += "\n" + loadQssFile(":/styles/menu.qss");
    style += "\n" + loadQssFile(":/styles/splitter.qss");


    // 🌍 全局生效
    qApp->setStyleSheet(style);

}
void cfdDisplay::changeProjectDisplay(QTreeWidgetItem* item, int column)
{
    //importProjectToolButton->setEnabled(false);
    if (!item->parent()) {
        int index = item->data(0, Qt::UserRole).toInt();
        projectChoiceIndex = index;

        funSetupWidget->initUISetup(inputArray[index]);
        funSetupWidget->switchToBox(funSetupWidget->globalSettingsBox);

        activateSolver(index);

        if (solverArray[index]->isRun) {
            progressBar->setVisible(true);
            stopButton->setVisible(true);
        } else {
            progressBar->setVisible(false);
            stopButton->setVisible(false);
        }

        // 🟡 暂停绘制（防止 solver 信号干扰）
        updatingPlots = false;
        clearAllPlots();
        plotViewA->replot();
        plotViewB->replot();

        // 🟢 重绘当前 solver 的历史数据
        redrawFromSolver(index);

        // ✅ 恢复实时绘制
        updatingPlots = true;
    }
}

void cfdDisplay::showFlowModelSetup(QTreeWidgetItem*item,int colum){
    if(item->parent()){
        QString txt1 = item->data(0,Qt::UserRole).toString();
        QString txt2 = "flowModel" + QString::number(projectChoiceIndex);
        if(txt1 == txt2){
            funSetupWidget->switchToBox(funSetupWidget->turbulentDiffusionBox);
        }

    }
}
void cfdDisplay::showVolumeOutputSetting(QTreeWidgetItem*item,int colum){
    if(item->parent()){
        QString txt1 = item->data(0,Qt::UserRole).toString();
        QString txt2 = "volumeOutput" + QString::number(projectChoiceIndex);
        if(txt1 == txt2){
            funSetupWidget->switchToBox(funSetupWidget->volumeOutputBox);
        }

    }
}
void cfdDisplay::showBoundaryOutputSetting(QTreeWidgetItem*item,int colum){
    if(item->parent()){
        QString txt1 = item->data(0,Qt::UserRole).toString();
        QString txt2 = "boundaryOutput" + QString::number(projectChoiceIndex);
        if(txt1 == txt2){
            funSetupWidget->switchToBox(funSetupWidget->boundaryOutputBox);
        }

    }
}
void cfdDisplay::showReferenceSetup(QTreeWidgetItem*item,int colum){
    if(item->parent()){
        QString txt1 = item->data(0,Qt::UserRole).toString();
        QString txt2 = "ref" + QString::number(projectChoiceIndex);
        if(txt1 == txt2){
            funSetupWidget->switchToBox(funSetupWidget->refBox);
        }

    }
}
void cfdDisplay::showEquationSetup(QTreeWidgetItem*item,int colum){
    if(item->parent()){
        QString txt1 = item->data(0,Qt::UserRole).toString();
        QString txt2 = "eqn" + QString::number(projectChoiceIndex);
        if(txt1 == txt2){
            funSetupWidget->switchToBox(funSetupWidget->eqnBox);
        }

    }
}
void cfdDisplay::showForceMomentSetup(QTreeWidgetItem*item,int colum){
    if(item->parent()){
        QString txt1 = item->data(0,Qt::UserRole).toString();
        QString txt2 = "forceMoment" + QString::number(projectChoiceIndex);
        if(txt1 == txt2){
            funSetupWidget->switchToBox(funSetupWidget->forceMomentBox);
        }

    }
}
void cfdDisplay::showRawGridSetup(QTreeWidgetItem*item,int colum){
    if(item->parent()){
        QString txt1 = item->data(0,Qt::UserRole).toString();
        QString txt2 = "rawGrid" + QString::number(projectChoiceIndex);
        if(txt1 == txt2){
            funSetupWidget->switchToBox(funSetupWidget->gridGenBox);
        }

    }
}
void cfdDisplay::showInviscidFluxSetup(QTreeWidgetItem*item,int colum){
    if(item->parent()){
        QString txt1 = item->data(0,Qt::UserRole).toString();
        QString txt2 = "flux" + QString::number(projectChoiceIndex);
        if(txt1 == txt2){
            funSetupWidget->switchToBox(funSetupWidget->fluxBox);
        }

    }
}
/*
void cfdDisplay::showSpalartSetup(QTreeWidgetItem*item,int colum){
    if(item->parent()){
        QString txt1 = item->data(0,Qt::UserRole).toString();
        QString txt2 = "spalart" + QString::number(projectChoiceIndex);
        if(txt1 == txt2){
            funSetupWidget->switchToBox(funSetupWidget->spalartBox);
        }

    }
}
*/
void cfdDisplay::showCodeRunControlSetup(QTreeWidgetItem*item,int colum){
    if(item->parent()){
        QString txt1 = item->data(0,Qt::UserRole).toString();
        QString txt2 = "codeRun" + QString::number(projectChoiceIndex);
        if(txt1 == txt2){
            funSetupWidget->switchToBox(funSetupWidget->codeRunBox);
        }

    }
}
void cfdDisplay::showNonlinearSolverSetup(QTreeWidgetItem*item,int colum){
    if(item->parent()){
        QString txt1 = item->data(0,Qt::UserRole).toString();
        QString txt2 = "nonlinearSolve" + QString::number(projectChoiceIndex);
        if(txt1 == txt2){
            funSetupWidget->switchToBox(funSetupWidget->nonlinearSolverBox);
        }

    }
}
void cfdDisplay::showLinearSolverSetup(QTreeWidgetItem*item,int colum){
    if(item->parent()){
        QString txt1 = item->data(0,Qt::UserRole).toString();
        QString txt2 = "linearSolve" + QString::number(projectChoiceIndex);
        if(txt1 == txt2){
            funSetupWidget->switchToBox(funSetupWidget->linearSolverBox);
        }

    }
}
void cfdDisplay::showAirfoil(QTreeWidgetItem*item,int colum){
    if(item->parent()){
        QString txt = item->data(0,Qt::UserRole).toString();
        QString txtTmp = txt.mid(0,7);
        QString tmp = txt.mid(7);
        bool ok;
        int index = tmp.toInt(&ok);
        if(txtTmp == "airfoil"){
            QString txt2 = item->parent()->data(0,Qt::UserRole).toString();
            QString tmp2 = txt2.mid(12);
            int ind = tmp2.toInt();
            if(ind == projectChoiceIndex){



                //vtk显示翼型
                airfoilChoiceArray[ind] = index;
                resultViewer->setAirfoil(airfoilArray[index]);
                resultViewer->switchToAirfoil();
                //检查网格是否存在
                if(QFile::exists(getMeshPath())){
                    meshRoots[ind]->setIcon(0,iconMeshB);
                    resultViewer->loadFlattenedGridAuto(getMeshPath());
                }

                else{
                    meshRoots[ind]->setIcon(0,iconMeshA);
                }
                //刷新尾缘状态
                if(airfoilArray[index][0][1] == 0.0){
                    funSetupWidget->airfoilInfoUI.trailingEdgeLabel->setText("闭合");

                }
                else{
                    int len = airfoilArray[index].length();
                    double thk = abs(airfoilArray[index][0][1] - airfoilArray[index][len - 1][1])*1000;
                    funSetupWidget->airfoilInfoUI.trailingEdgeLabel->setText(QString("未闭合,厚度:%1mm").arg(thk, 0, 'f', 3));

                }

                funSetupWidget->airfoilInfoUI.pointCountLabel->setText(QString::number(airfoilArray[index].length()));


            }
                funSetupWidget->switchToBox(funSetupWidget->airfoilBox);


        }
    }
}
void cfdDisplay::showCloudPlotSetup(QTreeWidgetItem*item,int colum){
    if(item->parent()){
        QString txt1 = item->data(0,Qt::UserRole).toString();
        QString txt2 = "cloudPlot" + QString::number(projectChoiceIndex);
        if(txt1 == txt2){
            //
            //importProjectToolButton->setEnabled(true);

            //找到目标文件

            funSetupWidget->switchToBox(funSetupWidget->cloudPlotBox);
        }
    }
}
void cfdDisplay::importPltData(){
    // 打开文件选择窗口
    QString fileName = QFileDialog::getOpenFileName(
        this,
        tr("选择 Tecplot PLT 文件"),
        QString(),
        tr("Tecplot PLT 文件 (*.dat);;所有文件 (*.*)")
    );

    // 未选择文件：直接退出
    if (fileName.isEmpty()) {
        return;
    }

    //pltReader reader(R"(C:\Users\13568\Desktop\FUN3D\CT_1_part1_tec_volume.dat)");
    pltReader reader(fileName.toStdString());
    vtkSmartPointer<vtkMultiBlockDataSet> multiBlock=reader.readTecplot();


    resultViewer->setInputData(multiBlock);



    funSetupWidget->setCloudPlotBoxItems(reader.getValueName());
    //funSetupWidget->setCloudPlotEdits(resultViewer->historyValueRange.min,resultViewer->historyValueRange.max,10);


}
void cfdDisplay::startAnalyse(){

    login->submitFun3d(4);

}
void cfdDisplay::appendOutput(const QString &text)
{
    interfaceEdit->moveCursor(QTextCursor::End);
    interfaceEdit->insertPlainText(text + "\n>>> ");
    interfaceEdit->moveCursor(QTextCursor::End);
    promptPosition = interfaceEdit->textCursor().position();
}

bool cfdDisplay::eventFilter(QObject *obj, QEvent *event)
{
    if (obj == interfaceEdit)
    {
        // ✅ 允许鼠标事件正常选中
        if (event->type() == QEvent::MouseButtonPress ||
            event->type() == QEvent::MouseButtonRelease ||
            event->type() == QEvent::MouseMove)
        {
            // 不拦截鼠标事件，让用户可以选中复制
            return false;
        }

        if (event->type() == QEvent::KeyPress)
        {
            QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
            QTextCursor cursor = interfaceEdit->textCursor();

            // 防止光标跑到提示符前
            if (cursor.position() < promptPosition)
            {
                cursor.setPosition(interfaceEdit->document()->characterCount() - 1);
                interfaceEdit->setTextCursor(cursor);
            }

            // 拦截 Backspace（在提示符处）
            if (keyEvent->key() == Qt::Key_Backspace && cursor.position() <= promptPosition)
                return true;

            // 拦截左箭头（防止跑到提示符前）
            if (keyEvent->key() == Qt::Key_Left && cursor.position() <= promptPosition)
                return true;

            // ✅ 放行 Ctrl+C（复制）
            if (keyEvent->key() == Qt::Key_C && keyEvent->modifiers() == Qt::ControlModifier)
                return false;

            // ✅ 放行 Ctrl+A（全选）
            if (keyEvent->key() == Qt::Key_A && keyEvent->modifiers() == Qt::ControlModifier)
                return false;

            // 回车键：提取命令
            if (keyEvent->key() == Qt::Key_Return || keyEvent->key() == Qt::Key_Enter)
            {
                QString fullText = interfaceEdit->toPlainText();
                QString command = fullText.mid(promptPosition).trimmed();

                emit commandEntered(command);

                interfaceEdit->appendPlainText("");
                interfaceEdit->insertPlainText(">>> ");
                promptPosition = interfaceEdit->toPlainText().size();
                return true;
            }
        }
    }

    return QWidget::eventFilter(obj, event);
}


void cfdDisplay::handleCommand(const QString& text){
    interfaceEdit->appendPlainText("\n" + text);


}



void cfdDisplay::handleIterationData(const IterationData &data)
{
     if (!updatingPlots) return;
     if (data.iter <= 0) return;
    int iter = data.iter;

    // ===============================
    // 📉 残差图数据更新（plotViewA）
    // ===============================
    if (densityRMSGraph) densityRMSGraph->addData(iter, data.densityRMS);
    if (densityMAXGraph) densityMAXGraph->addData(iter, data.densityMAX);
    if (densityXGraph)   densityXGraph->addData(iter, data.densityX);
    if (densityYGraph)   densityYGraph->addData(iter, data.densityY);
    if (densityZGraph)   densityZGraph->addData(iter, data.densityZ);

    if (turbRMSGraph) turbRMSGraph->addData(iter, data.turbRMS);
    if (turbMAXGraph) turbMAXGraph->addData(iter, data.turbMAX);
    if (turbXGraph)   turbXGraph->addData(iter, data.turbX);
    if (turbYGraph)   turbYGraph->addData(iter, data.turbY);
    if (turbZGraph)   turbZGraph->addData(iter, data.turbZ);

    // ===============================
    // ✈️ 升阻力图数据更新（plotViewB）
    // ===============================
    if (liftGraph) liftGraph->addData(iter, data.lift);
    if (dragGraph) dragGraph->addData(iter, data.drag);

    // ===============================
    // 📊 进度条与图形刷新
    // ===============================
    updateProgressBar(getProgressValue(iter));

    // 自动缩放
    plotViewA->rescaleAxes(true);
    plotViewB->rescaleAxes(true);

    // 异步刷新（不卡界面）
    plotViewA->replot(QCustomPlot::rpQueuedReplot);
    plotViewB->replot(QCustomPlot::rpQueuedReplot);
}



void cfdDisplay::onStopProjectClicked(){
    progressBar->setVisible(false);
    stopButton->setVisible(false);
    QMessageBox::information(this, tr("提示"), tr("计算暂停"));
    solverArray[projectChoiceIndex]->stop();
    solverArray[projectChoiceIndex]->isRun = false;


}

void cfdDisplay::updateProgressBar(const int value){
    progressBar->setValue(value);

    if(value >= 100){
        calculationFinished();
    }
}
int cfdDisplay::getProgressValue(const double iter) {
    double steps = inputArray[projectChoiceIndex].codeRunData.steps;
    if (steps <= 0) return 0;
    return static_cast<int>((iter / steps) * 100.0);

}
void cfdDisplay::calculationFinished()
{
    // 停止进度条动画（如果有定时器）


    QMessageBox::information(this, tr("提示"), tr("计算完成"));
    progressBar->setVisible(false);
    stopButton->setVisible(false);
    solverArray[projectChoiceIndex]->isRun = false;
    // ✅ 弹出提示框

    // 🧹（可选）执行收尾工作
    // 比如保存图表、重置状态、解锁界面按钮等
    // saveCurrentPlots();
    // runButton->setEnabled(true);
}
void cfdDisplay::clearAllPlots()
{
    // 🧹 清空残差曲线
    if (densityRMSGraph) densityRMSGraph->data()->clear();
    if (densityMAXGraph) densityMAXGraph->data()->clear();
    if (turbRMSGraph) turbRMSGraph->data()->clear();
    if (turbMAXGraph) turbMAXGraph->data()->clear();
    if (densityXGraph) densityXGraph->data()->clear();
    if (densityYGraph) densityYGraph->data()->clear();
    if (densityZGraph) densityZGraph->data()->clear();
    if (turbXGraph) turbXGraph->data()->clear();
    if (turbYGraph) turbYGraph->data()->clear();
    if (turbZGraph) turbZGraph->data()->clear();

    // 🧹 清空升阻力曲线
    if (liftGraph) liftGraph->data()->clear();
    if (dragGraph) dragGraph->data()->clear();

    // 🔄 重新绘制
    if (plotViewA) plotViewA->replot(QCustomPlot::rpQueuedReplot);
    if (plotViewB) plotViewB->replot(QCustomPlot::rpQueuedReplot);

    // 📉 可选：重置坐标轴范围
    if (plotViewA) plotViewA->rescaleAxes(true);
    if (plotViewB) plotViewB->rescaleAxes(true);

}
void cfdDisplay::redrawFromSolver(int index)
{
    // 边界检查
    if (index < 0 || index >= solverArray.size()) {
        qWarning() << "redrawFromSolver: index out of range:" << index;
        return;
    }

    fun3DRun *solver = solverArray.at(index);
    if (!solver) {
        qWarning() << "redrawFromSolver: solver is null at index" << index;
        return;
    }

    // 假设 historyData 是 QVector<IterationData> 或 QList<IterationData>
    // 复制一份以避免并发修改问题（solver 可能正在运行）
    QVector<IterationData> hist;
    {
        QMutexLocker locker(&solver->historyMutex);
        hist = solver->historyData;
    }



    // 清空旧数据
    clearAllPlots();

    // 快速填充：一次性加入数据（不每条都 replot）
    for (const IterationData &d : hist) {
        int iter = d.iter;

        if (densityRMSGraph) densityRMSGraph->addData(iter, d.densityRMS);
        if (densityMAXGraph) densityMAXGraph->addData(iter, d.densityMAX);
        if (densityXGraph)   densityXGraph->addData(iter, d.densityX);
        if (densityYGraph)   densityYGraph->addData(iter, d.densityY);
        if (densityZGraph)   densityZGraph->addData(iter, d.densityZ);

        if (turbRMSGraph) turbRMSGraph->addData(iter, d.turbRMS);
        if (turbMAXGraph) turbMAXGraph->addData(iter, d.turbMAX);
        if (turbXGraph)   turbXGraph->addData(iter, d.turbX);
        if (turbYGraph)   turbYGraph->addData(iter, d.turbY);
        if (turbZGraph)   turbZGraph->addData(iter, d.turbZ);

        if (liftGraph) liftGraph->addData(iter, d.lift);
        if (dragGraph) dragGraph->addData(iter, d.drag);


    }

    // 一次性自动缩放并重绘（比每次 addData 后 replot 快很多）
    if (plotViewA) {
        plotViewA->rescaleAxes(true);
        plotViewA->replot(QCustomPlot::rpQueuedReplot);
    }
    if (plotViewB) {
        plotViewB->rescaleAxes(true);
        plotViewB->replot(QCustomPlot::rpQueuedReplot);
    }

    // 更新进度条到最后一个迭代（如果你需要）
    if (!hist.isEmpty()) {
        int lastIter = hist.last().iter;
        updateProgressBar(getProgressValue(lastIter));
    }
}
void cfdDisplay::activateSolver(int index)
{
    if (index < 0 || index >= solverArray.size())
        return;

    // 🔹 1️⃣ 断开所有 solver 的连接
    for (int i = 0; i < solverArray.size(); ++i) {
        fun3DRun* s = solverArray[i];
        if (s->outputConn) disconnect(s->outputConn);
        if (s->errorConn)  disconnect(s->errorConn);
        if (s->iterConn)   disconnect(s->iterConn);
    }

    // 🔹 2️⃣ 重新连接当前 solver
    fun3DRun* solver = solverArray[index];
    connectSolverSignals(solver);
}

void cfdDisplay::connectSolverSignals(fun3DRun* solver)
{
    if (solver->outputConn) disconnect(solver->outputConn);
    if (solver->errorConn)  disconnect(solver->errorConn);
    if (solver->iterConn)   disconnect(solver->iterConn);

    solver->outputConn = connect(solver, &fun3DRun::newOutput, this, &cfdDisplay::appendOutput);
    solver->errorConn  = connect(solver, &fun3DRun::newError, this, &cfdDisplay::appendOutput);
    solver->iterConn   = connect(solver, &fun3DRun::iterationParsed, this, &cfdDisplay::handleIterationData);
}
void cfdDisplay::setCloudPlotLevel(){
    double min = funSetupWidget->cloudPlotUI.minEdit->text().toDouble();
    double max = funSetupWidget->cloudPlotUI.maxEdit->text().toDouble();
    int nums = funSetupWidget->cloudPlotUI.numsEdit->text().toUInt();

    resultViewer->setCpRange(min,max,nums);

}
void cfdDisplay::setTheme(int style)
{
    applyQtPalette(style);

    // 保存到 QSettings，下次启动自动生效
    QSettings settings("SmartCombatFlow", "Config");
    settings.setValue("ThemeStyle", style);
}
void cfdDisplay::updateAirfoilArray(const QVector<QVector<QVector<double>>>&airfoilArrayTmp,const QVector<QString>&nameArray,const QVector<int>&cstNumArrayTmp){

    if(!airfoilArrayTmp.isEmpty() && !nameArray.isEmpty()){

        QStringList boxList;
        for(int i = 0;i<nameArray.length();i++){
            boxList.append(nameArray[i]);
        }

        updateCFDTree(airfoilRoots,nameArray);

        /*
        for(int i = 0;i<profileAirfoilChoiceCombobox.length();i++){
            updateComboBox(profileAirfoilChoiceCombobox[i],boxList);
        }
        */
        airfoilArray = airfoilArrayTmp;;
        //VLMCSTArray = airfoilArrayTmp;
        AirfoilNameArray = nameArray;
        cstNumArray = cstNumArrayTmp;
        //updateWingData();


    }
}
void cfdDisplay::updateCFDTree(const QVector<QTreeWidgetItem*> &Roots,
                       const QVector<QString> &NameArray)
{
    int projectCount = qMin(Roots.size(), NameArray.size());

    for (int i = 0; i < projectCount; ++i) {

        QTreeWidgetItem *root = Roots[i];

        // ✅ 1. 删除旧节点
        qDeleteAll(root->takeChildren());

        // ✅ 2. 添加新节点


        for (int j = 0; j < NameArray.length(); ++j) {
            QTreeWidgetItem *child = new QTreeWidgetItem();
            child->setText(0, NameArray[j]);
            child->setData(0,Qt::UserRole,QVariant("airfoil" + QString::number(j)));

            root->addChild(child);
        }
    }
}
void cfdDisplay::selectAirfoil()
{
    QTreeWidgetItem* root = airfoilRoots[projectChoiceIndex];
    if(!root) return;

    // ⭐ 遍历所有子节点
    for(int i = 0; i < root->childCount(); i++){
        QTreeWidgetItem* child = root->child(i);

        // 清除 checkbox
        child->setFlags(child->flags() & ~Qt::ItemIsUserCheckable);
        child->setCheckState(0, Qt::Unchecked);

        // 清颜色（可选）
        child->setForeground(0, Qt::black);
    }

    // ⭐ 当前选中的
    QTreeWidgetItem* selected = root->child(airfoilChoiceArray[projectChoiceIndex]);

    selected->setFlags(selected->flags() | Qt::ItemIsUserCheckable);
    selected->setCheckState(0, Qt::Checked);

    // 高亮（推荐）
    selected->setForeground(0, QColor(60, 140, 90));  // 柔和绿色（推荐）
    int ind = airfoilChoiceArray[projectChoiceIndex];
    if(airfoilArray[ind][0][1] == 0.0){

        funSetupWidget->gridUI.cGridRadio->setChecked(true);
    }
    else{
        funSetupWidget->gridUI.oGridRadio->setChecked(true);
    }

}

QString cfdDisplay::getMeshPath(){
    QString dirPath = QDir::currentPath() + "/mesh";
    int ind = airfoilChoiceArray[projectChoiceIndex];
    QString filePath = dirPath + QString("/%1.p3d").arg(AirfoilNameArray[ind]);

    return filePath;

}
void cfdDisplay::GenerateMesh(){
    GridGenData input = funSetupWidget->getGridGenData();
    int ind = airfoilChoiceArray[projectChoiceIndex];

    login->submitJob(airfoilArray[ind],input,AirfoilNameArray[ind]);


}

void cfdDisplay::displayAirfoilMesh(){


    if(QFile::exists(getMeshPath())){
        meshRoots[projectChoiceIndex]->setIcon(0,iconMeshB);
        resultViewer->loadFlattenedGridAuto(getMeshPath());

    }
}
