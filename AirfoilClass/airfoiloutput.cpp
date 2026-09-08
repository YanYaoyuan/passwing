#if defined(_MSC_VER) && (_MSC_VER >= 1600)
# pragma execution_character_set("utf-8")
#endif


#include "airfoiloutput.h"
#include "airfoildesign.h"

#include <cmath>

#include <QMessageBox>
#include <QFile>
#include <QTextStream>
#include <QFileDialog>
#include <QDebug>
namespace {
constexpr double kPi = 3.14159265358979323846;
}
airfoilOutput::airfoilOutput()
{

}
void airfoilOutput::buildOutputAirfoilDialog(QVector<QString>&namearray){

    foilNameArray = namearray;

    if(!foilDialog){
        foilDialog = new QDialog();
        foilVbox = new QVBoxLayout(foilDialog);

        foilBoxA = new QGroupBox(foilDialog);
        foilLayoutA = new QGridLayout(foilBoxA);

        foilBoxB = new QGroupBox(foilDialog);
        foilLayoutB = new QGridLayout(foilBoxB);

        foilHbox = new QHBoxLayout();

        /***************foilBoxA****************/
        foilNameLabel = new QLabel(foilDialog);
        foilCombobox = new QComboBox(foilDialog);
        foilLibariesButton = new QPushButton(foilDialog);
        foilMoreChoiceButton = new QPushButton(foilDialog);


        foilNameLabel->setText("翼型名称");
        if(!namearray.isEmpty()){
            for(const QString &name:namearray)
                foilCombobox->addItem(name);
        }
        foilLibariesButton->setText("翼型库");
        foilBoxA->setTitle("输出翼型设置");
        foilBoxA->setFixedHeight(120);







        /***************foilBoxB****************/
        foilModelLabel = new QLabel(foilDialog);
        foilModelCombobox = new QComboBox(foilDialog);  //离散方式
        foilNumLabel = new QLabel(foilDialog);
        foilNumSpinBox = new QSpinBox(foilDialog);
        foilCSTNumLabel = new QLabel(foilDialog);
        foilCSTNumSpinBox = new QSpinBox(foilDialog);
        foilYteLabel = new QLabel(foilDialog);
        foilYteSpinBox = new QDoubleSpinBox(foilDialog);
        foilYteUnitLabel = new QLabel(foilDialog);




        foilChart = new QChart();
        foilView = new QChartView(foilChart);
        foilSeries = new QLineSeries();
        foilScaleSeries = new QScatterSeries();
        foilXaxis = new QValueAxis();
        foilYaxis = new QValueAxis();
        foilXaxis->setRange(-0.1,1.1);
        foilYaxis->setRange(-0.3,0.3);

        foilChart->addAxis(foilXaxis,Qt::AlignBottom);
        foilChart->addAxis(foilYaxis,Qt::AlignLeft);
        foilView->setRenderHint(QPainter::Antialiasing);



        foilScaleSeries->setMarkerSize(4);
        foilScaleSeries->setColor(Qt::blue);
        foilChart->addSeries(foilSeries); // 添加系列到图表
        foilChart->addSeries(foilScaleSeries);
        foilSeries->setColor(Qt::black);
        QPen pen;
        pen.setWidth(1);
        foilSeries->setPen(pen);

        foilScaleSeries->attachAxis(foilXaxis);
        foilScaleSeries->attachAxis(foilYaxis);
        foilSeries->attachAxis(foilXaxis);
        foilSeries->attachAxis(foilYaxis);

        foilChart->legend()->hide();
        foilXaxis->hide();
        foilYaxis->hide();




        foilModelLabel->setText("坐标离散方式");
        foilModelCombobox->addItem("默认");
        foilModelCombobox->addItem("平均分布");
        foilModelCombobox->addItem("余弦分布");


        foilNumLabel->setText("坐标数量");
        foilNumSpinBox->setValue(50);
        foilNumSpinBox->setMaximum(200);
        foilNumSpinBox->setMinimum(3);
        foilCSTNumLabel->setText("CST阶数");
        foilCSTNumSpinBox->setValue(6);
        foilCSTNumSpinBox->setMinimum(4);
        foilYteLabel->setText("尾缘厚度");
        foilYteSpinBox->setValue(0);
        foilYteSpinBox->setSingleStep(0.1);
        foilYteUnitLabel->setText("mm");



        foilBoxB->setTitle("输出坐标设置");





        /***********保存**********/
        foilSaveButton = new QPushButton(foilDialog);
        foilCancelButton = new QPushButton(foilDialog);

        foilSaveButton->setText("保存");
        foilCancelButton->setText("取消");

        foilLayoutA->addWidget(foilNameLabel,0,0,1,1);
        foilLayoutA->addWidget(foilCombobox,0,1,1,1);
        foilLayoutA->addWidget(foilLibariesButton,0,2,1,1);
        foilLayoutA->addWidget(foilMoreChoiceButton,0,3,1,2);

        foilLayoutB->addWidget(foilModelLabel,0,0,1,1);
        foilLayoutB->addWidget(foilModelCombobox,0,1,1,1);
        foilLayoutB->addWidget(foilNumLabel,0,2,1,1);
        foilLayoutB->addWidget(foilNumSpinBox,0,3,1,2);
        foilLayoutB->addWidget(foilCSTNumLabel,1,0,1,1);
        foilLayoutB->addWidget(foilCSTNumSpinBox,1,1,1,1);
        foilLayoutB->addWidget(foilYteLabel,1,2,1,1);
        foilLayoutB->addWidget(foilYteSpinBox,1,3,1,1);
        foilLayoutB->addWidget(foilYteUnitLabel,1,4,1,1);

        foilLayoutB->addWidget(foilView,2,0,4,5);

        foilHbox->addWidget(foilSaveButton);
        foilHbox->addWidget(foilCancelButton);

        foilVbox->addWidget(foilBoxA);
        foilVbox->addWidget(foilBoxB);
        foilVbox->addLayout(foilHbox);
        isFoilMoreChoice = false;
        hideFoilBoxB();


        foilDialog->setLayout(foilVbox);
        foilDialog->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        foilDialog->setFixedWidth(600);



        connect(foilCancelButton,&QPushButton::clicked,this,&airfoilOutput::deleteFoilDialog);
        connect(foilDialog,&QDialog::finished,this,&airfoilOutput::deleteFoilDialog);
        connect(foilMoreChoiceButton,&QPushButton::clicked,this,&airfoilOutput::hideFoilBoxB);

        //connect(foilModelCombobox,&QComboBox::currentIndexChanged,this,&airfoilOutput::changeFoil);
        //connect(foilCombobox,&QComboBox::currentIndexChanged,this,&airfoilOutput::changeFoil);
        //connect(foilNumSpinBox,&QSpinBox::valueChanged,this,&airfoilOutput::changeFoil);
        //connect(foilYteSpinBox,&QDoubleSpinBox::valueChanged,this,&airfoilOutput::changeFoil);

        // change by YYY
        connect(foilModelCombobox,QOverload<const int>::of(&QComboBox::activated),this,&airfoilOutput::changeFoil);
        connect(foilCombobox,QOverload<const int>::of(&QComboBox::activated),this,&airfoilOutput::changeFoil);
        connect(foilNumSpinBox,QOverload<const int>::of(&QSpinBox::valueChanged),this,&airfoilOutput::changeFoil);
        connect(foilYteSpinBox,QOverload<const double>::of(&QDoubleSpinBox::valueChanged),this,&airfoilOutput::changeFoil);
        connect(foilCSTNumSpinBox,QOverload<const int>::of(&QSpinBox::valueChanged),this,&airfoilOutput::drawFoil);

        connect(foilSaveButton,&QPushButton::clicked,this,&airfoilOutput::saveFoil);
        }else{
            if(!namearray.isEmpty()){
                foilCombobox->clear();
                for(const QString &name:namearray)
                    foilCombobox->addItem(name);
            }
        }
        foilDialog->show();
}
void airfoilOutput::buildLossModelADialog(){
    if(!lossModelADialog){
        lossModelADialog = new QDialog();
        lossLayoutA = new QGridLayout(lossModelADialog);

        for(int i = 0;i < 8;i++){
            QCheckBox *box = new QCheckBox(lossModelADialog);
            QLabel *label1 = new QLabel(lossModelADialog);
            QLineEdit *edit1 = new QLineEdit(lossModelADialog);
            QLabel *label2 = new QLabel(lossModelADialog);
            QLineEdit *edit2 = new QLineEdit(lossModelADialog);
            QLabel *label3 = new QLabel(lossModelADialog);
            QLineEdit *edit3 = new QLineEdit(lossModelADialog);
            QLabel *label4 = new QLabel(lossModelADialog);

            box->setChecked(false);
            label1->setText("圆孔");
            label2->setText("% 的翼肋厚度直径，X = ");
            label3->setText("% 的弦长，Y = ");
            label4->setText("% 的翼肋厚度");
            edit1->setText("50");
            edit2->setText(QString::number((i + 1) * 10));
            edit3->setText("50");

            lossLayoutA->addWidget(box,i,0,1,1);
            lossLayoutA->addWidget(label1,i,1,1,1);
            lossLayoutA->addWidget(edit1,i,2,1,1);
            lossLayoutA->addWidget(label2,i,3,1,1);
            lossLayoutA->addWidget(edit2,i,4,1,1);
            lossLayoutA->addWidget(label3,i,5,1,1);
            lossLayoutA->addWidget(edit3,i,6,1,1);
            lossLayoutA->addWidget(label4,i,7,1,1);

            choiceCheckBoxArray.append(box);
            textALabelArray1.append(label1);
            diameterLineEditArray.append(edit1);
            textALabelArray2.append(label2);
            locationXALineEditArray.append(edit2);
            textALabelArray3.append(label3);
            locationYALineEditArray.append(edit3);
            textALabelArray4.append(label4);

        }

        saveLossModelAButton = new QPushButton(lossModelADialog);
        cancelLossModelAButton = new QPushButton(lossModelADialog);
        saveLossModelAButton->setText("保存");
        cancelLossModelAButton->setText("取消");
        lossLayoutA->addWidget(saveLossModelAButton,8,1,1,3);
        lossLayoutA->addWidget(cancelLossModelAButton,8,5,1,3);

        connect(lossModelADialog,&QDialog::finished,this,&airfoilOutput::deleteLossADialog);
    }

    lossModelADialog->show();
}
void airfoilOutput::addFoil(const QString &name, const QVector<QVector<double>> &foil){



    foilDataArray.append(foil);
    foilNameArray.append(name);
    if(!name.isEmpty()){
        foilCombobox->addItem(name);
    }
    int length = foilCombobox->count();

    foilCombobox->setCurrentIndex(length - 1);
    drawFoil();



}
void airfoilOutput::hideFoilBoxB(){
    if(isFoilMoreChoice){
        isFoilMoreChoice = false;
        foilMoreChoiceButton->setText("更少");

        showAllWidgetsInBox(foilBoxB);
        foilBoxB->show();

    }else{
        isFoilMoreChoice = true;
        foilMoreChoiceButton->setText("更多");

        hideAllWidgetsInBox(foilBoxB);
        foilBoxB->hide();


    }
}

void airfoilOutput::drawFoil(){
    int cstNum = foilCSTNumSpinBox->value();
    int index = foilModelCombobox->currentIndex();
    int num = foilNumSpinBox->value();
    int foilIndex = foilCombobox->currentIndex();
    double yte = foilYteSpinBox->value() / 1000;


    airfoilDesign design(cstNum);

    if(!foilDataArray[foilIndex].isEmpty()){
        if(!foilSeries->points().isEmpty()){
            foilSeries->clear();
        }
        if(!foilScaleSeries->points().isEmpty()){
            foilScaleSeries->clear();
        }



        design.buildBenrnstein(foilDataArray[foilIndex]);


        double currentThickness = design.yTe1 - design.yTe2;

        if (std::abs(currentThickness) > 1e-12) {
            // 有有效厚度 → 按比例缩放
            double scale = yte / currentThickness;
            design.yTe1 *= scale;
            design.yTe2 *= scale;
        } else {
            // 厚度为0 → 只能重新分配
            design.yTe1 =  yte / 2.0;
            design.yTe2 = -yte / 2.0;
        }

        QVector<double>xData;


        switch (index) {
        case 0:



            break;
        case 1:
            for(int i = 0;i < num;i++){
                xData.append(double(i) / (num - 1));
            }
            break;
        case 2:
            for(int i = 0;i < num;i++){
                xData.append(0.5 - std::cos(static_cast<double>(i) * kPi / (num - 1)) * 0.5);
            }
            break;
        default:
            break;
        }



        if (index != 0 && xData.isEmpty()) {
            qWarning() << "xData is empty while index != 0";
            return;
        }

        if (index == 0) {
            design.buildAirfoilCurve(design.cstParameter);
        } else {
            design.buildAirfoilCurve(design.cstParameter, xData);
        }

        foilSeries->clear();
        foilScaleSeries->clear();

        for (int i = 0; i < design.newAirfoilData.length(); i++) {
            foilSeries->append(design.newAirfoilData[i][0],
                               design.newAirfoilData[i][1]);
            foilScaleSeries->append(design.newAirfoilData[i][0],
                                    design.newAirfoilData[i][1]);
        }
        outputFoilData = design.newAirfoilData;




    }

}
void airfoilOutput::changeFoil(const int index){
    drawFoil();
}
void airfoilOutput::copyFoilArray(const QVector<QVector<QVector<double>>> &array){
    foilDataArray = array;

}
void airfoilOutput::deleteFoilDialog(){
    foilDataArray.clear();
    foilNameArray.clear();
    foilDialog->close();
    if(foilDialog)
        delete foilDialog;
    foilDialog = nullptr;

}
void airfoilOutput::deleteLossADialog(){
    if(lossModelADialog)
        delete lossModelADialog;
    choiceCheckBoxArray.clear();
    textALabelArray1.clear();
    diameterLineEditArray.clear();
    textALabelArray2.clear();
    locationXALineEditArray.clear();
    textALabelArray3.clear();
    locationYALineEditArray.clear();
    textALabelArray4.clear();
}
void airfoilOutput::hideAllWidgetsInBox(QGroupBox *box) {
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
void airfoilOutput::showAllWidgetsInBox(QGroupBox *box) {
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
void airfoilOutput::saveFoil(){

    int foilIndex = foilCombobox->currentIndex();
    QString FoilName = foilNameArray[foilIndex];
    QVector<QVector<double>>FoilData;

    if(foilMoreChoiceButton->text() == "更多"){
        FoilData = foilDataArray[foilIndex];
    }
    else{
        FoilData = outputFoilData;
    }


    if(!FoilData.isEmpty()){
        QString filter = tr("Dat Files(*.dat);;Txt Files(*.txt)");
        QString fileName = QFileDialog::getSaveFileName(nullptr,tr("保存类型"),FoilName,filter);
        if(!fileName.isEmpty()){
            QFile file(fileName);
            if(file.open(QIODevice::WriteOnly|QIODevice::Text)){
                QTextStream stream(&file);
                stream<<FoilName<<"\n";
                for(const QVector<double> &row:FoilData)
                    if(row.size()>=2){
                        stream<<row[0]<<"  "<<" "<<" "<<row[1]<<"\n";
                    }
            }
            file.close();
    }
    }
    //drawAirfoilDXF(FoilData);
    foilDialog->close();
    QMessageBox::information(nullptr,"通知","保存成功");
}
void airfoilOutput::drawAirfoilDXF(const QVector<QVector<double>>&airfoil){

    QFile dxfFile("air.dxf");
    if(!dxfFile.open(QIODevice::WriteOnly|QIODevice::Truncate)){
        return ;
    }
    QTextStream dxfStream(&dxfFile);
    writeDXFheader(dxfStream);
    ACircle cir{0,0,0.02};
    ARectangle rec{0.05,0.05,45,1,0};


    writeDXFEntities(dxfStream,airfoil);
    drawCircleDXF(dxfStream,cir);
    drawRectangleDXF(dxfStream,rec);




    dxfStream<<"0"<<"\n"<<"ENDSEC"<<"\n";
    dxfStream<<"0"<<"\n"<<"EOF"<<"\n";
    dxfFile.close();
}

void airfoilOutput::writeDXFheader(QTextStream& dxfstream){

    dxfstream<<"0"<<"\n"<<"SECTION"<<"\n"<<"2"<<"\n"<<"HEADER"<<"\n"<<"0"<<"\n"<<"ENDSEC"<<"\n";
    dxfstream<<"0"<<"\n"<<"SECTION"<<"\n"<<"2"<<"\n"<<"ENTITIES"<<"\n";

}

void airfoilOutput::writeDXFEntities(QTextStream& dxfstream,const QVector<QVector<double>>&airfoil){



    dxfstream<<"0"<<"\n"<<"SPLINE"<<"\n";
    dxfstream<<"8"<<"\n"<<"Layer1"<<"\n";
    dxfstream<<"70"<<"\n"<<"8"<<"\n";
    dxfstream << "71"<<"\n"<<3<<"\n";  // 样条曲线的度数
    int numPoints = airfoil.length();
    int numKnots = numPoints + 4; // 三次样条的简单计算公式
    dxfstream << "72"<<"\n"<<numKnots<<"\n";  // 节点数
    dxfstream << "73"<<"\n"<<numPoints<<"\n";  // 控制点数量
    dxfstream << "74"<<"\n"<<0<<"\n";  // 适配点数量

    for (int i = 0; i < numKnots; ++i) {
        dxfstream << "40\n" << (double)i / (numKnots - 1) << "\n";  // 简化的均匀节点向量
    }

    for(const QVector<double>&data:airfoil){
        //dxfstream<<"0"<<"\n"<<"VERTEX"<<"\n";
        dxfstream<<"10"<<"\n"<<data[0]<<"\n";
        dxfstream<<"20"<<"\n"<<data[1]<<"\n";
    }
    // dxfstream<<"10"<<"\n"<<airfoil[airfoil.length() - 1][0]<<"\n";
    // dxfstream<<"10"<<"\n"<<airfoil[airfoil.length() - 1][1]<<"\n";


    //dxfstream<<"0"<<"\n"<<"SEQEND"<<"\n";







}
void airfoilOutput::drawCircleDXF(QTextStream&dxfstream,const ACircle &circle){
    dxfstream << "0\n"; // 实体类型标记
    dxfstream << "CIRCLE\n"; // 实体类型
    dxfstream << "8\n"; // 图层
    dxfstream << "0\n"; // 图层名（在这里假设图层名为"0"）
    dxfstream << "10\n"<< circle.x << "\n"; // 圆心 X 坐标值
    dxfstream << "20\n"<< circle.y << "\n"; // 圆心 Y 坐标值
    dxfstream << "40\n"<< circle.radius << "\n"; // 圆的半径值



}
void airfoilOutput::drawRectangleDXF(QTextStream&dxfstream, const ARectangle &rectangle){

    double X1 = -rectangle.a / 2;
    double Y1 = rectangle.b / 2;
    double X2 = rectangle.a / 2;
    double Y2 = rectangle.b / 2;
    double X3 = rectangle.a / 2;
    double Y3 = -rectangle.b / 2;
    double X4 = -rectangle.a / 2;
    double Y4 = -rectangle.b / 2;
    double x1,y1,x2,y2,x3,y3,x4,y4;
    if(rectangle.roateAngle != 0){
        double radians = rectangle.roateAngle / 180 * kPi;
        x1 = X1 * std::cos(radians) - Y1 * std::sin(radians);
        y1 = X1 * std::sin(radians) + Y1 * std::cos(radians);
        x2 = X2 * std::cos(radians) - Y2 * std::sin(radians);
        y2 = X2 * std::sin(radians) + Y2 * std::cos(radians);
        x3 = X3 * std::cos(radians) - Y3 * std::sin(radians);
        y3 = X3 * std::sin(radians) + Y3 * std::cos(radians);
        x4 = X4 * std::cos(radians) - Y4 * std::sin(radians);
        y4 = X4 * std::sin(radians) + Y4 * std::cos(radians);
    }else{
        x1 = X1;
        y1 = Y1;
        x2 = X2;
        y2 = Y2;
        x3 = X3;
        y3 = Y3;
        x4 = X4;
        y4 = Y4;
    }
    x1 = x1 + rectangle.x;
    y1 = y1 + rectangle.y;
    x2 = x2 + rectangle.x;
    y2 = y2 + rectangle.y;
    x3 = x3 + rectangle.x;
    y3 = y3 + rectangle.y;
    x4 = x4 + rectangle.x;
    y4 = y4 + rectangle.y;
    dxfstream << "0\n"; // 实体类型标记
    dxfstream << "POLYLINE\n"; // LWPOLYLINE 实体类型
    dxfstream << "8\n"; // 图层
    dxfstream << "0\n"; // 图层名（在这里假设图层名为"0"）
    //dxfstream << "90\n"; // 指定矩形为闭合多段线
    dxfstream << "70\n"; // 指定多段线属性为闭合
    dxfstream << "1\n"; // 指定多段线是否闭合（1表示闭合，0表示不闭合）
    dxfstream<<"0"<<"\n"<<"VERTEX"<<"\n";
    dxfstream << "10\n"<< x1 << "\n"; // 第一个顶点 X 坐标值
    dxfstream << "20\n"<< y1 << "\n"; // 第一个顶点 Y 坐标值
    dxfstream<<"0"<<"\n"<<"VERTEX"<<"\n";
    dxfstream << "10\n"<< x2 << "\n"; // 第二个顶点 X 坐标值
    dxfstream << "20\n"<< y2 << "\n"; // 第二个顶点 Y 坐标值
    dxfstream<<"0"<<"\n"<<"VERTEX"<<"\n";
    dxfstream << "10\n"<< x3 << "\n"; // 第三个顶点 X 坐标值
    dxfstream << "20\n"<< y3 << "\n"; // 第三个顶点 Y 坐标值
    dxfstream<<"0"<<"\n"<<"VERTEX"<<"\n";
    dxfstream << "10\n"<< x4 << "\n"; // 第四个顶点 X 坐标值
    dxfstream << "20\n"<< y4 << "\n"; // 第四个顶点 Y 坐标值
}

void airfoilOutput::buildConversionDialog(){
    if(!foilConversionDialog){
        foilConversionDialog = new QDialog();
        foilBoxC = new QGroupBox(foilConversionDialog);
        foilLayoutC = new QGridLayout(foilBoxC);

        inputLabelA = new QLabel(foilConversionDialog);
        inputButtonA = new QPushButton(foilConversionDialog);
        outputLabelA = new QLabel(foilConversionDialog);
        outputButTonA = new QPushButton(foilConversionDialog);

        inputLabelA->setText("DXF");
        inputButtonA->setText("输入");
        outputLabelA->setText("DAT");
        outputButTonA->setText("输出");

        foilLayoutC->addWidget(inputLabelA,0,0,1,1);
        foilLayoutC->addWidget(inputButtonA,0,1,1,1);
        foilLayoutC->addWidget(outputLabelA,0,2,1,1);
        foilLayoutC->addWidget(outputButTonA,0,3,1,1);

        connect(foilConversionDialog,&QDialog::finished,this,&airfoilOutput::deleteConversionDialog);
    }

}
void airfoilOutput::inputData(){
    QString filePath = QFileDialog::getOpenFileName(nullptr,"选择文件","","翼型文件(*.dat);;所有文件(*)");
    QVector<QVector<double>>airfoil;

    if(filePath.isEmpty()){
        exit(1);
    }

    QFile file(filePath);

    if(!file.open(QIODevice::ReadOnly | QIODevice::Text)){
        exit(1);

    }

    QTextStream in(&file);

    while(!in.atEnd()){
        QString line = in.readLine();
        QStringList parts = line.split(" ",Qt::SkipEmptyParts);
        if(parts.size() ==2){
            double value1 = parts[0].toDouble();
            double value2 = parts[1].toDouble();

            QVector<double>row;
            row.append(value1);
            row.append(value2);

            airfoil.append(row);
        }
    }
    file.close();
}
void airfoilOutput::deleteConversionDialog(){
    if(foilConversionDialog)
        delete foilConversionDialog;
}
