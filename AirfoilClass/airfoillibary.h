#ifndef AIRFOILLIBARY_H
#define AIRFOILLIBARY_H

#include <QWidget>
#include <QVector>
#include <QListView>
#include <QGridLayout>
#include <QStringListModel>
#include <QPushButton>
#include <QComboBox>
#include <QChart>
#include <QChartView>
#include <QLineSeries>
#include <QLineEdit>
#include <QValueAxis>
#include <QTableWidget>
#include <QLabel>
#include <H5Cpp.h>
#include "PublicClass/structDefinition.h"


#include <QtCharts/QChart>
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
QT_CHARTS_USE_NAMESPACE
#endif
enum librariesType{
    AIRFOIL_PROFILI,
    AIRFOIL_UIUC,
    AIRFOIL_WINDAIBENCH,
};


class airfoilLibary : public QWidget{
   Q_OBJECT

public:
/******************************** libary********************************************/
    airfoilLibary(QWidget *parent = nullptr);
    ~airfoilLibary();

    void initial();


    bool initialDone = false;
    int listLength = 0;
    int listLengthA = 0;
    int listLengthB = 0;
    int listLengthC = 0;


    QWidget *libaryWidget;
    QGridLayout *gridlayoutA;

    QComboBox *modelCombox;
    QPushButton *searchNameButton;
    QLineEdit *searchNameEdit;
    QPushButton *searchParametersButton;
    QPushButton *searchSimilarButton;
    QPushButton *importAirfoilButton;
    QComboBox *sortModelCombobox;

    //QWidget libaryWidget;
    QListView *listView;
    QChart *graphChart;
    QChartView *graphview;
    QLineSeries *series;

    QValueAxis *xaxis;
    QValueAxis *yaxis;



    QGraphicsSimpleTextItem *mouseLocitionLibary;//
    QStringListModel *listModel;
    QStringList airfoilNameArrayA;
    QStringList airfoilNameArrayB;
    QStringList airfoilNameArrayC;

   //All
   QVector<AirfoilParameters>Airfoil;
   QVector<AirfoilParameters>AirfoilCopy;
   QVector<QVector<double>>airfoilArray[2000];
   QVector<QVector<double>>resultArray;//计算结果
   QVector<QString>resultName;//计算翼型名

   //profili
   QVector<AirfoilParameters>AirfoilA;
   QVector<AirfoilParameters>AirfoilCopyA;
   QVector<QVector<double>>airfoilArrayA[1000];
   QVector<QVector<double>>resultArrayA;//计算结果
   QVector<QString>resultNameA;//计算翼型名



   //UIUC
   QVector<AirfoilParameters>AirfoilB;
   QVector<AirfoilParameters>AirfoilCopyB;
   QVector<QVector<double>>airfoilArrayB[1600];
   QVector<QVector<double>>resultArrayB;//计算结果
   QVector<QString>resultNameB;//计算翼型名


   //AIBench
   QVector<AirfoilParameters>AirfoilC;
   QVector<AirfoilParameters>AirfoilCopyC;
   QVector<QVector<double>>airfoilArrayC[2000];
   QVector<QString>resultNameC;
   QVector<QVector<double>>airfoilCSTArrayC;//CST形式翼型


   QVector<QVector<double>> readCST();


   //QVector<QVector<QVector<double>>>airfoilArray;


   int copyLength = 0;
   librariesType modelType = AIRFOIL_PROFILI;  //翼型库类型
   bool searchType = false;//搜索模式
   bool isSort = false;    //排序模式

   /*                    */

   int cstNUM = 6;
   int listChose = 0;
private:
    QVector<double> read1DData(const QString &datasetPath);
    std::unique_ptr<H5::H5File> file;  // HDF5 文件句柄
    bool openFile(const QString&);
private:
   void solveOtherParameters();
   void writeAirfoilParameters();



   int importAirfoilData();
   void newStructAirfoilData();

   void readAirfoilParametersA();
   void importMyAirfoilLibaryA();
   void importMyResultA();

   void readAirfoilParametersB();
   void importMyAirfoilLibaryB();
   void importMyResultB();

   void readAirfoilParametersC();
   void importMyAirfoilLibaryC();

   void sortMaxK();
   void sortMaxCl();
   void sortMinCm();
   //fun
   void sortData(QVector<QVector<double>>& data, int column, bool descending);
   int getBothNameIndexA(const QString);
   int getBothNameIndexB(const QString);
   //ui
   void updateTextPosition();
signals:
   void buttonClicked();
public slots:
   void drawAirfoil(const QItemSelection&,const QItemSelection&);
    void emitButtonClicked(){
        emit buttonClicked();
    }
    void searchAirfoilNameA();
    void searchAirfoilNameB();
    void searchAirfoilNameC();
    void searchAirfoil();
    void changeAirfoilList(const int);
    void changeAirfoilModel(const int);














};

#endif // AIRFOILLIBARY_H
