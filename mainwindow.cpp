#include "mainwindow.h"
#include <QCoreApplication>
#include <QDesktopServices>
#include <QFileInfo>
#include <QMessageBox>
#include <QUrl>
#if defined(_MSC_VER) && (_MSC_VER >= 1600)
# pragma execution_character_set("utf-8")
#endif


mainWindow::mainWindow(QMainWindow *parent)
    : QMainWindow(parent){


    setMinimumSize(1500,900);
    setWindowIcon(QIcon(QStringLiteral(":/images/main/passwing.ico")));

    setStyleSheet("QMainWindow { background-color: rgb(255, 255, 255); }");

    chatWindow = new chatWidget();
    designWingWindow = new wingDisplay();
    designAirplaneWindow = new airplaneDisplay();
    designAirfoilWindow = new airfoilDisplay();
    designPropellerWindow = new propellerDisplay();
    cfdWindow = new cfdDisplay();
    //displayAirCartWindow = new STLReader();
    projectFile = new myFile();
    displayAirportWindow = new airplaneLibrary();





    loadIcon();
    initialAboutMeDialog();
    initialMenu();
    initialLeftWindow();
    initialTopWindow();
    initialBottomWindow();
    updateState();//取消所有按钮选项



    //预加载
    //QString filePath = QDir::currentPath() + "/resoure/geometry/penguin.stl";
    //displayAirCartWindow->setModelName(filePath);
    setCentralWidget(displayAirportWindow);




    //翼型信息通讯
    connect(designAirfoilWindow,&airfoilDisplay::emitAirfoilArray,designWingWindow,&wingDisplay::updateAirfoilArray);//数据给机翼
    //connect(designAirfoilWindow,&airfoilDisplay::emitAirfoilArray,this,&mainWindow::updateAirfoilArray);//数据给主页
    //翼型插值数据通讯
    connect(designAirfoilWindow,&airfoilDisplay::emitAirfoilInterArray,designWingWindow,&wingDisplay::updateAirfoilInterArray);

    connect(designAirfoilWindow,&airfoilDisplay::emitAirfoilProgressBarValue,this,&mainWindow::updateMyProgressBar);
    //螺旋桨通讯
    connect(designAirfoilWindow,&airfoilDisplay::emitAirfoilArray,designPropellerWindow,&propellerDisplay::updateAirfoilArray);
    //cfd通讯
    connect(designAirfoilWindow,&airfoilDisplay::emitAirfoilArray,cfdWindow,&cfdDisplay::updateAirfoilArray);
    connect(designPropellerWindow,&propellerDisplay::emitPropellerProgressBarValue,this,&mainWindow::updateMyProgressBar);
    //connect(this,&mainWindow::interAirfoilArraySignal,designWingWindow,&wingDisplay::updateAirfoilArray);
    //机翼信息通讯
    connect(designWingWindow,&wingDisplay::emitWingProgressBarValue,this,&mainWindow::updateMyProgressBar);
    //飞机设计通讯
    connect(designWingWindow,&wingDisplay::emitList,designAirplaneWindow,&airplaneDisplay::updateList);
    connect(designAirplaneWindow,&airplaneDisplay::emitWingDefineDialog,designWingWindow,&wingDisplay::showWingDefineDialog);
    connect(designWingWindow,&wingDisplay::emitWingDefineFinish,designAirplaneWindow,&airplaneDisplay::updateAirplaneDefineWidget);
    connect(designAirplaneWindow,&airplaneDisplay::emitAirplaneProgressBarValue,this,&mainWindow::updateMyProgressBar);
    //打开项目
    connect(this,&mainWindow::emitAirfoilArray,designAirfoilWindow,&airfoilDisplay::updateAirfoilList);
    connect(this,&mainWindow::emitWingArray,designWingWindow,&wingDisplay::updateWingList);
    connect(this,&mainWindow::emitTailArray,designWingWindow,&wingDisplay::updateTailList);
    connect(this,&mainWindow::emitPropellerArray,designPropellerWindow,&propellerDisplay::updatePropList);
    connect(this,&mainWindow::emitAirplaneArray,designAirplaneWindow,&airplaneDisplay::updateAirplaneList);

}
void mainWindow::loadIcon(){

    homeViewIcon =  ":/images/home.png";
    airfoilViewIcon = ":/images/airfoil.png";
    airplaneViewIcon = ":/images/airplane.png";
    powerViewIcon = ":/images/propellerA.png";
    optimizationViewIcon = ":/images/optimization.png";
    saasViewIcon = ":/images/saas.png";
    saasViewIconA = ":/images/saasA.png";
    startViewIcon = ":/images/start.png";

    flyvisionIcon = ":/images/window/wait1.png";
    airportIcon = ":/images/window/home/airport.png";
    helpIcon = ":/images/window/home/help.png";
    theoryIcon = ":/images/window/home/theory.png";

    airfoilDesignIcon = ":/images/window/airfoil/airfoilDesign.png";
    airfoilChoiceIcon = ":/images/window/airfoil/airfoilExplorer.png";
    airfoilInterIcon = ":/images/window/airfoil/airfoilinter.png";


    wingDesignIcon = ":/images/window/airplane/wingDesign.png";
    tailDesignIcon = ":/images/window/airplane/tailDesign.png";
    airplaneDesignIcon = ":/images/window/airplane/airplaneDesign.png";

    propAnalyseIcon = ":/images/window/propeller/propAnalyse.png";
    propDesignIcon = ":/images/window/propeller/propDesign.png";
    propSolveIcon = ":/images/window/propeller/propSolve.png";

    optimizationAirfoilIcon = ":/images/window/optimization/airfoilOptimization.png";
    optimizationWingIcon = ":/images/window/optimization/wingOptimization.png";

}
void mainWindow::initialLeftWindow() {
    leftDock = new QDockWidget(this);
    QWidget *titleBarWidget = new QWidget();
    leftDock->setTitleBarWidget(titleBarWidget);

    leftDock->setFixedWidth(100);
    //leftDock->setFixedHeight(3500);

    // 设置 leftDock 的背景颜色
    QPalette leftDockPalette = leftDock->palette();
    leftDockPalette.setColor(QPalette::Window, QColor(64, 64, 64));
    leftDock->setAutoFillBackground(true);
    leftDock->setPalette(leftDockPalette);
    //leftDock->setFeatures(QDockWidget::NoDockWidgetFeatures);
    leftDock->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);


    leftWidget = new QWidget(leftDock);
    productVLayout = new QVBoxLayout(leftWidget);


    homeViewButton = new QToolButton(leftWidget);
    airfoilViewButton = new QToolButton(leftWidget);
    airplaneViewButton = new QToolButton(leftWidget);
    powerViewButton = new QToolButton(leftWidget);
    optimizationViewButton = new QToolButton(leftWidget);
    saasViewButton = new QToolButton(leftWidget);
    startAnalyseButton = new QToolButton(leftWidget);





    homeViewButton->setFixedSize(100,100);
    homeViewButton->setIconSize(QSize(50,50));
    homeViewButton->setIcon(QIcon(homeViewIcon));
    homeViewButton->setText("主页");
    homeViewButton->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);


    airfoilViewButton->setFixedSize(100,100);
    airfoilViewButton->setIconSize(QSize(50,50));
    airfoilViewButton->setIcon(QIcon(airfoilViewIcon));
    airfoilViewButton->setText("翼型设计");
    airfoilViewButton->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);



    airplaneViewButton->setFixedSize(100,100);
    airplaneViewButton->setIconSize(QSize(50,50));
    airplaneViewButton->setIcon(QIcon(airplaneViewIcon));
    airplaneViewButton->setText("飞机设计");
    airplaneViewButton->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);





    powerViewButton->setFixedSize(100,100);
    powerViewButton->setIconSize(QSize(50,50));
    powerViewButton->setIcon(QIcon(powerViewIcon));
    powerViewButton->setText("动力设计");
    powerViewButton->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);


    optimizationViewButton->setFixedSize(100,100);
    optimizationViewButton->setIconSize(QSize(50,50));
    optimizationViewButton->setIcon(QIcon(optimizationViewIcon));
    optimizationViewButton->setText("优化设计");
    optimizationViewButton->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);

    saasViewButton->setFixedSize(100,100);
    saasViewButton->setIconSize(QSize(50,50));
    saasViewButton->setIcon(QIcon(saasViewIcon));
    saasViewButton->setText("云计算");
    saasViewButton->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);


    startAnalyseButton->setFixedSize(100,80);
    startAnalyseButton->setIconSize(QSize(30,30));
    startAnalyseButton->setIcon(QIcon(startViewIcon));
    startAnalyseButton->setText("开始");
    startAnalyseButton->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);




    airfoilViewButton->setFixedWidth(100);

    initialHomeWidget();

    initialAirfoilWidget();

    initialAirplaneWidget();

    initialPowerWidget();

    initialOptimizationWidget();








    productVLayout->addWidget(homeViewButton);
    productVLayout->addWidget(airfoilViewButton);
    productVLayout->addWidget(airplaneViewButton);
    productVLayout->addWidget(powerViewButton);
    productVLayout->addWidget(optimizationViewButton);
    productVLayout->addWidget(saasViewButton);



    productVLayout->addSpacerItem(new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding));

    productVLayout->addWidget(startAnalyseButton);
    leftWidget->setLayout(productVLayout);
    leftDock->setWidget(leftWidget);
    productVLayout->setContentsMargins(0,0,0,0);
    productVLayout->setSpacing(0);
    connect(homeViewButton,&QToolButton::clicked,this,&mainWindow::changeButtonState);
    connect(airfoilViewButton,&QToolButton::clicked,this,&mainWindow::changeButtonState);
    connect(airplaneViewButton,&QToolButton::clicked,this,&mainWindow::changeButtonState);
    connect(powerViewButton,&QToolButton::clicked,this,&mainWindow::changeButtonState);
    connect(optimizationViewButton,&QToolButton::clicked,this,&mainWindow::changeButtonState);
    connect(saasViewButton,&QToolButton::clicked,this,&mainWindow::changeButtonState);
    connect(homeViewButton,&QToolButton::clicked,this,&mainWindow::changeWindowForHome);
    connect(airfoilViewButton,&QToolButton::clicked,this,&mainWindow::changeWindowForAirfoil);
    connect(airplaneViewButton,&QToolButton::clicked,this,&mainWindow::changeWindowForAirplane);
    connect(powerViewButton,&QToolButton::clicked,this,&mainWindow::changeWindowForPower);
    connect(optimizationViewButton,&QToolButton::clicked,this,&mainWindow::changeWindowForOptimization);
    connect(startAnalyseButton,&QPushButton::clicked,this,&mainWindow::startAnalyse);
    connect(saasViewButton, &QToolButton::clicked,
            this, &mainWindow::onSaasViewToggled);


    addDockWidget(Qt::LeftDockWidgetArea, leftDock);


}
// .cpp
void mainWindow::onSaasViewToggled()
{
    if (!cfdWindow->login->isJoin) {
        cfdWindow->login->exec();
        if(cfdWindow->login->isJoin){
            replaceCentralWidget(cfdWindow);
            saasViewButton->setIcon(QIcon(saasViewIconA));
        }


    } else {
        // SAAS模式

        replaceCentralWidget(cfdWindow);
    }
}
void mainWindow::initialTopWindow() {
    topDock = new QDockWidget();
    topDock->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    QWidget *titleBarWidget = new QWidget();
    QPalette topDockPalette = topDock->palette();
    topDockPalette.setColor(QPalette::Window, QColor(64, 64, 64));
    topDock->setAutoFillBackground(true);
    topDock->setPalette(topDockPalette);
    topDock->setTitleBarWidget(titleBarWidget);
    //topDock->setFeatures(QDockWidget::NoDockWidgetFeatures);
    topDock->setFixedHeight(30);
    topWidget = new QWidget(topDock);
    topDock->setWidget(topWidget);
    addDockWidget(Qt::TopDockWidgetArea, topDock);
}

void mainWindow::initialBottomWindow() {
    bottomDock = new QDockWidget();
    bottomWidget = new QWidget(bottomDock);
    timeBar = new QProgressBar();
    bottomVlayout = new QVBoxLayout();

    bottomDock->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    QWidget *titleBarWidget = new QWidget();
    QPalette bottomDockPalette = bottomDock->palette();
    bottomDockPalette.setColor(QPalette::Window, QColor(64, 64, 64));
    bottomDock->setAutoFillBackground(true);
    bottomDock->setPalette(bottomDockPalette);
    bottomDock->setTitleBarWidget(titleBarWidget);
    //bottomDock->setFeatures(QDockWidget::NoDockWidgetFeatures);
    bottomDock->setFixedHeight(30);

    timeBar->setObjectName("progressBar");
    timeBar->setRange(0,100);
    timeBar->setValue(0);
    timeBar->setFormat("当前进度：%p%");
    timeBar->hide();

    bottomVlayout->addWidget(timeBar);
    bottomDock->setWidget(bottomWidget);
    bottomWidget->setLayout(bottomVlayout);
    //bottomDock->setStyleSheet("padding: 0px; margin: 0px;");
    addDockWidget(Qt::BottomDockWidgetArea, bottomDock);
}
void mainWindow::initialMenu(){
    menuBar = new QMenuBar(this);



    fileMenu = new QMenu("文件");
    libaridesMenu = new QMenu("数据库");
    //airfoilDesignMenu = new QMenu("翼型设计");
    airfoilAnalysisMenu = new QMenu("翼型分析");
    airplaneDesignMenu = new QMenu("飞机设计");
    propellerDesignMenu = new QMenu("螺旋桨设计");
    aboutMeMenu = new QMenu("关于我们");
    //outputMenu = new QMenu("输出");

    importAirfoilActions = new QAction("导入翼型");
    openActions = new QAction("打开项目");
    saveActions = new QAction("保存项目");

    importAirfoilActions = new QAction(QIcon(":/images/importAirfoil.png"), "导入翼型");
    openActions         = new QAction(QIcon(":/images/importPro.png"), "打开项目");
    saveActions         = new QAction(QIcon(":/images/savePro.png"), "保存项目");



    airfoilActions = new QAction(QIcon(":/images/airfoilLib.png"), "翼型库");

    propActions = new QAction(QIcon(":/images/propellerLib.png"), "螺旋桨库");


    //airfoilDesignActions = new QAction("正设计");
    //airfoilInverseDesignActions = new QAction("反设计");
    //airfoilOptimizationActions = new QAction("优化");
    //airfoilBlendingActions = new QAction("翼型融合");

    simpleAnalysisActions = new QAction("简单分析");
    reynoldsAnalysisActions = new QAction("多雷诺数分析");


    wingDefineActions = new QAction("机翼定义");
    tailDefineActions = new QAction("尾翼定义");
    airplaneDefineActions = new QAction("飞机定义");
    //wingDesignActions = new QAction("机翼设计");
    //wingOptimizationActions = new QAction("机翼优化");

    propellerDefineActions = new QAction("螺旋桨定义");

    aboutMeActions = new QAction("飞视科技");

    // writeAirfoilDataActions = new QAction("翼型输出");
    // writeDXFDataActions = new QAction("DXF输出");
    // writeCATIAScriptActions = new QAction("CATIA脚本输出");
    // conversionDataActions = new QAction("数据转换");

    importAirfoilActions->setProperty("index",0);
    openActions->setProperty("index",1);







    fileMenu->addAction(importAirfoilActions);
    fileMenu->addAction(openActions);
    fileMenu->addAction(saveActions);



    libaridesMenu->addAction(airfoilActions);
    libaridesMenu->addAction(propActions);

    //airfoilDesignMenu->addAction(airfoilDesignActions);
    //airfoilDesignMenu->addAction(airfoilInverseDesignActions);
    //airfoilDesignMenu->addAction(airfoilOptimizationActions);
    //airfoilDesignMenu->addAction(airfoilBlendingActions);

    airfoilAnalysisMenu->addAction(simpleAnalysisActions);
    airfoilAnalysisMenu->addAction(reynoldsAnalysisActions);


    airplaneDesignMenu->addAction(wingDefineActions);
    airplaneDesignMenu->addAction(tailDefineActions);
    airplaneDesignMenu->addAction(airplaneDefineActions);
    //wingDesignMenu->addAction(wingDesignActions);
    //wingDesignMenu->addAction(wingOptimizationActions);

    propellerDesignMenu->addAction(propellerDefineActions);

    aboutMeMenu->addAction(aboutMeActions);


    airfoilActions->setShortcut(QKeySequence(Qt::Key_F1));
    propActions->setShortcut(QKeySequence(Qt::Key_F2));


    // outputMenu->addAction(writeAirfoilDataActions);
    // outputMenu->addAction(writeDXFDataActions);
    // outputMenu->addAction(writeCATIAScriptActions);
    // outputMenu->addAction(conversionDataActions);



    menuBar->addMenu(fileMenu);
    menuBar->addMenu(libaridesMenu);
    //menuBar->addMenu(airfoilDesignMenu);

    menuBar->addMenu(airplaneDesignMenu);
    menuBar->addMenu(propellerDesignMenu);
    menuBar->addMenu(airfoilAnalysisMenu);
    menuBar->addMenu(aboutMeMenu);
    //menuBar->addMenu(outputMenu);

    setMenuBar(menuBar);

    connect(importAirfoilActions,&QAction::triggered,this,&mainWindow::readData);
    connect(openActions,&QAction::triggered,this,&mainWindow::readData);
    connect(saveActions,&QAction::triggered,this,&mainWindow::saveData);


    //connect(airfoilDesignActions,&QAction::triggered,this,&mainWindow::changeWindowForAirfoilDesign);
    //connect(airfoilOptimizationActions,&QAction::triggered,this,&mainWindow::changeWindowForAirfoilOptimization);
    //connect(wingDesignActions,&QAction::triggered,this,&mainWindow::changeWindowForWingDesign);
    //connect(wingOptimizationActions,&QAction::triggered,this,&mainWindow::changeWindowForWingOptimization);
    connect(propellerDefineActions,&QAction::triggered,designPropellerWindow,&propellerDisplay::showPropDefineDialog);

    connect(airfoilActions,&QAction::triggered,designAirfoilWindow,&airfoilDisplay::showLibary);
    connect(propActions,&QAction::triggered,designPropellerWindow,&propellerDisplay::showPropLibary);
    connect(simpleAnalysisActions,&QAction::triggered,designAirfoilWindow,&airfoilDisplay::showAnalyseTable);
    connect(reynoldsAnalysisActions,&QAction::triggered,designAirfoilWindow,&airfoilDisplay::showAnalyseReTable);
    //connect(airfoilBlendingActions,&QAction::triggered,designAirfoilWindow,&airfoilDisplay::showAirfoilBlendingDialog);
    //connect(writeAirfoilDataActions,&QAction::triggered,designAirfoilWindow,&airfoilDisplay::showAirfoilOutputTable);
    connect(wingDefineActions,&QAction::triggered,designWingWindow,&wingDisplay::showWingDefineWidget);
    connect(tailDefineActions,&QAction::triggered,designWingWindow,&wingDisplay::showTailDefineDialog);
    connect(airplaneDefineActions,&QAction::triggered,designAirplaneWindow,&airplaneDisplay::showAirplaneDefineDialog);
    connect(aboutMeActions,&QAction::triggered,aboutMeDialog,&QDialog::show);
}
void mainWindow::initialHomeWidget(){
    homeWidget = new QWidget();
    homeGLayout = new QGridLayout();
    visionButton = new imageButton(flyvisionIcon,homeWidget);
    airportButton = new imageButton(airportIcon,homeWidget);
    helpButton = new imageButton(helpIcon,homeWidget);
    theoryButton = new imageButton(theoryIcon,homeWidget);




    // Set size policy to ensure buttons expand to fill the space
    visionButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    airportButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    helpButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    theoryButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    visionButton->setToolTip("自然语言处理模型");
    airportButton->setToolTip("无人机机库");
    helpButton->setToolTip("帮助文档");
    theoryButton->setToolTip("理论文档");

    visionButton->setEnabled(false);


    // Add buttons to the layout
    homeGLayout->addWidget(visionButton,0,0,1,1);
    homeGLayout->addWidget(airportButton,0,1,1,1);
    homeGLayout->addWidget(helpButton,1,0,1,1);
    homeGLayout->addWidget(theoryButton,1,1,1,1);
    homeWidget->setLayout(homeGLayout);

    connect(airportButton,&QPushButton::clicked,this,&mainWindow::changeWindowForAirport);
    connect(helpButton,&QPushButton::clicked,this,&mainWindow::showHelpHtml);
    connect(theoryButton,&QPushButton::clicked,this,&mainWindow::showTheoreticalFrameworkHtml);


}
void mainWindow::initialAirfoilWidget(){
    airfoilWidget = new QWidget();
    airfoilGLayout = new QGridLayout(airfoilWidget);

    airfoilDesignButton = new imageButton(airfoilDesignIcon,airfoilWidget);
    airfoilAntiDesignButton = new imageButton(flyvisionIcon,airfoilWidget);
    airfoilChoiceButton = new imageButton(airfoilChoiceIcon,airfoilWidget);
    otherAirfoilButton = new imageButton(airfoilInterIcon,airfoilWidget);

    // Set size policy to ensure buttons expand to fill the space
    airfoilDesignButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    airfoilAntiDesignButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    airfoilChoiceButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    otherAirfoilButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    airfoilDesignButton->setToolTip("翼型正设计");
    airfoilAntiDesignButton->setToolTip("翼型反设计");
    airfoilChoiceButton->setToolTip("翼型选型");
    otherAirfoilButton->setToolTip("翼型性能计算");

    airfoilAntiDesignButton->setEnabled(false);
    //otherAirfoilButton->setEnabled(false);

    // Add buttons to the layout
    airfoilGLayout->addWidget(airfoilDesignButton, 0, 0, 1, 1);
    airfoilGLayout->addWidget(airfoilAntiDesignButton, 0, 1, 1, 1);
    airfoilGLayout->addWidget(airfoilChoiceButton, 1, 0, 1, 1);
    airfoilGLayout->addWidget(otherAirfoilButton, 1, 1, 1, 1);

    airfoilWidget->setLayout(airfoilGLayout);

    connect(airfoilDesignButton,&QPushButton::clicked,this,&mainWindow::changeWindowForAirfoilDesign);
    //connect(airfoilChoiceButton,&QPushButton::clicked,designAirfoilWindow,&airfoilDisplay::showAirfoilBlendingDialog);
    connect(otherAirfoilButton,&QPushButton::clicked,this,&mainWindow::changeWindowForAirfoilInter);
    connect(airfoilChoiceButton,&QPushButton::clicked,designAirfoilWindow,&airfoilDisplay::showAirfoilExplorerWidget);

}
void mainWindow::initialAirplaneWidget(){
    airplaneWidget = new QWidget();
    airplaneGLayout = new QGridLayout(airplaneWidget);

    // 创建按钮
    wingDesignButton = new imageButton(wingDesignIcon,airplaneWidget);
    tailDesignButton = new imageButton(tailDesignIcon,airplaneWidget);
    airplaneDesignButton = new imageButton(airplaneDesignIcon,airplaneWidget);
    airplaneOtherButtonA = new imageButton(flyvisionIcon,airplaneWidget);
    // Set size policy to ensure buttons expand to fill the space
    wingDesignButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    tailDesignButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    airplaneDesignButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    airplaneOtherButtonA->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    wingDesignButton->setToolTip("机翼设计");
    tailDesignButton->setToolTip("尾翼设计");
    airplaneDesignButton->setToolTip("整机设计");
    airplaneOtherButtonA->setToolTip("整机性能计算");


    //airplaneDesignButton->setEnabled(false);
    airplaneOtherButtonA->setEnabled(false);



    // Add buttons to the layout

    airplaneGLayout->addWidget(wingDesignButton, 0, 0, 1, 1);
    airplaneGLayout->addWidget(tailDesignButton, 0, 1, 1, 1);
    airplaneGLayout->addWidget(airplaneDesignButton, 1, 0, 1, 1);
    airplaneGLayout->addWidget(airplaneOtherButtonA, 1, 1, 1, 1);

    airplaneWidget->setLayout(airplaneGLayout);
    connect(wingDesignButton,&QPushButton::clicked,this,&mainWindow::changeWindowForWingDesign);
    connect(tailDesignButton,&QPushButton::clicked,this,&mainWindow::changeWindowForTailDesign);
    connect(airplaneDesignButton,&QPushButton::clicked,this,&mainWindow::changeWindowForAirplaneDesign);

}
void mainWindow::initialPowerWidget(){
    powerWidget = new QWidget();
    powerGLayout = new QGridLayout();
    propellerAnalyseButton = new imageButton(propAnalyseIcon,powerWidget);
    propellerDesignButton = new imageButton(propDesignIcon,powerWidget);
    powerOtherButtonA = new imageButton(propSolveIcon,powerWidget);
    powerOtherButtonB = new imageButton(flyvisionIcon,powerWidget);
    // Set size policy to ensure buttons expand to fill the space
    propellerAnalyseButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    propellerDesignButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    powerOtherButtonA->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    powerOtherButtonB->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    propellerAnalyseButton->setToolTip("螺旋桨分析");
    propellerDesignButton->setToolTip("螺旋桨设计");
    powerOtherButtonA->setToolTip("螺旋桨仿真");
    powerOtherButtonB->setToolTip("动力系统优化");


    //powerOtherButtonA->setEnabled(false);
    powerOtherButtonB->setEnabled(false);


    powerGLayout->addWidget(propellerAnalyseButton,0,0,1,1);
    powerGLayout->addWidget(propellerDesignButton,0,1,1,1);
    powerGLayout->addWidget(powerOtherButtonA,1,0,1,1);
    powerGLayout->addWidget(powerOtherButtonB,1,1,1,1);
    powerWidget->setLayout(powerGLayout);


    connect(propellerAnalyseButton,&QPushButton::clicked,this,&mainWindow::changeWindowForPropResultAnalyse);
    connect(propellerDesignButton,&QPushButton::clicked,this,&mainWindow::changeWindowForPropDesign);
    connect(powerOtherButtonA,&QPushButton::clicked,this,&mainWindow::changeWindowForPropAnalyse);
}
void mainWindow::initialOptimizationWidget(){
    optimizationWidget = new QWidget();
    optimizationGLayout = new QGridLayout();
    airfoilOptimizationButton = new imageButton(optimizationAirfoilIcon,optimizationWidget);
    wingOptimizationButton = new imageButton(optimizationWingIcon,optimizationWidget);
    propellerOptimizationButton = new imageButton(flyvisionIcon,optimizationWidget);
    optimizationOtherButtonA = new imageButton(flyvisionIcon,optimizationWidget);
    // Set size policy to ensure buttons expand to fill the space
    airfoilOptimizationButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    wingOptimizationButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    propellerOptimizationButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    optimizationOtherButtonA->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    airfoilOptimizationButton->setToolTip("翼型优化");
    wingOptimizationButton->setToolTip("机翼优化");
    propellerOptimizationButton->setToolTip("螺旋桨优化");
    optimizationOtherButtonA->setToolTip("整机优化");


    propellerOptimizationButton->setEnabled(false);
    optimizationOtherButtonA->setEnabled(false);


    // Add buttons to the layout
    optimizationGLayout->addWidget(airfoilOptimizationButton,0,0,1,1);
    optimizationGLayout->addWidget(wingOptimizationButton,0,1,1,1);
    optimizationGLayout->addWidget(propellerOptimizationButton,1,0,1,1);
    optimizationGLayout->addWidget(optimizationOtherButtonA,1,1,1,1);
    optimizationWidget->setLayout(optimizationGLayout);

    connect(airfoilOptimizationButton,&QPushButton::clicked,this,&mainWindow::changeWindowForAirfoilOptimization);
    connect(wingOptimizationButton,&QPushButton::clicked,this,&mainWindow::changeWindowForWingOptimization);
}
void mainWindow::startAnalyse(){
    switch (ANALYSE_TYPE) {
    case 0:
        designAirfoilWindow->solveDesignAirfoil();
        break;
    case 1:
        designAirfoilWindow->saveOptimizationSetting();

        break;
    case 2:
        designAirfoilWindow->startInterAnalyse();

        break;
    case 3:
        designWingWindow->startAnalyseWing();

        break;
    case 4:
        designWingWindow->startOptimizationWing();
        break;
    case 5:
        designPropellerWindow->startSolve();
        break;
    case 6:
        designPropellerWindow->startAnalyseProp();
        break;
    case 7:
        //designAirplaneWindow->startAnalyseAirplane();
        designAirplaneWindow->startAnalyseAirplaneStability();
    case 8:
        //designAirplaneWindow->startAnalyseAirplaneStability();
    default:
        break;
    }
}
void mainWindow::readData(){
    QAction *action = static_cast<QAction*>(sender());
    int index = action->property("index").toInt();
    if(!index)
        READ_TYPE = DATA_AIRFOIL;
    else
        READ_TYPE = DATA_PROJECT;


    switch (READ_TYPE) {
        case DATA_AIRFOIL:

            designAirfoilWindow->readData();
            break;
        case DATA_PROJECT:

            projectFile->readFile();//读取文件
            if(!designAirfoilWindow->airfoilArray.isEmpty() || !designWingWindow->wingDataArray.isEmpty()){
                QMessageBox msgBox;
                msgBox.setText("要覆盖之前的文件么");
                msgBox.setIcon(QMessageBox::Question);
                msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
                msgBox.setDefaultButton(QMessageBox::Cancel);

                int ret = msgBox.exec();
                switch (ret) {
                            case QMessageBox::Ok:
                                //projectFile->copyData(wingArray,airfoilArray,nameArray);
                                emit emitAirfoilArray(projectFile->airfoilArray,projectFile->nameArray);
                                emit emitWingArray(projectFile->wingArray);
                                emit emitTailArray(projectFile->tailArray);
                                emit emitPropellerArray(projectFile->propellerArray);
                                emit emitAirplaneArray(projectFile->airplaneArray);
                                break;
                            case QMessageBox::Cancel:
                                //handleCancel();  // 用户点击取消
                                break;
                            default:
                                // 其他处理
                                break;
                        }
            }else{
                //projectFile->copyData(wingArray,airfoilArray,nameArray);
                emit emitAirfoilArray(projectFile->airfoilArray,projectFile->nameArray);
                emit emitWingArray(projectFile->wingArray);
                emit emitTailArray(projectFile->tailArray);
                emit emitPropellerArray(projectFile->propellerArray);
                emit emitAirplaneArray(projectFile->airplaneArray);

            }
            break;
        default:
            break;
    }

}
void mainWindow::saveData(){

    projectFile->initialDataA(designWingWindow->wingDataArray,designWingWindow->airfoilArray,designWingWindow->AirfoilNameArray);
    projectFile->initialDataB(designWingWindow->tailDataArray,designPropellerWindow->propArray,designAirplaneWindow->airplaneDataArray);
    projectFile->writeFile();
    QMessageBox::information(this, "信息", "保存成功");
}
void mainWindow::changeWindowForHome(){
    replaceCentralWidget(homeWidget);

}
void mainWindow::changeWindowForAirport(){
    displayAirportWindow->showAirportDialog();
    replaceCentralWidget(displayAirportWindow);
}
void mainWindow::changeWindowForAirfoil(){
    replaceCentralWidget(airfoilWidget);
}
void mainWindow::changeWindowForAirplane(){
    replaceCentralWidget(airplaneWidget);
}
void mainWindow::changeWindowForPower(){
    replaceCentralWidget(powerWidget);
}
void mainWindow::changeWindowForOptimization(){
    replaceCentralWidget(optimizationWidget);
}
void mainWindow::changeWindowForAirfoilDesign(){
    designAirfoilWindow->modelType = NOTHING;
    designAirfoilWindow->readDataType = DESIGN;
    ANALYSE_TYPE = MODEL_AIRFOILDESIGN;
    READ_TYPE = DATA_AIRFOIL;

    replaceCentralWidget(designAirfoilWindow->airfoilDesignWidget);
    startAnalyseButton->setEnabled(true);
    airfoilActions->setEnabled(true);
}
void mainWindow::changeWindowForAirfoilInter(){
    ANALYSE_TYPE = MODEL_AIRFOILINTER;
    replaceCentralWidget(designAirfoilWindow->dragInterWidget);
    startAnalyseButton->setEnabled(true);
    airfoilActions->setEnabled(false);
}
void mainWindow::changeWindowForAirfoilOptimization(){
    designAirfoilWindow->modelType = NOTHING;
    designAirfoilWindow->readDataType = OPTIMIZATION;
    ANALYSE_TYPE = MODEL_AIRFOILOPTIMIZATION;

    replaceCentralWidget(designAirfoilWindow->airfoilOptimizationWidget);
    startAnalyseButton->setEnabled(true);
}
void mainWindow::changeWindowForWingDesign(){
    designWingWindow->changeWindowForWingDesign();
    ANALYSE_TYPE = MODEL_WINGDESIGN;

    replaceCentralWidget(designWingWindow);

    wingDefineActions->setEnabled(true);
    tailDefineActions->setEnabled(false);
    airplaneDefineActions->setEnabled(false);
    startAnalyseButton->setEnabled(true);
}
void mainWindow::changeWindowForTailDesign(){
    designWingWindow->changeWindowForTailDesign();
    ANALYSE_TYPE = MODEL_WINGDESIGN;

    replaceCentralWidget(designWingWindow);
    startAnalyseButton->setEnabled(true);
    wingDefineActions->setEnabled(false);
    tailDefineActions->setEnabled(true);
    airplaneDefineActions->setEnabled(false);
}
void mainWindow::changeWindowForAirplaneDesign(){
    designAirplaneWindow->changeWindowForAirplaneDesign();

    ANALYSE_TYPE = MODEL_AIRPLANEDESIGN;
    replaceCentralWidget(designAirplaneWindow);
    wingDefineActions->setEnabled(false);
    tailDefineActions->setEnabled(false);
    airplaneDefineActions->setEnabled(true);
    startAnalyseButton->setEnabled(true);
}
void mainWindow::changeWindowForWingOptimization(){
    designWingWindow->changeWindowForWingOptimization();
    ANALYSE_TYPE = MODEL_WINGOPTIMIZATION;

    replaceCentralWidget(designWingWindow);
    startAnalyseButton->setEnabled(true);
}
void mainWindow::changeWindowForPropResultAnalyse(){

    replaceCentralWidget(designPropellerWindow->propResultDisplayWidget);
    propActions->setEnabled(true);
}
void mainWindow::changeWindowForPropDesign(){
    ANALYSE_TYPE = MODEL_PROPDESIGN;

    replaceCentralWidget(designPropellerWindow->propDesignWidget);
    startAnalyseButton->setEnabled(true);
}
void mainWindow::changeWindowForPropAnalyse(){
    ANALYSE_TYPE = MODEL_PROPANALYSE;

    replaceCentralWidget(designPropellerWindow->propAnalyseWidget);
    startAnalyseButton->setEnabled(true);
    propellerDefineActions->setEnabled(true);
}
void mainWindow::replaceCentralWidget(QWidget *newWidget) {
    // 暂时禁用绘制更新
    //setUpdatesEnabled(false);

    // 移除当前的中心widget但不删除
    QWidget *oldCentralWidget = takeCentralWidget();

    // 隐藏旧的中心widget
    if (oldCentralWidget) {
        oldCentralWidget->hide();
    }

    // 设置新的中心widget
    setCentralWidget(newWidget);
    //设置开始分析按钮状态
    updateState();
    // 显示新的中心widget
    newWidget->show();



    // 恢复绘制更新
    //setUpdatesEnabled(true);
}
void mainWindow::closeEvent(QCloseEvent *){


}
void mainWindow::keyPressEvent(QKeyEvent *event){
    switch (event->key()){
        case Qt::Key_Enter:
        case Qt::Key_Return:

            startAnalyse();
            break;

        case Qt::Key_Escape:

            break;

        case Qt::Key_A:

            break;

        case Qt::Key_B:

            break;

    // 处理更多按键
        default:
        // 让基类处理未处理的按键
        QMainWindow::keyPressEvent(event);
            break;
    }
}
void mainWindow::showHelpHtml(){
    const QString path = QDir(QCoreApplication::applicationDirPath()).filePath("help/help.html");
    if (!QFileInfo::exists(path)) {
        QMessageBox::information(this, tr("帮助文档"), tr("当前安装包未提供帮助文档。"));
        return;
    }
    QDesktopServices::openUrl(QUrl::fromLocalFile(path));
}
void mainWindow::showTheoreticalFrameworkHtml(){
    const QDir applicationDir(QCoreApplication::applicationDirPath());
    QString path = applicationDir.filePath("theoreticalFramework/document.html");
    if (!QFileInfo::exists(path)) {
        path = applicationDir.filePath("../share/PassWing/theoreticalFramework/document.html");
    }
    if (!QFileInfo::exists(path)) {
        QMessageBox::warning(this, tr("理论文档"), tr("找不到理论文档。"));
        return;
    }
    QDesktopServices::openUrl(QUrl::fromLocalFile(QFileInfo(path).absoluteFilePath()));
}
void mainWindow::changeButtonState() {
    QToolButton *button = qobject_cast<QToolButton*>(sender());
    if (!button) return;

    // 恢复之前激活按钮的样式
    if (activeButton) {
        activeButton->setStyleSheet("");
    }

    // 改变当前按钮的样式使其看起来像鼠标悬停
    activeButton = button;

    activeButton->setStyleSheet("background-color: blue;");
}
void mainWindow::initialAboutMeDialog() {
    // 初始化 QDialog 对象
    aboutMeDialog = new QDialog(this);
    aboutMeDialog->setWindowTitle("飞视科技");
    //aboutMeDialog->setAttribute(Qt::WA_DeleteOnClose);  // 自动删除对话框

    // 创建 QLabel 控件
    titleAboutMeLabel = new QLabel("<h2>PassWing</h2>", aboutMeDialog);
    versionAboutMeLabel = new QLabel("<p>版本: 1.4</p>", aboutMeDialog);
    authorAboutMeLabel = new QLabel("<p>关注飞视科技公众号，了解最新版本消息</p>", aboutMeDialog);
    infoAboutMeLabel = new QLabel("<p>www.aikj.org</p>", aboutMeDialog);

    // 创建 QPushButton 控件
    closeAboutMeButton = new QPushButton("关闭", aboutMeDialog);
    QObject::connect(closeAboutMeButton, &QPushButton::clicked, aboutMeDialog, &QDialog::hide);

    // 创建 QVBoxLayout 布局并将控件添加到布局中
    vAboutMeLayout = new QVBoxLayout;
    vAboutMeLayout->addWidget(titleAboutMeLabel);
    vAboutMeLayout->addWidget(versionAboutMeLabel);
    vAboutMeLayout->addWidget(authorAboutMeLabel);
    vAboutMeLayout->addWidget(infoAboutMeLabel);
    vAboutMeLayout->addWidget(closeAboutMeButton);

    // 设置对话框的布局
    aboutMeDialog->setLayout(vAboutMeLayout);
}
void mainWindow::updateState(){
    airfoilActions->setEnabled(false);
    propActions->setEnabled(false);
    wingDefineActions->setEnabled(false);
    tailDefineActions->setEnabled(false);
    propellerDefineActions->setEnabled(false);

    startAnalyseButton->setEnabled(false);
}
mainWindow::~mainWindow(){

    designAirfoilWindow->saveAllSetting();
    designAirfoilWindow->deleteLater();

    designWingWindow->deleteLater();
    displayAirportWindow->stopHtmlLoading();


}
