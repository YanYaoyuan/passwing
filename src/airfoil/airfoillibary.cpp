#include "airfoillibary.h"
#include <QDir>
#include<QMessageBox>
#include <QtMath>
#include "airfoildesign.h"

#include <QDebug>

#if defined(_MSC_VER) && (_MSC_VER >= 1600)
# pragma execution_character_set("utf-8")
#endif

using namespace H5;


airfoilLibary::airfoilLibary(QWidget *parent)
    : QWidget(parent) {

    libaryWidget = new QWidget(this);
    setFixedSize(650,450);
    setWindowTitle("library");
    gridlayoutA = new QGridLayout(this);
    graphChart = new QChart();
    graphview = new QChartView(graphChart);
    //graphview->setFixedSize(300,300);

    modelCombox = new QComboBox(libaryWidget);
    //listModel = new QStringListModel(libaryWidget);

    searchNameButton = new QPushButton(libaryWidget);

    searchNameEdit = new QLineEdit(libaryWidget);

    searchParametersButton = new QPushButton(libaryWidget);

    searchSimilarButton = new QPushButton(libaryWidget);

    importAirfoilButton = new QPushButton(libaryWidget);
    sortModelCombobox = new QComboBox(libaryWidget);
    series = new QLineSeries();
    xaxis = new QValueAxis;
    yaxis = new QValueAxis;


    modelCombox->addItem("profili数据");
    modelCombox->addItem("UIUC数据");
    modelCombox->addItem("windAIBench数据");

    searchNameButton->setText("查找");
    searchParametersButton->setText("按参数查找");
    searchSimilarButton->setText("按相似度查找");
    importAirfoilButton->setText("导入翼型");

    sortModelCombobox->addItem("默认排序");
    sortModelCombobox->addItem("最大升力系数降序");
    sortModelCombobox->addItem("最大升阻比降序");
    sortModelCombobox->addItem("最小力矩系数升序");

    searchParametersButton->setEnabled(false);
    searchSimilarButton->setEnabled(false);



    listView = new QListView(libaryWidget);
    listView->setEditTriggers(QAbstractItemView::NoEditTriggers);

    Airfoil.resize(2000);
    AirfoilCopy.resize(2000);

    AirfoilA.resize(1000);
    AirfoilCopyA.resize(1000);

    AirfoilB.resize(1600);
    AirfoilCopyB.resize(1600);

    AirfoilC.resize(2000);
    AirfoilCopyC.resize(2000);



    //importAirfoilData();
    //writeAirfoilParameters();

    //listView.show();

    searchNameButton->setFixedSize(150,25);
    searchNameEdit->setFixedSize(200,25);
    searchSimilarButton->setFixedSize(350,25);
    importAirfoilButton->setFixedSize(350,25);

    searchNameEdit->setStyleSheet("border : none");

    connect(importAirfoilButton,&QPushButton::clicked,this,&airfoilLibary::emitButtonClicked);
    connect(sortModelCombobox, SIGNAL(currentIndexChanged(int)), this, SLOT(changeAirfoilList(int)));

    gridlayoutA->addWidget(modelCombox,0,0,1,3);
    gridlayoutA->addWidget(listView,1,0,4,3);
    gridlayoutA->addWidget(sortModelCombobox,5,0,1,2);
    gridlayoutA->addWidget(searchNameButton,2,3,1,1);
    gridlayoutA->addWidget(searchNameEdit,2,4,1,1);
    gridlayoutA->addWidget(searchParametersButton,3,3,1,2);
    gridlayoutA->addWidget(searchSimilarButton,4,3,1,2);
    gridlayoutA->addWidget(importAirfoilButton,5,3,1,2);
    gridlayoutA->addWidget(graphview,0,3,2,2);


    graphview->setRubberBand(QChartView::RectangleRubberBand);
    graphview->setRubberBandSelectionMode(Qt::IntersectsItemBoundingRect);
    graphview->setRenderHint(QPainter::Antialiasing);
    xaxis->setRange(0,1);
    yaxis->setRange(-0.5,0.5);
    graphChart->addAxis(xaxis,Qt::AlignBottom);
    graphChart->addAxis(yaxis,Qt::AlignLeft);
    graphChart->addSeries(series);
    QPen pen(Qt::black);
    series->attachAxis(xaxis);
    series->attachAxis(yaxis);
    series->setPen(pen);
    graphChart->legend()->setVisible(false);
    xaxis->hide();
    yaxis->hide();


    mouseLocitionLibary = new QGraphicsSimpleTextItem("");
    //QPointF chartPos(50,200);



    mouseLocitionLibary->setFont(QFont("Arial", 8));


    mouseLocitionLibary->setBrush(QBrush(Qt::black));

    updateTextPosition();

    //QPointF scenePos = graphChart->mapToScene(chartPos);
    //mouseLocitionLibary->setPos(scenePos);
    graphChart->scene()->addItem(mouseLocitionLibary);

    // change by YYY
    connect(searchNameButton,SIGNAL(clicked()),this,SLOT(searchAirfoil()));
    connect(modelCombox, SIGNAL(currentIndexChanged(int)), this, SLOT(changeAirfoilModel(int)));

}
void airfoilLibary::initial(){


    //importAirfoilData();             //遍历文件内所有dat文件

    importMyAirfoilLibaryA();           //读所有的翼型数据
    importMyAirfoilLibaryB();           //读所有的翼型数据
    importMyAirfoilLibaryC();



    //solveOtherParameters();

    //writeAirfoilParameters();        //写入翼型的各种参数

    readAirfoilParametersA();           //读取翼型的各种参数
    readAirfoilParametersB();           //读取翼型的各种参数
    readAirfoilParametersC();

    //writeAirfoilParameters();

    //importMyResultA();


    initialDone = true;




}
void airfoilLibary::changeAirfoilModel(const int index){
    QVector<QString>nameArray;
    switch (index) {
    case 0:

        for(int i = 0; i<listLengthA;i++)
            nameArray.append(AirfoilA[i].Name);
        searchType = false;
        modelType = AIRFOIL_PROFILI;
        Airfoil = AirfoilA;
        for(int i = 0;i<listLengthA;i++)
            airfoilArray[i] = airfoilArrayA[i];
        listLength = listLengthA;

        sortModelCombobox->setEnabled(true);




        break;
    case 1:
        for(int i = 0; i<listLengthB;i++)
            nameArray.append(AirfoilB[i].Name);
        searchType = false;
        modelType = AIRFOIL_UIUC;
        isSort = false;
        Airfoil = AirfoilB;
        for(int i = 0;i<listLengthB;i++)
            airfoilArray[i] = airfoilArrayB[i];
        listLength = listLengthB;

        sortModelCombobox->setEnabled(false);
        break;
    case 2:
        for(int i = 0; i<listLengthC;i++)
            nameArray.append(AirfoilC[i].Name);
        searchType = false;
        modelType = AIRFOIL_WINDAIBENCH;
        isSort = false;
        Airfoil = AirfoilC;
        for(int i = 0;i<listLengthC;i++)
            airfoilArray[i] = airfoilArrayC[i];
        listLength = listLengthC;
        sortModelCombobox->setEditable(false);

        break;
    default:
        break;
    }



    QStringList stringList = QStringList::fromVector(nameArray);

    listModel->setStringList(stringList);

}
int airfoilLibary::importAirfoilData(){
    //QString Name =QDir::currentPath() +  "/libaries/profili/airfoil";
    QString Name =QDir::currentPath() +  "/libaries/airfoil/UIUC/airfoil";
    QDir directory(Name);
    QStringList entries = directory.entryList(QDir::AllEntries | QDir::NoDotAndDotDot);


    int entryCount = 0;
    for(const QString& entry :entries){
        //qDebug()<<entry;
        AirfoilA[entryCount].Name = entry;
        entryCount++;
    }

    int len = entryCount;
    listLengthA = entryCount;


    //读取翼型库里所有翼型
    for(int i = 0; i < len;i++){
        QVector<QVector<double>>tmp;
        QString filePath = Name + "/" + AirfoilA[i].Name;
        QFile file (filePath);


        if(!file.open(QIODevice::ReadOnly | QIODevice::Text)){
            exit(1);

        }

        QTextStream in(&file);

        if(!in.atEnd()){
             in.readLine();
        }


        while(!in.atEnd()){
            QString line = in.readLine();
            QStringList parts = line.split(" ",Qt::SkipEmptyParts);
            if(parts.size() ==2){
                double value1 = parts[0].toDouble();
                double value2 = parts[1].toDouble();

                QVector<double>row;
                row.append(value1);
                row.append(value2);
                tmp.append(row);

            }

        }

        airfoilArrayA[i].append(tmp);


        file.close();



    }




    //将所有的翼型数据写入新的结构体文件
        QString fileName = Name + "/data.txt";
        QFile files(fileName);
        if(!fileName.isEmpty()){

            if(files.open(QIODevice::WriteOnly|QIODevice::Text)){

                for(int i = 0; i<len;i++){
                QTextStream stream(&files);
                stream<<AirfoilA[i].Name<<"\n";
                for(const QVector<double> &row:airfoilArrayA[i])
                    if(row.size()>=2){
                        stream<<row[0]<<"  "<<" "<<" "<<row[1]<<"\n";
                    }
            }
            files.close();
        }
        }


    return 0;

}

void airfoilLibary::importMyResultA(){
    QString fileName = QDir::currentPath() + "/libaries/airfoil/profili/result.txt";
    QFile file(fileName);

    if (!file.exists()) {
        qDebug() << "File does not exist:" << fileName;
        return;
    }

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "Unable to open file:" << fileName;
        return;
    }

    QTextStream stream(&file);
    int i = 0;
    while (!stream.atEnd()) {
        QString line = stream.readLine();
        QStringList parts = line.split("   ", Qt::SkipEmptyParts);

        if (parts.size() < 5) {
            qDebug() << "Invalid line format:" << line<<i;
            continue;  // Skip this line if it doesn't have enough parts
        }

        bool ok1, ok2, ok3, ok4;
        double val1 = parts[1].toDouble(&ok1);
        double val2 = parts[2].toDouble(&ok2);
        double val3 = parts[3].toDouble(&ok3);
        double val4 = parts[4].toDouble(&ok4);

        if (!ok1 || !ok2 || !ok3 || !ok4) {
            qDebug() << "Conversion error in line:" << line;
            continue;  // Skip this line if conversion failed
        }

        resultNameA.append(parts[0]);
        QVector<double> tmp;
        tmp.append(val1);
        tmp.append(val2);
        tmp.append(val3);
        tmp.append(val4);
        resultArrayA.append(tmp);
        i++;
    }
    file.close();

}

void airfoilLibary::importMyAirfoilLibaryA(){

    QString Name =QDir::currentPath() +  "/libaries/airfoil/profili/airfoil/data.txt";
    //QString Name =QDir::currentPath() +  "/libaries/UIUC/airfoil/data.txt";
    //qDebug()<<Name;
    QFile file(Name);

    int i = 0;


    if(file.open(QIODevice::ReadOnly|QIODevice::Text)){



        QTextStream stream(&file);


        while(!stream.atEnd()){


            QString line = stream.readLine();

            if(!line.isEmpty()){
                if(line.at(0).isLetter()){
                    AirfoilA[i].Name = line.left(line.length() - 4);
                    Airfoil[i].Name = line.left(line.length() - 4);
                    airfoilNameArrayA.append(line.left(line.length() - 4));
                    i++;
                }
                else{
                    QStringList parts = line.split(" ",Qt::SkipEmptyParts);

                    if(parts.size() ==2){
                        double value1 = parts[0].toDouble();
                        double value2 = parts[1].toDouble();

                        QVector<double>row;
                        row.append(value1);
                        row.append(value2);

                        airfoilArrayA[i - 1].append(row);
                        airfoilArray[i - 1].append(row);


                    }

                }
        }
    }
    listLengthA = i;
    listLength = i;
    file.close();
    }
    //newStructAirfoilData()

}
void airfoilLibary::importMyAirfoilLibaryB(){

    //QString Name =QDir::currentPath() +  "/libaries/airfoil/profili/airfoil/data.txt";
    QString Name =QDir::currentPath() +  "/libaries/airfoil/UIUC/airfoil/data.txt";
    //qDebug()<<Name;
    QFile file(Name);

    int i = 0;


    if(file.open(QIODevice::ReadOnly|QIODevice::Text)){



        QTextStream stream(&file);


        while(!stream.atEnd()){


            QString line = stream.readLine();

            if(!line.isEmpty()){
                if(line.at(0).isLetter()){
                    AirfoilB[i].Name = line.left(line.length() - 4);
                    airfoilNameArrayB.append(line.left(line.length() - 4));
                    i++;
                    //等待更新UIUC数据
                    QVector<double>row;
                    row.append(1);
                    row.append(0);

                    airfoilArrayB[i - 1].append(row);
                    //之后可删除该段
                }
                else{
                    QStringList parts = line.split(" ",Qt::SkipEmptyParts);

                    if(parts.size() ==2){
                        double value1 = parts[0].toDouble();
                        double value2 = parts[1].toDouble();

                        QVector<double>row;
                        row.append(value1);
                        row.append(value2);

                        airfoilArrayB[i - 1].append(row);


                    }

                }
            }
        }
        listLengthB = i;
        file.close();
    }



    //newStructAirfoilData()

}
void airfoilLibary::importMyAirfoilLibaryC(){
    QString path = QDir::currentPath() + "/libaries/airfoil/windAIBench/windAIBench.h5";
    openFile(path);
    airfoilCSTArrayC = readCST();

    QVector<double>airfoilX;
    for(int i = 0;i<51;i++)
        airfoilX.append(0.5 - cos(static_cast<double>(i) * M_PI / 50 ) * 0.5);
    for(int i = 0;i<listLengthC;i++){
        QVector<double>cst;
        for(int j = 9;j<18;j++){
            cst.append(airfoilCSTArrayC[i][j]);
        }
        for(int j = 0;j<9;j++){
            cst.append(airfoilCSTArrayC[i][8 - j]);
        }

        airfoilDesign designA(9);

        designA.yTe1 = airfoilCSTArrayC[i][18] / 2;
        designA.yTe2 = airfoilCSTArrayC[i][18] / 2;
        designA.buildAirfoilCurve(cst,airfoilX);
        airfoilArrayC[i] = designA.newAirfoilData;
        //designA.computeSimpleParameters(designA.newAirfoilData);

        /*
        AirfoilC[i].MaxThickness = designA.maxThickness;
        AirfoilC[i].ThicknessLoaction = designA.locationThickness;
        AirfoilC[i].MaxCamber = designA.maxCamber;
        AirfoilC[i].CamberLoaction = designA.locationCamber;
        AirfoilC[i].minRadius = designA.minRadius;
        AirfoilC[i].trailingAngle = designA.trailingAngle;
        AirfoilC[i].id = i;
        */
    }


}
void airfoilLibary::newStructAirfoilData(){
    QString Name =QDir::currentPath() +  "/libaries/airfoil/profili/airfoil/newdata.txt";
    //QString Name =QDir::currentPath() +  "/libaries/UIUC/airfoil/data.txt";
    //qDebug()<<Name;
    QFile file(Name);

    if(file.open(QIODevice::WriteOnly|QIODevice::Text)){
        QTextStream stream(&file);
        for(int i = 0;i<listLengthA;i++){

        stream<<AirfoilA[i].Name<<" ";
        QVector<QVector<double>>tmp;
        tmp = airfoilArrayA[i];
        for(int j = 0; j < tmp.length(); j++){
            stream<<tmp[j][0]<<" "<<tmp[j][1]<<" ";
        }
        stream<<"\n";
        }

    }
    file.close();
}
void airfoilLibary::writeAirfoilParameters(){
    //QString fileName = QDir::currentPath() +  "/libaries/airfoil/profili/AirfoilList.txt";
    //QString fileName = QDir::currentPath() +  "/libaries/airfoil/UIUC/AirfoilList.txt";
    QString fileName = QDir::currentPath() +  "/libaries/airfoil/windAIBench/AirfoilList.txt";

    QFile file(fileName);
    if(!fileName.isEmpty()){

        if(file.open(QIODevice::WriteOnly|QIODevice::Text)){

            for(int i = 0; i<listLengthC;i++){
                QTextStream stream(&file);
                stream<<AirfoilC[i].Name<<"    "<<AirfoilC[i].ThicknessLoaction<<"    "<<AirfoilC[i].MaxThickness<<"    "<<AirfoilC[i].CamberLoaction
                    <<"    "<<AirfoilC[i].MaxCamber<<"    "<<AirfoilC[i].minRadius<<"    "<<AirfoilC[i].trailingAngle<<"\n";
            }
            file.close();
        }
    }
}
void airfoilLibary::readAirfoilParametersA(){
    QVector<QString>name;
    QString fileName = QDir::currentPath() +  "/libaries/airfoil/profili/AirfoilList.txt";
    //QString fileName = QDir::currentPath() +  "/libaries/UIUC/AirfoilList.txt";
    QFile file(fileName);
    AirfoilParameters tmp;
    int index = 0;

    if(!fileName.isEmpty()){

        if(file.open(QIODevice::ReadOnly|QIODevice::Text)){
            QTextStream stream(&file);

            while(!stream.atEnd()){
                QString line = stream.readLine();
                QStringList values = line.split("    ",Qt::SkipEmptyParts);
                if(values.size() != 7){
                    continue;
                }

                tmp.Name = values.at(0);
                tmp.ThicknessLoaction = values.at(1).toDouble();
                tmp.MaxThickness = values.at(2).toDouble();
                tmp.CamberLoaction = values.at(3).toDouble();
                tmp.MaxCamber = values.at(4).toDouble();
                tmp.minRadius = values.at(5).toDouble();
                tmp.trailingAngle = values.at(6).toDouble();
                tmp.id = index;
                AirfoilA[index] = tmp;

                index++;



                }
                //Airfoil[i].Name = line;
            }
            file.close();


        }



    for(int i = 0; i<listLengthA;i++)
            name.append(AirfoilA[i].Name);


    QStringList stringList = QStringList::fromVector(name);
    listModel = new QStringListModel(stringList);
    listView->setModel(listModel);
    connect(listView->selectionModel(),&QItemSelectionModel::selectionChanged,this,&airfoilLibary::drawAirfoil);
}

void airfoilLibary::readAirfoilParametersC(){
    //QVector<QString>name;
    //QString fileName = QDir::currentPath() +  "/libaries/profili/AirfoilList.txt";
    QString fileName = QDir::currentPath() +  "/libaries/airfoil/windAIBench/AirfoilList.txt";
    QFile file(fileName);
    AirfoilParameters tmp;
    int index = 0;

    if(!fileName.isEmpty()){

        if(file.open(QIODevice::ReadOnly|QIODevice::Text)){
            QTextStream stream(&file);

            while(!stream.atEnd()){
                QString line = stream.readLine();
                QStringList values = line.split("    ",Qt::SkipEmptyParts);
                if(values.size() != 7){
                    continue;

                }

                tmp.Name = values.at(0);
                tmp.ThicknessLoaction = values.at(1).toDouble();
                tmp.MaxThickness = values.at(2).toDouble();
                tmp.CamberLoaction = values.at(3).toDouble();
                tmp.MaxCamber = values.at(4).toDouble();
                tmp.minRadius = values.at(5).toDouble();
                tmp.trailingAngle = values.at(6).toDouble();
                tmp.id = index;
                AirfoilC[index] = tmp;
                index++;
            }
            //Airfoil[i].Name = line;
        }
        file.close();
    }
}

void airfoilLibary::readAirfoilParametersB(){
    //QVector<QString>name;
    //QString fileName = QDir::currentPath() +  "/libaries/profili/AirfoilList.txt";
    QString fileName = QDir::currentPath() +  "/libaries/airfoil/UIUC/AirfoilList.txt";
    QFile file(fileName);
    AirfoilParameters tmp;
    int index = 0;

    if(!fileName.isEmpty()){

        if(file.open(QIODevice::ReadOnly|QIODevice::Text)){
            QTextStream stream(&file);

            while(!stream.atEnd()){
                QString line = stream.readLine();
                QStringList values = line.split("    ",Qt::SkipEmptyParts);
                if(values.size() != 7){
                    continue;

                }

                tmp.Name = values.at(0);
                tmp.ThicknessLoaction = values.at(1).toDouble();
                tmp.MaxThickness = values.at(2).toDouble();
                tmp.CamberLoaction = values.at(3).toDouble();
                tmp.MaxCamber = values.at(4).toDouble();
                tmp.minRadius = values.at(5).toDouble();
                tmp.trailingAngle = values.at(6).toDouble();
                tmp.id = index;
                AirfoilB[index] = tmp;
                index++;
            }
            //Airfoil[i].Name = line;
        }
        file.close();
    }
}
void airfoilLibary::drawAirfoil(const QItemSelection &selected,const QItemSelection & /*deselected*/){
    if(series->count() != 0){              //判断series是否被定义
        series->clear();
    }



    QModelIndexList indexes = selected.indexes();

    int index = indexes.at(0).row();

    if(modelType == AIRFOIL_PROFILI){
        if(isSort){
            // 获取当前选中的项的索引列表
            QModelIndexList selectedIndexes = listView->selectionModel()->selectedIndexes();

            // 确保索引列表不为空
            if (!selectedIndexes.isEmpty()) {
                // 获取第一个选中的索引
                const QModelIndex selectedIndex = selectedIndexes.first();
                // 从模型中获取对应的文本
                QString selectedText = listModel->data(selectedIndex, Qt::DisplayRole).toString();
                listChose = getBothNameIndexA(selectedText);
            }
        }else{
            if(searchType){
                listChose = AirfoilCopyA[index].id;

            }
            else{
                listChose = AirfoilA[index].id;
            }
        }
        //airfoilDesign design(cstNUM);

        for(int i = 0; i < airfoilArrayA[listChose].length(); i++){

            series->append(airfoilArrayA[listChose][i][0],airfoilArrayA[listChose][i][1]);
            //series->append(design.newAirfoilData[i][0],design.newAirfoilData[i][1]);
        }

        QString tmp1 = QString("最大厚度: %1%     最大厚度位置: %2%    ").arg(AirfoilA[listChose].MaxThickness * 100,0,'f',1).arg(AirfoilA[listChose].ThicknessLoaction * 100,0,'f',1);
        QString tmp2 = QString("最大弯度: %1%     最大弯度位置: %2%    ").arg(AirfoilA[listChose].MaxCamber * 100,0,'f',1).arg(AirfoilA[listChose].CamberLoaction * 100,0,'f',1);
        QString tmp3 = QString("前缘半径: %1%         尾缘夹角: %2").arg(AirfoilA[listChose].minRadius * 100,0,'f',1).arg(AirfoilA[listChose].trailingAngle,0,'f',2);
        QString tmp4;
        if(isSort)
            tmp4 = "升力系数:" + QString::number(resultArrayA[index][1],'f',2) + "   升阻比:" + QString::number(resultArrayA[index][2],'f',1) + "   力矩系数:" + QString::number(resultArrayA[index][3],'f',3);
        QString pointFstring = tmp1 + "\n" +  tmp2 + "\n" + tmp3 + "\n" + tmp4;
        mouseLocitionLibary->setText(pointFstring);

    }else if(modelType == AIRFOIL_UIUC){

        if(searchType){
            listChose = AirfoilCopyB[index].id;
        }
        else{
            listChose = AirfoilB[index].id;
        }

        for(int i = 0; i < airfoilArrayB[listChose].length(); i++)
             series->append(airfoilArrayB[listChose][i][0],airfoilArrayB[listChose][i][1]);

        QString tmp1 = QString("最大厚度: %1%     最大厚度位置: %2%    ").arg(AirfoilB[listChose].MaxThickness * 100,0,'f',1).arg(AirfoilB[listChose].ThicknessLoaction * 100,0,'f',1);
        QString tmp2 = QString("最大弯度: %1%     最大弯度位置: %2%    ").arg(AirfoilB[listChose].MaxCamber * 100,0,'f',1).arg(AirfoilB[listChose].CamberLoaction * 100,0,'f',1);
        QString tmp3 = QString("前缘半径: %1%         尾缘夹角: %2").arg(AirfoilB[listChose].minRadius * 100,0,'f',1).arg(AirfoilB[listChose].trailingAngle,0,'f',2);
        QString tmp4;
        if(isSort)
            tmp4 = "升力系数:" + QString::number(resultArrayB[listChose][1],'f',2) + "   升阻比:" + QString::number(resultArrayB[listChose][2],'f',1) + "   力矩系数:" + QString::number(resultArrayB[listChose][3],'f',3);
        QString pointFstring = tmp1 + "\n" +  tmp2 + "\n" + tmp3 + "\n" + tmp4;
        mouseLocitionLibary->setText(pointFstring);

    }else if(modelType == AIRFOIL_WINDAIBENCH){
        if(searchType){
            listChose = AirfoilCopyC[index].id;
        }
        else{
            listChose = AirfoilC[index].id;
        }

        for(int i = 0; i < airfoilArrayC[listChose].length(); i++)
             series->append(airfoilArrayC[listChose][i][0],airfoilArrayC[listChose][i][1]);

        QString tmp1 = QString("最大厚度: %1%     最大厚度位置: %2%    ").arg(AirfoilC[listChose].MaxThickness * 100,0,'f',1).arg(AirfoilC[listChose].ThicknessLoaction * 100,0,'f',1);
        QString tmp2 = QString("最大弯度: %1%     最大弯度位置: %2%    ").arg(AirfoilC[listChose].MaxCamber * 100,0,'f',1).arg(AirfoilC[listChose].CamberLoaction * 100,0,'f',1);
        QString tmp3 = QString("前缘半径: %1%         尾缘夹角: %2").arg(AirfoilC[listChose].minRadius * 100,0,'f',1).arg(AirfoilC[listChose].trailingAngle,0,'f',2);
        QString pointFstring = tmp1 + "\n" +  tmp2 + "\n" + tmp3 + "\n";
        mouseLocitionLibary->setText(pointFstring);



    }
    updateTextPosition();

}

void airfoilLibary::updateTextPosition()
{
    if (!mouseLocitionLibary) return;

    // 获取图表视图的中心点（视图坐标）
    QRectF viewRect = graphview->viewport()->rect();
    QPointF viewCenter = viewRect.center();

    // 转换为场景坐标
    QPointF sceneCenter = graphview->mapToScene(viewCenter.toPoint());

    // 转换为图表坐标
    QPointF chartCenter = graphview->chart()->mapFromScene(sceneCenter);

    // 设置文本位置（考虑文本自身大小）
    QRectF textRect = mouseLocitionLibary->boundingRect();
    mouseLocitionLibary->setPos(chartCenter.x() - textRect.width()/2,
                              200);
}

void airfoilLibary::solveOtherParameters(){
    airfoilDesign design(cstNUM);

    for(int i = 0; i < listLengthA;i++){
        // solver.computeOtherParameters(airfoilArray[i]);
        design.computeSimpleParameters(airfoilArrayA[i]);
        AirfoilA[i].MaxThickness = design.maxThickness;
        AirfoilA[i].ThicknessLoaction = design.locationThickness;
        AirfoilA[i].MaxCamber = design.maxCamber;
        AirfoilA[i].CamberLoaction = design.locationCamber;
        AirfoilA[i].minRadius = design.minRadius;
        AirfoilA[i].trailingAngle = design.trailingAngle;


    }
}
void airfoilLibary::searchAirfoil(){
    if(modelType == AIRFOIL_PROFILI)
        searchAirfoilNameA();
    else if(modelType == AIRFOIL_UIUC)
        searchAirfoilNameB();
    else if(modelType == AIRFOIL_WINDAIBENCH)
        searchAirfoilNameC();
}
void airfoilLibary::searchAirfoilNameA(){
    int j = 0;
    QVector<QString>nameArray;
    QString name = searchNameEdit->text();

    if(name.isEmpty()){


        for(int i = 0; i<listLengthA;i++)
            nameArray.append(AirfoilA[i].Name);
        searchType = false;


    }else{
        for(int i = 0; i < listLengthA;i++){
            QString nameLower = AirfoilA[i].Name.toLower();
            if(nameLower.contains(name.toLower())){
                AirfoilCopyA[j] = AirfoilA[i];
                j++;
            }
        }
        for(int i = 0; i<j;i++)
            nameArray.append(AirfoilCopyA[i].Name);
        
        searchType = true;
    }

    copyLength = j;



    QStringList stringList = QStringList::fromVector(nameArray);
    listModel->setStringList(stringList);

}
void airfoilLibary::searchAirfoilNameB(){
    int j = 0;
    QVector<QString>nameArray;
    QString name = searchNameEdit->text();



    if(name.isEmpty()){


        for(int i = 0; i<listLengthB;i++)
            nameArray.append(AirfoilB[i].Name);
        searchType = false;


    }else{
        for(int i = 0; i < listLengthB;i++){
            QString nameLower = AirfoilB[i].Name.toLower();
            if(nameLower.contains(name.toLower())){
                AirfoilCopyB[j] = AirfoilB[i];
                j++;
            }
        }
        for(int i = 0; i<j;i++)
            nameArray.append(AirfoilCopyB[i].Name);

        searchType = true;
    }


    copyLength = j;



    QStringList stringList = QStringList::fromVector(nameArray);
    listModel->setStringList(stringList);

}
void airfoilLibary::searchAirfoilNameC(){
    int j = 0;
    QVector<QString>nameArray;
    QString name = searchNameEdit->text();



    if(name.isEmpty()){


        for(int i = 0; i<listLengthC;i++)
            nameArray.append(AirfoilC[i].Name);
        searchType = false;


    }else{
        for(int i = 0; i < listLengthC;i++){
            QString nameLower = AirfoilC[i].Name.toLower();
            if(nameLower.contains(name.toLower())){
                AirfoilCopyC[j] = AirfoilC[i];
                j++;
            }
        }
        for(int i = 0; i<j;i++)
            nameArray.append(AirfoilCopyC[i].Name);

        searchType = true;
    }


    copyLength = j;



    QStringList stringList = QStringList::fromVector(nameArray);
    listModel->setStringList(stringList);

}
void airfoilLibary::sortMaxK(){
    if(!resultArrayA.isEmpty()){
        sortData(resultArrayA,2,false);

        QVector<QString>nameArray;
        for(int i = 0;i<resultArrayA.length();i++){
            int index = resultArrayA[i][0];
            nameArray.append(resultNameA[index]);
        }
        QStringList stringList = QStringList::fromVector(nameArray);
        listModel->setStringList(stringList);
    }
}
void airfoilLibary::sortMaxCl(){
    if(!resultArrayA.isEmpty()){

        sortData(resultArrayA,1,false);

        QVector<QString>nameArray;
        for(int i = 0;i<resultArrayA.length();i++){
            int index = resultArrayA[i][0];
            nameArray.append(resultNameA[index]);
        }
        QStringList stringList = QStringList::fromVector(nameArray);
        listModel->setStringList(stringList);
    }
}
void airfoilLibary::sortMinCm(){
    if(!resultArrayA.isEmpty()){

        sortData(resultArrayA,3,false);

        QVector<QString>nameArray;
        for(int i = 0;i<resultArrayA.length();i++){
            int index = resultArrayA[i][0];
            nameArray.append(resultNameA[index]);
        }
        QStringList stringList = QStringList::fromVector(nameArray);
        listModel->setStringList(stringList);
    }
}
void airfoilLibary::changeAirfoilList(const int index){
    switch (index) {
    case 0:
        isSort = false;
        searchAirfoilNameA();
        break;
    case 1:
        isSort = true;
        sortMaxCl();
        break;
    case 2:
        isSort = true;
        sortMaxK();
        break;
    case 3:
        isSort = true;
        sortMinCm();
        break;
    default:
        break;
    }
}
int airfoilLibary::getBothNameIndexA(const QString name){
    int index = airfoilNameArrayA.indexOf(name);
    if(index >= 0)
        return index;
    else
        return 0;
}
int airfoilLibary::getBothNameIndexB(const QString name){
    int index = airfoilNameArrayB.indexOf(name);
    if(index >= 0)
        return index;
    else
        return 0;
}
void airfoilLibary::sortData(QVector<QVector<double>>& rows, int column, bool descending) {
    // 创建比较函数
    auto compare = [column, descending](const QVector<double>& a, const QVector<double>& b) {
        if (descending) {
            return a[column] > b[column];
        } else {
            return a[column] < b[column];
        }
    };

    // 冒泡排序算法
    int n = rows.size();
    for (int i = 0; i < n - 1; ++i) {
        for (int j = 0; j < n - i - 1; ++j) {
            if (compare(rows[j], rows[j + 1])) {
                QVector<double> temp = rows[j];
                rows[j] = rows[j + 1];
                rows[j + 1] = temp;
            }
        }
    }
}
QVector<double> airfoilLibary::read1DData(const QString &datasetPath)
{
    if(!hdf5File) return {};
    QVector<double> values;
    try {
        DataSet ds = hdf5File->openDataSet(datasetPath.toStdString());
        DataSpace space = ds.getSpace();
        hsize_t dims[1], maxdims[1];
        int rank = space.getSimpleExtentDims(dims,maxdims);
        if(rank!=1) return {};

        std::vector<double> buffer(dims[0]);
        ds.read(buffer.data(), PredType::NATIVE_DOUBLE);

        values.resize(dims[0]);
        for(hsize_t i=0;i<dims[0];++i)
            values[i] = buffer[i];
    } catch(H5::Exception &e){
        qDebug() << "Failed to read 1D dataset:" << datasetPath
                 << "Error:" << e.getCDetailMsg();
    }
    return values;
}
bool airfoilLibary::openFile(const QString &filePath)
{

    try {
        std::string path = filePath.toStdString();

        hdf5File = std::make_unique<H5::H5File>(
            path,
            H5F_ACC_RDONLY
        );

        qDebug() << "Successfully opened HDF5 file:" << filePath;
        return true;
    }
    catch (H5::FileIException &e) {
        qDebug() << "File open error:" << e.getCDetailMsg();
    }
    catch (H5::Exception &e) {
        qDebug() << "HDF5 error:" << e.getCDetailMsg();
    }
    catch (std::exception &e) {
        qDebug() << "Std exception:" << e.what();
    }

    return false;
}
// ------------------ 读取二维几何参数 ------------------
QVector<QVector<double>> airfoilLibary::readCST()
{
    QVector<QVector<double>> rows;
    if (!hdf5File) return rows;

    try {
        H5::DataSet ds = hdf5File->openDataSet("/shape/cst");
        H5::DataSpace space = ds.getSpace();

        hsize_t dims[2];
        if (space.getSimpleExtentDims(dims, nullptr) != 2)
            return rows;

        const int nAirfoils   = static_cast<int>(dims[0]); // 1830
        const int nParams = static_cast<int>(dims[1]); // 19
        listLengthC = nAirfoils;

        std::vector<double> raw(nParams * nAirfoils);
        ds.read(raw.data(), H5::PredType::NATIVE_DOUBLE);


        rows.resize(nAirfoils); // 1830
        for(int p = 0; p < nAirfoils; ++p){
            rows[p].resize(nParams); // 19
            for(int a = 0; a < nParams; ++a){
                rows[p][a] = raw[p * nParams + a]; // ✅ 正确
            }
        }

        // -------- 翼型名称 --------
        //airfoilNameArrayC.reserve(nAirfoils);
        for (int a = 0; a < nAirfoils; ++a) {
            const QString airfoilName = QString("windAI%1").arg(a + 1, 4, 10, QChar('0'));
            AirfoilC[a].Name = airfoilName;
            airfoilNameArrayC.append(airfoilName);
        }

    }
    catch (H5::Exception &e) {
        qDebug() << "readCST failed:" << e.getCDetailMsg();
    }


    return rows;
}




airfoilLibary::~airfoilLibary(){



    AirfoilA.clear();
    AirfoilA.squeeze();
    AirfoilB.clear();
    AirfoilB.squeeze();



    // delete gridlayout;


    // delete searchNameButton;
    // delete searchNameEdit;
    // delete searchParametersButton;
    // delete searchSimilarButton;
    // delete printfAirfoilButton;
    // delete sortModelCombobox;

    // //QWidget libaryWidget;
    // delete listView;
    // delete graphChart;
    // delete graphview;
    // delete series;

    // delete xaxis;
    // delete yaxis;

    //delete libaryWidget;



}
