#ifndef AIRFOILANALYSE_H
#define AIRFOILANALYSE_H

#include <QWidget>
#include <QGridLayout>
#include <QTableWidget>
#include <QLabel>
#include <QLineEdit>
#include <QStringListModel>
#include <QChartView>
#include <QListView>
#include <QLineSeries>
#include <QValueAxis>
#include <QDoubleSpinBox>
#include <QPushButton>
#include <QColorDialog>
#include <QCheckBox>
#include <QComboBox>
#include <QThread>
#include <QGroupBox>
#include "PublicClass/structDefinition.h"
#include <QRadioButton>
#include "PublicClass/mychartview.h"
#include <QFuture>
#include <QtConcurrent/QtConcurrent>
class airfoilAnalyse : public QWidget
{
    Q_OBJECT
public:
    airfoilAnalyse(QWidget *parent = nullptr);
    ~airfoilAnalyse();

    /*********************************UI**************************************/

    /*************************************************************************/
    void showAirfoilChoseWidget(){airfoilChoseWidget->show();}
    void showMoreReAnalyseWidget(){moreReAnalyseWidget->show();}
public:



    void initialSetting(const QVector<double>&);
    void initialAirfoilLibaryB(const QVector<AirfoilParameters>&,const int,const QVector<QVector<double>>*);


    void initialAirfoilLibaryC(const QVector<AirfoilParameters>&,const int,const QVector<QVector<double>>*);



signals:
    void workRequested(const xfoilSetting&, const QVector<QVector<double>>&);
    void progressUpdated(const int);

public slots:

    void updateAirfoilArrayA(const QVector<QVector<QVector<double>>>&,const QVector<QString>&);


private:
    QComboBox *listCombobox;
    QWidget *airfoilChoseWidget;          //翼型选择窗口
    QDialog *moreAirfoilAnalyseDialog;    //翼型设置窗口
    QWidget *resultWidget;                //结果显示窗口
    QColorDialog *colorDialog;            //颜色显示窗口
    QGridLayout *gridLayoutA;



    QListView *listViewA;
    QStringListModel *StringListModelA;                 //自己的库
    QVector<QVector<QVector<double>>>airfoilArrayA;     //翼型库
    QVector<QString>nameArrayA;
    QStringListModel *StringListModelB;                 //profili的库
    QVector<QVector<double>>airfoilArrayB[1000];        //翼型库

    QStringListModel *StringListModelC;                 //UIUC的库
    QVector<QVector<double>>airfoilArrayC[1700];        //翼型库

    QLineSeries *airfoilSeries;
    QValueAxis *xaxis;
    QValueAxis *yaxis;
    QGraphicsSimpleTextItem *mouseLocitionAnalyse;

    QVector<AirfoilParameters>AirfoilB;               //翼型的参数信息
    QVector<xfoilSetting>valueSetting;                //各个翼型的基本设置


    QVector<AirfoilParameters>AirfoilC;


    xfoilSetting setting[10];                         //翼型设置容器

    QVector<QVector<double>>airfoilArray[10];         //待分析翼型库
    QString nameArray[10];                            //待分析翼型名称
    QVector<QVector<double>>realAirfoil;             //选择的翼型
    QVector<QVector<double>>changeAirfoilData;
    QString realName;
    QVector<QVector<double>>all;                     //保存数据库所有翼型计算参数

    //QVector<QFutureWatcher <QVector<QVector<QVector<double>>>>*>watchers;  //线程监视

    int listChoseB = 0;                   //翼型列表选择
    int choseAirfoilIndex = 0;                 //选择的翼型序号
    int choseButtonIndex = 0;                  //指选择颜色的按钮序号


    int listLength = 0;
    int threadNum = 10;
    void initialWindow();
    void initialAirfoilView();
    void drawAirfoil(const QItemSelection&,const QItemSelection&);
    void initialSpinBox();
    void findData(const QVector<QVector<double>>&,const int);
    void initialXfoilSetting();
    void startXfoil(const int);

    /*************************************结果显示****************************************/


        QPushButton *saveResultButton;
        QVBoxLayout *vboxLayout;
        QHBoxLayout *comboboxALayout;
        QHBoxLayout *comboboxBLayout;
        QHBoxLayout *chartLayout;

        QChart *chartRA;
        QChart *chartRB;
        MyChartView *chartViewRA;
        MyChartView *chartViewRB;
        QValueAxis *rxAxisA;
        QValueAxis *ryAxisA;
        QValueAxis *rxAxisB;
        QValueAxis *ryAxisB;
        QVector<QLineSeries*>seriesArrayA;
        QVector<QLineSeries*>seriesArrayB;
        double MINAXISR[10];
        double MAXAXISR[10];
        QString axisNameA[10];


        int typeAX = 0;
        int typeAY = 0;
        int typeBX = 0;
        int typeBY = 0;


        QVector<QColor>colorArray;
        void initialChartRA();
        void initialChartRB();

        void drawResult();
        //图标类型
        QLabel *chartANameLabel;
        QLabel *chartBNameLabel;
        QComboBox *chartAXCombobox;
        QComboBox *chartAYCombobox;
        QComboBox *chartBXCombobox;
        QComboBox *chartBYCombobox;
        QComboBox *reCombobox;
        QComboBox *maCombobox;




        void initialResultUI();
        void updateAxes(QChart *chart, const QVector<QLineSeries*>&);








        int cstNUM = 6;
        QVector<double>simpleHicksHenneValue;
        QVector<double>hicksHenneValue;
        /*******************************************左下角显示**************************************************/
        QLabel *textLabel;





        /*******************************************new libary**************************************************/
        QChart *chartAirfoil;



        QChartView *viewAirfoil;
        QPushButton *searhButton;
        QLineEdit *searhTextEdit;








        /*******************************************choseList**************************************************/
        QGridLayout *gridLayout;
        QVector<QPushButton*>colorButton;
        QVector<QPushButton*>addAirfoilButton;
        QVector<QLabel*>mesageAirfoilLabel;
        QVector<QCheckBox*>airfoilCheckBox;
        QLabel *alphaLabel;                 QLabel *alphaCrLabel;                QLineEdit *minAlphaEdit;  QLineEdit *maxAlphaEdit; QLabel *stepLabel;QLineEdit *stepEdit;

        QPushButton *startAnalyseButton;   QPushButton *cancelAnalyseButton;


        /*********************************compute**************************************/
        /******************************************airfoilShap***************************************************/



        /*QListView *listView*/         QLabel *ReLabel;                    QLineEdit *ReEdit;                   QLabel *MaLabel;          QLineEdit *MaEdit;
                                        QLabel *locationFlapLabel;          QDoubleSpinBox *locationFlapBox;    QLabel *flapAngleLabel;   QDoubleSpinBox *flapAngleBox;
                                        QLabel *radiusLabel;                QDoubleSpinBox *radiusBox;      QLabel *trailingAngleLabel; QDoubleSpinBox *trailingAngleBox;
                                        QLabel *ThicknessLabel;   QDoubleSpinBox *thicknessBox;             QLabel *camberLabel; QDoubleSpinBox *camberBox;
                                        QLabel *NriclLabel;       QComboBox *NriclCombobox;
                                        QLabel *xtrTopLabel;                QLineEdit *xtrTopEdit;               QLabel *xtrBotLabel;      QLineEdit *xtrBotEdit;

                                        QVector<QDoubleSpinBox*>spinBox;

                                        QPushButton *saveButton; QPushButton *restoreButon; QPushButton *cancelButton;
                                        QVector<int>airfoilIndexA;
                                        QVector<int>airfoilIndexB;
    /**********************************************result**************************************************/
        QVector<QVector<QVector<double>>> airfoilResultData;  //气动系数结果

        /*******************************************多雷诺数分析*****************************************************/



          QWidget *moreReAnalyseWidget;
          //QDialog *moreReDisplayDialog;

          QVBoxLayout *Vlayout;
          QPushButton *openAirfoilListButton;
          QGroupBox *inputGroupBoxA;

          QGridLayout *GlayoutA;
                           QLabel *minValueLabel; QLabel *maxValueLabel; QLabel *stepValueLabel;
                           QLineEdit *minValueEdit; QLineEdit *maxValueEdit;QLineEdit *stepValueEdit;
          QLabel *maLabel; QLineEdit *maValueEdit;
          QLabel *nriclLabel;  QComboBox *nriclCombobox;

          QLabel *xtrtopLabel; QLineEdit *xtrtopEdit; QLabel *xtrbotLabel;QLineEdit *xtrbotEdit;




          QLabel *minAlphaAndLiftLabel; QLabel *maxAlphaAndLiftLabel;
          QLineEdit *minAlphaAndLiftReEdit;        QLineEdit *maxAlphaAndLiftReEdit;      QLabel *alphaAndLiftStepReLabel;        QLineEdit *alphaAndLiftStepReEdit;


          QGroupBox *inputGroupBoxB;
          QGridLayout *GlayoutB;



          QLabel *alphaRadioLabel; QLabel *liftRadioLabel;
          QRadioButton *designAlphaRadioButton; QRadioButton *designLiftRadioButton;
          QPushButton *startAnalyseReButton; QPushButton *cancelAnalyseReButton;

          //QGridLayout *GlayoutB;
          //QFrame *inputFrameB;
          QHBoxLayout *HlayoutB;







          QVector<xfoilSetting> reAnalyseSetting;



          int viewModel;
          QVector<int>reArray;
          QVector<double>alphaArray;
          QVector<QVector<double>>realAlphaArray;
          QGridLayout *gListViewLayout;
          QPushButton *reViewButton;
          QPushButton *alphaViewButton;
          QLabel *allLabelA;
          QLabel *allLabelB;
          QCheckBox *allCheckBoxA;
          QCheckBox *allCheckBoxB;
          QListView *listviewB;
          QListView *listviewC;
          QStringListModel *reynoldListModelB;
          QStringListModel *reynoldListModelC;

          void buildMoreReAnalyseWidget();

          void updateReAnalyseSetting();


private slots:
          int findAlphaIndex(const int,const QVector<double>&);
          void changeAirfoil(double);
          void changeThickness(double);
          void changeRadius(double);
          void changeTrailingAngle(double);
          void changeCamber(double);
          void changeFlap();
          void showAirfoilLibary();
          void showColorDialog();

          void addAirfoil();
          void startAnalyse();
          void startAllAnalyse();


          void changeButtonColor(const QColor &);
          void updateMousePositionA(const QPointF &);
          void updateMousePositionB(const QPointF &);

          void changeChartA();
          void changeChartB();
          void changeListModel(const int);

          void changeBoxTitle();

          void startReModelAnalyse();
          void changeReResult();
          void changeAlphaResult();
          void changeUIForReView();
          void changeUIForAlphaView();
          void drawReResult();
          void drawAlphaResult();
          void exportChartData();


};

#endif // AIRFOILANALYSE_H
