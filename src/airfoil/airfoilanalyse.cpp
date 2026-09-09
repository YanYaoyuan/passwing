#include "airfoilanalyse.h"
#include "airfoildesign.h"

#include "airfoilsolve.h"
#include <QMessageBox>
#include <QFileDialog>
#include <QComboBox>
#if defined(_MSC_VER) && (_MSC_VER >= 1600)
# pragma execution_character_set("utf-8")
#endif


airfoilAnalyse::airfoilAnalyse(QWidget *parent)
    : QWidget(parent) {

    airfoilChoseWidget = new QWidget();
    moreAirfoilAnalyseDialog = new QDialog();
    moreReAnalyseWidget = new QWidget();

    resultWidget = new QWidget();




    gridLayout = new QGridLayout(airfoilChoseWidget);
    gridLayoutA = new QGridLayout(moreAirfoilAnalyseDialog);

    AirfoilB.resize(1000);
    initialAirfoilView();



    mouseLocitionAnalyse = new QGraphicsSimpleTextItem("");
    QPointF chartPos(35,350);


    mouseLocitionAnalyse->setBrush(QBrush(Qt::black));

    QPointF scenePos = chartAirfoil->mapToScene(chartPos);
    mouseLocitionAnalyse->setPos(scenePos);
    chartAirfoil->scene()->addItem(mouseLocitionAnalyse);
    colorDialog = new QColorDialog();


    initialWindow();
    initialSpinBox();
    buildMoreReAnalyseWidget();




}

void airfoilAnalyse::initialSetting(const QVector<double>&inputSettings){
    if(inputSettings.isEmpty()){
        MINAXISR[0] = -5;            MAXAXISR[0] = 15;
        MINAXISR[1] = -0.5;          MAXAXISR[1] = 2;
        MINAXISR[2] = 0.008;         MAXAXISR[2] = 0.02;
        MINAXISR[3] = -50;           MAXAXISR[3] = 120;
        MINAXISR[4] = -50;           MAXAXISR[4] = 100;
        MINAXISR[5] = 0.3;           MAXAXISR[5] = 0.3;
        MINAXISR[6] = -3;            MAXAXISR[6] = 3;
        MINAXISR[7] = 6;             MAXAXISR[7] = 6;
    }
    else{
        MINAXISR[0] = inputSettings[0];    MAXAXISR[0] = inputSettings[8];
        MINAXISR[1] = inputSettings[1];    MAXAXISR[1] = inputSettings[9];
        MINAXISR[2] = inputSettings[2];    MAXAXISR[2] = inputSettings[10];
        MINAXISR[3] = inputSettings[3];    MAXAXISR[3] = inputSettings[11];
        MINAXISR[4] = inputSettings[4];    MAXAXISR[4] = inputSettings[12];
        MINAXISR[5] = inputSettings[5];    MAXAXISR[5] = inputSettings[13];
        MINAXISR[6] = inputSettings[6];    MAXAXISR[6] = inputSettings[14];
        MINAXISR[7] = inputSettings[7];    MAXAXISR[7] = inputSettings[15];
    }

    initialChartRA();
    initialChartRB();
    initialResultUI();

}
void airfoilAnalyse::initialWindow(){


    QColor color1(0,255,255);
    QColor color2(85,85,255);
    QColor color3(0,255,0);
    QColor color4(0,0,0);
    QColor color5(255,0,255);
    QColor color6(255,0,0);
    QColor color7(170,170,127);
    QColor color8(170,0,127);

    colorArray.append(color1);
    colorArray.append(color2);
    colorArray.append(color3);
    colorArray.append(color4);
    colorArray.append(color5);
    colorArray.append(color6);
    colorArray.append(color7);
    colorArray.append(color8);


    for(int i = 0; i < 8;i++){
        QCheckBox *checkbox = new QCheckBox(airfoilChoseWidget);
        QPushButton *button1 = new QPushButton(airfoilChoseWidget);
        QLabel *label = new QLabel(airfoilChoseWidget);
        QPushButton *button2 = new QPushButton(airfoilChoseWidget);


        QString colorStyle = QString("background-color: %1;").arg(colorArray[i].name());

        button1->setStyleSheet(colorStyle);
        button2->setText("翼型");

        checkbox->setProperty("checkNum",i);
        button1->setProperty("colorNum",i);
        label->setProperty("labelNum",i);
        button2->setProperty("choseAirfoilNum",i);
        button1->setFixedSize(25,25);





        gridLayout->addWidget(checkbox,i,0,1,1);
        gridLayout->addWidget(button1,i,1,1,1);
        gridLayout->addWidget(label,i,2,1,7);
        gridLayout->addWidget(button2,i,10,1,1);

        airfoilCheckBox.append(checkbox);
        colorButton.append(button1);
        mesageAirfoilLabel.append(label);
        addAirfoilButton.append(button2);
        QObject::connect(button1,SIGNAL(clicked()),this,SLOT(showColorDialog()));
        QObject::connect(button2,SIGNAL(clicked()),this,SLOT(showAirfoilLibary()));



    }

    alphaLabel = new QLabel(airfoilChoseWidget);
    alphaCrLabel = new QLabel(airfoilChoseWidget);
    minAlphaEdit = new QLineEdit(airfoilChoseWidget);
    maxAlphaEdit = new QLineEdit(airfoilChoseWidget);
    stepLabel = new QLabel(airfoilChoseWidget);
    stepEdit = new QLineEdit(airfoilChoseWidget);
    startAnalyseButton = new QPushButton(airfoilChoseWidget);
    cancelAnalyseButton = new QPushButton(airfoilChoseWidget);

    minAlphaEdit->setText("0");
    maxAlphaEdit->setText("10");
    stepEdit->setText("1");





    alphaLabel->setText("迎角:");
    alphaCrLabel->setText("-");
    stepLabel->setText("步进");
    startAnalyseButton->setText("分析");
    cancelAnalyseButton->setText("取消");

    gridLayout->addWidget(alphaLabel,10,4,1,1);
    gridLayout->addWidget(minAlphaEdit,10,5,1,1);
    gridLayout->addWidget(alphaCrLabel,10,6,1,1);
    gridLayout->addWidget(maxAlphaEdit,10,7,1,1);
    gridLayout->addWidget(stepLabel,10,8,1,1);
    gridLayout->addWidget(stepEdit,10,9,1,1);
    gridLayout->addWidget(startAnalyseButton,11,5,1,2);
    gridLayout->addWidget(cancelAnalyseButton,11,8,1,2);

    QObject::connect(startAnalyseButton,SIGNAL(clicked()),this,SLOT(startAnalyse()));
    //QObject::connect(startAnalyseButton,SIGNAL(clicked()),this,SLOT(initialXfoilSetting()));



}

void airfoilAnalyse::initialAirfoilView(){


    // frameA = new QFrame();

    ReLabel = new QLabel(moreAirfoilAnalyseDialog);
    MaLabel = new QLabel(moreAirfoilAnalyseDialog);
    locationFlapLabel = new QLabel(moreAirfoilAnalyseDialog);
    flapAngleLabel = new QLabel(moreAirfoilAnalyseDialog);
    radiusLabel = new QLabel(moreAirfoilAnalyseDialog);
    trailingAngleLabel = new QLabel(moreAirfoilAnalyseDialog);
    ThicknessLabel = new QLabel(moreAirfoilAnalyseDialog);
    camberLabel = new QLabel(moreAirfoilAnalyseDialog);
    NriclLabel = new QLabel(moreAirfoilAnalyseDialog);
    xtrTopLabel = new QLabel(moreAirfoilAnalyseDialog);
    xtrBotLabel = new QLabel(moreAirfoilAnalyseDialog);
    listCombobox = new QComboBox(moreAirfoilAnalyseDialog);



    ReEdit = new QLineEdit(moreAirfoilAnalyseDialog);
    MaEdit = new QLineEdit(moreAirfoilAnalyseDialog);
    xtrTopEdit = new QLineEdit(moreAirfoilAnalyseDialog);
    xtrBotEdit = new QLineEdit(moreAirfoilAnalyseDialog);

    locationFlapBox = new QDoubleSpinBox(moreAirfoilAnalyseDialog);
    flapAngleBox = new QDoubleSpinBox(moreAirfoilAnalyseDialog);
    radiusBox= new QDoubleSpinBox(moreAirfoilAnalyseDialog);
    trailingAngleBox = new QDoubleSpinBox(moreAirfoilAnalyseDialog);
    thicknessBox = new QDoubleSpinBox(moreAirfoilAnalyseDialog);
    camberBox = new QDoubleSpinBox(moreAirfoilAnalyseDialog);


    NriclCombobox = new QComboBox(moreAirfoilAnalyseDialog);



    saveButton = new QPushButton(moreAirfoilAnalyseDialog);
    restoreButon = new QPushButton(moreAirfoilAnalyseDialog);
    cancelButton = new QPushButton(moreAirfoilAnalyseDialog);

    searhButton = new QPushButton(moreAirfoilAnalyseDialog);
    searhTextEdit = new QLineEdit(moreAirfoilAnalyseDialog);

    xaxis = new QValueAxis;
    yaxis = new QValueAxis;


    ReLabel->setText("雷诺数:");
    MaLabel->setText("马赫数:");
    locationFlapLabel->setText("襟翼位置:");
    flapAngleLabel->setText("襟翼角度:");
    radiusLabel->setText("前缘半径:");
    trailingAngleLabel->setText("后缘角度");
    ThicknessLabel->setText("厚度");
    camberLabel->setText("弯度");
    NriclLabel->setText("转捩数:");
    xtrTopLabel->setText("上表面转捩位置");
    xtrBotLabel->setText("下表面转捩位置");


    listCombobox->addItem("我的数据库");
    listCombobox->addItem("profili数据库");
    listCombobox->addItem("UIUC数据库");
    listCombobox->setCurrentIndex(1);

    xtrTopEdit->setText("1");
    xtrBotEdit->setText("1");


    flapAngleBox->setRange(-90,90);
    flapAngleBox->setSingleStep(1);
    flapAngleBox->setValue(0);

    locationFlapBox->setRange(0,1);
    locationFlapBox->setSingleStep(0.01);
    locationFlapBox->setValue(0.7);


    radiusBox->setRange(-0.1,0.1);
    radiusBox->setSingleStep(0.001);
    radiusBox->setValue(0);

    trailingAngleBox->setRange(-0.1,0.1);
    trailingAngleBox->setSingleStep(0.001);
    trailingAngleBox->setValue(0);

    thicknessBox->setRange(-0.1,0.1);
    thicknessBox->setSingleStep(0.001);
    thicknessBox->setValue(0);

    camberBox->setRange(-0.1,0.1);
    camberBox->setSingleStep(0.001);
    camberBox->setValue(0);





    ReEdit->setText("300000");
    MaEdit->setText("0.0");

    NriclCombobox->addItem("8->一般风洞");
    NriclCombobox->addItem("9->平均风洞");
    NriclCombobox->addItem("10->干净的风洞");
    NriclCombobox->addItem("11->很干净的风洞");
    NriclCombobox->setCurrentIndex(1);


    saveButton->setText("选择");
    restoreButon->setText("还原");
    cancelButton->setText("取消");

    searhButton->setText("按名称搜索");

    chartAirfoil = new QChart();
    viewAirfoil = new QChartView(chartAirfoil);
    airfoilSeries = new QLineSeries();
    listViewA = new QListView(moreAirfoilAnalyseDialog);
    listViewA->setEditTriggers(QAbstractItemView::NoEditTriggers);
    viewAirfoil->setFixedSize(900,450);
    //viewAirfoil->resize(800,400);
    listViewA->setFixedHeight(300);

    gridLayoutA->addWidget(listCombobox,0,0,1,3);
    gridLayoutA->addWidget(listViewA,1,0,4,3);

    gridLayoutA->addWidget(ReLabel,0,3,1,1);
    gridLayoutA->addWidget(ReEdit,0,4,1,1);
    gridLayoutA->addWidget(MaLabel,0,5,1,1);
    gridLayoutA->addWidget(MaEdit,0,6,1,1);
    gridLayoutA->addWidget(NriclLabel,1,3,1,1);
    gridLayoutA->addWidget(NriclCombobox,1,4,1,1);
    gridLayoutA->addWidget(xtrTopLabel,2,3,1,1);
    gridLayoutA->addWidget(xtrTopEdit,2,4,1,1);
    gridLayoutA->addWidget(xtrBotLabel,2,5,1,1);
    gridLayoutA->addWidget(xtrBotEdit,2,6,1,1);
    gridLayoutA->addWidget(locationFlapLabel,3,3,1,1);
    gridLayoutA->addWidget(locationFlapBox,3,4,1,1);
    gridLayoutA->addWidget(flapAngleLabel,3,5,1,1);
    gridLayoutA->addWidget(flapAngleBox,3,6,1,1);
    gridLayoutA->addWidget(radiusLabel,4,3,1,1);
    gridLayoutA->addWidget(radiusBox,4,4,1,1);
    gridLayoutA->addWidget(trailingAngleLabel,4,5,1,1);
    gridLayoutA->addWidget(trailingAngleBox,4,6,1,1);
    gridLayoutA->addWidget(ThicknessLabel,5,3,1,1);
    gridLayoutA->addWidget(thicknessBox,5,4,1,1);
    gridLayoutA->addWidget(camberLabel,5,5,1,1);
    gridLayoutA->addWidget(camberBox,5,6,1,1);
    gridLayoutA->addWidget(viewAirfoil,6,0,4,7);

    gridLayoutA->addWidget(saveButton,10,1,1,1);
    gridLayoutA->addWidget(restoreButon,10,3,1,1);
    gridLayoutA->addWidget(cancelButton,10,5,1,1);

    gridLayoutA->addWidget(searhButton,5,0,1,1);
    gridLayoutA->addWidget(searhTextEdit,5,1,1,2);


    // viewAirfoil->setRubberBand(QChartView::RectangleRubberBand);
    // viewAirfoil->setRubberBandSelectionMode(Qt::IntersectsItemBoundingRect);
    xaxis->setRange(0,1);
    yaxis->setRange(-0.25,0.25);
    chartAirfoil->addAxis(xaxis,Qt::AlignBottom);
    chartAirfoil->addAxis(yaxis,Qt::AlignLeft);
    chartAirfoil->addSeries(airfoilSeries);
    QPen pen(Qt::black);
    airfoilSeries->attachAxis(xaxis);
    airfoilSeries->attachAxis(yaxis);
    airfoilSeries->setPen(pen);
    chartAirfoil->legend()->setVisible(false);
    xaxis->hide();
    yaxis->hide();


    QObject::connect(radiusBox,SIGNAL(valueChanged(double)),this,SLOT(changeRadius(double)));
    QObject::connect(trailingAngleBox,SIGNAL(valueChanged(double)),this,SLOT(changeTrailingAngle(double)));
    QObject::connect(thicknessBox,SIGNAL(valueChanged(double)),this,SLOT(changeThickness(double)));
    QObject::connect(camberBox,SIGNAL(valueChanged(double)),this,SLOT(changeCamber(double)));
    QObject::connect(flapAngleBox,SIGNAL(valueChanged(double)),this,SLOT(changeFlap()));
    QObject::connect(locationFlapBox,SIGNAL(valueChanged(double)),this,SLOT(changeFlap()));
    QObject::connect(saveButton,SIGNAL(clicked()),this,SLOT(addAirfoil()));
    QObject::connect(listCombobox, QOverload<int>::of(&QComboBox::activated), this, &airfoilAnalyse::changeListModel);
    //QObject::connect(listViewA->selectionModel(),&QItemSelectionModel::selectionChanged,this,&airfoilAnalyse::drawAirfoil);





}
void airfoilAnalyse::initialAirfoilLibaryB(const QVector<AirfoilParameters>& libary,const int len,const QVector<QVector<double>>*array){
    AirfoilB = libary;
    QVector<QString>name;
    listLength = len;
    for(int i = 0;i<len;i++){
        name.append(AirfoilB[i].Name);
        airfoilArrayB[i] = array[i];
    }


    QStringList stringList = QStringList::fromVector(name);
    StringListModelB = new QStringListModel(stringList);
    listViewA->setModel(StringListModelB);
    QObject::connect(listViewA->selectionModel(),&QItemSelectionModel::selectionChanged,this,&airfoilAnalyse::drawAirfoil);

}
void airfoilAnalyse::initialAirfoilLibaryC(const QVector<AirfoilParameters>& libary,const int len,const QVector<QVector<double>>*array){
    AirfoilC = libary;
        QVector<QString>name;
        listLength = len;
        for(int i = 0;i<len;i++){
            name.append(AirfoilC[i].Name);
            airfoilArrayC[i] = array[i];
        }


        QStringList stringList = QStringList::fromVector(name);
        StringListModelC = new QStringListModel(stringList);
        listViewA->setModel(StringListModelC);
        QObject::connect(listViewA->selectionModel(),&QItemSelectionModel::selectionChanged,this,&airfoilAnalyse::drawAirfoil);
}
void airfoilAnalyse::initialSpinBox(){


    for(int i = 0;i<6;i++)
        simpleHicksHenneValue.append(0);

}
void airfoilAnalyse::initialResultUI(){


    reViewButton = new QPushButton(resultWidget);
    alphaViewButton = new QPushButton(resultWidget);
    listviewB = new QListView(resultWidget);
    listviewC = new QListView(resultWidget);
    allLabelA = new QLabel(resultWidget);
    allLabelB = new QLabel(resultWidget);
    allCheckBoxA = new QCheckBox(resultWidget);
    allCheckBoxB = new QCheckBox(resultWidget);

    reynoldListModelB = new QStringListModel();
    reynoldListModelC = new QStringListModel();

    chartANameLabel = new QLabel(resultWidget);
    chartBNameLabel = new QLabel(resultWidget);
    vboxLayout = new QVBoxLayout(resultWidget);
    comboboxALayout = new QHBoxLayout();
    comboboxBLayout = new QHBoxLayout();
    chartLayout = new QHBoxLayout();
    gListViewLayout = new QGridLayout();


    chartAXCombobox = new QComboBox(resultWidget);
    chartAYCombobox = new QComboBox(resultWidget);
    chartBXCombobox = new QComboBox(resultWidget);
    chartBYCombobox = new QComboBox(resultWidget);

    saveResultButton = new QPushButton("保存数据",resultWidget);

    textLabel = new QLabel(resultWidget);


    listviewB->setSelectionMode(QAbstractItemView::MultiSelection);
    listviewB->setEditTriggers(QAbstractItemView::NoEditTriggers);
    listviewC->setSelectionMode(QAbstractItemView::MultiSelection);
    listviewC->setEditTriggers(QAbstractItemView::NoEditTriggers);


    axisNameA[0] = "Alpha";
    axisNameA[1] = "Cl";
    axisNameA[2] = "Cd";
    axisNameA[3] = "Cl/Cd";
    axisNameA[4] = "Cl^1.5/Cd";
    axisNameA[5] = "Cm";
    axisNameA[6] = "极曲线";

    for(int i = 0; i < 6;i++){
        chartAXCombobox->addItem(axisNameA[i]);
        chartAYCombobox->addItem(axisNameA[i]);
        chartBXCombobox->addItem(axisNameA[i]);
        chartBYCombobox->addItem(axisNameA[i]);
    }
    reViewButton->setText("雷诺数视图");
    alphaViewButton->setText("普通视图");

    allLabelA->setText("全选");
    allLabelB->setText("全选");
    allCheckBoxA->setChecked(true);
    allCheckBoxB->setChecked(true);
    chartAXCombobox->addItem(axisNameA[6]);
    chartBXCombobox->addItem(axisNameA[6]);



    chartAXCombobox->setCurrentIndex(0);
    chartAYCombobox->setCurrentIndex(1);

    chartBXCombobox->setCurrentIndex(2);
    chartBYCombobox->setCurrentIndex(1);



    comboboxALayout->addWidget(chartAXCombobox);
    comboboxALayout->addWidget(reViewButton);
    comboboxALayout->addWidget(chartBXCombobox);

    comboboxBLayout->addWidget(chartAYCombobox);
    comboboxBLayout->addWidget(alphaViewButton);
    comboboxBLayout->addWidget(chartBYCombobox);

    vboxLayout->addLayout(comboboxALayout);
    vboxLayout->addLayout(comboboxBLayout);

    gListViewLayout->addWidget(allLabelA,0,0,1,1);
    gListViewLayout->addWidget(allCheckBoxA,0,1,1,1);
    gListViewLayout->addWidget(listviewB,1,0,2,2);
    gListViewLayout->addWidget(allLabelB,3,0,1,1);
    gListViewLayout->addWidget(allCheckBoxB,3,1,1,1);
    gListViewLayout->addWidget(listviewC,4,0,2,2);
    gListViewLayout->addWidget(saveResultButton,6,0,1,1);



    chartLayout->addWidget(chartViewRA);
    chartLayout->addLayout(gListViewLayout);
    chartLayout->addWidget(chartViewRB);

    vboxLayout->addLayout(chartLayout);
    vboxLayout->addWidget(textLabel);



    resultWidget->hide();



    // 连接信号和槽
    connect(chartAXCombobox, QOverload<int>::of(&QComboBox::activated), this, &airfoilAnalyse::changeChartA);
    connect(chartAYCombobox, QOverload<int>::of(&QComboBox::activated), this, &airfoilAnalyse::changeChartA);

    connect(chartBXCombobox, QOverload<int>::of(&QComboBox::activated), this, &airfoilAnalyse::changeChartB);
    connect(chartBYCombobox, QOverload<int>::of(&QComboBox::activated), this, &airfoilAnalyse::changeChartB);


    connect(reViewButton,&QPushButton::clicked,this,&airfoilAnalyse::changeUIForReView);
    connect(alphaViewButton,&QPushButton::clicked,this,&airfoilAnalyse::changeUIForAlphaView);
    connect(saveResultButton,&QPushButton::clicked,this,&airfoilAnalyse::exportChartData);




}
void airfoilAnalyse::initialXfoilSetting(){
    if(!airfoilIndexB.isEmpty())
        airfoilIndexB.clear();
    double mina = minAlphaEdit->text().toDouble();
    double maxa = maxAlphaEdit->text().toDouble();
    double stepa = stepEdit->text().toDouble();
    double tmpXtrTop = xtrTopEdit->text().toDouble();
    double tmpXtrBot = xtrBotEdit->text().toDouble();



    for(int i = 0;i<airfoilIndexA.length();i++){
        if(airfoilCheckBox[airfoilIndexA[i]]->isChecked()){
            airfoilIndexB.append(airfoilIndexA[i]);
            setting[airfoilIndexA[i]].minAlpha = mina;
            setting[airfoilIndexA[i]].maxAlpha = maxa;
            setting[airfoilIndexA[i]].alphaStepSize = stepa;
            setting[airfoilIndexA[i]].s_IterLim = 150;
            setting[airfoilIndexA[i]].xtrTop = tmpXtrTop;
            setting[airfoilIndexA[i]].xtrBot = tmpXtrBot;
        }
    }





}
void airfoilAnalyse::initialChartRA(){
    chartRA = new QChart();
    chartViewRA = new MyChartView(chartRA);
    chartViewRA->setMinimumSize(600,600);



    rxAxisA = new QValueAxis();
    ryAxisA = new QValueAxis();
    rxAxisA->setRange(MINAXISR[0],MAXAXISR[0]);
    ryAxisA->setRange(MINAXISR[1],MAXAXISR[1]);
    rxAxisA->setTickCount(int(MINAXISR[7]));
    ryAxisA->setTickCount(int(MAXAXISR[7]));

    chartRA->addAxis(rxAxisA,Qt::AlignBottom);
    chartRA->addAxis(ryAxisA,Qt::AlignLeft);



    connect(chartViewRA,&MyChartView::mousePositionChanged,this,&airfoilAnalyse::updateMousePositionA);

}
void airfoilAnalyse::initialChartRB(){
    chartRB = new QChart();
    chartViewRB = new MyChartView(chartRB);
    chartViewRB->setMinimumSize(600,600);

    rxAxisB = new QValueAxis();
    ryAxisB = new QValueAxis();
    rxAxisB->setRange(MINAXISR[2],MAXAXISR[2]);
    ryAxisB->setRange(MINAXISR[1],MAXAXISR[1]);
    rxAxisB->setTickCount(int(MINAXISR[7]));
    ryAxisB->setTickCount(int(MAXAXISR[7]));

    chartRB->addAxis(rxAxisB,Qt::AlignBottom);
    chartRB->addAxis(ryAxisB,Qt::AlignLeft);
    connect(chartViewRB,&MyChartView::mousePositionChanged,this,&airfoilAnalyse::updateMousePositionB);

}
void airfoilAnalyse::showAirfoilLibary(){

    moreAirfoilAnalyseDialog->show();
    if(airfoilChoseWidget->isVisible()){
    QPushButton *button = qobject_cast<QPushButton*>(QObject::sender());
    choseAirfoilIndex = button->property("choseAirfoilNum").toInt();
    }
}
void airfoilAnalyse::findData(const QVector<QVector<double>>&result,const int index){
    int index1 = result.length();


    QVector<double>ALL;
    ALL.append(double(index));

    double tmp = result[0][1];
    for(int i = 1;i<index1;i++){
        if(result[i][1] > tmp)
            tmp = result[i][1];
    }
    ALL.append(tmp);

    double tmp3 = result[0][3];
    int cmindex = 0;
    for(int i = 1;i<index1;i++){
        if(result[i][3] > tmp3){
            tmp3 = result[i][3];
            cmindex++;
        }
    }
    ALL.append(tmp3);


    ALL.append(result[cmindex][5]);
    all.append(ALL);




}

void airfoilAnalyse::startAllAnalyse(){

    // for(int i = 0;i<40;i++){






    // }




    initialXfoilSetting();
    for(int i = 0;i<70;i++){
        startXfoil(i);

    }






}
void airfoilAnalyse::startXfoil(const int n){

    QVector<QFuture <void>>futures;  //线程监视
    QFutureSynchronizer<void> sync;
    QVector<airfoilSolve*>solverPools;


    for(int i = 0;i < threadNum;i++){
        int index = threadNum * n + i;
        airfoilSolve* solvers = new airfoilSolve();
        solverPools.append(solvers);

        solvers->refreshParaments(setting[airfoilIndexB[0]]);

        solvers->importAirfoil(airfoilArrayB[index]);

        QFuture<void>future = QtConcurrent::run([solvers](){
            solvers->solverInThread();
        });
        futures.append(future);

    }


    for (auto &f : futures) {
        sync.addFuture(f);
    }

    sync.waitForFinished();   // ✅ 等“所有线程”一起完成


    for(int i = 0; i < threadNum;i++){
        findData(solverPools[i]->resultData,threadNum * n + i);
        //airfoilResultData.append(solverPools[i]->resultData);

    }

    int i = 0;
    QString fileName = QDir::currentPath() + "/libaries/airfoil/result.txt";
    QFile file(fileName);
    if(file.open(QIODevice::Append|QIODevice::Text)){
        QTextStream stream(&file);

        for(const QVector<double>&row : all){
            stream<<AirfoilB[threadNum * n + i].Name<<"    ";
            for(const double da : row){
                stream<<da<<"    ";
            }
            stream<<"\n";
            i++;
        }

    }

    file.close();

    all.clear();
    qDeleteAll(solverPools);
    solverPools.clear();
    solverPools.squeeze();

}


void airfoilAnalyse::startAnalyse()
{
    airfoilChoseWidget->close();
    viewModel = 0;
    initialXfoilSetting();

    airfoilResultData.clear();

    const int totalTasks = airfoilIndexB.length();
    int completedTasks = 0;

    // ======== 关键：把 solverPools 放到共享指针，保证异步安全 ========
    auto solverPools = std::make_shared<QVector<airfoilSolve*>>();
    solverPools->reserve(totalTasks);
    // 创建一个临时同步器，只用来等待所有 watcher 结束
    QFutureSynchronizer<void> sync;

    // ======== 关键：为每个任务创建一个 watcher ========
    QVector<QFutureWatcher<void>*> watchers;
    watchers.reserve(totalTasks);

    // ================== 启动所有线程 ==================
    for (int i = 0; i < totalTasks; i++) {

        airfoilSolve* solvers = new airfoilSolve();
        solverPools->append(solvers);

        solvers->refreshParaments(setting[airfoilIndexB[i]]);
        solvers->importAirfoil(airfoilArray[airfoilIndexB[i]]);

        // 创建 watcher
        auto *watcher = new QFutureWatcher<void>(this);
        watchers.append(watcher);

        // ---- 每完成一个任务就更新进度 ----
        connect(watcher, &QFutureWatcher<void>::finished, this,
                [this, totalTasks, &completedTasks]() {

            completedTasks++;
            emit progressUpdated(
                (completedTasks * 100) / totalTasks
            );
        });

        // 启动线程
        QFuture<void> future =
            QtConcurrent::run([solvers]() {
                solvers->solverInThread();
            });

        watcher->setFuture(future);
    }

    // ============ 等“全部完成”再做后处理 ==============



    for (auto *w : watchers) {
        sync.addFuture(w->future());
    }

    // 不占 CPU、不卡 UI 的等待
    sync.waitForFinished();

    // 所有线程到这里“必然已经完成”
    emit progressUpdated(100);

    // ============ 汇总结果（你的原逻辑） =============
    for (int i = 0; i < totalTasks; i++) {
        airfoilResultData.append(
            solverPools->at(i)->resultData
        );
    }

    // ============ 清理内存 =============
    qDeleteAll(*solverPools);
    solverPools->clear();

    // 释放 watchers
    qDeleteAll(watchers);
    watchers.clear();

    // ============ 你的 UI 逻辑（保持原样） ============
    allLabelA->hide();
    allLabelB->hide();
    allCheckBoxA->hide();
    allCheckBoxB->hide();
    listviewB->hide();
    listviewC->hide();
    alphaViewButton->hide();
    reViewButton->hide();

    resultWidget->show();
    drawResult();
}


void airfoilAnalyse::addAirfoil(){
    int re = ReEdit->text().toInt();
    double ma = MaEdit->text().toDouble();
    int ncrit = NriclCombobox->currentIndex();

    if(airfoilArray[choseAirfoilIndex].isEmpty())
        airfoilIndexA.append(choseAirfoilIndex);


    airfoilDesign design(cstNUM);

    double axis = locationFlapBox->value();
    design.buildBenrnstein(realAirfoil);
    design.flapAngle = flapAngleBox->value();
    design.rotationAxis = axis;
    design.deflectedAirfoil(realAirfoil);
    design.computeSimpleParameters(realAirfoil);

    airfoilArray[choseAirfoilIndex] = design.flapAirfoilData;
    nameArray[choseAirfoilIndex] = realName;
    setting[choseAirfoilIndex].Re = re;
    setting[choseAirfoilIndex].Ma = ma;
    setting[choseAirfoilIndex].nCrit = ncrit + 8;

    setting[choseAirfoilIndex].MaType = 1;
    setting[choseAirfoilIndex].ReType = 1;

    QString charRe = "                     " + nameArray[choseAirfoilIndex] + " " + QString("雷诺数: %1  马赫数: %2   襟翼角度：%3   前缘半径：%4%   尾缘角度：%5   最大厚度：%6%"
                                                               "   最大弯度：%7%  ").arg(double(re),0,'f',0).arg(ma,0,'f',2).arg(design.flapAngle,0,'f',1)
            .arg(design.minRadius * 100,0,'f',2).arg(design.trailingAngle,0,'f',1).arg(design.maxThickness * 100,0,'f',1).arg(design.maxCamber * 100,0,'f',1);

    mesageAirfoilLabel[choseAirfoilIndex]->setText(charRe);





    moreAirfoilAnalyseDialog->close();


}
void airfoilAnalyse::drawAirfoil(const QItemSelection &selected,const QItemSelection & /*deselected*/){

    flapAngleBox->setValue(0);
    locationFlapBox->setValue(0.7);
    radiusBox->setValue(0);
    thicknessBox->setValue(0);
    trailingAngleBox->setValue(0);
    simpleHicksHenneValue.clear();
    simpleHicksHenneValue.resize(6);
    if(airfoilSeries->count() != 0){              //判断series是否被定义
        airfoilSeries->clear();
    }
    QString tmp1,tmp2,tmp3,pointFstring;
    tmp1 = " ";
    tmp2 = " ";
    tmp3 = " ";
    airfoilDesign design(cstNUM);






    QModelIndexList indexes = selected.indexes();

    int index = indexes.at(0).row();
    int index1 = listCombobox->currentIndex();
    listChoseB = index;


    switch (index1) {
    case 0:
        realAirfoil = airfoilArrayA[index];
        changeAirfoilData = realAirfoil;
        realName = nameArrayA[index];
        for(int i = 0; i < airfoilArrayA[index].length(); i++){

            airfoilSeries->append(airfoilArrayA[index][i][0],airfoilArrayA[index][i][1]);
        }
        design.computeSimpleParameters(airfoilArrayA[index]);
        tmp1 = QString("最大厚度: %1%  最大厚度位置: %2%  ").arg(design.maxThickness * 100,0,'f',1).arg(design.locationThickness * 100,0,'f',1);
        tmp2 = QString("最大弯度: %1%  最大弯度位置: %2%  ").arg(design.maxCamber * 100,0,'f',1).arg(design.locationCamber * 100,0,'f',1);
        tmp3 = QString("前缘半径: %1%  尾缘夹角: %2").arg(design.minRadius * 100,0,'f',2).arg(design.trailingAngle,0,'f',2);

        pointFstring = tmp1 + tmp2 + tmp3;
        mouseLocitionAnalyse->setText(pointFstring);
        break;
    case 1:
        realAirfoil = airfoilArrayB[index];
        changeAirfoilData = realAirfoil;
        realName = AirfoilB[index].Name;
        for(int i = 0; i < airfoilArrayB[index].length(); i++){

            airfoilSeries->append(airfoilArrayB[index][i][0],airfoilArrayB[index][i][1]);
        }

        design.computeSimpleParameters(airfoilArrayB[index]);
        tmp1 = QString("最大厚度: %1%  最大厚度位置: %2%  ").arg(design.maxThickness * 100,0,'f',1).arg(design.locationThickness * 100,0,'f',1);
        tmp2 = QString("最大弯度: %1%  最大弯度位置: %2%  ").arg(design.maxCamber * 100,0,'f',1).arg(design.locationCamber * 100,0,'f',1);
        tmp3 = QString("前缘半径: %1%  尾缘夹角: %2").arg(design.minRadius * 100,0,'f',2).arg(design.trailingAngle,0,'f',2);
        pointFstring = tmp1 + tmp2 + tmp3;
        mouseLocitionAnalyse->setText(pointFstring);

        break;
    case 2:
        realAirfoil = airfoilArrayC[index];
        changeAirfoilData = realAirfoil;
        realName = AirfoilC[index].Name;
        for(int i = 0; i < airfoilArrayC[index].length(); i++){

            airfoilSeries->append(airfoilArrayC[index][i][0],airfoilArrayC[index][i][1]);
        }

        design.computeSimpleParameters(airfoilArrayC[index]);
        tmp1 = QString("最大厚度: %1%  最大厚度位置: %2%  ").arg(design.maxThickness * 100,0,'f',1).arg(design.locationThickness * 100,0,'f',1);
        tmp2 = QString("最大弯度: %1%  最大弯度位置: %2%  ").arg(design.maxCamber * 100,0,'f',1).arg(design.locationCamber * 100,0,'f',1);
        tmp3 = QString("前缘半径: %1%  尾缘夹角: %2").arg(design.minRadius * 100,0,'f',2).arg(design.trailingAngle,0,'f',2);
        pointFstring = tmp1 + tmp2 + tmp3;
        mouseLocitionAnalyse->setText(pointFstring);



        break;
    default:
        break;
    }


    // QString tmp1 = QString("最大厚度: %1%     最大厚度位置: %2%    ").arg(AirfoilB[index].MaxThickness * 100,0,'f',1).arg(AirfoilB[index].ThicknessLoaction * 100,0,'f',1);
    // QString tmp2 = QString("最大弯度: %1%     最大弯度位置: %2%    ").arg(AirfoilB[index].MaxCamber * 100,0,'f',1).arg(AirfoilB[index].CamberLoaction * 100,0,'f',1);
    // QString tmp3 = QString("前缘半径: %1%     尾缘夹角: %2").arg(AirfoilB[index].minRadius * 100,0,'f',2).arg(AirfoilB[index].trailingAngle,0,'f',2);
    // QString pointFstring = tmp1 + tmp2 + tmp3;
    // mouseLocitionAnalyse->setText(pointFstring);


    //design.buildHicksHenne();










}
void airfoilAnalyse::drawResult(){
    if(!seriesArrayA.isEmpty()){
        for(QLineSeries *series:seriesArrayA)
            delete series;
        seriesArrayA.clear();
    }
    if(!seriesArrayB.isEmpty()){
        for(QLineSeries *series:seriesArrayB)
            delete series;
        seriesArrayB.clear();
    }



    typeAX = chartAXCombobox->currentIndex();
    typeAY = chartAYCombobox->currentIndex();

    typeBX = chartBXCombobox->currentIndex();
    typeBY = chartBYCombobox->currentIndex();




    for(int i = 0; i < airfoilResultData.length();i++){
        QLineSeries *rlineA = new QLineSeries();
        QLineSeries *rlineB = new QLineSeries();
        int len = airfoilResultData[i].length();
        for(int j = 0;j< len ;j++){
            rlineA->append(airfoilResultData[i][j][typeAX],airfoilResultData[i][j][typeAY]);

        }
        rlineA->setName(nameArray[airfoilIndexB[i]]);

        for(int j = 0;j< len ;j++){
            rlineB->append(airfoilResultData[i][j][typeBX],airfoilResultData[i][j][typeBY]);

        }

        rlineB->setName(nameArray[airfoilIndexB[i]]);
        QPen pen(colorArray[airfoilIndexB[i]]);
        pen.setWidth(2);
        rlineA->setPen(pen);
        rlineB->setPen(pen);
        chartRA->addSeries(rlineA);
        chartRB->addSeries(rlineB);
        rlineA->attachAxis(rxAxisA);
        rlineA->attachAxis(ryAxisA);
        rlineB->attachAxis(rxAxisB);
        rlineB->attachAxis(ryAxisB);





        seriesArrayA.append(rlineA);
        seriesArrayB.append(rlineB);
        chartRA->legend()->setVisible(true);
        chartRB->legend()->setVisible(true);

    }
    updateAxes(chartRA,seriesArrayA);
    updateAxes(chartRB,seriesArrayB);
}
void airfoilAnalyse::drawReResult(){

    if(!seriesArrayA.isEmpty()){
        for(QLineSeries *series:seriesArrayA)
            delete series;
        seriesArrayA.clear();
    }
    if(!seriesArrayB.isEmpty()){
        for(QLineSeries *series:seriesArrayB)
            delete series;
        seriesArrayB.clear();
    }



    typeAX = chartAXCombobox->currentIndex();
    typeAY = chartAYCombobox->currentIndex();

    typeBX = chartBXCombobox->currentIndex();
    typeBY = chartBYCombobox->currentIndex();


    rxAxisA->setRange(MINAXISR[0],MAXAXISR[0]);
    rxAxisB->setRange(MINAXISR[0],MAXAXISR[0]);


    for(int i = 0; i < airfoilResultData.length();i++){
        QLineSeries *rlineA = new QLineSeries();
        QLineSeries *rlineB = new QLineSeries();
        int len = airfoilResultData[i].length();
        for(int j = 0;j< len ;j++){
            rlineA->append(airfoilResultData[i][j][typeAX],airfoilResultData[i][j][typeAY]);

        }
        //rlineA->setName(nameArray[airfoilIndexB[i]]);

        for(int j = 0;j< len ;j++){
            rlineB->append(airfoilResultData[i][j][typeBX],airfoilResultData[i][j][typeBY]);

        }

        chartRA->addSeries(rlineA);
        chartRB->addSeries(rlineB);
        rlineA->attachAxis(rxAxisA);
        rlineA->attachAxis(ryAxisA);
        rlineB->attachAxis(rxAxisB);
        rlineB->attachAxis(ryAxisB);





        seriesArrayA.append(rlineA);
        seriesArrayB.append(rlineB);




    }
    chartRA->legend()->setVisible(false);
    chartRB->legend()->setVisible(false);




}

void airfoilAnalyse::drawAlphaResult(){

    if(!seriesArrayA.isEmpty()){
        for(QLineSeries *series:seriesArrayA)
            delete series;
        seriesArrayA.clear();
    }
    if(!seriesArrayB.isEmpty()){
        for(QLineSeries *series:seriesArrayB)
            delete series;
        seriesArrayB.clear();
    }





    typeAX = chartAXCombobox->currentIndex();
    typeAY = chartAYCombobox->currentIndex();

    typeBX = chartBXCombobox->currentIndex();
    typeBY = chartBYCombobox->currentIndex();






    int len = airfoilResultData.length();


    rxAxisA->setRange(reArray[0],reArray[len - 1]);
    rxAxisB->setRange(reArray[0],reArray[len - 1]);


    QVector<QVector<double>>alphaA;
    for(int i = 0;i < len;i++){
        QVector<double>alphaTmp;
        for(int j = 0; j<airfoilResultData[i].length();j++){
            alphaTmp.append(airfoilResultData[i][j][0]);
        }
        alphaA.append(alphaTmp);

    }
    realAlphaArray = alphaA;





    for(int i = 0;i<alphaArray.length();i++){
        QLineSeries *rlineA = new QLineSeries();
        QLineSeries *rlineB = new QLineSeries();
        for(int j = 0;j<len;j++){
            int index = findAlphaIndex(alphaArray[i],alphaA[j]);
            if(index != -1)
                rlineA->append(reArray[j],airfoilResultData[j][index][typeAY]);

        }

        for(int j = 0;j<len;j++){
            int index = findAlphaIndex(alphaArray[i],alphaA[j]);
            if(index != -1)
                rlineB->append(reArray[j],airfoilResultData[j][index][typeBY]);

        }


        chartRA->addSeries(rlineA);
        chartRB->addSeries(rlineB);

        rlineA->attachAxis(rxAxisA);
        rlineA->attachAxis(ryAxisA);
        rlineB->attachAxis(rxAxisB);
        rlineB->attachAxis(ryAxisB);






        seriesArrayA.append(rlineA);
        seriesArrayB.append(rlineB);
    }
    chartRA->legend()->setVisible(false);
    chartRB->legend()->setVisible(false);







}
int airfoilAnalyse::findAlphaIndex(const int index,const QVector<double>&tmp){

    int a = -1;
        for(int i = 0;i<tmp.length();i++){
        if(abs(tmp[i] - alphaArray[index]) < 0.001){
                a = i;
                break;
            }
        }

        return a;
}
void airfoilAnalyse::changeAirfoil(double value){
    if(airfoilSeries->count() != 0){              //判断series是否被定义
        airfoilSeries->clear();
    }
    QDoubleSpinBox *spinbox = qobject_cast<QDoubleSpinBox*>(QObject::sender());
    int index = spinbox->property("nameId").toInt();
    airfoilDesign design(cstNUM);
    design.splitAirfoilData(airfoilArrayB[listChoseB]);
    hicksHenneValue[index] = value;
    hicksHenneValue[cstNUM] = value;
    design.buildHicksHenne(hicksHenneValue);

    for(int i = 0; i < design.newAirfoilData.length(); i++){

        airfoilSeries->append(design.newAirfoilData[i][0],design.newAirfoilData[i][1]);

    }
    design.computeSimpleParameters(design.newAirfoilData);


    QString tmp1 = QString("最大厚度: %1%   最大厚度位置: %2%    ").arg(design.maxThickness * 100,0,'f',1).arg(design.locationThickness * 100,0,'f',1);
    QString tmp2 = QString("最大弯度: %1%   最大弯度位置: %2%    ").arg(design.maxCamber * 100,0,'f',1).arg(design.locationCamber * 100,0,'f',1);
    QString tmp3 = QString("前缘半径: %1%   尾缘夹角: %2").arg(design.minRadius * 100,0,'f',2).arg(design.trailingAngle,0,'f',2);
    QString pointFstring = tmp1 + tmp2 + tmp3;
    mouseLocitionAnalyse->setText(pointFstring);



}

void airfoilAnalyse::changeThickness(double value){
    if(airfoilSeries->count() != 0){              //判断series是否被定义
        airfoilSeries->clear();
    }
    double s;
    s = camberBox->value();
    airfoilDesign design(cstNUM);
    design.splitAirfoilData(changeAirfoilData);
    simpleHicksHenneValue[1] = value + s;
    simpleHicksHenneValue[4] = value - s ;
    design.buildHicksHenne(simpleHicksHenneValue,AirfoilB[listChoseB].ThicknessLoaction);
    realAirfoil = design.newAirfoilData;

    for(int i = 0; i < design.newAirfoilData.length(); i++){

        airfoilSeries->append(design.newAirfoilData[i][0],design.newAirfoilData[i][1]);

    }
    design.computeSimpleParameters(design.newAirfoilData);

    QString tmp1 = QString("最大厚度: %1%   最大厚度位置: %2%    ").arg(design.maxThickness * 100,0,'f',1).arg(design.locationThickness * 100,0,'f',1);
    QString tmp2 = QString("最大弯度: %1%   最大弯度位置: %2%    ").arg(design.maxCamber * 100,0,'f',1).arg(design.locationCamber * 100,0,'f',1);
    QString tmp3 = QString("前缘半径: %1%   尾缘夹角: %2").arg(design.minRadius * 100,0,'f',2).arg(design.trailingAngle,0,'f',2);
    QString pointFstring = tmp1 + tmp2 + tmp3;
    mouseLocitionAnalyse->setText(pointFstring);


}


void airfoilAnalyse::changeRadius(double value){
    if(airfoilSeries->count() != 0){              //判断series是否被定义
        airfoilSeries->clear();
    }

    airfoilDesign design(cstNUM);
    design.splitAirfoilData(changeAirfoilData);
    simpleHicksHenneValue[0] = value;
    simpleHicksHenneValue[3] = value;
    design.buildHicksHenne(simpleHicksHenneValue,AirfoilB[listChoseB].ThicknessLoaction);
    realAirfoil = design.newAirfoilData;
    for(int i = 0; i < design.newAirfoilData.length(); i++){

        airfoilSeries->append(design.newAirfoilData[i][0],design.newAirfoilData[i][1]);

    }
    design.computeSimpleParameters(design.newAirfoilData);

    QString tmp1 = QString("最大厚度: %1%   最大厚度位置: %2%    ").arg(design.maxThickness * 100,0,'f',1).arg(design.locationThickness * 100,0,'f',1);
    QString tmp2 = QString("最大弯度: %1%   最大弯度位置: %2%    ").arg(design.maxCamber * 100,0,'f',1).arg(design.locationCamber * 100,0,'f',1);
    QString tmp3 = QString("前缘半径: %1%   尾缘夹角: %2").arg(design.minRadius * 100,0,'f',2).arg(design.trailingAngle,0,'f',2);
    QString pointFstring = tmp1 + tmp2 + tmp3;
    mouseLocitionAnalyse->setText(pointFstring);






}
void airfoilAnalyse::changeTrailingAngle(double value){
    if(airfoilSeries->count() != 0){              //判断series是否被定义
        airfoilSeries->clear();
    }

    airfoilDesign design(cstNUM);
    design.splitAirfoilData(changeAirfoilData);
    simpleHicksHenneValue[2] = value;
    simpleHicksHenneValue[5] = value;
    design.buildHicksHenne(simpleHicksHenneValue,AirfoilB[listChoseB].ThicknessLoaction);
    realAirfoil = design.newAirfoilData;
    for(int i = 0; i < design.newAirfoilData.length(); i++){

        airfoilSeries->append(design.newAirfoilData[i][0],design.newAirfoilData[i][1]);

    }
    design.computeSimpleParameters(design.newAirfoilData);

    QString tmp1 = QString("最大厚度: %1%   最大厚度位置: %2%    ").arg(design.maxThickness * 100,0,'f',1).arg(design.locationThickness * 100,0,'f',1);
    QString tmp2 = QString("最大弯度: %1%   最大弯度位置: %2%    ").arg(design.maxCamber * 100,0,'f',1).arg(design.locationCamber * 100,0,'f',1);
    QString tmp3 = QString("前缘半径: %1%   尾缘夹角: %2").arg(design.minRadius * 100,0,'f',2).arg(design.trailingAngle,0,'f',2);
    QString pointFstring = tmp1 + tmp2 + tmp3;
    mouseLocitionAnalyse->setText(pointFstring);



}
void airfoilAnalyse::changeCamber(double value){
    if(airfoilSeries->count() != 0){              //判断series是否被定义
        airfoilSeries->clear();
    }

    double s;
    s = thicknessBox->value();

    airfoilDesign design(cstNUM);
    design.splitAirfoilData(changeAirfoilData);

    simpleHicksHenneValue[1] = value + s;
    simpleHicksHenneValue[4] = -value + s;
    design.buildHicksHenne(simpleHicksHenneValue,AirfoilB[listChoseB].ThicknessLoaction);
    realAirfoil = design.newAirfoilData;
    for(int i = 0; i < design.newAirfoilData.length(); i++){

        airfoilSeries->append(design.newAirfoilData[i][0],design.newAirfoilData[i][1]);

    }
    design.computeSimpleParameters(design.newAirfoilData);

    QString tmp1 = QString("最大厚度: %1%   最大厚度位置: %2%    ").arg(design.maxThickness * 100,0,'f',1).arg(design.locationThickness * 100,0,'f',1);
    QString tmp2 = QString("最大弯度: %1%   最大弯度位置: %2%    ").arg(design.maxCamber * 100,0,'f',1).arg(design.locationCamber * 100,0,'f',1);
    QString tmp3 = QString("前缘半径: %1%   尾缘夹角: %2").arg(design.minRadius * 100,0,'f',2).arg(design.trailingAngle,0,'f',2);
    QString pointFstring = tmp1 + tmp2 + tmp3;
    mouseLocitionAnalyse->setText(pointFstring);



}
void airfoilAnalyse::changeFlap(){
    if(airfoilSeries->count() != 0){              //判断series是否被定义
        airfoilSeries->clear();
    }
    airfoilDesign design(cstNUM);

    double axis = locationFlapBox->value();
    if(axis <= 0)
        axis = 0.7;


    design.buildBenrnstein(realAirfoil);
    design.flapAngle = flapAngleBox->value();
    design.rotationAxis = axis;
    design.deflectedAirfoil(realAirfoil);

    for(int i = 0; i < design.flapAirfoilData.length(); i++){

        airfoilSeries->append(design.flapAirfoilData[i][0],design.flapAirfoilData[i][1]);

    }
    design.computeSimpleParameters(design.flapAirfoilData);

    QString tmp1 = QString("最大厚度: %1%     最大厚度位置: %2%    ").arg(design.maxThickness * 100,0,'f',1).arg(design.locationThickness * 100,0,'f',1);
    QString tmp2 = QString("最大弯度: %1%     最大弯度位置: %2%    ").arg(design.maxCamber * 100,0,'f',1).arg(design.locationCamber * 100,0,'f',1);
    QString tmp3 = QString("前缘半径: %1%     尾缘夹角: %2").arg(design.minRadius * 100,0,'f',2).arg(design.trailingAngle,0,'f',2);
    QString pointFstring = tmp1 + tmp2 + tmp3;
    mouseLocitionAnalyse->setText(pointFstring);

}
void airfoilAnalyse::changeButtonColor(const QColor &color){
    QString styleSheet = QString("background-color: %1;").arg(color.name());

    colorArray[choseButtonIndex] = color;
    colorButton[choseButtonIndex]->setStyleSheet(styleSheet);

}
void airfoilAnalyse::showColorDialog(){
    QPushButton *button = qobject_cast<QPushButton *>(sender());
    int index = button->property("colorNum").toInt();
    choseButtonIndex = index;

    if (button) {
        connect(colorDialog,&QColorDialog::colorSelected,this,&airfoilAnalyse::changeButtonColor);
        colorDialog->show();
    }
}

void airfoilAnalyse::changeChartA(){

    if(!airfoilResultData.isEmpty()){

            typeAX = chartAXCombobox->currentIndex();
            typeAY = chartAYCombobox->currentIndex();



        for (QLineSeries *seriesItem : seriesArrayA) {
            seriesItem->clear(); // 清除系列中的所有数据点
        }



        if(viewModel ==1||viewModel == 0){


            for(int i = 0; i < airfoilResultData.length();i++){

                int len = airfoilResultData[i].length();
                for(int j = 0;j< len ;j++){
                    seriesArrayA[i]->append(airfoilResultData[i][j][typeAX],airfoilResultData[i][j][typeAY]);
                }
            }


            rxAxisA->setRange(MINAXISR[typeAX],MAXAXISR[typeAX]);
            ryAxisA->setRange(MINAXISR[typeAY],MAXAXISR[typeAY]);
        }else{
            int len = airfoilResultData.length();




        for(int i = 0;i<alphaArray.length();i++){
            for(int j = 0;j<len;j++){
                int index = findAlphaIndex(alphaArray[i],realAlphaArray[j]);
                if(index != -1)
                    seriesArrayA[i]->append(reArray[j],airfoilResultData[j][index][typeAY]);

            }
        }
        rxAxisA->setRange(reArray[0],reArray[len - 1]);
        ryAxisA->setRange(MINAXISR[typeAY],MAXAXISR[typeAY]);








    }




}
}
void airfoilAnalyse::changeChartB(){
    if(!airfoilResultData.isEmpty()){


            typeBX = chartBXCombobox->currentIndex();
            typeBY = chartBYCombobox->currentIndex();



        for (QLineSeries *seriesItem : seriesArrayB) {
            seriesItem->clear(); // 清除系列中的所有数据点
        }





        if(viewModel ==1 || viewModel == 0){

            for(int i = 0; i < airfoilResultData.length();i++){

                int len = airfoilResultData[i].length();
                for(int j = 0;j< len ;j++){
                    seriesArrayB[i]->append(airfoilResultData[i][j][typeBX],airfoilResultData[i][j][typeBY]);
                }
            }


            rxAxisB->setRange(MINAXISR[typeBX],MAXAXISR[typeBX]);
            ryAxisB->setRange(MINAXISR[typeBY],MAXAXISR[typeBY]);
        }else{
            int len = airfoilResultData.length();


            for(int i = 0;i<alphaArray.length();i++){
                for(int j = 0;j<len;j++){
                    int index = findAlphaIndex(alphaArray[i],realAlphaArray[j]);
                    if(index != -1)
                        seriesArrayB[i]->append(reArray[j],airfoilResultData[j][index][typeBY]);

                }
            }


            rxAxisB->setRange(reArray[0],reArray[len - 1]);
            ryAxisB->setRange(MINAXISR[typeBY],MAXAXISR[typeBY]);
        }







}

}
void airfoilAnalyse::updateMousePositionA(const QPointF &localPoint){


    QString text = axisNameA[typeAX] + QString(": %1        ").arg(localPoint.x()) + axisNameA[typeAY] + QString(": %1").arg(localPoint.y());
    textLabel->setText(text);



}
void airfoilAnalyse::updateMousePositionB(const QPointF &localPoint){



    QString text = axisNameA[typeBX] + QString(": %1        ").arg(localPoint.x()) + axisNameA[typeBY] + QString(": %1").arg(localPoint.y());
    textLabel->setText(text);



}
void airfoilAnalyse::changeListModel(const int index){
    switch (index) {
    case 0:
        if(!nameArrayA.isEmpty())
            listViewA->setModel(StringListModelA);
        break;
    case 1:
        listViewA->setModel(StringListModelB);
        break;
    case 2:
        listViewA->setModel(StringListModelC);
    default:
        break;
    }

    QObject::connect(listViewA->selectionModel(),&QItemSelectionModel::selectionChanged,this,&airfoilAnalyse::drawAirfoil);
}
void airfoilAnalyse::updateAxes(QChart *chart, const QVector<QLineSeries*> &seriesList) {
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
void airfoilAnalyse::updateAirfoilArrayA(const QVector<QVector<QVector<double>>>&airfoil,const QVector<QString>&name){

    airfoilArrayA = airfoil;
    nameArrayA = name;
    QStringList stringList = QStringList::fromVector(name);
    StringListModelA = new QStringListModel(stringList);
}
void airfoilAnalyse::changeBoxTitle(){

    if(designAlphaRadioButton->isChecked()){
        inputGroupBoxB->setTitle("指定迎角");
    }
    else{
        inputGroupBoxB->setTitle("指定升力系数");
    }

}
void airfoilAnalyse::buildMoreReAnalyseWidget(){


    Vlayout = new QVBoxLayout(moreReAnalyseWidget);
    openAirfoilListButton = new QPushButton(moreReAnalyseWidget);

    inputGroupBoxA = new QGroupBox("雷诺数设置",moreReAnalyseWidget);
    inputGroupBoxB = new QGroupBox("迎角设置",moreReAnalyseWidget);
    HlayoutB = new QHBoxLayout();



    GlayoutA = new QGridLayout(inputGroupBoxA);
    GlayoutB = new QGridLayout(inputGroupBoxB);
    // GlayoutB = new QGridLayout(inputFrameB);




    minValueLabel = new QLabel(moreReAnalyseWidget);
    maxValueLabel = new QLabel(moreReAnalyseWidget);
    stepValueLabel = new QLabel(moreReAnalyseWidget);
    maLabel = new QLabel(moreReAnalyseWidget);
    maValueEdit = new QLineEdit(moreReAnalyseWidget);
    nriclLabel = new QLabel(moreReAnalyseWidget);

    nriclCombobox = new QComboBox(moreReAnalyseWidget);


    minValueEdit = new QLineEdit(moreReAnalyseWidget);
    maxValueEdit = new QLineEdit(moreReAnalyseWidget);
    stepValueEdit = new QLineEdit(moreReAnalyseWidget);

    xtrtopLabel = new QLabel(moreReAnalyseWidget);
    xtrbotLabel = new QLabel(moreReAnalyseWidget);

    xtrtopEdit = new QLineEdit(moreReAnalyseWidget);
    xtrbotEdit = new QLineEdit(moreReAnalyseWidget);




    minAlphaAndLiftLabel = new QLabel(moreReAnalyseWidget);
    maxAlphaAndLiftLabel = new QLabel(moreReAnalyseWidget);
    alphaAndLiftStepReLabel = new QLabel(moreReAnalyseWidget);
    minAlphaAndLiftReEdit = new QLineEdit(moreReAnalyseWidget);
    maxAlphaAndLiftReEdit = new QLineEdit(moreReAnalyseWidget);
    alphaAndLiftStepReEdit = new QLineEdit(moreReAnalyseWidget);


    alphaRadioLabel = new QLabel(moreAirfoilAnalyseDialog);
    liftRadioLabel = new QLabel(moreReAnalyseWidget);
    designAlphaRadioButton = new QRadioButton(moreReAnalyseWidget);
    designLiftRadioButton = new QRadioButton(moreReAnalyseWidget);

    startAnalyseReButton = new QPushButton(moreReAnalyseWidget);
    cancelAnalyseReButton = new QPushButton(moreReAnalyseWidget);





    minAlphaAndLiftLabel->setText("最小值");
    maxAlphaAndLiftLabel->setText("最大值");
    alphaAndLiftStepReLabel->setText("步长");
    openAirfoilListButton->setText("导入翼型");
    maLabel->setText("马赫数");
    nriclLabel->setText("转捩因子");
    minValueLabel->setText("最小值");
    maxValueLabel->setText("最大值");
    stepValueLabel->setText("步长");
    xtrtopLabel->setText("上弧转捩位置");
    xtrbotLabel->setText("下弧转捩位置");
    designAlphaRadioButton->setChecked(true);
    alphaRadioLabel->setText("按迎角分析");
    liftRadioLabel->setText("按升力系数分析");

    startAnalyseReButton->setText("开始");
    cancelAnalyseReButton->setText("取消");


    nriclCombobox->addItem("8->一般风洞");
    nriclCombobox->addItem("9->平均风洞");
    nriclCombobox->addItem("10->干净的风洞");
    nriclCombobox->addItem("11->很干净的风洞");
    nriclCombobox->setCurrentIndex(1);

    minValueEdit->setText("100000");
    maxValueEdit->setText("200000");
    stepValueEdit->setText("10000");
    xtrtopEdit->setText("1");
    xtrbotEdit->setText("1");
    maValueEdit->setText("0");
    minAlphaAndLiftReEdit->setText("0");
    maxAlphaAndLiftReEdit->setText("10");
    alphaAndLiftStepReEdit->setText("1");


    minValueEdit->setFixedWidth(80);
    maxValueEdit->setFixedWidth(80);
    stepValueEdit->setFixedWidth(80);
    maValueEdit->setFixedWidth(80);

    xtrtopEdit->setFixedWidth(80);
    xtrbotEdit->setFixedWidth(80);

    minAlphaAndLiftReEdit->setFixedWidth(80);
    maxAlphaAndLiftReEdit->setFixedWidth(80);
    alphaAndLiftStepReEdit->setFixedWidth(80);











    //GlayoutA->addWidget(reLabel,0,0,1,1);
    GlayoutA->addWidget(openAirfoilListButton,0,4,1,2);
    GlayoutA->addWidget(minValueLabel,1,0,1,1);
    GlayoutA->addWidget(minValueEdit,1,1,1,1);
    GlayoutA->addWidget(maxValueLabel,1,2,1,1);
    GlayoutA->addWidget(maxValueEdit,1,3,1,1);
    GlayoutA->addWidget(stepValueLabel,1,4,1,1);
    GlayoutA->addWidget(stepValueEdit,1,5,1,1);




    GlayoutA->addWidget(maLabel,2,0,1,1);
    GlayoutA->addWidget(maValueEdit,2,1,1,1);
    GlayoutA->addWidget(nriclLabel,2,2,1,1);
    GlayoutA->addWidget(nriclCombobox,2,3,1,1);

    GlayoutA->addWidget(xtrtopLabel,3,3,1,2);
    GlayoutA->addWidget(xtrtopEdit,3,5,1,1);
    GlayoutA->addWidget(xtrbotLabel,4,3,1,2);
    GlayoutA->addWidget(xtrbotEdit,4,5,1,1);










    GlayoutB->addWidget(alphaRadioLabel,0,1,1,1);
    GlayoutB->addWidget(designAlphaRadioButton,0,2,1,1);
    GlayoutB->addWidget(liftRadioLabel,0,3,1,1);
    GlayoutB->addWidget(designLiftRadioButton,0,4,1,1);


    GlayoutB->addWidget(minAlphaAndLiftLabel,1,0,1,1);
    GlayoutB->addWidget(minAlphaAndLiftReEdit,1,1,1,1);
    GlayoutB->addWidget(maxAlphaAndLiftLabel,1,2,1,1);
    GlayoutB->addWidget(maxAlphaAndLiftReEdit,1,3,1,1);
    GlayoutB->addWidget(alphaAndLiftStepReLabel,1,4,1,1);
    GlayoutB->addWidget(alphaAndLiftStepReEdit,1,5,1,1);

    HlayoutB->addStretch();
    HlayoutB->addWidget(startAnalyseReButton);
    HlayoutB->addWidget(cancelAnalyseReButton);
    HlayoutB->addStretch();



    Vlayout->addWidget(inputGroupBoxA);
    Vlayout->addWidget(inputGroupBoxB);
    Vlayout->addLayout(HlayoutB);

    //Vlayout->addWidget(inputFrameB);
    moreReAnalyseWidget->setLayout(Vlayout);




    QObject::connect(designAlphaRadioButton,SIGNAL(clicked()),this,SLOT(changeBoxTitle()));
    QObject::connect(designLiftRadioButton,SIGNAL(clicked()),this,SLOT(changeBoxTitle()));
    QObject::connect(openAirfoilListButton,SIGNAL(clicked()),this,SLOT(showAirfoilLibary()));
    QObject::connect(startAnalyseReButton,SIGNAL(clicked()),this,SLOT(startReModelAnalyse()));




    //moreReAnalyseDialog->hide();

}
void airfoilAnalyse::updateReAnalyseSetting(){

    if(!reAnalyseSetting.isEmpty())
        reAnalyseSetting.clear();
    if(!reArray.isEmpty())
        reArray.clear();
    int threadSize;
    int alphaSize;
    QVector<QString>reNumber;
    QVector<QString>alphaNumber;

    int models;
    if(designAlphaRadioButton->isChecked())
        models = 0;
    else
        models = 1;

    int minReValue = minValueEdit->text().toInt();
    int maxReValue = maxValueEdit->text().toInt();
    int stepValue = stepValueEdit->text().toInt();
    double maValue = maValueEdit->text().toDouble();
    int ncrit = nriclCombobox->currentIndex() + 8;
    double mina = minAlphaAndLiftReEdit->text().toDouble();
    double maxa = maxAlphaAndLiftReEdit->text().toDouble();
    double stepa = alphaAndLiftStepReEdit->text().toDouble();
    double tmpXtrTop = xtrtopEdit->text().toDouble();
    double tmpXtrBot = xtrbotEdit->text().toDouble();

    if(minReValue == maxReValue){
        threadSize = 1;
    }else{
        threadSize = abs(maxReValue - minReValue) / stepValue + 1;
    }

    if(mina == maxa){
        alphaSize = 1;
    }else{
        alphaSize = abs(maxa - mina) / stepa;
    }
    for(int i = 0; i < alphaSize + 1;i++){
        double alpha = mina + i * stepa;
        alphaArray.append(alpha);
        alphaNumber.append(QString::number(alpha));
    }

    for(int i = 0; i < threadSize;i++){
        xfoilSetting tmp;
        int re = minReValue + i * stepValue;
        reArray.append(re);
        tmp.Re = re;
        tmp.Ma = maValue;
        tmp.nCrit = ncrit;
        tmp.minAlpha = mina;
        tmp.maxAlpha = maxa;
        tmp.alphaStepSize = stepa;
        tmp.model = models;
        tmp.MaType = 1;
        tmp.ReType = 1;


        tmp.xtrTop = tmpXtrTop;
        tmp.xtrBot = tmpXtrBot;

        reAnalyseSetting.append(tmp);
        reNumber.append(QString::number(re));

    }


    QStringList reNumberList = reNumber.toList();
    QStringList alphaberList = alphaNumber.toList();

    reynoldListModelB->setStringList(reNumberList);
    reynoldListModelC->setStringList(alphaberList);
    listviewB->setModel(reynoldListModelB);
    listviewC->setModel(reynoldListModelC);
    listviewB->selectAll();
    listviewC->selectAll();
    QObject::connect(listviewB->selectionModel(),&QItemSelectionModel::selectionChanged,this,&airfoilAnalyse::changeReResult);
    QObject::connect(listviewC->selectionModel(),&QItemSelectionModel::selectionChanged,this,&airfoilAnalyse::changeAlphaResult);



}
void airfoilAnalyse::startReModelAnalyse(){



    if(realAirfoil.isEmpty()){
        QMessageBox::information(nullptr, "通知", "未导入任何文件");
        showAirfoilLibary();

    }
    else{

        moreReAnalyseWidget->close();
        updateReAnalyseSetting();
        int len1 = reAnalyseSetting.length();


        if(!airfoilResultData.isEmpty())
            airfoilResultData.clear();

        int olen = len1;
        QVector<int> numMatrix;  //多线程循环的次数矩阵
        while(olen > 0){
            if(olen >= threadNum){
                numMatrix.append(threadNum);
                olen = olen - threadNum;
            }
            else{
                numMatrix.append(olen);
                break;
            }
        }
        int length = numMatrix.length();

        int progressValue = 0;
        emit progressUpdated(progressValue);
        for(int i = 0;i < length; i++){
            QVector<QFuture <void>>futures;  //线程监视
            QVector<airfoilSolve*>solverPools;


            for(int j = 0; j < numMatrix[i];j++){
                airfoilSolve* solvers = new airfoilSolve();
                solverPools.append(solvers);
                solvers->refreshParaments(reAnalyseSetting[i * threadNum + j]);
                solvers->importAirfoil(realAirfoil);
                QFuture<void>future = QtConcurrent::run([solvers](){
                    solvers->solverInThread();
                });
                futures.append(future);
            }

            bool allFinished = false;
            while(!allFinished){
                allFinished = true;
                for(int k = 0; k< numMatrix[i];k++){
                    if(!futures[k].isFinished()){
                        allFinished = false;
                        break;
                    }
                }
                QCoreApplication::processEvents();
            }

            for(int k = 0; k<numMatrix[i];k++){
                airfoilResultData.append(solverPools[k]->resultData);

            }

            qDeleteAll(solverPools);
            solverPools.clear();
            solverPools.squeeze();
            progressValue = (static_cast<double>(i) + 1) / length * 100;
            emit progressUpdated(progressValue);
        }

        listviewB->show();
        listviewC->show();
        allLabelA->show();
        allLabelB->show();
        allCheckBoxA->show();
        allCheckBoxB->show();
        alphaViewButton->show();
        reViewButton->show();



        changeUIForReView();
        resultWidget->show();

    }





}
void airfoilAnalyse::changeReResult(){


    for(int i = 0;i < reynoldListModelB->rowCount();i++){
        QModelIndex index = reynoldListModelB->index(i, 0);
        bool selction = listviewB->selectionModel()->isSelected(index);
        seriesArrayA[i]->setVisible(selction);
        seriesArrayB[i]->setVisible(selction);
    }
}
void airfoilAnalyse::changeAlphaResult(){
    for(int i = 0;i < reynoldListModelC->rowCount();i++){
        QModelIndex index = reynoldListModelC->index(i, 0);
        bool selction = listviewC->selectionModel()->isSelected(index);
        seriesArrayA[i]->setVisible(selction);
        seriesArrayB[i]->setVisible(selction);
    }


}
void airfoilAnalyse::changeUIForReView(){
    viewModel = 1;
    reViewButton->setEnabled(false);
    alphaViewButton->setEnabled(true);
    listviewC->setSelectionMode(QAbstractItemView::NoSelection);
    listviewB->setSelectionMode(QAbstractItemView::MultiSelection);
    drawReResult();

}
void airfoilAnalyse::changeUIForAlphaView(){
    viewModel = 2;
    alphaViewButton->setEnabled(false);
    reViewButton->setEnabled(true);
    listviewB->setSelectionMode(QAbstractItemView::NoSelection);
    listviewC->setSelectionMode(QAbstractItemView::MultiSelection);
    drawAlphaResult();


}
void airfoilAnalyse::exportChartData() {
    QString fileName = QFileDialog::getSaveFileName(nullptr, "Save Data", "", "Text Files (*.txt)");
    if (!fileName.isEmpty()) {
        QFile file(fileName);
        if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QTextStream stream(&file);

            // 获取坐标轴
            auto axisX = chartRA->axes(Qt::Horizontal).at(0);
            auto axisY = chartRA->axes(Qt::Vertical).at(0);

            // 获取坐标轴的标签
            QString xLabel = axisX->titleText();
            QString yLabel = axisY->titleText();

            // 遍历图表中的所有系列
            const auto seriesList = chartRA->series();
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
            QMessageBox::information(nullptr,"提示","保存成功");

        } else {

        }
    }

}
airfoilAnalyse::~airfoilAnalyse(){
    for (QLineSeries* line : seriesArrayA) {
        delete line;
    }
    for (QLineSeries* line : seriesArrayB) {
        delete line;
    }

}
