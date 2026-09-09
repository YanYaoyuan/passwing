
#include "wingdisplay.h"
#include <QSplitter>
#include <QDir>
#include <QFuture>
#include <QtConcurrent/QtConcurrent>
#include <QCoreApplication>
#include <QMessageBox>
#include <QScopedValueRollback>
#include <QFileDialog>
#include <QtCore/QTextStream>
#include <QApplication>
#include <QButtonGroup>
#include <QThread>
#include <iostream>
#include <algorithm>
#include <cmath>
#include <limits>
#include "widgets/datapointdialog.h"
#include "vtkCellData.h"
#include "dynamics/torsionalspring.h"

#if defined(_MSC_VER) && (_MSC_VER >= 1600)
# pragma execution_character_set("utf-8")
#endif


wingDisplay::wingDisplay(QWidget *parent)
    : QWidget(parent){


    // 创建主窗口部件
    //wingDesignWidget = new QWidget(this);
    wingDefineWidget = new QWidget();
    defineAirfoil = new airfoilDisplay(cstNum);
    initialSteup();
    initialRChartMenu();
    initialSpanRChartMenu();



    initialSeriesStyleDialog();
    initialWingDesignerModel();
    initialWingMessageBox();
    initialWingIterWidget();
    initialWingDesignWidget();
    initialWingOptimizationWidget();
    initialWingDefineWidget();
    initialWingDefineDialog();
    initialStreamDialog();
    initialChoiceDragDialog();
    initialWingShowModel();
    initialProgressDialog();
    initialTailDefineDialog();


    //翼型更新
    connect(defineAirfoil,&airfoilDisplay::emitAirfoilData,this,&wingDisplay::changeAirfoilData);
    connect(defineAirfoil,&airfoilDisplay::emitClicked,this,&wingDisplay::resetAirfoilData);

    //chart左键
    QObject::connect(chartViewWA,&PlotWidget::mouseRightLeftPress,this,&wingDisplay::showChartAMenu);
    QObject::connect(chartViewWB,&PlotWidget::mouseRightLeftPress,this,&wingDisplay::showChartBMenu);
    QObject::connect(chartViewWC,&PlotWidget::mouseRightLeftPress,this,&wingDisplay::showChartCMenu);
    QObject::connect(chartViewWD,&PlotWidget::mouseRightLeftPress,this,&wingDisplay::showChartDMenu);
    QObject::connect(spanChartView,&PlotWidget::mouseRightLeftPress,this,&wingDisplay::showSpanChartMenu);

    connect(iterViewA,&PlotWidget::chartResized,this,&wingDisplay::updateOptimizationTextItem);
    //进度条更新


}
void wingDisplay::addWingData(wingDefinition &newWingData){



    wingVLM *solver = new wingVLM;
    solver->initialGeometry(newWingData);
    VLMSolverArray.append(solver);

    newWingData.computeWingMessage();
    newWingData.interDataArray = interDataArray;



    QString path1 = ":/images/edit.png";
    QIcon icon1 ;
    icon1.addPixmap(QPixmap(path1),QIcon::Normal,QIcon::On);

    QString path2 = ":/images/wing.png";
    QIcon icon2 ;
    icon2.addPixmap(QPixmap(path2),QIcon::Normal,QIcon::On);

    QTreeWidgetItem *fItem = new QTreeWidgetItem(wingTreeWidget,QStringList(QString(newWingData.name)));
    fItem->setData(0,Qt::UserRole, QVariant(QString::number(wingIndex)));
    fItem->setIcon(0,icon2);

    QTreeWidgetItem *fItemGeometry = new QTreeWidgetItem(fItem,QStringList("几何模型"));
    fItemGeometry->setData(0,Qt::UserRole,"fItemGeometry");

    QTreeWidgetItem *childShape = new QTreeWidgetItem(fItemGeometry,QStringList("平面形状"));
    childShape->setData(0, Qt::UserRole, QVariant("childShape" + QString::number(wingIndex)));
    childShape->setIcon(0,icon1);

    QTreeWidgetItem *childAirfoil = new QTreeWidgetItem(fItemGeometry,QStringList("翼型"));
    childAirfoil->setData(0, Qt::UserRole, QVariant("childAirfoil" + QString::number(wingIndex)));
    for(int i = 0;i<newWingData.airfoilNameArray.length();i++){
        QTreeWidgetItem *childAirfoilTmp = new QTreeWidgetItem(childAirfoil,QStringList(newWingData.airfoilNameArray[i]));
        childAirfoilTmp->setData(0,Qt::UserRole,QVariant("airfoil" + QString::number(i)));
        childAirfoilTmp->setIcon(0,icon1);
    }
    QTreeWidgetItem *fItemAnalyse = new QTreeWidgetItem(fItem,QStringList("求解设置"));
    fItemAnalyse->setData(0, Qt::UserRole, QVariant("childSolver" + QString::number(wingIndex)));
    //fItemAnalyse->setData(0, Qt::UserRole, "fItemAnalyse");

    QTreeWidgetItem *childVinf = new QTreeWidgetItem(fItemAnalyse,QStringList("来流设置"));
    childVinf->setData(0, Qt::UserRole, QVariant("childVinf" + QString::number(wingIndex)));
    childVinf->setIcon(0,icon1);

    QTreeWidgetItem *childZeroLiftDrag = new QTreeWidgetItem(fItemAnalyse,QStringList("零升阻力"));
    childZeroLiftDrag->setData(0, Qt::UserRole, QVariant("childZeroLiftDrag" + QString::number(wingIndex)));
    childZeroLiftDrag->setCheckState(0,Qt::Unchecked);


    QTreeWidgetItem *childXfoilDrag = new QTreeWidgetItem(fItemAnalyse,QStringList("Xfoil阻力"));
    childXfoilDrag->setData(0, Qt::UserRole, QVariant("childXfoilDrag" + QString::number(wingIndex)));
    childXfoilDrag->setCheckState(0,Qt::Unchecked);


    //childZeroLiftDrag->setData(0, Qt::UserRole, QVariant(QString::number(wingIndex) + "childZeroLiftDrag"));

    QTreeWidgetItem *fItemResult = new QTreeWidgetItem(fItem,QStringList("后处理"));
    QTreeWidgetItem *childPrssure = new QTreeWidgetItem(fItemResult,QStringList("压力"));
    childPrssure->setData(0, Qt::UserRole, QVariant("childPrssure" + QString::number(wingIndex)));
    childPrssure->setCheckState(0,Qt::Unchecked);

    QTreeWidgetItem *childStreamLine = new QTreeWidgetItem(fItemResult,QStringList("流线"));
    childStreamLine->setData(0, Qt::UserRole, QVariant("childStreamLine" + QString::number(wingIndex)));
    childStreamLine->setCheckState(0,Qt::Unchecked);


    QTreeWidgetItem *childSeriesType = new QTreeWidgetItem(fItemResult,QStringList("曲线类型"));
    childSeriesType->setData(0, Qt::UserRole, QVariant("childSeriesType" + QString::number(wingIndex)));
    childSeriesType->setIcon(0,icon1);


    fItemResult->setHidden(true);




    // 连接信号和槽

    wingTreeItemArray.append(fItem);


    int r =rand()%255;
    int b =rand()%255;
    int g =rand()%255;

    QColor color(r,b,g);

    colorArray.append(color);
    resultPenArray.append(0);
    PlotSeries *seriesA = new PlotSeries;
    PlotSeries *seriesB = new PlotSeries;
    PlotSeries *seriesC = new PlotSeries;
    PlotSeries *seriesD = new PlotSeries;


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


    wingResultSeriesA.append(seriesA);
    wingResultSeriesB.append(seriesB);
    wingResultSeriesC.append(seriesC);
    wingResultSeriesD.append(seriesD);


    seriesA->setName(newWingData.name);
    seriesB->setName(newWingData.name);
    seriesC->setName(newWingData.name);
    seriesD->setName(newWingData.name);
    spanSeriesA->setName(newWingData.name);

    QVector<QVector<double>>tmp;
    resultArray.append(tmp);
    VLMSetting setting(0,10,1,20,0,0);
    wingSettingArray.append(setting);

    wingListCombox->addItem(newWingData.name);//添加机翼到列表

    //导入优化参数
    WingGaParameters sett;


    QVector<double>minChord;QVector<double>minOffset;QVector<double>minTwist;QVector<double>minDihed;
    QVector<double>maxChord;QVector<double>maxOffset;QVector<double>maxTwist;QVector<double>maxDihed;
    QVector<bool>airfoilBool;QVector<bool>spanBool;QVector<bool>chordBool;QVector<bool>offsetBool;QVector<bool>twistBool;QVector<bool>dihedBool;

    connect(solver, &wingVLM::progressUpdated, this, &wingDisplay::updateProgress);
    connect(solver, &wingVLM::workFinished, this, &wingDisplay::onCalculationFinished);

    for(int i = 0;i<newWingData.chordLengthW.length();i++){
        newWingData.cstPointYArray.append(cstPointY);
        minChord.append(0);
        maxChord.append(newWingData.chordLengthW[i] + 0.5);
        minOffset.append(newWingData.offsetLengthW[i] - 0.5);
        maxOffset.append(newWingData.offsetLengthW[i] + 0.5);
        minTwist.append(newWingData.twistAngleW[i] - 10);
        maxTwist.append(newWingData.twistAngleW[i] + 10);
        minDihed.append(0);
        maxDihed.append(90);

        airfoilBool.append(false);
        spanBool.append(false);
        chordBool.append(true);
        offsetBool.append(true);
        twistBool.append(true);
        dihedBool.append(false);
    }




    wingDataArray.append(newWingData);

    sett.MinSpan = 0;
    sett.MaxSpan = newWingData.Span() + 2;
    sett.minChord = minChord;
    sett.maxChord = maxChord;
    sett.minOffsetLength = minOffset;
    sett.maxOffsetLength = maxOffset;
    sett.minTwisAngle = minTwist;
    sett.maxTwisAngle = maxTwist;
    sett.minDihedralAngle = minDihed;
    sett.maxDihedralAngle = maxDihed;
    sett.spanVal = 0.01;
    sett.chordVal = 0.01;
    sett.offsetLengthVal = 0.01;
    sett.twisAngleVal = 0.01;
    sett.dihedralAngleVal = 0.01;
    sett.airfoilChangeArray = airfoilBool;
    sett.spanChangeArray = spanBool;
    sett.chordChangeArray = chordBool;
    sett.offsetChangeArray = offsetBool;
    sett.twistChangeArray = twistBool;
    sett.dihedChangeArray = dihedBool;


    optSettingArray.append(sett);
    optRealSettingArray.append(sett);
    optRangeChange(wingIndex);//刷新模型优化参数

    wingIndex++;

    connect(solver,&wingVLM::emitProgressValue,this,&wingDisplay::changeProgressUpdate);



    hidePressureContourView();//关闭云图
    updateGLTextA("计算完成！");//刷新模型参数
    updateWingDefineWidget(newWingData);//刷新模型视图

    updateListData();//传递最新的机翼



}
void wingDisplay::addTailData(wingDefinition &newWingData){
    newWingData.computeWingMessage();
    newWingData.interDataArray = interDataArray;
    //vTailDataArray.append(newWingData);



    tailDataArray.append(newWingData);

    QString path1 = ":/images/edit.png";
    QIcon icon1 ;
    icon1.addPixmap(QPixmap(path1),QIcon::Normal,QIcon::On);

    QString path2 = ":/images/wing.png";
    QIcon icon2 ;
    icon2.addPixmap(QPixmap(path2),QIcon::Normal,QIcon::On);

    QTreeWidgetItem *fItem = new QTreeWidgetItem(tailTreeWidget,QStringList(QString(newWingData.name)));
    fItem->setData(0,Qt::UserRole, QVariant(QString::number(tailIndex)));
    fItem->setIcon(0,icon2);

    QTreeWidgetItem *fItemGeometry = new QTreeWidgetItem(fItem,QStringList("几何模型"));
    fItemGeometry->setData(0,Qt::UserRole,"fItemGeometry");

    QTreeWidgetItem *childShape = new QTreeWidgetItem(fItemGeometry,QStringList("平面形状"));
    childShape->setData(0, Qt::UserRole, QVariant("tailChildShape" + QString::number(tailIndex)));
    childShape->setIcon(0,icon1);

    QTreeWidgetItem *childAirfoil = new QTreeWidgetItem(fItemGeometry,QStringList("翼型"));
    childAirfoil->setData(0, Qt::UserRole, QVariant("childAirfoil" + QString::number(tailIndex)));
    for(int i = 0;i<newWingData.airfoilNameArray.length();i++){
        QTreeWidgetItem *childAirfoilTmp = new QTreeWidgetItem(childAirfoil,QStringList(newWingData.airfoilNameArray[i]));
        childAirfoilTmp->setData(0,Qt::UserRole,QVariant("airfoil" + QString::number(i)));
        childAirfoilTmp->setIcon(0,icon1);
    }

    tailTreeItemArray.append(fItem);
    tailChoiceIndex = tailIndex;
    tailIndex++;


    updateGLTextA(wingDataToString(newWingData));//刷新模型参数
    updateWingDefineWidget(newWingData);//刷新模型视图
    updateListData();//传递最新的机翼

}
void wingDisplay::onCheckboxStateChanged(int state) {
    if (state == Qt::Checked) {
        actorCg->VisibilityOn();
    } else {
        actorCg->VisibilityOff();
    }
    wingDisplayWidgetB->renderWindow()->Render();
}
void wingDisplay::updateWingTip(int alphaIndex){
    double torque = VLMSolverArray[wingChoiceIndex]->getChoiceTorque(alphaIndex);
    torsionalSpring model(0,0.1);
    double value = model.getAngele(torque);
    int num = wingDataArray[wingChoiceIndex].dihedralAngleW.length() - 1;
    wingDataArray[wingChoiceIndex].dihedralAngleW[num - 1] = value;
    startAnalyseWing();

    wingAlphaCombox->blockSignals(true);
    wingAlphaCombox->setCurrentIndex(alphaIndex);
    wingAlphaCombox->blockSignals(false);

    qDebug()<<"当前扭矩为:" + QString::number(torque,'f',2);
    qDebug()<<"当前折叠角度为:" + QString::number(value,'f',1);


}
void wingDisplay::updateAirfoilArray(const QVector<QVector<QVector<double>>>&airfoilArrayTmp,const QVector<QString>&nameArray,const QVector<int>&cstNumArrayTmp){

    if(!airfoilArrayTmp.isEmpty() && !nameArray.isEmpty()){

        QStringList boxList;
        for(int i = 0;i<nameArray.length();i++){
            boxList.append(nameArray[i]);
        }
        for(int i = 0;i<profileAirfoilChoiceCombobox.length();i++){
            updateComboBox(profileAirfoilChoiceCombobox[i],boxList);
        }
        airfoilArray = airfoilArrayTmp;;
        //VLMCSTArray = airfoilArrayTmp;
        AirfoilNameArray = nameArray;
        cstNumArray = cstNumArrayTmp;
        updateWingData();


    }
}
void wingDisplay::updateAirfoilInterArray(const QVector<QVector<airfoilData>>&airfoilInterArrayTmp){

    interDataArray = airfoilInterArrayTmp;

}
void wingDisplay::resetAirfoilData(){
    defineAirfoil->hideModifyAirfoilDialogCST();
    wingDataArray[wingChoiceIndex].airfoilArray[airfoilIndex] = wingDataArray[wingChoiceIndex].oldAirfoilArray[airfoilIndex];
    wingDataArray[wingChoiceIndex].cstPointYArray[airfoilIndex] = cstPointY;

}
void wingDisplay::addDataPoint(){
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
void wingDisplay::clearDataPoint(){
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
void wingDisplay::changeResultType(){
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
            updateAxes(chartWA,wingResultSeriesA);
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
            updateAxes(chartWB,wingResultSeriesB);
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
            updateAxes(chartWC,wingResultSeriesC);
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
            updateAxes(chartWD,wingResultSeriesD);
        }else{
            exportChartData(chartWD);
            QMessageBox::information(this,"信息提示","保存成功");
        }


        break;
    default:
        break;
    }

}
void wingDisplay::changeSpanResultType(){
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
        if(spanSeriesB->isVisible())
            spanSeriesB->setVisible(false);
        else
            spanSeriesB->setVisible(true);
    }else{
        // 设置当前动作为选中状态
        if(index != 0)
            spanSeriesB->setVisible(false);
        action->setChecked(true);
        spanResultTypeIndex = index;
        spanXAxis->setTitleText(axisXName[index + 9]);
        spanYAxis->setTitleText(axisYName[index + 9]);
        spanChart->setTitle(titleName[index + 9]);
        drawSpanResult(wingChoiceIndex);


    }




}

void wingDisplay::changeProgressUpdate(const int value){
    emit emitWingProgressBarValue(value);
}
void wingDisplay::startOptimizationWing(){

    if (wingOptimizationRunning) {
        QMessageBox::information(this, "提示", "机翼优化正在运行，请稍候");
        return;
    }
    if(wingDataArray.isEmpty()){
        QMessageBox::information(this,"警告","未添加机翼");
        return;
    }


    QScopedValueRollback<bool> runningGuard(wingOptimizationRunning, true);

    if(!checkOptSetting()){

    }else{
        activeOptimizationWingIndex = wingChoiceIndex;

        wingDefinition newWingData =
            wingDataArray[activeOptimizationWingIndex];

        auto optimizer = std::make_unique<wingOptimization>(optSetting,
                                                             newWingData);
        const QString validationError = optimizer->validationError();
        if (!validationError.isEmpty()) {
            QMessageBox::information(this, "警告", validationError);
            return;
        }
        if (!optimizer->initialElite()) {
            QMessageBox::information(this, "警告", "机翼遗传算法初始化失败");
            return;
        }
        optModel = std::move(optimizer);

        changeUIForWingIter();
        updateGLTextA("开始.......");
        emit emitWingProgressBarValue(0);
        if (iterSeriesA->count() > 0)
            iterSeriesA->clear();
        iterXAxisA->setRange(0, optSetting.step - 1);
        iterYAxisA->setRange(0, 50);

        meshNum = newWingData.MeshNum();
        wingWorkNum = optSetting.initialEliteNum / wingThreadNum;

        int progressValue;

        for(int i = 0;i<optSetting.step;i++){
            for(int j = 0;j<wingWorkNum;j++){
                startVLMInThread(i,j);
                if(i == 0 && j == 0){
                    if (cdArray.isEmpty() || xSpan.isEmpty() ||
                        xSpanLiftA.isEmpty() || xSpanLiftB.isEmpty()) {
                        QMessageBox::information(this, "警告",
                                                 "VLM未返回有效优化结果");
                        return;
                    }
                    drawOptIterSeries(i,cdArray[0]);
                    drawOptSpanLiftSeries(xSpan[0],xSpanLiftA[0],xSpanLiftB[0]);
                }
            }
            const int expectedResults = i == 0
                ? optSetting.initialEliteNum : optSetting.eliteNum;
            if (cdArray.size() != expectedResults ||
                optWingDataArray.size() != expectedResults) {
                QMessageBox::information(this, "警告",
                                         "VLM优化结果数量不完整");
                return;
            }
            if (i >= 1 && optModel->bestSolutionArray.size() >= i) {
                const double previousBest = optModel->bestSolutionArray[i - 1];
                for (double &result : cdArray) {
                    if (!std::isfinite(result) ||
                        (previousBest > 0.0 && result > previousBest * 6.0))
                        result = 0.0;
                }
            } else {
                for (double &result : cdArray) {
                    if (!std::isfinite(result))
                        result = 0.0;
                }
            }



            progressValue = double(i + 1) / optSetting.step * 100;
            emit emitWingProgressBarValue(progressValue);


            optModel->sortResult(cdArray);
            optModel->updateChromomeSequenceDec();

            if (optModel->wingChromosomeSequenceDec.size() !=
                    optSetting.eliteNum ||
                optModel->bestIndex < 0) {
                QMessageBox::information(this, "警告", "机翼优化结果不完整");
                return;
            }

            optModel->selectionChromosomeSequenceDec();

            optModel->decToBin();

            optModel->overlappingOperations();

            optModel->variationOperations();

            optModel->binToDec();



            //updateWingDefineWidget(optModel->bestWingData);
            int index = optModel->bestIndex;
            //曲线显示


            if(i >= 1)
                drawOptIterSeries(i,optModel->bestSolution);
            updateOptIterText(optModel->bestSolution);



            //云图显示
            if(index<colorContourArray.length()){
                //printfWingData(optWingDataArray[index]);
                drawOptSpanLiftSeries(xSpan[index],xSpanLiftA[index],xSpanLiftB[index]);
                bestWingData = optWingDataArray[index];
                updatePressureContourView(xABArray[index],xBCArray[index],xCDArray[index],xDAArray[index],colorContourArray[index],meshNum);
            }

            cdArray.clear();
            xABArray.clear();
            xBCArray.clear();
            xCDArray.clear();
            xDAArray.clear();
            xSpan.clear();
            xSpanLiftA.clear();
            xSpanLiftB.clear();
            colorContourArray.clear();
            optWingDataArray.clear();
            wingWorkNum = optSetting.eliteNum / wingThreadNum;
        }

        finishWingOptimization();


    }
}
void wingDisplay::finishWingOptimization(){
    //保存优化后结果
    updateGLTextA("");
    const int index = activeOptimizationWingIndex;
    if (index < 0 || index >= wingDataArray.size())
        return;
    bestWingData.name = "opt_" + wingDataArray[index].name;
    bestWingData.airfoilArray = wingDataArray[index].airfoilArray;
    bestWingData.cstPointYArray = wingDataArray[index].cstPointYArray;
    bestWingData.airfoilNameArray = wingDataArray[index].airfoilNameArray;
    bestWingData.gridU = wingDataArray[index].gridU;
    bestWingData.gridV = wingDataArray[index].gridV;
    bestWingData.uMeshType = wingDataArray[index].uMeshType;
    bestWingData.vMeshType = wingDataArray[index].vMeshType;
    bestWingData.airfoilInputType = wingDataArray[index].airfoilInputType;
    bestWingData.spanW[0] = 0;


    addWingData(bestWingData);


    // wingResultSeriesA[wingChoiceIndex]->setName(bestWingData.name);
    // wingResultSeriesB[wingChoiceIndex]->setName(bestWingData.name);
    // wingResultSeriesC[wingChoiceIndex]->setName(bestWingData.name);
    // wingResultSeriesD[wingChoiceIndex]->setName(bestWingData.name);
    // spanSeriesA->setName(bestWingData.name);
    // wingTreeItemArray[wingChoiceIndex]->setText(0,bestWingData.name);
    wingNameEdit->setText(bestWingData.name);
    for(int i = 0;i<bestWingData.chordLengthW.length();i++){
        profileSpanEdit[i]->blockSignals(true);
        profilechordLengthEdit[i]->blockSignals(true);
        profileOffsetLengthEdit[i]->blockSignals(true);
        profileTwistAngleEdit[i]->blockSignals(true);
        profileDihedralAngleEdit[i]->blockSignals(true);
        profileSpanEdit[i]->setText(QString::number(bestWingData.spanW[i] * 1000,'f',2));
        profilechordLengthEdit[i]->setText(QString::number(bestWingData.chordLengthW[i] * 1000));
        profileOffsetLengthEdit[i]->setText(QString::number(bestWingData.offsetLengthW[i] * 1000));
        profileTwistAngleEdit[i]->setText(QString::number(bestWingData.twistAngleW[i]));
        profileDihedralAngleEdit[i]->setText(QString::number(bestWingData.dihedralAngleW[i]));
        profileSpanEdit[i]->blockSignals(false);
        profilechordLengthEdit[i]->blockSignals(false);
        profileOffsetLengthEdit[i]->blockSignals(false);
        profileTwistAngleEdit[i]->blockSignals(false);
        profileDihedralAngleEdit[i]->blockSignals(false);
    }
    bestWingData.computeWingMessage();

    areaTextLabel->setText(QString::number(bestWingData.Area(),'f', 3) + "㎡");
    realChordTextLabel->setText(QString::number(bestWingData.RealChord(),'f', 3) + "m");
    realSpanTextLabel->setText(QString::number(bestWingData.Span(),'f', 3) + "m");
    tipRatioTextLabel->setText(QString::number(bestWingData.TipRatio(),'f', 3));
    aspectRatioTextLabel->setText(QString::number(bestWingData.AspectRatio(),'f', 2));
    meshNumTextLabel->setText(QString::number(wingDataArray[wingChoiceIndex].MeshNum()));



    QMessageBox::information(this, "信息", "优化完成");

}
void wingDisplay::initialSteup(){

    /******定义一个平板机翼******/
    simpleCST<<0.00<<0.00<<0.00<<0.00<<0.00<<0.00;
    /*************定义一个CST本地参数************/
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
void wingDisplay::initialWingIterWidget() {

    iterVLayout = new QVBoxLayout();
    iterChartA = new PlotWidget();
    iterChartB = new PlotWidget();
    iterViewA = iterChartA;
    iterViewB = iterChartB;
    iterXAxisA = new PlotAxis;
    iterYAxisA = new PlotAxis;
    iterXAxisB = new PlotAxis;
    iterYAxisB = new PlotAxis;
    iterSeriesA = new PlotSeries;
    iterSeriesB = new PlotSeries;
    iterSeriesC = new PlotSeries;

    iterViewA->textItem->setFont(QFont("Arial",18));
    updateOptimizationTextItem(iterViewA->getRealSize().width() - 100,50);


    iterViewA->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    iterViewB->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    iterViewA->setMinimumWidth(600);
    iterViewB->setMinimumWidth(600);

    iterXAxisA->setGridLineVisible(false);
    iterYAxisA->setGridLineVisible(false);

    iterChartA->addSeries(iterSeriesA);
    iterChartB->addSeries(iterSeriesB);
    iterChartB->addSeries(iterSeriesC);

    iterChartA->addAxis(iterXAxisA, Qt::AlignBottom);
    iterChartA->addAxis(iterYAxisA, Qt::AlignLeft);
    iterChartB->addAxis(iterXAxisB, Qt::AlignBottom);
    iterChartB->addAxis(iterYAxisB, Qt::AlignLeft);

    iterSeriesA->attachAxis(iterXAxisA);
    iterSeriesA->attachAxis(iterYAxisA);

    iterSeriesB->attachAxis(iterXAxisB);
    iterSeriesB->attachAxis(iterYAxisB);

    iterSeriesC->attachAxis(iterXAxisB);
    iterSeriesC->attachAxis(iterYAxisB);

    iterViewA->hide();
    iterViewB->hide();

    iterXAxisA->setTitleText("迭代次数");
    iterYAxisA->setTitleText("目标值");
    iterXAxisB->setTitleText("翼展");
    iterYAxisB->setTitleText("环量");

    QPen pen1;
    pen1.setWidth(2);
    pen1.setColor(Qt::black);
    iterSeriesA->setPen(pen1);
    iterSeriesB->setPen(pen1);

    QPen pen2;
    pen2.setWidth(2);
    pen2.setColor(Qt::red);
    pen2.setStyle(Qt::DashLine);
    iterSeriesC->setPen(pen2);

    iterVLayout->addWidget(iterViewA, 1); // 设置伸缩因子为1
    iterVLayout->addWidget(iterViewB, 1); // 设置伸缩因子为1

    iterChartA->legend()->setVisible(false);
    iterChartB->legend()->setVisible(false);

}

void wingDisplay::initialWingDesignWidget()
{
    // 初始化图表


    initialChartWA();
    initialChartWB();
    initialChartWC();
    initialChartWD();
    initialSpanChart();


    // 创建子控件
    wingDisplayWidgetB = new QVTKOpenGLNativeWidget(this);
    chartWWidget = new QWidget(this);
    wingTreeWidget = new QTreeWidget(this);
    tailTreeWidget = new QTreeWidget(this);
    valueSettingOptBox = new QGroupBox("优化参数设置");
    geometrySettingOptBox = new QGroupBox("优化变量设置");
    analyseBox = new QGroupBox("优化参数设置");

    // 设置布局管理器
    //designMainVLayout = new QVBoxLayout(wingDesignWidget);
    listHboxLayout = new QHBoxLayout(this);
    listVboxLayout = new QVBoxLayout();




    // 设置树形控件
    wingTreeWidget->setColumnCount(1); // 设置列
    wingTreeWidget->setHeaderLabel(tr("机翼")); // 设置标题
    wingTreeWidget->setMaximumWidth(300);
    // 设置树形控件
    tailTreeWidget->setColumnCount(1); // 设置列
    tailTreeWidget->setHeaderLabel(tr("尾翼")); // 设置标题
    tailTreeWidget->setMaximumWidth(300);

    // 初始化布局
    wingSettingDialog = new QDialog();
    wingSettingLayout = new QGridLayout(wingSettingDialog);
    wingWidgetVLayout = new QVBoxLayout();
    viewChangeHLayout = new QHBoxLayout();
    wingChartVLayout = new QVBoxLayout(chartWWidget);

    curveIconA1 = ":/images/window/airplane/wing/3D.png";
    curveIconA2 = ":/images/window/airplane/wing/curve1.png";
    curveIconA3 = ":/images/window/airplane/wing/curve2.png";

    curveIconB1 = ":/images/window/airplane/wing/3DB.png";
    curveIconB2 = ":/images/window/airplane/wing/curve3.png";
    curveIconB3 = ":/images/window/airplane/wing/curve4.png";


    // 设置下拉框
    wingAlphaCombox = new QComboBox();
    wingAlphaCombox->setFixedSize(60,40);
    // 创建按钮
    changeModelViewButton = new hoverButton();
    changeResultViewButton = new hoverButton();
    changeSpanResultViewButton = new hoverButton();




    changeModelViewButton->setButtonIcon(curveIconB1,QSize(40,40));
    changeResultViewButton->setButtonIcon(curveIconA2,QSize(40,40));
    changeSpanResultViewButton->setButtonIcon(curveIconA3,QSize(40,40));

   // changeSpanResultViewButton->setText("展向结果");


    changeModelViewButton->setEnabled(false);

    // 创建标签
    wingMessageTextLabel = new QLabel();
    wingMessageTextLabel->setFixedHeight(30);

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
    wingChartVLayout->addWidget(vSplitter);

    // 添加控件到水平布局中
    viewChangeHLayout->addWidget(changeModelViewButton);
    viewChangeHLayout->addWidget(changeResultViewButton);
    viewChangeHLayout->addWidget(changeSpanResultViewButton);
    viewChangeHLayout->addWidget(wingAlphaCombox);
    viewChangeHLayout->addWidget(wingMessageTextLabel);

    // 添加控件到垂直布局中
    wingWidgetVLayout->addLayout(viewChangeHLayout);
    wingWidgetVLayout->addWidget(wingDisplayWidgetB);
    wingWidgetVLayout->addWidget(chartWWidget);
    wingWidgetVLayout->addWidget(spanChartView);

    // 添加树形控件到垂直布局中
    listVboxLayout->addWidget(wingTreeWidget);
    listVboxLayout->addWidget(tailTreeWidget);
    listVboxLayout->addWidget(geometrySettingOptBox);
    listVboxLayout->addWidget(valueSettingOptBox);
    listVboxLayout->addWidget(analyseBox);
    listVboxLayout->addLayout(iterVLayout);

    // 将所有布局添加到主水平布局中


    listHboxLayout->addLayout(listVboxLayout);
    listHboxLayout->addLayout(wingWidgetVLayout);
    //listHboxLayout->addLayout(iterVLayout);



    //wingDesignWidget->setLayout(listHboxLayout);
    setLayout(listHboxLayout);
    // 设置布局伸缩因子
    listHboxLayout->setStretch(0, 1);
    listHboxLayout->setStretch(1, 3);


    // 添加主布局到窗口
    // designMainVLayout->addLayout(listHboxLayout);
    // designMainVLayout->setStretch(0, 1);

    // 连接信号和槽
    connect(wingTreeWidget, &QTreeWidget::itemClicked, this, &wingDisplay::changeWingDisplay);
    connect(wingTreeWidget, &QTreeWidget::itemClicked, this, &wingDisplay::changeWingData);
    connect(wingTreeWidget, &QTreeWidget::itemClicked, this, &wingDisplay::showWingSettingVinfDialog);
    connect(wingTreeWidget, &QTreeWidget::itemChanged, this, &wingDisplay::showWingZeroLiftDragDialog);
    connect(wingTreeWidget, &QTreeWidget::itemChanged, this, &wingDisplay::setWingXfoilDrag);
    connect(wingTreeWidget, &QTreeWidget::itemClicked, this, &wingDisplay::showAirfoilDesignDialog);
    connect(wingTreeWidget, &QTreeWidget::itemChanged, this, &wingDisplay::showWingStreamLineView);
    connect(wingTreeWidget, &QTreeWidget::itemClicked, this, &wingDisplay::closeOtherRoots);
    connect(wingTreeWidget, &QTreeWidget::itemChanged, this, &wingDisplay::showWingPrssureContourView);
    connect(wingTreeWidget, &QTreeWidget::itemClicked, this, &wingDisplay::showSeriesTypeDialog);

    connect(tailTreeWidget, &QTreeWidget::itemClicked, this, &wingDisplay::changeTailData);


    connect(tailTreeWidget, &QTreeWidget::itemClicked, this, &wingDisplay::changeTailDisplay);


    initialWingSettingDialog();
    initialWingAddZeroLiftDragDialog();

    connect(saveWingButton, &QPushButton::clicked, this, &wingDisplay::saveData);
    connect(outputWingButton, &QPushButton::clicked, this, &wingDisplay::outputWingData);
    connect(changeModelViewButton, &QPushButton::clicked, this, &wingDisplay::changeUIForWingDesign);
    connect(changeResultViewButton, &QPushButton::clicked, this, &wingDisplay::changeUIForWingResult);
    connect(changeSpanResultViewButton, &QPushButton::clicked, this, &wingDisplay::changeUIForWingSpanResult);
    connect(wingAlphaCombox, QOverload<int>::of(&QComboBox::activated),
            this, &wingDisplay::changeAlphaView);



}

void wingDisplay::initialWingOptimizationWidget(){

    geometrySettingOptBox->setMinimumHeight(400);
    valueSettingOptGLayout = new QGridLayout(valueSettingOptBox);
    QVBoxLayout *geometrySettingOptVLayout = new QVBoxLayout(geometrySettingOptBox);
    QHBoxLayout *spinHLayout = new QHBoxLayout();
    geometrySettingOptGLayout = new QGridLayout();
    valueSettingOptBox->setFixedWidth(300);
    geometrySettingOptBox->setFixedWidth(300);
    warningEditStyle = "QLineEdit { background-color: rgb(255, 255, 0); }";
    nothingEditStyle = "";
    comboxMaxVal[0][0] = 2000;
    comboxMaxVal[0][1] = 200;
    comboxMaxVal[0][2] = 20;
    comboxMaxVal[1][0] = 500;
    comboxMaxVal[1][1] = 50;
    comboxMaxVal[1][2] = 5;
    comboxMaxVal[2][0] = 1000;
    comboxMaxVal[2][1] = 100;
    comboxMaxVal[2][2] = 10;
    comboxMaxVal[3][0] = 4000;
    comboxMaxVal[3][1] = 400;
    comboxMaxVal[3][2] = 40;
    comboxMaxVal[4][0] = 4000;
    comboxMaxVal[4][1] = 400;
    comboxMaxVal[4][2] = 40;

    minVal[0] = 0;
    minVal[1] = 0;
    minVal[2] = -5;
    minVal[3] = -20;
    minVal[4] = -20;
    unitVal[0] = "m";
    unitVal[1] = "m";
    unitVal[2] = "m";
    unitVal[3] = "°";
    unitVal[4] = "°";

    for(int i = 0;i<8;i++){
        QLabel *label = new QLabel();
        QLineEdit *edit = new QLineEdit();
        edit->setProperty("edit",i);
        optSettingLabel[i] = label;
        optSettingEdit[i] = edit;
    }
    for(int i = 0;i<4;i++){
        valueSettingOptGLayout->addWidget(optSettingLabel[2 * i],i,0,1,1);
        valueSettingOptGLayout->addWidget(optSettingEdit[2 * i],i,1,1,1);
        valueSettingOptGLayout->addWidget(optSettingLabel[2 * i + 1],i,2,1,1);
        valueSettingOptGLayout->addWidget(optSettingEdit[2 * i + 1],i,3,1,1);
    }


    for(int i = 0;i<5;i++){
        QLabel *labelA = new QLabel();
        QLabel *labelB = new QLabel();
        QLabel *labelC = new QLabel();
        QLabel *labelD = new QLabel();
        QComboBox *combox = new QComboBox();
        SwitchButton *button = new SwitchButton();
        RangeSlider *slider = new RangeSlider(Qt::Horizontal, RangeSlider::Option::DoubleHandles, nullptr);
        labelB->setText("精度");
        combox->addItem("0.01");
        combox->addItem("0.1");
        combox->addItem("1");
        button->setProperty("button",i);
        combox->setProperty("combox",i);
        slider->setProperty("slider",i);

        geometrySettingOptGLayout->addWidget(labelA,2 * i,0,1,1);
        geometrySettingOptGLayout->addWidget(button,2 * i,1,1,1);
        geometrySettingOptGLayout->addWidget(labelB,2 * i,2,1,1);
        geometrySettingOptGLayout->addWidget(combox,2 * i,3,1,1);
        geometrySettingOptGLayout->addWidget(labelC,2 * i + 1,0,1,1);
        geometrySettingOptGLayout->addWidget(slider,2 * i + 1,1,1,2);
        geometrySettingOptGLayout->addWidget(labelD,2 * i + 1,3,1,1);
        optLabel[i] = labelA;
        optValLabel[i] = labelB;
        optCombox[i] = combox;
        optSwithButton[i] = button;
        optSlider[i] = slider;
        optMinValLabel[i] = labelC;
        optMaxValLabel[i] = labelD;
        connect(slider,&RangeSlider::lowerValueChanged,this,&wingDisplay::optMinLabelChanged);
        connect(slider,&RangeSlider::upperValueChanged,this,&wingDisplay::optMaxLabelChanged);
        connect(combox, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &wingDisplay::optChangeVal);
        connect(button,&SwitchButton::checkedChanged,this,&wingDisplay::setIsChange);
    }

    analyseBox->setMaximumWidth(300);
    analyseBox->setMinimumHeight(250);

    analyseLayout = new QGridLayout(analyseBox);
    wingListCombox = new QComboBox();
    wingVelocitySetLabel = new QLabel();
    wingHeightSetLabel = new QLabel();
    wingLiftSetLabel = new QLabel();
    wingVelocitySetEdit = new QLineEdit();
    wingHeightSetEdit = new QLineEdit();
    wingLiftSetEdit = new QLineEdit();

    wingSolverCombox = new QComboBox();
    wingListLabel = new QLabel();
    wingSolverLabel = new QLabel();



    numsSpinBox = new QSpinBox();
    QLabel *textLabel = new QLabel();
    QLabel *spaceLabel = new QLabel();
    textLabel->setText("截面");
    textLabel->setMaximumHeight(20);
    spaceLabel->setMaximumHeight(20);

    numsSpinBox->setObjectName("choiceSpinBox");
    numsSpinBox->setMinimum(1);
    numsSpinBox->setFixedWidth(80);
    spinHLayout->addSpacerItem(new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum));
    spinHLayout->addWidget(textLabel);
    spinHLayout->addWidget(numsSpinBox);
    spinHLayout->addSpacerItem(new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum));
    geometrySettingOptVLayout->addWidget(spaceLabel);
    geometrySettingOptVLayout->addLayout(spinHLayout);
    geometrySettingOptVLayout->addLayout(geometrySettingOptGLayout);


    analyseLayout->addWidget(wingListLabel,0,0,1,1);
    analyseLayout->addWidget(wingListCombox,0,1,1,1);
    analyseLayout->addWidget(wingVelocitySetLabel,1,0,1,1);
    analyseLayout->addWidget(wingVelocitySetEdit,1,1,1,1);
    analyseLayout->addWidget(wingHeightSetLabel,2,0,1,1);
    analyseLayout->addWidget(wingHeightSetEdit,2,1,1,1);
    analyseLayout->addWidget(wingLiftSetLabel,3,0,1,1);
    analyseLayout->addWidget(wingLiftSetEdit,3,1,1,1);
    analyseLayout->addWidget(wingSolverLabel,4,0,1,1);
    analyseLayout->addWidget(wingSolverCombox,4,1,1,1);

    wingVelocitySetLabel->setText("设计速度(m/s)");
    wingHeightSetLabel->setText("设计高度(m)");
    wingLiftSetLabel->setText("设计升力(N)");

    wingVelocitySetEdit->setText("20");
    wingHeightSetEdit->setText("1000");
    wingLiftSetEdit->setText("60");

   // analyseLayout->addWidget(startSolveButton,2,0,1,2);
    wingListLabel->setText("机翼列表");

    wingSolverLabel->setText("求解器");
    wingSolverCombox->addItem("VLM");
    //startSolveButton->setText("开始");

    optSettingLabel[0]->setText("初始种群");optSettingEdit[0]->setText("40");
    optSettingLabel[1]->setText("迭代步数");optSettingEdit[1]->setText("20");
    optSettingLabel[2]->setText("精英种群");optSettingEdit[2]->setText("20");
    optSettingLabel[3]->setText("轮盘赌指数");optSettingEdit[3]->setText("1.2");
    optSettingLabel[4]->setText("交叉概率");optSettingEdit[4]->setText("0.9");
    optSettingLabel[5]->setText("变异概率");optSettingEdit[5]->setText("0.05");
    optSettingLabel[6]->setText("翼型变化率");optSettingEdit[6]->setText("0.5");
    optSettingLabel[7]->setText("翼型精度");optSettingEdit[7]->setText("1e-6");

    for(int i = 0;i<8;i++)
        connect(optSettingEdit[i],&QLineEdit::textChanged,this,&wingDisplay::updateOptSetting);

    optMinVal[0] = 4;optMaxVal[0] = 500000;
    optMinVal[1] = 1;optMaxVal[1] = 500000;
    optMinVal[2] = 2;optMaxVal[2] = 250000;
    optMinVal[3] = 0.999;optMaxVal[3] = 2;
    optMinVal[4] = 0.01;optMaxVal[4] = 1;
    optMinVal[5] = 0.2;optMaxVal[5] = 1;
    optMinVal[6] = 0.05;optMaxVal[6] = 1;
    optMinVal[7] = 1e-3;optMaxVal[7] = 1e-8;
    optSlider[0]->SetRange(0,comboxMaxVal[0][0]);
    optSlider[1]->SetRange(0,comboxMaxVal[1][0]);
    optSlider[2]->SetRange(0,comboxMaxVal[2][0]);
    optSlider[3]->SetRange(0,comboxMaxVal[3][0]);
    optSlider[4]->SetRange(0,comboxMaxVal[4][0]);
    optLabel[0]->setText("翼展范围");
    optLabel[1]->setText("弦长范围");
    optLabel[2]->setText("偏移范围");
    optLabel[3]->setText("扭转角范围");
    optLabel[4]->setText("上反角范围");



    listVboxLayout->addWidget(valueSettingOptBox);
    listVboxLayout->addWidget(geometrySettingOptBox);
    listVboxLayout->addWidget(analyseBox);
    connect(wingListCombox, QOverload<int>::of(&QComboBox::activated), this, &wingDisplay::optRangeChange);
    connect(numsSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), this, &wingDisplay::updateGeometryOptSetting);

}
void wingDisplay::optRangeChange(const int index){
    if(!optSettingArray.isEmpty()){
        wingChoiceIndex = index;
        updateWingDefineWidget(wingDataArray[index]);
        numsSpinBox->setMaximum(optSettingArray[index].maxChord.length());
        int ind = numsSpinBox->value() - 1;//剖面
        comboxMaxVal[0][0] = (optSettingArray[index].MaxSpan - optSettingArray[index].MinSpan) * 100;//0.01
        comboxMaxVal[0][1] = (optSettingArray[index].MaxSpan - optSettingArray[index].MinSpan) * 10;//0.1
        comboxMaxVal[0][2] = optSettingArray[index].MaxSpan - optSettingArray[index].MinSpan;//1
        comboxMaxVal[1][0] = (optSettingArray[index].maxChord[ind] - optSettingArray[index].minChord[ind]) * 100;
        comboxMaxVal[1][1] = (optSettingArray[index].maxChord[ind] - optSettingArray[index].minChord[ind]) * 10;
        comboxMaxVal[1][2] = optSettingArray[index].maxChord[ind] - optSettingArray[index].minChord[ind];
        comboxMaxVal[2][0] = (optSettingArray[index].maxOffsetLength[ind] - optSettingArray[index].minOffsetLength[ind]) * 100;
        comboxMaxVal[2][1] = (optSettingArray[index].maxOffsetLength[ind] - optSettingArray[index].minOffsetLength[ind]) * 10;
        comboxMaxVal[2][2] = optSettingArray[index].maxOffsetLength[ind] - optSettingArray[index].minOffsetLength[ind];
        comboxMaxVal[3][0] = (optSettingArray[index].maxTwisAngle[ind] - optSettingArray[index].minTwisAngle[ind]) * 100;
        comboxMaxVal[3][1] = (optSettingArray[index].maxTwisAngle[ind] - optSettingArray[index].minTwisAngle[ind]) * 10;
        comboxMaxVal[3][2] = optSettingArray[index].maxTwisAngle[ind] - optSettingArray[index].minTwisAngle[ind];
        comboxMaxVal[4][0] = (optSettingArray[index].maxDihedralAngle[ind] - optSettingArray[index].minDihedralAngle[ind]) * 100;
        comboxMaxVal[4][1] = (optSettingArray[index].maxDihedralAngle[ind] - optSettingArray[index].minDihedralAngle[ind]) * 10;
        comboxMaxVal[4][2] = optSettingArray[index].maxDihedralAngle[ind] - optSettingArray[index].minDihedralAngle[ind];
        minVal[0] = optSettingArray[index].MinSpan;
        minVal[1] = optSettingArray[index].minChord[ind];
        minVal[2] = optSettingArray[index].minOffsetLength[ind];
        minVal[3] = optSettingArray[index].minTwisAngle[ind];
        minVal[4] = optSettingArray[index].minDihedralAngle[ind];
        //qDebug()<<minVal[4]<<" "<<comboxMaxVal[4][0]<<" "<<ind<<" "<<optSettingArray[index].minDihedralAngle[ind];

        double valueArray[5];
        valueArray[0] = optSettingArray[index].spanVal;
        valueArray[1] = optSettingArray[index].chordVal;
        valueArray[2] = optSettingArray[index].offsetLengthVal;
        valueArray[3] = optSettingArray[index].twisAngleVal;
        valueArray[4] = optSettingArray[index].dihedralAngleVal;


        isChangeRealVal = false;
        for(int i = 0;i<5;i++){
            optSlider[i]->SetRange(0,comboxMaxVal[i][getValueIndex(valueArray[i])]);
        }
        isChangeRealVal = true;

        updateGeometryOptSetting();
    }
}
void wingDisplay::initialStreamDialog(){
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

    connect(saveButton,&QPushButton::clicked,this,&wingDisplay::showWingStreamLine);
    connect(cancelButton,&QPushButton::clicked,streamSettingDialog,&QDialog::hide);

}
void wingDisplay::initialTailDefineDialog(){
    tailDefineDialog = new QDialog();
    tailDefineBox = new QGroupBox();
    tailDefineVLayout = new QVBoxLayout();
    tailDefineGLayout = new QGridLayout();
    QVBoxLayout *vLayout = new QVBoxLayout(tailDefineBox);
    QHBoxLayout *hlayout = new QHBoxLayout();




    hTailLabel = new QLabel("水平尾翼",tailDefineDialog);
    vTailLabel = new QLabel("垂直尾翼",tailDefineDialog);
    tTailLabel = new QLabel("模板尾翼",tailDefineDialog);
    hTailButton = new QPushButton("定义",tailDefineDialog);
    vTailButton = new QPushButton("定义",tailDefineDialog);
    tTailButton = new QPushButton("定义",tailDefineDialog);
    hTailButton->setProperty("ID",0);
    vTailButton->setProperty("ID",1);
    tTailButton->setProperty("ID",2);


    QPushButton *button1 = new QPushButton("确认",tailDefineDialog);
    QPushButton *button2 = new QPushButton("取消",tailDefineDialog);





    tailDefineGLayout->addWidget(hTailLabel,0,0,1,1);
    tailDefineGLayout->addWidget(hTailButton,0,1,1,1);


    tailDefineGLayout->addWidget(vTailLabel,1,0,1,1);
    tailDefineGLayout->addWidget(vTailButton,1,1,1,1);


    tailDefineGLayout->addWidget(tTailLabel,2,0,1,1);
    tailDefineGLayout->addWidget(tTailButton,2,1,1,1);


    hlayout->addWidget(button1);
    hlayout->addWidget(button2);

    vLayout->addLayout(tailDefineGLayout);
    vLayout->addLayout(hlayout);
    tailDefineVLayout->addWidget(tailDefineBox);

    tailDefineDialog->setLayout(tailDefineVLayout);

    connect(hTailButton,&QPushButton::clicked,this,&wingDisplay::showTailDefineWidget);
    connect(vTailButton,&QPushButton::clicked,this,&wingDisplay::showTailDefineWidget);
    connect(tTailButton,&QPushButton::clicked,this,&wingDisplay::showTailDefineWidget);



}
void wingDisplay::initialTailTemplateDialog(){

}
void wingDisplay::initialWingSettingDialog(){
    wingVelocityLabel = new QLabel(wingSettingDialog);
    wingVelocityEdit = new QLineEdit(wingSettingDialog);
    wingVelocityUnitLabel = new QLabel(wingSettingDialog);
    wingHeightLabel = new QLabel(wingSettingDialog);
    wingHeightEdit = new QLineEdit(wingSettingDialog);
    wingHeightUnitLabel = new QLabel(wingSettingDialog);
    wingCgLocationLabel = new QLabel(wingSettingDialog);



    wingMinAlphaLabel = new QLabel(wingSettingDialog);
    wingMinAlphaEdit = new QLineEdit(wingSettingDialog);
    wingMaxAlphaLabel = new QLabel(wingSettingDialog);
    wingMaxAlphaEdit = new QLineEdit(wingSettingDialog);
    wingStepAlphaLabel = new QLabel(wingSettingDialog);
    wingStepAlphaEdit = new QLineEdit(wingSettingDialog);
    wingCgLocationEdit = new QLineEdit(wingSettingDialog);
    wingCgLocationCheckBox = new QCheckBox(wingSettingDialog);



    saveWingSettingButton = new QPushButton(wingSettingDialog);
    cancelWingSettingButton = new QPushButton(wingSettingDialog);



    wingVelocityLabel->setText("速度");
    wingVelocityEdit->setText("20");
    wingVelocityUnitLabel->setText("m/s");

    wingHeightLabel->setText("高度");
    wingHeightEdit->setText("0");
    wingHeightUnitLabel->setText("m");

    wingMinAlphaLabel->setText("最小迎角");
    wingMinAlphaEdit->setText("0");

    wingMaxAlphaLabel->setText("最大迎角");
    wingMaxAlphaEdit->setText("10");

    wingStepAlphaLabel->setText("迎角步长");
    wingStepAlphaEdit->setText("1");

    wingCgLocationLabel->setText("力矩参考点");
    wingCgLocationEdit->setText("0");
    wingCgLocationCheckBox->setChecked(true);


    saveWingSettingButton->setText("保存");
    cancelWingSettingButton->setText("取消");



    wingSettingLayout->addWidget(wingVelocityLabel,0,0,1,1);
    wingSettingLayout->addWidget(wingVelocityEdit,0,1,1,1);
    wingSettingLayout->addWidget(wingVelocityUnitLabel,0,2,1,1);

    wingSettingLayout->addWidget(wingHeightLabel,1,0,1,1);
    wingSettingLayout->addWidget(wingHeightEdit,1,1,1,1);
    wingSettingLayout->addWidget(wingHeightUnitLabel,1,2,1,1);



    wingSettingLayout->addWidget(wingMinAlphaLabel,2,0,1,1);
    wingSettingLayout->addWidget(wingMinAlphaEdit,2,1,1,1);
    wingSettingLayout->addWidget(wingMaxAlphaLabel,3,0,1,1);
    wingSettingLayout->addWidget(wingMaxAlphaEdit,3,1,1,1);
    wingSettingLayout->addWidget(wingStepAlphaLabel,4,0,1,1);
    wingSettingLayout->addWidget(wingStepAlphaEdit,4,1,1,1);
    wingSettingLayout->addWidget(wingCgLocationLabel,5,0,1,1);
    wingSettingLayout->addWidget(wingCgLocationEdit,5,1,1,1);
    wingSettingLayout->addWidget(wingCgLocationCheckBox,5,2,1,1);

    wingSettingLayout->addWidget(saveWingSettingButton,6,0,1,1);
    wingSettingLayout->addWidget(cancelWingSettingButton,6,2,1,1);


    connect(saveWingSettingButton,&QPushButton::clicked,this,&wingDisplay::getWingSetting);
    connect(wingCgLocationCheckBox,&QCheckBox::stateChanged,this,&wingDisplay::onCheckboxStateChanged);
    connect(wingCgLocationEdit,&QLineEdit::textChanged,this,&wingDisplay::changeCgLocation);
}
void wingDisplay::initialWingAddZeroLiftDragDialog(){
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

            button->setIconSize(size);
            button->setFixedSize(size);
            button->setProperty("button",G);
            dragButtonArray.append(button);
            connect(button,&QPushButton::clicked,this,&wingDisplay::changeZeroLiftDragText);
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
    connect(saveDragButton,&QPushButton::clicked,this,&wingDisplay::setZeroLiftDrag);
    connect(saveDragButton,&QPushButton::clicked,zeroLiftDragChoiceDialog,&QDialog::hide);
}
void wingDisplay::initialChoiceDragDialog(){
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
    connect(button1,&QPushButton::clicked,this,&wingDisplay::saveDragDialog);
    connect(button2,&QPushButton::clicked,this,&wingDisplay::cancelDragDialog);

}

void wingDisplay::initialChartWA(){
    chartWA = new PlotWidget();
    chartViewWA = chartWA;
    chartViewWA->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    chartWA->setBackgroundBrush(Qt::NoBrush);


    axisXWA = new PlotAxis;
    axisYWA = new PlotAxis;




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
    autoSeriesA = new PlotSeries;
    chartWA->addSeries(autoSeriesA);
    autoSeriesA->attachAxis(axisXWA);
    autoSeriesA->attachAxis(axisYWA);
    removeSeriesLegendItem(chartWA,autoSeriesA);
    QPen pen;
    pen.setStyle(Qt::DashLine);
    pen.setColor(Qt::red);
    pen.setWidth(2);
    autoSeriesA->setPen(pen);
    chartWA->legend()->setVisible(true);
}
void wingDisplay::initialChartWB(){
    chartWB = new PlotWidget();
    chartViewWB = chartWB;
    chartViewWB->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    chartWB->setBackgroundBrush(Qt::NoBrush);


    axisXWB = new PlotAxis;
    axisYWB = new PlotAxis;
    chartViewWB->setRenderHint(QPainter::Antialiasing);
    //chartViewB->setBackgroundBrush(QBrush(colors[5]));
    //chartViewWB->setStyleSheet(chartViewBackGroundStyle);
    axisXWB->setLinePenColor(Qt::black);
    axisYWB->setLinePenColor(Qt::black);
    //axisXWB->setTickCount(int(MINAXIS[7]));
    //axisYWB->setTickCount(int(MAXAXIS[7]));
    axisXWB->setRange(0,10);
    axisYWB->setRange(-0.5,0.1);
    chartWB->addAxis(axisXWB,Qt::AlignBottom);
    chartWB->addAxis(axisYWB,Qt::AlignLeft);
    axisXWB->setTitleText(axisXName[0]);
    axisYWB->setTitleText(axisYName[1]);
    chartViewWB->hide();
    chartWB->setTitle(titleName[1]);

    autoSeriesB = new PlotSeries;
    chartWB->addSeries(autoSeriesB);
    autoSeriesB->attachAxis(axisXWB);
    autoSeriesB->attachAxis(axisYWB);
    removeSeriesLegendItem(chartWB,autoSeriesB);
    QPen pen;
    pen.setStyle(Qt::DashLine);
    pen.setColor(Qt::red);
    pen.setWidth(2);
    autoSeriesB->setPen(pen);
    chartWB->legend()->setVisible(true);
}
void wingDisplay::initialChartWC(){
    chartWC = new PlotWidget();
    chartViewWC = chartWC;
    chartViewWC->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    chartWC->setBackgroundBrush(Qt::NoBrush);


    axisXWC = new PlotAxis;
    axisYWC = new PlotAxis;
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

    autoSeriesC = new PlotSeries;
    chartWC->addSeries(autoSeriesC);
    autoSeriesC->attachAxis(axisXWC);
    autoSeriesC->attachAxis(axisYWC);
    removeSeriesLegendItem(chartWC,autoSeriesC);
    chartWC->legend()->setVisible(true);
    QPen pen;
    pen.setStyle(Qt::DashLine);
    pen.setColor(Qt::red);
    pen.setWidth(2);
    autoSeriesC->setPen(pen);
}
void wingDisplay::initialChartWD(){
    chartWD = new PlotWidget();
    chartViewWD = chartWD;
    chartViewWD->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    chartWD->setBackgroundBrush(Qt::NoBrush);


    axisXWD = new PlotAxis;
    axisYWD = new PlotAxis;
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

    autoSeriesD = new PlotSeries;
    chartWD->addSeries(autoSeriesD);
    autoSeriesD->attachAxis(axisXWD);
    autoSeriesD->attachAxis(axisYWD);
    removeSeriesLegendItem(chartWD,autoSeriesD);
    chartWD->legend()->setVisible(true);
    QPen pen;
    pen.setStyle(Qt::DashLine);
    pen.setColor(Qt::red);
    pen.setWidth(2);
    autoSeriesD->setPen(pen);
}
void wingDisplay::initialSpanChart(){
    spanChart = new PlotWidget();
    spanChartView = spanChart;
    spanSeriesA = new PlotSeries;
    spanSeriesB = new PlotSeries;
    spanXAxis = new PlotAxis;
    spanYAxis = new PlotAxis;


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
    spanChart->legend()->setVisible(false);

    spanChart->setTitle(titleName[9]);

    QPen pen2;
    pen2.setWidth(2);
    pen2.setColor(Qt::red);
    pen2.setStyle(Qt::DashLine);
    spanSeriesB->setPen(pen2);


    spanChartView->hide();

}
void wingDisplay::initialSeriesStyleDialog(){
    seriesStyleDialog = new QDialog();
    //seriesTypeBox = new QGroupBox(seriesTypeDialog);
    QVBoxLayout *seriesStyleVLayout = new QVBoxLayout();
    QHBoxLayout *seriesStyleHLayout = new QHBoxLayout();
    colorDialog = new QColorDialog();
    colorButton = new QPushButton();
    QPushButton *button2 = new QPushButton();
    QPushButton *button3 = new QPushButton();
    QComboBox *combox = new QComboBox();
    seriesStyleDialog->resize(50,100);
    seriesStyleDialog->setToolTip("曲线类型设置");
    QString lineStyleIcon[8];
    for(int i = 0;i<8;i++)
        lineStyleIcon[i] = QDir::currentPath() + "/resoure/images/lineStyle/style" + QString::number(i) + ".png";
    colorButton->setObjectName("iconButton");


    button2->setText("确认");
    button3->setText("取消");

    combox->setIconSize(QSize(80,25));
    for(int i = 0;i<8;i++){
        combox->addItem(QIcon(lineStyleIcon[i]),"");
    }
    seriesStyleVLayout->addWidget(colorButton);
    seriesStyleVLayout->addWidget(combox);
    seriesStyleHLayout->addWidget(button2);
    seriesStyleHLayout->addWidget(button3);
    seriesStyleVLayout->addLayout(seriesStyleHLayout);
    seriesStyleDialog->setLayout(seriesStyleVLayout);
    connect(colorButton,&QPushButton::clicked,this,&wingDisplay::showColorDialog);
    connect(colorDialog,&QColorDialog::colorSelected,this,&wingDisplay::changeSeriesColor);
    connect(combox, QOverload<int>::of(&QComboBox::activated),
            this, &wingDisplay::changeSeriesStyle);

    connect(button2,&QPushButton::clicked,this,&wingDisplay::hideSeriesStyleDialog);
    connect(button3,&QPushButton::clicked,this,&wingDisplay::hideSeriesStyleDialog);
}
void wingDisplay::initialRChartMenu(){
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


    connect(rAction1,&QAction::triggered,this,&wingDisplay::changeResultType);
    connect(rAction2,&QAction::triggered,this,&wingDisplay::changeResultType);
    connect(rAction3,&QAction::triggered,this,&wingDisplay::changeResultType);
    connect(rAction4,&QAction::triggered,this,&wingDisplay::changeResultType);
    connect(rAction5,&QAction::triggered,this,&wingDisplay::changeResultType);
    connect(rAction6,&QAction::triggered,this,&wingDisplay::changeResultType);
    connect(rAction7,&QAction::triggered,this,&wingDisplay::changeResultType);
    connect(rAction8,&QAction::triggered,this,&wingDisplay::changeResultType);
    connect(rAction9,&QAction::triggered,this,&wingDisplay::changeResultType);
    connect(rAction10,&QAction::triggered,this,&wingDisplay::changeResultType);
    connect(rAction11,&QAction::triggered,this,&wingDisplay::addDataPoint);
    connect(rAction12,&QAction::triggered,this,&wingDisplay::clearDataPoint);

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

void wingDisplay::initialSpanRChartMenu(){
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



    connect(rAction1,&QAction::triggered,this,&wingDisplay::changeSpanResultType);
    connect(rAction2,&QAction::triggered,this,&wingDisplay::changeSpanResultType);
    connect(rAction3,&QAction::triggered,this,&wingDisplay::changeSpanResultType);
    connect(rAction4,&QAction::triggered,this,&wingDisplay::changeSpanResultType);
    connect(rAction5,&QAction::triggered,this,&wingDisplay::changeSpanResultType);
    connect(rAction6,&QAction::triggered,this,&wingDisplay::changeSpanResultType);

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
void wingDisplay::changeSeriesStyle(const int index){
    if(wingDataArray.length() > 0){
        resultPenArray[wingChoiceIndex] = index;
        drawResultChartA(wingChoiceIndex);
        drawResultChartB(wingChoiceIndex);
        drawResultChartC(wingChoiceIndex);
        drawResultChartD(wingChoiceIndex);


        //drawWingResult(wingChoiceIndex);
    }
}
void wingDisplay::changeUIForWingDesign(){
    modelType = WING_DESIGN;


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
    iterViewA->hide();
    iterViewB->hide();
    spanChartView->hide();

    wingDisplayWidgetB->show();
    wingTreeWidget->show();
    tailTreeWidget->hide();

    if(wingDataArray.length() > wingChoiceIndex){

        updateGLTextA(wingDataToString(wingDataArray[wingChoiceIndex]));
    }
    textActorA->Modified();


}
void wingDisplay::changeUIForWingResult(){
    modelType = WING_RESULTA;

    changeModelViewButton->setButtonIcon(curveIconA1,QSize(40,40));
    changeResultViewButton->setButtonIcon(curveIconB2,QSize(40,40));
    changeSpanResultViewButton->setButtonIcon(curveIconA3,QSize(40,40));


    changeModelViewButton->setEnabled(true);
    changeResultViewButton->setEnabled(false);
    changeSpanResultViewButton->setEnabled(true);

    wingDisplayWidgetB->hide();
    spanChartView->hide();
    chartViewWA->show();
    chartViewWB->show();
    chartViewWC->show();
    chartViewWD->show();
    iterViewA->hide();
    iterViewB->hide();

    chartWWidget->show();
    wingTreeWidget->show();
}
void wingDisplay::changeUIForWingSpanResult(){
    modelType = WING_RESULTB;

    changeModelViewButton->setButtonIcon(curveIconA1,QSize(40,40));
    changeResultViewButton->setButtonIcon(curveIconA2,QSize(40,40));
    changeSpanResultViewButton->setButtonIcon(curveIconB3,QSize(40,40));

    changeSpanResultViewButton->setEnabled(false);
    changeModelViewButton->setEnabled(true);
    changeResultViewButton->setEnabled(true);

    chartWWidget->hide();
    wingDisplayWidgetB->hide();
    spanChartView->show();
    chartViewWA->hide();
    chartViewWB->hide();
    chartViewWC->hide();
    chartViewWD->hide();
    iterViewA->hide();
    iterViewB->hide();

    wingTreeWidget->show();

}
void wingDisplay::changeUIForWingIter(){


    rendererB->SetBackground(1, 1, 1);
    textProperty->SetColor(0.0,0.0,0.0);
    textProperty->SetFontSize(12); // 初始字体大小


    geometrySettingOptBox->hide();
    valueSettingOptBox->hide();
    analyseBox->hide();
    hideAllWidgetsInBox(geometrySettingOptBox);
    hideAllWidgetsInBox(valueSettingOptBox);
    hideAllWidgetsInBox(analyseBox);


    iterViewA->show();
    iterViewB->show();



}
void wingDisplay::changeWindowForTailDesign(){


    saveType = WING_HTAIL;
    changeUIForWingDesign();



    changeResultViewButton->setEnabled(false);
    changeSpanResultViewButton->setEnabled(false);
    actorB->VisibilityOff();
    actorC->VisibilityOff();
    actorS->VisibilityOff();
    actorH->VisibilityOn();
    actorV->VisibilityOn();
    rendererB->SetBackground(0.1, 0.2, 0.4); // 深蓝色背景
    textProperty->SetColor(1.0, 1.0, 1.0); // 白色文本
    textProperty->SetFontSize(18); // 初始字体大小
    tailTreeWidget->show();
    wingTreeWidget->hide();

    if(tailDataArray.length() > tailChoiceIndex){

        updateGLTextA(wingDataToString(tailDataArray[tailChoiceIndex]));
    }else{
        textActorA->SetInput("");
    }

    textActorA->Modified();



    geometrySettingOptBox->hide();
    valueSettingOptBox->hide();
    analyseBox->hide();

    wingAlphaCombox->setEnabled(false);


    hideAllWidgetsInBox(geometrySettingOptBox);
    hideAllWidgetsInBox(valueSettingOptBox);
    hideAllWidgetsInBox(analyseBox);

    wingDisplayWidgetB->renderWindow()->Render();




}
void wingDisplay::changeWindowForWingDesign(){

    saveType = WING_WING;
    actorB->VisibilityOn();
    actorC->VisibilityOff();
    actorS->VisibilityOff();
    actorH->VisibilityOff();
    actorV->VisibilityOff();
    rendererB->SetBackground(0.1, 0.2, 0.4); // 深蓝色背景
    textProperty->SetColor(1.0, 1.0, 1.0); // 白色文本
    textProperty->SetFontSize(18); // 初始字体大小

    textActorA->SetInput("");
    textActorA->Modified();

    wingTreeWidget->show();
    tailTreeWidget->hide();
    geometrySettingOptBox->hide();
    valueSettingOptBox->hide();
    analyseBox->hide();

    hideAllWidgetsInBox(geometrySettingOptBox);
    hideAllWidgetsInBox(valueSettingOptBox);
    hideAllWidgetsInBox(analyseBox);


    wingDisplayWidgetB->renderWindow()->Render();

    bool button1 = changeModelViewButton->isEnabled();
    bool button2 = changeResultViewButton->isEnabled();
    bool button3 = changeSpanResultViewButton->isEnabled();
    wingAlphaCombox->setEnabled(true);

    if(!button1){
        changeUIForWingDesign();

        return;
    }
    if(!button2){
        changeUIForWingResult();

        return;
    }
    if(!button3){
        changeUIForWingSpanResult();

        return;
    }




    //chartWWidget->hide();
    //iterViewA->hide();
    //iterViewB->hide();



    //changeModelViewButton->setEnabled(false);
    //changeResultViewButton->setEnabled(true);
    //changeSpanResultViewButton->setEnabled(true);
    //
    /*
    wingVelocitySetEdit->setEnabled(false);
    wingHeightSetEdit->setEnabled(false);
    wingLiftSetEdit->setEnabled(false);
    */





}
void wingDisplay::changeWindowForWingOptimization(){

    actorB->VisibilityOn();
    actorC->VisibilityOff();
    actorS->VisibilityOff();
    actorH->VisibilityOff();
    actorV->VisibilityOff();

    rendererB->SetBackground(0.1, 0.2, 0.4); // 深蓝色背景
    textProperty->SetColor(1.0, 1.0, 1.0); // 白色文本
    textProperty->SetFontSize(18); // 初始字体大小

    wingTreeWidget->hide();
    tailTreeWidget->hide();
    chartWWidget->hide();
    geometrySettingOptBox->show();
    valueSettingOptBox->show();
    analyseBox->show();
    iterViewA->hide();
    iterViewB->hide();

    changeModelViewButton->setEnabled(false);
    changeResultViewButton->setEnabled(false);
    changeSpanResultViewButton->setEnabled(false);
    wingAlphaCombox->setEnabled(false);

    wingVelocitySetEdit->setEnabled(true);
    wingHeightSetEdit->setEnabled(true);
    wingLiftSetEdit->setEnabled(true);
    showAllWidgetsInBox(geometrySettingOptBox);
    showAllWidgetsInBox(valueSettingOptBox);
    showAllWidgetsInBox(analyseBox);


}
void wingDisplay::changeWingDisplay(QTreeWidgetItem* item, int /*column*/){
    if (!item->parent()) { // 判断是否是子节点
        int index = item->data(0, Qt::UserRole).toInt();
        wingChoiceIndex = index;
        hidePressureContourView();//关闭云图
        updateGLTextA(wingDataToString(wingDataArray[index]));
        updateWingDefineWidget(wingDataArray[index]);

        if(spanResultTypeIndex != 0)
            spanSeriesB->setVisible(false);
        spanActionArray[spanResultTypeIndex]->setChecked(true);
        spanXAxis->setTitleText(axisXName[spanResultTypeIndex + 9]);
        spanYAxis->setTitleText(axisYName[spanResultTypeIndex + 9]);
        spanChart->setTitle(titleName[spanResultTypeIndex + 9]);
        drawSpanResult(index);


    }
}
void wingDisplay::changeTailDisplay(QTreeWidgetItem* item, int /*column*/){
    if (!item->parent()) { // 判断是否是子节点
        int index = item->data(0, Qt::UserRole).toInt();
        tailChoiceIndex = index;
        //hidePressureContourView();//关闭云图
        if(tailDataArray[index].isSymmetry){
            saveType = WING_HTAIL;
        }else{
            saveType = WING_VTAIL;
        }



        updateGLTextA(wingDataToString(tailDataArray[index]));//刷新模型参数
        updateWingDefineWidget(tailDataArray[index]);//刷新模型视图



    }
}
void wingDisplay::changeWingData(QTreeWidgetItem* item, int /*column*/){
    if (item->parent()) { // 判断是否是子节点



        QString txt = item->data(0,Qt::UserRole).toString();
        QString txtTmp = txt.mid(0,10);
        QString tmp1 = txt.mid(10);
        QString tmp = "childShape";

        if(tmp == txtTmp &&  tmp1.toInt() == wingChoiceIndex){
            outputWingButton->setEnabled(true);
            isModify = true;

            SpanNumB = wingDataArray[wingChoiceIndex].chordLengthW.length();//更新机翼编辑列表的行数
            updateWingEdit(wingChoiceIndex);
            wingNameEdit->setText(wingDataArray[wingChoiceIndex].name);
            wingDefineWidget->show();
            updateView(wingDataArray[wingChoiceIndex]);

        }
    }
}
void wingDisplay::changeTailData(QTreeWidgetItem* item, int /*column*/){
    if (item->parent()) { // 判断是否是子节点



        QString txt = item->data(0,Qt::UserRole).toString();
        QString txtTmp = txt.mid(0,14);
        QString tmp1 = txt.mid(14);
        QString tmp = "tailChildShape";

        if(tmp == txtTmp &&  tmp1.toInt() == tailChoiceIndex){
            outputWingButton->setEnabled(true);
            isModify = true;

            SpanNumB = tailDataArray[wingChoiceIndex].chordLengthW.length();//更新机翼编辑列表的行数
            updateWingEdit(tailChoiceIndex);
            wingNameEdit->setText(tailDataArray[tailChoiceIndex].name);
            wingDefineWidget->show();
            updateView(tailDataArray[tailChoiceIndex]);

        }
    }
}
void wingDisplay::changeCgLocation(){
    double x = wingCgLocationEdit->text().toDouble();
    VLMSolverArray[wingChoiceIndex]->cgX = x;
    actorCg->SetPosition(x, 0.0, 0.0);
    wingDisplayWidgetB->renderWindow()->Render();
}
void wingDisplay::changeZeroLiftDragText(){
    QPushButton *button = static_cast<QPushButton*>(sender());
    int index = button->property("button").toInt();
    dragEdit->setText(QString::number(zeroLiftDragValueArray[index]));
}

void wingDisplay::showColorDialog(){
    colorDialog->show();
}

void wingDisplay::showWingSettingVinfDialog(QTreeWidgetItem* item, int /*column*/){

    if (item->parent()) { // 判断是否是子节点

        QString txt = item->data(0,Qt::UserRole).toString();
        QString txtTmp = txt.mid(0,9);
        QString tmp1 = txt.mid(9);
        QString tmp = "childVinf";
        if(tmp == txtTmp &&  tmp1.toInt() == wingChoiceIndex){
            wingMinAlphaEdit->setText(QString::number(wingSettingArray[wingChoiceIndex].minAlpha));
            wingMaxAlphaEdit->setText(QString::number(wingSettingArray[wingChoiceIndex].maxAlpha));
            wingStepAlphaEdit->setText(QString::number(wingSettingArray[wingChoiceIndex].stepAlpha));
            wingVelocityEdit->setText(QString::number(wingSettingArray[wingChoiceIndex].vinf));
            wingCgLocationEdit->setText(QString::number(wingSettingArray[wingChoiceIndex].referencePointX));

            wingSettingDialog->show();
            // defineAirfoil->setArifoil(wingDataArray[wingChoiceIndex].airfoilArray[0]);
            // defineAirfoil->modifyAirfoilDialog->show();
        }
    }
}
void wingDisplay::showWingZeroLiftDragDialog(QTreeWidgetItem* item, int column){


    if (item->checkState(column) == Qt::Checked) { // 判断是否是子节点
        QString txt = item->data(0,Qt::UserRole).toString();
        QString txtTmp = txt.mid(0,17);
        QString tmp1 = txt.mid(17);
        QString tmp = "childZeroLiftDrag";

        if(tmp == txtTmp &&  tmp1.toInt() == wingChoiceIndex){

            zeroLiftDragChoiceDialog->show();
            VLMSolverArray[wingChoiceIndex]->setZeroLiftDrag(true);
            //defineWing->updatewingDefineWidget(wingDataArray[wingChoiceIndex]);
        }
    }else{

        QString txt = item->data(0,Qt::UserRole).toString();
        QString txtTmp = txt.mid(0,17);
        QString tmp1 = txt.mid(17);
        QString tmp = "childZeroLiftDrag";
        if(tmp == txtTmp &&  tmp1.toInt() == wingChoiceIndex){
            if(!VLMSolverArray.isEmpty()){
                VLMSolverArray[wingChoiceIndex]->setZeroLiftDrag(false);
            //defineWing->updatewingDefineWidget(wingDataArray[wingChoiceIndex]);
            }
        }
    }
}
void wingDisplay::setZeroLiftDrag(){
    zeroLiftDragChoiceDialog->hide();
    double drag = dragEdit->text().toDouble();
    if(drag <0 || drag >1)
        QMessageBox::information(this,"警告","设置正确的零升阻力系数");
    VLMSolverArray[wingChoiceIndex]->viscousCd = drag;

}
void wingDisplay::setWingXfoilDrag(QTreeWidgetItem* item, int column){

    if (item->checkState(column) == Qt::Checked) { // 判断是否是子节点
        QString txt = item->data(0,Qt::UserRole).toString();
        QString txtTmp = txt.mid(0,14);
        QString tmp1 = txt.mid(14);
        QString tmp = "childXfoilDrag";
        if(tmp == txtTmp &&  tmp1.toInt() == wingChoiceIndex ){

            dragChoiceDialog->show();


            // VLMSolverArray[wingChoiceIndex]->setXfoilDrag(false);
            // qDebug()<<wingChoiceIndex;
        }
    }else{

        QString txt = item->data(0,Qt::UserRole).toString();
        QString txtTmp = txt.mid(0,14);
        QString tmp1 = txt.mid(14);
        QString tmp = "childXfoilDrag";
        if(tmp == txtTmp &&  tmp1.toInt() == wingChoiceIndex){
            if(!VLMSolverArray.isEmpty()){
            VLMSolverArray[wingChoiceIndex]->setXfoilDrag(false);

            }
        }
    }
}
void wingDisplay::hideResultNode(QTreeWidgetItem *root, const QString &textToFind) {
    for (int i = 0; i < root->childCount(); ++i) {
        QTreeWidgetItem *child = root->child(i);
        if (child->text(0) == textToFind) {
            child->setHidden(true); // 显示匹配的节点
            return;
        } else {
            showResultNode(child, textToFind); // 递归遍历子节点
        }
    }
}

void wingDisplay::showTailDefineDialog(){
    tailDefineDialog->show();
}
void wingDisplay::showTailDefineWidget(){
    QPushButton *button = qobject_cast<QPushButton*>(sender());
    int tmp = button->property("ID").toInt();
    switch (tmp) {
    case 0:
        isModify = false;
        wingNameEdit->setText("水平尾翼");
        saveType = WING_HTAIL;
        tailDefineDialog->hide();
        outputWingButton->setEnabled(false);
        updateWingData();
        wingDefineWidget->show();

        break;
    case 1:
        isModify = false;
        wingNameEdit->setText("垂直尾翼");
        saveType = WING_VTAIL;
        tailDefineDialog->hide();
        outputWingButton->setEnabled(false);
        updateWingData();
        wingDefineWidget->show();

        break;
    case 2:
        tailDefineDialog->hide();
        //showTailTemplateDialog();
        break;
    default:
        break;
    }
}

void wingDisplay::showResultNode(QTreeWidgetItem *root, const QString &textToFind) {
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




void wingDisplay::showSeriesTypeDialog(QTreeWidgetItem* item, int /*column*/){
    if (item->parent()) { // 判断是否是子节点

        QString txt = item->data(0,Qt::UserRole).toString();
        QString txtTmp = txt.mid(0,15);
        QString tmp1 = txt.mid(15);
        QString tmp = "childSeriesType";
        if(tmp == txtTmp &&  tmp1.toInt() == wingChoiceIndex){
            QString colorStyle = QString("background-color: %1;").arg(colorArray[wingChoiceIndex].name());
            colorButton->setStyleSheet(colorStyle);
            seriesStyleDialog->show();

            // defineAirfoil->setArifoil(wingDataArray[wingChoiceIndex].airfoilArray[0]);
            // defineAirfoil->modifyAirfoilDialog->show();
        }
    }

}
void wingDisplay::showChartAMenu(){
    chartIndex = 0;
    for (QAction *act : rChartMenu->actions()) {
        act->setChecked(false);
    }
    actionArray[chartTypeIndexArray[0]]->setChecked(true);

    rChartMenu->exec(QCursor::pos());
}
void wingDisplay::showChartBMenu(){
    chartIndex = 1;
    for (QAction *act : rChartMenu->actions()) {
        act->setChecked(false);
    }
    actionArray[chartTypeIndexArray[1]]->setChecked(true);

    rChartMenu->exec(QCursor::pos());

}
void wingDisplay::showChartCMenu(){
    chartIndex = 2;
    for (QAction *act : rChartMenu->actions()) {
        act->setChecked(false);
    }
    actionArray[chartTypeIndexArray[2]]->setChecked(true);

    rChartMenu->exec(QCursor::pos());

}
void wingDisplay::showChartDMenu(){
    chartIndex = 3;
    for (QAction *act : rChartMenu->actions()) {
        act->setChecked(false);
    }
    actionArray[chartTypeIndexArray[3]]->setChecked(true);

    rChartMenu->exec(QCursor::pos());

}
void wingDisplay::showSpanChartMenu(){
    for (QAction *act : spanRMenu->actions()) {
        act->setChecked(false);
    }
    spanActionArray[spanResultTypeIndex]->setChecked(true);
    spanRMenu->exec(QCursor::pos());
}
void wingDisplay::showAirfoilDesignDialog(QTreeWidgetItem* item, int /*column*/){

    if (item->parent()) { // 判断是否是子节点



        QString txt = item->data(0,Qt::UserRole).toString();
        QString txtTmp = txt.mid(0,7);
        QString tmp = txt.mid(7);
        bool ok;
        int index = tmp.toInt(&ok);

        if(txtTmp == "airfoil"){
            QString txt2 = item->parent()->data(0,Qt::UserRole).toString();
            QString tmp2 = txt2.mid(12);
            int ind = tmp2.toInt();

            if(ind == wingChoiceIndex){
                if(!wingDataArray[wingChoiceIndex].airfoilArray.isEmpty()){
                    defineAirfoil->setAirfoil(wingDataArray[wingChoiceIndex].airfoilArray[index],wingDataArray[wingChoiceIndex].cstPointYArray[index]);
                    defineAirfoil->showModifyAirfoilDialogCST();
                    airfoilIndex = index;
                }
            }

        }
    }
}
void wingDisplay::showWingStreamLine(){
    streamSettingDialog->hide();
    int index = wingAlphaCombox->currentIndex();
    double a1 = streamEditArray[0]->text().toDouble();
    double a2 = streamEditArray[1]->text().toDouble();
    double a3 = streamEditArray[2]->text().toDouble();
    double a4 = streamEditArray[3]->text().toDouble();

    VLMSolverArray[wingChoiceIndex]->isShowStreamLine = true;
    if(index >= 0){
        startCalculation();
        VLMSolverArray[wingChoiceIndex]->solveStreamLine(index,a1,a2,a3,a4);
        updateStreamLineView(VLMSolverArray[wingChoiceIndex]);
    }

}
void wingDisplay::showWingStreamLineView(QTreeWidgetItem* item, int column){

    if (item->checkState(column) == Qt::Checked) { // 判断是否是子节点


        QString txt = item->data(0,Qt::UserRole).toString();
        QString txtTmp = txt.mid(0,15);
        QString tmp1 = txt.mid(15);
        QString tmp = "childStreamLine";
        if(tmp == txtTmp &&  tmp1.toInt() == wingChoiceIndex){
            streamSettingDialog->show();
        }
    }else{
        QString txt = item->data(0,Qt::UserRole).toString();
        QString txtTmp = txt.mid(0,15);
        QString tmp1 = txt.mid(15);
        QString tmp = "childStreamLine";
        if(tmp == txtTmp &&  tmp1.toInt() == wingChoiceIndex){

            hideStreamLineView();
        }
    }
}
void wingDisplay::showWingPrssureContourView(QTreeWidgetItem* item, int column){
    if (item->checkState(column) == Qt::Checked) { // 判断是否是子节点


        QString txt = item->data(0,Qt::UserRole).toString();
        QString txtTmp = txt.mid(0,12);
        QString tmp1 = txt.mid(12);
        QString tmp = "childPrssure";
        if(tmp == txtTmp &&  tmp1.toInt() == wingChoiceIndex){
            //zeroLiftDragChoiceDialog->show();
            //defineWing->updatewingDefineWidget(wingDataArray[wingChoiceIndex]);
            //defineWing->updateStreamLineView();
            //qDebug()<<"PrssureContour"<<wingChoiceIndex;



            updatePressureContourView(VLMSolverArray[wingChoiceIndex]);
        }
    }else{

        QString txt = item->data(0,Qt::UserRole).toString();
        QString txtTmp = txt.mid(0,12);
        QString tmp1 = txt.mid(12);
        QString tmp = "childPrssure";
        if(tmp == txtTmp &&  tmp1.toInt() == wingChoiceIndex){
            //zeroLiftDragChoiceDialog->show();
            //defineWing->updatewingDefineWidget(wingDataArray[wingChoiceIndex]);
            //qDebug()<<"NoPrssureContour"<<wingChoiceIndex;

            hidePressureContourView();

        }
    }

}
QPen wingDisplay::getPen(const int index){
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
void wingDisplay::changeAirfoilData(const QVector<QVector<double>>&airfoil,const QVector<double>&pointY){
    wingDataArray[wingChoiceIndex].airfoilArray[airfoilIndex] = airfoil;
    wingDataArray[wingChoiceIndex].cstPointYArray[airfoilIndex] = pointY;
    wingDataArray[wingChoiceIndex].airfoilInputType = true;
}
void wingDisplay::changeSeriesColor(const QColor&color){
    colorArray[wingChoiceIndex] = color;
    QString colorStyle = QString("background-color: %1;").arg(color.name());
    colorButton->setStyleSheet(colorStyle);
    //drawDesignAirfoil(choiceIndex);
    drawResultChartA(wingChoiceIndex);
    drawResultChartB(wingChoiceIndex);
    drawResultChartC(wingChoiceIndex);
    drawResultChartD(wingChoiceIndex);
    //drawWingResult(wingChoiceIndex);
}
void wingDisplay::getWingSetting(){
    wingSettingDialog->hide();
    double mina = wingMinAlphaEdit->text().toDouble();
    double maxa = wingMaxAlphaEdit->text().toDouble();
    double stepa = wingStepAlphaEdit->text().toDouble();
    double v = wingVelocityEdit->text().toDouble();
    double hei = wingHeightEdit->text().toDouble();
    double referenceX = wingCgLocationEdit->text().toDouble();
    VLMSetting setting(mina,maxa,stepa,v,hei,referenceX);
    wingSettingArray[wingChoiceIndex] = setting;

    //VLMSolverArray[wingChoiceIndex]->height = hei;
}
void wingDisplay::drawResultChartA(const int index){


    if(!resultArray[index].isEmpty()){
        if(wingResultSeriesA[index]->count() > 0)
            wingResultSeriesA[index]->clear();
        for(int i = 0;i<resultArray[index][0].length();i++){
            wingResultSeriesA[index]->append(resultArray[index][xAxisIndexA][i],resultArray[index][yAxisIndexA][i]);
        }
        wingResultSeriesA[index]->setPen(getPen(index));
        //updateAxis(axisXWA,axisYWA,xAxisIndexA,yAxisIndexA);

    }
}
void wingDisplay::drawResultChartB(const int index){

    if(!resultArray[index].isEmpty()){
        if(wingResultSeriesB[index]->count() > 0)
            wingResultSeriesB[index]->clear();
        for(int i = 0;i<resultArray[index][0].length();i++){
            wingResultSeriesB[index]->append(resultArray[index][xAxisIndexB][i],resultArray[index][yAxisIndexB][i]);
        }
        wingResultSeriesB[index]->setPen(getPen(index));
        //updateAxis(axisXWB,axisYWB,xAxisIndexB,yAxisIndexB);
        updateAxes(chartWB,wingResultSeriesB);
    }
}
void wingDisplay::drawResultChartC(const int index){

    if(!resultArray[index].isEmpty()){
        if(wingResultSeriesC[index]->count() > 0)
            wingResultSeriesC[index]->clear();
        for(int i = 0;i<resultArray[index][0].length();i++){
            wingResultSeriesC[index]->append(resultArray[index][xAxisIndexC][i],resultArray[index][yAxisIndexC][i]);
        }
        wingResultSeriesC[index]->setPen(getPen(index));
        //updateAxis(axisXWC,axisYWC,xAxisIndexC,yAxisIndexC);
        updateAxes(chartWC,wingResultSeriesC);
    }
}
void wingDisplay::drawResultChartD(const int index){

    if(!resultArray[index].isEmpty()){
        if(wingResultSeriesD[index]->count() > 0)
            wingResultSeriesD[index]->clear();
        for(int i = 0;i<resultArray[index][0].length();i++){
            wingResultSeriesD[index]->append(resultArray[index][xAxisIndexD][i],resultArray[index][yAxisIndexD][i]);
        }
        wingResultSeriesD[index]->setPen(getPen(index));
        //updateAxis(axisXWD,axisYWD,xAxisIndexD,yAxisIndexD);
        updateAxes(chartWD,wingResultSeriesD);
    }
}
void wingDisplay::drawSpanResult(const int index){
    if(!resultArray[index].isEmpty()){
        if(spanSeriesA->count() > 0)
            spanSeriesA->clear();
        int ind = wingAlphaCombox->currentIndex();
        spanSeriesA->setPen(getPen(index));
        QVector<double>yt;
        QVector<double>result;
        switch (spanResultTypeIndex) {
        case 0:
            yt = VLMSolverArray[index]->spanForceYt;
            result = VLMSolverArray[index]->spanForce[ind];
            if(spanSeriesB->count()>0)
                spanSeriesB->clear();
            for(int i = 0;i<yt.length();i++)
                spanSeriesB->append(yt[i],VLMSolverArray[index]->ideaSpanForce[ind][i]);

            break;
        case 1:
            yt = VLMSolverArray[index]->spanForceYt;
            result = VLMSolverArray[index]->spanLiftCoefficient[ind];
            break;
        case 2:
            yt = VLMSolverArray[index]->yt;
            result = VLMSolverArray[index]->spanMonmentArray[ind];
            break;
        case 3:
            yt = VLMSolverArray[index]->spanForceYt;
            result = VLMSolverArray[index]->spanLiftForce[ind];
            break;
        default:
            break;
        }



        for(int i = 0;i<yt.length();i++)
            spanSeriesA->append(yt[i],result[i]);
        spanXAxis->setRange(mathSolver.minV(yt) * 1.05,mathSolver.maxV(yt) * 1.05);
        spanYAxis->setRange(mathSolver.minV(result) * 1.05,mathSolver.maxV(result) * 1.05);
    }

}

void wingDisplay::drawOptIterSeries(const int x,const double value){
    iterSeriesA->append(x,value);
}
void wingDisplay::drawOptSpanLiftSeries(const QVector<double>&x,const QVector<double>&y1,const QVector<double>&y2){
    if(iterSeriesB->count() > 0){
        iterSeriesB->clear();
        iterSeriesC->clear();
    }

    iterXAxisB->setRange(x[0] * 1.2,-x[0] * 1.2);
    iterYAxisB->setRange(mathSolver.minV(y1) * 1.2,mathSolver.maxV(y1) * 1.2);
    for(int i = 0;i<x.length();i++){
        iterSeriesB->append(x[i],y1[i]);
        iterSeriesC->append(x[i],y2[i]);
    }

}
QString wingDisplay::wingDataToString(wingDefinition&wingData){
    int index = wingAlphaCombox->currentIndex();
    QString text,tmp[14];

    wingData.computeWingMessage();
    int len = wingData.chordLengthW.length() - 1;
    tmp[0] = wingData.name +  "\n"+ "翼展:" + QString::number(wingData.spanW[len] * 2,'f',2) +"m" +  "\n";
    tmp[1] = "参考面积:" + QString::number(wingData.Area(),'f',2) +"㎡" + "\n";
    tmp[2] = "展弦比:" + QString::number(wingData.AspectRatio(),'f',2) + "\n";
    tmp[3] = "平均气动弦长" + QString::number(wingData.RealChord(),'f',2) + "m" + "\n";

    if(saveType == WING_WING){
        if(resultArray[wingChoiceIndex].isEmpty()){
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
            double v = VLMSolverArray[wingChoiceIndex]->vinf;
            double tmpP = VLMSolverArray[wingChoiceIndex]->density * v * v * 0.5 * wingData.Area();
            tmp[4] = "迎角:" + QString::number(resultArray[wingChoiceIndex][0][index],'f',2) + "\n";
            tmp[5] = "升力系数:" + QString::number(resultArray[wingChoiceIndex][1][index],'f',3) + "\n";
            tmp[6] = "阻力系数:" + QString::number(resultArray[wingChoiceIndex][2][index],'f',4) + "\n";
            tmp[7] = "力矩系数:" + QString::number(resultArray[wingChoiceIndex][3][index],'f',4) + "\n";
            tmp[8] = "升阻比:" + QString::number(resultArray[wingChoiceIndex][4][index],'f',2) + "\n";
            tmp[9] = "功率因子:" + QString::number(resultArray[wingChoiceIndex][5][index],'f',2) + "\n";
            tmp[10] = "速度:" + QString::number(VLMSolverArray[wingChoiceIndex]->vinf,'f',2) +"m/s" + "\n";
            tmp[11] = "升力:" + QString::number(VLMSolverArray[wingChoiceIndex]->wingCL[index] * tmpP,'f',2)+"N" + "\n";
            tmp[12] = "阻力:" + QString::number(VLMSolverArray[wingChoiceIndex]->wingCD[index] * tmpP,'f',2)+"N" + "\n";
            tmp[13] = "翼载荷:" + QString::number(VLMSolverArray[wingChoiceIndex]->wingCL[index] * tmpP / wingData.Area() / 9.81,'f',2) + "kg/㎡";
        }
    }


    for(int i = 0;i<14;i++){
        text += tmp[i];
    }
    return text;
}
QString wingDisplay::resultDataToString(wingDefinition&wingData,const wingVLM&solver){

    QString text;
    wingData.computeWingMessage();
    int len = wingData.chordLengthW.length() - 1;
    QString tmp1 = "迎角:" + QString::number(solver.designAlpha,'f',2) + "\n";
    QString tmp2 = "参考面积:" + QString::number(wingData.Area(),'f',2) + "\n";
    QString tmp3 = "翼展:" + QString::number(wingData.spanW[len],'f',2) + "\n";
    QString tmp4,tmp5,tmp6,tmp7,tmp8,tmp9;
    tmp4 = "展弦比:" + QString::number(wingData.AspectRatio(),'f',2) + "\n";
    tmp5 = "升力系数:" + QString::number(solver.fixCl,'f',2) + "\n";
    tmp6 = "阻力系数:" + QString::number(solver.fixCd,'f',2) + "\n";
    tmp7 = "力矩系数:" + QString::number(solver.fixCm,'f',2) + "\n";

    if(solver.fixCd < 0.0001){
        tmp8 = "升阻比:" + QString::number(0,'f',2) + "\n";
        tmp9 = "功率因子:" + QString::number(0,'f',2);
    }else{
        tmp8 = "升阻比:" + QString::number(solver.fixCl / solver.fixCd,'f',2) + "\n";
        tmp9 = "功率因子:" + QString::number(pow(solver.fixCl,1.5) / solver.fixCd,'f',2);
    }


    text = tmp1 + tmp2 + tmp3 + tmp4 + tmp5 + tmp6 + tmp7 + tmp8 + tmp9;
    return text;
}


void wingDisplay::startAnalyseWing(){
    //getWingSetting();

    QVector<QVector<double>>resultTmp;
    wingVLM *solvers = VLMSolverArray[wingChoiceIndex];
    solvers->refreshParaments(wingSettingArray[wingChoiceIndex]);
    solvers->initialGeometry(wingDataArray[wingChoiceIndex]);

    QFuture<void>future = QtConcurrent::run([solvers](){
        solvers->solver();

    });

    bool allFinished = false;
    while(!allFinished){
        allFinished = true;
        if(!future.isFinished()){
            allFinished = false;
        }
        QCoreApplication::processEvents();
        if (!allFinished)
            QThread::msleep(1);
    }


    //保存计算结果
    QVector<double>tmp1,tmp2,tmp3,tmp4,tmp5,tmp6;
    for(int i = 0;i<solvers->wingCD.length();i++){
        double cl = solvers->wingCL[i];
        double cd = solvers->wingCD[i];
        double cm = solvers->wingCM[i];
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
    resultArray[wingChoiceIndex] = resultTmp;

    //更新机翼设计迎角
    wingAlphaCombox->blockSignals(true);
    wingAlphaCombox->clear();
    for(int i = 0;i<tmp1.length();i++){
        wingAlphaCombox->addItem(QString::number(tmp1[i]));
    }
    wingAlphaCombox->blockSignals(false);



    solvers = nullptr;
    drawResultChartA(wingChoiceIndex);
    drawResultChartB(wingChoiceIndex);
    drawResultChartC(wingChoiceIndex);
    drawResultChartD(wingChoiceIndex);
    drawSpanResult(wingChoiceIndex);
    updateAxes(chartWA,wingResultSeriesA);
    updateAxes(chartWB,wingResultSeriesB);
    updateAxes(chartWC,wingResultSeriesC);
    updateAxes(chartWD,wingResultSeriesD);

    //drawWingResult(wingChoiceIndex);
    updateGLTextA(wingDataToString(wingDataArray[wingChoiceIndex]));//刷新模型参数
    showResultNode(wingTreeItemArray[wingChoiceIndex],"后处理");


}

void wingDisplay::saveData(){
    switch (saveType) {
    case 0:
        saveWingData();


        break;
    case 1:
        saveTailData();

        break;
    case 2:
        saveTailData();

        break;
    default:
        break;
    }

    updateListData();
    if(isAirplaneSingal){
        emit emitWingDefineFinish();
        isAirplaneSingal = false;
    }


}



void wingDisplay::saveTailData(){

    wingDefineWidget->hide();
    wingDefinition newWingData = getWingDefine();
    newWingData.computeWingMessage();
    newWingData.interDataArray = interDataArray;
    //vTailDataArray.append(newWingData);

    if(!isModify){

        tailDataArray.append(newWingData);

        QString path1 = ":/images/edit.png";
        QIcon icon1 ;
        icon1.addPixmap(QPixmap(path1),QIcon::Normal,QIcon::On);

        QString path2 = ":/images/wing.png";
        QIcon icon2 ;
        icon2.addPixmap(QPixmap(path2),QIcon::Normal,QIcon::On);

        QTreeWidgetItem *fItem = new QTreeWidgetItem(tailTreeWidget,QStringList(QString(newWingData.name)));
        fItem->setData(0,Qt::UserRole, QVariant(QString::number(tailIndex)));
        fItem->setIcon(0,icon2);

        QTreeWidgetItem *fItemGeometry = new QTreeWidgetItem(fItem,QStringList("几何模型"));
        fItemGeometry->setData(0,Qt::UserRole,"fItemGeometry");

        QTreeWidgetItem *childShape = new QTreeWidgetItem(fItemGeometry,QStringList("平面形状"));
        childShape->setData(0, Qt::UserRole, QVariant("tailChildShape" + QString::number(tailIndex)));
        childShape->setIcon(0,icon1);

        QTreeWidgetItem *childAirfoil = new QTreeWidgetItem(fItemGeometry,QStringList("翼型"));
        childAirfoil->setData(0, Qt::UserRole, QVariant("childAirfoil" + QString::number(tailIndex)));
        for(int i = 0;i<newWingData.airfoilNameArray.length();i++){
            QTreeWidgetItem *childAirfoilTmp = new QTreeWidgetItem(childAirfoil,QStringList(newWingData.airfoilNameArray[i]));
            childAirfoilTmp->setData(0,Qt::UserRole,QVariant("airfoil" + QString::number(i)));
            childAirfoilTmp->setIcon(0,icon1);
        }

        tailTreeItemArray.append(fItem);
        tailChoiceIndex = tailIndex;
        tailIndex++;
    }else{

        tailTreeItemArray[tailChoiceIndex]->setText(0,newWingData.name);
        QTreeWidgetItemIterator it(tailTreeWidget);
        while (*it) {
            QTreeWidgetItem *item = *it;
            if (item->text(0) == "翼型") {
                for (int i = 0; i < item->childCount(); ++i) {
                    QTreeWidgetItem *childAirfoilTmp = item->child(i);
                    // 修改每个 "airfoil" 项的名称
                    childAirfoilTmp->setText(0,newWingData.airfoilNameArray[i]);
                }
                break;
            }
            ++it;
        }
        //vTailDataArray[tailChoiceIndex] = newWingData;
        tailDataArray[tailChoiceIndex] = newWingData;

    }

    updateGLTextA(wingDataToString(newWingData));//刷新模型参数
    updateWingDefineWidget(newWingData);//刷新模型视图
}
void wingDisplay::saveWingData(){
    wingDefineWidget->hide();
    wingDefinition newWingData = getWingDefine();
    newWingData.computeWingMessage();
    newWingData.interDataArray = interDataArray;

    if(!isModify){


        wingVLM *solver = new wingVLM;
        solver->initialGeometry(newWingData);
        VLMSolverArray.append(solver);

        QString path1 = ":/images/edit.png";
        QIcon icon1 ;
        icon1.addPixmap(QPixmap(path1),QIcon::Normal,QIcon::On);

        QString path2 = ":/images/wing.png";
        QIcon icon2 ;
        icon2.addPixmap(QPixmap(path2),QIcon::Normal,QIcon::On);

        // 创建顶层项目
        //创建两个节点

        QTreeWidgetItem *fItem = new QTreeWidgetItem(wingTreeWidget,QStringList(QString(newWingData.name)));
        fItem->setData(0,Qt::UserRole, QVariant(QString::number(wingIndex)));
        fItem->setIcon(0,icon2);

        QTreeWidgetItem *fItemGeometry = new QTreeWidgetItem(fItem,QStringList("几何模型"));
        fItemGeometry->setData(0,Qt::UserRole,"fItemGeometry");

        QTreeWidgetItem *childShape = new QTreeWidgetItem(fItemGeometry,QStringList("平面形状"));
        childShape->setData(0, Qt::UserRole, QVariant("childShape" + QString::number(wingIndex)));
        childShape->setIcon(0,icon1);

        QTreeWidgetItem *childAirfoil = new QTreeWidgetItem(fItemGeometry,QStringList("翼型"));
        childAirfoil->setData(0, Qt::UserRole, QVariant("childAirfoil" + QString::number(wingIndex)));
        for(int i = 0;i<newWingData.airfoilNameArray.length();i++){
            QTreeWidgetItem *childAirfoilTmp = new QTreeWidgetItem(childAirfoil,QStringList(newWingData.airfoilNameArray[i]));
            childAirfoilTmp->setData(0,Qt::UserRole,QVariant("airfoil" + QString::number(i)));
            childAirfoilTmp->setIcon(0,icon1);
        }
        QTreeWidgetItem *fItemAnalyse = new QTreeWidgetItem(fItem,QStringList("求解设置"));
        fItemAnalyse->setData(0, Qt::UserRole, QVariant("childSolver" + QString::number(wingIndex)));
        //fItemAnalyse->setData(0, Qt::UserRole, "fItemAnalyse");


        QTreeWidgetItem *childVinf = new QTreeWidgetItem(fItemAnalyse,QStringList("来流设置"));
        childVinf->setData(0, Qt::UserRole, QVariant("childVinf" + QString::number(wingIndex)));
        childVinf->setIcon(0,icon1);

        QTreeWidgetItem *childZeroLiftDrag = new QTreeWidgetItem(fItemAnalyse,QStringList("零升阻力"));
        childZeroLiftDrag->setData(0, Qt::UserRole, QVariant("childZeroLiftDrag" + QString::number(wingIndex)));
        childZeroLiftDrag->setCheckState(0,Qt::Unchecked);

        QTreeWidgetItem *childXfoilDrag = new QTreeWidgetItem(fItemAnalyse,QStringList("Xfoil阻力"));
        childXfoilDrag->setData(0, Qt::UserRole, QVariant("childXfoilDrag" + QString::number(wingIndex)));
        childXfoilDrag->setCheckState(0,Qt::Unchecked);

        //childZeroLiftDrag->setData(0, Qt::UserRole, QVariant(QString::number(wingIndex) + "childZeroLiftDrag"));

        QTreeWidgetItem *fItemResult = new QTreeWidgetItem(fItem,QStringList("后处理"));
        QTreeWidgetItem *childPrssure = new QTreeWidgetItem(fItemResult,QStringList("压力"));
        childPrssure->setData(0, Qt::UserRole, QVariant("childPrssure" + QString::number(wingIndex)));
        childPrssure->setCheckState(0,Qt::Unchecked);

        QTreeWidgetItem *childStreamLine = new QTreeWidgetItem(fItemResult,QStringList("流线"));
        childStreamLine->setData(0, Qt::UserRole, QVariant("childStreamLine" + QString::number(wingIndex)));
        childStreamLine->setCheckState(0,Qt::Unchecked);

        QTreeWidgetItem *childSeriesType = new QTreeWidgetItem(fItemResult,QStringList("曲线类型"));
        childSeriesType->setData(0, Qt::UserRole, QVariant("childSeriesType" + QString::number(wingIndex)));
        childSeriesType->setIcon(0,icon1);

        fItemResult->setHidden(true);


        // 连接信号和槽

        wingTreeItemArray.append(fItem);

        int r =rand()%255;
        int b =rand()%255;
        int g =rand()%255;

        QColor color(r,b,g);

        colorArray.append(color);
        resultPenArray.append(0);
        PlotSeries *seriesA = new PlotSeries;
        PlotSeries *seriesB = new PlotSeries;
        PlotSeries *seriesC = new PlotSeries;
        PlotSeries *seriesD = new PlotSeries;


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



        wingResultSeriesA.append(seriesA);
        wingResultSeriesB.append(seriesB);
        wingResultSeriesC.append(seriesC);
        wingResultSeriesD.append(seriesD);


        seriesA->setName(newWingData.name);
        seriesB->setName(newWingData.name);
        seriesC->setName(newWingData.name);
        seriesD->setName(newWingData.name);
        spanSeriesA->setName(newWingData.name);

        //QVector<double>clTmp;
        // clTmp.append(0.0);
        // wingLift.append(clTmp);

        QVector<QVector<double>>tmp;
        resultArray.append(tmp);
        VLMSetting setting(0,10,1,20,0,0);
        wingSettingArray.append(setting);

        wingListCombox->addItem(newWingData.name);//添加机翼到列表
        //导入优化参数
        WingGaParameters sett;


        QVector<double>minChord;QVector<double>minOffset;QVector<double>minTwist;QVector<double>minDihed;
        QVector<double>maxChord;QVector<double>maxOffset;QVector<double>maxTwist;QVector<double>maxDihed;
        QVector<bool>airfoilBool;QVector<bool>spanBool;QVector<bool>chordBool;QVector<bool>offsetBool;QVector<bool>twistBool;QVector<bool>dihedBool;


        connect(solver, &wingVLM::progressUpdated, this, &wingDisplay::updateProgress);
        connect(solver, &wingVLM::workFinished, this, &wingDisplay::onCalculationFinished);
        for(int i = 0;i<newWingData.chordLengthW.length();i++){
            newWingData.cstPointYArray.append(cstPointY);
            minChord.append(0);
            maxChord.append(newWingData.chordLengthW[i] + 0.5);
            minOffset.append(newWingData.offsetLengthW[i] - 0.5);
            maxOffset.append(newWingData.offsetLengthW[i] + 0.5);
            minTwist.append(newWingData.twistAngleW[i] - 10);
            maxTwist.append(newWingData.twistAngleW[i] + 10);
            minDihed.append(0);
            maxDihed.append(90);

            airfoilBool.append(false);
            spanBool.append(false);
            chordBool.append(true);
            offsetBool.append(true);
            twistBool.append(true);
            dihedBool.append(false);
        }





        wingDataArray.append(newWingData);

        sett.MinSpan = 0;
        sett.MaxSpan = newWingData.Span() + 2;
        sett.minChord = minChord;
        sett.maxChord = maxChord;
        sett.minOffsetLength = minOffset;
        sett.maxOffsetLength = maxOffset;
        sett.minTwisAngle = minTwist;
        sett.maxTwisAngle = maxTwist;
        sett.minDihedralAngle = minDihed;
        sett.maxDihedralAngle = maxDihed;
        sett.spanVal = 0.01;
        sett.chordVal = 0.01;
        sett.offsetLengthVal = 0.01;
        sett.twisAngleVal = 0.01;
        sett.dihedralAngleVal = 0.01;
        sett.airfoilChangeArray = airfoilBool;
        sett.spanChangeArray = spanBool;
        sett.chordChangeArray = chordBool;
        sett.offsetChangeArray = offsetBool;
        sett.twistChangeArray = twistBool;
        sett.dihedChangeArray = dihedBool;

        optSettingArray.append(sett);
        optRealSettingArray.append(sett);
        optRangeChange(wingIndex);//刷新模型优化参数



        connect(solver,&wingVLM::emitProgressValue,this,&wingDisplay::changeProgressUpdate);

        wingChoiceIndex = wingIndex;
        wingIndex++;
    }else{

        //qDebug()<<newWingData.airfoilArray.length();
        wingTreeItemArray[wingChoiceIndex]->setText(0,newWingData.name);
        QTreeWidgetItemIterator it(wingTreeWidget);
        while (*it) {
            QTreeWidgetItem *item = *it;
            if (item->text(0) == "翼型") {
                for (int i = 0; i < item->childCount(); ++i) {
                    QTreeWidgetItem *childAirfoilTmp = item->child(i);
                    // 修改每个 "airfoil" 项的名称
                    childAirfoilTmp->setText(0,newWingData.airfoilNameArray[i]);
                }
                break;
            }
            ++it;
        }

        updateOptUI(newWingData);

        wingDataArray[wingChoiceIndex] = newWingData;

        wingResultSeriesA[wingChoiceIndex]->setName(newWingData.name);
        wingResultSeriesB[wingChoiceIndex]->setName(newWingData.name);
        wingResultSeriesC[wingChoiceIndex]->setName(newWingData.name);
        wingResultSeriesD[wingChoiceIndex]->setName(newWingData.name);
        spanSeriesA->setName(newWingData.name);


    }


    hidePressureContourView();//关闭云图
    updateGLTextA(wingDataToString(newWingData));//刷新模型参数
    updateWingDefineWidget(newWingData);//刷新模型视图


}

void wingDisplay::initialWingDesignerModel(){
    wingHLayout = new QHBoxLayout(wingDefineWidget);
    wingVLayout = new QVBoxLayout();

    wingDefineBox = new QGroupBox(wingDefineWidget);
    wingMessageBox = new QGroupBox(wingDefineWidget);



    wingVLayout->addWidget(wingDefineBox);
    wingHLayout->addLayout(wingVLayout);
    wingHLayout->addWidget(wingMessageBox);


    wingDefineLayout = new QGridLayout(wingDefineBox);
    wingMessageLayout = new QGridLayout(wingMessageBox);

    wingDefineBox->setFixedHeight(150);
    wingMessageBox->setMaximumWidth(300);




    wingTypeCombobox = new QComboBox(wingDefineWidget);
    chordLenthgLabel = new QLabel(wingDefineWidget);
    spanLabel = new QLabel(wingDefineWidget);
    twistAngleLabel = new QLabel(wingDefineWidget);
    dihedralAngleLabel = new QLabel(wingDefineWidget);
    offsetLengthLabel = new QLabel(wingDefineWidget);
    nameLabel = new QLabel(wingDefineWidget);
    xGridNumLabel = new QLabel(wingDefineWidget);
    yGridNumLabel = new QLabel(wingDefineWidget);




    QLabel *labelA = new QLabel(wingDefineWidget);
    QLineEdit *editA1 = new QLineEdit(wingDefineWidget);
    QLineEdit *editA2 = new QLineEdit(wingDefineWidget);
    QLineEdit *editA3 = new QLineEdit(wingDefineWidget);
    QLineEdit *editA4 = new QLineEdit(wingDefineWidget);
    QLineEdit *editA5 = new QLineEdit(wingDefineWidget);
    QLineEdit *editA6 = new QLineEdit(wingDefineWidget);
    QLineEdit *editA7 = new QLineEdit(wingDefineWidget);
    QComboBox *comboboxA = new QComboBox(wingDefineWidget);


    QLabel *labelB = new QLabel(wingDefineWidget);
    QLineEdit *editB1 = new QLineEdit(wingDefineWidget);
    QLineEdit *editB2 = new QLineEdit(wingDefineWidget);
    QLineEdit *editB3 = new QLineEdit(wingDefineWidget);
    QLineEdit *editB4 = new QLineEdit(wingDefineWidget);
    QLineEdit *editB5 = new QLineEdit(wingDefineWidget);
    QLineEdit *editB6 = new QLineEdit(wingDefineWidget);
    QLineEdit *editB7 = new QLineEdit(wingDefineWidget);
    QComboBox *comboboxB = new QComboBox(wingDefineWidget);




    editA1->setText("0");editA2->setText("400");editA3->setText("0");editA4->setText("0");editA5->setText("0");editA6->setText("10");editA7->setText("10");
    editB1->setText("1000");editB2->setText("200");editB3->setText("0");editB4->setText("0");editB5->setText("0");editB6->setText("10");editB7->setText("10");



    profileLabel.append(labelA);
    profileLabel.append(labelB);
    profileSpanEdit.append(editA1);
    profileSpanEdit.append(editB1);
    profilechordLengthEdit.append(editA2);
    profilechordLengthEdit.append(editB2);
    profileOffsetLengthEdit.append(editA3);
    profileOffsetLengthEdit.append(editB3);
    profileTwistAngleEdit.append(editA4);
    profileTwistAngleEdit.append(editB4);
    profileDihedralAngleEdit.append(editA5);
    profileDihedralAngleEdit.append(editB5);
    profileXGridEdit.append(editA6);
    profileXGridEdit.append(editB6);
    profileYGridEdit.append(editA7);
    profileYGridEdit.append(editB7);
    profileAirfoilChoiceCombobox.append(comboboxA);
    profileAirfoilChoiceCombobox.append(comboboxB);


    wingTypeCombobox->addItem("梯形翼");
    wingTypeCombobox->addItem("多段翼");
    wingTypeCombobox->addItem("椭圆翼");

    spanLabel->setText("翼展(mm)");
    chordLenthgLabel->setText("弦长(mm)");
    offsetLengthLabel->setText("偏移(mm)");
    twistAngleLabel->setText("扭转角(°)");
    dihedralAngleLabel->setText("上反角(°)");
    xGridNumLabel->setText("X网格数");
    yGridNumLabel->setText("Y网格数");
    nameLabel->setText("翼型名称");



    for(int i = 0;i<AirfoilNameArray.length();i++){
        comboboxA->addItem("平板翼型");
        comboboxB->addItem("平板翼型");
    }
    comboboxA->setCurrentIndex(0);
    comboboxB->setCurrentIndex(0);


    wingDefineLayout->addWidget(wingTypeCombobox,0,0,1,1);
    wingDefineLayout->addWidget(spanLabel,0,1,1,1);
    wingDefineLayout->addWidget(chordLenthgLabel,0,2,1,1);
    wingDefineLayout->addWidget(offsetLengthLabel,0,3,1,1);
    wingDefineLayout->addWidget(twistAngleLabel,0,4,1,1);
    wingDefineLayout->addWidget(dihedralAngleLabel,0,5,1,1);
    wingDefineLayout->addWidget(xGridNumLabel,0,6,1,1);
    wingDefineLayout->addWidget(yGridNumLabel,0,7,1,1);
    wingDefineLayout->addWidget(nameLabel,0,8,1,1);




    labelA->setText("截面(1)");
    labelB->setText("截面(2)");

    wingDefineLayout->addWidget(labelA,1,0,1,1);
    wingDefineLayout->addWidget(editA1,1,1,1,1);
    wingDefineLayout->addWidget(editA2,1,2,1,1);
    wingDefineLayout->addWidget(editA3,1,3,1,1);
    wingDefineLayout->addWidget(editA4,1,4,1,1);
    wingDefineLayout->addWidget(editA5,1,5,1,1);
    wingDefineLayout->addWidget(editA6,1,6,1,1);
    wingDefineLayout->addWidget(editA7,1,7,1,1);
    wingDefineLayout->addWidget(comboboxA,1,8,1,1);

    wingDefineLayout->addWidget(labelB,2,0,1,1);
    wingDefineLayout->addWidget(editB1,2,1,1,1);
    wingDefineLayout->addWidget(editB2,2,2,1,1);
    wingDefineLayout->addWidget(editB3,2,3,1,1);
    wingDefineLayout->addWidget(editB4,2,4,1,1);
    wingDefineLayout->addWidget(editB5,2,5,1,1);
    wingDefineLayout->addWidget(editB6,2,6,1,1);
    wingDefineLayout->addWidget(editB7,2,7,1,1);
    wingDefineLayout->addWidget(comboboxB,2,8,1,1);



    connect(editA1,&QLineEdit::editingFinished,this,&wingDisplay::updateWingData);
    connect(editA2,&QLineEdit::editingFinished,this,&wingDisplay::updateWingData);
    connect(editA3,&QLineEdit::editingFinished,this,&wingDisplay::updateWingData);
    connect(editA4,&QLineEdit::editingFinished,this,&wingDisplay::updateWingData);
    connect(editA5,&QLineEdit::editingFinished,this,&wingDisplay::updateWingData);
    connect(editA6,&QLineEdit::editingFinished,this,&wingDisplay::updateWingData);
    connect(editA7,&QLineEdit::editingFinished,this,&wingDisplay::updateWingData);

    connect(editB1,&QLineEdit::editingFinished,this,&wingDisplay::updateWingData);
    connect(editB2,&QLineEdit::editingFinished,this,&wingDisplay::updateWingData);
    connect(editB3,&QLineEdit::editingFinished,this,&wingDisplay::updateWingData);
    connect(editB4,&QLineEdit::editingFinished,this,&wingDisplay::updateWingData);
    connect(editB5,&QLineEdit::editingFinished,this,&wingDisplay::updateWingData);
    connect(editB6,&QLineEdit::editingFinished,this,&wingDisplay::updateWingData);
    connect(editB7,&QLineEdit::editingFinished,this,&wingDisplay::updateWingData);

    // 连接 comboboxA 的 currentIndexChanged 信号到槽函数 updateWingData
    connect(comboboxA, QOverload<int>::of(&QComboBox::activated),
            this, &wingDisplay::updateWingData);

    // 连接 comboboxB 的 currentIndexChanged 信号到槽函数 updateWingData
    connect(comboboxB, QOverload<int>::of(&QComboBox::activated),
            this, &wingDisplay::updateWingData);

    // 连接 wingTypeCombobox 的 currentIndexChanged 信号到槽函数 defineWingTypes
    connect(wingTypeCombobox, QOverload<int>::of(&QComboBox::activated),
            this, &wingDisplay::defineWingTypes);



}

void wingDisplay::updateView(const wingDefinition&newWing){

    wingVLM ned;

    ned.initialGeometry(newWing);


    if (ned.checkGeometry()){
        points->Reset();
        lines->Reset();


        for (int i = 0; i < ned.getMeshNum() * 2; i++) {
            points->InsertNextPoint(ned.meshA[i].x, ned.meshA[i].y, ned.meshA[i].z);
            points->InsertNextPoint(ned.meshB[i].x, ned.meshB[i].y, ned.meshB[i].z);
            points->InsertNextPoint(ned.meshC[i].x, ned.meshC[i].y, ned.meshC[i].z);
            points->InsertNextPoint(ned.meshD[i].x, ned.meshD[i].y, ned.meshD[i].z);
            // points->InsertNextPoint(ned.xyA[i].x, ned.xyA[i].y, ned.xyA[i].z);
            // points->InsertNextPoint(ned.xyB[i].x, ned.xyB[i].y, ned.xyB[i].z);
            // points->InsertNextPoint(ned.xyC[i].x, ned.xyC[i].y, ned.xyC[i].z);
            // points->InsertNextPoint(ned.xyD[i].x, ned.xyD[i].y, ned.xyD[i].z);
            // points->InsertNextPoint(ned.xyTA[i].x, ned.xyTA[i].y, ned.xyTA[i].z);
            // points->InsertNextPoint(ned.xyTB[i].x, ned.xyTB[i].y, ned.xyTB[i].z);
            // points->InsertNextPoint(ned.xyTC[i].x, ned.xyTC[i].y, ned.xyTC[i].z);
            // points->InsertNextPoint(ned.xyTD[i].x, ned.xyTD[i].y, ned.xyTD[i].z);

    }
        for (vtkIdType i = 0; i < points->GetNumberOfPoints() - 3; i += 4) {
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
            lines->InsertNextCell(line1);
            lines->InsertNextCell(line2);
            lines->InsertNextCell(line3);
            lines->InsertNextCell(line4);
    }

        renwin->Render();

    }



}

void wingDisplay::initialWingShowModel(){


    renderer = vtkSmartPointer<vtkRenderer>::New();
    //renderer->SetBackground(0, 0, 0);

    renwin = vtkSmartPointer<vtkGenericOpenGLRenderWindow>::New();
    renwin->AddRenderer(renderer);

    wingDisplayWidgetA = new QVTKOpenGLNativeWidget;
    wingDisplayWidgetA->setRenderWindow(renwin);
    wingDisplayWidgetA->setMinimumHeight(400);




    // 创建两个截面数据
    points = vtkSmartPointer<vtkPoints>::New();


    lines = vtkSmartPointer<vtkCellArray>::New();


    profilePolyData = vtkSmartPointer<vtkPolyData>::New();
    profilePolyData->SetPoints(points);
    profilePolyData->SetLines(lines);

    appendFilter = vtkSmartPointer<vtkAppendPolyData>::New();
    appendFilter->AddInputData(profilePolyData);
    appendFilter->Update();

    // 创建Mapper和Actor
    mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
    mapper->SetInputConnection(appendFilter->GetOutputPort());
    actor = vtkSmartPointer<vtkActor>::New();
    actor->SetMapper(mapper);

    // 将 Actor 添加到渲染器中
    renderer->AddActor(actor);


    // 设置背景颜色
    renderer->SetBackground(0.1, 0.2, 0.4); // 深蓝色背景


    // 设置光照效果
    lightKit = vtkSmartPointer<vtkLightKit>::New();
    lightKit->AddLightsToRenderer(renderer);

    // 获取 QVTKOpenGLNativeWidget 提供的交互器
    vtkRenderWindowInteractor* interactor = this->wingDisplayWidgetA->interactor();

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
    wingVLayout->addWidget(wingDisplayWidgetA);
    //wingDisplayWidgetA->show();
}
void wingDisplay::initialWingDefineWidget(){


    // 初始化 VTK 渲染器和窗口
    rendererB = vtkSmartPointer<vtkRenderer>::New();
    renwinB = vtkSmartPointer<vtkGenericOpenGLRenderWindow>::New();
    renwinB->AddRenderer(rendererB);
    wingDisplayWidgetB->setRenderWindow(renwinB);

    // 初始化 VTK 点和单元格数组

    sphereSourceCg = vtkSmartPointer<vtkSphereSource>::New();
    sphereSourceCg->SetRadius(0.02);
    sphereSourceCg->SetThetaResolution(50);
    sphereSourceCg->SetPhiResolution(50);


    pointsB = vtkSmartPointer<vtkPoints>::New();
    pointsS = vtkSmartPointer<vtkPoints>::New();
    pointsC = vtkSmartPointer<vtkPoints>::New();
    pointsH = vtkSmartPointer<vtkPoints>::New();
    pointsV = vtkSmartPointer<vtkPoints>::New();

    linesB = vtkSmartPointer<vtkCellArray>::New();
    linesS = vtkSmartPointer<vtkCellArray>::New();
    linesC = vtkSmartPointer<vtkCellArray>::New();
    linesH = vtkSmartPointer<vtkCellArray>::New();
    linesV = vtkSmartPointer<vtkCellArray>::New();

    profilePolyDataB = vtkSmartPointer<vtkPolyData>::New();
    profilePolyDataB->SetPoints(pointsB);
    profilePolyDataB->SetPolys(linesB);

    profilePolyDataS = vtkSmartPointer<vtkPolyData>::New();
    profilePolyDataS->SetPoints(pointsS);
    profilePolyDataS->SetLines(linesS);

    profilePolyDataC = vtkSmartPointer<vtkPolyData>::New();
    profilePolyDataC->SetPoints(pointsC);
    profilePolyDataC->SetPolys(linesC);

    profilePolyDataH = vtkSmartPointer<vtkPolyData>::New();
    profilePolyDataH->SetPoints(pointsH);
    profilePolyDataH->SetPolys(linesH);

    profilePolyDataV = vtkSmartPointer<vtkPolyData>::New();
    profilePolyDataV->SetPoints(pointsH);
    profilePolyDataV->SetPolys(linesH);

    // appendFilterB = vtkSmartPointer<vtkAppendPolyData>::New();
    // appendFilterB->AddInputData(profilePolyDataB);
    // appendFilterB->Update();

    // 初始化 VTK 映射器和演员
    mapperB = vtkSmartPointer<vtkPolyDataMapper>::New();
    mapperB->SetInputData(profilePolyDataB);
    actorB = vtkSmartPointer<vtkActor>::New();
    actorB->SetMapper(mapperB);

    mapperS = vtkSmartPointer<vtkPolyDataMapper>::New();
    mapperS->SetInputData(profilePolyDataS);
    actorS = vtkSmartPointer<vtkActor>::New();
    actorS->GetProperty()->SetColor(0.8, 0.6, 1.0); // 红色
    actorS->SetMapper(mapperS);

    mapperC = vtkSmartPointer<vtkPolyDataMapper>::New();
    mapperC->SetInputData(profilePolyDataC);
    actorC = vtkSmartPointer<vtkActor>::New();
    actorC->SetMapper(mapperC);

    mapperH = vtkSmartPointer<vtkPolyDataMapper>::New();
    mapperH->SetInputData(profilePolyDataH);
    actorH = vtkSmartPointer<vtkActor>::New();
    actorH->SetMapper(mapperH);


    mapperV = vtkSmartPointer<vtkPolyDataMapper>::New();
    mapperV->SetInputData(profilePolyDataV);
    actorV = vtkSmartPointer<vtkActor>::New();
    actorV->SetMapper(mapperV);

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

    // 初始化颜色数组
    colorB = vtkSmartPointer<vtkUnsignedCharArray>::New();
    colorB->SetNumberOfComponents(3); // 设置为 RGB
    colorB->SetName("Colors");
    profilePolyDataB->GetCellData()->SetScalars(colorB);

    // 初始化颜色数组
    colorH = vtkSmartPointer<vtkUnsignedCharArray>::New();
    colorH->SetNumberOfComponents(3); // 设置为 RGB
    colorH->SetName("Colors");
    profilePolyDataH->GetCellData()->SetScalars(colorH);

    // 初始化颜色数组
    colorV = vtkSmartPointer<vtkUnsignedCharArray>::New();
    colorV->SetNumberOfComponents(3); // 设置为 RGB
    colorV->SetName("Colors");
    profilePolyDataV->GetCellData()->SetScalars(colorV);

    colorC = vtkSmartPointer<vtkUnsignedCharArray>::New();
    colorC->SetNumberOfComponents(3); // 设置为 RGB
    colorC->SetName("Colors");
    profilePolyDataC->GetCellData()->SetScalars(colorC);





    // 将 Actor 添加到渲染器中
    rendererB->AddActor(actorB);
    rendererB->AddActor(actorS);
    rendererB->AddActor(actorC);
    rendererB->AddActor(actorH);
    rendererB->AddActor(actorV);
    rendererB->AddActor(actorCg);
    rendererB->AddActor2D(textActorA);

    actorB->VisibilityOff();
    actorS->VisibilityOff();
    actorC->VisibilityOff();
    actorH->VisibilityOff();
    actorV->VisibilityOff();
    actorCg->VisibilityOn();

    // 设置背景颜色
    rendererB->SetBackground(0.1, 0.2, 0.4); // 深蓝色背景

    // // 设置光照效果
    // lightKitB = vtkSmartPointer<vtkLightKit>::New();
    // lightKitB->AddLightsToRenderer(rendererB);

    // 获取 QVTKOpenGLNativeWidget 提供的交互器
    vtkRenderWindowInteractor* interactor = this->wingDisplayWidgetB->interactor();

    // 创建并设置交互样式
    vtkSmartPointer<vtkInteractorStyleTrackballCamera> style = vtkSmartPointer<vtkInteractorStyleTrackballCamera>::New();
    interactor->SetInteractorStyle(style);

    // 创建坐标轴
    vtkSmartPointer<vtkAxesActor> axes = vtkSmartPointer<vtkAxesActor>::New();
    axes->SetTotalLength(2.0, 2.0, 2.0); // 设置坐标轴的长度
    axes->SetShaftType(0);
    axes->SetCylinderRadius(0.05);

    // 创建 OrientationMarkerWidget
    orientationMarkerB = vtkSmartPointer<vtkOrientationMarkerWidget>::New();
    orientationMarkerB->SetOrientationMarker(axes);
    orientationMarkerB->SetInteractor(interactor);
    orientationMarkerB->SetViewport(0.8, 0.0, 1.0, 0.2); // 设置坐标轴的位置和大小
    orientationMarkerB->SetEnabled(1);
    orientationMarkerB->InteractiveOff(); // 禁止交互



    // 渲染并启动交互
    renwinB->Render();
}

void wingDisplay::updateWingList(QVector<wingDefinition>&wing){
    wingIndex = 0;



    if(!wingDataArray.isEmpty()){
        for(int i = 0;i<wingResultSeriesA.length();i++){
            chartWA->removeSeries(wingResultSeriesA[i]);
            chartWB->removeSeries(wingResultSeriesB[i]);
            chartWC->removeSeries(wingResultSeriesC[i]);
            chartWD->removeSeries(wingResultSeriesD[i]);
        }

        for (QTreeWidgetItem* item : wingTreeItemArray) {
             delete item; // 删除指针指向的 QTreeWidgetItem 对象
        }
        for(wingVLM* solver:VLMSolverArray){
            delete solver;
        }
        for(int i = 0;i<wingResultSeriesA.length();i++){
            delete wingResultSeriesA[i];
            delete wingResultSeriesB[i];
            delete wingResultSeriesC[i];
            delete wingResultSeriesD[i];
        }
        wingTreeItemArray.clear(); // 清空 QVector
        wingResultSeriesA.clear();
        wingResultSeriesB.clear();
        wingResultSeriesC.clear();
        wingResultSeriesD.clear();
        resultArray.clear();
        wingSettingArray.clear();
        optRealSettingArray.clear();
        optSettingArray.clear();
        VLMSolverArray.clear();
        colorArray.clear();
        wingListCombox->clear();
        wingDataArray.clear();
    }


    for(int i = 0;i<wing.length();i++){
        wing[i].airfoilInputType = true;
        wing[i].uMeshType = 1;
        wing[i].vMeshType = 1;
        for(int j = 0;j<wing[i].chordLengthW.length();j++){
            int index = getAirfoilIndex(wing[i].airfoilNameArray[j]);
            wing[i].airfoilArray.append(airfoilArray[index]);
            wing[i].oldAirfoilArray.append(airfoilArray[index]);
        }
    }


    for(int i = 0;i<wing.length();i++){

        addWingData(wing[i]);
    }

}
void wingDisplay::updateTailList(QVector<wingDefinition>&wing){
    tailIndex = 0;
    if(!tailDataArray.isEmpty()){
        for (QTreeWidgetItem* item : tailTreeItemArray) {
             delete item; // 删除指针指向的 QTreeWidgetItem 对象
        }
        tailTreeItemArray.clear();
        tailDataArray.clear();
    }


    for(int i = 0;i<wing.length();i++){
        wing[i].airfoilInputType = true;
        wing[i].uMeshType = 1;
        wing[i].vMeshType = 1;
        for(int j = 0;j<wing[i].chordLengthW.length();j++){
            int index = getAirfoilIndex(wing[i].airfoilNameArray[j]);
            wing[i].airfoilArray.append(airfoilArray[index]);
            wing[i].oldAirfoilArray.append(airfoilArray[index]);
        }
    }

    for(int i = 0;i<wing.length();i++){

        addTailData(wing[i]);
    }

}
void wingDisplay::updateListData(){

    emit emitList(wingDataArray,tailDataArray);
}
void wingDisplay::updateAxes(PlotWidget *chart, const QVector<PlotSeries*> &seriesList) {
    if (seriesList.isEmpty()) return;

    // 初始化数据范围，设置为一个极端值以便于后续比较
    qreal minX = std::numeric_limits<qreal>::max();
    qreal maxX = std::numeric_limits<qreal>::lowest();
    qreal minY = std::numeric_limits<qreal>::max();
    qreal maxY = std::numeric_limits<qreal>::lowest();

    // 遍历所有曲线数据，计算总体范围
    for (PlotSeries *series : seriesList) {
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
    QVector<PlotAxis*> axesX = chart->axes(Qt::Horizontal);
    QVector<PlotAxis*> axesY = chart->axes(Qt::Vertical);

    if (!axesX.isEmpty() && !axesY.isEmpty()) {
        PlotAxis *axisX = axesX.first();
        PlotAxis *axisY = axesY.first();

        if (axisX && axisY) {
            axisX->setRange(minX, maxX);
            axisY->setRange(minY, maxY);
        }
    }
}

void wingDisplay::updateWingDefineWidget(const wingDefinition &wingData) {
    wingVLM ned;

    ned.initialGeometry(wingData);
    actorC->VisibilityOff();
    actorB->VisibilityOn();

    if (ned.checkGeometry()) {
        pointsB->Reset();
        linesB->Reset();
        colorB->Reset();

        // 插入网格点
        for (int i = 0; i < ned.getMeshNum() * 2; i++) {
            pointsB->InsertNextPoint(ned.meshA[i].x, ned.meshA[i].y, ned.meshA[i].z);
            pointsB->InsertNextPoint(ned.meshB[i].x, ned.meshB[i].y, ned.meshB[i].z);
            pointsB->InsertNextPoint(ned.meshC[i].x, ned.meshC[i].y, ned.meshC[i].z);
            pointsB->InsertNextPoint(ned.meshD[i].x, ned.meshD[i].y, ned.meshD[i].z);
        }



        unsigned char white[3] = {200, 200, 200};
        for (vtkIdType i = 0; i < pointsB->GetNumberOfPoints() - 3; i += 4) {

            vtkSmartPointer<vtkQuad> quad = vtkSmartPointer<vtkQuad>::New();
            quad->GetPointIds()->SetId(0, i);
            quad->GetPointIds()->SetId(1, i + 1);
            quad->GetPointIds()->SetId(2, i + 2);
            quad->GetPointIds()->SetId(3, i + 3);
            linesB->InsertNextCell(quad);                // 添加颜色

            colorB->InsertNextTypedTuple(white);

        }
        profilePolyDataB->SetPoints(pointsB);
        profilePolyDataB->SetPolys(linesB);
        profilePolyDataB->GetCellData()->SetScalars(colorB);


        renwinB->Render();
    }
    //CreateTranslatedArray(rendererB,profilePolyDataB);


}
void wingDisplay::CreateTranslatedArray(vtkSmartPointer<vtkRenderer> ren, vtkSmartPointer<vtkPolyData> inputPolyData) {
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
    vtkSmartPointer<vtkPolyDataMapper> translatedMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
    translatedMapper->SetInputData(transformedData);

    vtkSmartPointer<vtkActor> translatedActor = vtkSmartPointer<vtkActor>::New();
    translatedActor->SetMapper(translatedMapper);

    vtkSmartPointer<vtkProperty> property = vtkSmartPointer<vtkProperty>::New();
    property->SetColor(1.0, 0.0, 0.0); // 可选：设置颜色或其他属性
    translatedActor->SetProperty(property);

    // 将演员添加到渲染器
    ren->AddActor(translatedActor);

    // 如果需要，可以在这里清除旧的演员或进行其他处理
}

void wingDisplay::updateStreamLineView(const wingVLM *ned){
    pointsS->Reset();
    linesS->Reset();
    actorS->VisibilityOn();

    // 用于记录所有点的全局索引
    vtkIdType globalPointIndex = 0;

    // 遍历每条线段
    for (int i = 0; i < ned->streamLineArray.length(); i++) {
        const QVector<point3d>& linePoints = ned->streamLineArray[i];

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

    wingDisplayWidgetB->renderWindow()->Render();
    //renwinB->Render();
}
void wingDisplay::updatePressureContourView(const wingVLM *ned){


    int index = wingAlphaCombox->currentIndex();
    actorC->VisibilityOn();
    actorB->VisibilityOff();
    pointsC->Reset();
    linesC->Reset();
    colorC->Reset();



    for (int i = 0; i < ned->getMeshNum(); i++) {
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
    colorC->SetNumberOfComponents(3);
    colorC->SetNumberOfTuples(profilePolyDataC->GetNumberOfCells());

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

        colorC->InsertTypedTuple(i, rgb);
    }

    // 将颜色数组添加到polyData









    wingDisplayWidgetB->renderWindow()->Render();


}
void wingDisplay::updatePressureContourView(QVector<point3d>a,QVector<point3d>b,QVector<point3d>c,QVector<point3d>d,
const QVector<double>&contourArray,const int num){
    actorC->VisibilityOn();
    actorB->VisibilityOff();
    pointsC->Reset();
    linesC->Reset();
    colorC->Reset();



    for (int i = 0; i < num; i++) {
        pointsC->InsertNextPoint(a[i].x, a[i].y, a[i].z);
        pointsC->InsertNextPoint(b[i].x, b[i].y, b[i].z);
        pointsC->InsertNextPoint(c[i].x, c[i].y, c[i].z);
        pointsC->InsertNextPoint(d[i].x, d[i].y, d[i].z);

    }
    for (vtkIdType i = 0; i < pointsC->GetNumberOfPoints() - 3; i += 4) {
        vtkSmartPointer<vtkQuad> quad = vtkSmartPointer<vtkQuad>::New();
        quad->GetPointIds()->SetId(0, i);
        quad->GetPointIds()->SetId(1, i + 1);
        quad->GetPointIds()->SetId(2, i + 2);
        quad->GetPointIds()->SetId(3, i + 3);
        linesC->InsertNextCell(quad);
    }






    int blue[3] = {0, 0, 255};    // 蓝色 (0, 0, 255)
    int yellow[3] = {255, 255, 0}; // 黄色 (255, 255, 0)
    int red[3] = {255, 0, 0};
    // 设置每个四边形的颜色
    for (vtkIdType i = 0; i < profilePolyDataC->GetNumberOfCells(); i++) {
        unsigned char rgb[3];
        double value = contourArray[i];

        if (value <= 0.5) {
            // 计算从蓝色到黄色的插值
            double ratio = value / 0.5;
            rgb[0] = (1 - ratio) * blue[0] + ratio * yellow[0];
            rgb[1] = (1 - ratio) * blue[1] + ratio * yellow[1];
            rgb[2] = (1 - ratio) * blue[2] + ratio * yellow[2];
        } else {
            // 计算从黄色到红色的插值
            double ratio = (value - 0.5) / 0.5;
            rgb[0] = (1 - ratio) * yellow[0] + ratio * red[0];
            rgb[1] = (1 - ratio) * yellow[1] + ratio * red[1];
            rgb[2] = (1 - ratio) * yellow[2] + ratio * red[2];
        }




        // red = value;
        // green = 0;
        // blue = 1 - value;



        // // 确保颜色分量的值在0到1之间
        // red = std::max(0.0, std::min(1.0, red));
        // green = std::max(0.0, std::min(1.0, green));
        // blue = std::max(0.0, std::min(1.0, blue));
        // rgb[0] = static_cast<unsigned char>(red * 255);
        // rgb[1] = static_cast<unsigned char>(green * 255);
        // rgb[2] = static_cast<unsigned char>(blue * 255);

        colorC->InsertNextTypedTuple(rgb);
    }



    // 将颜色数组添加到polyData



    wingDisplayWidgetB->renderWindow()->Render();


}
void wingDisplay::showWingDefineWidget(){
    saveType = WING_WING;
    isModify = false;
    outputWingButton->setEnabled(false);
    wingNameEdit->setText("我的机翼");
    wingDefineWidget->show();
}
void wingDisplay::hidePressureContourView(){
    actorC->VisibilityOff();
    actorB->VisibilityOn();
    wingDisplayWidgetB->renderWindow()->Render();
}
void wingDisplay::hideStreamLineView(){

    VLMSolverArray[wingChoiceIndex]->isShowStreamLine = false;
    actorS->VisibilityOff();
    wingDisplayWidgetB->renderWindow()->Render();
}
void wingDisplay::hideSeriesStyleDialog(){
    seriesStyleDialog->hide();
}
void wingDisplay::updateWingData(){

    int gridvType = meshVTypeCombobox->currentIndex();
    double griduType = double(meshUTypeCombobox->currentIndex()) / 20.0 + 1.0;
    double yteValue = yteValueEdit->text().toDouble() / 1000;




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
    for(int i = 0;i<profileDihedralAngleEdit.length();i++){
        double tmp = profileDihedralAngleEdit[i]->text().toDouble();
        e.append(tmp);
    }
    for(int i = 0;i<profileXGridEdit.length();i++){
        int tmp = profileXGridEdit[i]->text().toInt();
        xnum.append(tmp);
    }
    for(int i = 0;i<profileYGridEdit.length();i++){
        int tmp = profileYGridEdit[i]->text().toInt();
        ynum.append(tmp);
    }


    wingDefinition newWing(a,b,c,d,e,xnum,ynum);




    int tmp = yteTypeCombobox->currentIndex();
    if(tmp)
        newWing.yteType = true;
    else
        newWing.yteType = false;


    if(saveType == WING_VTAIL)
        newWing.isSymmetry = false;
    else
        newWing.isSymmetry = true;


    newWing.ctYte = yteValue;
    newWing.vMeshType = gridvType;
    newWing.uMeshType = griduType;

    newWing.computeWingMessage();


    areaTextLabel->setText(QString::number(newWing.Area(),'f', 3) + "㎡");
    realChordTextLabel->setText(QString::number(newWing.RealChord(),'f', 3) + "m");
    realSpanTextLabel->setText(QString::number(newWing.Span(),'f', 3) + "m");
    tipRatioTextLabel->setText(QString::number(newWing.TipRatio(),'f', 3));
    aspectRatioTextLabel->setText(QString::number(newWing.AspectRatio(),'f', 3));
    meshNumTextLabel->setText(QString::number(newWing.MeshNum()));


    //newWing.airfoilArray = airfoilArray;

    if(!airfoilArray.isEmpty() && !AirfoilNameArray.isEmpty()){

        for(int i = 0;i<profileAirfoilChoiceCombobox.length();i++){
            int index = profileAirfoilChoiceCombobox[i]->currentIndex();
            newWing.airfoilArray.append(airfoilArray[index]);
            newWing.oldAirfoilArray.append(airfoilArray[index]);
        }
    }


    updateView(newWing);

}
void wingDisplay::defineWingTypes(){
    wingTypeIndex = wingTypeCombobox->currentIndex();


    switch (wingTypeIndex) {
    case 0:

        generateSimpleWing();
        updateWingData();


        break;
    case 1:
        //显示机翼分段数窗口进行下一步生成步骤
        wingDefineDialogB->show();

        break;
    case 2:
        wingDefineDialogB->show();

        break;

    }
}

void wingDisplay::generateSimpleWing(){

    QVector<double>a;
    QVector<double>b;
    QVector<double>c;
    QVector<double>d;
    QVector<double>e;
    QVector<int>xnum;
    QVector<int>ynum;



    removeWidget();
    for(int i = 0;i<2;i++){

        QLabel *labelA = new QLabel(wingDefineWidget);
        QLineEdit *editA1 = new QLineEdit(wingDefineWidget);
        QLineEdit *editA2 = new QLineEdit(wingDefineWidget);
        QLineEdit *editA3 = new QLineEdit(wingDefineWidget);
        QLineEdit *editA4 = new QLineEdit(wingDefineWidget);
        QLineEdit *editA5 = new QLineEdit(wingDefineWidget);
        QLineEdit *editA6 = new QLineEdit(wingDefineWidget);
        QLineEdit *editA7 = new QLineEdit(wingDefineWidget);
        QComboBox *comboboxA = new QComboBox(wingDefineWidget);
        for(int availableAirfoilIndex = 0;availableAirfoilIndex<AirfoilNameArray.length();availableAirfoilIndex++){
            comboboxA->addItem(AirfoilNameArray[availableAirfoilIndex]);
        }
        comboboxA->setCurrentIndex(0);




        connect(editA1,&QLineEdit::editingFinished,this,&wingDisplay::updateWingData);
        connect(editA2,&QLineEdit::editingFinished,this,&wingDisplay::updateWingData);
        connect(editA3,&QLineEdit::editingFinished,this,&wingDisplay::updateWingData);
        connect(editA4,&QLineEdit::editingFinished,this,&wingDisplay::updateWingData);
        connect(editA5,&QLineEdit::editingFinished,this,&wingDisplay::updateWingData);
        connect(editA6,&QLineEdit::editingFinished,this,&wingDisplay::updateWingData);
        connect(editA7,&QLineEdit::editingFinished,this,&wingDisplay::updateWingData);
        //connect(comboboxA,&QComboBox::currentIndexChanged,this,&wingDisplay::updateWingData);

        wingDefineLayout->addWidget(labelA,i + 1,0,1,1);
        wingDefineLayout->addWidget(editA1,i + 1,1,1,1);
        wingDefineLayout->addWidget(editA2,i + 1,2,1,1);
        wingDefineLayout->addWidget(editA3,i + 1,3,1,1);
        wingDefineLayout->addWidget(editA4,i + 1,4,1,1);
        wingDefineLayout->addWidget(editA5,i + 1,5,1,1);
        wingDefineLayout->addWidget(editA6,i + 1,6,1,1);
        wingDefineLayout->addWidget(editA7,i + 1,7,1,1);
        wingDefineLayout->addWidget(comboboxA,i + 1,8,1,1);






        profileLabel.append(labelA);
        profileSpanEdit.append(editA1);
        profilechordLengthEdit.append(editA2);
        profileOffsetLengthEdit.append(editA3);
        profileTwistAngleEdit.append(editA4);
        profileDihedralAngleEdit.append(editA5);
        profileXGridEdit.append(editA6);
        profileYGridEdit.append(editA7);
        profileAirfoilChoiceCombobox.append(comboboxA);

    }
    wingDefineBox->setFixedHeight(150);

    for(int i = 0;i<2;i++){
        a.append(i * 1500);
        b.append(300);
        c.append(0);
        d.append(0);
        e.append(0);
        xnum.append(10);
        ynum.append(10);
        profileLabel[i]->setText("截面(" + QString::number(i + 1) + ")");
        profileSpanEdit[i]->setText(QString::number(a[i]));
        profilechordLengthEdit[i]->setText(QString::number(b[i]));
        profileOffsetLengthEdit[i]->setText(QString::number(c[i]));
        profileTwistAngleEdit[i]->setText(QString::number(d[i]));
        profileDihedralAngleEdit[i]->setText(QString::number(e[i]));
        profileXGridEdit[i]->setText(QString::number(xnum[i]));
        profileYGridEdit[i]->setText(QString::number(ynum[i]));

    }

}
void wingDisplay::generateMultiWing(){
    removeWidget();
    updateWingDesignerModel();

    if(SpanB <= 0)
        SpanB = 1;
    if(AreaB <= 0)
        AreaB = 0.5;
    if(AspectRatioB <= 0)
        AspectRatioB = 0.4;
    if(SpanNumB <= 1)
        SpanNumB = 3;


    double cr = AreaB * 2 / SpanB / (1 + AspectRatioB) * 1000;
    double ct = cr * AspectRatioB;
    double realSpan = SpanB * 1000 / 2;



    for(int i = 0;i<SpanNumB;i++){
        double x = realSpan / (SpanNumB - 1) * i;
        double y = cr - (cr - ct) / (SpanNumB - 1) * i;
        double a = x;
        double b = y;
        double c = ((cr - y) / 4);
        double d = 0;
        double e = 0;
        double xnum = 10;
        double ynum = 10;
        profileLabel[i]->setText("截面(" + QString::number(i + 1) + ")");
        profileSpanEdit[i]->setText(QString::number(a));
        profilechordLengthEdit[i]->setText(QString::number(b));
        profileOffsetLengthEdit[i]->setText(QString::number(c));
        profileTwistAngleEdit[i]->setText(QString::number(d));
        profileDihedralAngleEdit[i]->setText(QString::number(e));
        profileXGridEdit[i]->setText(QString::number(xnum));
        profileYGridEdit[i]->setText(QString::number(ynum));
        }


}
void wingDisplay::generateEllipticalWing(){
    removeWidget();
    updateWingDesignerModel();

    QVector<double>a;
    QVector<double>b;
    QVector<double>c;
    QVector<double>d;
    QVector<double>e;
    QVector<int>xnum;
    QVector<int>ynum;
    if(SpanB <= 0)
        SpanB = 1;
    if(AreaB <= 0)
        AreaB = 0.5;
    if(AspectRatioB <= 0)
        AspectRatioB = 0.4;
    if(SpanNumB <= 1)
        SpanNumB = 8;
    double cr = AreaB / SpanB * 2 / M_PI * 1000 * 2;
    double realSpan = SpanB * 1000 / 2;

    double tmp = 0;
    QVector<double>selectProbabilityA;

    selectProbabilityA.append(0);
    for(int i = 0; i < SpanNumB; i++){
        tmp = (1 - 1.5) / (1 - pow(1.5,SpanNumB)) * pow(1.5,(SpanNumB - i - 1)) + tmp;
        selectProbabilityA.append(tmp);
    }


    for(int i = 0;i<SpanNumB;i++){
        double x = realSpan * selectProbabilityA[i];
        double y = sqrt((1 - x * x / realSpan  / realSpan ) * cr * cr);
        a.append(x);
        b.append(y);
        c.append((cr - y) / 4);
        d.append(0);
        e.append(0);
        xnum.append(10);
        ynum.append(10);
        profileLabel[i]->setText("截面(" + QString::number(i + 1) + ")");
        profileSpanEdit[i]->setText(QString::number(a[i]));
        profilechordLengthEdit[i]->setText(QString::number(b[i]));
        profileOffsetLengthEdit[i]->setText(QString::number(c[i]));
        profileTwistAngleEdit[i]->setText(QString::number(d[i]));
        profileDihedralAngleEdit[i]->setText(QString::number(e[i]));
        profileXGridEdit[i]->setText(QString::number(xnum[i]));
        profileYGridEdit[i]->setText(QString::number(ynum[i]));
    }
}
void wingDisplay::updateWingEdit(const int index){
    removeWidget();

    updateWingDesignerModel();
    QVector<wingDefinition>tmp;
    switch(saveType){
    case WING_WING:
        tmp = wingDataArray;
        break;
    case WING_HTAIL:

        tmp = tailDataArray;
        break;
    case WING_VTAIL:

        tmp = tailDataArray;
        break;

    }

    for(int i = 0;i<tmp[index].chordLengthW.length();i++){
        double spanVal = tmp[index].spanW[i] * 1000;
        double chordVal = tmp[index].chordLengthW[i] * 1000;
        double offsetVal = tmp[index].offsetLengthW[i] * 1000;
        double twistVal = tmp[index].twistAngleW[i];
        double dihedVal = tmp[index].dihedralAngleW[i];
        int xVal = tmp[index].gridV[i];
        int yVal = tmp[index].gridU[i];

        profileAirfoilChoiceCombobox[i]->blockSignals(true);
        int indexTmp = getAirfoilIndex(tmp[index].airfoilNameArray[i]);
        profileAirfoilChoiceCombobox[i]->setCurrentIndex(indexTmp);
        profileAirfoilChoiceCombobox[i]->blockSignals(false);


        profileLabel[i]->setText("截面(" + QString::number(i + 1) + ")");
        profileSpanEdit[i]->setText(QString::number(spanVal));
        profilechordLengthEdit[i]->setText(QString::number(chordVal));
        profileOffsetLengthEdit[i]->setText(QString::number(offsetVal));
        profileTwistAngleEdit[i]->setText(QString::number(twistVal));
        profileDihedralAngleEdit[i]->setText(QString::number(dihedVal));
        profileXGridEdit[i]->setText(QString::number(xVal));
        profileYGridEdit[i]->setText(QString::number(yVal));
    }
}

void wingDisplay::generateWing(){


    SpanB = SpanEditB->text().toDouble();
    SpanNumB = SpanNumEditB->text().toInt();
    AreaB = AreaEditB->text().toDouble();
    AspectRatioB = AspectRatioEditB->text().toDouble();


    switch (wingTypeIndex) {
    case 0:


        break;
    case 1:
        wingDefineDialogB->close();
        generateMultiWing();
        updateWingData();
        break;
    case 2:
        wingDefineDialogB->close();
        generateEllipticalWing();
        updateWingData();
        break;
    default:
        break;
    }
}

void wingDisplay::removeWidget(){



    for (int i = profileSpanEdit.length() - 1; i >= 0; --i) {

        wingDefineLayout->removeWidget(profileLabel[i]);
        delete profileLabel[i];
        profileLabel.remove(i);


        wingDefineLayout->removeWidget(profileSpanEdit[i]);
        delete profileSpanEdit[i];
        profileSpanEdit.remove(i);

        wingDefineLayout->removeWidget(profilechordLengthEdit[i]);
        delete profilechordLengthEdit[i];
        profilechordLengthEdit.remove(i);

        wingDefineLayout->removeWidget(profileOffsetLengthEdit[i]);
        delete profileOffsetLengthEdit[i];
        profileOffsetLengthEdit.remove(i);


        wingDefineLayout->removeWidget(profileTwistAngleEdit[i]);
        delete profileTwistAngleEdit[i];
        profileTwistAngleEdit.remove(i);


        wingDefineLayout->removeWidget(profileDihedralAngleEdit[i]);
        delete profileDihedralAngleEdit[i];
        profileDihedralAngleEdit.remove(i);

        wingDefineLayout->removeWidget(profileXGridEdit[i]);
        delete profileXGridEdit[i];
        profileXGridEdit.remove(i);

        wingDefineLayout->removeWidget(profileYGridEdit[i]);
        delete profileYGridEdit[i];
        profileYGridEdit.remove(i);

        wingDefineLayout->removeWidget(profileAirfoilChoiceCombobox[i]);
        delete profileAirfoilChoiceCombobox[i];
        profileAirfoilChoiceCombobox.remove(i);

    }
}
void wingDisplay::changeAlphaView(const int index){
    switch (modelType) {
    case WING_DESIGN:

        updateGLTextA(wingDataToString(wingDataArray[wingChoiceIndex]));//刷新模型参数
        updatePressureContourView(VLMSolverArray[wingChoiceIndex]);
        if(index >= 0 && VLMSolverArray[wingChoiceIndex]->isShowStreamLine){
            double a1 = streamEditArray[0]->text().toDouble();
            double a2 = streamEditArray[1]->text().toDouble();
            double a3 = streamEditArray[2]->text().toDouble();
            double a4 = streamEditArray[3]->text().toDouble();
            startCalculation();
            VLMSolverArray[wingChoiceIndex]->solveStreamLine(index,a1,a2,a3,a4);
            updateStreamLineView(VLMSolverArray[wingChoiceIndex]);
        }
        /*弹性铰链（需删除）
         *
         * */
        //updateWingTip(index);


        QApplication::processEvents(); // 确保界面更新

        break;
    case WING_RESULTA:

        break;
    case WING_RESULTB:
        drawSpanResult(wingChoiceIndex);
        break;
    default:
        break;
    }
}
void wingDisplay::initialWingDefineDialog(){
    wingDefineDialogB = new QDialog();
    wingDefineLayoutB = new QGridLayout(wingDefineDialogB);
    SpanLabelB = new QLabel(wingDefineDialogB);
    SpanNumLabelB = new QLabel(wingDefineDialogB);
    AreaLabelB = new QLabel(wingDefineDialogB);
    SpanEditB = new QLineEdit(wingDefineDialogB);
    SpanNumEditB = new QLineEdit(wingDefineDialogB);
    AreaEditB = new QLineEdit(wingDefineDialogB);
    saveButtonB = new QPushButton(wingDefineDialogB);
    cancelButtonB = new QPushButton(wingDefineDialogB);
    AspectRatioLabelB = new QLabel(wingDefineDialogB);
    AspectRatioEditB = new QLineEdit(wingDefineDialogB);



    SpanLabelB->setText("翼展");
    SpanNumLabelB->setText("机翼分段数");
    AreaLabelB->setText("翼面积");
    AspectRatioLabelB->setText("稍根比");
    saveButtonB->setText("保存");


    SpanEditB->setText("2");
    SpanNumEditB->setText("3");
    AreaEditB->setText("0.5");
    AspectRatioEditB->setText("0.4");
    cancelButtonB->setText("取消");

    wingDefineLayoutB->addWidget(SpanLabelB,0,0,1,1);
    wingDefineLayoutB->addWidget(SpanEditB,0,1,1,1);
    wingDefineLayoutB->addWidget(SpanNumLabelB,1,0,1,1);
    wingDefineLayoutB->addWidget(SpanNumEditB,1,1,1,1);
    wingDefineLayoutB->addWidget(AreaLabelB,2,0,1,1);
    wingDefineLayoutB->addWidget(AreaEditB,2,1,1,1);
    wingDefineLayoutB->addWidget(AspectRatioLabelB,3,0,1,1);
    wingDefineLayoutB->addWidget(AspectRatioEditB,3,1,1,1);
    wingDefineLayoutB->addWidget(saveButtonB,4,0,1,1);
    wingDefineLayoutB->addWidget(cancelButtonB,4,1,1,1);

    connect(saveButtonB,&QPushButton::clicked,this,&wingDisplay::generateWing);
    connect(cancelButtonB,&QPushButton::clicked,wingDefineDialogB,&QDialog::close);

    wingDefineDialogB->hide();

}
void wingDisplay::initialWingMessageBox(){




    areaLabel = new QLabel(wingDefineWidget);
    realChordLabel = new QLabel(wingDefineWidget);
    realSpanLabel = new QLabel(wingDefineWidget);
    tipRatioLabel = new QLabel(wingDefineWidget);
    aspectRatioLabel = new QLabel(wingDefineWidget);
    meshNumLabel = new QLabel(wingDefineWidget);
    meshVTypeLabel = new QLabel(wingDefineWidget);
    meshUTypeLabel = new QLabel(wingDefineWidget);
    yteTypeLabel = new QLabel(wingDefineWidget);
    yteValueLabel = new QLabel(wingDefineWidget);
    saveWingButton = new QPushButton(wingDefineWidget);


    areaTextLabel = new QLabel(wingDefineWidget);
    realChordTextLabel = new QLabel(wingDefineWidget);
    realSpanTextLabel = new QLabel(wingDefineWidget);
    tipRatioTextLabel = new QLabel(wingDefineWidget);
    aspectRatioTextLabel = new QLabel(wingDefineWidget);
    meshNumTextLabel = new QLabel(wingDefineWidget);
    meshVTypeCombobox = new QComboBox(wingDefineWidget);
    meshUTypeCombobox = new QComboBox(wingDefineWidget);
    yteTypeCombobox = new QComboBox(wingDefineWidget);
    yteValueEdit = new QLineEdit(wingDefineWidget);
    outputWingButton = new QPushButton(wingDefineWidget);


    verticalSpacerB = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);



    wingNameLabel = new QLabel(wingDefineWidget);
    wingNameEdit = new QLineEdit(wingDefineWidget);

    areaLabel->setText("翼面积");
    realChordLabel->setText("平均气动弦长");
    realSpanLabel->setText("翼展");
    tipRatioLabel->setText("稍根比");
    aspectRatioLabel->setText("展弦比");
    meshNumLabel->setText("网格数");
    meshVTypeLabel->setText("弦向网格生成方式");
    meshUTypeLabel->setText("展向网格生成方式");
    yteTypeLabel->setText("尾缘厚度分布方式");
    yteValueLabel->setText("翼根尾缘厚度(mm)");



    areaTextLabel->setText("");
    realChordTextLabel->setText("");
    realSpanTextLabel->setText("");
    tipRatioTextLabel->setText("");
    aspectRatioTextLabel->setText("");
    meshNumTextLabel->setText("");
    meshVTypeCombobox->addItem("平均分布");
    meshVTypeCombobox->addItem("余弦分布");

    meshUTypeCombobox->addItem("平均分布");
    meshUTypeCombobox->addItem("比例分布A");
    meshUTypeCombobox->addItem("比例分布B");
    meshUTypeCombobox->addItem("比例分布C");

    yteTypeCombobox->addItem("等厚度分布");
    yteTypeCombobox->addItem("按弦长比例分布");




    meshVTypeCombobox->setCurrentIndex(0);
    meshUTypeCombobox->setCurrentIndex(0);
    yteValueEdit->setText("0");

    wingNameLabel->setText("机翼名称");
    wingNameEdit->setText("我的机翼");

    saveWingButton->setText("保存");
    outputWingButton->setText("输出Catia文件");








    wingMessageLayout->addWidget(areaLabel,0,0,1,1);
    wingMessageLayout->addWidget(realChordLabel,1,0,1,1);
    wingMessageLayout->addWidget(realSpanLabel,2,0,1,1);
    wingMessageLayout->addWidget(tipRatioLabel,3,0,1,1);
    wingMessageLayout->addWidget(aspectRatioLabel,4,0,1,1);
    wingMessageLayout->addWidget(meshNumLabel,5,0,1,1);
    wingMessageLayout->addWidget(meshVTypeLabel,7,0,1,1);
    wingMessageLayout->addWidget(meshUTypeLabel,8,0,1,1);
    wingMessageLayout->addWidget(yteTypeLabel,9,0,1,1);
    wingMessageLayout->addWidget(yteValueLabel,10,0,1,1);
    wingMessageLayout->addWidget(wingNameLabel,11,0,1,1);
    wingMessageLayout->addWidget(saveWingButton,12,0,1,1);

    wingMessageLayout->addWidget(areaTextLabel,0,1,1,1);
    wingMessageLayout->addWidget(realChordTextLabel,1,1,1,1);
    wingMessageLayout->addWidget(realSpanTextLabel,2,1,1,1);
    wingMessageLayout->addWidget(tipRatioTextLabel,3,1,1,1);
    wingMessageLayout->addWidget(aspectRatioTextLabel,4,1,1,1);
    wingMessageLayout->addWidget(meshNumTextLabel,5,1,1,1);
    wingMessageLayout->addWidget(meshVTypeCombobox,7,1,1,1);
    wingMessageLayout->addWidget(meshUTypeCombobox,8,1,1,1);
    wingMessageLayout->addWidget(yteTypeCombobox,9,1,1,1);
    wingMessageLayout->addWidget(yteValueEdit,10,1,1,1);
     wingMessageLayout->addWidget(wingNameEdit,11,1,1,1);
     wingMessageLayout->addWidget(outputWingButton,12,1,1,1);







    wingMessageLayout->addItem(verticalSpacerB, 6, 0, 1, 1); // 在第二行第一列添加一个占位符


    connect(meshVTypeCombobox, QOverload<int>::of(&QComboBox::activated),
            this, &wingDisplay::updateWingData);
    connect(meshUTypeCombobox, QOverload<int>::of(&QComboBox::activated),
            this, &wingDisplay::updateWingData);
    connect(yteTypeCombobox, QOverload<int>::of(&QComboBox::activated),
            this, &wingDisplay::updateWingData);
    connect(yteValueEdit, &QLineEdit::editingFinished,this, &wingDisplay::updateWingData);
    //connect(saveWingButton,&QPushButton::clicked,this,&wingDisplay::saveWingData);

}
void wingDisplay::updateWingDesignerModel(){


    for(int i = 0;i<SpanNumB;i++){


        QLabel *labelA = new QLabel(wingDefineWidget);
        QLineEdit *editA1 = new QLineEdit(wingDefineWidget);
        QLineEdit *editA2 = new QLineEdit(wingDefineWidget);
        QLineEdit *editA3 = new QLineEdit(wingDefineWidget);
        QLineEdit *editA4 = new QLineEdit(wingDefineWidget);
        QLineEdit *editA5 = new QLineEdit(wingDefineWidget);
        QLineEdit *editA6 = new QLineEdit(wingDefineWidget);
        QLineEdit *editA7 = new QLineEdit(wingDefineWidget);
        QComboBox *comboboxA = new QComboBox(wingDefineWidget);

        if(!airfoilArray.isEmpty() && !AirfoilNameArray.isEmpty()){
            for(int j = 0;j<AirfoilNameArray.length();j++){
                comboboxA->addItem(AirfoilNameArray[j]);
            }
        }else{
            for(int j = 0;j<AirfoilNameArray.length();j++){
                comboboxA->addItem("平板");
            }
        }
        comboboxA->setCurrentIndex(0);


        wingDefineLayout->addWidget(labelA,i + 1,0,1,1);
        wingDefineLayout->addWidget(editA1,i + 1,1,1,1);
        wingDefineLayout->addWidget(editA2,i + 1,2,1,1);
        wingDefineLayout->addWidget(editA3,i + 1,3,1,1);
        wingDefineLayout->addWidget(editA4,i + 1,4,1,1);
        wingDefineLayout->addWidget(editA5,i + 1,5,1,1);
        wingDefineLayout->addWidget(editA6,i + 1,6,1,1);
        wingDefineLayout->addWidget(editA7,i + 1,7,1,1);
        wingDefineLayout->addWidget(comboboxA,i + 1,8,1,1);


        profileLabel.append(labelA);
        profileSpanEdit.append(editA1);
        profilechordLengthEdit.append(editA2);
        profileOffsetLengthEdit.append(editA3);
        profileTwistAngleEdit.append(editA4);
        profileDihedralAngleEdit.append(editA5);
        profileXGridEdit.append(editA6);
        profileYGridEdit.append(editA7);
        profileAirfoilChoiceCombobox.append(comboboxA);

        connect(editA1,&QLineEdit::editingFinished,this,&wingDisplay::updateWingData);
        connect(editA2,&QLineEdit::editingFinished,this,&wingDisplay::updateWingData);
        connect(editA3,&QLineEdit::editingFinished,this,&wingDisplay::updateWingData);
        connect(editA4,&QLineEdit::editingFinished,this,&wingDisplay::updateWingData);
        connect(editA5,&QLineEdit::editingFinished,this,&wingDisplay::updateWingData);
        connect(editA6,&QLineEdit::editingFinished,this,&wingDisplay::updateWingData);
        connect(editA7,&QLineEdit::editingFinished,this,&wingDisplay::updateWingData);
        connect(comboboxA,QOverload<const int>::of(&QComboBox::activated),this,&wingDisplay::updateWingData);


    }

    wingDefineBox->setFixedHeight(80 + SpanNumB * 30);


}
void wingDisplay::updateIterText(const std::vector<double>& value){



    QString tmp1 = "cl:" + QString::number(value[0],'f',3) + " ";
    QString tmp2 = "cd:" + QString::number(value[1],'f',4) + " ";
    QString tmp3 = "cm:" + QString::number(value[2],'f',4) + " ";
    QString tmp4 = "k:" + QString::number(value[3],'f',2) + " ";
    QString tmp5 = "p:" + QString::number(value[4],'f',2) + "\n";
    QString tmp = tmp1 + tmp2 + tmp3 + tmp4 + tmp5;
    if(iterTextArray.length() > 5)
        iterTextArray.removeAt(0);
    iterTextArray.append(tmp);
    QString sum = "正在迭代.....\n";
    for(int i = 0;i<iterTextArray.length();i++)
        sum += iterTextArray[i];
    updateGLTextA(sum);

}

wingDefinition wingDisplay::getWingDefine(){
    int gridvType = meshVTypeCombobox->currentIndex();
    double griduType = double(meshUTypeCombobox->currentIndex()) / 20.0 + 1.0;
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
    for(int i = 0;i<profileDihedralAngleEdit.length();i++){
        double tmp = profileDihedralAngleEdit[i]->text().toDouble();
        e.append(tmp);
    }
    for(int i = 0;i<profileXGridEdit.length();i++){
        int tmp = profileXGridEdit[i]->text().toInt();
        xnum.append(tmp);
    }
    for(int i = 0;i<profileYGridEdit.length();i++){
        int tmp = profileYGridEdit[i]->text().toInt();
        ynum.append(tmp);
    }


    wingDefinition newWing(a,b,c,d,e,xnum,ynum);
    newWing.vMeshType = gridvType;
    newWing.uMeshType = griduType;
    newWing.yteType = yteTypeCombobox->currentIndex();
    newWing.ctYte = yteValueEdit->text().toDouble() / 1000;

    if(saveType == WING_VTAIL)
        newWing.isSymmetry = false;
    else
        newWing.isSymmetry = true;






    if(!AirfoilNameArray.isEmpty()&&!airfoilArray.isEmpty()){

        newWing.cstNum = cstNumArray[profileAirfoilChoiceCombobox[0]->currentIndex()];//参数化数目是第一个翼型的阶数;

        for(int i = 0;i<profileAirfoilChoiceCombobox.length();i++){
            int index = profileAirfoilChoiceCombobox[i]->currentIndex();
            //newWing.airfoilMeanCamberCST.append(VLMCSTArray[index]);
            newWing.airfoilArray.append(airfoilArray[index]);
            newWing.oldAirfoilArray.append(airfoilArray[index]);
            newWing.airfoilNameArray.append(AirfoilNameArray[index]);
                //这里定义翼型
        }
    }else{
        for(int i = 0;i<profileAirfoilChoiceCombobox.length();i++){

            //newWing.airfoilMeanCamberCST.append(VLMCSTArray[index]);
            newWing.airfoilNameArray.append("无");
                //这里定义翼型
        }
    }


   // newWing.airfoilArray = airfoilArray;


    newWing.name = wingNameEdit->text();
    //qDebug()<<newWing.name;
    return newWing;

}
void wingDisplay::updateOptIterText(const double value){
    iterViewA->textItem->setPlainText(QString::number(value,'f',2));
    iterViewA->update();
}
void wingDisplay::updateOptimizationTextItem(const int x,const int /*y*/){
    //QRectF textRect = optimizationChartViewA->textItem->boundingRect();

    // 计算位置
    qreal xPosition = x - 100;
    qreal yPosition = 50;

    // Update the position of the text item
    iterViewA->textItem->setPos(xPosition, yPosition);

    iterViewA->update();  // Ensure the view is updated
}
void wingDisplay::updateGLTextA(QString text){

    std::string tmp = text.toStdString();
    textActorA->SetInput(tmp.c_str());
    textActorA->Modified();
    wingDisplayWidgetB->renderWindow()->Render();


}

void wingDisplay::updateOptUI(wingDefinition& newWingData){
    WingGaParameters sett;
    newWingData.computeWingMessage();

    QVector<double>minChord;QVector<double>minOffset;QVector<double>minTwist;QVector<double>minDihed;
    QVector<double>maxChord;QVector<double>maxOffset;QVector<double>maxTwist;QVector<double>maxDihed;
    QVector<bool>airfoilBool;QVector<bool>spanBool;QVector<bool>chordBool;QVector<bool>offsetBool;QVector<bool>twistBool;QVector<bool>dihedBool;
    QVector<QVector<double>>cstTmp;
    for(int i = 0;i<newWingData.chordLengthW.length();i++){

        cstTmp.append(cstPointY);
        minChord.append(0);
        maxChord.append(newWingData.chordLengthW[i] + 0.5);
        minOffset.append(newWingData.offsetLengthW[i] - 0.5);
        maxOffset.append(newWingData.offsetLengthW[i] + 0.5);
        minTwist.append(newWingData.twistAngleW[i] - 10);
        maxTwist.append(newWingData.twistAngleW[i] + 10);
        minDihed.append(0);
        maxDihed.append(90);

        airfoilBool.append(false);
        spanBool.append(false);
        chordBool.append(true);
        offsetBool.append(true);
        twistBool.append(true);
        dihedBool.append(false);
    }
    newWingData.cstPointYArray = cstTmp;



    sett.MinSpan = 0;
    sett.MaxSpan = newWingData.Span() + 2;
    sett.minChord = minChord;
    sett.maxChord = maxChord;
    sett.minOffsetLength = minOffset;
    sett.maxOffsetLength = maxOffset;
    sett.minTwisAngle = minTwist;
    sett.maxTwisAngle = maxTwist;
    sett.minDihedralAngle = minDihed;
    sett.maxDihedralAngle = maxDihed;
    sett.spanVal = 0.01;
    sett.chordVal = 0.01;
    sett.offsetLengthVal = 0.01;
    sett.twisAngleVal = 0.01;
    sett.dihedralAngleVal = 0.01;
    sett.airfoilChangeArray = airfoilBool;
    sett.spanChangeArray = spanBool;
    sett.chordChangeArray = chordBool;
    sett.offsetChangeArray = offsetBool;
    sett.twistChangeArray = twistBool;
    sett.dihedChangeArray = dihedBool;

    optSettingArray[wingChoiceIndex] = sett;
    optRealSettingArray[wingChoiceIndex] = sett;
    optRangeChange(wingChoiceIndex);//刷新模型优化参数

}
void wingDisplay::updateComboBox(QComboBox* comboBox, const QStringList& items) {
    // 暂时断开信号
    bool signalsBlocked = comboBox->blockSignals(true);

    // 清空QComboBox
    comboBox->clear();

    // 添加新项
    for (const QString& item : items) {
        comboBox->addItem(item);
    }

    // 恢复信号连接状态
    comboBox->blockSignals(signalsBlocked);
}
void wingDisplay::startVLMInThread(int step,int n){
    QVector<QFuture<void>>futures;
    QVector<wingVLM*>testSolve;
    //QFutureSynchronizer<void> sync;
    wingDefinition wingData;

    int index1;
    if (activeOptimizationWingIndex < 0 ||
        activeOptimizationWingIndex >= wingDataArray.size())
        return;

    double v = wingVelocitySetEdit->text().toDouble();
    double h = wingHeightSetEdit->text().toDouble();
    double f = wingLiftSetEdit->text().toDouble();
    if(v<= 0||h < 0||f == 0)
        return;

    VLMSetting set(0,10,1,20,h,0);

    for(int i = 0;i<wingThreadNum;i++){
        wingVLM *solvers = new wingVLM();
        solvers->setXfoilDrag(true);
        set.vinf = v;
        solvers->height = h;
        solvers->designLiftForce = f;
        testSolve.append(solvers);
        connect(solvers,&wingVLM::emitResultValue,this,&wingDisplay::updateIterText);

        solvers->refreshParaments(set);

        if (step == 0) {
            index1 = i + (optSetting.initialEliteNum / wingThreadNum - wingWorkNum + n) *
                             wingThreadNum;

            wingData = optModel->initialWingDataArray[index1];
            if (index1 == 0) { // 先计算一次初始
                solvers->initialGeometry(
                    wingDataArray[activeOptimizationWingIndex]);

            } else {

                solvers->initialGeometry(wingData);
            }
            optWingDataArray.append(wingData);
        }else{
            index1 = i + (optSetting.eliteNum / wingThreadNum - wingWorkNum + n) * wingThreadNum;
            wingData = optModel->wingChromosomeSequenceDec[index1];
            solvers->initialGeometry(wingData);
            optWingDataArray.append(wingData);

        }



        QFuture<void>future = QtConcurrent::run([solvers](){
            solvers->computeFixLiftDrag();
        });


        futures.append(future);


    }





    //for (auto &f : futures) {
        //sync.addFuture(f);
    //}

    //sync.waitForFinished();   // ✅ 等“所有线程”一起完成

    bool allFinished = false;
    while(!allFinished){
        allFinished = true;
        for(int i = 0; i< wingThreadNum;i++){
            if(!futures[i].isFinished()){
                allFinished = false;
                break;
            }
        }
        QCoreApplication::processEvents();
    }






    for(int i = 0;i<wingThreadNum;i++){
        double k = 0.0;
        if (std::isfinite(testSolve[i]->fixCl) &&
            std::isfinite(testSolve[i]->fixCd) &&
            std::abs(testSolve[i]->fixCd) >
                std::numeric_limits<double>::epsilon()) {
            k = testSolve[i]->fixCl / testSolve[i]->fixCd;
            if (!std::isfinite(k))
                k = 0.0;
        }
        cdArray.append(k);
        colorContourArray.append(testSolve[i]->contourArray);
        xABArray.append(testSolve[i]->xyA);
        xBCArray.append(testSolve[i]->xyB);
        xCDArray.append(testSolve[i]->xyC);
        xDAArray.append(testSolve[i]->xyD);
        xSpan.append(testSolve[i]->spanForceYt);
        xSpanLiftA.append(testSolve[i]->designSpanLift);
        xSpanLiftB.append(testSolve[i]->designSpanEllipseLift);

    }
    if(step == 0 && n == 0)
        updatePressureContourView(xABArray[0],xBCArray[0],xCDArray[0],xDAArray[0],colorContourArray[0],meshNum);



    qDeleteAll(testSolve);
    testSolve.clear();


}
int wingDisplay::getValueIndex(const double num){
    const double epsilon = 0.0001; // 设置一个小的误差范围

    // 判断 num 是否接近于 1.0
    if (std::fabs(num - 1.0) < epsilon) {
        return 2;
    }
    // 判断 num 是否接近于 0.1
    else if (std::fabs(num - 0.1) < epsilon) {
        return 1;
    }
    // 判断 num 是否接近于 0.01
    else if (std::fabs(num - 0.01) < epsilon) {
        return 0;
    }
    // 如果 num 与预期值都不接近，返回一个特殊值
    else {
        return 0;
    }
}
void wingDisplay::hideAllWidgetsInBox(QGroupBox *box) {
    // 检查frame是否为nullptr
    if (!box) return;

    // 获取frame的所有子对象
    const auto children = box->children();

    // 遍历子对象
    for (QObject *child : children) {
        QWidget *widget = qobject_cast<QWidget *>(child);
        if (widget) { // 如果child是QWidget的实例
            widget->hide(); // 隐藏控件
        }
    }
}
void wingDisplay::showAllWidgetsInBox(QGroupBox *box) {
    // 检查frame是否为nullptr
    if (!box) return;

    // 获取frame的所有子对象
    const auto children = box->children();

    // 遍历子对象
    for (QObject *child : children) {
        QWidget *widget = qobject_cast<QWidget *>(child);
        if (widget) { // 如果child是QWidget的实例
            widget->show(); // 隐藏控件
        }
    }
}
void wingDisplay::printfWingData(const wingDefinition&wingData){
    qDebug()<<"翼展"<<wingData.spanW;
    qDebug()<<"弦长"<<wingData.chordLengthW;
    qDebug()<<"偏移距离"<<wingData.offsetLengthW;
    qDebug()<<"扭转角"<<wingData.twistAngleW;
    qDebug()<<"上反角"<<wingData.dihedralAngleW;
    qDebug()<<"cst参数"<<wingData.cstArray;

}
void wingDisplay::optChangeVal(int index){
    QComboBox *combox = qobject_cast<QComboBox*>(sender());
    int ind = combox->property("combox").toInt();
    if(combox){
        changeSliderType(ind,index);
    }
}
void wingDisplay::optMinLabelChanged(int min){

    RangeSlider *slider = qobject_cast<RangeSlider*>(sender());
    if(slider){
        int index = slider->property("slider").toInt();
        double stringMin = min * optCombox[index]->currentText().toDouble() + minVal[index];
        optMinValLabel[index]->setText(QString::number(stringMin, 'f', 2)+ unitVal[index]);
        if(isChangeRealVal)
            saveGeometryOptSetting(index);
        slider = nullptr;

    }
}
void wingDisplay::optMaxLabelChanged(int max){

    RangeSlider *slider = qobject_cast<RangeSlider*>(sender());
    if(slider){
        int index = slider->property("slider").toInt();
        double stringMax = max * optCombox[index]->currentText().toDouble() + minVal[index];

        optMaxValLabel[index]->setText(QString::number(stringMax, 'f', 2) + unitVal[index]);
        if(isChangeRealVal)
            saveGeometryOptSetting(index);
        slider = nullptr;
    }

}
void wingDisplay::saveGeometryOptSetting(const int tmp){
    if(!optRealSettingArray.isEmpty()){
        int index1 = numsSpinBox->value() - 1;
        double min = optSlider[tmp]->GetLowerValue() * optCombox[tmp]->currentText().toDouble() + minVal[tmp];
        double max = optSlider[tmp]->GetUpperValue() * optCombox[tmp]->currentText().toDouble() + minVal[tmp];
        switch (tmp) {
        case 0:
            optRealSettingArray[wingChoiceIndex].MinSpan = min;
            optRealSettingArray[wingChoiceIndex].MaxSpan = max;

            break;
        case 1:
            optRealSettingArray[wingChoiceIndex].minChord[index1] = min;
            optRealSettingArray[wingChoiceIndex].maxChord[index1] = max;

            break;
        case 2:
            optRealSettingArray[wingChoiceIndex].minOffsetLength[index1] = min;
            optRealSettingArray[wingChoiceIndex].maxOffsetLength[index1] = max;

            break;
        case 3:
            optRealSettingArray[wingChoiceIndex].minTwisAngle[index1] = min;
            optRealSettingArray[wingChoiceIndex].maxTwisAngle[index1] = max;

            break;
        case 4:
            optRealSettingArray[wingChoiceIndex].minDihedralAngle[index1] = min;
            optRealSettingArray[wingChoiceIndex].maxDihedralAngle[index1] = max;

            break;
        default:
            break;
        }

    }
}
void wingDisplay::setIsChange(bool state){
    if(!optRealSettingArray.isEmpty()){
        SwitchButton *button = qobject_cast<SwitchButton*>(sender());
        int num = numsSpinBox->value() - 1;
        int index = button->property("button").toInt();
        switch (index) {
        case 0:
            optRealSettingArray[wingChoiceIndex].spanChangeArray[num] = state;
            break;
        case 1:
            optRealSettingArray[wingChoiceIndex].chordChangeArray[num] = state;
            break;
        case 2:
            optRealSettingArray[wingChoiceIndex].offsetChangeArray[num] = state;
            break;
        case 3:
            optRealSettingArray[wingChoiceIndex].twistChangeArray[num] = state;
            break;
        case 4:
            optRealSettingArray[wingChoiceIndex].dihedChangeArray[num] = state;
            break;
        default:
            break;
        }
    }
}
void wingDisplay::updateGeometryOptSetting(){
    if(!optRealSettingArray.isEmpty()){
        int index = numsSpinBox->value() - 1;
        int range[5][2];
        bool isTmp[5];
        range[0][0] = (optRealSettingArray[wingChoiceIndex].MinSpan - minVal[0]) / optCombox[0]->currentText().toDouble();
        range[1][0] = (optRealSettingArray[wingChoiceIndex].minChord[index] - minVal[1]) / optCombox[1]->currentText().toDouble();
        range[2][0] = (optRealSettingArray[wingChoiceIndex].minOffsetLength[index] - minVal[2]) / optCombox[2]->currentText().toDouble();
        range[3][0] = (optRealSettingArray[wingChoiceIndex].minTwisAngle[index] - minVal[3]) / optCombox[3]->currentText().toDouble();
        range[4][0] = (optRealSettingArray[wingChoiceIndex].minDihedralAngle[index] - minVal[4]) / optCombox[4]->currentText().toDouble();

        range[0][1] = (optRealSettingArray[wingChoiceIndex].MaxSpan - minVal[0]) / optCombox[0]->currentText().toDouble();
        range[1][1] = (optRealSettingArray[wingChoiceIndex].maxChord[index] - minVal[1]) / optCombox[1]->currentText().toDouble();
        range[2][1] = (optRealSettingArray[wingChoiceIndex].maxOffsetLength[index] - minVal[2]) / optCombox[2]->currentText().toDouble();
        range[3][1] = (optRealSettingArray[wingChoiceIndex].maxTwisAngle[index] - minVal[3]) / optCombox[3]->currentText().toDouble();
        range[4][1] = (optRealSettingArray[wingChoiceIndex].maxDihedralAngle[index] - minVal[4]) / optCombox[4]->currentText().toDouble();

        isTmp[0] = optRealSettingArray[wingChoiceIndex].spanChangeArray[index];
        isTmp[1] = optRealSettingArray[wingChoiceIndex].chordChangeArray[index];
        isTmp[2] = optRealSettingArray[wingChoiceIndex].offsetChangeArray[index];
        isTmp[3] = optRealSettingArray[wingChoiceIndex].twistChangeArray[index];
        isTmp[4] = optRealSettingArray[wingChoiceIndex].dihedChangeArray[index];

        for(int i = 0;i<5;i++){
            optSlider[i]->SetLowerValue(range[i][0]);
            optSlider[i]->SetUpperValue(range[i][1]);
            optSwithButton[i]->setChecked(isTmp[i]);
        }

        update();
    }
}
void wingDisplay::changeSliderType(const int tmp,const int index){
    optSlider[tmp]->SetRange(0,comboxMaxVal[tmp][index]);

}
void wingDisplay::updateOptSetting(){
    QLineEdit *edit = qobject_cast<QLineEdit*>(sender());
    if(edit){
        double tmp = edit->text().toDouble();
        int index = edit->property("edit").toInt();
        const double lower = std::min(optMinVal[index], optMaxVal[index]);
        const double upper = std::max(optMinVal[index], optMaxVal[index]);
        if (!std::isfinite(tmp) || tmp <= lower || tmp >= upper) {
            edit->setStyleSheet(warningEditStyle);
        }else{
            edit->setStyleSheet(nothingEditStyle);
        }
    }
    edit = nullptr;
}

bool wingDisplay::checkOptSetting(){
    if (wingChoiceIndex < 0 || wingChoiceIndex >= wingDataArray.size() ||
        wingChoiceIndex >= optRealSettingArray.size()) {
        QMessageBox::information(this, "警告", "机翼优化数据不完整");
        return false;
    }

    optSetting = optRealSettingArray[wingChoiceIndex];

    optSetting.initialEliteNum = optSettingEdit[0]->text().toInt();
    optSetting.step = optSettingEdit[1]->text().toInt();
    optSetting.eliteNum = optSettingEdit[2]->text().toInt();
    optSetting.selection = optSettingEdit[3]->text().toDouble();
    optSetting.cross = optSettingEdit[4]->text().toDouble();
    optSetting.variation = optSettingEdit[5]->text().toDouble();
    optSetting.cstRadio = optSettingEdit[6]->text().toDouble();
    optSetting.val = optSettingEdit[7]->text().toDouble();

    optSetting.solutionsNum = 24;

    if (wingThreadNum <= 0 || optSetting.initialEliteNum <= 0 ||
        optSetting.eliteNum <= 0 || optSetting.step <= 0) {
        QMessageBox::information(this, "警告", "线程、种群和迭代数量必须大于0");
        return false;
    }
    if (optSetting.initialEliteNum % wingThreadNum != 0 ||
        optSetting.eliteNum % wingThreadNum != 0) {
        QMessageBox::information(
            this, "警告", "初始种群和精英种群必须能被线程数量整除");
        return false;
    }
    if (optSetting.eliteNum > optSetting.initialEliteNum / 2) {
        QMessageBox::information(this, "警告",
                                 "初始种群至少是精英种群数量的两倍");
        return false;
    }
    if (!std::isfinite(optSetting.selection) || optSetting.selection <= 0.0 ||
        !std::isfinite(optSetting.cross) || optSetting.cross < 0.0 ||
        optSetting.cross > 1.0 || !std::isfinite(optSetting.variation) ||
        optSetting.variation < 0.0 || optSetting.variation > 1.0 ||
        !std::isfinite(optSetting.cstRadio) || optSetting.cstRadio <= 0.0 ||
        optSetting.cstRadio >= 1.0 || !std::isfinite(optSetting.val) ||
        optSetting.val <= 0.0) {
        QMessageBox::information(this, "警告", "遗传算法参数超出有效范围");
        return false;
    }

    const double velocity = wingVelocitySetEdit->text().toDouble();
    const double height = wingHeightSetEdit->text().toDouble();
    const double lift = wingLiftSetEdit->text().toDouble();
    if (!std::isfinite(velocity) || velocity <= 0.0 ||
        !std::isfinite(height) || height < 0.0 ||
        !std::isfinite(lift) || lift == 0.0) {
        QMessageBox::information(this, "警告", "设计速度、高度或升力无效");
        return false;
    }

    wingDefinition &wing = wingDataArray[wingChoiceIndex];
    QVector<QVector<double>> cstParameters;
    cstParameters.reserve(wing.spanW.size());
    for (int i = 0; i < wing.spanW.size(); ++i) {
        airfoilDesign design(12);
        if (i >= wing.airfoilArray.size()) {
            QMessageBox::information(this, "警告", "机翼翼型截面数据不完整");
            return false;
        }
        design.buildBenrnstein(wing.airfoilArray[i]);
        if (design.cstParameter.size() != optSetting.solutionsNum) {
            QMessageBox::information(this, "警告", "机翼CST参数生成失败");
            return false;
        }
        cstParameters.append(design.cstParameter);
    }
    wing.cstArray = cstParameters;
    wing.airfoilInputType = false;

    optSetting.spanVal = optCombox[0]->currentText().toDouble();
    optSetting.chordVal = optCombox[1]->currentText().toDouble();
    optSetting.offsetLengthVal = optCombox[2]->currentText().toDouble();
    optSetting.twisAngleVal = optCombox[3]->currentText().toDouble();
    optSetting.dihedralAngleVal = optCombox[4]->currentText().toDouble();

    return true;
}
void wingDisplay::exportChartData(PlotWidget *chart) {
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
            for (const PlotSeries *lineSeries : seriesList) {
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

        } else {

        }
    }
}

void wingDisplay::outputWingData(){

    wingVLM *solver = VLMSolverArray[wingChoiceIndex];




    //solver->initialGeometry(wingDataArray[wingChoiceIndex]);
    solver->initialGeometry(getWingDefine());
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
    solver = nullptr;
    QMessageBox::information(this, "信息", "保存成功");
}
void wingDisplay::resizeEvent(QResizeEvent *event){
    QWidget::resizeEvent(event);
    // 获取窗口的宽度和高度
}
void wingDisplay::addDataPointA()
{
    DataPointDialog dialog(this);
    if (dialog.exec() == QDialog::Accepted) {
        QVector<QPair<double, double>> dialogPoints = dialog.getPoints();

        for (const auto &point : dialogPoints) {
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
void wingDisplay::addDataPointB()
{
    DataPointDialog dialog(this);
    if (dialog.exec() == QDialog::Accepted) {
        QVector<QPair<double, double>> dialogPoints = dialog.getPoints();

        for (const auto &point : dialogPoints) {
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
void wingDisplay::addDataPointC()
{
    DataPointDialog dialog(this);
    if (dialog.exec() == QDialog::Accepted) {
        QVector<QPair<double, double>> dialogPoints = dialog.getPoints();

        for (const auto &point : dialogPoints) {
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
void wingDisplay::addDataPointD()
{
    DataPointDialog dialog(this);
    if (dialog.exec() == QDialog::Accepted) {
        QVector<QPair<double, double>> dialogPoints = dialog.getPoints();

        for (const auto &point : dialogPoints) {
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
void wingDisplay::clearDataPointA()
{
    // Clear all data points from the autoSeriesA and autoSeriesB
    autoSeriesA->clear();
    //autoSeriesB->clear();
    updateAxes(chartWA,wingResultSeriesA);

}
void wingDisplay::clearDataPointB()
{
    // Clear all data points from the autoSeriesA and autoSeriesB
    autoSeriesB->clear();
    //autoSeriesB->clear();
    updateAxes(chartWB,wingResultSeriesB);

}
void wingDisplay::clearDataPointC()
{
    // Clear all data points from the autoSeriesA and autoSeriesB
    autoSeriesC->clear();
    //autoSeriesB->clear();
    updateAxes(chartWC,wingResultSeriesC);

}
void wingDisplay::clearDataPointD()
{
    // Clear all data points from the autoSeriesA and autoSeriesB
    autoSeriesD->clear();
    //autoSeriesB->clear();
    updateAxes(chartWD,wingResultSeriesD);

}
void wingDisplay::removeSeriesLegendItem(PlotWidget *chart, PlotSeries *series) {
    Q_UNUSED(chart)
    series->setLegendVisible(false);
}
int wingDisplay::getAirfoilIndex(const QString tmp){
    int index = 0;
    for(int i = 0;i<AirfoilNameArray.length();i++){
        if(AirfoilNameArray[i] == tmp){
            return i;
        }
    }
    return index;
}
void wingDisplay::startCalculation() {

    dialogBar->setValue(0);
    progressDialog->show(); // 显示对话框

}
void wingDisplay::updateProgress(int value) {

    dialogBar->setValue(value);
    QApplication::processEvents(); // 确保界面更新

}
void wingDisplay::onCalculationFinished() {

    progressDialog->close();
}
void wingDisplay::initialProgressDialog(){

    // 初始化进度对话框
    progressDialog = new QDialog();
    QVBoxLayout *layout= new QVBoxLayout();
    dialogBar = new QProgressBar(progressDialog);
    layout->addWidget(dialogBar);
    progressDialog->setLayout(layout);

}
void wingDisplay::showWingDefineDialog(int index1,int index2){
    switch (index1) {
    case 0:

        wingChoiceIndex = index2;
        isAirplaneSingal = true;
        isModify = true;
        saveType = WING_WING;
        SpanNumB = wingDataArray[index2].chordLengthW.length();//更新机翼编辑列表的行数

        updateWingEdit(index2);

        wingNameEdit->setText(wingDataArray[index2].name);

        wingDefineWidget->show();

        updateView(wingDataArray[index2]);



        break;
    case 1:
        tailChoiceIndex = index2;
        isAirplaneSingal = true;
        isModify = true;
        if(tailDataArray[index2].isSymmetry){
            saveType = WING_HTAIL;
        }else{
            saveType = WING_VTAIL;
        }

        SpanNumB = tailDataArray[index2].chordLengthW.length();//更新机翼编辑列表的行数

        updateWingEdit(index2);

        wingNameEdit->setText(tailDataArray[index2].name);

        wingDefineWidget->show();
        updateView(tailDataArray[index2]);


        break;
    case 2:
        tailChoiceIndex = index2;
        isAirplaneSingal = true;
        isModify = true;
        if(tailDataArray[index2].isSymmetry){
            saveType = WING_HTAIL;
        }else{
            saveType = WING_VTAIL;
        }
        SpanNumB = tailDataArray[index2].chordLengthW.length();//更新机翼编辑列表的行数

        updateWingEdit(index2);

        wingNameEdit->setText(tailDataArray[index2].name);

        wingDefineWidget->show();
        updateView(tailDataArray[index2]);


        break;

    }
}
void wingDisplay::cancelDragDialog(){
    VLMSolverArray[wingChoiceIndex]->setXfoilDrag(false);

    QString name = "childXfoilDrag" + QString::number(wingChoiceIndex);
    QTreeWidgetItem* target = findNodeByIdentifier(wingTreeItemArray[wingChoiceIndex], name);
    if (target) {
        target->setCheckState(0, Qt::Unchecked);
    }
    VLMSolverArray[wingChoiceIndex]->setXfoilDrag(false);
    VLMSolverArray[wingChoiceIndex]->setUseLibraries(false);

    //uncheckNodeByIdentifier(wingTreeWidget, name);
    dragChoiceDialog->close();
}
void wingDisplay::saveDragDialog(){
    bool ok1 = dragBoxA->checkState();
    bool ok2 = dragBoxB->checkState();
    if(!ok1 && !ok2){

    }else{
        if(ok1){
            VLMSolverArray[wingChoiceIndex]->setXfoilDrag(true);
            VLMSolverArray[wingChoiceIndex]->setUseLibraries(false);

        }else{
            VLMSolverArray[wingChoiceIndex]->setXfoilDrag(true);
            VLMSolverArray[wingChoiceIndex]->setUseLibraries(true);

        }
    }



    dragChoiceDialog->close();
}
QTreeWidgetItem* wingDisplay::findNodeByIdentifier(QTreeWidgetItem* parent, const QString& identifier) {
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

// 在整个树中查找并取消指定标识节点的选中状态
void wingDisplay::uncheckNodeByIdentifier(QTreeWidget* treeWidget, const QString& identifier) {
    if (!treeWidget) return;

    // 遍历所有顶层节点
    for (int i = 0; i < treeWidget->topLevelItemCount(); ++i) {
        QTreeWidgetItem* node = findNodeByIdentifier(treeWidget->topLevelItem(i), identifier);
        if (node) {
            // 取消复选框选中状态
            node->setCheckState(0, Qt::Unchecked);
            // 取消节点选中高亮状态
            node->setSelected(false);
            return;
        }
    }

    // 如果没找到节点，输出调试信息
    qDebug() << "未找到标识为" << identifier << "的节点";
}
void wingDisplay::closeOtherRoots(QTreeWidgetItem* item, int /*column*/){
    if (!wingTreeWidget || !item) return;

    // 找到当前点击项所在的根节点
    QTreeWidgetItem* currentRoot = item;
    while (currentRoot->parent() != nullptr) {
        currentRoot = currentRoot->parent();
    }

    // 遍历所有根节点，折叠非当前根节点
    for (int i = 0; i < wingTreeWidget->topLevelItemCount(); ++i) {
        QTreeWidgetItem* root = wingTreeWidget->topLevelItem(i);

        if (root != currentRoot) {
            wingTreeWidget->collapseItem(root); // 折叠根节点，子节点会自动折叠
        }else{
            //wingTreeWidget->expandItem(root); // 展开当前根节点（可选）
        }

    }
}
