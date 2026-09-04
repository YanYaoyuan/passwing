


#include <QtConcurrent/QtConcurrent>
#include <QtCharts/QLegend>
#include <QtCharts/QLegendMarker>
#include <QSplitter>
#include <QFileDialog>
#include <QMessageBox>
#include <QApplication>
#include <QButtonGroup>
#include "publicWidgetClass/datapointdialog.h"
#include "airplanedisplay.h"
#if defined(_MSC_VER) && (_MSC_VER >= 1600)
# pragma execution_character_set("utf-8")
#endif
airplaneDisplay::airplaneDisplay(QWidget *parent)
    : QWidget(parent){
    loadIcon();

    initialSteup();
    initialSpanRChartMenu();
    initialRChartMenu();

    initialAirplaneDesignWidget();
    initialAirplaneDefineDialog();
    initialAirplaneMessage();
    initialAirplaneShowModel();



    initialAirplaneAddZeroLiftDragDialog();
    initialChoiceDragDialog();

    initialListDialog();
    initialStreamDialog();
    initialProgressDialog();

    //chart左键
    QObject::connect(chartViewWA,&MyChartView::mouseRightLeftPress,this,&airplaneDisplay::showChartAMenu);
    QObject::connect(chartViewWB,&MyChartView::mouseRightLeftPress,this,&airplaneDisplay::showChartBMenu);
    QObject::connect(chartViewWC,&MyChartView::mouseRightLeftPress,this,&airplaneDisplay::showChartCMenu);
    QObject::connect(chartViewWD,&MyChartView::mouseRightLeftPress,this,&airplaneDisplay::showChartDMenu);
    QObject::connect(spanChartView,&MyChartView::mouseRightLeftPress,this,&airplaneDisplay::showSpanChartMenu);



}
void airplaneDisplay::initialSteup(){





    axisXName[0] = "alpha"; axisYName[0] = "Cl"; titleName[0] = "升力系数曲线";
    axisXName[1] = "alpha"; axisYName[1] = "Cd"; titleName[1] = "阻力系数曲线";
    axisXName[2] = "alpha"; axisYName[2] = "Cm"; titleName[2] = "力矩曲线";
    axisXName[3] = "alpha"; axisYName[3] = "升阻比"; titleName[3] = "升阻比曲线";
    axisXName[4] = "alpha"; axisYName[4] = "功率因子"; titleName[4] = "功率因子曲线";
    axisXName[5] = "Cd"; axisYName[5] = "Cl"; titleName[5] = "极曲线";
    axisXName[6] = "Cl"; axisYName[6] = "升阻比"; titleName[6] = "升力系数-升阻比曲线";
    axisXName[7] = "Cl"; axisYName[7] = "功率因子"; titleName[7] = "升力系数-功率因子曲线";
    axisXName[8] = "Cl"; axisYName[8] = "Cm"; titleName[8] = "升力系数-力矩曲线";

    axisXName[9] = "翼展(m)"; axisYName[9] = "环量"; titleName[9] = "环量分布曲线";
    axisXName[10] = "翼展(m)"; axisYName[10] = "升力系数"; titleName[10] = "升力系数分布曲线";
    axisXName[11] = "翼展(m)"; axisYName[11] = "弯矩分布(N/m)"; titleName[11] = "弯矩分布曲线";
    axisXName[12] = "翼展(m)"; axisYName[12] = "升力(N)"; titleName[12] = "升力分布曲线";




    axisIndex[0][0] = 0,axisIndex[0][1] = 1;
    axisIndex[1][0] = 0,axisIndex[1][1] = 2;
    axisIndex[2][0] = 0,axisIndex[2][1] = 3;
    axisIndex[3][0] = 0,axisIndex[3][1] = 4;
    axisIndex[4][0] = 0,axisIndex[4][1] = 5;
    axisIndex[5][0] = 2,axisIndex[5][1] = 1;
    axisIndex[6][0] = 1,axisIndex[6][1] = 4;
    axisIndex[7][0] = 1,axisIndex[7][1] = 5;
    axisIndex[8][0] = 1,axisIndex[8][1] = 3;

    xAxisIndexA = axisIndex[0][0], yAxisIndexA = axisIndex[0][1];
    xAxisIndexB = axisIndex[1][0], yAxisIndexB = axisIndex[1][1];
    xAxisIndexC = axisIndex[2][0], yAxisIndexC = axisIndex[2][1];
    xAxisIndexD = axisIndex[3][0], yAxisIndexD = axisIndex[3][1];

    chartTypeIndexArray[0] = 0;
    chartTypeIndexArray[1] = 1;
    chartTypeIndexArray[2] = 2;
    chartTypeIndexArray[3] = 3;



}
void airplaneDisplay::startAnalyseAirplaneStability(){

    getAirplaneSetting();
    bool isReady1 = airplaneDataArray[airplaneChoiceIndex].isChoice[0];
    bool isReady2 = airplaneDataArray[airplaneChoiceIndex].isChoice[1];
    bool isReady3 = airplaneDataArray[airplaneChoiceIndex].isChoice[2];
    int index1 = airplaneDataArray[airplaneChoiceIndex].compoentIndex[0];
    int index2 = airplaneDataArray[airplaneChoiceIndex].compoentIndex[1];
    int index3 = airplaneDataArray[airplaneChoiceIndex].compoentIndex[2];


    double aValue[4],bValue[4],cValue[4];
    aValue[0] = airplaneDataArray[airplaneChoiceIndex].xLocation[0];aValue[1] = 0;aValue[2] = airplaneDataArray[airplaneChoiceIndex].zLocation[0];aValue[3] = airplaneDataArray[airplaneChoiceIndex].aAlpha[0];
    bValue[0] = airplaneDataArray[airplaneChoiceIndex].xLocation[1];bValue[1] = 0;bValue[2] = airplaneDataArray[airplaneChoiceIndex].zLocation[1];bValue[3] = airplaneDataArray[airplaneChoiceIndex].aAlpha[1];
    cValue[0] = airplaneDataArray[airplaneChoiceIndex].xLocation[2];cValue[1] = 0;cValue[2] = airplaneDataArray[airplaneChoiceIndex].zLocation[2];cValue[3] = airplaneDataArray[airplaneChoiceIndex].aAlpha[2];

    if(isReady1){

        airplaneSolverArray[airplaneChoiceIndex]->importWingGeometry(wingDataArray[index1]);

    }

    if(isReady2){

        airplaneSolverArray[airplaneChoiceIndex]->importHTailGeometry(tailDataArray[index2]);

    }

    if(isReady3){

        airplaneSolverArray[airplaneChoiceIndex]->importVTailGeometry(tailDataArray[index3]);

    }

    airplaneVLM *solvers = airplaneSolverArray[airplaneChoiceIndex];
    airplaneStability *analyse = airplaneStabilityArray[airplaneChoiceIndex];
    solvers->setIsStability(true);
    solvers->setOffsetValue(aValue,bValue,cValue);
    solvers->setComponents(isReady1,isReady2,isReady3);

    //计算扰动场
    analyse->setStabilityValue(20,0);

    analyse->initialRHS();

    //
    solvers->setVinf(analyse->perturbationArray);

    solvers->refreshParaments(airplaneSettingArray[airplaneChoiceIndex]);
    solvers->initialize();




    //求解

    QFuture<void>future = QtConcurrent::run([solvers](){
        solvers->solver();

    });
    future.waitForFinished();


    solvers = nullptr;
    //


}
void airplaneDisplay::startAnalyseAirplane(){

    QVector<QVector<double>>resultTmp;
    getAirplaneSetting();
    bool isReady1 = airplaneDataArray[airplaneChoiceIndex].isChoice[0];
    bool isReady2 = airplaneDataArray[airplaneChoiceIndex].isChoice[1];
    bool isReady3 = airplaneDataArray[airplaneChoiceIndex].isChoice[2];
    int index1 = airplaneDataArray[airplaneChoiceIndex].compoentIndex[0];
    int index2 = airplaneDataArray[airplaneChoiceIndex].compoentIndex[1];
    int index3 = airplaneDataArray[airplaneChoiceIndex].compoentIndex[2];


    double aValue[4],bValue[4],cValue[4];
    aValue[0] = airplaneDataArray[airplaneChoiceIndex].xLocation[0];aValue[1] = 0;aValue[2] = airplaneDataArray[airplaneChoiceIndex].zLocation[0];aValue[3] = airplaneDataArray[airplaneChoiceIndex].aAlpha[0];
    bValue[0] = airplaneDataArray[airplaneChoiceIndex].xLocation[1];bValue[1] = 0;bValue[2] = airplaneDataArray[airplaneChoiceIndex].zLocation[1];bValue[3] = airplaneDataArray[airplaneChoiceIndex].aAlpha[1];
    cValue[0] = airplaneDataArray[airplaneChoiceIndex].xLocation[2];cValue[1] = 0;cValue[2] = airplaneDataArray[airplaneChoiceIndex].zLocation[2];cValue[3] = airplaneDataArray[airplaneChoiceIndex].aAlpha[2];

    if(isReady1){

        airplaneSolverArray[airplaneChoiceIndex]->importWingGeometry(wingDataArray[index1]);

    }

    if(isReady2){

        airplaneSolverArray[airplaneChoiceIndex]->importHTailGeometry(tailDataArray[index2]);

    }

    if(isReady3){

        airplaneSolverArray[airplaneChoiceIndex]->importVTailGeometry(tailDataArray[index3]);

    }

    airplaneVLM *solvers = airplaneSolverArray[airplaneChoiceIndex];






    solvers->setIsStability(false);
    solvers->setOffsetValue(aValue,bValue,cValue);
    solvers->setComponents(isReady1,isReady2,isReady3);
    solvers->refreshParaments(airplaneSettingArray[airplaneChoiceIndex]);
    solvers->initialize();

    QFuture<void>future = QtConcurrent::run([solvers](){
        solvers->solver();

    });


    future.waitForFinished();

    //保存计算结果
    QVector<double>tmp1,tmp2,tmp3,tmp4,tmp5,tmp6;
    for(int i = 0;i<solvers->cdArray.length();i++){
        double cl = solvers->clArray[i];
        double cd = solvers->cdArray[i];
        double cm = solvers->cMArray[i];
        tmp1.append(solvers->alphaArray[i]);
        tmp2.append(cl);
        tmp3.append(cd);
        tmp4.append(cm);
        tmp5.append(cl / cd);
        if(cl >= 0)
            tmp6.append(pow(cl,1.5) / cd);
        else
            tmp6.append(0);


    }

    resultTmp.append(tmp1);
    resultTmp.append(tmp2);
    resultTmp.append(tmp3);
    resultTmp.append(tmp4);
    resultTmp.append(tmp5);
    resultTmp.append(tmp6);
    resultArray[airplaneChoiceIndex] = resultTmp;

    //更新机翼设计迎角
    airplaneAlphaCombox->blockSignals(true);
    airplaneAlphaCombox->clear();
    for(int i = 0;i<tmp1.length();i++){
        airplaneAlphaCombox->addItem(QString::number(tmp1[i]));
    }


    airplaneAlphaCombox->blockSignals(false);


    solvers = nullptr;

    drawResultChartA(airplaneChoiceIndex);
    drawResultChartB(airplaneChoiceIndex);
    drawResultChartC(airplaneChoiceIndex);
    drawResultChartD(airplaneChoiceIndex);
    drawSpanResult(airplaneChoiceIndex);

    updateAxes(chartWA,airplaneResultSeriesA);
    updateAxes(chartWB,airplaneResultSeriesB);
    updateAxes(chartWC,airplaneResultSeriesC);
    updateAxes(chartWD,airplaneResultSeriesD);


    showResultNode(airplaneTreeItemArray[airplaneChoiceIndex],"后处理");


    if(airplaneDataArray.length() > airplaneChoiceIndex){

        int index = airplaneDataArray[airplaneChoiceIndex].compoentIndex[0];
        updateGLTextA(airplaneDataToString(wingDataArray[index]));
    }


}
void airplaneDisplay::loadIcon(){
    greenIcon = ":/images/airplane/greenStatus.png";
    redIcon = ":/images/airplane/redStatus.png";
}
void airplaneDisplay::initialAirplaneDefineDialog(){

    airplaneDefineDialog = new QDialog();
    wingBox = new QGroupBox(airplaneDefineDialog);
    fuselageBox = new QGroupBox(airplaneDefineDialog);
    hTailBox = new QGroupBox(airplaneDefineDialog);
    vTailBox = new QGroupBox(airplaneDefineDialog);
    powerBox = new QGroupBox(airplaneDefineDialog);
    messageBox = new QGroupBox(airplaneDefineDialog);
    defineGridLayout = new QGridLayout();
    //
    gLayout[0] = new QGridLayout(wingBox);
    gLayout[1] = new QGridLayout(hTailBox);
    gLayout[2] = new QGridLayout(vTailBox);
    gLayout[3] = new QGridLayout(powerBox);
    gLayout[4] = new QGridLayout(fuselageBox);

    QLabel *textLabelA = new QLabel("机翼",airplaneDefineDialog);
    QLabel *textLabelB = new QLabel("平尾",airplaneDefineDialog);
    QLabel *textLabelC = new QLabel("垂尾",airplaneDefineDialog);
    QLabel *textLabelD = new QLabel("动力",airplaneDefineDialog);
    QLabel *textLabelE = new QLabel("机身",airplaneDefineDialog);



    airplaneDefineDialog->setWindowTitle("飞机设计");
    gLayout[0]->addWidget(textLabelA,0,0,1,1);
    gLayout[1]->addWidget(textLabelB,0,0,1,1);
    gLayout[2]->addWidget(textLabelC,0,0,1,1);
    gLayout[3]->addWidget(textLabelD,0,0,1,1);
    gLayout[4]->addWidget(textLabelE,0,0,1,1);

    for(int i = 0;i<5;i++){

        listIndexArray[i] = 0;

        choiceButton[i] = new QPushButton("选择",airplaneDefineDialog);
        xLocationLabel[i] = new QLabel("X位置(米)",airplaneDefineDialog);
        zLocationLabel[i] = new QLabel("Z位置(米)",airplaneDefineDialog);
        aAlphaLabel[i] = new QLabel("安装角(°)",airplaneDefineDialog);
        if(i>0 && i<3){
            xLocationEdit[i] = new QLineEdit("1",airplaneDefineDialog);

        }else{
            xLocationEdit[i] = new QLineEdit("0",airplaneDefineDialog);
        }

        zLocationEdit[i] = new QLineEdit("0",airplaneDefineDialog);
        aAlphaEdit[i] = new QLineEdit("0",airplaneDefineDialog);
        readyButton[i] = new QPushButton(airplaneDefineDialog);

        readyButton[i]->setStyleSheet("QPushButton { border: none; background: transparent; }");
        readyButton[i]->setIcon(QIcon(redIcon));
        readyButton[i]->setIconSize(QSize(40, 40)); // 设置图标显示尺寸为 40x40
        readyButton[i]->setProperty("status",0);
        choiceButton[i]->setProperty("index",i);
        gLayout[i]->addWidget(readyButton[i],0,3,1,1);
        gLayout[i]->addWidget(choiceButton[i],1,1,1,1);
        gLayout[i]->addWidget(xLocationLabel[i],2,1,1,1);
        gLayout[i]->addWidget(xLocationEdit[i],2,2,1,1);
        gLayout[i]->addWidget(zLocationLabel[i],3,1,1,1);
        gLayout[i]->addWidget(zLocationEdit[i],3,2,1,1);
        gLayout[i]->addWidget(aAlphaLabel[i],4,1,1,1);
        gLayout[i]->addWidget(aAlphaEdit[i],4,2,1,1);

        connect(readyButton[i],&QPushButton::clicked,this,&airplaneDisplay::setButtonStatus);
        connect(choiceButton[i],&QPushButton::clicked,this,&airplaneDisplay::showListDialog);
    }
    listIndexArray[5] = 0;







    wingBox->setFixedSize(400,400);
    fuselageBox->setFixedSize(400,400);
    hTailBox->setFixedSize(400,400);
    vTailBox->setFixedSize(400,400);
    powerBox->setFixedSize(400,400);
    messageBox->setFixedSize(400,400);





    defineGridLayout->addWidget(wingBox,0,0,1,1);
    defineGridLayout->addWidget(hTailBox,0,1,1,1);
    defineGridLayout->addWidget(vTailBox,0,2,1,1);
    defineGridLayout->addWidget(powerBox,1,0,1,1);
    defineGridLayout->addWidget(fuselageBox,1,1,1,1);
    defineGridLayout->addWidget(messageBox,1,2,1,1);
    airplaneDefineDialog->setLayout(defineGridLayout);

}
void airplaneDisplay::initialListDialog(){
    listDialog = new QDialog();
    listView = new QListView(listDialog);
    listLayout = new QVBoxLayout();
    listModel = new QStringListModel();
    listView->setModel(listModel);
    QHBoxLayout *layout = new QHBoxLayout();
    QPushButton *confirmButton = new QPushButton("确认",listDialog);
    QPushButton *cancelButton = new QPushButton("取消",listDialog);
    listLayout->addWidget(listView);
    layout->addWidget(confirmButton);
    layout->addWidget(cancelButton);
    listLayout->addLayout(layout);
    listDialog->setLayout(listLayout);
    connect(listView->selectionModel(),&QItemSelectionModel::selectionChanged,this,&airplaneDisplay::updateListViewIndex);
    connect(confirmButton,&QPushButton::clicked,listDialog,&QDialog::close);
}
void airplaneDisplay::initialAirplaneAddZeroLiftDragDialog(){
    zeroLiftDragChoiceDialog = new QDialog();
    int row = 3;
    int col = 3;
    QSize size(300,145);
    QString path = QDir::currentPath() + "/resoure/images/liftDrag/";
    zeroLiftDragLayout = new QGridLayout(zeroLiftDragChoiceDialog);
    for(int i = 0;i<row;i++){
        for(int j = 0;j<col;j++){
            int G = i * col + j + 1;
            QPushButton *button = new QPushButton(zeroLiftDragChoiceDialog);
            zeroLiftDragLayout->addWidget(button,i,j,1,1);
            QPixmap map;
            map.load(path + QString::number(G) +  ".png");
            QPixmap scaledPixmap = map.scaled(size,Qt::IgnoreAspectRatio,Qt::SmoothTransformation);
            button->setIcon(QIcon(scaledPixmap));
            //button->setStyleSheet(buttonStyle);
            button->setIconSize(size);
            button->setFixedSize(size);
            button->setProperty("button",G);
            dragButtonArray.append(button);
            connect(button,&QPushButton::clicked,this,&airplaneDisplay::changeZeroLiftDragText);
        }
    }
    zeroLiftDragValueArray[0] = 0.0515;
    zeroLiftDragValueArray[1] = 0.0375;
    zeroLiftDragValueArray[2] = 0.0303;
    zeroLiftDragValueArray[3] = 0.0432;
    zeroLiftDragValueArray[4] = 0.0471;
    zeroLiftDragValueArray[5] = 0.0408;
    zeroLiftDragValueArray[6] = 0.0422;
    zeroLiftDragValueArray[7] = 0.0617;
    zeroLiftDragValueArray[8] = 0.0345;


    saveDragButton = new QPushButton(zeroLiftDragChoiceDialog);
    cancelDragButton = new QPushButton(zeroLiftDragChoiceDialog);

    dragEdit = new QLineEdit(zeroLiftDragChoiceDialog);

    zeroLiftDragLayout->addWidget(saveDragButton,row,0,1,1);
    zeroLiftDragLayout->addWidget(dragEdit,row,col - 2,1,1);
    zeroLiftDragLayout->addWidget(cancelDragButton,row,col - 1,1,1);
    saveDragButton->setText("确认");
    cancelDragButton->setText("取消");
    connect(saveDragButton,&QPushButton::clicked,this,&airplaneDisplay::setZeroLiftDrag);
    connect(saveDragButton,&QPushButton::clicked,zeroLiftDragChoiceDialog,&QDialog::hide);
}
void airplaneDisplay::initialAirplaneDesignWidget(){

    initialChartWA();
    initialChartWB();
    initialChartWC();
    initialChartWD();
    initialSpanChart();
    //
    airplaneDisplayWidgetA = new QVTKOpenGLNativeWidget(this);
    chartWWidget = new QWidget(this);
    airplaneTreeWidget = new QTreeWidget(this);

    airplaneHLayout = new QHBoxLayout(this);

    airplaneVLayout = new QVBoxLayout();
    airplaneChartVLayout = new QVBoxLayout(chartWWidget);


    // 设置树形控件
    airplaneTreeWidget->setColumnCount(1); // 设置列
    airplaneTreeWidget->setHeaderLabel(tr("飞机")); // 设置标题
    airplaneTreeWidget->setMaximumWidth(300);



    // 初始化布局
    airplaneSettingDialog = new QDialog();
    airplaneSettingLayout = new QGridLayout(airplaneSettingDialog);


    airplaneWidgetVLayout = new QVBoxLayout();
    viewChangeHLayout = new QHBoxLayout();


    curveIconA1 = ":/images/window/airplane/wing/3D.png";
    curveIconA2 = ":/images/window/airplane/wing/curve1.png";
    curveIconA3 = ":/images/window/airplane/wing/curve2.png";

    curveIconB1 = ":/images/window/airplane/wing/3DB.png";
    curveIconB2 = ":/images/window/airplane/wing/curve3.png";
    curveIconB3 = ":/images/window/airplane/wing/curve4.png";
    // 设置下拉框
    airplaneAlphaCombox = new QComboBox();
    airplaneAlphaCombox->setFixedSize(60,40);
    // 创建按钮
    changeModelViewButton = new hoverButton();
    changeResultViewButton = new hoverButton();
    changeSpanResultViewButton = new hoverButton();

    changeModelViewButton->setButtonIcon(curveIconB1,QSize(40,40));
    changeResultViewButton->setButtonIcon(curveIconA2,QSize(40,40));
    changeSpanResultViewButton->setButtonIcon(curveIconA3,QSize(40,40));

    changeModelViewButton->setEnabled(false);

    // 创建标签
    airplaneMessageTextLabel = new QLabel();
    airplaneMessageTextLabel->setFixedHeight(30);

    // 创建分割器
    QSplitter *vSplitter = new QSplitter(Qt::Vertical);
    QSplitter *hSplitterTop = new QSplitter(Qt::Horizontal);
    QSplitter *hSplitterBottom = new QSplitter(Qt::Horizontal);

    hSplitterTop->addWidget(chartViewWA);
    hSplitterTop->addWidget(chartViewWB);
    hSplitterBottom->addWidget(chartViewWC);
    hSplitterBottom->addWidget(chartViewWD);

    vSplitter->addWidget(hSplitterTop);
    vSplitter->addWidget(hSplitterBottom);
    vSplitter->addWidget(spanChartView);

    // 将分割器添加到布局中
    airplaneChartVLayout->addWidget(vSplitter);



    // 添加控件到水平布局中
    viewChangeHLayout->addWidget(changeModelViewButton);
    viewChangeHLayout->addWidget(changeResultViewButton);
    viewChangeHLayout->addWidget(changeSpanResultViewButton);
    viewChangeHLayout->addWidget(airplaneAlphaCombox);
    viewChangeHLayout->addWidget(airplaneMessageTextLabel);
    // 添加控件到垂直布局中
    airplaneWidgetVLayout->addLayout(viewChangeHLayout);
    airplaneWidgetVLayout->addWidget(airplaneDisplayWidgetA);
    airplaneWidgetVLayout->addWidget(chartWWidget);
    airplaneWidgetVLayout->addWidget(spanChartView);

    // 添加树形控件到垂直布局中

    // 将所有布局添加到主水平布局中
    airplaneHLayout->addWidget(airplaneTreeWidget);
    airplaneHLayout->addLayout(airplaneWidgetVLayout);

    setLayout(airplaneHLayout);

    airplaneHLayout->setStretch(0, 1);
    airplaneHLayout->setStretch(1, 3);







    initialAirplaneSettingDialog();
    connect(airplaneTreeWidget, &QTreeWidget::itemClicked, this, &airplaneDisplay::changeAirplaneDisplay);
    connect(airplaneTreeWidget, &QTreeWidget::itemClicked, this, &airplaneDisplay::changeAirplaneData);
    connect(airplaneTreeWidget, &QTreeWidget::itemClicked, this, &airplaneDisplay::showAirplaneSettingVinfDialog);
    connect(airplaneTreeWidget, &QTreeWidget::itemClicked, this, &airplaneDisplay::showAirplaneCompoentDefineWidget);
    connect(airplaneTreeWidget, &QTreeWidget::itemChanged, this, &airplaneDisplay::setAirplaneXfoilDrag);
    connect(airplaneTreeWidget, &QTreeWidget::itemChanged, this, &airplaneDisplay::showAirplanePressureContour);
    connect(airplaneTreeWidget, &QTreeWidget::itemChanged, this, &airplaneDisplay::showAirplaneStreamLineView);
    connect(airplaneTreeWidget, &QTreeWidget::itemChanged, this, &airplaneDisplay::showAirplaneZeroLiftDragDialog);
    connect(airplaneTreeWidget, &QTreeWidget::itemClicked, this, &airplaneDisplay::closeOtherRoots);

    connect(changeModelViewButton, &QPushButton::clicked, this, &airplaneDisplay::changeUIForAirplaneDesign);
    connect(changeResultViewButton, &QPushButton::clicked, this, &airplaneDisplay::changeUIForAirplaneResult);
    connect(changeSpanResultViewButton, &QPushButton::clicked, this, &airplaneDisplay::changeUIForAirplaneSpanResult);
    connect(airplaneAlphaCombox, QOverload<int>::of(&QComboBox::activated),
            this, &airplaneDisplay::changeAlphaView);

}
void airplaneDisplay::initialAirplaneSettingDialog(){
    airplaneVelocityLabel = new QLabel(airplaneSettingDialog);
    airplaneVelocityEdit = new QLineEdit(airplaneSettingDialog);
    airplaneVelocityUnitLabel = new QLabel(airplaneSettingDialog);
    airplaneHeightLabel = new QLabel(airplaneSettingDialog);
    airplaneHeightEdit = new QLineEdit(airplaneSettingDialog);
    airplaneHeightUnitLabel = new QLabel(airplaneSettingDialog);
    airplaneCgLocationLabel = new QLabel(airplaneSettingDialog);



    airplaneMinAlphaLabel = new QLabel(airplaneSettingDialog);
    airplaneMinAlphaEdit = new QLineEdit(airplaneSettingDialog);
    airplaneMaxAlphaLabel = new QLabel(airplaneSettingDialog);
    airplaneMaxAlphaEdit = new QLineEdit(airplaneSettingDialog);
    airplaneStepAlphaLabel = new QLabel(airplaneSettingDialog);
    airplaneStepAlphaEdit = new QLineEdit(airplaneSettingDialog);
    airplaneCgLocationEdit = new QLineEdit(airplaneSettingDialog);
    airplaneCgLocationCheckBox = new QCheckBox(airplaneSettingDialog);



    saveAirplaneSettingButton = new QPushButton(airplaneSettingDialog);
    cancelAirplaneSettingButton = new QPushButton(airplaneSettingDialog);



    airplaneVelocityLabel->setText("速度");
    airplaneVelocityEdit->setText("20");
    airplaneVelocityUnitLabel->setText("m/s");



    airplaneHeightLabel->setText("高度");
    airplaneHeightEdit->setText("0");
    airplaneHeightUnitLabel->setText("m");

    airplaneMinAlphaLabel->setText("最小迎角");
    airplaneMinAlphaEdit->setText("0");

    airplaneMaxAlphaLabel->setText("最大迎角");
    airplaneMaxAlphaEdit->setText("10");

    airplaneStepAlphaLabel->setText("迎角步长");
    airplaneStepAlphaEdit->setText("1");

    airplaneCgLocationLabel->setText("力矩参考点");
    airplaneCgLocationEdit->setText("0");
    airplaneCgLocationCheckBox->setChecked(true);


    saveAirplaneSettingButton->setText("保存");
    cancelAirplaneSettingButton->setText("取消");



    airplaneSettingLayout->addWidget(airplaneVelocityLabel,0,0,1,1);
    airplaneSettingLayout->addWidget(airplaneVelocityEdit,0,1,1,1);
    airplaneSettingLayout->addWidget(airplaneVelocityUnitLabel,0,2,1,1);

    airplaneSettingLayout->addWidget(airplaneHeightLabel,1,0,1,1);
    airplaneSettingLayout->addWidget(airplaneHeightEdit,1,1,1,1);
    airplaneSettingLayout->addWidget(airplaneHeightUnitLabel,1,2,1,1);



    airplaneSettingLayout->addWidget(airplaneMinAlphaLabel,2,0,1,1);
    airplaneSettingLayout->addWidget(airplaneMinAlphaEdit,2,1,1,1);
    airplaneSettingLayout->addWidget(airplaneMaxAlphaLabel,3,0,1,1);
    airplaneSettingLayout->addWidget(airplaneMaxAlphaEdit,3,1,1,1);
    airplaneSettingLayout->addWidget(airplaneStepAlphaLabel,4,0,1,1);
    airplaneSettingLayout->addWidget(airplaneStepAlphaEdit,4,1,1,1);
    airplaneSettingLayout->addWidget(airplaneCgLocationLabel,5,0,1,1);
    airplaneSettingLayout->addWidget(airplaneCgLocationEdit,5,1,1,1);
    airplaneSettingLayout->addWidget(airplaneCgLocationCheckBox,5,2,1,1);

    airplaneSettingLayout->addWidget(saveAirplaneSettingButton,6,0,1,1);
    airplaneSettingLayout->addWidget(cancelAirplaneSettingButton,6,2,1,1);

    connect(saveAirplaneSettingButton,&QPushButton::clicked,this,&airplaneDisplay::getAirplaneSetting);
    connect(airplaneCgLocationCheckBox,&QCheckBox::stateChanged,this,&airplaneDisplay::onCheckboxStateChanged);
    connect(airplaneCgLocationEdit,&QLineEdit::textChanged,this,&airplaneDisplay::changeCgLocation);
}
void airplaneDisplay::initialAirplaneShowModel(){


    renderer = vtkSmartPointer<vtkRenderer>::New();
    //renderer->SetBackground(0, 0, 0);

    renwin = vtkSmartPointer<vtkGenericOpenGLRenderWindow>::New();
    renwin->AddRenderer(renderer);

    //airplaneDisplayWidgetA = new QVTKOpenGLNativeWidget;
    airplaneDisplayWidgetA->setRenderWindow(renwin);
    airplaneDisplayWidgetA->setMinimumHeight(400);

    sphereSourceCg = vtkSmartPointer<vtkSphereSource>::New();
    sphereSourceCg->SetRadius(0.02);
    sphereSourceCg->SetThetaResolution(50);
    sphereSourceCg->SetPhiResolution(50);

   //创建机翼
    pointsWing = vtkSmartPointer<vtkPoints>::New();
    linesWing = vtkSmartPointer<vtkCellArray>::New();
    profilePolyDataWing = vtkSmartPointer<vtkPolyData>::New();
    transformWing = vtkSmartPointer<vtkTransform>::New();
    appendFilterWing = vtkSmartPointer<vtkAppendPolyData>::New();
    pointsContourWing = vtkSmartPointer<vtkPoints>::New();
    linesContourWing = vtkSmartPointer<vtkCellArray>::New();
    profilePolyDataContourWing = vtkSmartPointer<vtkPolyData>::New();

    profilePolyDataWing->SetPoints(pointsWing);
    profilePolyDataContourWing->SetPoints(pointsContourWing);
    profilePolyDataWing->SetPolys(linesWing);
    profilePolyDataContourWing->SetPolys(linesContourWing);


    appendFilterWing->AddInputData(profilePolyDataWing);
    appendFilterWing->Update();
    // 创建Mapper和Actor
    mapperWing = vtkSmartPointer<vtkPolyDataMapper>::New();
    mapperContourWing = vtkSmartPointer<vtkPolyDataMapper>::New();
    actorWing = vtkSmartPointer<vtkActor>::New();
    actorContourWing = vtkSmartPointer<vtkActor>::New();

    mapperWing->SetInputConnection(appendFilterWing->GetOutputPort());
    mapperContourWing->SetInputData(profilePolyDataContourWing);
    actorWing->SetMapper(mapperWing);
    actorContourWing->SetMapper(mapperContourWing);

    //创建垂尾
    pointsVTail = vtkSmartPointer<vtkPoints>::New();
    linesVTail = vtkSmartPointer<vtkCellArray>::New();
    profilePolyDataVTail = vtkSmartPointer<vtkPolyData>::New();
    transformVTail = vtkSmartPointer<vtkTransform>::New();
    appendFilterVTail = vtkSmartPointer<vtkAppendPolyData>::New();
    pointsContourVTail = vtkSmartPointer<vtkPoints>::New();
    linesContourVTail = vtkSmartPointer<vtkCellArray>::New();
    profilePolyDataContourVTail = vtkSmartPointer<vtkPolyData>::New();

    profilePolyDataVTail->SetPoints(pointsVTail);
    profilePolyDataContourVTail->SetPoints(pointsContourVTail);
    profilePolyDataVTail->SetPolys(linesVTail);
    profilePolyDataContourVTail->SetPolys(linesContourVTail);

    appendFilterVTail->AddInputData(profilePolyDataVTail);
    appendFilterVTail->Update();
    // 创建Mapper和Actor
    mapperVTail = vtkSmartPointer<vtkPolyDataMapper>::New();
    mapperContourVTail = vtkSmartPointer<vtkPolyDataMapper>::New();
    actorVTail = vtkSmartPointer<vtkActor>::New();
    actorContourVTail = vtkSmartPointer<vtkActor>::New();


    mapperVTail->SetInputConnection(appendFilterVTail->GetOutputPort());
    mapperContourVTail->SetInputData(profilePolyDataContourVTail);
    actorVTail->SetMapper(mapperVTail);
    actorContourVTail->SetMapper(mapperContourVTail);
    //创建平尾
    pointsHTail = vtkSmartPointer<vtkPoints>::New();
    linesHTail = vtkSmartPointer<vtkCellArray>::New();
    profilePolyDataHTail = vtkSmartPointer<vtkPolyData>::New();
    transformHTail = vtkSmartPointer<vtkTransform>::New();
    appendFilterHTail = vtkSmartPointer<vtkAppendPolyData>::New();
    pointsContourHTail = vtkSmartPointer<vtkPoints>::New();
    linesContourHTail = vtkSmartPointer<vtkCellArray>::New();
    profilePolyDataContourHTail = vtkSmartPointer<vtkPolyData>::New();

    profilePolyDataHTail->SetPoints(pointsHTail);
    profilePolyDataContourHTail->SetPoints(pointsContourHTail);
    profilePolyDataHTail->SetPolys(linesHTail);
    profilePolyDataContourHTail->SetPolys(linesContourHTail);

    appendFilterHTail->AddInputData(profilePolyDataHTail);
    appendFilterHTail->Update();
    // 创建Mapper和Actor
    mapperHTail = vtkSmartPointer<vtkPolyDataMapper>::New();
    mapperContourHTail = vtkSmartPointer<vtkPolyDataMapper>::New();
    actorHTail = vtkSmartPointer<vtkActor>::New();
    actorContourHTail = vtkSmartPointer<vtkActor>::New();

    mapperHTail->SetInputConnection(appendFilterHTail->GetOutputPort());
    mapperContourHTail->SetInputData(profilePolyDataContourHTail);
    actorHTail->SetMapper(mapperHTail);
    actorContourHTail->SetMapper(mapperContourHTail);
    //创建机身
    pointsFuselage = vtkSmartPointer<vtkPoints>::New();
    linesFuselage = vtkSmartPointer<vtkCellArray>::New();
    profilePolyDataFuselage = vtkSmartPointer<vtkPolyData>::New();
    transformFuselage = vtkSmartPointer<vtkTransform>::New();
    profilePolyDataFuselage->SetPoints(pointsFuselage);
    profilePolyDataFuselage->SetPolys(linesFuselage);
    appendFilterFuselage = vtkSmartPointer<vtkAppendPolyData>::New();
    appendFilterFuselage->AddInputData(profilePolyDataFuselage);
    appendFilterFuselage->Update();
    // 创建Mapper和Actor
    mapperFuselage = vtkSmartPointer<vtkPolyDataMapper>::New();
    mapperFuselage->SetInputConnection(appendFilterFuselage->GetOutputPort());
    actorFuselage = vtkSmartPointer<vtkActor>::New();
    actorFuselage->SetMapper(mapperFuselage);
    //创建模板尾翼
    pointsTTail = vtkSmartPointer<vtkPoints>::New();
    linesTTail = vtkSmartPointer<vtkCellArray>::New();
    profilePolyDataTTail = vtkSmartPointer<vtkPolyData>::New();
    transformTTail = vtkSmartPointer<vtkTransform>::New();
    profilePolyDataTTail->SetPoints(pointsTTail);
    profilePolyDataTTail->SetPolys(linesTTail);
    appendFilterTTail = vtkSmartPointer<vtkAppendPolyData>::New();
    appendFilterTTail->AddInputData(profilePolyDataTTail);
    appendFilterTTail->Update();
    // 创建Mapper和Actor
    mapperTTail = vtkSmartPointer<vtkPolyDataMapper>::New();
    mapperTTail->SetInputConnection(appendFilterTTail->GetOutputPort());
    actorTTail = vtkSmartPointer<vtkActor>::New();
    actorTTail->SetMapper(mapperTTail);

    //创建流线
    pointsS = vtkSmartPointer<vtkPoints>::New();
    linesS = vtkSmartPointer<vtkCellArray>::New();
    profilePolyDataS = vtkSmartPointer<vtkPolyData>::New();
    profilePolyDataS->SetPoints(pointsS);
    profilePolyDataS->SetLines(linesS);
    mapperS = vtkSmartPointer<vtkPolyDataMapper>::New();
    mapperS->SetInputData(profilePolyDataS);
    actorS = vtkSmartPointer<vtkActor>::New();
    actorS->GetProperty()->SetColor(0.8, 0.6, 1.0); // 红色
    actorS->SetMapper(mapperS);
    //
    actorWing->SetUserTransform(transformWing);
    actorHTail->SetUserTransform(transformHTail);
    actorVTail->SetUserTransform(transformVTail);
    actorTTail->SetUserTransform(transformTTail);
    actorFuselage->SetUserTransform(transformFuselage);
    //
    colorWing = vtkSmartPointer<vtkUnsignedCharArray>::New();
    colorWing->SetNumberOfComponents(3); // 设置为 RGB
    colorWing->SetName("Colors");
    profilePolyDataWing->GetCellData()->SetScalars(colorWing);

    colorContourWing = vtkSmartPointer<vtkUnsignedCharArray>::New();
    colorContourWing->SetNumberOfComponents(3); // 设置为 RGB
    colorContourWing->SetName("Colors");
    profilePolyDataContourWing->GetCellData()->SetScalars(colorContourWing);

    colorVTail = vtkSmartPointer<vtkUnsignedCharArray>::New();
    colorVTail->SetNumberOfComponents(3); // 设置为 RGB
    colorVTail->SetName("Colors");
    profilePolyDataVTail->GetCellData()->SetScalars(colorVTail);


    colorContourVTail = vtkSmartPointer<vtkUnsignedCharArray>::New();
    colorContourVTail->SetNumberOfComponents(3); // 设置为 RGB
    colorContourVTail->SetName("Colors");
    profilePolyDataContourVTail->GetCellData()->SetScalars(colorContourWing);


    colorHTail = vtkSmartPointer<vtkUnsignedCharArray>::New();
    colorHTail->SetNumberOfComponents(3); // 设置为 RGB
    colorHTail->SetName("Colors");
    profilePolyDataHTail->GetCellData()->SetScalars(colorHTail);

    colorContourHTail = vtkSmartPointer<vtkUnsignedCharArray>::New();
    colorContourHTail->SetNumberOfComponents(3); // 设置为 RGB
    colorContourHTail->SetName("Colors");
    profilePolyDataContourHTail->GetCellData()->SetScalars(colorContourWing);


    colorTTail = vtkSmartPointer<vtkUnsignedCharArray>::New();
    colorTTail->SetNumberOfComponents(3); // 设置为 RGB
    colorTTail->SetName("Colors");
    profilePolyDataTTail->GetCellData()->SetScalars(colorTTail);

    colorFuselage = vtkSmartPointer<vtkUnsignedCharArray>::New();
    colorFuselage->SetNumberOfComponents(3); // 设置为 RGB
    colorFuselage->SetName("Colors");
    profilePolyDataFuselage->GetCellData()->SetScalars(colorFuselage);


    mapperCg = vtkSmartPointer<vtkPolyDataMapper>::New();
    mapperCg->SetInputConnection(sphereSourceCg->GetOutputPort());;
    actorCg = vtkSmartPointer<vtkActor>::New();
    actorCg->SetMapper(mapperCg);
    actorCg->GetProperty()->SetColor(1.0, 0.0, 0.0);

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

    // 将 Actor 添加到渲染器中
    renderer->AddActor(actorWing);
    renderer->AddActor(actorVTail);
    renderer->AddActor(actorHTail);
    renderer->AddActor(actorFuselage);
    renderer->AddActor(actorTTail);
    renderer->AddActor(actorCg);
    renderer->AddActor(actorContourWing);
    renderer->AddActor(actorContourHTail);
    renderer->AddActor(actorContourVTail);
    renderer->AddActor(actorS);


    renderer->AddActor2D(textActorA);

    actorWing->VisibilityOff();
    actorVTail->VisibilityOff();
    actorHTail->VisibilityOff();
    actorFuselage->VisibilityOff();
    actorTTail->VisibilityOff();
    actorContourWing->VisibilityOff();
    actorContourHTail->VisibilityOff();
    actorContourVTail->VisibilityOff();



    // 设置背景颜色
    renderer->SetBackground(0.1, 0.2, 0.4); // 深蓝色背景


    // 设置光照效果
    lightKit = vtkSmartPointer<vtkLightKit>::New();
    lightKit->AddLightsToRenderer(renderer);

    // 获取 QVTKOpenGLNativeWidget 提供的交互器
    vtkRenderWindowInteractor* interactor = this->airplaneDisplayWidgetA->interactor();

    // 创建并设置交互样式
    vtkSmartPointer<vtkInteractorStyleTrackballCamera> style = vtkSmartPointer<vtkInteractorStyleTrackballCamera>::New();
    interactor->SetInteractorStyle(style);

    // 创建坐标轴
    vtkSmartPointer<vtkAxesActor> axes = vtkSmartPointer<vtkAxesActor>::New();
    axes->SetTotalLength(2.0, 2.0, 2.0); // 设置坐标轴的长度
    axes->SetShaftType(0);
    axes->SetCylinderRadius(0.05);

    // 创建 OrientationMarkerWidget
    orientationMarkerA = vtkSmartPointer<vtkOrientationMarkerWidget>::New();
    orientationMarkerA->SetOrientationMarker(axes);
    orientationMarkerA->SetInteractor(interactor);
    orientationMarkerA->SetViewport(0.8, 0.0, 1.0, 0.2); // 设置坐标轴的位置和大小
    orientationMarkerA->SetEnabled(1);
    orientationMarkerA->InteractiveOff(); // 禁止交互


    // 渲染并启动交互
    renwin->Render();

}
void airplaneDisplay::initialAirplaneMessage(){
    QGridLayout *gLayout = new QGridLayout(messageBox);
    for(int i = 0;i<20;i++){
        QLabel *label = new QLabel(airplaneDefineDialog);
        labelArray[i] = label;
    }
    labelArray[0]->setText("翼展");
    labelArray[1]->setText("翼面积");
    labelArray[2]->setText("展弦比");
    labelArray[3]->setText("平尾面积");
    labelArray[4]->setText("垂尾面积");
    labelArray[5]->setText("参考力臂");
    labelArray[6]->setText("平尾容量");
    labelArray[7]->setText("垂尾容量");
    labelArray[8]->setText("名称");

    QSpacerItem *verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);
    nameEdit = new QLineEdit(airplaneDefineDialog);
    nameEdit->setText("我的飞机");
    QPushButton *saveButton = new QPushButton("确认",airplaneDefineDialog);
    QPushButton *cancelButton = new QPushButton("取消",airplaneDefineDialog);

    gLayout->addWidget(labelArray[0],0,0,1,1);
    gLayout->addWidget(labelArray[1],1,0,1,1);
    gLayout->addWidget(labelArray[2],2,0,1,1);
    gLayout->addWidget(labelArray[3],3,0,1,1);
    gLayout->addWidget(labelArray[4],4,0,1,1);
    gLayout->addWidget(labelArray[5],5,0,1,1);
    gLayout->addWidget(labelArray[6],6,0,1,1);
    gLayout->addWidget(labelArray[7],7,0,1,1);

    gLayout->addItem(verticalSpacer,8,0,1,1);
    gLayout->addWidget(labelArray[8],9,0,1,1);
    gLayout->addWidget(nameEdit,9,1,1,1);
    gLayout->addWidget(saveButton,10,0,1,1);
    gLayout->addWidget(cancelButton,10,1,1,1);

    connect(saveButton,&QPushButton::clicked,this,&airplaneDisplay::saveAirplaneData);

}
void airplaneDisplay::initialProgressDialog(){

    // 初始化进度对话框
    progressDialog = new QDialog();
    QVBoxLayout *layout= new QVBoxLayout();
    dialogBar = new QProgressBar(progressDialog);
    layout->addWidget(dialogBar);
    progressDialog->setLayout(layout);

}
void airplaneDisplay::initialStreamDialog(){
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
    streamEditArray[1]->setText("-0.005");
    streamEditArray[2]->setText("0.01");
    streamEditArray[3]->setText("0.0005");

    connect(saveButton,&QPushButton::clicked,this,&airplaneDisplay::showAirplaneStreamLine);
    connect(cancelButton,&QPushButton::clicked,streamSettingDialog,&QDialog::hide);

}
void airplaneDisplay::initialChoiceDragDialog(){
    dragChoiceDialog = new QDialog();
    dragChoiceDialog->setWindowTitle("阻力来源");
    dragChoiceDialog->setFixedSize(300,150);

    // 使用垂直布局作为主布局
    QVBoxLayout *mainLayout = new QVBoxLayout(dragChoiceDialog);
    mainLayout->setContentsMargins(20, 20, 20, 20); // 整体边距

    // 创建水平布局放置复选框
    QHBoxLayout *checkBoxLayout = new QHBoxLayout();
    checkBoxLayout->setAlignment(Qt::AlignCenter); // 复选框居中

    // 创建按钮布局
    QHBoxLayout *buttonLayout = new QHBoxLayout();

    // 添加伸缩项将按钮推到两侧
    buttonLayout->addStretch(); // 左侧伸缩项，将确认按钮推到左边
    // 按钮之间可以加一个小的伸缩项增加间距
    buttonLayout->addStretch(1);

    dragBoxGroup = new QButtonGroup(dragChoiceDialog);
    dragBoxGroup->setExclusive(true); // 互斥模式

    dragBoxA = new QCheckBox("Xfoil求解器");
    dragBoxB = new QCheckBox("本地数据库");
    QPushButton *button1 = new QPushButton("确认");
    QPushButton *button2 = new QPushButton("取消");

    // 添加复选框到布局
    checkBoxLayout->addWidget(dragBoxA);
    checkBoxLayout->addWidget(dragBoxB);

    // 添加按钮到布局（注意顺序）
    buttonLayout->addWidget(button1);  // 确认按钮（左侧）
    buttonLayout->addSpacing(10);      // 按钮之间的固定间距
    buttonLayout->addWidget(button2);  // 取消按钮（右侧）
    buttonLayout->addStretch(2);       // 右侧伸缩项，将取消按钮推到右边

    // 将子布局添加到主布局
    mainLayout->addLayout(checkBoxLayout);
    mainLayout->addStretch(); // 复选框和按钮之间的伸缩项，将按钮推到底部
    mainLayout->addLayout(buttonLayout);

    dragBoxGroup->addButton(dragBoxA);
    dragBoxGroup->addButton(dragBoxB);

    dragChoiceDialog->setLayout(mainLayout);
    connect(button1,&QPushButton::clicked,this,&airplaneDisplay::saveDragDialog);
    connect(button2,&QPushButton::clicked,this,&airplaneDisplay::cancelDragDialog);

}
void airplaneDisplay::saveAirplaneData(){
    airplaneDefineDialog->hide();
    airplaneDefinition airplaneData;
    for(int i = 0;i<3;i++){
        double x = xLocationEdit[i]->text().toDouble();
        double a = aAlphaEdit[i]->text().toDouble();
        double z = zLocationEdit[i]->text().toDouble();
        airplaneData.xLocation[i] = x;
        airplaneData.aAlpha[i] = a;
        airplaneData.zLocation[i] = z;

    }

    double aValue[4],bValue[4],cValue[4];
    aValue[0] = airplaneData.xLocation[0];aValue[1] = 0;aValue[2] = airplaneData.zLocation[0];aValue[3] = airplaneData.aAlpha[0];
    bValue[0] = airplaneData.xLocation[1];bValue[1] = 0;bValue[2] = airplaneData.zLocation[1];bValue[3] = airplaneData.aAlpha[1];
    cValue[0] = airplaneData.xLocation[2];cValue[1] = 0;cValue[2] = airplaneData.zLocation[2];cValue[3] = airplaneData.aAlpha[2];

    if(!isModify){

        airplaneData.name = nameEdit->text();
        airplaneVLM *VLMSolver = new airplaneVLM();
        VLMSolver->initialize();//初始化
        airplaneSolverArray.append(VLMSolver);
        airplaneStability *stabilitySolver = new airplaneStability();
        airplaneStabilityArray.append(stabilitySolver);
        for(int i = 0;i<5;i++){
            airplaneData.isChoice[i] = readyButton[i]->property("status").toInt();
        }


        for(int i = 0;i<5;i++){
            airplaneData.compoentIndex[i] = listIndexArray[i];
        }









        airplaneDataArray.append(airplaneData);

        addTreeNode(airplaneData.name);//添加节点



        airplaneChoiceIndex = airplaneIndex;








        VLMSolver->setOffsetValue(aValue,bValue,cValue);


        if(airplaneData.isChoice[0]){

            VLMSolver->importWingGeometry(wingDataArray[listIndexArray[0]]);

        }

        if(airplaneData.isChoice[1]){

            VLMSolver->importHTailGeometry(tailDataArray[listIndexArray[1]]);

        }

        if(airplaneData.isChoice[2]){

            VLMSolver->importVTailGeometry(tailDataArray[listIndexArray[2]]);

        }

        connect(VLMSolver, &airplaneVLM::progressUpdated, this, &airplaneDisplay::updateProgress);
        connect(VLMSolver, &airplaneVLM::workFinished, this, &airplaneDisplay::onCalculationFinished);
        connect(VLMSolver, &airplaneVLM::emitProgressValue,this,&airplaneDisplay::changeProgressUpdate);




        int r =rand()%255;
        int b =rand()%255;
        int g =rand()%255;

        QColor color(r,b,g);

        colorArray.append(color);
        resultPenArray.append(0);

        QVector<QVector<double>>tmp;
        resultArray.append(tmp);

        QLineSeries *seriesA = new QLineSeries;
        QLineSeries *seriesB = new QLineSeries;
        QLineSeries *seriesC = new QLineSeries;
        QLineSeries *seriesD = new QLineSeries;


        chartWA->addSeries(seriesA);
        chartWB->addSeries(seriesB);
        chartWC->addSeries(seriesC);
        chartWD->addSeries(seriesD);


        seriesA->attachAxis(axisXWA);
        seriesA->attachAxis(axisYWA);
        seriesB->attachAxis(axisXWB);
        seriesB->attachAxis(axisYWB);
        seriesC->attachAxis(axisXWC);
        seriesC->attachAxis(axisYWC);
        seriesD->attachAxis(axisXWD);
        seriesD->attachAxis(axisYWD);


        airplaneResultSeriesA.append(seriesA);
        airplaneResultSeriesB.append(seriesB);
        airplaneResultSeriesC.append(seriesC);
        airplaneResultSeriesD.append(seriesD);


        seriesA->setName(airplaneData.name);
        seriesB->setName(airplaneData.name);
        seriesC->setName(airplaneData.name);
        seriesD->setName(airplaneData.name);
        spanSeriesA->setName(airplaneData.name);


        VLMSetting setting(0,10,1,20,0,0);
        airplaneSettingArray.append(setting);


        airplaneIndex++;


    }else{

        airplaneTreeItemArray[airplaneChoiceIndex]->setText(0,nameEdit->text());
        for(int i = 0;i<5;i++){

            airplaneDataArray[airplaneChoiceIndex].isChoice[i] = readyButton[i]->property("status").toInt();

            airplaneDataArray[airplaneChoiceIndex].compoentIndex[i] = listIndexArray[i];
        }

        for(int i = 0;i<3;i++){
            double x = xLocationEdit[i]->text().toDouble();
            double a = aAlphaEdit[i]->text().toDouble();
            double z = zLocationEdit[i]->text().toDouble();
            airplaneDataArray[airplaneChoiceIndex].xLocation[i] = x;
            airplaneDataArray[airplaneChoiceIndex].aAlpha[i] = a;
            airplaneDataArray[airplaneChoiceIndex].zLocation[i] = z;

        }


        airplaneResultSeriesA[airplaneChoiceIndex]->setName(nameEdit->text());
        airplaneResultSeriesB[airplaneChoiceIndex]->setName(nameEdit->text());
        airplaneResultSeriesC[airplaneChoiceIndex]->setName(nameEdit->text());
        airplaneResultSeriesD[airplaneChoiceIndex]->setName(nameEdit->text());
        spanSeriesA->setName(nameEdit->text());





        //memcpy(airplaneDataArray[airplaneChoiceIndex].compoentIndex,listIndexArray,8);

    }

    updateAirplaneDefineWidget();
}
void airplaneDisplay::updateAirplaneDialog(){

    if(isModify){
        for(int i = 0;i<5;i++){
            bool ok = airplaneDataArray[airplaneChoiceIndex].isChoice[i];
            if(ok){
                readyButton[i]->setIcon(QIcon(greenIcon));
                int index = airplaneDataArray[airplaneChoiceIndex].compoentIndex[i];
                choiceButton[i]->setText(nameListArray[i].at(index));
                listView->setModelColumn(index);
                readyButton[i]->setProperty("status",1);


            }else{
                readyButton[i]->setIcon(QIcon(redIcon));
                readyButton[i]->setProperty("status",0);
                choiceButton[i]->setText("选择");
            }
        }
    }else{
        for(int i = 0;i<5;i++){
            readyButton[i]->setIcon(QIcon(redIcon));
            choiceButton[i]->setText("选择");
            readyButton[i]->setProperty("status",0);
        }

    }

}
void airplaneDisplay::addAirplaneData(airplaneDefinition&airplaneData){
    airplaneVLM *VLMSolver = new airplaneVLM();
    airplaneStability *stabilitySolver = new airplaneStability();
    VLMSolver->initialize();//初始化
    airplaneSolverArray.append(VLMSolver);
    airplaneStabilityArray.append(stabilitySolver);
    airplaneDataArray.append(airplaneData);


    addTreeNode(airplaneData.name);//添加节点



    airplaneChoiceIndex = airplaneIndex;



    double aValue[4],bValue[4],cValue[4];
    aValue[0] = airplaneData.xLocation[0];aValue[1] = 0;aValue[2] = airplaneData.zLocation[0];aValue[3] = airplaneData.aAlpha[0];
    bValue[0] = airplaneData.xLocation[1];bValue[1] = 0;bValue[2] = airplaneData.zLocation[1];bValue[3] = airplaneData.aAlpha[1];
    cValue[0] = airplaneData.xLocation[2];cValue[1] = 0;cValue[2] = airplaneData.zLocation[2];cValue[3] = airplaneData.aAlpha[2];

    VLMSolver->setOffsetValue(aValue,bValue,cValue);


    if(airplaneData.isChoice[0]){

        VLMSolver->importWingGeometry(wingDataArray[airplaneData.compoentIndex[0]]);

    }

    if(airplaneData.isChoice[1]){

        VLMSolver->importHTailGeometry(tailDataArray[airplaneData.compoentIndex[1]]);

    }

    if(airplaneData.isChoice[2]){


        VLMSolver->importVTailGeometry(tailDataArray[airplaneData.compoentIndex[2]]);

    }

    connect(VLMSolver, &airplaneVLM::progressUpdated, this, &airplaneDisplay::updateProgress);
    connect(VLMSolver, &airplaneVLM::workFinished, this, &airplaneDisplay::onCalculationFinished);
    connect(VLMSolver, &airplaneVLM::emitProgressValue,this,&airplaneDisplay::changeProgressUpdate);

    int r =rand()%255;
    int b =rand()%255;
    int g =rand()%255;

    QColor color(r,b,g);

    colorArray.append(color);
    resultPenArray.append(0);

    QVector<QVector<double>>tmp;
    resultArray.append(tmp);

    QLineSeries *seriesA = new QLineSeries;
    QLineSeries *seriesB = new QLineSeries;
    QLineSeries *seriesC = new QLineSeries;
    QLineSeries *seriesD = new QLineSeries;


    chartWA->addSeries(seriesA);
    chartWB->addSeries(seriesB);
    chartWC->addSeries(seriesC);
    chartWD->addSeries(seriesD);


    seriesA->attachAxis(axisXWA);
    seriesA->attachAxis(axisYWA);
    seriesB->attachAxis(axisXWB);
    seriesB->attachAxis(axisYWB);
    seriesC->attachAxis(axisXWC);
    seriesC->attachAxis(axisYWC);
    seriesD->attachAxis(axisXWD);
    seriesD->attachAxis(axisYWD);


    airplaneResultSeriesA.append(seriesA);
    airplaneResultSeriesB.append(seriesB);
    airplaneResultSeriesC.append(seriesC);
    airplaneResultSeriesD.append(seriesD);


    seriesA->setName(airplaneData.name);
    seriesB->setName(airplaneData.name);
    seriesC->setName(airplaneData.name);
    seriesD->setName(airplaneData.name);
    spanSeriesA->setName(airplaneData.name);


    VLMSetting setting(0,10,1,20,0,0);
    airplaneSettingArray.append(setting);


    airplaneIndex++;

    updateAirplaneDefineWidget();


}
void airplaneDisplay::updateList(const QVector<wingDefinition>&a,const QVector<wingDefinition>&b){
    wingDataArray = a;
    tailDataArray = b;

    nameListArray[0].clear();
    nameListArray[1].clear();
    nameListArray[2].clear();
    for(int i = 0;i<a.length();i++){
        nameListArray[0].append(a[i].name);
    }
    for(int i = 0;i<b.length();i++){
        nameListArray[1].append(b[i].name);
    }
    for(int i = 0;i<b.length();i++){
        nameListArray[2].append(b[i].name);
    }

}
void airplaneDisplay::updateAirplaneList(QVector<airplaneDefinition>&airplaneData){
    airplaneIndex = 0;
    if(!airplaneDataArray.isEmpty()){
        for(int i = 0;i<airplaneResultSeriesA.length();i++){
            chartWA->removeSeries(airplaneResultSeriesA[i]);
            chartWB->removeSeries(airplaneResultSeriesB[i]);
            chartWC->removeSeries(airplaneResultSeriesC[i]);
            chartWD->removeSeries(airplaneResultSeriesD[i]);
        }
        for(QTreeWidgetItem* item : airplaneTreeItemArray){
            delete item;
        }
        for(airplaneVLM* solver : airplaneSolverArray){
            delete solver;
        }
        for(int i = 0;i<airplaneResultSeriesA.length();i++){
            delete airplaneResultSeriesA[i];
            delete airplaneResultSeriesB[i];
            delete airplaneResultSeriesC[i];
            delete airplaneResultSeriesD[i];
            //delete airplaneResultSeriesE[i];
        }
        airplaneTreeItemArray.clear();
        airplaneResultSeriesA.clear();
        airplaneResultSeriesB.clear();
        airplaneResultSeriesC.clear();
        airplaneResultSeriesD.clear();
        resultArray.clear();
        airplaneSettingArray.clear();
        airplaneSolverArray.clear();
        airplaneDataArray.clear();
        colorArray.clear();







    }

    for(int i = 0;i<airplaneData.length();i++){

        addAirplaneData(airplaneData[i]);
    }


}
void airplaneDisplay::updateListViewIndex(const QItemSelection &selected,const QItemSelection &deselected){
    QModelIndexList indexes = selected.indexes();
    int index = indexes.at(0).row();
    listIndexArray[modelIndex] = index;

    readyButton[modelIndex]->setIcon(QIcon(greenIcon));
    readyButton[modelIndex]->setProperty("status",1);
    choiceButton[modelIndex]->setText(nameListArray[modelIndex].at(index));

    //airplaneDataArray[airplaneChoiceIndex].isChoice[modelIndex] = true;
    updateAirplaneMessage();



}

void airplaneDisplay::showAirplaneDefineDialog(){
    isModify = false;

    updateAirplaneDialog();
    airplaneDefineDialog->show();
}
void airplaneDisplay::showListDialog(){
    QPushButton *button = qobject_cast<QPushButton*>(sender());
    modelIndex = button->property("index").toInt();
    listModel->setStringList(nameListArray[modelIndex]);
    listDialog->show();
}

void airplaneDisplay::setButtonStatus(){
    QPushButton *button = qobject_cast<QPushButton*>(sender());

    int status = button->property("status").toInt();
    int index = getReadyButtonIndex(button);
    modelIndex = index;

    if(!status){
        button->setIcon(QIcon(greenIcon));
        button->setProperty("status",1);
    }
    else{
        button->setIcon(QIcon(redIcon));
        button->setProperty("status",0);
        choiceButton[index]->setText("选择");
    }
    //airplaneDataArray[airplaneChoiceIndex].isChoice[modelIndex] = status;
    updateAirplaneMessage();
    button = nullptr;
}
void airplaneDisplay::createTranslatedArray(vtkSmartPointer<vtkRenderer> ren, vtkSmartPointer<vtkPolyData> inputPolyData) {
    // 检查输入数据是否有效
    if (!inputPolyData || inputPolyData->GetNumberOfPoints() == 0) {
        std::cerr << "Error: inputPolyData is empty or invalid!" << std::endl;
        return;
    }

    // 创建变换对象并进行平移
    vtkSmartPointer<vtkTransform> transform = vtkSmartPointer<vtkTransform>::New();
    transform->Translate(1.5, 0.0, 0.0);  // 沿x轴平移1.5米

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
    property->SetObjectName("translate");
    actor->SetProperty(property);

    // 将演员添加到渲染器
    ren->AddActor(actor);

    // 如果需要，可以在这里清除旧的演员或进行其他处理
}
void airplaneDisplay::addWingToVTK(){
    actorContourWing->VisibilityOff();
    actorWing->VisibilityOn();

    if(airplaneDataArray[airplaneChoiceIndex].isChoice[0]){
        wingVLM ned;
        int index = airplaneDataArray[airplaneChoiceIndex].compoentIndex[0];

        //transformWing->Identity();
        double x = airplaneDataArray[airplaneChoiceIndex].xLocation[0];
        double z = airplaneDataArray[airplaneChoiceIndex].zLocation[0];
        double angle = airplaneDataArray[airplaneChoiceIndex].aAlpha[0];

        ned.setOffsetValue(x,0,z,angle);
        ned.initialGeometry(wingDataArray[index]);

        if(ned.checkGeometry()){
            pointsWing->Reset();
            linesWing->Reset();
            colorWing->Reset();



            // 插入网格点
            for (int i = 0; i < ned.getMeshNum() * 2; i++) {
                pointsWing->InsertNextPoint(ned.meshA[i].x, ned.meshA[i].y, ned.meshA[i].z);
                pointsWing->InsertNextPoint(ned.meshB[i].x, ned.meshB[i].y, ned.meshB[i].z);
                pointsWing->InsertNextPoint(ned.meshC[i].x, ned.meshC[i].y, ned.meshC[i].z);
                pointsWing->InsertNextPoint(ned.meshD[i].x, ned.meshD[i].y, ned.meshD[i].z);
            }


            unsigned char white[3] = {200, 200, 200};
            for (vtkIdType i = 0; i < pointsWing->GetNumberOfPoints() - 3; i += 4) {

                vtkSmartPointer<vtkQuad> quad = vtkSmartPointer<vtkQuad>::New();
                quad->GetPointIds()->SetId(0, i);
                quad->GetPointIds()->SetId(1, i + 1);
                quad->GetPointIds()->SetId(2, i + 2);
                quad->GetPointIds()->SetId(3, i + 3);
                linesWing->InsertNextCell(quad);                // 添加颜色

                colorWing->InsertNextTypedTuple(white);

            }
            profilePolyDataWing->SetPoints(pointsWing);
            profilePolyDataWing->SetPolys(linesWing);
            profilePolyDataWing->GetCellData()->SetScalars(colorWing);





            renwin->Render();
        }
    }else{
        pointsWing->Reset();
        linesWing->Reset();
        colorWing->Reset();
    }

}
void airplaneDisplay::addVTailToVTK(){
    actorContourVTail->VisibilityOff();
    actorVTail->VisibilityOn();
    if(airplaneDataArray[airplaneChoiceIndex].isChoice[2]){

        wingVLM ned;
        int index = airplaneDataArray[airplaneChoiceIndex].compoentIndex[2];

        //transformWing->Identity();
        double x = airplaneDataArray[airplaneChoiceIndex].xLocation[2];
        double z = airplaneDataArray[airplaneChoiceIndex].zLocation[2];
        double angle = airplaneDataArray[airplaneChoiceIndex].aAlpha[2];


        ned.setOffsetValue(x,0,z,angle);

        ned.initialGeometry(tailDataArray[index]);
        if(ned.checkGeometry()){
            pointsVTail->Reset();
            linesVTail->Reset();
            colorVTail->Reset();

            // 插入网格点
            for (int i = 0; i < ned.getMeshNum() * 2; i++) {
                pointsVTail->InsertNextPoint(ned.meshA[i].x, ned.meshA[i].y, ned.meshA[i].z);
                pointsVTail->InsertNextPoint(ned.meshB[i].x, ned.meshB[i].y, ned.meshB[i].z);
                pointsVTail->InsertNextPoint(ned.meshC[i].x, ned.meshC[i].y, ned.meshC[i].z);
                pointsVTail->InsertNextPoint(ned.meshD[i].x, ned.meshD[i].y, ned.meshD[i].z);
            }


            unsigned char white[3] = {200, 200, 200};
            for (vtkIdType i = 0; i < pointsVTail->GetNumberOfPoints() - 3; i += 4) {

                vtkSmartPointer<vtkQuad> quad = vtkSmartPointer<vtkQuad>::New();
                quad->GetPointIds()->SetId(0, i);
                quad->GetPointIds()->SetId(1, i + 1);
                quad->GetPointIds()->SetId(2, i + 2);
                quad->GetPointIds()->SetId(3, i + 3);
                linesVTail->InsertNextCell(quad);                // 添加颜色

                colorVTail->InsertNextTypedTuple(white);

            }
            profilePolyDataVTail->SetPoints(pointsVTail);
            profilePolyDataVTail->SetPolys(linesVTail);
            profilePolyDataVTail->GetCellData()->SetScalars(colorVTail);

            renwin->Render();
        }
    }else{
        pointsVTail->Reset();
        linesVTail->Reset();
        colorVTail->Reset();
    }

}
void airplaneDisplay::addHTailToVTK(){

    actorContourHTail->VisibilityOff();
    actorHTail->VisibilityOn();

    if(airplaneDataArray[airplaneChoiceIndex].isChoice[1]){
        wingVLM ned;
        int index = airplaneDataArray[airplaneChoiceIndex].compoentIndex[1];

        //transformWing->Identity();
        double x = airplaneDataArray[airplaneChoiceIndex].xLocation[1];
        double z = airplaneDataArray[airplaneChoiceIndex].zLocation[1];
        double angle = airplaneDataArray[airplaneChoiceIndex].aAlpha[1];
        ned.setOffsetValue(x,0,z,angle);


        ned.initialGeometry(tailDataArray[index]);
        if(ned.checkGeometry()){
            pointsHTail->Reset();
            linesHTail->Reset();
            colorHTail->Reset();

            // 插入网格点
            for (int i = 0; i < ned.getMeshNum() * 2; i++) {
                pointsHTail->InsertNextPoint(ned.meshA[i].x, ned.meshA[i].y, ned.meshA[i].z);
                pointsHTail->InsertNextPoint(ned.meshB[i].x, ned.meshB[i].y, ned.meshB[i].z);
                pointsHTail->InsertNextPoint(ned.meshC[i].x, ned.meshC[i].y, ned.meshC[i].z);
                pointsHTail->InsertNextPoint(ned.meshD[i].x, ned.meshD[i].y, ned.meshD[i].z);
            }


            unsigned char white[3] = {200, 200, 200};
            for (vtkIdType i = 0; i < pointsHTail->GetNumberOfPoints() - 3; i += 4) {

                vtkSmartPointer<vtkQuad> quad = vtkSmartPointer<vtkQuad>::New();
                quad->GetPointIds()->SetId(0, i);
                quad->GetPointIds()->SetId(1, i + 1);
                quad->GetPointIds()->SetId(2, i + 2);
                quad->GetPointIds()->SetId(3, i + 3);
                linesHTail->InsertNextCell(quad);                // 添加颜色

                colorHTail->InsertNextTypedTuple(white);

            }
            profilePolyDataHTail->SetPoints(pointsHTail);
            profilePolyDataHTail->SetPolys(linesHTail);
            profilePolyDataHTail->GetCellData()->SetScalars(colorHTail);

            //transformHTail->Identity();
            //double x = airplaneDataArray[airplaneChoiceIndex].xLocation[1];
            //double z = airplaneDataArray[airplaneChoiceIndex].zLocation[1];

            //transformHTail->Translate(x,0,z);
            renwin->Render();
        }
    }else{
        pointsHTail->Reset();
        linesHTail->Reset();
        colorHTail->Reset();
    }

}
void airplaneDisplay::addFuselageToVTK(){
    wingVLM ned;
    int index = 0;
    ned.initialGeometry(fuselageDataArray[index]);
    if(ned.checkGeometry()){
        pointsFuselage->Reset();
        linesFuselage->Reset();
        colorFuselage->Reset();

        // 插入网格点
        for (int i = 0; i < ned.getMeshNum() * 2; i++) {
            pointsFuselage->InsertNextPoint(ned.meshA[i].x, ned.meshA[i].y, ned.meshA[i].z);
            pointsFuselage->InsertNextPoint(ned.meshB[i].x, ned.meshB[i].y, ned.meshB[i].z);
            pointsFuselage->InsertNextPoint(ned.meshC[i].x, ned.meshC[i].y, ned.meshC[i].z);
            pointsFuselage->InsertNextPoint(ned.meshD[i].x, ned.meshD[i].y, ned.meshD[i].z);
        }


        unsigned char white[3] = {200, 200, 200};
        for (vtkIdType i = 0; i < pointsFuselage->GetNumberOfPoints() - 3; i += 4) {

            vtkSmartPointer<vtkQuad> quad = vtkSmartPointer<vtkQuad>::New();
            quad->GetPointIds()->SetId(0, i);
            quad->GetPointIds()->SetId(1, i + 1);
            quad->GetPointIds()->SetId(2, i + 2);
            quad->GetPointIds()->SetId(3, i + 3);
            linesFuselage->InsertNextCell(quad);                // 添加颜色

            colorFuselage->InsertNextTypedTuple(white);

        }
        profilePolyDataFuselage->SetPoints(pointsFuselage);
        profilePolyDataFuselage->SetPolys(linesFuselage);
        profilePolyDataFuselage->GetCellData()->SetScalars(colorFuselage);

        renwin->Render();
    }
}
void airplaneDisplay::addTTailToVTK(){
    wingVLM ned;
    int index = 0;
    ned.initialGeometry(tTailDataArray[index]);
    if(ned.checkGeometry()){
        pointsTTail->Reset();
        linesTTail->Reset();
        colorTTail->Reset();

        // 插入网格点
        for (int i = 0; i < ned.getMeshNum() * 2; i++) {
            pointsTTail->InsertNextPoint(ned.meshA[i].x, ned.meshA[i].y, ned.meshA[i].z);
            pointsTTail->InsertNextPoint(ned.meshB[i].x, ned.meshB[i].y, ned.meshB[i].z);
            pointsTTail->InsertNextPoint(ned.meshC[i].x, ned.meshC[i].y, ned.meshC[i].z);
            pointsTTail->InsertNextPoint(ned.meshD[i].x, ned.meshD[i].y, ned.meshD[i].z);
        }


        unsigned char white[3] = {200, 200, 200};
        for (vtkIdType i = 0; i < pointsTTail->GetNumberOfPoints() - 3; i += 4) {

            vtkSmartPointer<vtkQuad> quad = vtkSmartPointer<vtkQuad>::New();
            quad->GetPointIds()->SetId(0, i);
            quad->GetPointIds()->SetId(1, i + 1);
            quad->GetPointIds()->SetId(2, i + 2);
            quad->GetPointIds()->SetId(3, i + 3);
            linesTTail->InsertNextCell(quad);                // 添加颜色

            colorTTail->InsertNextTypedTuple(white);

        }
        profilePolyDataTTail->SetPoints(pointsTTail);
        profilePolyDataTTail->SetPolys(linesTTail);
        profilePolyDataTTail->GetCellData()->SetScalars(colorTTail);

        renwin->Render();
    }
}

void airplaneDisplay::onCheckboxStateChanged(int state){
    if (state == Qt::Checked) {
        actorCg->VisibilityOn();
    } else {
        actorCg->VisibilityOff();
    }
    airplaneDisplayWidgetA->renderWindow()->Render();

}
void airplaneDisplay::changeCgLocation(){

}
void airplaneDisplay::showAirplaneSettingVinfDialog(QTreeWidgetItem* item, int column){

    if (item->parent()) { // 判断是否是子节点

        QString txt = item->data(0,Qt::UserRole).toString();
        QString txtTmp = txt.mid(0,9);
        QString tmp1 = txt.mid(9);
        QString tmp = "childVinf";
        if(tmp == txtTmp &&  tmp1.toInt() == airplaneChoiceIndex){
            airplaneMinAlphaEdit->setText(QString::number(airplaneSettingArray[airplaneChoiceIndex].minAlpha));
            airplaneMaxAlphaEdit->setText(QString::number(airplaneSettingArray[airplaneChoiceIndex].maxAlpha));
            airplaneStepAlphaEdit->setText(QString::number(airplaneSettingArray[airplaneChoiceIndex].stepAlpha));
            airplaneVelocityEdit->setText(QString::number(airplaneSettingArray[airplaneChoiceIndex].vinf));
            airplaneCgLocationEdit->setText(QString::number(airplaneSettingArray[airplaneChoiceIndex].referencePointX));


            airplaneSettingDialog->show();
            // defineAirfoil->setArifoil(wingDataArray[wingChoiceIndex].airfoilArray[0]);
            // defineAirfoil->modifyAirfoilDialog->show();
        }
    }
}


void airplaneDisplay::initialChartWA(){
    chartWA = new QChart();
    chartViewWA = new MyChartView(chartWA,rChartMenu);
    chartViewWA->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    toolTipA = new Callout(chartWA);

    chartWA->setBackgroundBrush(Qt::NoBrush);


    axisXWA = new QValueAxis;
    axisYWA = new QValueAxis;

    chartViewWA->setRenderHint(QPainter::Antialiasing);


    axisXWA->setLinePenColor(Qt::black);
    axisYWA->setLinePenColor(Qt::black);

    axisXWA->setRange(0,10);
    axisYWA->setRange(0,1);
    chartWA->addAxis(axisXWA,Qt::AlignBottom);
    chartWA->addAxis(axisYWA,Qt::AlignLeft);
    axisXWA->setTitleText(axisXName[0]);
    axisYWA->setTitleText(axisYName[0]);
    chartViewWA->hide();
    chartWA->setTitle(titleName[0]);
    autoSeriesA = new QLineSeries;
    chartWA->addSeries(autoSeriesA);
    autoSeriesA->attachAxis(axisXWA);
    autoSeriesA->attachAxis(axisYWA);
    removeSeriesLegendItem(chartWA,autoSeriesA);
    QPen pen;
    pen.setStyle(Qt::DashLine);
    pen.setColor(Qt::red);
    pen.setWidth(2);
    autoSeriesA->setPen(pen);
    //chartWA->legend()->setVisible(false);
}
void airplaneDisplay::initialChartWB(){
    chartWB = new QChart();
    chartViewWB = new MyChartView(chartWB,rChartMenu);
    chartViewWB->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    chartWB->setBackgroundBrush(Qt::NoBrush);

    toolTipB = new Callout(chartWB);


    axisXWB = new QValueAxis;
    axisYWB = new QValueAxis;
    chartViewWB->setRenderHint(QPainter::Antialiasing);

    axisXWB->setLinePenColor(Qt::black);
    axisYWB->setLinePenColor(Qt::black);

    axisXWB->setRange(0,10);
    axisYWB->setRange(-0.5,0.1);
    chartWB->addAxis(axisXWB,Qt::AlignBottom);
    chartWB->addAxis(axisYWB,Qt::AlignLeft);
    axisXWB->setTitleText(axisXName[0]);
    axisYWB->setTitleText(axisYName[1]);
    chartViewWB->hide();
    chartWB->setTitle(titleName[1]);

    autoSeriesB = new QLineSeries;
    chartWB->addSeries(autoSeriesB);
    autoSeriesB->attachAxis(axisXWB);
    autoSeriesB->attachAxis(axisYWB);
    removeSeriesLegendItem(chartWB,autoSeriesB);
    QPen pen;
    pen.setStyle(Qt::DashLine);
    pen.setColor(Qt::red);
    pen.setWidth(2);
    autoSeriesB->setPen(pen);
    //chartWB->legend()->setVisible(false);

}
void airplaneDisplay::initialChartWC(){
    chartWC = new QChart();
    chartViewWC = new MyChartView(chartWC,rChartMenu);
    chartViewWC->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    chartWC->setBackgroundBrush(Qt::NoBrush);

    toolTipC = new Callout(chartWC);


    axisXWC = new QValueAxis;
    axisYWC = new QValueAxis;
    chartViewWC->setRenderHint(QPainter::Antialiasing);
    //chartViewB->setBackgroundBrush(QBrush(colors[5]));
    //chartViewWC->setStyleSheet(chartViewBackGroundStyle);
    axisXWC->setLinePenColor(Qt::black);
    axisYWC->setLinePenColor(Qt::black);
    //axisXWC->setTickCount(int(MINAXIS[7]));
    //axisYWC->setTickCount(int(MAXAXIS[7]));
    axisXWC->setRange(0,10);
    axisYWC->setRange(0,30);
    chartWC->addAxis(axisXWC,Qt::AlignBottom);
    chartWC->addAxis(axisYWC,Qt::AlignLeft);
    axisXWC->setTitleText(axisXName[0]);
    axisYWC->setTitleText(axisYName[2]);
    chartViewWC->hide();
    chartWC->setTitle(titleName[2]);

    autoSeriesC = new QLineSeries;
    chartWC->addSeries(autoSeriesC);
    autoSeriesC->attachAxis(axisXWC);
    autoSeriesC->attachAxis(axisYWC);
    removeSeriesLegendItem(chartWC,autoSeriesC);
    //chartWC->legend()->setVisible(false);
    QPen pen;
    pen.setStyle(Qt::DashLine);
    pen.setColor(Qt::red);
    pen.setWidth(2);
    autoSeriesC->setPen(pen);

}
void airplaneDisplay::initialChartWD(){
    chartWD = new QChart();
    chartViewWD = new MyChartView(chartWD,rChartMenu);
    chartViewWD->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    chartWD->setBackgroundBrush(Qt::NoBrush);

    toolTipD = new Callout(chartWD);


    axisXWD = new QValueAxis;
    axisYWD = new QValueAxis;
    chartViewWD->setRenderHint(QPainter::Antialiasing);
    //chartViewB->setBackgroundBrush(QBrush(colors[5]));
    //chartViewWD->setStyleSheet(chartViewBackGroundStyle);
    axisXWD->setLinePenColor(Qt::black);
    axisYWD->setLinePenColor(Qt::black);
    //axisXWD->setTickCount(int(MINAXIS[7]));
    //axisYWD->setTickCount(int(MAXAXIS[7]));
    axisXWD->setRange(0,10);
    axisYWD->setRange(0,30);
    chartWD->addAxis(axisXWD,Qt::AlignBottom);
    chartWD->addAxis(axisYWD,Qt::AlignLeft);
    axisXWD->setTitleText(axisXName[0]);
    axisYWD->setTitleText(axisYName[3]);
    chartViewWD->hide();
    chartWD->setTitle(titleName[3]);

    autoSeriesD = new QLineSeries;
    chartWD->addSeries(autoSeriesD);
    autoSeriesD->attachAxis(axisXWD);
    autoSeriesD->attachAxis(axisYWD);
    removeSeriesLegendItem(chartWD,autoSeriesD);
    //chartWD->legend()->setVisible(false);
    QPen pen;
    pen.setStyle(Qt::DashLine);
    pen.setColor(Qt::red);
    pen.setWidth(2);
    autoSeriesD->setPen(pen);

}
void airplaneDisplay::initialSpanChart(){
    spanChart = new QChart();
    spanChartView = new MyChartView(spanChart,spanRMenu);
    spanSeriesA = new QLineSeries;
    spanSeriesB = new QLineSeries;
    spanXAxis = new QValueAxis;
    spanYAxis = new QValueAxis;


    spanChart->addSeries(spanSeriesA);
    spanChart->addSeries(spanSeriesB);
    spanChartView->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    spanChart->setBackgroundBrush(Qt::NoBrush);


    spanXAxis->setLinePenColor(Qt::black);
    spanYAxis->setLinePenColor(Qt::black);
    spanXAxis->setRange(-1,1);
    spanYAxis->setRange(-1,1);

    spanChartView->setRenderHint(QPainter::Antialiasing);
    spanChart->addAxis(spanXAxis,Qt::AlignBottom);
    spanChart->addAxis(spanYAxis,Qt::AlignLeft);
    spanSeriesA->attachAxis(spanXAxis);
    spanSeriesA->attachAxis(spanYAxis);
    spanSeriesB->attachAxis(spanXAxis);
    spanSeriesB->attachAxis(spanYAxis);
    spanChart->legend()->hide();

    spanChart->setTitle(titleName[9]);

    QPen pen2;
    pen2.setWidth(2);
    pen2.setColor(Qt::red);
    pen2.setStyle(Qt::DashLine);
    spanSeriesB->setPen(pen2);


    spanChartView->hide();

}
void airplaneDisplay::initialSpanRChartMenu(){
    spanRMenu = new QMenu();
    QAction *rAction1 = new QAction(titleName[9],spanRMenu);
    QAction *rAction2 = new QAction(titleName[10],spanRMenu);
    QAction *rAction3 = new QAction(titleName[11],spanRMenu);
    QAction *rAction4 = new QAction(titleName[12],spanRMenu);
    QAction *rAction5 = new QAction("显示椭圆曲线",spanRMenu);
    QAction *rAction6 = new QAction("数据导出",spanRMenu);



    rAction1->setProperty("actions",0);
    rAction2->setProperty("actions",1);
    rAction3->setProperty("actions",2);
    rAction4->setProperty("actions",3);
    rAction5->setProperty("actions",4);
    rAction6->setProperty("actions",5);



    connect(rAction1,&QAction::triggered,this,&airplaneDisplay::changeSpanResultType);
    connect(rAction2,&QAction::triggered,this,&airplaneDisplay::changeSpanResultType);
    connect(rAction3,&QAction::triggered,this,&airplaneDisplay::changeSpanResultType);
    connect(rAction4,&QAction::triggered,this,&airplaneDisplay::changeSpanResultType);
    connect(rAction5,&QAction::triggered,this,&airplaneDisplay::changeSpanResultType);
    connect(rAction6,&QAction::triggered,this,&airplaneDisplay::changeSpanResultType);

    rAction1->setCheckable(true);
    rAction2->setCheckable(true);
    rAction3->setCheckable(true);
    rAction4->setCheckable(true);
    rAction5->setCheckable(true);
    rAction6->setCheckable(true);



    spanRMenu->addAction(rAction1);
    spanRMenu->addAction(rAction2);
    spanRMenu->addAction(rAction3);
    spanRMenu->addAction(rAction4);
    spanRMenu->addAction(rAction5);
    spanRMenu->addAction(rAction6);


    spanActionArray.append(rAction1);
    spanActionArray.append(rAction2);
    spanActionArray.append(rAction3);
    spanActionArray.append(rAction4);
    spanActionArray.append(rAction5);
    spanActionArray.append(rAction6);

}
void airplaneDisplay::changeSpanResultType(){
    QAction *action = qobject_cast<QAction*>(sender());
    int index = action->property("actions").toInt();
    if (action) {
        // 取消所有其他动作的选中状态
        for (QAction *act : spanRMenu->actions()) {
            act->setChecked(false);
        }
    }
    if(index >= 5){
        exportChartData(spanChart);
        QMessageBox::information(this,"信息提示","保存成功");
    }else if(index == 4){



    }else{
        // 设置当前动作为选中状态

        action->setChecked(true);
        spanResultTypeIndex = index;
        spanXAxis->setTitleText(axisXName[index + 9]);
        spanYAxis->setTitleText(axisYName[index + 9]);
        spanChart->setTitle(titleName[index + 9]);
        drawSpanResult(airplaneChoiceIndex);


    }




}
void airplaneDisplay::exportChartData(QChart *chart) {
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
}
void airplaneDisplay::initialRChartMenu(){
    rChartMenu = new QMenu();
    QAction *rAction1 = new QAction("升力系数曲线",rChartMenu);
    QAction *rAction2 = new QAction("阻力系数曲线",rChartMenu);
    QAction *rAction3 = new QAction("力矩曲线",rChartMenu);
    QAction *rAction4 = new QAction("升阻比曲线",rChartMenu);
    QAction *rAction5 = new QAction("功率因子曲线",rChartMenu);
    QAction *rAction6 = new QAction("极曲线",rChartMenu);
    QAction *rAction7 = new QAction("升力系数-升阻比曲线",rChartMenu);
    QAction *rAction8 = new QAction("升力系数-功率因子曲线",rChartMenu);
    QAction *rAction9 = new QAction("升力系数-力矩曲线",rChartMenu);
    QAction *rAction10 = new QAction("数据导出",rChartMenu);
    QAction *rAction11 = new QAction("添加点",rChartMenu);
    QAction *rAction12 = new QAction("删除点",rChartMenu);


    rAction1->setProperty("actions",0);
    rAction2->setProperty("actions",1);
    rAction3->setProperty("actions",2);
    rAction4->setProperty("actions",3);
    rAction5->setProperty("actions",4);
    rAction6->setProperty("actions",5);
    rAction7->setProperty("actions",6);
    rAction8->setProperty("actions",7);
    rAction9->setProperty("actions",8);
    rAction10->setProperty("actions",9);



    connect(rAction1,&QAction::triggered,this,&airplaneDisplay::changeResultType);
    connect(rAction2,&QAction::triggered,this,&airplaneDisplay::changeResultType);
    connect(rAction3,&QAction::triggered,this,&airplaneDisplay::changeResultType);
    connect(rAction4,&QAction::triggered,this,&airplaneDisplay::changeResultType);
    connect(rAction5,&QAction::triggered,this,&airplaneDisplay::changeResultType);
    connect(rAction6,&QAction::triggered,this,&airplaneDisplay::changeResultType);
    connect(rAction7,&QAction::triggered,this,&airplaneDisplay::changeResultType);
    connect(rAction8,&QAction::triggered,this,&airplaneDisplay::changeResultType);
    connect(rAction9,&QAction::triggered,this,&airplaneDisplay::changeResultType);
    connect(rAction10,&QAction::triggered,this,&airplaneDisplay::changeResultType);
    connect(rAction11,&QAction::triggered,this,&airplaneDisplay::addDataPoint);
    connect(rAction12,&QAction::triggered,this,&airplaneDisplay::clearDataPoint);


    rAction1->setCheckable(true);
    rAction2->setCheckable(true);
    rAction3->setCheckable(true);
    rAction4->setCheckable(true);
    rAction5->setCheckable(true);
    rAction6->setCheckable(true);
    rAction7->setCheckable(true);
    rAction8->setCheckable(true);
    rAction9->setCheckable(true);
    rAction10->setCheckable(true);



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
    rChartMenu->addAction(rAction12);


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
    actionArray.append(rAction12);


}
void airplaneDisplay::removeSeriesLegendItem(QChart *chart, QLineSeries *series) {
    // Get all legend markers for the given series
    QList<QLegendMarker *> markers = chart->legend()->markers(series);

    // Iterate through all legend markers
    for (QLegendMarker *marker : markers) {
        // Hide the legend marker
        marker->setVisible(false);
    }
}
void airplaneDisplay::changeWindowForAirplaneDesign(){
    airplaneTreeWidget->show();
    chartWWidget->hide();
    changeModelViewButton->setEnabled(false);
    changeResultViewButton->setEnabled(true);
    changeSpanResultViewButton->setEnabled(true);
    airplaneAlphaCombox->setEnabled(true);

}
void airplaneDisplay::getAirplaneSetting(){
    airplaneSettingDialog->hide();
    double mina = airplaneMinAlphaEdit->text().toDouble();
    double maxa = airplaneMaxAlphaEdit->text().toDouble();
    double stepa = airplaneStepAlphaEdit->text().toDouble();
    double v = airplaneVelocityEdit->text().toDouble();
    double hei = airplaneHeightEdit->text().toDouble();
    double referenceX = airplaneCgLocationEdit->text().toDouble();
    VLMSetting setting(mina,maxa,stepa,v,hei,referenceX);
    airplaneSettingArray[airplaneChoiceIndex] = setting;

    //VLMSolverArray[wingChoiceIndex]->height = hei;
}
void airplaneDisplay::drawResultChartA(const int index){


    if(!resultArray[index].isEmpty()){
        if(airplaneResultSeriesA[index]->count() > 0)
            airplaneResultSeriesA[index]->clear();
        for(int i = 0;i<resultArray[index][0].length();i++){
            airplaneResultSeriesA[index]->append(resultArray[index][xAxisIndexA][i],resultArray[index][yAxisIndexA][i]);
        }
        airplaneResultSeriesA[index]->setPen(getPen(index));
        //updateAxis(axisXWA,axisYWA,xAxisIndexA,yAxisIndexA);

    }
}
void airplaneDisplay::drawResultChartB(const int index){


    if(!resultArray[index].isEmpty()){
        if(airplaneResultSeriesB[index]->count() > 0)
            airplaneResultSeriesB[index]->clear();
        for(int i = 0;i<resultArray[index][0].length();i++){
            airplaneResultSeriesB[index]->append(resultArray[index][xAxisIndexB][i],resultArray[index][yAxisIndexB][i]);
        }
        airplaneResultSeriesB[index]->setPen(getPen(index));
        //updateAxis(axisXWA,axisYWA,xAxisIndexA,yAxisIndexA);

    }
}
void airplaneDisplay::drawResultChartC(const int index){


    if(!resultArray[index].isEmpty()){
        if(airplaneResultSeriesC[index]->count() > 0)
            airplaneResultSeriesC[index]->clear();
        for(int i = 0;i<resultArray[index][0].length();i++){
            airplaneResultSeriesC[index]->append(resultArray[index][xAxisIndexC][i],resultArray[index][yAxisIndexC][i]);
        }
        airplaneResultSeriesC[index]->setPen(getPen(index));
        //updateAxis(axisXWA,axisYWA,xAxisIndexA,yAxisIndexA);

    }
}
void airplaneDisplay::drawResultChartD(const int index){


    if(!resultArray[index].isEmpty()){
        if(airplaneResultSeriesD[index]->count() > 0)
            airplaneResultSeriesD[index]->clear();
        for(int i = 0;i<resultArray[index][0].length();i++){
            airplaneResultSeriesD[index]->append(resultArray[index][xAxisIndexD][i],resultArray[index][yAxisIndexD][i]);
        }
        airplaneResultSeriesD[index]->setPen(getPen(index));
        //updateAxis(axisXWA,axisYWA,xAxisIndexA,yAxisIndexA);

    }
}
void airplaneDisplay::drawSpanResult(const int index){

    if(!resultArray[index].isEmpty()){
        if(spanSeriesA->count()>0)
            spanSeriesA->clear();
        if(spanSeriesB->count()>0)
            spanSeriesB->clear();
        int ind = airplaneAlphaCombox->currentIndex();
        spanSeriesA->setPen(getPen(index));
        QVector<double>ytA;
        QVector<double>ytB;
        QVector<double>resultA;
        QVector<double>resultB;
        QVector<QLineSeries*>tmp;
        if(airplaneDataArray[airplaneChoiceIndex].isChoice[0]){
            wingVLM *wingSolver = airplaneSolverArray[airplaneChoiceIndex]->wingModel;
            switch(spanResultTypeIndex){
            case 0:
                ytA = wingSolver->spanForceYt;
                resultA = wingSolver->spanForce[ind];
                //resultA = wingSolver->spanLiftCoefficient[ind];

                break;
            case 1:
                ytA = wingSolver->spanForceYt;
                resultA = wingSolver->spanLiftCoefficient[ind];
                break;
            case 2:
                ytA = wingSolver->yt;
                resultA = wingSolver->spanMonmentArray[ind];
                break;
            case 3:
                ytA = wingSolver->spanForceYt;
                resultA = wingSolver->spanLiftForce[ind];
                break;

            default:
                break;
            }
            wingSolver = nullptr;

            for(int i = 0;i<ytA.length();i++)
                spanSeriesA->append(ytA[i],resultA[i]);

            tmp.append(spanSeriesA);



        }
        if(airplaneDataArray[airplaneChoiceIndex].isChoice[1]){
            wingVLM *wingSolver = airplaneSolverArray[airplaneChoiceIndex]->hTailModel;
            switch(spanResultTypeIndex){
            case 0:
                ytB = wingSolver->spanForceYt;
                resultB = wingSolver->spanForce[ind];
                //resultB = wingSolver->spanLiftCoefficient[ind];

                break;
            case 1:
                ytB = wingSolver->spanForceYt;
                resultB = wingSolver->spanLiftCoefficient[ind];
                break;
            case 2:
                ytB = wingSolver->yt;
                resultB = wingSolver->spanMonmentArray[ind];
                break;
            case 3:
                ytB = wingSolver->spanForceYt;
                resultB = wingSolver->spanLiftForce[ind];
                break;

            default:
                break;
            }
            for(int i = 0;i<ytB.length();i++)
                spanSeriesB->append(ytB[i],resultB[i]);

            tmp.append(spanSeriesB);


        }
        updateAxes(spanChart,tmp);
    }

}
QPen airplaneDisplay::getPen(const int index){
    QPen pen;
    pen.setColor(colorArray[index]);
    switch (resultPenArray[index]) {
    case 0:
        pen.setStyle(Qt::SolidLine);
        pen.setWidth(2);
        break;
    case 1:
        pen.setStyle(Qt::SolidLine);
        pen.setWidth(3);
        break;
    case 2:
        pen.setStyle(Qt::DashLine);
        pen.setWidth(2);
        break;
    case 3:
        pen.setStyle(Qt::DashLine);
        pen.setWidth(3);
        break;
    case 4:
        pen.setStyle(Qt::DashDotLine);
        pen.setWidth(2);
        break;
    case 5:
        pen.setStyle(Qt::DashDotLine);
        pen.setWidth(3);
        break;
    case 6:
        pen.setStyle(Qt::DotLine);
        pen.setWidth(2);
        break;
    case 7:
        pen.setStyle(Qt::DotLine);
        pen.setWidth(3);
        break;
    default:
        break;
    }
    return pen;
}
void airplaneDisplay::changeUIForAirplaneDesign(){
    modelType = AIRPLANE_DESIGN;
    changeModelViewButton->setButtonIcon(curveIconB1,QSize(40,40));
    changeResultViewButton->setButtonIcon(curveIconA2,QSize(40,40));
    changeSpanResultViewButton->setButtonIcon(curveIconA3,QSize(40,40));

    changeModelViewButton->setEnabled(false);
    changeResultViewButton->setEnabled(true);
    changeSpanResultViewButton->setEnabled(true);

    chartWWidget->hide();
    chartViewWA->hide();
    chartViewWB->hide();
    chartViewWC->hide();
    chartViewWD->hide();

    spanChartView->hide();

    airplaneDisplayWidgetA->show();

    if(airplaneDataArray.length() > airplaneChoiceIndex){

        int index = airplaneDataArray[airplaneChoiceIndex].compoentIndex[0];
        updateGLTextA(airplaneDataToString(wingDataArray[index]));
    }

}
void airplaneDisplay::changeUIForAirplaneResult(){
    modelType = AIRPLANE_RESULTA;
    changeModelViewButton->setButtonIcon(curveIconA1,QSize(40,40));
    changeResultViewButton->setButtonIcon(curveIconB2,QSize(40,40));
    changeSpanResultViewButton->setButtonIcon(curveIconA3,QSize(40,40));

    changeModelViewButton->setEnabled(true);
    changeResultViewButton->setEnabled(false);
    changeSpanResultViewButton->setEnabled(true);

    airplaneDisplayWidgetA->hide();
    spanChartView->hide();
    chartViewWA->show();
    chartViewWB->show();
    chartViewWC->show();
    chartViewWD->show();

    chartWWidget->show();

    airplaneTreeWidget->show();


}
void airplaneDisplay::changeUIForAirplaneSpanResult(){
    modelType = AIRPLANE_RESULTB;
    changeModelViewButton->setButtonIcon(curveIconA1,QSize(40,40));
    changeResultViewButton->setButtonIcon(curveIconA2,QSize(40,40));
    changeSpanResultViewButton->setButtonIcon(curveIconB3,QSize(40,40));

    changeSpanResultViewButton->setEnabled(false);
    changeModelViewButton->setEnabled(true);
    changeResultViewButton->setEnabled(true);

    chartWWidget->hide();
    airplaneDisplayWidgetA->hide();
    spanChartView->show();
    chartViewWA->hide();
    chartViewWB->hide();
    chartViewWC->hide();
    chartViewWD->hide();


    airplaneTreeWidget->show();

}
void airplaneDisplay::updateGLTextA(QString text){

    std::string tmp = text.toStdString();
    textActorA->SetInput(tmp.c_str());
    textActorA->Modified();
    airplaneDisplayWidgetA->renderWindow()->Render();


}
QString airplaneDisplay::airplaneDataToString(wingDefinition&wingData){
    int index = airplaneAlphaCombox->currentIndex();
    QString text,tmp[14];

    wingData.computeWingMessage();
    int len = wingData.chordLengthW.length() - 1;
    tmp[0] = airplaneDataArray[airplaneChoiceIndex].name +  "\n"+ "翼展:" + QString::number(wingData.spanW[len] * 2,'f',2) +"m" +  "\n";
    tmp[1] = "参考面积:" + QString::number(wingData.Area(),'f',2) +"㎡" + "\n";
    tmp[2] = "展弦比:" + QString::number(wingData.AspectRatio(),'f',2) + "\n";
    tmp[3] = "平均气动弦长" + QString::number(wingData.RealChord(),'f',2) + "m" + "\n";


    if(resultArray[airplaneChoiceIndex].isEmpty()){
        tmp[4] = "迎角:\n";
        tmp[5] = "升力系数:\n";
        tmp[6] = "阻力系数:\n";
        tmp[7] = "力矩系数:\n";
        tmp[8] = "升阻比:\n";
        tmp[9] = "功率因子:\n";
        tmp[10] = "速度:\n";
        tmp[11] = "升力:\n";
        tmp[12] = "阻力:\n";
        tmp[13] = "翼载荷:";

    }else{
        double v = airplaneSolverArray[airplaneChoiceIndex]->vinf;
        double tmpP = airplaneSolverArray[airplaneChoiceIndex]->density * v * v * 0.5 * wingData.Area();
        tmp[4] = "迎角:" + QString::number(resultArray[airplaneChoiceIndex][0][index],'f',2) + "\n";
        tmp[5] = "升力系数:" + QString::number(resultArray[airplaneChoiceIndex][1][index],'f',3) + "\n";
        tmp[6] = "阻力系数:" + QString::number(resultArray[airplaneChoiceIndex][2][index],'f',4) + "\n";
        tmp[7] = "力矩系数:" + QString::number(resultArray[airplaneChoiceIndex][3][index],'f',4) + "\n";
        tmp[8] = "升阻比:" + QString::number(resultArray[airplaneChoiceIndex][4][index],'f',2) + "\n";
        tmp[9] = "功率因子:" + QString::number(resultArray[airplaneChoiceIndex][5][index],'f',2) + "\n";
        tmp[10] = "速度:" + QString::number(airplaneSolverArray[airplaneChoiceIndex]->vinf,'f',2) +"m/s" + "\n";
        tmp[11] = "升力:" + QString::number(airplaneSolverArray[airplaneChoiceIndex]->clArray[index] * tmpP,'f',2)+"N" + "\n";
        tmp[12] = "阻力:" + QString::number(airplaneSolverArray[airplaneChoiceIndex]->cdArray[index] * tmpP,'f',2)+"N" + "\n";
        tmp[13] = "翼载荷:" + QString::number(airplaneSolverArray[airplaneChoiceIndex]->clArray[index] * tmpP / wingData.Area() / 9.81,'f',2) + "kg/㎡";
        }



    for(int i = 0;i<14;i++){
        text += tmp[i];
    }
    return text;
}
void airplaneDisplay::changeAirplaneDisplay(QTreeWidgetItem* item, int column){
    if (!item->parent()) { // 判断是否是子节点
        int index = item->data(0, Qt::UserRole).toInt();
        airplaneChoiceIndex = index;
        if(airplaneDataArray.length() > airplaneChoiceIndex){

            int index = airplaneDataArray[airplaneChoiceIndex].compoentIndex[0];
            if(!wingDataArray.isEmpty())
                updateGLTextA(airplaneDataToString(wingDataArray[index]));
        }

        updateAirplaneDefineWidget();
    }
}
void airplaneDisplay::setAirplaneXfoilDrag(QTreeWidgetItem *item, int column){
    if (item->checkState(column) == Qt::Checked) { // 判断是否是子节点
        QString txt = item->data(0,Qt::UserRole).toString();
        QString txtTmp = txt.mid(0,14);
        QString tmp1 = txt.mid(14);
        QString tmp = "childXfoilDrag";
        if(tmp == txtTmp &&  tmp1.toInt() == airplaneChoiceIndex ){
            dragChoiceDialog->show();
            //airplaneSolverArray[airplaneChoiceIndex]->setXfoilDrag(true);


            // VLMSolverArray[wingChoiceIndex]->setXfoilDrag(false);
            // qDebug()<<wingChoiceIndex;
        }
    }else{

        QString txt = item->data(0,Qt::UserRole).toString();
        QString txtTmp = txt.mid(0,14);
        QString tmp1 = txt.mid(14);
        QString tmp = "childXfoilDrag";
        if(tmp == txtTmp &&  tmp1.toInt() == airplaneChoiceIndex){
            if(!airplaneSolverArray.isEmpty()){
            airplaneSolverArray[airplaneChoiceIndex]->setXfoilDrag(false);

            }
        }
    }
}
void airplaneDisplay::showAirplaneZeroLiftDragDialog(QTreeWidgetItem* item, int column){


    if (item->checkState(column) == Qt::Checked) { // 判断是否是子节点
        QString txt = item->data(0,Qt::UserRole).toString();
        QString txtTmp = txt.mid(0,17);
        QString tmp1 = txt.mid(17);
        QString tmp = "childZeroLiftDrag";

        if(tmp == txtTmp &&  tmp1.toInt() == airplaneChoiceIndex){

            zeroLiftDragChoiceDialog->show();
            if(!airplaneSolverArray.isEmpty()){
                airplaneSolverArray[airplaneChoiceIndex]->setZeroLiftDrag(true);
            //defineWing->updatewingDefineWidget(wingDataArray[wingChoiceIndex]);
        }
            }
    }else{

        QString txt = item->data(0,Qt::UserRole).toString();
        QString txtTmp = txt.mid(0,17);
        QString tmp1 = txt.mid(17);
        QString tmp = "childZeroLiftDrag";
        if(tmp == txtTmp &&  tmp1.toInt() == airplaneChoiceIndex){
            if(!airplaneSolverArray.isEmpty()){
                airplaneSolverArray[airplaneChoiceIndex]->setZeroLiftDrag(false);
            //defineWing->updatewingDefineWidget(wingDataArray[wingChoiceIndex]);
            }
        }
    }
}
void airplaneDisplay::changeAirplaneData(QTreeWidgetItem* item, int column){
    if (item->parent()) { // 判断是否是子节点



        QString txt = item->data(0,Qt::UserRole).toString();
        QString txtTmp = txt.mid(0,10);
        QString tmp1 = txt.mid(10);
        QString tmp = "childShape";

        if(tmp == txtTmp &&  tmp1.toInt() == airplaneChoiceIndex){
            //outputWingButton->setEnabled(true);
            //isModify = true;
            isModify = true;

            //SpanNumB = wingDataArray[wingChoiceIndex].chordLengthW.length();//更新机翼编辑列表的行数
            //updateWingEdit(wingChoiceIndex);
            //wingNameEdit->setText(wingDataArray[wingChoiceIndex].name);
            //wingDefineWidget->show();

            for(int i = 0;i<5;i++)
                listIndexArray[i] = airplaneDataArray[airplaneChoiceIndex].compoentIndex[i];
            updateAirplaneDialog();
            airplaneDefineDialog->show();
            //updateView(wingDataArray[wingChoiceIndex]);
            //defineWing->updatewingDefineWidget(wingDataArray[wingChoiceIndex]);



        }
    }
}
void airplaneDisplay::showAirplaneCompoentDefineWidget(QTreeWidgetItem *item, int column){
    if (item->parent()) { // 判断是否是子节点
        QString txt = item->data(0,Qt::UserRole).toString();
        QString txtTmp = txt.mid(0,8);
        QString tmp1 = txt.mid(9);
        QString tmp2 = txt.at(8);
        QString tmp = "compoent";
        if(txtTmp == tmp &&  tmp1.toInt() == airplaneChoiceIndex){
            int index = tmp2.toInt();
            int compoentIndex = airplaneDataArray[airplaneChoiceIndex].compoentIndex[index];//获得部件的索引
            if(index <= 2){
                emit emitWingDefineDialog(index,compoentIndex);



            }


        }



    }
}
void airplaneDisplay::showAirplanePressureContour(QTreeWidgetItem *item, int column){
    if (item->checkState(column) == Qt::Checked) { // 判断是否是子节点


        QString txt = item->data(0,Qt::UserRole).toString();
        QString txtTmp = txt.mid(0,12);
        QString tmp1 = txt.mid(12);
        QString tmp = "childPrssure";
        if(tmp == txtTmp &&  tmp1.toInt() == airplaneChoiceIndex){
            //zeroLiftDragChoiceDialog->show();
            //defineWing->updatewingDefineWidget(wingDataArray[wingChoiceIndex]);
            //defineWing->updateStreamLineView();
            //qDebug()<<"PrssureContour"<<wingChoiceIndex;
            //updatePressureContourView(VLMSolverArray[wingChoiceIndex]);
            updateAirplanePressureContourView();
        }
    }else{

        QString txt = item->data(0,Qt::UserRole).toString();
        QString txtTmp = txt.mid(0,12);
        QString tmp1 = txt.mid(12);
        QString tmp = "childPrssure";
        if(tmp == txtTmp &&  tmp1.toInt() == airplaneChoiceIndex){
            //zeroLiftDragChoiceDialog->show();
            //defineWing->updatewingDefineWidget(wingDataArray[wingChoiceIndex]);
            //qDebug()<<"NoPrssureContour"<<wingChoiceIndex;
            hidePressureContourView();
        }
    }
}
void airplaneDisplay::showAirplaneStreamLineView(QTreeWidgetItem* item, int column){
    if (item->checkState(column) == Qt::Checked) { // 判断是否是子节点
        QString txt = item->data(0,Qt::UserRole).toString();
        QString txtTmp = txt.mid(0,15);
        QString tmp1 = txt.mid(15);
        QString tmp = "childStreamLine";
        if(tmp == txtTmp &&  tmp1.toInt() == airplaneChoiceIndex){
            streamSettingDialog->show();
        }
    }else{
        QString txt = item->data(0,Qt::UserRole).toString();
        QString txtTmp = txt.mid(0,15);
        QString tmp1 = txt.mid(15);
        QString tmp = "childStreamLine";
        if(tmp == txtTmp &&  tmp1.toInt() == airplaneChoiceIndex){
            hideStreamLineView();
        }
    }
}
void airplaneDisplay::showAirplaneStreamLine(){
    streamSettingDialog->hide();
    int index = airplaneAlphaCombox->currentIndex();
    double a1 = streamEditArray[0]->text().toDouble();
    double a2 = streamEditArray[1]->text().toDouble();
    double a3 = streamEditArray[2]->text().toDouble();
    double a4 = streamEditArray[3]->text().toDouble();

    airplaneSolverArray[airplaneChoiceIndex]->isShowStreamLine = true;
    if(index >= 0){
        startCalculation();
        airplaneSolverArray[airplaneChoiceIndex]->solveStreamLine(index,a1,a2,a3,a4);
        updateStreamLineView();
    }

}
void airplaneDisplay::changeAlphaView(const int index){
    switch (modelType){
    case AIRPLANE_DESIGN:

        updateAirplanePressureContourView();
        if(index >= 0 && airplaneSolverArray[airplaneChoiceIndex]->isShowStreamLine){
            double a1 = streamEditArray[0]->text().toDouble();
            double a2 = streamEditArray[1]->text().toDouble();
            double a3 = streamEditArray[2]->text().toDouble();
            double a4 = streamEditArray[3]->text().toDouble();
            startCalculation();

            airplaneSolverArray[airplaneChoiceIndex]->solveStreamLine(index,a1,a2,a3,a4);
            updateStreamLineView();



        }

        if(airplaneDataArray.length() > airplaneChoiceIndex){

            int index = airplaneDataArray[airplaneChoiceIndex].compoentIndex[0];
            updateGLTextA(airplaneDataToString(wingDataArray[index]));
        }

        break;
    case AIRPLANE_RESULTA:

        break;
    case AIRPLANE_RESULTB:

        drawSpanResult(airplaneChoiceIndex);

        break;
    default:
        break;

    }
}
void airplaneDisplay::updateAirplaneDefineWidget(){


     addWingToVTK();
     addHTailToVTK();
     addVTailToVTK();



}
void airplaneDisplay::updateAirplanePressureContourView(){

    bool ok1 = airplaneDataArray[airplaneChoiceIndex].isChoice[0];
    bool ok2 = airplaneDataArray[airplaneChoiceIndex].isChoice[1];
    bool ok3 = airplaneDataArray[airplaneChoiceIndex].isChoice[2];
    if(ok1)
        updateWingPressureContour();
    if(ok2)
        updateHTailPressureContour();
    if(ok3)
        updateVTailPressureContour();

}
void airplaneDisplay::updateWingPressureContour(){
    int index = airplaneAlphaCombox->currentIndex();
    actorWing->VisibilityOff();
    actorContourWing->VisibilityOn();
    pointsContourWing->Reset();
    linesContourWing->Reset();
    colorContourWing->Reset();
    //int ind = airplaneDataArray[airplaneChoiceIndex].compoentIndex[0];
    wingVLM *ned = airplaneSolverArray[airplaneChoiceIndex]->wingModel;
    for (int i = 0; i < ned->getMeshNum(); i++) {
        pointsContourWing->InsertNextPoint(ned->xyA[i].x, ned->xyA[i].y, ned->xyA[i].z);
        pointsContourWing->InsertNextPoint(ned->xyB[i].x, ned->xyB[i].y, ned->xyB[i].z);
        pointsContourWing->InsertNextPoint(ned->xyC[i].x, ned->xyC[i].y, ned->xyC[i].z);
        pointsContourWing->InsertNextPoint(ned->xyD[i].x, ned->xyD[i].y, ned->xyD[i].z);
    }

    for (vtkIdType i = 0; i < pointsContourWing->GetNumberOfPoints() - 3; i += 4) {
        vtkSmartPointer<vtkQuad> quad = vtkSmartPointer<vtkQuad>::New();
        quad->GetPointIds()->SetId(0, i);
        quad->GetPointIds()->SetId(1, i + 1);
        quad->GetPointIds()->SetId(2, i + 2);
        quad->GetPointIds()->SetId(3, i + 3);
        linesContourWing->InsertNextCell(quad);
    }

    // Set colors for each cell
    int blue[3] = {0, 0, 255};    // Blue
    int yellow[3] = {255, 255, 0}; // Yellow
    int red[3] = {255, 0, 0};     // Red

    // Ensure colors array is allocated and set correct number of components
    colorContourWing->SetNumberOfComponents(3);
    colorContourWing->SetNumberOfTuples(profilePolyDataContourWing->GetNumberOfCells());

    for (vtkIdType i = 0; i < profilePolyDataContourWing->GetNumberOfCells(); i++) {
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

        colorContourWing->InsertTypedTuple(i, rgb);
    }

    renwin->Render();

    // 将颜色数组添加到polyData

}
void airplaneDisplay::updateVTailPressureContour(){
    int index = airplaneAlphaCombox->currentIndex();
    actorVTail->VisibilityOff();
    actorContourVTail->VisibilityOn();
    pointsContourVTail->Reset();
    linesContourVTail->Reset();
    colorContourVTail->Reset();
    //int ind = airplaneDataArray[airplaneChoiceIndex].compoentIndex[0];
    wingVLM *ned = airplaneSolverArray[airplaneChoiceIndex]->vTailModel;
    for (int i = 0; i < ned->getMeshNum(); i++) {
        pointsContourVTail->InsertNextPoint(ned->xyA[i].x, ned->xyA[i].y, ned->xyA[i].z);
        pointsContourVTail->InsertNextPoint(ned->xyB[i].x, ned->xyB[i].y, ned->xyB[i].z);
        pointsContourVTail->InsertNextPoint(ned->xyC[i].x, ned->xyC[i].y, ned->xyC[i].z);
        pointsContourVTail->InsertNextPoint(ned->xyD[i].x, ned->xyD[i].y, ned->xyD[i].z);
    }

    for (vtkIdType i = 0; i < pointsContourVTail->GetNumberOfPoints() - 3; i += 4) {
        vtkSmartPointer<vtkQuad> quad = vtkSmartPointer<vtkQuad>::New();
        quad->GetPointIds()->SetId(0, i);
        quad->GetPointIds()->SetId(1, i + 1);
        quad->GetPointIds()->SetId(2, i + 2);
        quad->GetPointIds()->SetId(3, i + 3);
        linesContourVTail->InsertNextCell(quad);
    }

    // Set colors for each cell
    int blue[3] = {0, 0, 255};    // Blue
    int yellow[3] = {255, 255, 0}; // Yellow
    int red[3] = {255, 0, 0};     // Red

    // Ensure colors array is allocated and set correct number of components
    colorContourVTail->SetNumberOfComponents(3);
    colorContourVTail->SetNumberOfTuples(profilePolyDataContourVTail->GetNumberOfCells());

    for (vtkIdType i = 0; i < profilePolyDataContourVTail->GetNumberOfCells(); i++) {
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

        colorContourVTail->InsertTypedTuple(i, rgb);
    }

    renwin->Render();

    // 将颜色数组添加到polyData

}
void airplaneDisplay::updateHTailPressureContour(){
    int index = airplaneAlphaCombox->currentIndex();
    actorHTail->VisibilityOff();
    actorContourHTail->VisibilityOn();
    pointsContourHTail->Reset();
    linesContourHTail->Reset();
    colorContourHTail->Reset();
    //int ind = airplaneDataArray[airplaneChoiceIndex].compoentIndex[0];
    wingVLM *ned = airplaneSolverArray[airplaneChoiceIndex]->hTailModel;
    for (int i = 0; i < ned->getMeshNum(); i++) {
        pointsContourHTail->InsertNextPoint(ned->xyA[i].x, ned->xyA[i].y, ned->xyA[i].z);
        pointsContourHTail->InsertNextPoint(ned->xyB[i].x, ned->xyB[i].y, ned->xyB[i].z);
        pointsContourHTail->InsertNextPoint(ned->xyC[i].x, ned->xyC[i].y, ned->xyC[i].z);
        pointsContourHTail->InsertNextPoint(ned->xyD[i].x, ned->xyD[i].y, ned->xyD[i].z);
    }

    for (vtkIdType i = 0; i < pointsContourHTail->GetNumberOfPoints() - 3; i += 4) {
        vtkSmartPointer<vtkQuad> quad = vtkSmartPointer<vtkQuad>::New();
        quad->GetPointIds()->SetId(0, i);
        quad->GetPointIds()->SetId(1, i + 1);
        quad->GetPointIds()->SetId(2, i + 2);
        quad->GetPointIds()->SetId(3, i + 3);
        linesContourHTail->InsertNextCell(quad);
    }

    // Set colors for each cell
    int blue[3] = {0, 0, 255};    // Blue
    int yellow[3] = {255, 255, 0}; // Yellow
    int red[3] = {255, 0, 0};     // Red

    // Ensure colors array is allocated and set correct number of components
    colorContourHTail->SetNumberOfComponents(3);
    colorContourHTail->SetNumberOfTuples(profilePolyDataContourHTail->GetNumberOfCells());

    for (vtkIdType i = 0; i < profilePolyDataContourHTail->GetNumberOfCells(); i++) {
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

        colorContourHTail->InsertTypedTuple(i, rgb);
    }

    renwin->Render();

    // 将颜色数组添加到polyData

}
void airplaneDisplay::updateStreamLineView(){
    pointsS->Reset();
    linesS->Reset();
    actorS->VisibilityOn();

    // 用于记录所有点的全局索引
    vtkIdType globalPointIndex = 0;

    // 遍历每条线段

    for (int i = 0; i < airplaneSolverArray[airplaneChoiceIndex]->streamLineArray.length(); i++) {
        const QVector<point3d>& linePoints = airplaneSolverArray[airplaneChoiceIndex]->streamLineArray[i];

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

    airplaneDisplayWidgetA->renderWindow()->Render();
    //renwinB->Render();
}
void airplaneDisplay::hideStreamLineView(){

    airplaneSolverArray[airplaneChoiceIndex]->isShowStreamLine = false;
    actorS->VisibilityOff();
    airplaneDisplayWidgetA->renderWindow()->Render();
}
void airplaneDisplay::startCalculation() {

    dialogBar->setValue(0);
    progressDialog->show(); // 显示对话框

}
void airplaneDisplay::onCalculationFinished() {

    progressDialog->close();
}
void airplaneDisplay::updateProgress(int value) {

    dialogBar->setValue(value);
    QApplication::processEvents(); // 确保界面更新

}
void airplaneDisplay::updateAirplaneMessage(){
    bool isOK1 = readyButton[0]->property("status").toInt();
    bool isOK2 = readyButton[1]->property("status").toInt();
    bool isOK3 = readyButton[2]->property("status").toInt();
    if(isOK1){
        int index1 = listIndexArray[0];
        int index2 = listIndexArray[1];
        int index3 = listIndexArray[2];
        wingDataArray[index1].computeWingMessage();
        labelArray[0]->setText("翼展: "  + QString::number(wingDataArray[index1].Span(),'f',2) + "m");
        labelArray[1]->setText("参考面积: " + QString::number(wingDataArray[index1].Area(),'f',2) + "㎡");
        labelArray[2]->setText("展弦比: " + QString::number(wingDataArray[index1].AspectRatio(),'f',2));
        if(isOK2){
            labelArray[3]->setText("平尾面积: " + QString::number(tailDataArray[index2].Area(),'f',2) + "㎡");
        }else{
            labelArray[3]->setText("平尾面积:");
        }
        if(isOK3){
            labelArray[4]->setText("垂尾面积: " + QString::number(tailDataArray[index3].Area(),'f',2) + "㎡");
        }else{
            labelArray[4]->setText("垂尾面积:");
        }
        labelArray[5]->setText("参考力臂:");
        labelArray[6]->setText("平尾容量");
        labelArray[7]->setText("垂尾容量");
        labelArray[8]->setText("名称");

    }


}
void airplaneDisplay::updateAxes(QChart *chart, const QVector<QLineSeries*> &seriesList) {
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
void airplaneDisplay::showResultNode(QTreeWidgetItem *root, const QString &textToFind) {
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
void airplaneDisplay::changeProgressUpdate(const int value){
    emit emitAirplaneProgressBarValue(value);
}
void airplaneDisplay::changeZeroLiftDragText(){
    QPushButton *button = static_cast<QPushButton*>(sender());
    int index = button->property("button").toInt();
    dragEdit->setText(QString::number(zeroLiftDragValueArray[index]));
}
void airplaneDisplay::setZeroLiftDrag(){
    zeroLiftDragChoiceDialog->hide();
    double drag = dragEdit->text().toDouble();
    if(drag <0 || drag >1)
        QMessageBox::information(this,"警告","设置正确的零升阻力系数");
    airplaneSolverArray[airplaneChoiceIndex]->viscousCd = drag;

}
void airplaneDisplay::cancelDragDialog(){
    airplaneSolverArray[airplaneChoiceIndex]->setXfoilDrag(false);

    QString name = "childXfoilDrag" + QString::number(airplaneChoiceIndex);
    QTreeWidgetItem* target = findNodeByIdentifier(airplaneTreeItemArray[airplaneChoiceIndex], name);
    if (target) {
        target->setCheckState(0, Qt::Unchecked);
    }
    airplaneSolverArray[airplaneChoiceIndex]->setXfoilDrag(false);
    airplaneSolverArray[airplaneChoiceIndex]->setUseLibraries(false);

    //uncheckNodeByIdentifier(wingTreeWidget, name);
    dragChoiceDialog->close();
}
void airplaneDisplay::saveDragDialog(){
    bool ok1 = dragBoxA->checkState();
    bool ok2 = dragBoxB->checkState();
    if(!ok1 && !ok2){

    }else{
        if(ok1){
            airplaneSolverArray[airplaneChoiceIndex]->setXfoilDrag(true);
            airplaneSolverArray[airplaneChoiceIndex]->setUseLibraries(false);
        }else{
            airplaneSolverArray[airplaneChoiceIndex]->setXfoilDrag(true);
            airplaneSolverArray[airplaneChoiceIndex]->setUseLibraries(true);
        }
    }



    dragChoiceDialog->close();
}
QTreeWidgetItem* airplaneDisplay::findNodeByIdentifier(QTreeWidgetItem* parent, const QString& identifier) {
    if (!parent) return nullptr;

    // 检查当前节点是否匹配标识
    if (parent->data(0, Qt::UserRole).toString() == identifier) {
        return parent;
    }

    // 递归检查所有子节点
    for (int i = 0; i < parent->childCount(); ++i) {
        QTreeWidgetItem* found = findNodeByIdentifier(parent->child(i), identifier);
        if (found) {
            return found;
        }
    }

    return nullptr;
}
void airplaneDisplay::showChartAMenu(){
    chartIndex = 0;
    for (QAction *act : rChartMenu->actions()) {
        act->setChecked(false);
    }
    actionArray[chartTypeIndexArray[0]]->setChecked(true);

    rChartMenu->exec(QCursor::pos());
}
void airplaneDisplay::showChartBMenu(){
    chartIndex = 1;
    for (QAction *act : rChartMenu->actions()) {
        act->setChecked(false);
    }
    actionArray[chartTypeIndexArray[1]]->setChecked(true);

    rChartMenu->exec(QCursor::pos());

}
void airplaneDisplay::showChartCMenu(){
    chartIndex = 2;
    for (QAction *act : rChartMenu->actions()) {
        act->setChecked(false);
    }
    actionArray[chartTypeIndexArray[2]]->setChecked(true);

    rChartMenu->exec(QCursor::pos());

}
void airplaneDisplay::showChartDMenu(){
    chartIndex = 3;
    for (QAction *act : rChartMenu->actions()) {
        act->setChecked(false);
    }
    actionArray[chartTypeIndexArray[3]]->setChecked(true);

    rChartMenu->exec(QCursor::pos());

}
void airplaneDisplay::showSpanChartMenu(){
    for (QAction *act : spanRMenu->actions()) {
        act->setChecked(false);
    }
    spanActionArray[spanResultTypeIndex]->setChecked(true);
    spanRMenu->exec(QCursor::pos());
}
void airplaneDisplay::changeResultType(){
    QAction *action = qobject_cast<QAction*>(sender());
    int index = action->property("actions").toInt();
    if (action) {
        // 取消所有其他动作的选中状态
        for (QAction *act : rChartMenu->actions()) {
            act->setChecked(false);
        }
    }
    int index2;


    // 设置当前动作为选中状态
    action->setChecked(true);
    switch (chartIndex) {
    case 0:
        if(index < 9){
            clearDataPointA();
            index2 = index;
            xAxisIndexA = axisIndex[index2][0], yAxisIndexA = axisIndex[index2][1];
            axisXWA->setTitleText(axisXName[index2]);
            axisYWA->setTitleText(axisYName[index2]);
            chartWA->setTitle(titleName[index2]);
            chartTypeIndexArray[0] = index2;
            for(int i = 0;i<resultArray.length();i++)
                drawResultChartA(i);
            updateAxes(chartWA,airplaneResultSeriesA);
        }else{
            exportChartData(chartWA);
            QMessageBox::information(this,"信息提示","保存成功");
        }


        break;
    case 1:
        if(index < 9){
            clearDataPointB();
            index2 = index;
            xAxisIndexB = axisIndex[index2][0], yAxisIndexB = axisIndex[index2][1];

            axisXWB->setTitleText(axisXName[index2]);
            axisYWB->setTitleText(axisYName[index2]);
            chartWB->setTitle(titleName[index2]);
            chartTypeIndexArray[1] = index2;
            for(int i = 0;i<resultArray.length();i++)
                drawResultChartB(i);
            updateAxes(chartWB,airplaneResultSeriesB);
        }else{
            exportChartData(chartWB);
            QMessageBox::information(this,"信息提示","保存成功");
        }


        break;
    case 2:
        if(index < 9){
            clearDataPointC();
            index2 = index;
            xAxisIndexC = axisIndex[index2][0], yAxisIndexC = axisIndex[index2][1];

            axisXWC->setTitleText(axisXName[index2]);
            axisYWC->setTitleText(axisYName[index2]);
            chartWC->setTitle(titleName[index2]);
            chartTypeIndexArray[2] = index2;
            for(int i = 0;i<resultArray.length();i++)
                drawResultChartC(i);
            updateAxes(chartWC,airplaneResultSeriesC);
        }else{
            exportChartData(chartWC);
            QMessageBox::information(this,"信息提示","保存成功");
        }


        break;
    case 3:
        if(index < 9){
            clearDataPointD();
            index2 = index;
            xAxisIndexD = axisIndex[index2][0], yAxisIndexD = axisIndex[index2][1];

            axisXWD->setTitleText(axisXName[index2]);
            axisYWD->setTitleText(axisYName[index2]);
            chartWD->setTitle(titleName[index2]);
            chartTypeIndexArray[3] = index2;
            for(int i = 0;i<resultArray.length();i++)
                drawResultChartD(i);
            updateAxes(chartWD,airplaneResultSeriesD);
        }else{
            exportChartData(chartWD);
            QMessageBox::information(this,"信息提示","保存成功");
        }


        break;
    default:
        break;
    }

}
void airplaneDisplay::clearDataPointA()
{
    // Clear all data points from the autoSeriesA and autoSeriesB
    autoSeriesA->clear();
    //autoSeriesB->clear();
    updateAxes(chartWA,airplaneResultSeriesA);

}
void airplaneDisplay::clearDataPointB()
{
    // Clear all data points from the autoSeriesA and autoSeriesB
    autoSeriesB->clear();
    //autoSeriesB->clear();
    updateAxes(chartWB,airplaneResultSeriesB);

}
void airplaneDisplay::clearDataPointC()
{
    // Clear all data points from the autoSeriesA and autoSeriesB
    autoSeriesC->clear();
    //autoSeriesB->clear();
    updateAxes(chartWC,airplaneResultSeriesC);

}
void airplaneDisplay::clearDataPointD()
{
    // Clear all data points from the autoSeriesA and autoSeriesB
    autoSeriesD->clear();
    //autoSeriesB->clear();
    updateAxes(chartWD,airplaneResultSeriesD);

}
void airplaneDisplay::addDataPointA()
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
            if (x > axisXWA->max()) {
                axisXWA->setMax(x);
            }
            if (x < axisXWA->min()) {
                axisXWA->setMin(x);
            }
            if (y > axisYWA->max()) {
                axisYWA->setMax(y);
            }
            if (y < axisYWA->min()) {
                axisYWA->setMin(y);
            }
        }
    }
}
void airplaneDisplay::addDataPointB()
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
            if (x > axisXWB->max()) {
                axisXWB->setMax(x);
            }
            if (x < axisXWB->min()) {
                axisXWB->setMin(x);
            }
            if (y > axisYWB->max()) {
                axisYWB->setMax(y);
            }
            if (y < axisYWB->min()) {
                axisYWB->setMin(y);
            }
        }
    }
}
void airplaneDisplay::addDataPointC()
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
            if (x > axisXWC->max()) {
                axisXWC->setMax(x);
            }
            if (x < axisXWC->min()) {
                axisXWC->setMin(x);
            }
            if (y > axisYWC->max()) {
                axisYWC->setMax(y);
            }
            if (y < axisYWC->min()) {
                axisYWC->setMin(y);
            }
        }
    }
}
void airplaneDisplay::addDataPointD()
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
            if (x > axisXWD->max()) {
                axisXWD->setMax(x);
            }
            if (x < axisXWD->min()) {
                axisXWD->setMin(x);
            }
            if (y > axisYWD->max()) {
                axisYWD->setMax(y);
            }
            if (y < axisYWD->min()) {
                axisYWD->setMin(y);
            }
        }
    }
}
void airplaneDisplay::clearDataPoint(){
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
void airplaneDisplay::addDataPoint(){
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
void airplaneDisplay::closeOtherRoots(QTreeWidgetItem* item, int column){
    if (!airplaneTreeWidget || !item) return;

    // 找到当前点击项所在的根节点
    QTreeWidgetItem* currentRoot = item;
    while (currentRoot->parent() != nullptr) {
        currentRoot = currentRoot->parent();
    }

    // 遍历所有根节点，折叠非当前根节点
    for (int i = 0; i < airplaneTreeWidget->topLevelItemCount(); ++i) {
        QTreeWidgetItem* root = airplaneTreeWidget->topLevelItem(i);

        if (root != currentRoot) {
            airplaneTreeWidget->collapseItem(root); // 折叠根节点，子节点会自动折叠
        }else{
            //wingTreeWidget->expandItem(root); // 展开当前根节点（可选）
        }

    }
}
void airplaneDisplay::hidePressureContourView(){
    bool ok1 = airplaneDataArray[airplaneChoiceIndex].isChoice[0];
    bool ok2 = airplaneDataArray[airplaneChoiceIndex].isChoice[1];
    bool ok3 = airplaneDataArray[airplaneChoiceIndex].isChoice[2];

    if(ok1){
        actorContourWing->VisibilityOff();
        actorWing->VisibilityOn();
    }
    if(ok2){
        actorContourHTail->VisibilityOff();
        actorHTail->VisibilityOn();
    }
    if(ok3){
        actorContourVTail->VisibilityOff();
        actorVTail->VisibilityOn();
    }
    airplaneDisplayWidgetA->renderWindow()->Render();
}
void airplaneDisplay::addTreeNode(const QString name){
    QString path1 = ":/images/edit.png";
    QIcon icon1 ;
    icon1.addPixmap(QPixmap(path1),QIcon::Normal,QIcon::On);

    QString path2 = ":/images/window/airplane/airplane.png";
    QIcon icon2 ;
    icon2.addPixmap(QPixmap(path2),QIcon::Normal,QIcon::On);

    QString path3 = ":/images/window/airplane/wing.png";
    QIcon icon3 ;
    icon3.addPixmap(QPixmap(path3),QIcon::Normal,QIcon::On);

    QString path4 = ":/images/window/airplane/power.png";
    QIcon icon4 ;
    icon4.addPixmap(QPixmap(path4),QIcon::Normal,QIcon::On);

    QString path5 = ":/images/window/airplane/fuselage.png";
    QIcon icon5 ;
    icon5.addPixmap(QPixmap(path5),QIcon::Normal,QIcon::On);


    QTreeWidgetItem *fItem = new QTreeWidgetItem(airplaneTreeWidget,QStringList(name));
    fItem->setData(0,Qt::UserRole, QVariant(QString::number(airplaneIndex)));
    fItem->setIcon(0,icon2);

    QTreeWidgetItem *fItemGeometry = new QTreeWidgetItem(fItem,QStringList("几何模型"));
    fItemGeometry->setData(0,Qt::UserRole,"fItemGeometry");

    QTreeWidgetItem *childShape = new QTreeWidgetItem(fItemGeometry,QStringList("平面形状"));
    childShape->setData(0, Qt::UserRole, QVariant("childShape" + QString::number(airplaneIndex)));
    childShape->setIcon(0,icon1);

    QTreeWidgetItem *compoent = new QTreeWidgetItem(fItem,QStringList("组件"));
    compoent->setData(0, Qt::UserRole, QVariant("airplaneCompoent" + QString::number(airplaneIndex)));

    QTreeWidgetItem *wing = new QTreeWidgetItem(compoent,QStringList("机翼"));
    wing->setData(0, Qt::UserRole, QVariant("compoent0" + QString::number(airplaneIndex)));
    wing->setIcon(0,icon3);

    QTreeWidgetItem *hTail = new QTreeWidgetItem(compoent,QStringList("平尾"));
    hTail->setData(0, Qt::UserRole, QVariant("compoent1" + QString::number(airplaneIndex)));
    hTail->setIcon(0,icon3);

    QTreeWidgetItem *vTail = new QTreeWidgetItem(compoent,QStringList("垂尾"));
    vTail->setData(0, Qt::UserRole, QVariant("compoent2" + QString::number(airplaneIndex)));
    vTail->setIcon(0,icon3);

    QTreeWidgetItem *power = new QTreeWidgetItem(compoent,QStringList("动力"));
    power->setData(0, Qt::UserRole, QVariant("compoent3" + QString::number(airplaneIndex)));
    power->setIcon(0,icon4);

    QTreeWidgetItem *fuselage = new QTreeWidgetItem(compoent,QStringList("机身"));
    fuselage->setData(0, Qt::UserRole, QVariant("compoent4" + QString::number(airplaneIndex)));
    fuselage->setIcon(0,icon5);


    QTreeWidgetItem *fItemAnalyse = new QTreeWidgetItem(fItem,QStringList("求解设置"));
    fItemAnalyse->setData(0, Qt::UserRole, QVariant("childSolver" + QString::number(airplaneIndex)));
    //fItemAnalyse->setData(0, Qt::UserRole, "fItemAnalyse");


    QTreeWidgetItem *childVinf = new QTreeWidgetItem(fItemAnalyse,QStringList("来流设置"));
    childVinf->setData(0, Qt::UserRole, QVariant("childVinf" + QString::number(airplaneIndex)));
    childVinf->setIcon(0,icon1);

    QTreeWidgetItem *childZeroLiftDrag = new QTreeWidgetItem(fItemAnalyse,QStringList("零升阻力"));
    childZeroLiftDrag->setData(0, Qt::UserRole, QVariant("childZeroLiftDrag" + QString::number(airplaneIndex)));
    childZeroLiftDrag->setCheckState(0,Qt::Unchecked);

    QTreeWidgetItem *childXfoilDrag = new QTreeWidgetItem(fItemAnalyse,QStringList("Xfoil阻力"));
    childXfoilDrag->setData(0, Qt::UserRole, QVariant("childXfoilDrag" + QString::number(airplaneIndex)));
    childXfoilDrag->setCheckState(0,Qt::Unchecked);


    QTreeWidgetItem *wakeView = new QTreeWidgetItem(fItemAnalyse,QStringList("显示尾涡"));
    wakeView->setData(0, Qt::UserRole, QVariant("wakeView" + QString::number(airplaneIndex)));
    wakeView->setCheckState(0,Qt::Unchecked);




    //childZeroLiftDrag->setData(0, Qt::UserRole, QVariant(QString::number(wingIndex) + "childZeroLiftDrag"));

    QTreeWidgetItem *fItemResult = new QTreeWidgetItem(fItem,QStringList("后处理"));
    QTreeWidgetItem *childPrssure = new QTreeWidgetItem(fItemResult,QStringList("压力"));
    childPrssure->setData(0, Qt::UserRole, QVariant("childPrssure" + QString::number(airplaneIndex)));
    childPrssure->setCheckState(0,Qt::Unchecked);

    QTreeWidgetItem *childStreamLine = new QTreeWidgetItem(fItemResult,QStringList("流线"));
    childStreamLine->setData(0, Qt::UserRole, QVariant("childStreamLine" + QString::number(airplaneIndex)));
    childStreamLine->setCheckState(0,Qt::Unchecked);

    QTreeWidgetItem *childSeriesType = new QTreeWidgetItem(fItemResult,QStringList("曲线类型"));
    childSeriesType->setData(0, Qt::UserRole, QVariant("childSeriesType" + QString::number(airplaneIndex)));
    childSeriesType->setIcon(0,icon1);

    fItemResult->setHidden(true);

    airplaneTreeItemArray.append(fItem);
}
int airplaneDisplay::getReadyButtonIndex(QPushButton* button) {
    for (int i = 0; i < 5; ++i) { // 遍历数组（注意数组长度需正确）
        if (readyButton[i] == button) { // 指针地址比对
            return i; // 找到匹配，返回索引
        }
    }
    return -1; // 未找到（无效索引）
}
