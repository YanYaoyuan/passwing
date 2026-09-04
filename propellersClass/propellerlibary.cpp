#if defined(_MSC_VER) && (_MSC_VER >= 1600)
# pragma execution_character_set("utf-8")
#endif


#include "propellerlibary.h"
#include <QDir>
#include <QFile>
#include <QTextStream>
#include <qDebug>

propellerLibary::propellerLibary() {
    initialPropLibaries();
    readPropAName();
    readPropAPicture();
    readPropGeometey();
}
void propellerLibary::readPropGeometey(){
    QString name = QDir::currentPath() + "/libaries/propeller/apc/geometry/propData";
    QDir directory(name);

    QStringList entries = directory.entryList(QDir::AllEntries | QDir::NoDotAndDotDot);

    for(const QString& entry:entries){
        propGeoNameArray.append(entry.chopped(4));
    }

    QRegularExpression regExp("\\s{3,}");  // 匹配三个或更多空格


    for(int i = 0;i<propGeoNameArray.length();i++){

        QString fileName = name + "/" + propGeoNameArray[i] + ".txt";
        QFile file(fileName);
        if(file.open(QIODevice::ReadOnly|QIODevice::Text)){
            QVector<QVector<double>>tmp1;
            QTextStream stream(&file);
            QStringList airfoilType = stream.readLine().split(regExp, Qt::SkipEmptyParts);
            if(airfoilType.size() >= 2)
                propAirfoilNameArray.append(airfoilType);




            while (!stream.atEnd()) {
                QStringList values = stream.readLine().split(QRegularExpression("\\s+"), Qt::SkipEmptyParts);
                if (values.size() == 4) {  // 假设每行有13个数据
                    QVector<double> tmp2;  // 用于存储当前行的13个数据
                    for (const QString& value : values) {
                        tmp2.append(value.toDouble());
                    }
                    tmp1.append(tmp2);  // 添加当前行的数据到tmp1
                }
            }
            file.close();
            propGeoArray.append(tmp1);
        }
    }    
}
void propellerLibary::initialPropLibaries(){
    propLibaryWidget = new QWidget();
    libaryGLayout = new QGridLayout(propLibaryWidget);
    libaryHLayout = new QHBoxLayout();
    listView = new QListView;
    StringListModelA = new QStringListModel;
    StringListModelB = new QStringListModel;
    libaryTypeCombobox = new QComboBox(propLibaryWidget);
    propDisplayLabel = new QLabel(propLibaryWidget);
    propDiameterLabel = new QLabel(propLibaryWidget);
    propPitchLabel = new QLabel(propLibaryWidget);
    propDiameterEdit = new QLineEdit(propLibaryWidget);
    propPitchEdit = new QLineEdit(propLibaryWidget);
    searchPropButton = new QPushButton(propLibaryWidget);
    addPropButton = new QPushButton(propLibaryWidget);
    cancelPropButton = new QPushButton(propLibaryWidget);

    libaryTypeCombobox->addItem("apc双叶桨");
    libaryTypeCombobox->addItem("apc三叶桨");
    propDisplayLabel->setText(" ");
    propDiameterLabel->setText("直径（英寸）");
    propPitchLabel->setText("螺距（英寸）");
    propDiameterEdit->setText("0");
    propPitchEdit->setText("0");
    searchPropButton->setText("搜索");
    addPropButton->setText("添加");
    cancelPropButton->setText("取消");

    propDiameterLabel->setAlignment(Qt::AlignHCenter);
    propPitchLabel->setAlignment(Qt::AlignHCenter);



    libaryGLayout->addWidget(libaryTypeCombobox,0,0,1,3);
    libaryGLayout->addWidget(listView,1,0,5,3);
    libaryGLayout->addWidget(propDisplayLabel,0,3,4,3);
    libaryGLayout->addWidget(propDiameterLabel,4,3,1,1);
    libaryGLayout->addWidget(propPitchLabel,4,4,1,1);
    libaryGLayout->addWidget(propDiameterEdit,5,3,1,1);
    libaryGLayout->addWidget(propPitchEdit,5,4,1,1);
    libaryGLayout->addWidget(searchPropButton,5,5,1,1);

    libaryHLayout->addWidget(addPropButton);
    libaryHLayout->addWidget(cancelPropButton);

    libaryGLayout->addLayout(libaryHLayout,6,0,1,7);
    propLibaryWidget->setLayout(libaryGLayout);

    propDisplayLabel->setScaledContents(true);
    propDisplayLabel->setFixedSize(QSize(450,400));
    propLibaryWidget->setFixedSize(QSize(700,500));

    connect(searchPropButton,&QPushButton::clicked,this,&propellerLibary::searchProp);
}

void propellerLibary::initialPropModel(){

}
void propellerLibary::searchProp(){
    QVector<QString>nameArray;
    QString name1 = propDiameterEdit->text();
    QString name2 = propPitchEdit->text();
    QString name = name1 + "x" + name2;

    if(name1 == " "||name1 =="0"||name2 ==" "||name2 =="0"){
        searchType = false;
        QStringList stringList = QStringList::fromVector(propNameA);
        StringListModelA->setStringList(stringList);
    }else{
        for(int i = 0;i<propNameA.length();i++){
            if(propNameA[i].contains(name)){
                nameArray.append(propNameA[i]);

            }
        }
        searchType = true;
        propNameCopy = nameArray;
        QStringList stringList = QStringList::fromVector(propNameCopy);
        StringListModelA->setStringList(stringList);
    }
}
void propellerLibary::readPropAName(){
    QString name = QDir::currentPath() + "/libaries/propeller/apc/data/PERFILES2";
    QDir directory(name);
    QStringList entries = directory.entryList(QDir::AllEntries | QDir::NoDotAndDotDot);

    for(const QString& entry:entries){
        propNameA.append(entry.chopped(4));
    }

    QStringList stringList = QStringList::fromVector(propNameA);
    StringListModelA->setStringList(stringList);
    listView->setModel(StringListModelA);
    connect(listView->selectionModel(),&QItemSelectionModel::selectionChanged,this,&propellerLibary::changePropPicture);
}
void propellerLibary::readPropBName(){


}
void propellerLibary::readPropAPicture(){
    QString Name = QDir::currentPath() + "/libaries/propeller/apc/geometry";
    QDir directory(Name);
    QStringList entries = directory.entryList(QDir::AllEntries | QDir::NoDotAndDotDot);

    for(const QString& entry:entries){
        propPictureNameA.append(entry.chopped(4));
    }
}
void propellerLibary::changePropPicture(const QItemSelection &selected,const QItemSelection &deselected){
    QModelIndexList indexes = selected.indexes();
    int index = indexes.at(0).row();
    int findIndex = findPictureId(index);
    QString buttonIcon;
    if(findIndex != -1){
        buttonIcon = QDir::currentPath() + "/libaries/propeller/apc/geometry/" + propPictureNameA[findIndex] + ".png";
    }else{
        buttonIcon = QDir::currentPath() + "/libaries/propeller/apc/geometry/notFind.png";
    }
    QPixmap image(buttonIcon);
    if(!searchType)
        choicePropName = propNameA[index];
    else
        choicePropName = propNameCopy[index];
    propDisplayLabel->setPixmap(image);
    choiceIndex = index;
}
int propellerLibary::findPictureId(const int index){
    int i = 0;

    for(const QString &name:propPictureNameA){
        if(name == propNameA[index])
            return i;
        i++;
    }
    return -1;
}
// Helper function to find the three nearest points
void propellerLibary::findNearestPoints(const QVector<QVector<double>>& data, double target_x, QVector<QVector<double>>& nearestPoints) {
    // Vectors to store distances and indices
       QVector<double> distances(data.size());
       QVector<int> indices(data.size());

       for (int i = 0; i < data.size(); ++i) {
           double x_value = data[i][0]; // Assume the first column is x
           distances[i] = std::abs(x_value - target_x);
           indices[i] = i;
       }

       // Manual selection sort to find the three smallest distances
       for (int i = 0; i < 3; ++i) {
           int min_index = i;
           for (int j = i + 1; j < data.size(); ++j) {
               if (distances[j] < distances[min_index]) {
                   min_index = j;
               }
           }
           // Instead of swap, do manual value exchange
           double temp_distance = distances[i];
           distances[i] = distances[min_index];
           distances[min_index] = temp_distance;

           int temp_index = indices[i];
           indices[i] = indices[min_index];
           indices[min_index] = temp_index;
       }

       // Collect the three nearest points
       for (int i = 0; i < 3; ++i) {
           nearestPoints.append(data[indices[i]]);
       }
}

// Function to perform Lagrange interpolation using three points for a specific column
double propellerLibary::lagrangeInterpolation(const QVector<QVector<double>>& nearestPoints, double target_x, int column) {
    double result = 0.0;

    for (int i = 0; i < 3; ++i) {
        double xi = nearestPoints[i][0]; // x value of point i
        double yi = nearestPoints[i][column]; // y value from the specified column

        double term = yi;
        for (int j = 0; j < 3; ++j) {
            if (i != j) {
                double xj = nearestPoints[j][0];
                term *= (target_x - xj) / (xi - xj);
            }
        }
        result += term;
    }

    return result;
}

// Main function to perform three-point interpolation for any column (handles out-of-bounds as well)
double propellerLibary::threePointInterpolation(const QVector<QVector<double>>& data, double target_x, int column) {
    QVector<QVector<double>> nearestPoints;

    // Find nearest points for interpolation
    findNearestPoints(data, target_x, nearestPoints);

    // Perform Lagrange interpolation using the three nearest points for the specified column
    return lagrangeInterpolation(nearestPoints, target_x, column);
}













