#ifndef AIRFOILOUTPUT_H
#define AIRFOILOUTPUT_H

#include <QWidget>
#include <QBoxLayout>
#include <QLabel>
#include <QComboBox>
#include <QPushButton>
#include <QGroupBox>
#include <QChart>
#include <QChartView>
#include <QLineEdit>
#include <QSpinBox>
#include <QCheckBox>
#include <QLineSeries>
#include <QValueAxis>
#include <QScatterSeries>
#include <QLineEdit>

#include <QtCharts/QChart>
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
QT_CHARTS_USE_NAMESPACE
#endif


struct ACircle
{
    double x;
    double y;
    double radius;
};
struct ARectangle
{
    double a;
    double b;
    double roateAngle;
    double x,y;

};
class airfoilOutput:public QWidget
{
    Q_OBJECT
public:
    airfoilOutput();
/***************************************翼型坐标输出*****************************************/
    void buildOutputAirfoilDialog(QVector<QString>&);

    QDialog *foilDialog = nullptr;
    QVBoxLayout *foilVbox;

    QGroupBox *foilBoxA;
    QGridLayout *foilLayoutA;

    QGroupBox *foilBoxB;
    QGridLayout *foilLayoutB;

    QHBoxLayout *foilHbox;


    /***************foilBoxA****************/
    QLabel *foilNameLabel;
    QComboBox *foilCombobox;
    QPushButton *foilLibariesButton;

    QPushButton *foilMoreChoiceButton;
    bool isFoilMoreChoice;

    QLabel *foilModelLabel;
    QComboBox *foilModelCombobox;  //离散方式
    QLabel *foilNumLabel;
    QSpinBox *foilNumSpinBox;
    QLabel *foilCSTNumLabel;
    QSpinBox *foilCSTNumSpinBox;
    QLabel *foilYteLabel;
    QDoubleSpinBox *foilYteSpinBox;
    QLabel *foilYteUnitLabel;


    QChart *foilChart;
    QChartView *foilView;
    QLineSeries *foilSeries;
    QScatterSeries *foilScaleSeries;
    QValueAxis *foilXaxis;
    QValueAxis *foilYaxis;
    QPushButton *foilSaveButton;
    QPushButton *foilCancelButton;




    QVector<QString>foilNameArray;
    QVector<QVector<QVector<double>>>foilDataArray;
    QVector<QVector<double>>outputFoilData;


    void drawFoil();
    void hideFoilBoxB();
    void showAllWidgetsInBox(QGroupBox*);
    void hideAllWidgetsInBox(QGroupBox*);
    void copyFoilArray(const QVector<QVector<QVector<double>>>&);
    void addFoil(const QString&,const QVector<QVector<double>>&);

    void deleteFoilDialog();   //销毁


    void changeFoil(const int);
    void saveFoil();

/****************************************翼型DXF输出******************************************/

public:

    void drawAirfoilDXF(const QVector<QVector<double>>&);
    void drawCircleDXF(QTextStream&,const ACircle&);
    void drawRectangleDXF(QTextStream&,const ARectangle&);
    QString dxfData;                                       //翼型总dxf数据
/********************************************减重模式一*******************************************/

    QDialog *lossModelADialog = nullptr;
    QGridLayout *lossLayoutA;
    QVector<QCheckBox*>choiceCheckBoxArray;
    QVector<QLabel*>textALabelArray1;
    QVector<QLineEdit*>diameterLineEditArray;
    QVector<QLabel*>textALabelArray2;
    QVector<QLineEdit*>locationXALineEditArray;
    QVector<QLabel*>textALabelArray3;
    QVector<QLineEdit*>locationYALineEditArray;
    QVector<QLabel*>textALabelArray4;
    QPushButton *saveLossModelAButton;
    QPushButton *cancelLossModelAButton;
    void buildLossModelADialog();
    void weightLossModelA();
    void deleteLossADialog();

/******************************************结构*****************************************/
    QDialog *foilStrcutADialog = nullptr;
    QGridLayout *foilStructLayoutA;


    void buildFoilStructADialog();


/********************************************减重模式二*******************************************/
    void weightLossModelB();

/********************************************减重模式一*******************************************/
private:
    void writeDXFheader(QTextStream&);
    void writeDXFEntities(QTextStream&,const QVector<QVector<double>>&);
/****************************************DXF转DAT********************************************/
public:
    QDialog *foilConversionDialog = nullptr;
    QGroupBox *foilBoxC;
    QGridLayout *foilLayoutC;
    QLabel *inputLabelA;
    QPushButton *inputButtonA;
    QLabel *outputLabelA;
    QPushButton *outputButTonA;
    void buildConversionDialog();
    void inputData();   //输入
    void outputData();  //输出
    void deleteConversionDialog();






};

#endif // AIRFOILOUTPUT_H
