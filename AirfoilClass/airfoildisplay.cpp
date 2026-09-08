
#include "AirfoilClass/airfoildisplay.h"
#include "AirfoilClass/cfddatareader.h"
#include <QFile>
#include <QFileDialog>
#include <QDir>
#include <QMessageBox>
#include <QtGlobal>
#include <QFutureWatcher>
#include <QtConcurrent/QtConcurrent>
#include <QFont>
#include "PublicClass/myfile.h"

#if defined(_MSC_VER) && (_MSC_VER >= 1600)
# pragma execution_character_set("utf-8")
#endif


airfoilDisplay::airfoilDisplay() {
    initialSetup();
    loadIcon();
    initialCheckCSTDialog();
    initialAirfoilLibaries();
    initialExplorerWidget();
    initialAnalyse();
    initialAirfoilOutput();
    initialAirfoilDesignWidget();
    initialAirfoilOptimizationWidget();
    initialDesignChartMenu();
    initialRChartMenu();
    initialAirfoilBlendingDialog();
    initialInterDragWidget();
    initialInterChartMenu();
    initialModifyAirfoilDialog();



    QObject::connect(resultChartViewA,&AirfoilPlot::mouseRightLeftPress,this,&airfoilDisplay::showRChartAMenu);
    QObject::connect(resultChartViewB,&AirfoilPlot::mouseRightLeftPress,this,&airfoilDisplay::showRChartBMenu);
    QObject::connect(dragInterViewA,&AirfoilPlot::mouseRightLeftPress,this,&airfoilDisplay::showInterChartMenu);
    QObject::connect(resultChartViewA,&AirfoilPlot::mousePositionChanged,this,&airfoilDisplay::updateResultAMousePosition);
    QObject::connect(resultChartViewB,&AirfoilPlot::mousePositionChanged,this,&airfoilDisplay::updateResultBMousePosition);


    QObject::connect(designChartViewA,&AirfoilPlot::mousePositionChanged,this,&airfoilDisplay::updateDesignMousePosition);
    QObject::connect(designChartViewA,&AirfoilPlot::chartResized,this,&airfoilDisplay::updateDesignTextItem);
    QObject::connect(designChartViewA,&AirfoilPlot::mousePressBegin,this,&airfoilDisplay::getMousePressBeginPosition);
    QObject::connect(designChartViewA,&AirfoilPlot::mouseReleaseEnd,this,&airfoilDisplay::getMouseReleaseEndPosition);
    QObject::connect(designChartViewA,&AirfoilPlot::mouseRightLeftPress,this,&airfoilDisplay::showDesignChartMenu);

    QObject::connect(optimizationChartViewA,&AirfoilPlot::chartResized,this,&airfoilDisplay::updateOptimizationTextItem);

    connect(this,&airfoilDisplay::emitOptimizationUI,this,&airfoilDisplay::updateOptimizationUI);







    /*
     *



    if(!reader.openFile(filePath)){
        qDebug() << "Failed to open HDF5 file.";

    }
    // -------------------------
    // 读取几何参数
    auto cst = reader.readCST();
    qDebug() << "CST shape:" << cst.size() << "x" << (cst.isEmpty() ? 0 : cst[0].size());

    auto bezier = reader.readBezier();
    qDebug() << "Bezier shape:" << bezier.size() << "x" << (bezier.isEmpty() ? 0 : bezier[0].size());

    auto grassmann = reader.readGrassmann();
    qDebug() << "Grassmann shape:" << grassmann.size() << "x" << (grassmann.isEmpty() ? 0 : grassmann[0].size());

    auto landmarks = reader.readLandmarks();
    qDebug() << "Landmarks shape:" << landmarks.size()
             << "x" << (landmarks.isEmpty() ? 0 : landmarks[0].size())
             << "x" << (landmarks.isEmpty() ? 0 : landmarks[0].isEmpty() ? 0 : landmarks[0][0].size());

    // -------------------------
    // 读取气动系数
    auto cl = reader.readCL("/alpha_0"); // 根据 HDF5 文件实际 group 名
     << "C_l size:" << cl.size();

    auto cd = reader.readCD("/alpha_0");
     << "C_d size:" << cd.size();

    auto cm = reader.readCM("/alpha_0");
     << "C_m size:" << cm.size();
    */



}
airfoilDisplay::airfoilDisplay(const int num){
    cstNum = num;
    initialSetup();
    initialModifyAirfoilDialogCST();
    designModel = new airfoilDesign(cstNum);//机翼设计用
    QObject::connect(chartViewA,&AirfoilPlot::mousePositionChanged,this,&airfoilDisplay::updateMousePosition);
    QObject::connect(chartViewA,&AirfoilPlot::mousePressBegin,this,&airfoilDisplay::getMousePressBeginPosition);
    QObject::connect(chartViewA,&AirfoilPlot::mouseReleaseEnd,this,&airfoilDisplay::getMouseReleaseEndPosition);

}

void airfoilDisplay::setAirfoil(QVector<QVector<double>>&airfoilData,QVector<double>&pointY){

    newAirfoil = airfoilData;
    if(!airfoilData.isEmpty()){  
        designModel->buildBenrnstein(airfoilData);
        cst = designModel->cstParameter;
        cstPointY = pointY;
        drawAirfoil();
    }
}
void airfoilDisplay::exportChartData(AirfoilPlot *chart) {
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
            for (const AirfoilPlotSeries *lineSeries : seriesList) {
                if (lineSeries && !lineSeries->points().isEmpty()) {
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

void airfoilDisplay::saveAirfoilData(){
    emit emitAirfoilData(newAirfoil,cstPointY);
    modifyAirfoilDialog->hide();
}




void airfoilDisplay::loadIcon(){



    checkIcon = QDir::currentPath() + "/resoure/images/check.png";
    removeIcon = QDir::currentPath() + "/resoure/images/remove.png";

    for(int i = 0;i<8;i++)
        lineStyleIcon[i] = QDir::currentPath() + "/resoure/images/lineStyle/style" + QString::number(i) + ".png";


}
void airfoilDisplay::initialSetup(){

    QString initialName = QCoreApplication::applicationDirPath() + "/setting/airfoilSetting.txt";
    QFile file(initialName);
    if(QFile::exists(initialName)){
        if(file.open(QIODevice::ReadWrite | QIODevice::Text)){
            QTextStream in(&file);
            while(!in.atEnd()){
                QString line = in.readLine();
                QStringList parts = line.split(" ",Qt::SkipEmptyParts);
                if(parts.size() == 2){
                    //double value1 = parts[0].toDouble();
                    double value1 = parts[1].toDouble();


                    historySettingData.append(value1);
                    //historySettingData.append(value2);
                }
            }
            file.close();

            //xfoil setting
            input.Re = int(historySettingData[0]); input.Ma = historySettingData[1]; input.nCrit = int(historySettingData[2]); input.alphaStepSize = historySettingData[3];
            input.minAlpha = historySettingData[4];input.maxAlpha = historySettingData[5];input.xtrTop = historySettingData[6];input.xtrBot = historySettingData[7];
            cstNum = historySettingData[8];threadNum = historySettingData[9];input.designCL = historySettingData[10]; input.designALPHA = historySettingData[11];
            input.model = 0;input.s_IterLim = historySettingData[13];



            //Ga setting
            GaSetting.step = historySettingData[14];GaSetting.selection = historySettingData[15];GaSetting.initialEliteNum = historySettingData[16];GaSetting.eliteNum = historySettingData[17];
            GaSetting.cross = historySettingData[18];GaSetting.variation = historySettingData[19];GaSetting.val = historySettingData[20];GaSetting.cstRadio = historySettingData[21];GaSetting.solutionsNum = cstNum * 2;


            //axis setting
            MINAXIS[0] = historySettingData[22];MINAXIS[1] = historySettingData[24];MINAXIS[2] = historySettingData[26];MINAXIS[3] = historySettingData[28];MINAXIS[4] = historySettingData[30];MINAXIS[5] = historySettingData[32];MINAXIS[6] = historySettingData[34];
            MAXAXIS[0] = historySettingData[23];MAXAXIS[1] = historySettingData[25];MAXAXIS[2] = historySettingData[27];MAXAXIS[3] = historySettingData[29];MAXAXIS[4] = historySettingData[31];MAXAXIS[5] = historySettingData[33];MAXAXIS[6] = historySettingData[35];
            MINAXIS[7] = historySettingData[36];
            MAXAXIS[7] = historySettingData[37];
        }
    }
    else {
        QFile newfile(initialName);
        if(newfile.open(QIODevice::ReadWrite | QIODevice::Text)){
            QTextStream stream(&newfile);
            stream<<"Re    100000"<<"\n"<<"Ma    0.01"<<"\n"<<"Ncrit   9"<<"\n"<<"AlphaStep    0.5"<<"\n "<<"MinAlpha    0"<<"\n"<<"MaxAlpha   10"<<"\n"<<"XtrTop    1"<<"\n"<<"XtrBot    1"<<"\n"<<"CstNum    6"<<"\n"<<"ThreadNum    10"<<"\n"<<"DesignCl    0.5"<<"\n"<<"DesignAlpha    2"<<"\n"<<"Model    0"<<"\n"<<"S_IterLim    200"
                   <<"\n"<<"Step   10"<<"\n"<<"Selection    1.4"<<"\n"<<"InitialElite    20"<<"\n"<<"Elite   20"<<"\n"<<"Cross    0.95"<<"\n"<<"Variation    0.05"<<"\n"<<"Val    0.000001"<<"\n"<<"CstRatio    0.5";
            stream<<"\n"<<"MINAXIS1    -5"<<"\n"<<"MAXAXIS1   12"<<"\n"<<"MINAXIS2    -1"<<"\n"<<"MAXAXIS2    2"<<"\n"<<"MINAXIS3   0"<<"\n"<<"MAXAXIS3   0.03"<<"\n"<<"MINAXIS4   -50"<<"\n"<<"MAXAXIS4   120"<<"\n"<<"MINAXIS5   -50"<<"\n"<<"MAXAXIS5   100"<<"\n"<<"MINAXIS6  -0.3"<<"\n"<<"MAXAXIS6   0.3"<<"\n"<<"MINAXIS7   -3"<<"\n"<<"MAXAXIS7    3"<<"\n"
                   <<"MINAXIS8    6"<<"\n"<<"MAXAXIS8    6";
            newfile.close();
            input.Re = 100000; input.Ma = 0.00; input.nCrit = 9; input.alphaStepSize = 0.5;
            input.minAlpha = 0;input.maxAlpha = 10;input.xtrTop = 1;input.xtrBot = 1;
            cstNum = 6;threadNum = 10;input.designCL = 0.5;input.designALPHA = 2;input.model = 0;input.s_IterLim = 200;

            GaSetting.step = 10;GaSetting.selection = 1.4;GaSetting.initialEliteNum = 20;GaSetting.eliteNum = 20;
            GaSetting.cross = 0.95;GaSetting.variation = 0.05;GaSetting.val = 0.00001;GaSetting.cstRadio = 0.5;GaSetting.solutionsNum = cstNum * 2;

            MINAXIS[0] = -5;MINAXIS[1] = -1;MINAXIS[2] = 0;MINAXIS[3] = -50;MINAXIS[4] = -50;MINAXIS[5] = -0.3;MINAXIS[6] = -3;MINAXIS[7] = 6;
            MAXAXIS[0] = 12;MAXAXIS[1] = 2;MAXAXIS[2] = 0.03;MAXAXIS[3] = 120;MAXAXIS[4] = 100;MAXAXIS[5] = 0.3;MAXAXIS[6] = 3;MAXAXIS[7] = 6;

        }
    }


    cstPointX.append(0);
    cstPointY.append(0.1);
    for(int i = 0;i < cstNum - 2;i++){
        cstPointX.append(static_cast<double>(i + 1) / (cstNum - 1));
        cstPointY.append(0.15);
    }
    cstPointX.append(1);
    cstPointY.append(0.1);
    cstPointX.append(1);
    cstPointY.append(-0.1);
    for(int i = 0;i < cstNum - 2;i++){
        cstPointX.append(static_cast<double>(cstNum - i - 2) / (cstNum - 1));
        cstPointY.append(-0.15);
    }
    cstPointX.append(0);
    cstPointY.append(-0.1);


    axisXName[0] = "Alpha"; axisYName[0] = "Cl"; titleName[0] = "升力系数曲线";
    axisXName[1] = "Alpha"; axisYName[1] = "Cl"; titleName[1] = "升力系数曲线";
    axisXName[2] = "Alpha"; axisYName[2] = "Cd"; titleName[2] = "阻力系数曲线";
    axisXName[3] = "Alpha"; axisYName[3] = "升阻比"; titleName[3] = "升阻比曲线";
    axisXName[4] = "Alpha"; axisYName[4] = "功率因子"; titleName[4] = "功率因子曲线";
    axisXName[5] = "Alpha"; axisYName[5] = "Cm"; titleName[5] = "力矩曲线";
    axisXName[6] = "X"; axisYName[6] = "Cp"; titleName[6] = "压力系数曲线";
    axisXName[7] = "迭代次数";axisYName[7] = "目标值";titleName[7] = "迭代曲线";
    axisXName[8] = "X"; axisYName[8] = "Cp"; titleName[8] = "极曲线";
}


void airfoilDisplay::initialAirfoilOptimizationWidget(){
    airfoilOptimizationWidget = new QWidget();


    optimizationVLayout = new QVBoxLayout(airfoilOptimizationWidget);
    optimizationHLayoutA = new QHBoxLayout();

    optimizationHLayoutB = new QHBoxLayout();
    optimizationVLayoutB1 = new QVBoxLayout();

    optimizationHLayoutB2 = new QHBoxLayout();

    optimizationBox = new QGroupBox(airfoilOptimizationWidget);
    optimizationBox->setTitle("翼型列表");

    optimizationToolBox = new QGroupBox(airfoilOptimizationWidget);
    optimizationToolBox->setTitle("工具栏");



    optimizationBox->setFixedWidth(400);

    optimizationToolBox->setFixedWidth(400);

    optimizationGLayoutA1 = new QGridLayout(optimizationBox);

    optimizationGLayoutB2 = new QGridLayout(optimizationToolBox);

    optimizationChartA = new AirfoilPlot();
    iterateChart = new AirfoilPlot();
    cpxchart = new AirfoilPlot();

    optimizationChartViewA = optimizationChartA;
    iterateView = iterateChart;
    cpxchartView = cpxchart;

    optimizationChartViewA->setRenderHint(QPainter::Antialiasing);
    iterateView->setRenderHint(QPainter::Antialiasing);
    cpxchartView->setRenderHint(QPainter::Antialiasing);

    optimizationSeriesA = new AirfoilPlotSeries();
    iterateSeries = new AirfoilPlotSeries();
    oriCpxSeries = new AirfoilPlotSeries();
    optCpxSeries = new AirfoilPlotSeries();

    optimizationAxisXA = new AirfoilPlotAxis;
    optimizationAxisYA = new AirfoilPlotAxis;
    iterateAxAxis = new AirfoilPlotAxis;
    iterateAyAxis = new AirfoilPlotAxis;
    cpxAxAxis = new AirfoilPlotAxis;
    cpxAyAxis = new AirfoilPlotAxis;

    optimizationAxisXA->setVisible(false);
    optimizationAxisYA->setVisible(false);

    iterateAxAxis->setGridLineVisible(false);
    iterateAyAxis->setGridLineVisible(false);


    cpxAxAxis->setGridLineVisible(false);
    cpxAyAxis->setGridLineVisible(false);




    QPen pen;
    pen.setColor(Qt::black);
    pen.setStyle(Qt::DashLine);
    pen.setWidth(2);
    oriCpxSeries->setColor(Qt::red);
    optCpxSeries->setPen(pen);
    iterateSeries->setColor(Qt::black);

    optimizationChartViewA->installEventFilter(this);

    optimizationChartViewA->textItem->setFont(QFont("Arial",12));
    optimizationChartA->addSeries(optimizationSeriesA);
    iterateChart->addSeries(iterateSeries);
    cpxchart->addSeries(oriCpxSeries);
    cpxchart->addSeries(optCpxSeries);

    optimizationAxisXA->setRange(-0.1,1.1);
    optimizationAxisYA->setRange(-0.25,0.25);
    iterateAxAxis->setRange(0,GaSetting.step);
    iterateAyAxis->setRange(0,MAXAXIS[3]);
    cpxAxAxis->setRange(-0.05,1.05);
    cpxAyAxis->setRange(-3,2);
    cpxAyAxis->setReverse(true);


    optimizationChartA->addAxis(optimizationAxisXA,Qt::AlignBottom);
    optimizationChartA->addAxis(optimizationAxisYA,Qt::AlignLeft);
    iterateChart->addAxis(iterateAxAxis,Qt::AlignBottom);
    iterateChart->addAxis(iterateAyAxis,Qt::AlignLeft);
    cpxchart->addAxis(cpxAxAxis,Qt::AlignBottom);
    cpxchart->addAxis(cpxAyAxis,Qt::AlignLeft);

    optimizationSeriesA->attachAxis(optimizationAxisXA);
    optimizationSeriesA->attachAxis(optimizationAxisYA);
    iterateSeries->attachAxis(iterateAxAxis);
    iterateSeries->attachAxis(iterateAyAxis);
    oriCpxSeries->attachAxis(cpxAxAxis);
    oriCpxSeries->attachAxis(cpxAyAxis);
    optCpxSeries->attachAxis(cpxAxAxis);
    optCpxSeries->attachAxis(cpxAyAxis);
    optimizationChartA->legend()->setVisible(false);
    iterateChart->legend()->setVisible(false);
    cpxchart->legend()->setVisible(false);

    iterateAxAxis->setTitleText(axisXName[7]);
    iterateAyAxis->setTitleText(axisYName[7]);
    cpxAxAxis->setTitleText(axisXName[6]);
    cpxAyAxis->setTitleText(axisYName[6]);
    iterateChart->setTitle(titleName[7]);
    cpxchart->setTitle(titleName[6]);


    optimizationVLayout->addLayout(optimizationHLayoutA);
    optimizationVLayout->addLayout(optimizationHLayoutB);

    optimizationHLayoutB->addLayout(optimizationVLayoutB1);
    optimizationHLayoutB->addLayout(optimizationHLayoutB2);

    optimizationHLayoutA->addWidget(optimizationBox);
    optimizationHLayoutA->addWidget(optimizationChartViewA);


    optimizationVLayoutB1->addWidget(optimizationToolBox);
    optimizationHLayoutB2->addWidget(iterateView);
    optimizationHLayoutB2->addWidget(cpxchartView);

    stepLabel = new QLabel(airfoilOptimizationWidget);
    selectionLabel = new QLabel(airfoilOptimizationWidget);
    initialEliteNumLabel = new QLabel(airfoilOptimizationWidget);
    eliteNumLabel = new QLabel(airfoilOptimizationWidget);
    crossLabel = new QLabel(airfoilOptimizationWidget);
    variationLabel = new QLabel(airfoilOptimizationWidget);
    valLabel = new QLabel(airfoilOptimizationWidget);
    cstRadioLabel = new QLabel(airfoilOptimizationWidget);
    ThreadNumLabel = new QLabel(airfoilOptimizationWidget);
    iterLimLabel = new QLabel(airfoilOptimizationWidget);
    optReLabel = new QLabel(airfoilOptimizationWidget);
    optMaLabel = new QLabel(airfoilOptimizationWidget);


    stepEdit = new QLineEdit(airfoilOptimizationWidget);
    selectionEdit = new QLineEdit(airfoilOptimizationWidget);
    initialEliteNumEdit = new QLineEdit(airfoilOptimizationWidget);
    eliteNumEdit = new QLineEdit(airfoilOptimizationWidget);
    crossEdit = new QLineEdit(airfoilOptimizationWidget);
    variationEdit = new QLineEdit(airfoilOptimizationWidget);
    valEdit = new QLineEdit(airfoilOptimizationWidget);
    cstRadioEdit = new QLineEdit(airfoilOptimizationWidget);
    ThreadNumEdit = new QLineEdit(airfoilOptimizationWidget);
    iterLimEdit = new QLineEdit(airfoilOptimizationWidget);
    optReEdit = new QLineEdit(airfoilOptimizationWidget);
    optMaEdit = new QLineEdit(airfoilOptimizationWidget);


    designAlphaLabel = new QLabel(airfoilOptimizationWidget);
    designClLabel = new QLabel(airfoilOptimizationWidget);
    targetRadioButtonA = new QRadioButton(airfoilOptimizationWidget);
    targetRadioButtonB = new QRadioButton(airfoilOptimizationWidget);
    valueEdit = new QLineEdit(airfoilOptimizationWidget);


    thickWeightedLabel = new QLabel(airfoilOptimizationWidget) ;
    cmWeightedLabel = new QLabel(airfoilOptimizationWidget) ;
    thickWeightedSlider = new QSlider(Qt::Horizontal,airfoilOptimizationWidget);
    cmWeightedSlider = new QSlider(Qt::Horizontal,airfoilOptimizationWidget);
    thickWeightedValueLabel = new QLabel(airfoilOptimizationWidget) ;
    cmWeightedValueLabel = new QLabel(airfoilOptimizationWidget) ;







    choseAirfoilLabel = new QLabel(airfoilOptimizationWidget);
    //modelCombobox = new QComboBox(airfoilOptimizationWidget);
    choiceCombobox = new QComboBox(airfoilOptimizationWidget);
    targetLabel = new QLabel(airfoilOptimizationWidget);
    //移动位置
    QLabel *labelSpace = new QLabel(airfoilOptimizationWidget);
    labelSpace->setText(" ");
    labelSpace->setFixedWidth(50);


    valueEdit->setText(QString::number(input.designCL));


    stepLabel->setText("迭代步数");
    selectionLabel->setText("轮盘赌指数");
    initialEliteNumLabel->setText("初始种群");
    eliteNumLabel->setText("精英种群");
    crossLabel->setText("交叉概率");
    variationLabel->setText("变异概率");
    valLabel->setText("数值精度");
    cstRadioLabel->setText("变量上限");
    ThreadNumLabel->setText("线程数");
    iterLimLabel->setText("XFOIL最大迭代数");
    optReLabel->setText("雷诺数");
    optMaLabel->setText("马赫数");



    designAlphaLabel->setText("设计升力系数");
    designClLabel->setText("设计迎角");
    targetRadioButtonB->setChecked(true);
    choseAirfoilLabel->setText("翼型");
    thickWeightedLabel->setText("厚度权重");
    cmWeightedLabel->setText("力矩权重");
    thickWeightedValueLabel->setText("0");
    cmWeightedValueLabel->setText("0");
    thickWeightedSlider->setFixedWidth(100);
    cmWeightedSlider->setFixedWidth(100);
    thickWeightedSlider->setRange(0, 100);
    cmWeightedSlider->setRange(0, 100);
    targetLabel->setText("目标值");










    stepEdit->setText(QString::number(GaSetting.step));
    selectionEdit->setText(QString::number(GaSetting.selection));
    initialEliteNumEdit->setText(QString::number(GaSetting.initialEliteNum));
    eliteNumEdit->setText(QString::number(GaSetting.eliteNum));
    crossEdit->setText(QString::number(GaSetting.cross));
    variationEdit->setText(QString::number(GaSetting.variation));
    valEdit->setText(QString::number(GaSetting.val));
    cstRadioEdit->setText(QString::number(GaSetting.cstRadio));
    ThreadNumEdit->setText(QString::number(threadNum));
    iterLimEdit->setText(QString::number(input.s_IterLim));
    optReEdit->setText(QString::number(input.Re));
    optMaEdit->setText(QString::number(input.Ma));



    optimizationGLayoutA1->addWidget(stepLabel,0,0,1,1);
    optimizationGLayoutA1->addWidget(selectionLabel,0,2,1,1);
    optimizationGLayoutA1->addWidget(initialEliteNumLabel,1,0,1,1);
    optimizationGLayoutA1->addWidget(eliteNumLabel,1,2,1,1);
    optimizationGLayoutA1->addWidget(crossLabel,2,0,1,1);
    optimizationGLayoutA1->addWidget(variationLabel,2,2,1,1);
    optimizationGLayoutA1->addWidget(valLabel,3,0,1,1);
    optimizationGLayoutA1->addWidget(cstRadioLabel,3,2,1,1);
    optimizationGLayoutA1->addWidget(ThreadNumLabel,4,0,1,1);
    optimizationGLayoutA1->addWidget(iterLimLabel,4,2,1,1);
    optimizationGLayoutA1->addWidget(optReLabel,5,0,1,1);
    optimizationGLayoutA1->addWidget(optMaLabel,5,2,1,1);




    optimizationGLayoutA1->addWidget(stepEdit,0,1,1,1);
    optimizationGLayoutA1->addWidget(selectionEdit,0,3,1,1);
    optimizationGLayoutA1->addWidget(initialEliteNumEdit,1,1,1,1);
    optimizationGLayoutA1->addWidget(eliteNumEdit,1,3,1,1);
    optimizationGLayoutA1->addWidget(crossEdit,2,1,1,1);
    optimizationGLayoutA1->addWidget(variationEdit,2,3,1,1);
    optimizationGLayoutA1->addWidget(valEdit,3,1,1,1);
    optimizationGLayoutA1->addWidget(cstRadioEdit,3,3,1,1);
    optimizationGLayoutA1->addWidget(ThreadNumEdit,4,1,1,1);
    optimizationGLayoutA1->addWidget(iterLimEdit,4,3,1,1);
    optimizationGLayoutA1->addWidget(optReEdit,5,1,1,1);
    optimizationGLayoutA1->addWidget(optMaEdit,5,3,1,1);


    optimizationGLayoutB2->addWidget(labelSpace,0,0,1,1);
    optimizationGLayoutB2->addWidget(choseAirfoilLabel,0,1,1,2);
    optimizationGLayoutB2->addWidget(choiceCombobox,0,3,1,3);
    optimizationGLayoutB2->addWidget(designAlphaLabel,1,1,1,1);
    optimizationGLayoutB2->addWidget(designClLabel,1,3,1,1);
    optimizationGLayoutB2->addWidget(targetRadioButtonA,1,2,1,1);
    optimizationGLayoutB2->addWidget(targetRadioButtonB,1,4,1,1);
    optimizationGLayoutB2->addWidget(thickWeightedLabel,2,1,1,1);
    optimizationGLayoutB2->addWidget(thickWeightedSlider,2,2,1,2);
    optimizationGLayoutB2->addWidget(thickWeightedValueLabel,2,4,1,1);
    optimizationGLayoutB2->addWidget(cmWeightedLabel,3,1,1,1);
    optimizationGLayoutB2->addWidget(cmWeightedSlider,3,2,1,2);
    optimizationGLayoutB2->addWidget(cmWeightedValueLabel,3,4,1,1);
    optimizationGLayoutB2->addWidget(targetLabel,4,1,1,2);
    optimizationGLayoutB2->addWidget(valueEdit,4,3,1,2);




    connect(thickWeightedSlider,&QSlider::valueChanged,this,&airfoilDisplay::updateThickWeightedValueLabel);
    connect(cmWeightedSlider,&QSlider::valueChanged,this,&airfoilDisplay::updateCmWeightedValueLabel);

    connect(choiceCombobox, QOverload<int>::of(&QComboBox::activated),
            this, &airfoilDisplay::drawOptIndexAirfoil);


}
void airfoilDisplay::initialAirfoilDesignWidget(){

    airfoilDesignWidget = new QWidget();
    colorDialog = new QColorDialog();


    designVLayout = new QVBoxLayout(airfoilDesignWidget);
    designHLayoutA = new QHBoxLayout();

    designHLayoutB = new QHBoxLayout();
    designVLayoutB1 = new QVBoxLayout();

    designHLayoutB2 = new QHBoxLayout();

    airfoilListBox = new QGroupBox(airfoilDesignWidget);
    airfoilListBox->setTitle("翼型列表");
    designSettingBox = new QGroupBox(airfoilDesignWidget);
    designSettingBox->setTitle("XFOIL设置");
    airfoilToolBox = new QGroupBox(airfoilDesignWidget);
    airfoilToolBox->setTitle("工具栏");


    airfoilListBox->setFixedWidth(400);
    designSettingBox->setFixedWidth(400);
    airfoilToolBox->setFixedWidth(400);
    
    designGLayoutA1 = new QGridLayout(airfoilListBox);
    designGLayoutB1 = new QGridLayout(designSettingBox);
    designGLayoutB2 = new QGridLayout(airfoilToolBox);

    designChartA = new AirfoilPlot();
    resultChartA = new AirfoilPlot();
    resultChartB = new AirfoilPlot();



    designChartViewA = designChartA;
    resultChartViewA = resultChartA;
    resultChartViewB = resultChartB;

    designChartViewA->setRenderHint(QPainter::Antialiasing);
    resultChartViewA->setRenderHint(QPainter::Antialiasing);
    resultChartViewB->setRenderHint(QPainter::Antialiasing);





    designSeriesUpper = new AirfoilPlotSeries();
    designSeriesLower = new AirfoilPlotSeries();
    designScatterSeriesA = new AirfoilPlotSeries();
    designSeriesPointA = new AirfoilPlotSeries(true);
    designSeriesPointB = new AirfoilPlotSeries(true);
    transitionPoint = new AirfoilPlotSeries(true);
    designPointA = new AirfoilPlotSeries(true);
    designPointB = new AirfoilPlotSeries(true);
    xblUpperSeries = new AirfoilPlotSeries();
    xblLowerSeries = new AirfoilPlotSeries();



    autoSeriesA = new AirfoilPlotSeries;
    autoSeriesB = new AirfoilPlotSeries;



    designAxisXA = new AirfoilPlotAxis;
    designAxisYA = new AirfoilPlotAxis;
    resultAxisXA = new AirfoilPlotAxis;
    resultAxisYA = new AirfoilPlotAxis;
    resultAxisXB = new AirfoilPlotAxis;
    resultAxisYB = new AirfoilPlotAxis;



    designSeriesPointA->setMarkerSize(20);
    designSeriesPointB->setMarkerSize(12);
    transitionPoint->setMarkerSize(10);
    designPointA->setMarkerSize(15);
    designPointB->setMarkerSize(15);
    designSeriesPointA->setColor(Qt::red);
    designSeriesPointB->setColor(Qt::white);
    transitionPoint->setColor(Qt::black);
    designPointA->setColor(Qt::red);
    designPointB->setColor(Qt::red);
    designScatterSeriesA->setColor(Qt::blue);

    xblUpperSeries->setPen(QPen(QColor("#1f77b4"), 1.5, Qt::DashLine));
    xblLowerSeries->setPen(QPen(QColor("#d62728"), 1.5, Qt::DashLine));



    designChartViewA->installEventFilter(this);
    designChartViewA->setRangeDragEnabled(false);

    designChartA->addSeries(designSeriesUpper);
    designChartA->addSeries(designSeriesLower);
    designChartA->addSeries(designScatterSeriesA);
    designChartA->addSeries(designSeriesPointA);
    designChartA->addSeries(designSeriesPointB);
    designChartA->addSeries(transitionPoint);


    designChartA->addSeries(xblUpperSeries);
    designChartA->addSeries(xblLowerSeries);



    resultChartA->addSeries(autoSeriesA);
    resultChartA->addSeries(designPointA);
    resultChartB->addSeries(autoSeriesB);
    resultChartB->addSeries(designPointB);





    designAxisXA->setRange(-0.1,1.1);
    designAxisYA->setRange(-0.25,0.25);
    resultAxisXA->setRange(MINAXIS[0],MAXAXIS[0]);
    resultAxisYA->setRange(MINAXIS[1],MAXAXIS[1]);
    resultAxisXB->setRange(MINAXIS[0],MAXAXIS[0]);
    resultAxisYB->setRange(MINAXIS[0],MAXAXIS[0]);

    designChartA->addAxis(designAxisXA,Qt::AlignBottom);
    designChartA->addAxis(designAxisYA,Qt::AlignLeft);
    resultChartA->addAxis(resultAxisXA,Qt::AlignBottom);
    resultChartA->addAxis(resultAxisYA,Qt::AlignLeft);
    resultChartB->addAxis(resultAxisXB,Qt::AlignBottom);
    resultChartB->addAxis(resultAxisYB,Qt::AlignLeft);

    resultAxisYB->setReverse(true);  // 反转 Y 轴方向

    designSeriesUpper->attachAxis(designAxisXA);
    designSeriesUpper->attachAxis(designAxisYA);
    designSeriesLower->attachAxis(designAxisXA);
    designSeriesLower->attachAxis(designAxisYA);
    designScatterSeriesA->attachAxis(designAxisXA);
    designScatterSeriesA->attachAxis(designAxisYA);
    designSeriesPointA->attachAxis(designAxisXA);
    designSeriesPointA->attachAxis(designAxisYA);
    designSeriesPointB->attachAxis(designAxisXA);
    designSeriesPointB->attachAxis(designAxisYA);
    transitionPoint->attachAxis(designAxisXA);
    transitionPoint->attachAxis(designAxisYA);
    xblUpperSeries->attachAxis(designAxisXA);
    xblUpperSeries->attachAxis(designAxisYA);
    xblLowerSeries->attachAxis(designAxisXA);
    xblLowerSeries->attachAxis(designAxisYA);


    autoSeriesA->attachAxis(resultAxisXA);
    autoSeriesA->attachAxis(resultAxisYA);
    designPointA->attachAxis(resultAxisXA);
    designPointA->attachAxis(resultAxisYA);
    autoSeriesB->attachAxis(resultAxisXB);
    autoSeriesB->attachAxis(resultAxisYB);
    designPointB->attachAxis(resultAxisXB);
    designPointB->attachAxis(resultAxisYB);
    QPen pen;
    pen.setStyle(Qt::DashLine);
    pen.setColor(Qt::red);
    pen.setWidth(2);
    autoSeriesA->setPen(pen);
    autoSeriesB->setPen(pen);

    designChartA->legend()->setVisible(false);
    resultChartA->legend()->setVisible(false);
    resultChartB->legend()->setVisible(false);


    designVLayout->addLayout(designHLayoutA);
    designVLayout->addLayout(designHLayoutB);

    designHLayoutB->addLayout(designVLayoutB1);
    designHLayoutB->addLayout(designHLayoutB2);

    designHLayoutA->addWidget(airfoilListBox);
    designHLayoutA->addWidget(designChartViewA);

    designVLayoutB1->addWidget(designSettingBox);
    designVLayoutB1->addWidget(airfoilToolBox);
    designHLayoutB2->addWidget(resultChartViewA);
    designHLayoutB2->addWidget(resultChartViewB);





    seriesTypeCombobox = new QComboBox(airfoilDesignWidget);
    alphaCombobox = new QComboBox(airfoilDesignWidget);
    setNameEdit = new QLineEdit(airfoilDesignWidget);
    cstRatioSpinBox = new QDoubleSpinBox(airfoilDesignWidget);

    setNameLabel = new QLabel(airfoilDesignWidget);
    seriesNameLabel = new QLabel(airfoilDesignWidget);
    alphaNameLabel = new QLabel(airfoilDesignWidget);
    cstRatioNameLabel = new QLabel(airfoilDesignWidget);

    xLabelTextA = new QLabel(airfoilDesignWidget);
    yLabelTextA = new QLabel(airfoilDesignWidget);
    xLabelTextB = new QLabel(airfoilDesignWidget);
    yLabelTextB = new QLabel(airfoilDesignWidget);
    xLabelTextC = new QLabel(airfoilDesignWidget);
    yLabelTextC = new QLabel(airfoilDesignWidget);

    designAxisXA->setGridLineVisible(false);
    designAxisYA->setGridLineVisible(false);
    designAxisXA->setVisible(false);
    designAxisYA->setVisible(false);


    // 设置字体大小
    QFont font;
    font.setPointSize(11); // 设置文字大小为
    xLabelTextA->setFont(font);
    yLabelTextA->setFont(font);
    xLabelTextB->setFont(font);
    yLabelTextB->setFont(font);
    xLabelTextC->setFont(font);
    yLabelTextC->setFont(font);




    ReLabel = new QLabel(airfoilDesignWidget);
    MaLabel = new QLabel(airfoilDesignWidget);
    NcriLabel = new QLabel(airfoilDesignWidget);
    MinAlphaLabel = new QLabel(airfoilDesignWidget);

    MaxAlphaLabel = new QLabel(airfoilDesignWidget);
    AlphaStepLabel = new QLabel(airfoilDesignWidget);
    XtrTopLabel = new QLabel(airfoilDesignWidget);
    XtrBotLabel = new QLabel(airfoilDesignWidget);

    ReEdit = new QLineEdit(airfoilDesignWidget);
    MaEdit = new QLineEdit(airfoilDesignWidget);
    NcriEdit = new QLineEdit(airfoilDesignWidget);
    MinAlphaEdit = new QLineEdit(airfoilDesignWidget);
    MaxAlphaEdit = new QLineEdit(airfoilDesignWidget);
    AlphaStepEdit = new QLineEdit(airfoilDesignWidget);
    //saveDesignButton = new QPushButton(airfoilDesignWidget);
    //cancelDesignButton = new QPushButton(airfoilDesignWidget);
    XtrTopEdit = new QLineEdit(airfoilDesignWidget);
    XtrBotEdit = new QLineEdit(airfoilDesignWidget);

    //seriesTypeCombobox->setFixedWidth(100);
    seriesTypeCombobox->setIconSize(QSize(70,20));
    for(int i = 0;i<8;i++){
        seriesTypeCombobox->addItem(QIcon(lineStyleIcon[i]),"");
    }

    cstRatioSpinBox->setMinimum(0.5);
    cstRatioSpinBox->setMaximum(6);
    cstRatioSpinBox->setValue(1.1);


    ReLabel->setText("雷诺数：");
    MaLabel->setText("马赫数：");
    NcriLabel->setText("转捩因子：");
    MinAlphaLabel->setText("最小迎角：");

    MaxAlphaLabel->setText("最大迎角：");
    AlphaStepLabel->setText("迎角步长：");
    XtrTopLabel->setText("上弧转捩位置:");
    XtrBotLabel->setText("下弧转捩位置:");
    //saveDesignButton->setText("保存");
    //cancelDesignButton->setText("还原");

    ReEdit->setText(QString::number(input.Re));
    MaEdit->setText(QString::number(input.Ma));
    NcriEdit->setText(QString::number(input.nCrit));
    MinAlphaEdit->setText(QString::number(input.minAlpha));
    MaxAlphaEdit->setText(QString::number(input.maxAlpha));
    AlphaStepEdit->setText(QString::number(input.alphaStepSize));
    XtrTopEdit->setText(QString::number(input.xtrTop));
    XtrBotEdit->setText(QString::number(input.xtrBot));




    setNameLabel->setText("名称");
    seriesNameLabel->setText("曲线类型");
    alphaNameLabel->setText("迎角");
    cstRatioNameLabel->setText("CST倍数");
    xLabelTextA->setText("  X:");
    yLabelTextA->setText("  Y:");

    xLabelTextB->setText("  X:");
    yLabelTextB->setText("  Y:");

    xLabelTextC->setText("  X:");
    yLabelTextC->setText("  Y:");




    designGLayoutB1->addWidget(ReLabel,0,0,1,1);         designGLayoutB1->addWidget(ReEdit,0,1,1,1);
    designGLayoutB1->addWidget(MaLabel,0,2,1,1);         designGLayoutB1->addWidget(MaEdit,0,3,1,1);
    designGLayoutB1->addWidget(NcriLabel,1,0,1,1);       designGLayoutB1->addWidget(NcriEdit,1,1,1,1);
    designGLayoutB1->addWidget(MinAlphaLabel,2,0,1,1);   designGLayoutB1->addWidget(MinAlphaEdit,2,1,1,1);
    designGLayoutB1->addWidget(MaxAlphaLabel,2,2,1,1);   designGLayoutB1->addWidget(MaxAlphaEdit,2,3,1,1);
    designGLayoutB1->addWidget(AlphaStepLabel,1,2,1,1);  designGLayoutB1->addWidget(AlphaStepEdit,1,3,1,1);
    designGLayoutB1->addWidget(XtrTopLabel,3,0,1,1);     designGLayoutB1->addWidget(XtrTopEdit,3,1,1,1);
    designGLayoutB1->addWidget(XtrBotLabel,3,2,1,1);     designGLayoutB1->addWidget(XtrBotEdit,3,3,1,1);
    //designGLayoutB1->addWidget(saveDesignButton,4,0,1,2);designGLayoutB1->addWidget(cancelDesignButton,4,2,1,2);


    designGLayoutB2->addWidget(alphaNameLabel,0,0,1,1);         designGLayoutB2->addWidget(alphaCombobox,0,1,1,1);
    designGLayoutB2->addWidget(seriesNameLabel,0,2,1,1);        designGLayoutB2->addWidget(seriesTypeCombobox,0,3,1,1);
    designGLayoutB2->addWidget(setNameLabel,1,0,1,1);           designGLayoutB2->addWidget(setNameEdit,1,1,1,1);
    designGLayoutB2->addWidget(cstRatioNameLabel,1,2,1,1);      designGLayoutB2->addWidget(cstRatioSpinBox,1,3,1,1);
    designGLayoutB2->addWidget(xLabelTextA,2,0,1,2);      designGLayoutB2->addWidget(yLabelTextA,2,2,1,2);
    designGLayoutB2->addWidget(xLabelTextB,3,0,1,2);      designGLayoutB2->addWidget(yLabelTextB,3,2,1,2);
    designGLayoutB2->addWidget(xLabelTextC,4,0,1,2);      designGLayoutB2->addWidget(yLabelTextC,4,2,1,2);

    connect(alphaCombobox, QOverload<int>::of(&QComboBox::activated),
            this, &airfoilDisplay::changeAlpha);

    connect(setNameEdit,&QLineEdit::textChanged,this,&airfoilDisplay::changeAirfoilName);
    connect(seriesTypeCombobox, QOverload<int>::of(&QComboBox::activated),
            this, &airfoilDisplay::changeResultPen);

    connect(colorDialog,&QColorDialog::colorSelected,this,&airfoilDisplay::changeAirfoilColor);
}


void airfoilDisplay::initialModifyAirfoilDialogCST(){
    modifyAirfoilDialog = new QDialog();
    gridLayoutA = new QGridLayout(modifyAirfoilDialog);

    chartA = new AirfoilPlot();
    chartA->setBackgroundBrush(Qt::NoBrush);

    chartViewA = chartA;
    chartViewA->setRenderHint(QPainter::Antialiasing);
    chartViewA->installEventFilter(this);
    seriesA = new AirfoilPlotSeries();
    scatterSeries = new AirfoilPlotSeries();
    seriesCstPointA = new AirfoilPlotSeries(true);
    seriesCstPointB = new AirfoilPlotSeries(true);
    chartA->addSeries(seriesA);
    chartA->addSeries(scatterSeries);
    chartA->addSeries(seriesCstPointA);
    chartA->addSeries(seriesCstPointB);
    chartViewA->setMinimumHeight(400);
    chartViewA->setMinimumWidth(700);



    axisXA = new AirfoilPlotAxis;
    axisYA = new AirfoilPlotAxis;

    axisXA->setRange(-0.1,1.1);
    axisYA->setRange(-0.25,0.25);
    chartA->addAxis(axisXA,Qt::AlignBottom);
    chartA->addAxis(axisYA,Qt::AlignLeft);

    seriesA->attachAxis(axisXA);
    seriesA->attachAxis(axisYA);
    scatterSeries->attachAxis(axisXA);
    scatterSeries->attachAxis(axisYA);
    seriesCstPointA->attachAxis(axisXA);
    seriesCstPointA->attachAxis(axisYA);
    seriesCstPointB->attachAxis(axisXA);
    seriesCstPointB->attachAxis(axisYA);
    chartA->legend()->setVisible(false);
    chartA->setRangeDragEnabled(false);


    saveAirfoilButton = new QPushButton(modifyAirfoilDialog);
    //restoreAirfoilButton = new QPushButton(modifyAirfoilDialog);
    cancelAirfoilButton = new QPushButton(modifyAirfoilDialog);
    saveAirfoilButton->setText("保存");
    cancelAirfoilButton->setText("还原");
    gridLayoutA->addWidget(chartViewA,0,0,3,3);
    gridLayoutA->addWidget(saveAirfoilButton,3,0,1,1);
    //gridLayoutA->addWidget(restoreAirfoilButton,3,1,1,1);
    gridLayoutA->addWidget(cancelAirfoilButton,3,2,1,1);
    connect(saveAirfoilButton,&QPushButton::clicked,this,&airfoilDisplay::saveAirfoilData);
    connect(cancelAirfoilButton,&QPushButton::clicked,this,&airfoilDisplay::resetAirfoilData);
}
void airfoilDisplay::initialCheckCSTDialog(){
    checkCSTDialog = new QDialog();
    checkGridLayout = new QGridLayout(checkCSTDialog);

    chartF = new AirfoilPlot();
    chartG = new AirfoilPlot();
    chartViewF = chartF;
    chartViewG = chartG;

    airfoilSeriesF = new AirfoilPlotSeries();
    airfoilSeriesG = new AirfoilPlotSeries();

    upperToleranceSeries = new AirfoilPlotSeries();
    lowerToleranceSeries = new AirfoilPlotSeries();

    checkXAxis = new AirfoilPlotAxis;
    checkYAxis = new AirfoilPlotAxis;

    checkValueXAxis = new AirfoilPlotAxis;
    checkValueYAxis = new AirfoilPlotAxis;

    checkXAxis->setTickCount(int(MINAXIS[7]));
    checkYAxis->setTickCount(int(MAXAXIS[7]));
    checkXAxis->setRange(-0.1,1.1);
    checkYAxis->setRange(-0.3,0.3);

    checkValueXAxis->setTickCount(int(MINAXIS[7]));
    checkValueYAxis->setTickCount(int(MAXAXIS[7]));
    checkValueXAxis->setRange(-0.1,1.1);
    checkValueYAxis->setRange(-0.003,0.003);


    chartViewF->setMinimumSize(500,300);
    chartViewG->setMinimumSize(500,300);

    chartF->addAxis(checkXAxis,Qt::AlignBottom);
    chartF->addAxis(checkYAxis,Qt::AlignLeft);

    chartG->addAxis(checkValueXAxis,Qt::AlignBottom);
    chartG->addAxis(checkValueYAxis,Qt::AlignLeft);


    CSTNumBox = new QSpinBox(checkCSTDialog);



    CSTNumBox->setMinimum(6);
    CSTNumBox->setSingleStep(1);

    checkGridLayout->addWidget(chartViewF,0,0,2,2);
    checkGridLayout->addWidget(chartViewG,0,2,2,2);
;
    checkGridLayout->addWidget(CSTNumBox,2,0,1,1);




    QColor color1(0,245,125);
    QColor color2(245,0,0);
    airfoilSeriesF->setPen(color1);
    airfoilSeriesG->setPen(color2);

    upperToleranceSeries->setPen(color1);
    lowerToleranceSeries->setPen(color2);

    chartF->addSeries(airfoilSeriesF);
    chartF->addSeries(airfoilSeriesG);

    chartG->addSeries(upperToleranceSeries);
    chartG->addSeries(lowerToleranceSeries);



    airfoilSeriesF->attachAxis(checkXAxis);
    airfoilSeriesF->attachAxis(checkYAxis);

    airfoilSeriesG->attachAxis(checkXAxis);
    airfoilSeriesG->attachAxis(checkYAxis);

    upperToleranceSeries->attachAxis(checkValueXAxis);
    upperToleranceSeries->attachAxis(checkValueYAxis);

    lowerToleranceSeries->attachAxis(checkValueXAxis);
    lowerToleranceSeries->attachAxis(checkValueYAxis);
    chartF->legend()->setVisible(false);
    chartG->legend()->setVisible(false);


    //connect(importAirfoilButton,SIGNAL(clicked()),this,SLOT(showLibary()));
    connect(CSTNumBox,SIGNAL(valueChanged(int)),this,SLOT(drawCheckCSTAirfoil()));

}
void airfoilDisplay::initialAirfoilBlendingDialog(){
    blendingDialog = new QDialog();
    blendingGLayout = new QGridLayout(blendingDialog);
    blendingChart = new AirfoilPlot();
    blendingView = blendingChart;
    blendingSeriesA = new AirfoilPlotSeries;
    blendingSeriesB = new AirfoilPlotSeries;
    blendingSeriesC = new AirfoilPlotSeries;
    blendingAxisX = new AirfoilPlotAxis;
    blendingAxisY = new AirfoilPlotAxis;

    blendingSlider = new QSlider(Qt::Horizontal);

    blendingTextLabelA = new QLabel();
    blendingTextLabelB = new QLabel();
    blendingTextLabelC = new QLabel();
    blendingValueLabelA = new QLabel();
    blendingValueLabelB = new QLabel();
    blendingCheckA = new QCheckBox();
    blendingCheckB = new QCheckBox();
    blendingCheckC = new QCheckBox();
    blendingComboboxA = new QComboBox();
    blendingComboboxB = new QComboBox();
    blendingSaveButton = new QPushButton();
    blendingCancelButton = new QPushButton();

    QFont f;
    f.setPointSize(7);   // 默认一般是 10 或 11
    blendingTextLabelA->setFont(f);
    blendingTextLabelB->setFont(f);
    blendingTextLabelC->setFont(f);

    blendingChart->addSeries(blendingSeriesA);
    blendingChart->addSeries(blendingSeriesB);
    blendingChart->addSeries(blendingSeriesC);
    blendingChart->addAxis(blendingAxisX,Qt::AlignBottom);
    blendingChart->addAxis(blendingAxisY,Qt::AlignLeft);
    blendingChart->legend()->setVisible(false);
    blendingSeriesA->attachAxis(blendingAxisX);
    blendingSeriesA->attachAxis(blendingAxisY);
    blendingSeriesB->attachAxis(blendingAxisX);
    blendingSeriesB->attachAxis(blendingAxisY);
    blendingSeriesC->attachAxis(blendingAxisX);
    blendingSeriesC->attachAxis(blendingAxisY);

    blendingCheckA->setProperty("index",0);
    blendingCheckB->setProperty("index",1);
    blendingCheckC->setProperty("index",2);
    blendingCheckA->setChecked(true);
    blendingCheckB->setChecked(true);
    blendingCheckC->setChecked(true);

    blendingSeriesA->setColor(Qt::red);
    blendingSeriesB->setColor(Qt::green);
    QPen pen;
    pen.setWidth(2);
    pen.setStyle(Qt::DashLine);
    pen.setColor(Qt::black);
    blendingSeriesC->setPen(pen);

    blendingAxisX->setVisible(false);
    blendingAxisY->setVisible(false);
    blendingAxisX->setGridLineVisible(false);
    blendingAxisY->setGridLineVisible(false);




    blendingValueLabelA->setAlignment(Qt::AlignRight);
    blendingValueLabelB->setAlignment(Qt::AlignLeft);
    blendingDialog->setWindowTitle("翼型融合");

    blendingAxisX->setRange(-0.1,1.1);
    blendingAxisY->setRange(-0.25,0.25);
    blendingCheckA->setToolTip("隐藏/显示");
    blendingCheckB->setToolTip("隐藏/显示");
    blendingCheckC->setToolTip("隐藏/显示");

    blendingValueLabelA->setText("0.5");
    blendingValueLabelB->setText("0.5");
    blendingComboboxA->addItem("选择翼型");
    blendingComboboxB->addItem("选择翼型");


    blendingSaveButton->setText("导入");
    blendingCancelButton->setText("取消");
    blendingSlider->setMinimum(0);
    blendingSlider->setMaximum(100);
    blendingSlider->setValue(50);
    blendingDialog->setMinimumSize(1000,600);


    blendingGLayout->addWidget(blendingView,0,0,5,6);
    blendingGLayout->addWidget(blendingCheckA,5,0,1,5);
    blendingGLayout->addWidget(blendingCheckB,6,0,1,5);
    blendingGLayout->addWidget(blendingCheckC,7,0,1,5);
    blendingGLayout->addWidget(blendingTextLabelA,5,1,1,5);
    blendingGLayout->addWidget(blendingTextLabelB,6,1,1,5);
    blendingGLayout->addWidget(blendingTextLabelC,7,1,1,5);

    blendingGLayout->addWidget(blendingComboboxA,8,0,1,1);
    blendingGLayout->addWidget(blendingValueLabelA,8,1,1,1);
    blendingGLayout->addWidget(blendingSlider,8,2,1,2);
    blendingGLayout->addWidget(blendingValueLabelB,8,4,1,1);
    blendingGLayout->addWidget(blendingComboboxB,8,5,1,1);
    blendingGLayout->addWidget(blendingSaveButton,9,0,1,3);
    blendingGLayout->addWidget(blendingCancelButton,9,3,1,3);
    // 连接 blendingComboboxA 的 currentIndexChanged 信号到槽函数 changeBlendingAirfoilA
    connect(blendingComboboxA, QOverload<int>::of(&QComboBox::activated),
            this, &airfoilDisplay::changeBlendingAirfoilA);

    // 连接 blendingComboboxB 的 currentIndexChanged 信号到槽函数 changeBlendingAirfoilB
    connect(blendingComboboxB, QOverload<int>::of(&QComboBox::activated),
            this, &airfoilDisplay::changeBlendingAirfoilB);
    connect(blendingSlider,&QSlider::valueChanged,this,&airfoilDisplay::updateBlendingAirfoil);
    connect(blendingSaveButton,&QPushButton::clicked,this,&airfoilDisplay::addBlendingAirfoil);
    connect(blendingCheckA,&QCheckBox::clicked,this,&airfoilDisplay::hideBlendingAirfoil);
    connect(blendingCheckB,&QCheckBox::clicked,this,&airfoilDisplay::hideBlendingAirfoil);
    connect(blendingCheckC,&QCheckBox::clicked,this,&airfoilDisplay::hideBlendingAirfoil);
    blendingDialog->setLayout(blendingGLayout);

}
void airfoilDisplay::initialInterChartMenu(){
    interChartMenu = new QMenu(dragInterViewA);
    QAction *action1 = new QAction("升力系数曲线",interChartMenu);
    QAction *action2 = new QAction("阻力系数曲线",interChartMenu);
    QAction *action3 = new QAction("升阻比曲线",interChartMenu);
    QAction *action4 = new QAction("功率因子曲线",interChartMenu);
    QAction *action5 = new QAction("力矩曲线",interChartMenu);
    QAction *action6 = new QAction("极曲线",interChartMenu);

    action1->setProperty("actions",0);
    action2->setProperty("actions",1);
    action3->setProperty("actions",2);
    action4->setProperty("actions",3);
    action5->setProperty("actions",4);
    action6->setProperty("actions",5);

    action1->setCheckable(true);
    action2->setCheckable(true);
    action3->setCheckable(true);
    action4->setCheckable(true);
    action5->setCheckable(true);
    action6->setCheckable(true);
    interActionArray.append(action1);
    interActionArray.append(action2);
    interActionArray.append(action3);
    interActionArray.append(action4);
    interActionArray.append(action5);
    interActionArray.append(action6);

    interChartMenu->addAction(action1);
    interChartMenu->addAction(action2);
    interChartMenu->addAction(action3);
    interChartMenu->addAction(action4);
    interChartMenu->addAction(action5);
    interChartMenu->addAction(action6);

    connect(action1,&QAction::triggered,this,&airfoilDisplay::changeInterResultType);
    connect(action2,&QAction::triggered,this,&airfoilDisplay::changeInterResultType);
    connect(action3,&QAction::triggered,this,&airfoilDisplay::changeInterResultType);
    connect(action4,&QAction::triggered,this,&airfoilDisplay::changeInterResultType);
    connect(action5,&QAction::triggered,this,&airfoilDisplay::changeInterResultType);
    connect(action6,&QAction::triggered,this,&airfoilDisplay::changeInterResultType);


}
void airfoilDisplay::initialDesignChartMenu(){
    designChartMenu = new QMenu(designChartViewA);
    QAction *action0 = new QAction("显示网格",designChartMenu);
    QAction *action1 = new QAction("隐藏翼型信息",designChartMenu);
    QAction *action2 = new QAction("等比例显示",designChartMenu);
    QAction *action3 = new QAction("修改CST阶数",designChartMenu);
    QAction *action4 = new QAction("隐藏控制点",designChartMenu);
    QAction *action5 = new QAction("隐藏转捩位置",designChartMenu);
    QAction *action6 = new QAction("隐藏边界层",designChartMenu);
    QAction *action7 = new QAction("翼型融合",designChartMenu);
    QAction *action8 = new QAction("保存翼型",designChartMenu);


    action0->setProperty("action",0);
    action1->setProperty("action",1);
    action2->setProperty("action",2);
    action3->setProperty("action",3);
    action4->setProperty("action",4);
    action5->setProperty("action",5);
    action6->setProperty("action",6);

    action0->setCheckable(true);
    action1->setCheckable(true);
    action2->setCheckable(true);
    action3->setCheckable(true);
    action4->setCheckable(true);
    action5->setCheckable(true);
    action7->setCheckable(true);
    action6->setChecked(true);

    designActionArray.append(action0);
    designActionArray.append(action1);
    designActionArray.append(action2);
    designActionArray.append(action3);
    designActionArray.append(action4);
    designActionArray.append(action5);
    designActionArray.append(action6);
    designActionArray.append(action7);
    designActionArray.append(action8);


    designChartMenu->addAction(action0);
    designChartMenu->addAction(action1);
    designChartMenu->addAction(action2);
    designChartMenu->addSeparator();
    designChartMenu->addAction(action3);
    designChartMenu->addAction(action4);

    designChartMenu->addSeparator();
    designChartMenu->addAction(action5);
    designChartMenu->addAction(action6);
    designChartMenu->addSeparator();
    designChartMenu->addAction(action7);
    designChartMenu->addAction(action8);

    connect(action0,&QAction::triggered,this,&airfoilDisplay::changeDesignChartType);
    connect(action1,&QAction::triggered,this,&airfoilDisplay::changeDesignChartType);
    connect(action6,&QAction::triggered,this,&airfoilDisplay::changeDesignChartType);
    connect(action3,&QAction::triggered,this,&airfoilDisplay::changeDesignChartType);
    connect(action4,&QAction::triggered,this,&airfoilDisplay::changeDesignChartType);
    connect(action5,&QAction::triggered,this,&airfoilDisplay::changeDesignChartType);
    connect(action7,&QAction::triggered,this,&airfoilDisplay::showAirfoilBlendingDialog);
    connect(action8,&QAction::triggered,this,&airfoilDisplay::showAirfoilOutputTable);
    connect(action3,&QAction::triggered,this,&airfoilDisplay::showCheckAirfoilDialog);


}
void airfoilDisplay::initialRChartMenu(){
    rChartAMenu = new QMenu(resultChartViewA);
    QAction *rActionA1; QAction *rActionB1;
    QAction *rActionA2; QAction *rActionB2;
    QAction *rActionA3; QAction *rActionB3;
    QAction *rActionA4; QAction *rActionB4;
    QAction *rActionA5; QAction *rActionB5;
    QAction *rActionA6; QAction *rActionB6;
    QAction *rActionA7; QAction *rActionB7;
    QAction *rActionA8; QAction *rActionB8;
    QAction *rActionA9; QAction *rActionB9;
    rActionA1 = new QAction("升力系数曲线",rChartAMenu);
    rActionA1->setProperty("actions",1);
    rActionA2 = new QAction("阻力系数曲线",rChartAMenu);
    rActionA2->setProperty("actions",2);
    rActionA3 = new QAction("升阻比曲线",rChartAMenu);
    rActionA3->setProperty("actions",3);
    rActionA4 = new QAction("功率因子曲线",rChartAMenu);
    rActionA4->setProperty("actions",4);
    rActionA5 = new QAction("力矩曲线",rChartAMenu);
    rActionA5->setProperty("actions",5);
    rActionA6 = new QAction("极曲线",rChartAMenu);
    rActionA6->setProperty("actions",6);
    rActionA7 = new QAction("数据导出",rChartAMenu);
    rActionA7->setProperty("actions",7);
    rActionA8 = new QAction("添加点",rChartAMenu);
    rActionA8->setProperty("actions",8);
    rActionA9 = new QAction("删除点",rChartAMenu);
    rActionA9->setProperty("actions",9);

    connect(rActionA1,&QAction::triggered,this,&airfoilDisplay::changeResultTypeA);
    connect(rActionA2,&QAction::triggered,this,&airfoilDisplay::changeResultTypeA);
    connect(rActionA3,&QAction::triggered,this,&airfoilDisplay::changeResultTypeA);
    connect(rActionA4,&QAction::triggered,this,&airfoilDisplay::changeResultTypeA);
    connect(rActionA5,&QAction::triggered,this,&airfoilDisplay::changeResultTypeA);
    connect(rActionA6,&QAction::triggered,this,&airfoilDisplay::changeResultTypeA);
    connect(rActionA7,&QAction::triggered,this,&airfoilDisplay::changeResultTypeA);
    connect(rActionA8,&QAction::triggered,this,&airfoilDisplay::addDataPointA);
    connect(rActionA9,&QAction::triggered,this,&airfoilDisplay::clearDataPointA);

    rActionA1->setCheckable(true);
    rActionA2->setCheckable(true);
    rActionA3->setCheckable(true);
    rActionA4->setCheckable(true);
    rActionA5->setCheckable(true);
    rActionA6->setCheckable(true);
    rActionA7->setCheckable(true);

    rChartAMenu->addAction(rActionA1);
    rChartAMenu->addAction(rActionA2);
    rChartAMenu->addAction(rActionA3);
    rChartAMenu->addAction(rActionA4);
    rChartAMenu->addAction(rActionA5);
    rChartAMenu->addAction(rActionA6);
    rChartAMenu->addAction(rActionA7);
    rChartAMenu->addAction(rActionA8);
    rChartAMenu->addAction(rActionA9);

    rActionArray.append(rActionA1);
    rActionArray.append(rActionA2);
    rActionArray.append(rActionA3);
    rActionArray.append(rActionA4);
    rActionArray.append(rActionA5);
    rActionArray.append(rActionA6);
    rActionArray.append(rActionA7);
    rActionArray.append(rActionA8);
    rActionArray.append(rActionA9);






    rChartBMenu = new QMenu(resultChartViewB);
    rActionB1 = new QAction("升力系数曲线",rChartBMenu);
    rActionB1->setProperty("actions",1);
    rActionB2 = new QAction("阻力系数曲线",rChartBMenu);
    rActionB2->setProperty("actions",2);
    rActionB3 = new QAction("升阻比曲线",rChartBMenu);
    rActionB3->setProperty("actions",3);
    rActionB4 = new QAction("功率因子曲线",rChartBMenu);
    rActionB4->setProperty("actions",4);
    rActionB5 = new QAction("力矩曲线",rChartBMenu);
    rActionB5->setProperty("actions",5);
    rActionB6 = new QAction("压力分布曲线",rChartBMenu);
    rActionB6->setProperty("actions",6);
    rActionB7 = new QAction("数据导出",rChartBMenu);
    rActionB7->setProperty("actions",7);
    rActionB8 = new QAction("添加点",rChartBMenu);
    rActionB8->setProperty("actions",8);
    rActionB9 = new QAction("删除点",rChartBMenu);
    rActionB9->setProperty("actions",9);
    connect(rActionB1,&QAction::triggered,this,&airfoilDisplay::changeResultTypeB);
    connect(rActionB2,&QAction::triggered,this,&airfoilDisplay::changeResultTypeB);
    connect(rActionB3,&QAction::triggered,this,&airfoilDisplay::changeResultTypeB);
    connect(rActionB4,&QAction::triggered,this,&airfoilDisplay::changeResultTypeB);
    connect(rActionB5,&QAction::triggered,this,&airfoilDisplay::changeResultTypeB);
    connect(rActionB6,&QAction::triggered,this,&airfoilDisplay::changeResultTypeB);
    connect(rActionB7,&QAction::triggered,this,&airfoilDisplay::changeResultTypeB);
    connect(rActionB8,&QAction::triggered,this,&airfoilDisplay::addDataPointB);
    connect(rActionB9,&QAction::triggered,this,&airfoilDisplay::clearDataPointB);


    rActionB1->setCheckable(true);
    rActionB2->setCheckable(true);
    rActionB3->setCheckable(true);
    rActionB4->setCheckable(true);
    rActionB5->setCheckable(true);
    rActionB6->setCheckable(true);
    rActionB7->setCheckable(true);

    rChartBMenu->addAction(rActionB1);
    rChartBMenu->addAction(rActionB2);
    rChartBMenu->addAction(rActionB3);
    rChartBMenu->addAction(rActionB4);
    rChartBMenu->addAction(rActionB5);
    rChartBMenu->addAction(rActionB6);
    rChartBMenu->addAction(rActionB7);
    rChartBMenu->addAction(rActionB8);
    rChartBMenu->addAction(rActionB9);

    rBctionArray.append(rActionB1);
    rBctionArray.append(rActionB2);
    rBctionArray.append(rActionB3);
    rBctionArray.append(rActionB4);
    rBctionArray.append(rActionB5);
    rBctionArray.append(rActionB6);
    rBctionArray.append(rActionB7);
    rBctionArray.append(rActionB8);
    rBctionArray.append(rActionB9);



}
void airfoilDisplay::initialAirfoilLibaries(){
    airfoilList = new airfoilLibary();
    connect(airfoilList,&airfoilLibary::buttonClicked,this,&airfoilDisplay::useAirfoilLibaries);

    airfoilList->initial();

}
void airfoilDisplay::initialAnalyse(){
    analyse = new airfoilAnalyse();
    QVector<double>axisRange;
    for(int i = 0; i < 8;i++)
        axisRange.append(MINAXIS[i]);
    for(int i = 0; i < 8;i++)
        axisRange.append(MAXAXIS[i]);

    analyse->initialSetting(axisRange);

    analyse->initialAirfoilLibaryC(airfoilList->AirfoilB,airfoilList->listLengthB,airfoilList->airfoilArrayB);
    analyse->initialAirfoilLibaryB(airfoilList->AirfoilA,airfoilList->listLengthA,airfoilList->airfoilArrayA);
    connect(this,&airfoilDisplay::emitMyChoiceAirfoil,analyse,&airfoilAnalyse::updateAirfoilArrayA);
    connect(analyse,&airfoilAnalyse::progressUpdated,this,&airfoilDisplay::changeAnalyseProgressBar);
}
void airfoilDisplay::initialAirfoilOutput(){
    foilOutput = new airfoilOutput();//
}
void airfoilDisplay::initialInterDragWidget(){
    dragInterWidget = new QWidget();
    dragSettingVLayout = new QVBoxLayout();
    dragSettingBox = new QGroupBox(dragInterWidget);
    dragInterHLayout = new QHBoxLayout();
    dragViewVlayout = new QVBoxLayout();
    QHBoxLayout *layout = new QHBoxLayout();
    dragSettingGLayout = new QGridLayout(dragSettingBox);

    solutionModelButtonA = new QPushButton("计算指定翼型",dragInterWidget);
    solutionModelButtonB = new QPushButton("计算全部翼型",dragInterWidget);

    airfoilLabel = new QLabel("翼型名称",dragInterWidget);airfoilCombox = new QComboBox(dragInterWidget);
    minAlphaLabel = new QLabel("最小迎角",dragInterWidget);minAlphaEdit = new QLineEdit("-10",dragInterWidget);
    maxAlphaLabel = new QLabel("最大迎角",dragInterWidget);maxAlphaEdit = new QLineEdit("18",dragInterWidget);
    stepAlphaLabel = new QLabel("迎角步长",dragInterWidget);stepAlphaEdit = new QLineEdit("1",dragInterWidget);
    minReLabel = new QLabel("最小雷诺数",dragInterWidget);minReEdit = new QLineEdit("5000",dragInterWidget);
    maxReLabel = new QLabel("最大雷诺数",dragInterWidget);maxReEdit = new QLineEdit("1500000",dragInterWidget);
    stepReLabel = new QLabel("雷诺数步长",dragInterWidget);stepReEdit = new QLineEdit("5000",dragInterWidget);
    maLabel = new QLabel("马赫数",dragInterWidget);maEdit = new QLineEdit("0",dragInterWidget);
    ncriLabel = new QLabel("转捩判定因子",dragInterWidget);ncriEdit = new QLineEdit("9",dragInterWidget);
    iterlimLabel = new QLabel("Xfoil迭代次数",dragInterWidget);iterlimEdit = new QLineEdit("150",dragInterWidget);
    threadNumLabel = new QLabel("线程数量",dragInterWidget);threadNumEdit = new QLineEdit("10",dragInterWidget);
    interMethodLabel = new QLabel("插值模型",dragInterWidget);interMethodCombox = new QComboBox(dragInterWidget);
    iterTextEdit = new QTextEdit();
    dragInterChartA = new AirfoilPlot();
    dragInterChartB = new AirfoilPlot();
    dragInterViewA = dragInterChartA;
    dragInterViewB = dragInterChartB;
    dragInterSeries = new AirfoilPlotSeries();
    dragInterAxisXA = new AirfoilPlotAxis();
    dragInterAxisYA = new AirfoilPlotAxis();
    dragInterAxisXB = new AirfoilPlotAxis();
    dragInterAxisYB = new AirfoilPlotAxis();
    dragInterChartA->addAxis(dragInterAxisXA,Qt::AlignBottom);
    dragInterChartA->addAxis(dragInterAxisYA,Qt::AlignLeft);
    dragInterChartB->addAxis(dragInterAxisXB,Qt::AlignBottom);
    dragInterChartB->addAxis(dragInterAxisYB,Qt::AlignLeft);
    dragInterChartB->addSeries(dragInterSeries);
    dragInterSeries->attachAxis(dragInterAxisXB);
    dragInterSeries->attachAxis(dragInterAxisYB);
    dragInterViewA->setRenderHint(QPainter::Antialiasing);
    dragInterViewB->setRenderHint(QPainter::Antialiasing);


    QPen pen;
    pen.setColor(Qt::black);
    pen.setWidth(2);
    dragInterSeries->setPen(pen);
    dragInterAxisXB->setRange(-0.1,1.1);
    dragInterAxisYB->setRange(-0.25,0.25);

    dragSettingBox->setFixedWidth(300);
    iterTextEdit->setFixedWidth(300);
    iterTextEdit->setFixedHeight(300);
    dragInterViewB->setFixedHeight(300);

    solutionModelButtonA->setEnabled(false);
    dragInterChartA->legend()->setVisible(false);
    dragInterChartB->legend()->setVisible(false);
    dragInterAxisXB->setVisible(false);
    dragInterAxisYB->setVisible(false);





    dragSettingGLayout->addWidget(airfoilLabel,0,0,1,1);
    dragSettingGLayout->addWidget(airfoilCombox,0,1,1,1);
    dragSettingGLayout->addWidget(minAlphaLabel,1,0,1,1);
    dragSettingGLayout->addWidget(minAlphaEdit,1,1,1,1);
    dragSettingGLayout->addWidget(maxAlphaLabel,2,0,1,1);
    dragSettingGLayout->addWidget(maxAlphaEdit,2,1,1,1);
    dragSettingGLayout->addWidget(stepAlphaLabel,3,0,1,1);
    dragSettingGLayout->addWidget(stepAlphaEdit,3,1,1,1);
    dragSettingGLayout->addWidget(minReLabel,4,0,1,1);
    dragSettingGLayout->addWidget(minReEdit,4,1,1,1);
    dragSettingGLayout->addWidget(maxReLabel,5,0,1,1);
    dragSettingGLayout->addWidget(maxReEdit,5,1,1,1);
    dragSettingGLayout->addWidget(stepReLabel,6,0,1,1);
    dragSettingGLayout->addWidget(stepReEdit,6,1,1,1);
    dragSettingGLayout->addWidget(maLabel,7,0,1,1);
    dragSettingGLayout->addWidget(maEdit,7,1,1,1);
    dragSettingGLayout->addWidget(ncriLabel,8,0,1,1);
    dragSettingGLayout->addWidget(ncriEdit,8,1,1,1);
    dragSettingGLayout->addWidget(iterlimLabel,9,0,1,1);
    dragSettingGLayout->addWidget(iterlimEdit,9,1,1,1);
    dragSettingGLayout->addWidget(threadNumLabel,10,0,1,1);
    dragSettingGLayout->addWidget(threadNumEdit,10,1,1,1);
    dragSettingGLayout->addWidget(interMethodLabel,11,0,1,1);
    dragSettingGLayout->addWidget(interMethodCombox,11,1,1,1);



    layout->addWidget(solutionModelButtonA);
    layout->addWidget(solutionModelButtonB);
    dragViewVlayout->addWidget(dragInterViewA);
    dragViewVlayout->addWidget(dragInterViewB);
    dragSettingVLayout->addLayout(layout);
    dragSettingVLayout->addWidget(dragSettingBox);
    dragSettingVLayout->addWidget(iterTextEdit);

    dragInterHLayout->addLayout(dragSettingVLayout);
    dragInterHLayout->addLayout(dragViewVlayout);
    dragInterWidget->setLayout(dragInterHLayout);
    connect(minAlphaEdit,&QLineEdit::editingFinished,this,&airfoilDisplay::updateAnalyseSetting);
    connect(maxAlphaEdit,&QLineEdit::editingFinished,this,&airfoilDisplay::updateAnalyseSetting);
    connect(stepAlphaEdit,&QLineEdit::editingFinished,this,&airfoilDisplay::updateAnalyseSetting);
    connect(minReEdit,&QLineEdit::editingFinished,this,&airfoilDisplay::updateAnalyseSetting);
    connect(maxReEdit,&QLineEdit::editingFinished,this,&airfoilDisplay::updateAnalyseSetting);
    connect(stepReEdit,&QLineEdit::editingFinished,this,&airfoilDisplay::updateAnalyseSetting);
    connect(ncriEdit,&QLineEdit::editingFinished,this,&airfoilDisplay::updateAnalyseSetting);
    connect(maEdit,&QLineEdit::editingFinished,this,&airfoilDisplay::updateAnalyseSetting);
    connect(iterlimEdit,&QLineEdit::editingFinished,this,&airfoilDisplay::updateAnalyseSetting);
    connect(threadNumEdit,&QLineEdit::editingFinished,this,&airfoilDisplay::updateAnalyseSetting);
    connect(solutionModelButtonA,&QPushButton::clicked,this,&airfoilDisplay::updateInterButtonState);
    connect(solutionModelButtonB,&QPushButton::clicked,this,&airfoilDisplay::updateInterButtonState);

    connect(airfoilCombox, QOverload<int>::of(&QComboBox::activated),
            this, &airfoilDisplay::drawInterAirfoil);



}
void airfoilDisplay::initialExplorerWidget(){
    explorer = new airfoilExplorer();
    explorer->importAirfoilName(airfoilList->airfoilNameArrayA,airfoilList->airfoilNameArrayB,
                                   airfoilList->airfoilNameArrayC);
    explorer->importAirfoilData(airfoilList->airfoilArrayA,airfoilList->airfoilArrayB,airfoilList->airfoilArrayC);
    explorer->initialAirfoilLib();
    explorer->importAirfoilParameters(airfoilList->AirfoilA,airfoilList->AirfoilB,airfoilList->AirfoilC);


}
void airfoilDisplay::showAnalyseTable(){
    emit emitMyChoiceAirfoil(airfoilArray,airfoilNameArray);
    analyse->showAirfoilChoseWidget();
}
void airfoilDisplay::showAnalyseReTable(){
    emit emitMyChoiceAirfoil(airfoilArray,airfoilNameArray);
    analyse->showMoreReAnalyseWidget();
}
void airfoilDisplay::showLibary(){
    if(airfoilList->initialDone){

        airfoilList->show();
    }
    else
        QMessageBox::information(nullptr,"警告","初始化未完成");
}
void airfoilDisplay::showAirfoilOutputTable(){

    modelType = OUTPUTFOIL;
    foilOutput->buildOutputAirfoilDialog(airfoilNameArray);
    foilOutput->copyFoilArray(airfoilArray);
    //<<airfoilArray;
    connect(foilOutput->foilLibariesButton,&QPushButton::clicked,this,&airfoilDisplay::showLibary);

}
void airfoilDisplay::useAirfoilLibaries(){
    QVector<QVector<double>>Airfoil;
    airfoilDesign design(cstNUM);


    switch (modelType) {
    case NOTHING:

        airfoilList->hide();
        //nameArray.append(airfoilList->Airfoil[airfoilList->listChose].Name);
        airfoilNameArray.append(airfoilList->Airfoil[airfoilList->listChose].Name);
        design.splitAirfoilData(airfoilList->airfoilArray[airfoilList->listChose]);


        Airfoil = design.newAirfoilData;
        switch (readDataType) {
        case DESIGN:
            //defineAirfoil->addNewAirfoil(Airfoil);
            addNewAirfoil(Airfoil);
            break;
        case OPTIMIZATION:

            break;
        case BLENDINGA:

            // blendingAirfoilA = Airfoil;
            // if(!blendingAirfoilA.isEmpty() && !blendingAirfoilB.isEmpty())
            //     blendingGenerateAirfoilButton->setEnabled(true);
            // blendingAirfoilNameLabelA->setText(airfoilList->Airfoil[airfoilList->listChose].Name);
            // addNewAirfoil(Airfoil);
            break;
        case BLENDINGB:

            // blendingAirfoilB = Airfoil;
            // if(!blendingAirfoilA.isEmpty() && !blendingAirfoilB.isEmpty())
            //     blendingGenerateAirfoilButton->setEnabled(true);
            // blendingAirfoilNameLabelB->setText(airfoilList->Airfoil[airfoilList->listChose].Name);
            // addNewAirfoil(Airfoil);
            break;
        default:
            break;
        }


        break;
    case CHECKCST:
        //airfoilList->hide();
        // checkName = airfoilList->Airfoil[airfoilList->listChose].Name;
        // checkAirfoil = airfoilList->airfoilArray[airfoilList->listChose];
        // drawCheckCSTAirfoil();

        break;
    case OUTPUTFOIL:
        airfoilList->hide();
        foilOutput->addFoil(airfoilList->Airfoil[airfoilList->listChose].Name,airfoilList->airfoilArray[airfoilList->listChose]);
        foilOutput->drawFoil();


        break;
    case PROPDESIGN:
        airfoilList->hide();



    default:
        break;
    }
}
void airfoilDisplay::changeDesignChartType(){
    QAction *action = qobject_cast<QAction*>(sender());
    int index = action->property("action").toInt();
    bool state;

    switch (index) {
    case 0:
        toggleGridLines();
        designChartIndex = 0;
        break;
    case 1:
        state = designChartViewA->textItem->isVisible();
        designChartViewA->textItem->setVisible(!state);
        if(!state)
            designActionArray[1]->setText("隐藏翼型信息");
        else
            designActionArray[1]->setText("显示翼型信息");
        designChartIndex = 1;
        break;
    case 2:
        adjustChartViewAspectRatio(designChartViewA);
        designChartIndex = 2;
        break;
    case 4:
        if(designSeriesPointA->isVisible()){
            designSeriesPointA->setVisible(false);
            designSeriesPointB->setVisible(false);
            designScatterSeriesA->setVisible(false);
            designActionArray[4]->setText("显示控制点");
        }else{
            designSeriesPointA->setVisible(true);
            designSeriesPointB->setVisible(true);
            designScatterSeriesA->setVisible(true);
            designActionArray[4]->setText("隐藏控制点");
        }
        designChartIndex = 4;
        break;
    case 5:
        if(transitionPoint->isVisible()){
            transitionPoint->setVisible(false);

            designActionArray[5]->setText("显示转捩点");
        }else{
            transitionPoint->setVisible(true);
            designActionArray[5]->setText("隐藏转捩点");
        }
        designChartIndex = 5;
        break;
    case 6:
        if(xblLowerSeries->isVisible()){
            xblLowerSeries->setVisible(false);
            xblUpperSeries->setVisible(false);
            if (upperAreaSeries)
                upperAreaSeries->setVisible(false);
            if (lowerAreaSeries)
                lowerAreaSeries->setVisible(false);
            designActionArray[6]->setText("显示边界层");
            //designActionArray[7]->setChecked(false);
        }else{
            xblLowerSeries->setVisible(true);
            xblUpperSeries->setVisible(true);
            if (upperAreaSeries)
                upperAreaSeries->setVisible(true);
            if (lowerAreaSeries)
                lowerAreaSeries->setVisible(true);
            designActionArray[6]->setText("隐藏边界层");
            //designActionArray[7]->setChecked(true);
        }
        designChartIndex = 6;
        break;

    default:
        break;
    }

}
void airfoilDisplay::adjustChartViewAspectRatio(AirfoilPlot *chartView) {
    if (!chartView || !chartView->chart()) return;

    AirfoilPlot *chart = chartView->chart();
    AirfoilPlotAxis *xAxis = nullptr;
    AirfoilPlotAxis *yAxis = nullptr;

    // Get the X and Y axes
    foreach (AirfoilPlotAxis* axis, chart->axes()) {
        if (axis->orientation() == Qt::Horizontal) {
            xAxis = axis;
        } else if (axis->orientation() == Qt::Vertical) {
            yAxis = axis;
        }
    }

    if (xAxis && yAxis) {
        // Fixed Y axis range
        qreal yMin = yAxis->min();
        qreal yMax = yAxis->max();
        qreal yRange = yMax - yMin;

        // Calculate the aspect ratio of the view
        qreal viewAspectRatio = static_cast<qreal>(chartView->width()) / chartView->height();

        // Fixed chart aspect ratio based on fixed Y axis range
        qreal chartAspectRatio = viewAspectRatio;

        // Calculate new X axis range to maintain aspect ratio
        qreal newYRange = yRange; // Fixed Y range
        qreal newXRange = newYRange * chartAspectRatio;
        qreal xMin = xAxis->min();
        qreal xMax = xAxis->max();
        qreal xCenter = (xMax + xMin) / 2;
        qreal newXMin = xCenter - newXRange / 2;
        qreal newXMax = xCenter + newXRange / 2;

        // Update X axis range
        xAxis->setRange(newXMin, newXMax);
    }
}
void airfoilDisplay::toggleGridLines() {
    bool xAxisGridVisible = designAxisXA->isGridLineVisible();
    bool yAxisGridVisible = designAxisYA->isGridLineVisible();

    bool newVisibilitx = !xAxisGridVisible; // Toggle based on current state
    bool newVisibility = !yAxisGridVisible; // Toggle based on current state

    designAxisXA->setVisible(newVisibilitx);
    designAxisYA->setVisible(newVisibility);

    designAxisXA->setGridLineVisible(newVisibilitx);
    designAxisYA->setGridLineVisible(newVisibility);
    if(newVisibilitx)
        designActionArray[0]->setText("隐藏网格");
    else
        designActionArray[0]->setText("显示网格");


}
void airfoilDisplay::changeInterResultType(){
    QAction *action = qobject_cast<QAction*>(sender());
    int index = action->property("actions").toInt();
    interActionIndex = index;
    for (AirfoilPlotSeries *series : dragSeriesArray) {
        delete series; // 删除每个 QLineseries* 指针对象，释放内存
    }
    dragSeriesArray.clear(); // 清空容器

    switch (index) {
    case 0:
        for(int i = 0;i<interResultArray.length();i++)
            drawInterClResult(interResultArray[i]);
        updateAxes(dragInterChartA,dragSeriesArray);

        break;
    case 1:
        for(int i = 0;i<interResultArray.length();i++)
            drawInterCdResult(interResultArray[i]);
        updateAxes(dragInterChartA,dragSeriesArray);

        break;
    case 2:
        for(int i = 0;i<interResultArray.length();i++)
            drawInterKResult(interResultArray[i]);
        updateAxes(dragInterChartA,dragSeriesArray);

        break;
    case 3:
        for(int i = 0;i<interResultArray.length();i++)
            drawInterPResult(interResultArray[i]);
        updateAxes(dragInterChartA,dragSeriesArray);

        break;
    case 4:
        for(int i = 0;i<interResultArray.length();i++)
            drawInterCmResult(interResultArray[i]);
        updateAxes(dragInterChartA,dragSeriesArray);

        break;
    case 5:
        for(int i = 0;i<interResultArray.length();i++)
            drawInterRResult(interResultArray[i]);
        updateAxes(dragInterChartA,dragSeriesArray);

        break;
    default:
        break;
    }
}
void airfoilDisplay::changeResultTypeA(){
    QAction *action = qobject_cast<QAction*>(sender());
    int index = action->property("actions").toInt();
    if(index < 7){
        clearDataPointA();
        resultAIndex = index;
        for(int i = 0;i<airfoilSolveModelArray.length();i++){
            drawDesignResult(i);
        }
        updateAxes(resultChartA,resultSeriesA);
    }else{
        exportChartData(resultChartA);

    }
}
void airfoilDisplay::changeResultTypeB(){
    QAction *action = qobject_cast<QAction*>(sender());
    int index = action->property("actions").toInt();
    if(index < 7){
        clearDataPointB();
        resultBIndex = index;
        for(int i = 0;i<airfoilSolveModelArray.length();i++){
            drawDesignResult(i);
        }
        updateAxes(resultChartB,resultSeriesB);
    }else{
        exportChartData(resultChartB);

    }
}
void airfoilDisplay::showDesignChartMenu(){


    for (QAction *act : designChartMenu->actions()) {
        act->setChecked(false);
    }

    designActionArray[designChartIndex]->setChecked(true);
    designChartMenu->exec(QCursor::pos());
}
void airfoilDisplay::showRChartAMenu(){
    for (QAction *act : rChartAMenu->actions()) {
        act->setChecked(false);
    }
    rActionArray[resultAIndex - 1]->setChecked(true);

    rChartAMenu->exec(QCursor::pos());
}
void airfoilDisplay::showRChartBMenu(){
    for (QAction *act : rChartBMenu->actions()) {
        act->setChecked(false);
    }
    rBctionArray[resultBIndex - 1]->setChecked(true);
    rChartBMenu->exec(QCursor::pos());
}
void airfoilDisplay::showInterChartMenu(){
    for (QAction *act : interChartMenu->actions()) {
        act->setChecked(false);
    }
    interActionArray[interActionIndex]->setChecked(true);
    interChartMenu->exec(QCursor::pos());
}
void airfoilDisplay::drawCheckCSTAirfoil(){

    if(!checkAirfoil.isEmpty()){

        int cst = CSTNumBox->value();

        if(airfoilSeriesF->count() != 0)
            airfoilSeriesF->clear();
        if(airfoilSeriesG->count() != 0)
            airfoilSeriesG->clear();
        if(upperToleranceSeries->count() != 0)
            upperToleranceSeries->clear();
        if(lowerToleranceSeries->count() != 0)
            lowerToleranceSeries->clear();
        airfoilDesign design(cst);

        design.buildBenrnstein(checkAirfoil);
        design.buildAirfoilCurve(design.cstParameter);
        cstArray[choiceIndex] = design.cstParameter;
        airfoilDesignModelArray[choiceIndex]->cstNum = cst;




        for(int i = 0; i<checkAirfoil.length();i++)
            airfoilSeriesF->append(checkAirfoil[i][0],checkAirfoil[i][1]);

        for(int i = 0; i<design.newAirfoilData.length();i++)
            airfoilSeriesG->append(design.newAirfoilData[i][0],design.newAirfoilData[i][1]);

        airfoilDesign designB(cst);
        designB.splitAirfoilData(design.newAirfoilData);

        if(design.upperData.length() == designB.upperData.length()){
            for(int i = 0 ; i < design.upperData.length();i++)
                upperToleranceSeries->append(design.upperData[i][0],design.upperData[i][1] - designB.upperData[i][1]);

        }

        if(design.lowerData.length() == designB.lowerData.length()){
            for(int i = 0 ; i < design.lowerData.length();i++)
                lowerToleranceSeries->append(design.lowerData[i][0],design.lowerData[i][1] - designB.lowerData[i][1]);
        }

        //drawDesignAirfoil(choiceIndex);



        airfoilArray[choiceIndex] = design.newAirfoilData;
        updateDesignCSTPoint();
        drawDesignAirfoil(choiceIndex);

        emit emitAirfoilArray(airfoilArray,airfoilNameArray,cstNumArray);
    }

}
void airfoilDisplay::drawInterClResult(const airfoilData &data){
    if(!data.alphaData.isEmpty()){
        AirfoilPlotSeries *series = new AirfoilPlotSeries;
        for(int i = 0;i<data.alphaData.length();i++){
            series->append(data.alphaData[i],data.clData[i]);
        }
        dragInterChartA->addSeries(series);
        series->attachAxis(dragInterAxisXA);
        series->attachAxis(dragInterAxisYA);
        dragSeriesArray.append(series);
    }
}
void airfoilDisplay::drawInterCdResult(const airfoilData &data){
    if(!data.alphaData.isEmpty()){
        AirfoilPlotSeries *series = new AirfoilPlotSeries;
        for(int i = 0;i<data.alphaData.length();i++){
            series->append(data.alphaData[i],data.cdData[i]);
        }
        dragInterChartA->addSeries(series);
        series->attachAxis(dragInterAxisXA);
        series->attachAxis(dragInterAxisYA);
        dragSeriesArray.append(series);
    }
}
void airfoilDisplay::drawInterKResult(const airfoilData &data){
    if(!data.alphaData.isEmpty()){
        AirfoilPlotSeries *series = new AirfoilPlotSeries;
        for(int i = 0;i<data.alphaData.length();i++){
            series->append(data.alphaData[i],data.clData[i] / data.cdData[i]);
        }
        dragInterChartA->addSeries(series);
        series->attachAxis(dragInterAxisXA);
        series->attachAxis(dragInterAxisYA);
        dragSeriesArray.append(series);
    }
}
void airfoilDisplay::drawInterPResult(const airfoilData &data){
    if(!data.alphaData.isEmpty()){
        AirfoilPlotSeries *series = new AirfoilPlotSeries;
        for(int i = 0;i<data.alphaData.length();i++){
            series->append(data.alphaData[i],pow(data.clData[i],1.5) / data.cdData[i]);
        }
        dragInterChartA->addSeries(series);
        series->attachAxis(dragInterAxisXA);
        series->attachAxis(dragInterAxisYA);
        dragSeriesArray.append(series);
    }
}
void airfoilDisplay::drawInterCmResult(const airfoilData &data){
    if(!data.alphaData.isEmpty()){
        AirfoilPlotSeries *series = new AirfoilPlotSeries;
        for(int i = 0;i<data.alphaData.length();i++){
            series->append(data.alphaData[i],data.cmData[i]);
        }
        dragInterChartA->addSeries(series);
        series->attachAxis(dragInterAxisXA);
        series->attachAxis(dragInterAxisYA);
        dragSeriesArray.append(series);
    }
}
void airfoilDisplay::drawInterRResult(const airfoilData &data){
    if(!data.alphaData.isEmpty()){
        AirfoilPlotSeries *series = new AirfoilPlotSeries;
        for(int i = 0;i<data.alphaData.length();i++){
            series->append(data.cdData[i],data.clData[i]);
        }
        dragInterChartA->addSeries(series);
        series->attachAxis(dragInterAxisXA);
        series->attachAxis(dragInterAxisYA);
        dragSeriesArray.append(series);
    }
}
void airfoilDisplay::drawInterAirfoil(){
    int index = airfoilCombox->currentIndex();
    if(dragInterSeries->count()>0)
        dragInterSeries->clear();
    if(airfoilArray.length() >= index + 1)
    for(int i = 0;i<airfoilArray[index].length();i++)
        dragInterSeries->append(airfoilArray[index][i][0],airfoilArray[index][i][1]);
}
void airfoilDisplay::drawAirfoil(){
    if(seriesA->count() > 0){
        seriesA->clear();
        scatterSeries->clear();
        seriesCstPointA->clear();
        seriesCstPointB->clear();;
    }
    for(int i = 0;i<newAirfoil.length();i++){
        seriesA->append(newAirfoil[i][0],newAirfoil[i][1]);
    }


    for(int i = 0; i < cst.length();i++){
        scatterSeries->append(cstPointX[i],cstPointY[i]);
        seriesCstPointA->append(cstPointX[i],cstPointY[i]);
        seriesCstPointB->append(cstPointX[i],cstPointY[i]);
    }
    scatterSeries->append(cstPointX[0],cstPointY[0]);
}
void airfoilDisplay::addBlendingAirfoil(){
    if(!blendingAirfoilData.isEmpty()){
        airfoilNameArray.append("blendingAirfoil");
        addNewAirfoil(blendingAirfoilData);
        blendingDialog->hide();
    }
}
void airfoilDisplay::addNewAirfoil(const QVector<QVector<double>>&airfoil){
    airfoilDesign *airDesign = new airfoilDesign(cstNum);
    
    airDesign->buildBenrnstein(airfoil);

    if(!airDesign->isFinshed()){
        QMessageBox::information(nullptr,"警告","参数化失败，无法进入设计模式");
       
        delete airDesign;
        airfoilNameArray.remove(airfoilIndex);
         
    }
    else{



        cstNumArray.append(6);
        QVector<double>cstx;
        QVector<double>csty;
        cstPointXArray.append(cstx);
        cstPointYArray.append(csty);
        airfoilArray.append(airfoil);
        historyAirfoilArray.append(airfoil);

        airfoilDesignModelArray.append(airDesign);
        resultPenArray.append(0);

        airDesign->computeOtherParameters(airfoil);
        cstArray.append(airDesign->cstParameter);

        //


        airfoilSolve* solvers = new airfoilSolve;
        XFoil *foil = new XFoil;


        airfoilSolveModelArray.append(solvers);
        airfoilXfoilArray.append(foil);

        int r =rand()%255;
        int b =rand()%255;
        int g =rand()%255;

        QColor color(r,b,g);

        airfoilColorArray.append(color);
        QString colorStyle = QString("background-color: %1;").arg(color.name());


        QPushButton *button1 = new QPushButton(airfoilDesignWidget);
        hoverButton *button2 = new hoverButton(airfoilDesignWidget);
        hoverButton *button3 = new hoverButton(airfoilDesignWidget);
        hoverButton *button4 = new hoverButton(airfoilDesignWidget);


        button1->setFixedSize(180,40);
        button1->setText(airfoilNameArray[airfoilIndex]);
        button2->setButtonIcon(checkIcon,QSize(40,40));
        //button3->setMinimumSize(36,36);
        button4->setButtonIcon(removeIcon,QSize(40,40));


        button1->setObjectName("childButton");
        button2->setObjectName("iconButton");
        button3->setObjectName("iconButton");
        button4->setObjectName("iconButton");

        button3->setStyleSheet(colorStyle);


        airfoilButton.append(button1);
        airfoilCheckButton.append(button2);
        airfoilColorButton.append(button3);
        airfoilRemoveButton.append(button4);
        /*
        designGLayoutA1->addWidget(button1,airfoilIndex,0,1,1);
        designGLayoutA1->addWidget(button2,airfoilIndex,1,1,1);
        designGLayoutA1->addWidget(button3,airfoilIndex,2,1,1);
        designGLayoutA1->addWidget(button4,airfoilIndex,3,1,1);
        */
        for (int i = 0; i < airfoilButton.size(); ++i) {
            designGLayoutA1->addWidget(airfoilButton.at(i), i, 0,1,1); // 第一列
            designGLayoutA1->addWidget(airfoilCheckButton.at(i), i, 2,1,1); // 第二列
            designGLayoutA1->addWidget(airfoilColorButton.at(i), i, 3,1,1); // 第二列
            designGLayoutA1->addWidget(airfoilRemoveButton.at(i), i, 4,1,1); // 第二列
            airfoilButton.at(i)->setProperty("airfoil",i);
            airfoilCheckButton.at(i)->setProperty("check",i);
            airfoilColorButton.at(i)->setProperty("color",i);
            airfoilRemoveButton.at(i)->setProperty("remove",i);
        }


        connect(button1,&QPushButton::clicked,this,&airfoilDisplay::choiceAirfoilButton);
        connect(button2,&QPushButton::clicked,this,&airfoilDisplay::showModifyAirfoilDialog);
        connect(button3,&QPushButton::clicked,this,&airfoilDisplay::showColorDialog);
        connect(button4,&QPushButton::clicked,this,&airfoilDisplay::deleteAirfoilDesignButton);


        AirfoilPlotSeries *seriesA = new AirfoilPlotSeries;
        AirfoilPlotSeries *seriesB = new AirfoilPlotSeries;
        seriesA->setName(airfoilNameArray[airfoilIndex]);
        seriesB->setName(airfoilNameArray[airfoilIndex]);
        resultChartA->addSeries(seriesA);
        resultChartB->addSeries(seriesB);
        seriesA->attachAxis(resultAxisXA);
        seriesA->attachAxis(resultAxisYA);
        seriesB->attachAxis(resultAxisXB);
        seriesB->attachAxis(resultAxisYB);

        resultSeriesA.append(seriesA);
        resultSeriesB.append(seriesB);

        choiceIndex = airfoilIndex;

        updateDesignCSTPoint();



        //更新翼型参数信息
        airDesign->computeParameters(airDesign->cstParameter);
        //updateDesignTextView(airDesign);
        //updateDesignTextItem(designChartViewA->getRealSize().width(),designChartViewA->getRealSize().height());


        //2


        updateDesignTextViewFormXfoil(foil);

        updateDesignTextItem(designChartViewA->getRealSize().width(),designChartViewA->getRealSize().height());



        //checkAirfoil = historyAirfoilArray[airfoilIndex];

        setDesignButtonEnabled(airfoilIndex);
        checkAirfoil = airfoil;



        //在未得到结果前，删除转捩点
        if(transitionPoint->count() != 0)
            transitionPoint->clear();
        if(designPointA->count() != 0)
            designPointA->clear();
        if(designPointB->count() != 0)
            designPointB->clear();





        connect(solvers,&airfoilSolve::progressUpdat,this,&airfoilDisplay::changeDesignProgressBar);
        choiceCombobox->addItem(airfoilNameArray[airfoilIndex]);
        airfoilCombox->addItem(airfoilNameArray[airfoilIndex]);

        //翼型优化同步更新
        choiceCombobox->blockSignals(true);
        choiceCombobox->setCurrentIndex(airfoilIndex);
        drawOptIndexAirfoil(airfoilIndex);
        choiceCombobox->blockSignals(false);
        //翼型气动数据
        interDataArray.append(myFile::getInterData(airfoilNameArray[airfoilIndex]));

        //<<interDataArray[airfoilIndex][0].clData;
        //

        drawInterAirfoil();

        if(isSolve)
            solveDesignAirfoil();

        airfoilIndex++;
        airDesign = nullptr;
        solvers = nullptr;


        emit emitAirfoilArray(airfoilArray,airfoilNameArray,cstNumArray);
        emit emitAirfoilInterArray(interDataArray);

    }   
}
void airfoilDisplay::initialModifyAirfoilDialog()
{
    airfoilModifyDialog = new QDialog(this);
    airfoilModifyDialog->setWindowTitle("Airfoil Modification");

    // ===== 控件（注意 parent 是 dialog）=====
    thicknessSpin = new QDoubleSpinBox(airfoilModifyDialog);
    camberSpin    = new QDoubleSpinBox(airfoilModifyDialog);
    thkPosSpin    = new QDoubleSpinBox(airfoilModifyDialog);
    camPosSpin    = new QDoubleSpinBox(airfoilModifyDialog);
    //leRadiusSpin  = new QDoubleSpinBox(airfoilModifyDialog);
    //leBlendSpin   = new QDoubleSpinBox(airfoilModifyDialog);

    auto cfg = [](QDoubleSpinBox* s, double min, double max, double val, int decimals = 4)
    {
        s->setRange(min, max);
        s->setValue(val);
        s->setSingleStep(0.005);
        s->setDecimals(decimals);
    };

    cfg(thicknessSpin, 0.01, 0.5, 0.0,4);
    cfg(camberSpin,    0.01, 0.2, 0.0,4);
    cfg(thkPosSpin,    0.01, 0.9, 0.0,4);
    cfg(camPosSpin,    0.01, 0.9, 0.0,4);
    //cfg(leRadiusSpin,  0.1, 5, 0.0,3);
    //cfg(leBlendSpin,   0.1, 1, 0.1,3);

    // ===== 按钮 =====
    okBtn     = new QPushButton("确认", airfoilModifyDialog);
    cancelBtn = new QPushButton("还原", airfoilModifyDialog);


    // ===== 信号（关键修正）=====
    connect(okBtn, &QPushButton::clicked,
            airfoilModifyDialog, &QDialog::close);

    connect(cancelBtn, &QPushButton::clicked,
            this, &airfoilDisplay::resetAirfoil);

    connect(camberSpin,
            QOverload<double>::of(&QDoubleSpinBox::valueChanged),
            this,
            &airfoilDisplay::modifyCamber);

    connect(thicknessSpin,
            QOverload<double>::of(&QDoubleSpinBox::valueChanged),
            this,
            &airfoilDisplay::modifyThickness);

    connect(thkPosSpin,
            QOverload<double>::of(&QDoubleSpinBox::valueChanged),
            this,
            &airfoilDisplay::modifyThicknessLocation);


    connect(camPosSpin,
            QOverload<double>::of(&QDoubleSpinBox::valueChanged),
            this,
            &airfoilDisplay::modifyCamberLocation);

    /*
    connect(leRadiusSpin,
            QOverload<double>::of(&QDoubleSpinBox::valueChanged),
            this,
            &airfoilDisplay::modifyRadius);


    connect(leBlendSpin,
            QOverload<double>::of(&QDoubleSpinBox::valueChanged),
            this,
            &airfoilDisplay::modifyRadiusInfluence);
            */






    //connect(applyBtn, &QPushButton::clicked,
            //this, &airfoilDisplay::onApplyAirfoilModify);

    // ===== 布局 =====
    QFormLayout *form = new QFormLayout;
    form->addRow("最大厚度", thicknessSpin);
    form->addRow("最大弯度", camberSpin);
    form->addRow("厚度位置", thkPosSpin);
    form->addRow("弯度位置", camPosSpin);
    //form->addRow("前缘半径", leRadiusSpin);
    //form->addRow("半径影响", leBlendSpin);

    QHBoxLayout *btn = new QHBoxLayout;

    btn->addWidget(okBtn);
    btn->addWidget(cancelBtn);

    QVBoxLayout *main = new QVBoxLayout(airfoilModifyDialog);
    main->addLayout(form);
    main->addLayout(btn);

    airfoilModifyDialog->setLayout(main);
}


void airfoilDisplay::initialGa(){

    int cstNum = airfoilDesignModelArray[choiceOPTAirfoilIndex]->cstNum * 2;
    GaSetting.solutionsNum = cstNum;
    exp = new airfoilOptimization(GaSetting,cstArray[choiceOPTAirfoilIndex]);
    exp->initialElite();
    optStep = GaSetting.step;
    workerNum = GaSetting.initialEliteNum / threadNum;

    if(iterateSeries->count()>0)
        iterateSeries->clear();

}
void airfoilDisplay::updateInterButtonState() {
    // 判断哪个按钮被点击，并更新状态

    if (sender() == solutionModelButtonA) {
        solutionModelButtonA->setEnabled(false);  // 禁用A
        solutionModelButtonB->setEnabled(true);   // 启用B
        airfoilCombox->setEnabled(true);

    } else if (sender() == solutionModelButtonB) {
        solutionModelButtonB->setEnabled(false);  // 禁用B
        solutionModelButtonA->setEnabled(true);   // 启用A
        airfoilCombox->setEnabled(false);
    }

}
void airfoilDisplay::updateAnalyseSetting(){
    double minRe = minReEdit->text().toDouble();
    double maxRe = maxReEdit->text().toDouble();
    double stepRe = stepReEdit->text().toDouble();

    double minAlpha = minAlphaEdit->text().toDouble();
    double maxAlpha = maxAlphaEdit->text().toDouble();
    double stepAlpha = stepAlphaEdit->text().toDouble();

    double ma = maEdit->text().toDouble();
    int threadnum = threadNumEdit->text().toInt();
    int ncri = ncriEdit->text().toInt();
    int step = iterlimEdit->text().toInt();

    int iterStep = abs(maxRe - minRe) / stepRe + 1;
    int alphaStep = abs(maxAlpha - minAlpha) / stepAlpha + 1;


    bool ok1 = (iterStep > 4000);
    bool ok2 = (alphaStep > 100);
    bool ok3 = (ncri > 13 || ncri < 7);
    bool ok4 = (threadnum > 30);
    bool ok5 = (ma > 0.8);
    bool ok6 = (minRe < 5000 || maxRe < 5000 || stepRe < 10);
    bool ok7 = (step < 25);




    if(ok1){
        QMessageBox::information(nullptr,"警告","雷诺数计算点过大");
        isSettingOk = false;
        return;
    }
    if(ok2){
        QMessageBox::information(nullptr,"警告","迎角计算点过大");
        isSettingOk = false;
        return;
    }
    if(ok3){
        ncriEdit->setStyleSheet("background-color: yellow;");
    }else{
        ncriEdit->setStyleSheet("");
    }

    if(ok4){
        threadNumEdit->setStyleSheet("background-color: yellow;");
    }else{
        threadNumEdit->setStyleSheet("");
    }

    if(ok5){
        maEdit->setStyleSheet("background-color: yellow;");
    }else{
        maEdit->setStyleSheet("");
    }

    if(ok6){
        minReEdit->setStyleSheet("background-color: yellow;");
        maxReEdit->setStyleSheet("background-color: yellow;");
        stepReEdit->setStyleSheet("background-color: yellow;");
    }else{
        minReEdit->setStyleSheet("");
        maxReEdit->setStyleSheet("");
        stepReEdit->setStyleSheet("");
    }

    if(ok7)
        iterlimEdit->setStyleSheet("background-color: yellow;");



    interSetting.minAlpha = minAlpha;
    interSetting.maxAlpha = maxAlpha;
    interSetting.alphaStepSize = stepAlpha;
    interSetting.Re = minRe;
    interSetting.Ma = ma;
    interSetting.model = 0;
    interSetting.nCrit = ncri;
    interSetting.s_IterLim = step;

    interSetting.xtrBot = 1.0;
    interSetting.xtrTop = 1.0;

    isSettingOk = true;


}
void airfoilDisplay::startInterAnalyse(){
    bool tmp = solutionModelButtonA->isEnabled();
    if(!tmp){
       

        startInterChoiceSolver();
    }else{
        

        startInterSolver();
    }
}
void airfoilDisplay::startInterChoiceSolver(){

    updateAnalyseSetting();
    if(!interResultArray.isEmpty()){
        interResultArray.clear();
        interReValueArray.clear();
    }
    if(!dragSeriesArray.isEmpty()){
        for(AirfoilPlotSeries*series : dragSeriesArray)
            delete series;
        dragSeriesArray.clear();
    }



    if(isSettingOk){


        int minRe = minReEdit->text().toInt();
        int maxRe = maxReEdit->text().toInt();
        int stepRe = stepReEdit->text().toInt();
        int threadnum = threadNumEdit->text().toInt();
        int iterStep = abs(maxRe - minRe) / stepRe + 1;

        for(int i = 0;i<iterStep;i++){
            interReValueArray.append(minRe + i * stepRe);
        }
        interAirfoil = airfoilArray[interChoiceIndex];

        int step = iterStep / threadnum;
        int remainderTmp = iterStep % threadnum;

        for(int i = 0;i<step;i++){
            int index = i * threadnum;

            emit emitAirfoilProgressBarValue(double(i) / step * 100);

            startXfoilInThreadB(index,threadnum);
            QString text = "第(" + QString::number(i + 1) + ")批线程计算完成\n";
            iterTextEdit->append(text);
            for(int j = index;j<threadnum + index;j++)
                drawInterClResult(interResultArray[j]);
            updateAxes(dragInterChartA,dragSeriesArray);

        }
        emit emitAirfoilProgressBarValue(100);

        if(remainderTmp > 0){
            int beginIndex = iterStep - remainderTmp;
            startXfoilInThreadB(beginIndex,remainderTmp);
        }

        saveInterText();

    }else{
        QMessageBox::information(nullptr,"警告","检查输入数据可靠性");
    }
}
void airfoilDisplay::startInterSolver(){

    updateAnalyseSetting();
    if(!interResultArray.isEmpty()){
        interResultArray.clear();
        interReValueArray.clear();
    }

    if(isSettingOk){


        int minRe = minReEdit->text().toInt();
        int maxRe = maxReEdit->text().toInt();
        int stepRe = stepReEdit->text().toInt();
        int threadnum = threadNumEdit->text().toInt();
        int iterStep = abs(maxRe - minRe) / stepRe + 1;

        for(int i = 0;i<iterStep;i++){
            interReValueArray.append(minRe + i * stepRe);
        }

        int num = airfoilArray.length();
        for(int n = 0;n<num;n++){
            emit emitAirfoilProgressBarValue(double(n) / num * 100);

            if(!interResultArray.isEmpty())
                interResultArray.clear();

            interAirfoil = airfoilArray[n];


            int step = iterStep / threadnum;
            int remainderTmp = iterStep % threadnum;

            for(int i = 0;i<step;i++){
                int index = i * threadnum;
                startXfoilInThreadB(index,threadnum);
            }

            if(remainderTmp > 0){
                int beginIndex = iterStep - remainderTmp;
                startXfoilInThreadB(beginIndex,remainderTmp);
            }

            saveInterText(n);


            QString text = airfoilNameArray[n] + "计算完成\n";
            iterTextEdit->append(text);


        }

        emit emitAirfoilProgressBarValue(100);


    }else{
        QMessageBox::information(nullptr,"警告","检查输入数据可靠性");
    }
}
void airfoilDisplay::saveInterText(){

    QString fileName = QFileDialog::getSaveFileName(nullptr, "Save Data", "", "Text Files (*.txt)");
    //QString fileName =QDir::currentPath() +  "/libaries/profili/airfoilData/";
    if (!fileName.isEmpty()) {
        QFile file(fileName);
        if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QTextStream stream(&file);
            for(int i = 0;i<interResultArray.length();i++){
                stream << interResultArray[i].Re;
                stream<<"\n";
                for(int j = 0;j<interResultArray[i].alphaData.length();j++){
                    stream<<interResultArray[i].alphaData[j]<<"  ";
                }
                stream<<"\n";
                for(int j = 0;j<interResultArray[i].alphaData.length();j++){
                    stream<<interResultArray[i].clData[j]<<"  ";
                }
                stream<<"\n";
                for(int j = 0;j<interResultArray[i].alphaData.length();j++){
                    stream<<interResultArray[i].cdData[j]<<"  ";
                }
                stream<<"\n";
                for(int j = 0;j<interResultArray[i].alphaData.length();j++){
                    stream<<interResultArray[i].cmData[j]<<"  ";
                }
                stream<<"\n";

                stream<<"\n";
            }
            file.close();
            QMessageBox::information(nullptr,"提示","保存成功");
        }
    }

}
void airfoilDisplay::startInterAllSolver(){

    updateAnalyseSetting();
    if(!interResultArray.isEmpty()){
        interResultArray.clear();
        interReValueArray.clear();
    }

    if(isSettingOk){


        int minRe = minReEdit->text().toInt();
        int maxRe = maxReEdit->text().toInt();
        int stepRe = stepReEdit->text().toInt();
        int threadnum = threadNumEdit->text().toInt();
        int iterStep = abs(maxRe - minRe) / stepRe + 1;

        for(int i = 0;i<iterStep;i++){
            interReValueArray.append(minRe + i * stepRe);
        }

        int num = airfoilList->AirfoilA.length();
        for(int n = 0;n<num;n++){

            if(!interResultArray.isEmpty())
                interResultArray.clear();

            interAirfoil = airfoilList->airfoilArrayA[n];


            int step = iterStep / threadnum;
            int remainderTmp = iterStep % threadnum;

            for(int i = 0;i<step;i++){
                int index = i * threadnum;
                startXfoilInThreadB(index,threadnum);
            }

            if(remainderTmp > 0){
                int beginIndex = iterStep - remainderTmp;
                startXfoilInThreadB(beginIndex,remainderTmp);
            }

            saveInterText(n);

            emit emitAirfoilProgressBarValue(double(n) / num * 100);
            QString text = airfoilList->airfoilNameArrayA.at(n) + "计算完成\n";
            iterTextEdit->append(text);


        }

        emit emitAirfoilProgressBarValue(100);


    }else{
        QMessageBox::information(nullptr,"警告","检查输入数据可靠性");
    }
}
void airfoilDisplay::saveInterText(int index){

    //QString fileName = QFileDialog::getSaveFileName(nullptr, "Save Data", "", "Text Files (*.txt)");
    QString fileName =QDir::currentPath() +  "/libaries/airfoil/profili/airfoilData/" + airfoilList->airfoilNameArrayA.at(index) + ".txt";
    if (!fileName.isEmpty()) {
        QFile file(fileName);
        if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QTextStream stream(&file);
            for(int i = 0;i<interResultArray.length();i++){
                stream << interResultArray[i].Re;
                stream<<"\n";
                for(int j = 0;j<interResultArray[i].alphaData.length();j++){
                    stream<<interResultArray[i].alphaData[j]<<"   ";
                }
                stream<<"\n";
                for(int j = 0;j<interResultArray[i].alphaData.length();j++){
                    stream<<interResultArray[i].clData[j]<<"   ";
                }
                stream<<"\n";
                for(int j = 0;j<interResultArray[i].alphaData.length();j++){
                    stream<<interResultArray[i].cdData[j]<<"   ";
                }
                stream<<"\n";
                for(int j = 0;j<interResultArray[i].alphaData.length();j++){
                    stream<<interResultArray[i].cmData[j]<<"   ";
                }
                stream<<"\n";

                stream<<"\n";
            }
            file.close();
        }
    }

    QMessageBox::information(nullptr,"提示","保存成功");

}
void airfoilDisplay::startXfoilInThreadB(const int index,const int num){
    QVector<QFuture<void>>futures;
    QFutureSynchronizer<void> sync;

    QVector<airfoilSolve*>testSolve;


    for(int i = 0;i<num;i++){
        airfoilSolve *solvers = new airfoilSolve();
        testSolve.append(solvers);

        interSetting.Re = interReValueArray[index + i];
        solvers->importAirfoil(interAirfoil);
        solvers->refreshParaments(interSetting);
        QFuture<void>future = QtConcurrent::run([solvers](){
            solvers->solver();
        });
        futures.append(future);
    }

    for (auto &f : futures) {
        sync.addFuture(f);
    }

    sync.waitForFinished();   // ✅ 等“所有线程”一起完成


    for(int i = 0;i<num;i++){

        airfoilData dataTmp;
        for(int j = 0;j<testSolve[i]->resultData.length();j++){
            dataTmp.clData.append(testSolve[i]->resultData[j][1]);
            dataTmp.cdData.append(testSolve[i]->resultData[j][2]);
            dataTmp.alphaData.append(testSolve[i]->resultData[j][0]);

            dataTmp.cmData.append(testSolve[i]->resultData[j][5]);
        }
        dataTmp.Re = interReValueArray[index + i];

        interResultArray.append(dataTmp);

    }



    qDeleteAll(testSolve);
    testSolve.clear();
}
void airfoilDisplay::startOpt(){
    if(airfoilArray.isEmpty()){
        QMessageBox::information(this,"警告","未添加翼型");
        return;
    }
    initialGa();
    changeOptimizationProgressBar(0);
    QFuture<void>future = QtConcurrent::run([&](){
        solveGa();
    });

}
void airfoilDisplay::solveGa(){

    for(int i = 0; i<GaSetting.step;i++){
        for(int j = 0; j<workerNum;j++){
            startXfoilInThread(i,j);
        }
        /*************************************************/  //XFOIL有时会计算失败，给出错误的值。

        if(i >= 1){
            for(int k = 0; k < resultK.length(); k++){
                if(resultK[k] > exp->bestSolution[i - 1] * 3)
                    resultK[k] = 0;
            }
        }
        /***********************************************/

        exp->sortResult(resultK);
        exp->updateChromosomeSequenceDec();
        exp->selectionChromosomeSequenceDec();
        exp->decToBin();
        exp->overlappingOperations();
        exp->variationOperations();
        exp->binToDec();
        workerNum = GaSetting.eliteNum / threadNum;

        airfoilDesign design(airfoilDesignModelArray[choiceOPTAirfoilIndex]->cstNum);
        design.buildBenrnstein(airfoilArray[choiceOPTAirfoilIndex]);
        design.buildAirfoilCurve(exp->bestCST);
        if( i==0){
            oriK = resultK[0];
            if(oriK == 0){
                oriCpx = historyData[exp->bestIndex].cpx;
                optCpx = historyData[exp->bestIndex].cpx;
            }else{
                oriCpx = historyData[0].cpx;
                optCpx = historyData[0].cpx;
            }

        }
        optK = exp->bestSolution[i];


        if(exp->bestIndex < GaSetting.eliteNum ){
            optCpxIsChange = true;
            optCpx = historyData[exp->bestIndex].cpx;

        }
        //qDebug()<<exp->bestIndex;


        emit emitOptimizationUI(i,design.newAirfoilData);  //update UI
        QVector<fixClResult>().swap(historyData);
        QVector<double>().swap(resultK);
        resultK.clear();
        thick.clear();
        historyData.clear();
    }

}
void airfoilDisplay::finishOpt(){
    input.model = 0;
    airfoilNameArray.append("opt_" + airfoilNameArray[choiceOPTAirfoilIndex]);
    addNewAirfoil(optAirfoil);

}
void airfoilDisplay::startXfoilInThread(int step,int n){

    QVector<QFuture<void>>futures;

    QVector<airfoilSolve*>testSolve;
    QFutureSynchronizer<void> sync;
    int index1;

    for(int i = 0; i<threadNum;i++){
        QVector<double>cst;

        airfoilSolve *solvers = new airfoilSolve();
        testSolve.append(solvers);


        airfoilDesign design(airfoilDesignModelArray[choiceOPTAirfoilIndex]->cstNum);
        design.buildBenrnstein(airfoilArray[choiceOPTAirfoilIndex]);
        //qDebug() << airfoilArray[choseOPTAirfoilIndex];

        if (step == 0) {
            index1 = i + (GaSetting.initialEliteNum / threadNum - workerNum + n) *
                             threadNum;
            cst = exp->initialCst[index1];
            design.buildAirfoilCurve(cst);
            if (index1 == 0) { // 先计算一次初始翼型
                solvers->importAirfoil(airfoilArray[choiceOPTAirfoilIndex]);
                design.computeSimpleParameters(airfoilArray[choiceOPTAirfoilIndex]);
                thick.append(design.maxThickness);
            } else {
                solvers->importAirfoil(design.newAirfoilData);
                design.computeSimpleParameters(design.newAirfoilData);
                thick.append(design.maxThickness);
            }
        }

        else{
            index1 = i + (GaSetting.eliteNum / threadNum - workerNum + n) * threadNum;
            cst = exp->chromosomeSequenceDec[index1];
            design.buildAirfoilCurve(cst);

            solvers->importAirfoil(design.newAirfoilData);
            design.computeSimpleParameters(design.newAirfoilData);
            thick.append(design.maxThickness);
        }


        solvers->refreshParaments(input);
        QFuture<void>future = QtConcurrent::run([solvers](){
            solvers->emitResult();
        });


        futures.append(future);
    }


    for (auto &f : futures) {
        sync.addFuture(f);
    }

    sync.waitForFinished();   // ✅ 等“所有线程”一起完成



    double tmp = 0;
    for(int i = 0; i<threadNum;i++){
        historyData.append(testSolve[i]->onceData);

        tmp = testSolve[i]->onceData.K + testSolve[i]->onceData.cM * cmWeighted + thick[i] * thickWeighted;
        resultK.append(tmp);
    }

    qDeleteAll(testSolve);
    testSolve.clear();

}
void airfoilDisplay::updateOptimizationUI(const int n,const QVector<QVector<double>>&airfoilData){



    airfoilDesign a1(airfoilDesignModelArray[choiceOPTAirfoilIndex]->cstNum);

    a1.computeOtherParameters(airfoilData);


    drawOptAirfoil(airfoilData);

    int progressVal = (n + 1)  * 100 / GaSetting.step;
    changeOptimizationProgressBar(progressVal);
    if(n == 0){
        iterateSeries->append(n,oriK);
        iterateAyAxis->setRange(oriK - 20,oriK + 20);
        drawCpxData(oriCpx,oriCpxSeries);
        drawCpxData(optCpx,optCpxSeries);
        iterateAxAxis->setRange(0,optStep);
    }
    else{
        iterateSeries->append(n,optK);

        iterateAyAxis->setRange(oriK - 20,optK + 20);
        //chartE->removeSeries(optCpxSeries);
        if(optCpxIsChange){
            optCpxSeries->clear();
            drawCpxData(optCpx,optCpxSeries);
        }
        optCpxIsChange = false;
    }
    //刷新坐标系
    QVector<AirfoilPlotSeries*>tmpSeriesArray;
    tmpSeriesArray.append(oriCpxSeries);
    tmpSeriesArray.append(optCpxSeries);
    updateAxes(cpxchart,tmpSeriesArray);


    updateOptimizationTextView(optK);


    if(n == GaSetting.step - 1){
        optAirfoil = airfoilData;

        finishOpt();
        QMessageBox::information(this, "信息", "优化完成");
    }

}

void airfoilDisplay::drawDesignAirfoil(const int index){
    //designSeries

    if (upperAreaSeries)
        upperAreaSeries->setVisible(false);
    if (lowerAreaSeries)
        lowerAreaSeries->setVisible(false);

    if(designSeriesUpper->count() != 0){
        designSeriesUpper->clear();
        designSeriesLower->clear();
    }

    if(designScatterSeriesA->count() != 0)
        designScatterSeriesA->clear();
    if(designSeriesPointA->count() != 0)
        designSeriesPointA->clear();
    if(designSeriesPointB->count() != 0)
        designSeriesPointB->clear();

    int len = findLeadingEdgeIndex(choiceIndex, airfoilArray);
    for(int i = 0;i<len;i++){
        designSeriesUpper->append(airfoilArray[index][i][0],airfoilArray[index][i][1]);
    }


    for(int i = len;i<airfoilArray[index].length();i++){
        designSeriesLower->append(airfoilArray[index][i][0],airfoilArray[index][i][1]);
    }
    QPen pen;
    pen.setWidth(2);
    pen.setColor(airfoilColorArray[index]);
    designSeriesUpper->setPen(pen);
    designSeriesLower->setPen(pen);
    for(int i = 0;i<cstPointXArray[index].length();i++){
        designScatterSeriesA->append(cstPointXArray[index][i],cstPointYArray[index][i]);
        designSeriesPointA->append(cstPointXArray[index][i],cstPointYArray[index][i]);
        designSeriesPointB->append(cstPointXArray[index][i],cstPointYArray[index][i]);
    }
    designScatterSeriesA->append(cstPointXArray[index][0],cstPointYArray[index][0]);

}
void airfoilDisplay::drawOptIndexAirfoil(const int index){
    choiceOPTAirfoilIndex = index;
    if(!airfoilArray[index].isEmpty()){
        if(optimizationSeriesA->count() > 0)
            optimizationSeriesA->clear();
        for(int i = 0;i<airfoilArray[index].length();i++){
            optimizationSeriesA->append(airfoilArray[index][i][0],airfoilArray[index][i][1]);
        }
    }
}
void airfoilDisplay::drawOptAirfoil(const QVector<QVector<double>>&airfoil){
    if(!airfoil.isEmpty()){
        if(optimizationSeriesA->count() > 0)
            optimizationSeriesA->clear();
        for(int i = 0;i<airfoil.length();i++){
            optimizationSeriesA->append(airfoil[i][0],airfoil[i][1]);
        }
    }
}
void airfoilDisplay::drawCpxData(const QVector<double>&cpx,AirfoilPlotSeries*series){
    if(series->count() > 0)
        series->clear();
    for(int i = 0;i<cpx.length();i++){
        series->append(airfoilArray[choiceOPTAirfoilIndex][i][0],cpx[i]);
    }

}
void airfoilDisplay::solveDesignAirfoil(){
    if(airfoilArray.isEmpty()){
        QMessageBox::information(this,"警告","请导入翼型");
        return;
    }
    airfoilSolve *solvers = airfoilSolveModelArray[choiceIndex];
    saveDesignSetting();//刷新Xfoil设置
    solvers->refreshParaments(input);
    solvers->importAirfoil(airfoilArray[choiceIndex]);

    // 初始化 watcher
    QFutureWatcher<void>* watcher = new QFutureWatcher<void>(this);
    connect(watcher, &QFutureWatcher<void>::finished, this, [=]{
        // 在槽函数中使用捕获列表 [=] 可以安全地捕获局部变量
        drawDesignResult(choiceIndex);
        changeTransitionLocation(choiceIndex);
        changeBlrSeries(choiceIndex);
        updateAxes(resultChartA,resultSeriesA);
        updateAxes(resultChartB,resultSeriesB);

        alphaCombobox->blockSignals(true);
        alphaCombobox->clear();
        for(int i = 0;i<airfoilSolveModelArray[choiceIndex]->resultData.length();i++)
            alphaCombobox->addItem(QString::number(airfoilSolveModelArray[choiceIndex]->resultData[i][0]));
        alphaCombobox->setCurrentIndex(activeAlphaIndex);
        alphaCombobox->blockSignals(false);

        emit emitAirfoilArray(airfoilArray,airfoilNameArray,cstNumArray);

        // 删除 watcher 以防止内存泄漏
        watcher->deleteLater();
    });

    QFuture<void> future = QtConcurrent::run([=](){
        solvers->solver();
    });

    // 将 future 关联到 watcher
    watcher->setFuture(future);
}
void airfoilDisplay::changeAlpha(const int index){
    if(airfoilSolveModelArray[choiceIndex]->resultData.length() > index){
        changeResultCpx(index);
        changeTransitionLocation(index);
        changeBlrSeries(index);
        changeDesignPoint(index);
    }
}
void airfoilDisplay::changeResultCpx(const int index){


    if(resultBIndex == 6 && index < airfoilSolveModelArray[choiceIndex]->cpxResultData.length() - 1){
        if(resultSeriesB[choiceIndex]->count() != 0)
            resultSeriesB[choiceIndex]->clear();
        QVector<double>cpData = airfoilSolveModelArray[choiceIndex]->cpxResultData[index];
        for(int i = 0;i<airfoilArray[choiceIndex].length();i++){
            resultSeriesB[choiceIndex]->append(airfoilArray[choiceIndex][i][0],cpData[i]);
        }

        updateAxes(resultChartB,resultSeriesB);

    }
    activeAlphaIndex = index;
}
void airfoilDisplay::changeTransitionLocation(const int index){
    if(transitionPoint->count() != 0)
        transitionPoint->clear();
    double upperX = airfoilSolveModelArray[choiceIndex]->resultData[index][7];
    double lowerX = airfoilSolveModelArray[choiceIndex]->resultData[index][8];


    airfoilDesign design;
    design.splitAirfoilData(airfoilArray[choiceIndex]);
    //double upperY = airfoilDesignModelArray[choiceIndex]->getUpperY(upperX);
    //double lowerY = airfoilDesignModelArray[choiceIndex]->getLowerY(lowerX);
    double upperY = design.getUpperY(upperX);
    double lowerY = design.getLowerY(lowerX);
    transitionPoint->append(upperX,upperY);
    transitionPoint->append(lowerX,lowerY);
}
void airfoilDisplay::changeBlrSeries(const int index){
    if(xblUpperSeries->count() != 0){
        xblUpperSeries->clear();
        xblLowerSeries->clear();

    }

    designSeriesUpper->clear();
    designSeriesLower->clear();

    QVector<double> upperTmp = airfoilSolveModelArray[choiceIndex]->xblDataUpper[index];
    QVector<double> lowerTmp = airfoilSolveModelArray[choiceIndex]->xblDataLower[index];

    int len = airfoilSolveModelArray[choiceIndex]->xWakeArray[index].length() + 2;

    for(int i = 0;i<upperTmp.length();i++){
        designSeriesUpper->append(airfoilArray[choiceIndex][i][0],airfoilArray[choiceIndex][i][1]);
        xblUpperSeries->append(airfoilArray[choiceIndex][i][0],airfoilArray[choiceIndex][i][1] + upperTmp[upperTmp.length() - i - 1]);
    }

    for(int i = -1;i<lowerTmp.length() - len;i++){
        designSeriesLower->append(airfoilArray[choiceIndex][upperTmp.length() + i][0],airfoilArray[choiceIndex][upperTmp.length() +i][1]);
        xblLowerSeries->append(airfoilArray[choiceIndex][upperTmp.length() + i][0],airfoilArray[choiceIndex][upperTmp.length() +i][1] - lowerTmp[i + 1]);
    }

    updateBlAreaSeries();

}
void airfoilDisplay::updateBlAreaSeries()
{
    // --- 1️⃣ 先删除旧的 AreaSeries（如果已经存在）
    if (upperAreaSeries) {
        designChartA->removeSeries(upperAreaSeries);
        delete upperAreaSeries;
        upperAreaSeries = nullptr;

    }
    if (lowerAreaSeries) {
        designChartA->removeSeries(lowerAreaSeries);
        delete lowerAreaSeries;
        lowerAreaSeries = nullptr;

    }



    // 创建 QCustomPlot 通道填充区域。
    // 上表面区域：xblUpperSeries 在上，designSeriesUpper 在下
    upperAreaSeries = new AirfoilPlotArea(xblUpperSeries, designSeriesUpper);
    // 下表面区域：designSeriesLower 在上，xblLowerSeries 在下
    lowerAreaSeries = new AirfoilPlotArea(designSeriesLower, xblLowerSeries);

    // --- 3️⃣ 设置颜色和透明度
    upperAreaSeries->setBrush(QColor(31, 119, 180, 80));  // 半透明蓝
    upperAreaSeries->setPen(Qt::NoPen);                    // 不显示边框

    lowerAreaSeries->setBrush(QColor(214, 39, 40, 80));   // 半透明红
    lowerAreaSeries->setPen(Qt::NoPen);

    designChartA->addSeries(upperAreaSeries);
    designChartA->addSeries(lowerAreaSeries);

    // --- 4️⃣ 绑定坐标轴
    upperAreaSeries->attachAxis(designAxisXA);
    upperAreaSeries->attachAxis(designAxisYA);

    lowerAreaSeries->attachAxis(designAxisXA);
    lowerAreaSeries->attachAxis(designAxisYA);

    designAxisXA->setRange(-0.1,1.1);
    designAxisYA->setRange(-0.25,0.25);


    if(designActionArray[6]->text() == "显示边界层"){
        upperAreaSeries->setVisible(false);
        lowerAreaSeries->setVisible(false);
    }else{
        upperAreaSeries->setVisible(true);
        lowerAreaSeries->setVisible(true);
    }

    adjustChartViewAspectRatio(designChartViewA);


}
void airfoilDisplay::changeDesignPoint(const int index){
    if(designPointA->count() != 0)
        designPointA->clear();
    if(designPointB->count() != 0)
        designPointB->clear();
    double x1,y1,x2,y2;
    if(resultAIndex != 6){
        x1 = airfoilSolveModelArray[choiceIndex]->resultData[index][0];
        y1 = airfoilSolveModelArray[choiceIndex]->resultData[index][resultAIndex];
    }else{
        x1 = airfoilSolveModelArray[choiceIndex]->resultData[index][2];
        y1 = airfoilSolveModelArray[choiceIndex]->resultData[index][1];
    }

    designPointA->append(x1,y1);
    if(resultBIndex != 6){
        x2 = airfoilSolveModelArray[choiceIndex]->resultData[index][0];
        y2 = airfoilSolveModelArray[choiceIndex]->resultData[index][resultBIndex];
        designPointB->append(x2,y2);
    }

}
void airfoilDisplay::drawDesignResult(const int index){

    QVector<QVector<double>>result = airfoilSolveModelArray[index]->resultData;
    QVector<QVector<double>>cpData = airfoilSolveModelArray[index]->cpxResultData;
    int realIndex;
    if(activeAlphaIndex < cpData.size() - 1)
        realIndex = activeAlphaIndex;
    else
        realIndex = 0;

    if(!result.isEmpty()){
        if(resultSeriesA[index]->count() != 0){
            resultSeriesA[index]->clear();
        }
        if(resultSeriesB[index]->count() != 0){
            resultSeriesB[index]->clear();
        }
        if(resultAIndex != 6){
            for(int i = 0;i<result.length();i++){
                resultSeriesA[index]->append(result[i][0],result[i][resultAIndex]);
            }
            resultSeriesA[index]->setPen(getPen(index));
            //resultAxisXA->setRange(MINAXIS[0],MAXAXIS[0]);
            //resultAxisYA->setRange(MINAXIS[resultAIndex],MAXAXIS[resultAIndex]);
            resultAxisXA->setTitleText(axisXName[resultAIndex]);
            resultAxisYA->setTitleText(axisYName[resultAIndex]);
            resultChartA->setTitle(titleName[resultAIndex]);
        }else{
            for(int i = 0;i<result.length();i++){
                resultSeriesA[index]->append(result[i][2],result[i][1]);
            }
            resultSeriesA[index]->setPen(getPen(index));
            //resultAxisXA->setRange(MINAXIS[2],MAXAXIS[2]);
            //resultAxisYA->setRange(MINAXIS[1],MAXAXIS[1]);
            resultAxisXA->setTitleText(axisYName[2]);
            resultAxisYA->setTitleText(axisYName[1]);
            resultChartA->setTitle(titleName[8]);
        }


        if(resultBIndex != 6){
            resultAxisYB->setReverse(false);
            for(int i = 0;i<result.length();i++){
                resultSeriesB[index]->append(result[i][0],result[i][resultBIndex]);
            }
            resultSeriesB[index]->setPen(getPen(index));
            //resultAxisXB->setRange(MINAXIS[0],MAXAXIS[0]);
            //resultAxisYB->setRange(MINAXIS[resultBIndex],MAXAXIS[resultBIndex]);
            resultAxisXB->setTitleText(axisXName[resultBIndex]);
            resultAxisYB->setTitleText(axisYName[resultBIndex]);
            resultChartB->setTitle(titleName[resultBIndex]);
        }else{

            resultAxisYB->setReverse(true);
            for(int i = 0;i<airfoilArray[index].length();i++){
                resultSeriesB[index]->append(airfoilArray[index][i][0],cpData[realIndex][i]);
            }
            resultSeriesB[index]->setPen(getPen(index));
            //resultAxisXB->setRange(-0.1,1.1);
            //resultAxisYB->setRange(MINAXIS[6],MAXAXIS[6]);
            resultAxisXB->setTitleText(axisXName[6]);
            resultAxisYB->setTitleText(axisYName[6]);
            resultChartB->setTitle(titleName[6]);
        }

        //changeTransitionLocation(realIndex);
        changeDesignPoint(realIndex);
        //changeBlrSeries(index);


    }
}
void airfoilDisplay::deleteAirfoilDesignButton(){
    QPushButton *button = static_cast<QPushButton*>(sender());
    int index = button->property("remove").toInt();
    button = nullptr;
    removeDesignAirfoil(index);

}
void airfoilDisplay::resetAirfoilData(){
    emit emitClicked();
}
void airfoilDisplay::readData(){

    QString filePath = QFileDialog::getOpenFileName(
        nullptr,
        "选择文件",
        "",
        "翼型文件(*.dat);;所有文件(*)"
    );

    // 用户取消
    if(filePath.isEmpty()){
        return;
    }

    QVector<QVector<double>> airfoil;
    QString name;

    QFile file(filePath);

    if(!file.open(QIODevice::ReadOnly | QIODevice::Text)){
        QMessageBox::warning(nullptr, "错误", "文件打开失败");
        return;
    }

    QTextStream in(&file);

    if(!in.atEnd()){
        name = in.readLine();
        airfoilNameArray.append(name);
    }

    while(!in.atEnd()){
        QString line = in.readLine();
        QStringList parts = line.split(" ", Qt::SkipEmptyParts);

        if(parts.size() == 2){
            bool ok1, ok2;
            double value1 = parts[0].toDouble(&ok1);
            double value2 = parts[1].toDouble(&ok2);

            if(ok1 && ok2){
                airfoil.append({value1, value2});
            }
        }
    }

    file.close();

    addNewAirfoil(airfoil);
}
void airfoilDisplay::removeDesignAirfoil(const int index){

    designChartViewA->textItem->setPlainText(" ");

    delete resultSeriesA[index];
    delete resultSeriesB[index];
    delete airfoilDesignModelArray[index];
    delete airfoilSolveModelArray[index];
    delete airfoilXfoilArray[index];

    resultSeriesA.remove(index);
    resultSeriesB.remove(index);
    airfoilDesignModelArray.remove(index);
    airfoilSolveModelArray.remove(index);
    airfoilXfoilArray.remove(index);


    //choiceCombobox->removeItem(index);
    cstNumArray.remove(index);
    airfoilArray.remove(index);
    interDataArray.remove(index);
    historyAirfoilArray.remove(index);
    airfoilNameArray.remove(index);
    cstArray.remove(index);
    cstPointXArray.remove(index);
    cstPointYArray.remove(index);
    resultPenArray.remove(index);
    airfoilColorArray.remove(index);

    choiceCombobox->blockSignals(true);
    choiceCombobox->removeItem(index);
    choiceCombobox->blockSignals(false);

    airfoilCombox->blockSignals(true);
    airfoilCombox->removeItem(index);
    airfoilCombox->blockSignals(false);

    //choiceCombobox->removeItem(index);



    if (index < airfoilButton.length()) {
        QPushButton* buttonToRemove = airfoilButton.at(index);
        designGLayoutA1->removeWidget(buttonToRemove);
        buttonToRemove->deleteLater();
        airfoilButton.removeAt(index);
    }

    if (index < airfoilCheckButton.length()) {
        QPushButton* buttonToRemove = airfoilCheckButton.at(index);
        designGLayoutA1->removeWidget(buttonToRemove);
        buttonToRemove->deleteLater();
        airfoilCheckButton.removeAt(index);
    }

    if (index < airfoilColorButton.length()) {
        QPushButton* buttonToRemove = airfoilColorButton.at(index);
        designGLayoutA1->removeWidget(buttonToRemove);
        buttonToRemove->deleteLater();
        airfoilColorButton.removeAt(index);
    }

    if (index < airfoilRemoveButton.length()) {
        QPushButton* buttonToRemove = airfoilRemoveButton.at(index);
        designGLayoutA1->removeWidget(buttonToRemove);
        buttonToRemove->deleteLater();
        airfoilRemoveButton.removeAt(index);
    }

    for (int i = 0; i < airfoilButton.size(); ++i) {
        designGLayoutA1->addWidget(airfoilButton.at(i), i, 0,1,1); // 第一列
        designGLayoutA1->addWidget(airfoilCheckButton.at(i), i, 2,1,1); // 第二列
        designGLayoutA1->addWidget(airfoilColorButton.at(i), i, 3,1,1); // 第二列
        designGLayoutA1->addWidget(airfoilRemoveButton.at(i), i, 4,1,1); // 第二列
        airfoilButton.at(i)->setProperty("airfoil",i);
        airfoilCheckButton.at(i)->setProperty("check",i);
        airfoilColorButton.at(i)->setProperty("color",i);
        airfoilRemoveButton.at(i)->setProperty("remove",i);
    }


    airfoilIndex--;

    if(airfoilIndex <= 0){
        //airfoilIndex = 0;
        designSeriesUpper->clear();
        designSeriesLower->clear();
        xblLowerSeries->clear();
        xblUpperSeries->clear();


        designScatterSeriesA->clear();
        designSeriesPointA->clear();
        designSeriesPointB->clear();
        transitionPoint->clear();
        designPointA->clear();
        designPointB->clear();
        alphaCombobox->clear();
    }else{
        choiceIndex = 0;
        choiceAirfoilButton();//刷新剩余翼型列表
    }

    emit emitAirfoilArray(airfoilArray,airfoilNameArray,cstNumArray);
    emit emitAirfoilInterArray(interDataArray);

}



void airfoilDisplay::saveDesignSetting(){
    input.Re = ReEdit->text().toDouble();
    input.Ma = MaEdit->text().toDouble();
    input.nCrit = NcriEdit->text().toInt();
    input.minAlpha = MinAlphaEdit->text().toDouble();
    input.maxAlpha = MaxAlphaEdit->text().toDouble();
    input.alphaStepSize = AlphaStepEdit->text().toDouble();
    input.xtrTop = XtrTopEdit->text().toDouble();
    input.xtrBot = XtrBotEdit->text().toDouble();
    // MINAXIS[0] = minAlphaAxisEdit->text().toDouble();
    // MAXAXIS[0] = maxAlphaAxisEdit->text().toDouble();

}

void airfoilDisplay::cancelDesignSetting(){

    input.Re = historySettingData[0]; input.Ma = historySettingData[1]; input.nCrit = int(historySettingData[2]); input.alphaStepSize = historySettingData[3];
    input.minAlpha = historySettingData[4];input.maxAlpha = historySettingData[5];input.xtrTop = historySettingData[6];input.xtrBot = historySettingData[7];


    ReEdit->setText(QString::number(input.Re));
    MaEdit->setText(QString::number(input.Ma));
    NcriEdit->setText(QString::number(input.nCrit));
    MinAlphaEdit->setText(QString::number(input.minAlpha));
    MaxAlphaEdit->setText(QString::number(input.maxAlpha));
    AlphaStepEdit->setText(QString::number(input.alphaStepSize));
    XtrTopEdit->setText(QString::number(input.xtrTop));
    XtrBotEdit->setText(QString::number(input.xtrBot));
}
void airfoilDisplay::saveOptimizationSetting(){
    GaSetting.step = stepEdit->text().toInt();
    GaSetting.selection = selectionEdit->text().toDouble();
    GaSetting.initialEliteNum = initialEliteNumEdit->text().toInt();
    GaSetting.eliteNum = eliteNumEdit->text().toInt();
    GaSetting.cross = crossEdit->text().toDouble();
    GaSetting.variation = variationEdit->text().toDouble();
    GaSetting.val = valEdit->text().toDouble();
    GaSetting.cstRadio = cstRadioEdit->text().toDouble();
    input.Re = optReEdit->text().toInt();
    input.Ma = optMaEdit->text().toDouble();

    choiceOPTAirfoilIndex = choiceCombobox->currentIndex();
    if(targetRadioButtonB->isChecked()){
        input.designALPHA = valueEdit->text().toDouble();
        input.model = 0;
    }
    else {
        input.designCL = valueEdit->text().toDouble();;
        input.model = 1;

    }
    threadNum = ThreadNumEdit->text().toInt();


    if(GaSetting.eliteNum % threadNum !=0 || GaSetting.initialEliteNum % threadNum !=0 ){
        QMessageBox::information(nullptr,"警告","初始种群或精英种群都应该被线程数量整除");

    }else if(GaSetting.eliteNum * 2 > GaSetting.initialEliteNum){
        QMessageBox::information(nullptr,"警告","初始种群至少是精英种群数量的两倍");
    }
    else if(GaSetting.cstRadio >= 1 && GaSetting.cstRadio<=0){
        QMessageBox::information(nullptr,"警告","变量上限应小于1大于0");

    }

    else{

        //saveAllSetting();
        startOpt();
    }




}
void airfoilDisplay::cancelOptimizationSetting(){
    GaSetting.step = historySettingData[14];GaSetting.selection = historySettingData[15];GaSetting.initialEliteNum = historySettingData[16];GaSetting.eliteNum = historySettingData[17];
    GaSetting.cross = historySettingData[18];GaSetting.variation = historySettingData[19];GaSetting.val = historySettingData[20];GaSetting.cstRadio = historySettingData[21];GaSetting.solutionsNum = cstNUM * 2;


    stepEdit->setText(QString::number(GaSetting.step));
    selectionEdit->setText(QString::number(GaSetting.selection));
    initialEliteNumEdit->setText(QString::number(GaSetting.initialEliteNum));
    eliteNumEdit->setText(QString::number(GaSetting.eliteNum));
    crossEdit->setText(QString::number(GaSetting.cross));
    variationEdit->setText(QString::number(GaSetting.variation));
    valEdit->setText(QString::number(GaSetting.val));
    cstRadioEdit->setText(QString::number(GaSetting.cstRadio));
    ThreadNumEdit->setText(QString::number(threadNum));
    iterLimEdit->setText(QString::number(input.s_IterLim));
}


void airfoilDisplay::saveAllSetting(){
    QString initialName =QDir::currentPath() +  "/setting/airfoilSetting.txt";
    QFile file(initialName);



    if(file.open(QIODevice::WriteOnly | QIODevice::Truncate)){
        QTextStream stream(&file);
        //stream.setRealNumberNotation(QTextStream::FixedNotation); // 使用固定小数点表示法

           // 选择显示小数点后的位数
        //stream.setRealNumberPrecision(6); // 设置精度，例如6位小数
        stream<<"Re  "<<input.Re<<" \nMa   "<<input.Ma<<"\nNcrit   "<<input.nCrit<<"\nAlphaStep   "<<input.alphaStepSize<<"\nMinAlpha   "<<input.minAlpha<<"\nMaxAlpha   "<<input.maxAlpha<<"\nXtrTop   "<<input.xtrTop<<"\nXtrBot   "<<input.xtrBot
               <<"\nCstNum   "<<cstNum<<"\nThreadNum   "<<threadNum<<"\nDesignCl   "<<input.designCL<<"\nDesignAlpha   "<<input.designALPHA<<"\nModel   "<<input.model<<"\nS_IterLim   "<<input.s_IterLim
               <<"\nStep   "<<GaSetting.step<<"\nSelection   "<<GaSetting.selection<<"\nInitialElite   "<<GaSetting.initialEliteNum<<"\nElite   "<<GaSetting.eliteNum<<"\nCross   "<<GaSetting.cross<<"\nVariation   "<<GaSetting.variation<<"\nVal   "<<GaSetting.val<<"\nRatio   "<<GaSetting.cstRadio;
        stream<<"\nMINAXIS1   "<<MINAXIS[0]<<"\nMAXAXIS1   "<<MAXAXIS[0]<<"\nMINAXIS2   "<<MINAXIS[1]<<"\nMAXAXIS2   "<<MAXAXIS[1]<<"\nMINAXIS3   "<<MINAXIS[2]<<"\nMAXAXIS3  "<<MAXAXIS[2]<<"\nMINAXIS4    "<<MINAXIS[3]<<"\nMAXAXIS4    "<<MAXAXIS[3]<<"\nMINAXIS5   "<<MINAXIS[4]<<"\nMAXAXIS5   "<<MAXAXIS[4]
               <<"\nMINAXIS6   "<<MINAXIS[5]<<"\nMAXAXIS6   "<<MAXAXIS[5]<<"\nMINAXIS7   "<<MINAXIS[6]<<"\nMAXAXIS7   "<<MAXAXIS[6]<<"\nMINAXIS8   "<<MINAXIS[7]<<" \nMAXAXIS8   "<<MAXAXIS[7];
        file.close();
    }
}
void airfoilDisplay::updateAirfoilList(const QVector<QVector<QVector<double>>>&airfoil,const QVector<QString>&name){

   for(int i = airfoilButton.length() - 1;i>=0;i--)
       removeDesignAirfoil(i);

   isSolve = false;
   airfoilNameArray = name;



   for(int i = 0;i< airfoil.length();i++){
       addNewAirfoil(airfoil[i]);
   }

   isSolve = true;



}
void airfoilDisplay::updateThickWeightedValueLabel(const int value){
    QString text = QString::number(static_cast<double>(value) / 100);
    thickWeighted = value;
    thickWeightedValueLabel->setText(text);

}
void airfoilDisplay::updateCmWeightedValueLabel(const int value){
    QString text = QString::number(static_cast<double>(value) / 100);
    cmWeighted = value;
    cmWeightedValueLabel->setText(text);

}

void airfoilDisplay::updateDesignCSTPoint(){
    QVector<double>cstx;
    QVector<double>csty;
    int cst = airfoilDesignModelArray[choiceIndex]->cstNum;
    cstNumArray[choiceIndex] = cst;

    cstx.append(0);
    csty.append(0.1);
    for(int i = 0;i < cst - 2;i++){
        cstx.append(static_cast<double>(i + 1) / (cst - 1));
        csty.append(0.15);
    }
    cstx.append(1);
    csty.append(0.1);
    cstx.append(1);
    csty.append(-0.1);
    for(int i = 0;i < cst - 2;i++){
        cstx.append(static_cast<double>(cst - i - 2) / (cst - 1));
        csty.append(-0.15);
    }
    cstx.append(0);
    csty.append(-0.1);


    cstPointXArray[choiceIndex] = cstx;
    cstPointYArray[choiceIndex] = csty;
    drawDesignAirfoil(choiceIndex);

    //changeDesignCSTControlPoint(0.1);

}
void airfoilDisplay::changeDesignCSTControlPoint(const double s){



    double ds;



    if(!airfoilArray[choiceIndex].isEmpty()){              //判断series是否被定义

        double cstRadio = cstRatioSpinBox->value();
        ds = s - cstPointYArray[choiceIndex][cstnum];
        ds = ds * cstRadio;
        cstPointYArray[choiceIndex][cstnum] = s;
        cstArray[choiceIndex][cstnum] += ds;
        airfoilDesignModelArray[choiceIndex]->buildAirfoilCurve(cstArray[choiceIndex]);

        airfoilDesignModelArray[choiceIndex]->computeParameters(cstArray[choiceIndex]);



        //updateDesignTextView(airfoilDesignModelArray[choiceIndex]);

        airfoilArray[choiceIndex] = airfoilDesignModelArray[choiceIndex]->newAirfoilData; 
        updateDesignTextViewFormXfoil(airfoilXfoilArray[choiceIndex]);


        drawDesignAirfoil(choiceIndex);

    }


}
void airfoilDisplay::updateDesignTextViewFormXfoil(XFoil *foil){

    initialXfoil(foil,airfoilArray[choiceIndex]);
    foil->tcset(999.0,999.0);
    foil->pangen();
    airfoilArray[choiceIndex].clear();
    for (int i = 1; i <= foil->nb; i++) {   // 注意 XFOIL 通常是 1-based
        QVector<double> pt(2);
        pt[0] = foil->xb[i];
        pt[1] = foil->yb[i];
        airfoilArray[choiceIndex].push_back(pt);
    }
    //光顺



    QString text0 = "最大厚度: " + QString::number(foil->maxThickness * 100,'f',2) + "%  ";
    QString text1 = "最大厚度位置: "+ QString::number(foil->locationThickness * 100,'f',2) + "%  ";
    QString text2 = "最大弯度: "+ QString::number(foil->maxCamber * 100,'f',2) + "%  ";
    QString text3 = "最大弯度位置: "+ QString::number(foil->locationCamber * 100,'f',2) + "%  ";
    //QString text4 = "前缘半径: "+ QString::number(design->minRadius * 100,'f',2) + "%  ";
    //QString text5 = "尾缘角度: "+ QString::number(design->trailingAngle,'f',2);
    QString text4 = "前缘半径: "+ QString::number(airfoilDesignModelArray[choiceIndex]->minRadius * 100,'f',2) + "%  ";
    QString text5 = "尾缘角度: "+ QString::number(airfoilDesignModelArray[choiceIndex]->trailingAngle,'f',2);
    QString text = text0 + text1 + text2 + text3 + text4 + text5;

    designChartViewA->textItem->setPlainText(text);
    designChartViewA->update();


}
void airfoilDisplay::updateDesignTextView(airfoilDesign* design){



    QString text0 = "最大厚度: " + QString::number(design->maxThickness * 100,'f',2) + "%  ";
    QString text1 = "最大厚度位置: "+ QString::number(design->locationThickness * 100,'f',2) + "%  ";
    QString text2 = "最大弯度: "+ QString::number(design->maxCamber * 100,'f',2) + "%  ";
    QString text3 = "最大弯度位置: "+ QString::number(design->locationCamber * 100,'f',2) + "%  ";
    QString text4 = "前缘半径: "+ QString::number(design->minRadius * 100,'f',2) + "%  ";
    QString text5 = "尾缘角度: "+ QString::number(design->trailingAngle,'f',2);
    QString text = text0 + text1 + text2 + text3 + text4 + text5;

    designChartViewA->textItem->setPlainText(text);
    designChartViewA->update();



}
void airfoilDisplay::updateOptimizationTextView(const double value){

    optimizationChartViewA->textItem->setPlainText(QString::number(value,'f',2));

    //optimizationChartViewA->update();
}

void airfoilDisplay::updateDesignMousePosition(const QPointF &localPoint){

    double realy = localPoint.y();
    if (xLabelTextA->styleSheet() != "font-weight: bold; font-size: 12pt;") {
        xLabelTextA->setStyleSheet("font-weight: bold; font-size: 12pt;"); // 调整 pt 值
        yLabelTextA->setStyleSheet("font-weight: bold; font-size: 12pt;");
        // 恢复其他标签
        xLabelTextB->setStyleSheet("font-weight: normal;");
        yLabelTextB->setStyleSheet("font-weight: normal;");
        xLabelTextC->setStyleSheet("font-weight: normal;");
        yLabelTextC->setStyleSheet("font-weight: normal;");
    }

    xLabelTextA->setText("  X  :" + QString::number(localPoint.x(),'f',4));
    yLabelTextA->setText("  Y  :" + QString::number(localPoint.y(),'f',4));

    if(isMousePressed && airfoilIndex>0) {
        if(returnDesignCSTNum(localPoint)){
            changeDesignCSTControlPoint(realy);
        }
    }


}
void airfoilDisplay::updateMousePosition(const QPointF &localPoint){



    //double realx = localPoint.x();
    double realy = localPoint.y();

    //QString pointFstring = QString("X: %1\nY: %2").arg(realx,0,'f',4).arg(realy,0,'f',4);


    //mouseLocitionA->setText(pointFstring);


    if(isMousePressed&&!newAirfoil.isEmpty()) {
        if(returnCstNum(localPoint)){
            changeCstControlPoint(realy);
        }

    }
}
void airfoilDisplay::updateResultAMousePosition(const QPointF &localPoint){
    if(resultBIndex < 7){
        if (xLabelTextB->styleSheet() != "font-weight: bold; font-size: 12pt;") {
            xLabelTextB->setStyleSheet("font-weight: bold; font-size: 12pt;"); // 调整 pt 值
            yLabelTextB->setStyleSheet("font-weight: bold; font-size: 12pt;");
            // 恢复其他标签
            xLabelTextA->setStyleSheet("font-weight: normal;");
            yLabelTextA->setStyleSheet("font-weight: normal;");
            xLabelTextC->setStyleSheet("font-weight: normal;");
            yLabelTextC->setStyleSheet("font-weight: normal;");
        }
        /*
        if(resultAIndex != 6){
            xLabelTextB->setText(axisXName[resultAIndex] + "  : " + QString::number(localPoint.x(),'f',4));
            yLabelTextB->setText(axisYName[resultAIndex] + " : " + QString::number(localPoint.y(),'f',4));
        }else{
            xLabelTextB->setText(axisYName[2] + "  : " + QString::number(localPoint.x(),'f',4));
            yLabelTextB->setText(axisYName[1] + " : " + QString::number(localPoint.y(),'f',4));
        }
        */
        xLabelTextB->setText("  X  :" + QString::number(localPoint.x(),'f',4));
        yLabelTextB->setText("  Y  :" + QString::number(localPoint.y(),'f',4));
    }
}
void airfoilDisplay::updateResultBMousePosition(const QPointF &localPoint){
    if(resultBIndex < 7){
        if (xLabelTextC->styleSheet() != "font-weight: bold; font-size: 12pt;") {
            xLabelTextC->setStyleSheet("font-weight: bold; font-size: 12pt;"); // 调整 pt 值
            yLabelTextC->setStyleSheet("font-weight: bold; font-size: 12pt;");
            // 恢复其他标签
            xLabelTextA->setStyleSheet("font-weight: normal;");
            yLabelTextA->setStyleSheet("font-weight: normal;");
            xLabelTextB->setStyleSheet("font-weight: normal;");
            yLabelTextB->setStyleSheet("font-weight: normal;");
        }


        /*
        if(resultBIndex != 6){
            xLabelTextC->setText(axisXName[resultBIndex] + "  : " + QString::number(localPoint.x(),'f',4));
            yLabelTextC->setText(axisYName[resultBIndex] + " : " + QString::number(localPoint.y(),'f',4));
        }else{
            xLabelTextC->setText(axisXName[6] + "  : " + QString::number(localPoint.x(),'f',4));
            yLabelTextC->setText(axisYName[6] + " : " + QString::number(localPoint.y(),'f',4));
        }
        */

        xLabelTextC->setText("  X  :" + QString::number(localPoint.x(),'f',4));
        yLabelTextC->setText("  Y  :" + QString::number(localPoint.y(),'f',4));
    }
}
bool airfoilDisplay::returnDesignCSTNum(const QPointF position){


    bool isinside = false;

    int length = airfoilDesignModelArray[choiceIndex]->cstNum;
    int index = (position.x() + 0.02) * (length - 1);
    if(position.x()>cstPointXArray[choiceIndex][index] - 0.02&&position.x()<cstPointXArray[choiceIndex][index] + 0.02){
        isinside = true;
        if(position.y() >= 0){
            cstnum = index;
        }
        else{
            cstnum =  length * 2 - 1 - index;
        }
    }
    else{
        isinside = false;
    }

    return isinside;
}

bool airfoilDisplay::returnCstNum(const QPointF position){

    int  index;
    bool isinside = false;

    int length = cstNum;
    index = (position.x() + 0.02) * (length - 1);
    if(position.x()>cstPointX[index] - 0.02&&position.x()<cstPointX[index] + 0.02){
        isinside = true;
        if(position.y() >= 0){
            cstnum = index;
        }
        else{
            cstnum =  length * 2 - 1 - index;
        }
    }
    else{
        isinside = false;
    }

    return isinside;
}

void airfoilDisplay::changeCstControlPoint(double s){


    if(!newAirfoil.isEmpty()){              //判断series是否被定义
        double ds;
        double cstRadio = 1.2;
        ds = s - cstPointY[cstnum];
        ds = ds * cstRadio;
        cstPointY[cstnum] = s;
        cst[cstnum] += ds;
        designModel->buildAirfoilCurve(cst);
        newAirfoil = designModel->newAirfoilData;

        drawAirfoil();
    }


}
void airfoilDisplay::getMousePressBeginPosition(const QPointF &beginPoint){

    //mouseStartPosition = beginPoint;
    isMousePressed = true;
}

void airfoilDisplay::getMouseReleaseEndPosition(const QPointF &endPoint){


    isMousePressed = false;
    // if(chose >= 1)
    //     updateResultChart();
}

void airfoilDisplay::changeDesignProgressBar(const int value){

    emit emitAirfoilProgressBarValue(value);
}
void airfoilDisplay::changeOptimizationProgressBar(const int value){

    emit emitAirfoilProgressBarValue(value);
}
void airfoilDisplay::changeAnalyseProgressBar(const int value){
    emit emitAirfoilProgressBarValue(value);
}
void airfoilDisplay::setDesignButtonEnabled(const int index){
    for(int i = 0;i < airfoilButton.length();i++){
        if(i != index){
            airfoilButton[i]->setEnabled(true);
            airfoilCheckButton[i]->setEnabled(false);
            airfoilRemoveButton[i]->setEnabled(false);
        }
    }

    airfoilButton[index]->setEnabled(false);
    airfoilCheckButton[index]->setEnabled(true);
    airfoilRemoveButton[index]->setEnabled(true);

}
void airfoilDisplay::changeResultPen(const int index){
    if(airfoilButton.length() > 0){
        resultPenArray[choiceIndex] = index;
        drawDesignResult(choiceIndex);
    }

}
void airfoilDisplay::changeAirfoilName(const QString name){
    if(!airfoilButton.isEmpty()){
        airfoilButton[choiceIndex]->setText(name);
        airfoilNameArray[choiceIndex] = name;
    }
}

void airfoilDisplay::choiceAirfoilButton(){
    QPushButton *button = qobject_cast<QPushButton*>(sender());
    int index = button->property("airfoil").toInt();
    choiceIndex = index;
    alphaCombobox->blockSignals(true);
    alphaCombobox->clear();
    for(int i = 0;i<airfoilSolveModelArray[index]->resultData.length();i++)
        alphaCombobox->addItem(QString::number(airfoilSolveModelArray[index]->resultData[i][0]));
    alphaCombobox->blockSignals(false);
    checkAirfoil = historyAirfoilArray[index];
    setDesignButtonEnabled(index);

    //updateDesignTextView(airfoilDesignModelArray[index]);
    updateDesignTextViewFormXfoil(airfoilXfoilArray[choiceIndex]);
    button = nullptr;
    drawDesignAirfoil(index);

    if(airfoilSolveModelArray[index]->resultData.length() > activeAlphaIndex){
        changeTransitionLocation(activeAlphaIndex);
        changeDesignPoint(activeAlphaIndex);
        changeBlrSeries(activeAlphaIndex);
    }







}
void airfoilDisplay::showColorDialog(){
    QPushButton *button = qobject_cast<QPushButton *>(sender());
    int index = button->property("color").toInt();
    choiceIndex = index;

    if (button) {        
        colorDialog->show();
    }
}

void airfoilDisplay::showCheckAirfoilDialog(){

    checkCSTDialog->show();
    drawCheckCSTAirfoil();

}
void airfoilDisplay::showAirfoilBlendingDialog(){



    blendingReadyA = false;
    blendingReadyB = false;
    blendingComboboxA->blockSignals(true);
    blendingComboboxB->blockSignals(true);
    blendingTextLabelA->setText("");
    blendingTextLabelB->setText("");
    blendingTextLabelC->setText("");

    if(blendingSeriesA->count() > 0)
        blendingSeriesA->clear();
    if(blendingSeriesB->count() > 0)
        blendingSeriesB->clear();
    if(blendingSeriesC->count() > 0)
        blendingSeriesC->clear();

    int len = blendingComboboxA->count();
    for(int i = 1;i<len;i++){
        blendingComboboxA->removeItem(len - i);
        blendingComboboxB->removeItem(len - i);
    }

    if(!airfoilNameArray.isEmpty()){
        for(int i = 0;i<airfoilNameArray.length();i++){
            blendingComboboxA->addItem(airfoilNameArray[i]);
            blendingComboboxB->addItem(airfoilNameArray[i]);
        }
    }
    blendingComboboxA->blockSignals(false);
    blendingComboboxB->blockSignals(false);

    blendingDialog->show();
}
void airfoilDisplay::changeBlendingAirfoilA(const int index){
    if(!airfoilArray[index - 1].isEmpty()){
        if(blendingSeriesA->count() > 0)
            blendingSeriesA->clear();
        for(int i = 0;i<airfoilArray[index - 1].length();i++){
            blendingSeriesA->append(airfoilArray[index - 1][i][0],airfoilArray[index - 1][i][1]);
        }
        blendingReadyA = true;
        updateBlendingAirfoil();
    }
}
void airfoilDisplay::changeBlendingAirfoilB(const int index){
    if(!airfoilArray[index - 1].isEmpty()){
        if(blendingSeriesB->count() > 0)
            blendingSeriesB->clear();
        for(int i = 0;i<airfoilArray[index - 1].length();i++){
            blendingSeriesB->append(airfoilArray[index - 1][i][0],airfoilArray[index - 1][i][1]);
        }
        blendingReadyB = true;
        updateBlendingAirfoil();
    }
}
void airfoilDisplay::updateOptimizationTextItem(const int x,const int y){
   //QRectF textRect = optimizationChartViewA->textItem->boundingRect();

    // 计算位置
    qreal xPosition = x - 150;
    qreal yPosition = 50;

    // Update the position of the text item
    optimizationChartViewA->textItem->setPos(xPosition, yPosition);

    optimizationChartViewA->update();  // Ensure the view is updated
}
void airfoilDisplay::updateDesignTextItem(const int x,const int y){
    // 设置文本内容

    QRectF textRect = designChartViewA->textItem->boundingRect();

        // 计算位置
    qreal xPosition = (x - textRect.width()) / 2;
    qreal yPosition = y - textRect.height() - 40;

        // Update the position of the text item
    designChartViewA->textItem->setPos(xPosition, yPosition);
    adjustChartViewAspectRatio(designChartViewA);

    designChartViewA->update();  // Ensure the view is updated
}


void airfoilDisplay::updateAxes(AirfoilPlot *chart, const QVector<AirfoilPlotSeries*> &seriesList) {
    if (seriesList.isEmpty()) return;

    // 初始化数据范围，设置为一个极端值以便于后续比较
    qreal minX = std::numeric_limits<qreal>::max();
    qreal maxX = std::numeric_limits<qreal>::lowest();
    qreal minY = std::numeric_limits<qreal>::max();
    qreal maxY = std::numeric_limits<qreal>::lowest();

    // 遍历所有曲线数据，计算总体范围
    for (AirfoilPlotSeries *series : seriesList) {
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
    QVector<AirfoilPlotAxis*> axesX = chart->axes(Qt::Horizontal);
    QVector<AirfoilPlotAxis*> axesY = chart->axes(Qt::Vertical);

    if (!axesX.isEmpty() && !axesY.isEmpty()) {
        AirfoilPlotAxis *axisX = axesX.first();
        AirfoilPlotAxis *axisY = axesY.first();

        if (axisX && axisY) {
            axisX->setRange(minX, maxX);
            axisY->setRange(minY, maxY);
        }
    }
}
void airfoilDisplay::updateBlendingAirfoil(){
    if(blendingReadyA&&blendingReadyB){
        int cstNUM = 6;
        int index1 = blendingComboboxA->currentIndex() - 1;
        int index2 = blendingComboboxB->currentIndex() - 1;
        double ratio = double(blendingSlider->value()) / 100;

        airfoilDesign design1(cstNUM);
        airfoilDesign design2(cstNUM);
        airfoilDesign design3(cstNUM);
        design1.computeOtherParameters(airfoilArray[index1]);
        design2.computeOtherParameters(airfoilArray[index2]);

        //design1.buildBenrnstein(airfoilArray[index1]);
        //design2.buildBenrnstein(airfoilArray[index2]);
        double blendingYte1 = (design1.yTe1 + design2.yTe1) / 2;
        double blendingYte2 = (design1.yTe2 + design2.yTe2) / 2;
        QVector<double>cstTmp;
        for(int i = 0;i<cstNUM * 2;i++){
            cstTmp.append(design1.cstParameter[i] * (1 - ratio) + design2.cstParameter[i] * ratio);
        }
        design3.blendingAirfoil(cstTmp,blendingYte1,blendingYte2);
        design3.computeSimpleParameters(design3.newAirfoilData);
        if(blendingSeriesC->count() > 0)
            blendingSeriesC->clear();
        for(int i = 0;i<design3.newAirfoilData.length();i++)
            blendingSeriesC->append(design3.newAirfoilData[i][0],design3.newAirfoilData[i][1]);

        QString tmpA1 = QString("最大厚度: %1%  最大厚度位置: %2%    ").arg(design1.maxThickness * 100,0,'f',1).arg(design1.locationThickness * 100,0,'f',1);
        QString tmpA2 = QString("最大弯度: %1%  最大弯度位置: %2%    ").arg(design1.maxCamber * 100,0,'f',1).arg(design1.locationCamber * 100,0,'f',1);
        QString tmpA3 = QString("前缘半径: %1%  后缘夹角: %2").arg(design1.minRadius * 100,0,'f',1).arg(design1.trailingAngle,0,'f',2);
        QString tmpB1 = QString("最大厚度: %1%  最大厚度位置: %2%    ").arg(design2.maxThickness * 100,0,'f',1).arg(design2.locationThickness * 100,0,'f',1);
        QString tmpB2 = QString("最大弯度: %1%  最大弯度位置: %2%    ").arg(design2.maxCamber * 100,0,'f',1).arg(design2.locationCamber * 100,0,'f',1);
        QString tmpB3 = QString("前缘半径: %1%  后缘夹角: %2").arg(design2.minRadius * 100,0,'f',1).arg(design2.trailingAngle,0,'f',2);
        QString tmpC1 = QString("最大厚度: %1%  最大厚度位置: %2%    ").arg(design3.maxThickness * 100,0,'f',1).arg(design3.locationThickness * 100,0,'f',1);
        QString tmpC2 = QString("最大弯度: %1%  最大弯度位置: %2%    ").arg(design3.maxCamber * 100,0,'f',1).arg(design3.locationCamber * 100,0,'f',1);
        QString tmpC3 = QString("前缘半径: %1%  后缘夹角: %2").arg(design3.minRadius * 100,0,'f',1).arg(design3.trailingAngle,0,'f',2);
        QString tmp1 = tmpA1 + tmpA2 + tmpA3;
        QString tmp2 = tmpB1 + tmpB2 + tmpB3;
        QString tmp3 = tmpC1 + tmpC2 + tmpC3;
        blendingTextLabelA->setText(tmp1);
        blendingTextLabelB->setText(tmp2);
        blendingTextLabelC->setText(tmp3);
        blendingValueLabelA->setText(QString::number(1 - ratio));
        blendingValueLabelB->setText(QString::number(ratio));


        blendingAirfoilData = design3.newAirfoilData;
    }
}

void airfoilDisplay::hideBlendingAirfoil(){
    QCheckBox *checkbox = qobject_cast<QCheckBox*>(sender());
    int tmp = checkbox->property("index").toInt();

    switch (tmp) {
    case 0:
        if(blendingSeriesA->isVisible())
            blendingSeriesA->setVisible(false);
        else
            blendingSeriesA->setVisible(true);


        break;
    case 1:
        if(blendingSeriesB->isVisible())
            blendingSeriesB->setVisible(false);
        else
            blendingSeriesB->setVisible(true);

        break;
    case 2:
        if(blendingSeriesC->isVisible())
            blendingSeriesC->setVisible(false);
        else
            blendingSeriesC->setVisible(true);
        break;
    default:
        break;
    }
}
void airfoilDisplay::changeAirfoilColor(const QColor& color){
    airfoilColorArray[choiceIndex] = color;
    QString colorStyle = QString("background-color: %1;").arg(color.name());
    airfoilColorButton[choiceIndex]->setStyleSheet(colorStyle);
    //drawDesignAirfoil(choiceIndex);
    drawDesignResult(choiceIndex);
    drawDesignAirfoil(choiceIndex);
}
QPen airfoilDisplay::getPen(const int index){
    QPen pen;
    pen.setColor(airfoilColorArray[index]);
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
void airfoilDisplay::addDataPointA()
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
void airfoilDisplay::addDataPointB()
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
void airfoilDisplay::clearDataPointA()
{
    // Clear all data points from the autoSeriesA and autoSeriesB
    autoSeriesA->clear();
    //autoSeriesB->clear();
    updateAxes(resultChartA,resultSeriesA);

}
void airfoilDisplay::clearDataPointB()
{
    // Clear all data points from the autoSeriesA and autoSeriesB
    autoSeriesB->clear();
    //autoSeriesB->clear();
    updateAxes(resultChartB,resultSeriesB);

}
int airfoilDisplay::findLeadingEdgeIndex(
    int index,
    const QVector<QVector<QVector<double>>>& airfoilArray)
{
    const auto& airfoil = airfoilArray[index];

    if (airfoil.isEmpty())
        return -1;

    int minIndex = 0;
    double minX = airfoil[0][0];

    for (int i = 1; i < airfoil.size(); ++i) {
        if (airfoil[i][0] < minX) {
            minX = airfoil[i][0];
            minIndex = i;
        }
    }

    return minIndex;
}
void airfoilDisplay::showAirfoilExplorerWidget(){
    explorer->show();
}
void airfoilDisplay::modifyCamberAndThickness(double value1,double value2,QVector<QVector<double>>&airfoilData){
    //999  Nothing!
    double x[600],y[600],nx[600],ny[600];
    int n = airfoilData.size();


    for(int i = 0; i < n;i++){
        x[i] = airfoilData[i][0];
        y[i] = airfoilData[i][1];

    }
    XFoil *foil = new XFoil();
    foil->initXFoilGeometry(n, x, y, nx, ny);



    foil->tcset(value1,value2);
    foil->pangen();



    airfoilData.clear();

    for (int i = 1; i <= foil->nb; i++) {   // 注意 XFOIL 通常是 1-based
        QVector<double> pt(2);
        pt[0] = foil->xb[i];
        pt[1] = foil->yb[i];
        airfoilData.push_back(pt);
    }
    delete foil;
}
void airfoilDisplay::modifyCamberAndThicknessLocation(double value1,double value2,QVector<QVector<double>>&airfoilData){
    // 0.0  Nothing!
    double x[600],y[600],nx[600],ny[600];
    int n = airfoilData.size();
    QVector<double>tmp;

    for(int i = 0; i < n;i++){
        x[i] = airfoilData[i][0];
        y[i] = airfoilData[i][1];

    }
    XFoil *foil = new XFoil();
    foil->initXFoilGeometry(n, x, y, nx, ny);
    foil->hipnt(value1,value2);
    foil->pangen();

    airfoilData.clear();

    for (int i = 1; i <= foil->nb; i++) {   // 注意 XFOIL 通常是 1-based
        QVector<double> pt(2);
        pt[0] = foil->xb[i];
        pt[1] = foil->yb[i];
        airfoilData.push_back(pt);
    }
    delete foil;
}
void airfoilDisplay::modifyCamber(double value){
    modifyCamberAndThickness(value,999,airfoilArray[choiceIndex]);
    drawDesignAirfoil(choiceIndex);

    airfoilDesignModelArray[choiceIndex]->buildBenrnstein(airfoilArray[choiceIndex]);
    cstArray[choiceIndex] = airfoilDesignModelArray[choiceIndex]->cstParameter;
    //airfoilDesignModelArray[choiceIndex]->computeOtherParameters(airfoilArray[choiceIndex]);

    updateDesignTextViewFormXfoil(airfoilXfoilArray[choiceIndex]);
    //updateDesignTextView(airfoilDesignModelArray[choiceIndex]);

}
void airfoilDisplay::modifyThickness(double value){
    modifyCamberAndThickness(999,value,airfoilArray[choiceIndex]);
    drawDesignAirfoil(choiceIndex);

    airfoilDesignModelArray[choiceIndex]->buildBenrnstein(airfoilArray[choiceIndex]);
    cstArray[choiceIndex] = airfoilDesignModelArray[choiceIndex]->cstParameter;
    //airfoilDesignModelArray[choiceIndex]->computeOtherParameters(airfoilArray[choiceIndex]);

    //updateDesignTextView(airfoilDesignModelArray[choiceIndex]);

    updateDesignTextViewFormXfoil(airfoilXfoilArray[choiceIndex]);
}
void airfoilDisplay::modifyCamberLocation(double value){
    modifyCamberAndThicknessLocation(value,0.0,airfoilArray[choiceIndex]);
    drawDesignAirfoil(choiceIndex);

    airfoilDesignModelArray[choiceIndex]->buildBenrnstein(airfoilArray[choiceIndex]);
    cstArray[choiceIndex] = airfoilDesignModelArray[choiceIndex]->cstParameter;
    //airfoilDesignModelArray[choiceIndex]->computeOtherParameters(airfoilArray[choiceIndex]);

    //updateDesignTextView(airfoilDesignModelArray[choiceIndex]);

    updateDesignTextViewFormXfoil(airfoilXfoilArray[choiceIndex]);
}
void airfoilDisplay::modifyThicknessLocation(double value){
    modifyCamberAndThicknessLocation(0.0,value,airfoilArray[choiceIndex]);
    drawDesignAirfoil(choiceIndex);

    airfoilDesignModelArray[choiceIndex]->buildBenrnstein(airfoilArray[choiceIndex]);
    cstArray[choiceIndex] = airfoilDesignModelArray[choiceIndex]->cstParameter;
    //airfoilDesignModelArray[choiceIndex]->computeOtherParameters(airfoilArray[choiceIndex]);

    //updateDesignTextView(airfoilDesignModelArray[choiceIndex]);

    updateDesignTextViewFormXfoil(airfoilXfoilArray[choiceIndex]);
}
void airfoilDisplay::modifyRadius(double value){
    modifyRadiusAndInfluence(value,0.1,airfoilArray[choiceIndex]);
    drawDesignAirfoil(choiceIndex);

    airfoilDesignModelArray[choiceIndex]->buildBenrnstein(airfoilArray[choiceIndex]);
    cstArray[choiceIndex] = airfoilDesignModelArray[choiceIndex]->cstParameter;
    //airfoilDesignModelArray[choiceIndex]->computeOtherParameters(airfoilArray[choiceIndex]);

    //updateDesignTextView(airfoilDesignModelArray[choiceIndex]);

    updateDesignTextViewFormXfoil(airfoilXfoilArray[choiceIndex]);
}
void airfoilDisplay::modifyRadiusInfluence(double value){
    modifyRadiusAndInfluence(1,value,airfoilArray[choiceIndex]);
    drawDesignAirfoil(choiceIndex);

    airfoilDesignModelArray[choiceIndex]->buildBenrnstein(airfoilArray[choiceIndex]);
    cstArray[choiceIndex] = airfoilDesignModelArray[choiceIndex]->cstParameter;
    //airfoilDesignModelArray[choiceIndex]->computeOtherParameters(airfoilArray[choiceIndex]);

    //updateDesignTextView(airfoilDesignModelArray[choiceIndex]);

    updateDesignTextViewFormXfoil(airfoilXfoilArray[choiceIndex]);
}
void airfoilDisplay::modifyRadiusAndInfluence(double value1,double value2,QVector<QVector<double>>&airfoilData){

    //0.1
    double x[600],y[600],nx[600],ny[600];
    int n = airfoilData.size();
    QVector<double>tmp;

    for(int i = 0; i < n;i++){
        x[i] = airfoilData[i][0];
        y[i] = airfoilData[i][1];

    }
    XFoil *foil = new XFoil();
    foil->initXFoilGeometry(n, x, y, nx, ny);
    if(value2 < 0.1)
        value2 = 0.1;
    if(value1 < 0.02)
        value1 = 0.02;
    foil->lerad(value1,value2);
    foil->pangen();

    airfoilData.clear();

    for (int i = 1; i <= foil->nb; i++) {   // 注意 XFOIL 通常是 1-based
        QVector<double> pt(2);
        pt[0] = foil->xb[i];
        pt[1] = foil->yb[i];
        airfoilData.push_back(pt);
    }
    delete foil;

}

void airfoilDisplay::initialXfoil(XFoil *foil, QVector<QVector<double>>airfoilData){
    double x[600],y[600],nx[600],ny[600];
    int n = airfoilData.size();
    QVector<double>tmp;

    for(int i = 0; i < n;i++){
        x[i] = airfoilData[i][0];
        y[i] = airfoilData[i][1];

    }
    foil->initXFoilGeometry(n, x, y, nx, ny);
}
void airfoilDisplay::resetAirfoil(){
    airfoilDesign *design = airfoilDesignModelArray[choiceIndex];
    design->buildBenrnstein(resetAirfoilArray);
    airfoilArray[choiceIndex] = resetAirfoilArray;

    cstArray[choiceIndex] = design->cstParameter;
    //design->computeOtherParameters(resetAirfoilArray);


    updateDesignTextViewFormXfoil(airfoilXfoilArray[choiceIndex]);

    thicknessSpin->blockSignals(true);
    camberSpin->blockSignals(true);
    thkPosSpin->blockSignals(true);
    camPosSpin->blockSignals(true);

    thicknessSpin->setValue(airfoilXfoilArray[choiceIndex]->maxThickness);
    camberSpin->setValue(airfoilXfoilArray[choiceIndex]->maxCamber);
    thkPosSpin->setValue(airfoilXfoilArray[choiceIndex]->locationThickness);
    camPosSpin->setValue(airfoilXfoilArray[choiceIndex]->locationCamber);

    thicknessSpin->blockSignals(false);
    camberSpin->blockSignals(false);
    thkPosSpin->blockSignals(false);
    camPosSpin->blockSignals(false);

    //updateDesignTextView(design);
    drawDesignAirfoil(choiceIndex);


}
void airfoilDisplay::showModifyAirfoilDialog(){
   updateDesignTextViewFormXfoil(airfoilXfoilArray[choiceIndex]);
   //airfoilDesign *design = airfoilDesignModelArray[choiceIndex];
   resetAirfoilArray = airfoilArray[choiceIndex];

   thicknessSpin->blockSignals(true);
   camberSpin->blockSignals(true);
   thkPosSpin->blockSignals(true);
   camPosSpin->blockSignals(true);

   thicknessSpin->setValue(airfoilXfoilArray[choiceIndex]->maxThickness);
   camberSpin->setValue(airfoilXfoilArray[choiceIndex]->maxCamber);
   thkPosSpin->setValue(airfoilXfoilArray[choiceIndex]->locationThickness);
   camPosSpin->setValue(airfoilXfoilArray[choiceIndex]->locationCamber);

   thicknessSpin->blockSignals(false);
   camberSpin->blockSignals(false);
   thkPosSpin->blockSignals(false);
   camPosSpin->blockSignals(false);
   //leRadiusSpin->setValue(design->minRadius);

   airfoilModifyDialog->exec();

}

airfoilDisplay::~airfoilDisplay(){

    delete exp;
    delete designModel;
    for(airfoilSolve *solver:airfoilSolveModelArray)
        delete solver;
    for(XFoil *solver:airfoilXfoilArray)
        delete solver;
    for(airfoilDesign *solver:airfoilDesignModelArray)
        delete solver;
    for(AirfoilPlotSeries* series:dragSeriesArray)
        delete series;
    dragSeriesArray.clear();


}
