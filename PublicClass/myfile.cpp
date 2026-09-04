#include "myfile.h"
#include <QTextStream>
#include <QFileDialog>
#include <QDateTime>
#include <QMessageBox>
#include <QDebug>

myFile::myFile(){
    //readInterData("CLARK Y");

}
void myFile::readFile(){

    if(!airfoilArray.isEmpty())
        clearData();
    QString fileName = QFileDialog::getOpenFileName(nullptr, "Open Data", "", "Text Files (*.xml)");
    if (!fileName.isEmpty()) {
        QFile file(fileName);
        if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QTextStream stream(&file);
            stream.setCodec("UTF-8");

            QString title = stream.readLine();
            QStringList titleParts = title.split("  ",Qt::SkipEmptyParts);
            if(titleParts[0] != "project"){
                QMessageBox::information(nullptr, "警告", "数据损坏");
                return;
            }


            QString numberString1 = stream.readLine();
            QString numberString2 = stream.readLine();
            QString numberString3 = stream.readLine();
            QString numberString4 = stream.readLine();
            QString numberString5 = stream.readLine();


            QStringList parts1 = numberString1.split("  ",Qt::SkipEmptyParts);
            QStringList parts2 = numberString2.split("  ",Qt::SkipEmptyParts);
            QStringList parts3 = numberString3.split("  ",Qt::SkipEmptyParts);
            QStringList parts4 = numberString4.split("  ",Qt::SkipEmptyParts);
            QStringList parts5 = numberString5.split("  ",Qt::SkipEmptyParts);


            int airfoilNum = parts1[1].toInt();
            int wingNum = parts2[1].toInt();
            int tailNum = parts3[1].toInt();
            int propellerNum = parts4[1].toInt();
            int airplaneNum = parts5[1].toInt();




            //
            stream.readLine();

            if(airfoilNum != 0){
                QString line = stream .readLine();//
                for(int i = 0;i<airfoilNum;i++){
                    QVector<QVector<double>>tmp1;

                    nameArray.append(line);
                    line = stream.readLine();
                    while(!line.at(0).isLetter()){
                        QStringList parts = line.split("  ",Qt::SkipEmptyParts);

                        if(parts.size() == 2){
                            double value1 = parts[0].toDouble();
                            double value2 = parts[1].toDouble();

                            QVector<double>row;
                            row.append(value1);
                            row.append(value2);
                            tmp1.append(row);
                    }
                        line = stream.readLine();
                 }
                 airfoilArray.append(tmp1);
                }
            }


            //
            if(wingNum != 0){
                stream.readLine();
                for(int i = 0;i<wingNum;i++){
                    wingDefinition tmp2;
                    tmp2.name = stream.readLine();

                    QStringList listTmp[9];
                    for(int j = 0;j<8;j++){
                        QString tmp = stream.readLine();
                        listTmp[j] = tmp.split("  ",Qt::SkipEmptyParts);
                    }

                    int len = listTmp[1].size();
                    for(int j = 0;j<len;j++){
                        tmp2.airfoilNameArray.append(listTmp[0][j]);
                        tmp2.spanW.append(listTmp[1][j].toDouble());
                        tmp2.chordLengthW.append(listTmp[2][j].toDouble());
                        tmp2.offsetLengthW.append(listTmp[3][j].toDouble());
                        tmp2.twistAngleW.append(listTmp[4][j].toDouble());
                        tmp2.dihedralAngleW.append(listTmp[5][j].toDouble());
                        tmp2.gridV.append(listTmp[6][j].toInt());
                        tmp2.gridU.append(listTmp[7][j].toInt());
                    }
                    wingArray.append(tmp2);
                }
                stream.readLine();
            }

            if(tailNum != 0){
                stream.readLine();
                for(int i = 0;i<tailNum;i++){
                    wingDefinition tmp3;
                    tmp3.name = stream.readLine();

                    QStringList listTmp[10];
                    for(int j = 0;j<9;j++){
                        QString tmp = stream.readLine();
                        listTmp[j] = tmp.split("  ",Qt::SkipEmptyParts);


                    }

                    int len = listTmp[1].size();
                    for(int j = 0;j<len;j++){
                        tmp3.airfoilNameArray.append(listTmp[0][j]);
                        tmp3.spanW.append(listTmp[1][j].toDouble());
                        tmp3.chordLengthW.append(listTmp[2][j].toDouble());
                        tmp3.offsetLengthW.append(listTmp[3][j].toDouble());
                        tmp3.twistAngleW.append(listTmp[4][j].toDouble());
                        tmp3.dihedralAngleW.append(listTmp[5][j].toDouble());
                        tmp3.gridV.append(listTmp[6][j].toInt());
                        tmp3.gridU.append(listTmp[7][j].toInt());
                    }
                    tmp3.isSymmetry = listTmp[8][1].toInt();
                    tailArray.append(tmp3);
                }

                stream.readLine();
            }


            if(propellerNum != 0){
                stream.readLine();
                for(int i = 0;i<propellerNum;i++){
                    wingDefinition tmp4;
                    tmp4.name = stream.readLine();

                    QStringList listTmp[10];
                    for(int j = 0;j<9;j++){
                        QString tmp = stream.readLine();
                        listTmp[j] = tmp.split("  ",Qt::SkipEmptyParts);



                    }

                    int len = listTmp[1].size();
                    for(int j = 0;j<len;j++){
                        tmp4.airfoilNameArray.append(listTmp[0][j]);
                        tmp4.spanW.append(listTmp[1][j].toDouble());
                        tmp4.chordLengthW.append(listTmp[2][j].toDouble());
                        tmp4.offsetLengthW.append(listTmp[3][j].toDouble());
                        tmp4.twistAngleW.append(listTmp[4][j].toDouble());
                        tmp4.dihedralAngleW.append(listTmp[5][j].toDouble());
                        tmp4.gridV.append(listTmp[6][j].toInt());
                        tmp4.gridU.append(listTmp[7][j].toInt());
                    }
                    tmp4.num = listTmp[8][1].toInt();
                    propellerArray.append(tmp4);
                }

                stream.readLine();
            }

            if(airplaneNum != 0){
                stream.readLine();
                for(int i = 0;i<airplaneNum;i++){
                    airplaneDefinition tmp5;
                    QStringList listTmp[6];
                    for(int j = 0;j<5;j++){
                        QString tmp = stream.readLine();
                        listTmp[j] = tmp.split("  ",Qt::SkipEmptyParts);



                    }
                    int len = listTmp[1].size();
                    for(int j = 0;j<len;j++){
                        tmp5.compoentIndex[j] = listTmp[0][j].toInt();
                        tmp5.xLocation[j] = listTmp[1][j].toDouble();
                        tmp5.aAlpha[j] = listTmp[2][j].toDouble();
                        tmp5.zLocation[j] = listTmp[3][j].toDouble();
                        tmp5.isChoice[j] = listTmp[4][j].toInt();
                    }

                    tmp5.name = stream.readLine();
                    airplaneArray.append(tmp5);

                }
                stream.readLine();

            }

        }
        file.close();
    }
}
void myFile::writeFile(){
    QString fileName = QFileDialog::getSaveFileName(nullptr, "Save Data", "", "Text Files (*.xml)");
    if (!fileName.isEmpty()) {
        QFile file(fileName);
        if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QTextStream stream(&file);
            stream.setCodec("UTF-8");
            QString dateTimeString = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss");

            stream<<"project"<<"  "<<dateTimeString<<"\n";
            stream<<"airfoil  "<<airfoilArray.length()<<"\n";
            stream<<"wing  "<<wingArray.length()<<"\n";
            stream<<"tail  "<<tailArray.length()<<"\n";
            stream<<"propeller  "<<propellerArray.length()<<"\n";
            stream<<"airplane  "<<airplaneArray.length()<<"\n";

            if(!airfoilArray.isEmpty()){
                stream<<"airfoilStart"<<"\n";
                for(int i = 0;i<airfoilArray.length();i++){
                    stream<<nameArray[i]<<"\n";
                    for(int j = 0;j<airfoilArray[i].length();j++){
                        stream<<airfoilArray[i][j][0]<<"  "<<airfoilArray[i][j][1]<<" \n";
                    }
                }
                stream<<"airfoilEnd"<<"\n";
            }

            if(!wingArray.isEmpty()){
                stream<<"wingStart"<<"\n";
                for(int i = 0;i<wingArray.length();i++){
                    int length = wingArray[i].chordLengthW.length();
                    stream<<wingArray[i].name<<"\n";

                    for(int j = 0;j<length;j++){
                        stream<<wingArray[i].airfoilNameArray[j]<<"  ";
                    }
                    stream<<"\n";


                    for(int j = 0;j<length;j++){
                        stream<<wingArray[i].spanW[j]<<"  ";
                    }
                    stream<<"\n";

                    for(int j = 0;j<length;j++){
                        stream<<wingArray[i].chordLengthW[j]<<"  ";
                    }
                    stream<<"\n";

                    for(int j = 0;j<length;j++){
                        stream<<wingArray[i].offsetLengthW[j]<<"  ";
                    }
                    stream<<"\n";

                    for(int j = 0;j<length;j++){
                        stream<<wingArray[i].twistAngleW[j]<<"  ";
                    }
                    stream<<"\n";

                    for(int j = 0;j<length;j++){
                        stream<<wingArray[i].dihedralAngleW[j]<<"  ";
                    }
                    stream<<"\n";

                    for(int j = 0;j<length;j++){
                        stream<<wingArray[i].gridV[j]<<"  ";
                    }
                    stream<<"\n";

                    for(int j = 0;j<length;j++){
                        stream<<wingArray[i].gridU[j]<<"  ";
                    }
                    stream<<"\n";


                }
                stream<<"wingEnd"<<"\n";
            }

            if(!tailArray.isEmpty()){
                stream<<"tailStart"<<"\n";
                for(int i = 0;i<tailArray.length();i++){
                    int length = tailArray[i].chordLengthW.length();
                    stream<<tailArray[i].name<<"\n";

                    for(int j = 0;j<length;j++){
                        stream<<tailArray[i].airfoilNameArray[j]<<"  ";
                    }
                    stream<<"\n";


                    for(int j = 0;j<length;j++){
                        stream<<tailArray[i].spanW[j]<<"  ";
                    }
                    stream<<"\n";

                    for(int j = 0;j<length;j++){
                        stream<<tailArray[i].chordLengthW[j]<<"  ";
                    }
                    stream<<"\n";

                    for(int j = 0;j<length;j++){
                        stream<<tailArray[i].offsetLengthW[j]<<"  ";
                    }
                    stream<<"\n";

                    for(int j = 0;j<length;j++){
                        stream<<tailArray[i].twistAngleW[j]<<"  ";
                    }
                    stream<<"\n";

                    for(int j = 0;j<length;j++){
                        stream<<tailArray[i].dihedralAngleW[j]<<"  ";
                    }
                    stream<<"\n";

                    for(int j = 0;j<length;j++){
                        stream<<tailArray[i].gridV[j]<<"  ";
                    }
                    stream<<"\n";

                    for(int j = 0;j<length;j++){
                        stream<<tailArray[i].gridU[j]<<"  ";
                    }
                    stream<<"\n";
                    stream<<"isSymmtery  "<<tailArray[i].isSymmetry;
                    stream<<"\n";


                }
                stream<<"tailEnd"<<"\n";
            }

            if(!propellerArray.isEmpty()){
                stream<<"propellerStart"<<"\n";
                for(int i = 0;i<propellerArray.length();i++){
                    int length = propellerArray[i].chordLengthW.length();
                    stream<<propellerArray[i].name<<"\n";

                    for(int j = 0;j<length;j++){
                        stream<<propellerArray[i].airfoilNameArray[j]<<"  ";
                    }
                    stream<<"\n";


                    for(int j = 0;j<length;j++){
                        stream<<propellerArray[i].spanW[j]<<"  ";
                    }
                    stream<<"\n";

                    for(int j = 0;j<length;j++){
                        stream<<propellerArray[i].chordLengthW[j]<<"  ";
                    }
                    stream<<"\n";

                    for(int j = 0;j<length;j++){
                        stream<<propellerArray[i].offsetLengthW[j]<<"  ";
                    }
                    stream<<"\n";

                    for(int j = 0;j<length;j++){
                        stream<<propellerArray[i].twistAngleW[j]<<"  ";
                    }
                    stream<<"\n";

                    for(int j = 0;j<length;j++){
                        stream<<propellerArray[i].dihedralAngleW[j]<<"  ";
                    }
                    stream<<"\n";

                    for(int j = 0;j<length;j++){
                        stream<<propellerArray[i].gridV[j]<<"  ";
                    }
                    stream<<"\n";

                    for(int j = 0;j<length;j++){
                        stream<<propellerArray[i].gridU[j]<<"  ";
                    }
                    stream<<"\n";
                    stream<<"propellerNum  "<<tailArray[i].num;
                    stream<<"\n";


                }
                stream<<"propellerEnd"<<"\n";
            }

            if(!airplaneArray.isEmpty()){
                stream<<"airplaneStart"<<"\n";
                for(int i = 0;i<airplaneArray.length();i++){
                    for(int j = 0;j<5;j++){
                        stream<<airplaneArray[i].compoentIndex[j]<<"  ";
                    }
                    stream<<"\n";

                    for(int j = 0;j<5;j++){
                        stream<<airplaneArray[i].xLocation[j]<<"  ";
                    }
                    stream<<"\n";

                    for(int j = 0;j<5;j++){
                        stream<<airplaneArray[i].aAlpha[j]<<"  ";
                    }
                    stream<<"\n";

                    for(int j = 0;j<5;j++){
                        stream<<airplaneArray[i].zLocation[j]<<"  ";
                    }
                    stream<<"\n";

                    for(int j = 0;j<5;j++){
                        stream<<airplaneArray[i].isChoice[j]<<"  ";
                    }
                    stream<<"\n";

                    stream<<airplaneArray[i].name;
                    stream<<"\n";



                }
                stream<<"airplaneEnd"<<"\n";
            }




        }
        file.close();
    }
}
void myFile::initialDataA(const QVector<wingDefinition>&wing,const QVector<QVector<QVector<double>>>&airfoil,const QVector<QString>&name){
    wingArray = wing;
    airfoilArray = airfoil;
    nameArray = name;
}
void myFile::initialDataB(const QVector<wingDefinition>&tail,const QVector<wingDefinition>&propeller,const QVector<airplaneDefinition>&airplane){
    tailArray = tail;
    propellerArray = propeller;
    airplaneArray = airplane;

}

void myFile::clearData(){
    wingArray.clear();
    airfoilArray.clear();
    nameArray.clear();
    tailArray.clear();
}
/* airfoil 2
 * wing 3
 * propeler 0
 * airfoilStart
 * airfoilName
 * x y
 * x y
 * x y
 * airfoilEnd
 * wingStart
 * wingName
 * airfoilName - airfoilName 2
 * span-span
 * chord-chord
 * offsetLength-offsetLength
 * twistAngle-twistAngle
 * dihedAngle-dihedAngle
 * Xmesh
 * Ymesh-Ymesh
 * wingEnd
 *
 *
 *
 *
 *
 *
 *
 *
 *
  */
void myFile::readInterData(QString name){
    if(!interData.isEmpty())
        interData.clear();
    QString fileName = QDir::currentPath() + "/libaries/airfoil/profili/airfoilData/" + name + ".txt";

    QFile file(fileName);
    if(file.open(QIODevice::ReadOnly|QIODevice::Text)){
        QTextStream in(&file);
        while (!in.atEnd()) {
                airfoilData data;

                // 读取第一个整数 Re
                in >> data.Re;

                // 依次读取四组数据
                QString line;

                // 读取 alphaData
                line = in.readLine();
                if (line.isEmpty() && !in.atEnd()) {
                    line = in.readLine(); // 处理可能的空行
                }
                QStringList alphaValues = line.split(" ", QString::SkipEmptyParts);
                for (const QString& value : alphaValues) {
                    data.alphaData.append(value.toDouble());
                }

                // 读取 clData
                line = in.readLine();
                if (line.isEmpty() && !in.atEnd()) {
                    line = in.readLine();
                }
                QStringList clValues = line.split(" ", QString::SkipEmptyParts);
                for (const QString& value : clValues) {
                    data.clData.append(value.toDouble());
                }

                // 读取 cdData
                line = in.readLine();
                if (line.isEmpty() && !in.atEnd()) {
                    line = in.readLine();
                }
                QStringList cdValues = line.split(" ", QString::SkipEmptyParts);
                for (const QString& value : cdValues) {
                    data.cdData.append(value.toDouble());
                }

                // 读取 cmData
                line = in.readLine();
                if (line.isEmpty() && !in.atEnd()) {
                    line = in.readLine();
                }
                QStringList cmValues = line.split(" ", QString::SkipEmptyParts);
                for (const QString& value : cmValues) {
                    data.cmData.append(value.toDouble());
                }

                in.readLine();
                interData.append(data); // 将结构体添加到列表中
            }
        file.close();
    }

}
QVector<airfoilData> myFile::getInterData(QString name) {
    QVector<airfoilData> array;
    QString fileName = QDir::currentPath() + "/libaries/airfoil/profili/airfoilData/" + name + ".txt";


    // 检查文件是否存在
    if (!QFile::exists(fileName)) {
        // 可以输出错误信息到控制台
        //qWarning() << "文件不存在: " << fileName;
        // 也可以返回空数组，让调用者处理
        return array;
    }

    QFile file(fileName);
    // 尝试打开文件并检查是否成功
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        // 输出打开失败的原因
        //qWarning() << "无法打开文件: " << fileName << "，错误: " << file.errorString();
        return array;
    }

    QTextStream in(&file);
    while (!in.atEnd()) {
        airfoilData data;

        // 读取第一个整数 Re
        in >> data.Re;

        // 依次读取四组数据
        QString line;

        // 读取 alphaData
        line = in.readLine();
        if (line.isEmpty() && !in.atEnd()) {
            line = in.readLine(); // 处理可能的空行
        }
        QStringList alphaValues = line.split(" ", QString::SkipEmptyParts);
        for (const QString& value : alphaValues) {
            data.alphaData.append(value.toDouble());
        }

        // 读取 clData
        line = in.readLine();
        if (line.isEmpty() && !in.atEnd()) {
            line = in.readLine();
        }
        QStringList clValues = line.split(" ", QString::SkipEmptyParts);
        for (const QString& value : clValues) {
            data.clData.append(value.toDouble());
        }

        // 读取 cdData
        line = in.readLine();
        if (line.isEmpty() && !in.atEnd()) {
            line = in.readLine();
        }
        QStringList cdValues = line.split(" ", QString::SkipEmptyParts);
        for (const QString& value : cdValues) {
            data.cdData.append(value.toDouble());
        }

        // 读取 cmData
        line = in.readLine();
        if (line.isEmpty() && !in.atEnd()) {
            line = in.readLine();
        }
        QStringList cmValues = line.split(" ", QString::SkipEmptyParts);
        for (const QString& value : cmValues) {
            data.cmData.append(value.toDouble());
        }

        in.readLine();
        array.append(data); // 将结构体添加到列表中
    }

    file.close();
    return array;
}
