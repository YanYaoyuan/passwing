#ifndef AIRFOILDISPLAY_H
#define AIRFOILDISPLAY_H

#include <QObject>
#include <QChart>
#include <QVector>
#include <QLineSeries>
#include <QScatterSeries>
#include <QAreaSeries>
#include <QValueAxis>
#include <QGridLayout>
#include <QDialog>
#include <QComboBox>
#include <QPushButton>
#include <QSpinBox>
#include <QGroupBox>
#include <QLabel>
#include <QLineEdit>
#include <QColorDialog>
#include <QProgressBar>
#include <QMenu>
#include <QRadioButton>
#include <QTextEdit>
#include "airfoiloptimization.h"
#include "PublicClass/mychartview.h"
#include "AirfoilClass/airfoildesign.h"
#include "AirfoilClass/airfoilsolve.h"
#include "AirfoilClass/airfoillibary.h"
#include "AirfoilClass/airfoilanalyse.h"
#include "AirfoilClass/airfoiloutput.h"
#include "AirfoilClass/airfoilexplorer.h"
#include "PublicClass/structDefinition.h"
#include "PublicClass/hoverbutton.h"
#include "publicWidgetClass/datapointdialog.h"
enum libariesUsagePattern {
    NOTHING,      //什么都不作时
    CHECKCST,     //检查CST误差时
    OUTPUTFOIL,   //输出翼型时
    PROPDESIGN,       //螺旋桨设计

};
enum readDataUsagePattern {
    DESIGN,           //设计模式
    OPTIMIZATION,     //优化模式
    BLENDINGA,        //  导入融合A翼型
    BLENDINGB,        //  导入融合B翼型
    BLENDING,

};

class airfoilDisplay:public QWidget
{
    Q_OBJECT
public:
    airfoilDisplay();
    airfoilDisplay(const int);
    ~airfoilDisplay();
    int cstNum = 6;
    void setAirfoil(QVector<QVector<double>>&,QVector<double>&);//设置修改的翼型
    void showModifyAirfoilDialogCST(){modifyAirfoilDialog->show();}
    void hideModifyAirfoilDialogCST(){modifyAirfoilDialog->hide();}
    void showLibary();
    //*************Main**************//
    QVector<QVector<QVector<double>>>airfoilArray;
    QVector<QVector<airfoilData>>interDataArray;

    QWidget *airfoilDesignWidget;
    QWidget *airfoilOptimizationWidget;

    libariesUsagePattern modelType;  //什么模式下使用的翼型库
    readDataUsagePattern readDataType;
    void saveAllSetting();
    void solveDesignAirfoil();
    void saveOptimizationSetting();

public slots:
    //接受项目文件
    //生成导入项目的翼型
    void updateAirfoilList(const QVector<QVector<QVector<double>>>&,const QVector<QString>&);
    void showAirfoilExplorerWidget();
signals:
    void emitAirfoilData(const QVector<QVector<double>>,const QVector<double>);
    void emitClicked();

private:

    QVector<QVector<double>>newAirfoil;
    QVector<double>cst;


    QDialog *modifyAirfoilDialog;

    void initialSetup();
/***************************加载背景*****************************/

    void loadIcon();


    QString removeIcon;
    QString checkIcon;


    QString lineStyleIcon[8];

    /***************************翼型正设计***************************/
    void initialAirfoilDesignWidget();

    QVBoxLayout *designVLayout;
    QHBoxLayout *designHLayoutA;
    QGridLayout *designGLayoutA1;

    QHBoxLayout *designHLayoutB;
    QGridLayout *designGLayoutB1;
    QGridLayout *designGLayoutB2;
    QVBoxLayout *designVLayoutB1;
    QHBoxLayout *designHLayoutB2;

    QGroupBox *airfoilListBox;
    QGroupBox *designSettingBox;
    QGroupBox *airfoilToolBox;


    QVector<QPushButton*>airfoilButton;
    QVector<hoverButton*>airfoilCheckButton;
    QVector<hoverButton*>airfoilColorButton;
    QVector<hoverButton*>airfoilRemoveButton;
    QComboBox *seriesTypeCombobox;     QLabel *seriesNameLabel;                    //曲线类型
    QComboBox *alphaCombobox;          QLabel *alphaNameLabel;                   //迎角
    QLineEdit *setNameEdit;            QLabel *setNameLabel;                    //翼型名称
    QDoubleSpinBox *cstRatioSpinBox;   QLabel *cstRatioNameLabel;                   //cst变化倍率
    QLabel *xLabelTextA;               QLabel *yLabelTextA;
    QLabel *xLabelTextB;               QLabel *yLabelTextB;
    QLabel *xLabelTextC;               QLabel *yLabelTextC;


    QChart *designChartA;
    MyChartView *designChartViewA;
    QLineSeries *designSeriesUpper;
    QLineSeries *designSeriesLower;
    QLineSeries *designScatterSeriesA;
    QLineSeries *xblUpperSeries;
    QLineSeries *xblLowerSeries;

    QAreaSeries* upperAreaSeries = nullptr;
    QAreaSeries* lowerAreaSeries = nullptr;


    QScatterSeries *designSeriesPointA;
    QScatterSeries *designSeriesPointB;

    //后处理
    QScatterSeries *transitionPoint;//转捩
    QScatterSeries *designPointA;//设计点
    QScatterSeries *designPointB;//设计点

    QValueAxis *designAxisXA;
    QValueAxis *designAxisYA;
    bool fontChangedA = false;
    bool fontChangedB = false;
    bool fontChangedC = false;
    bool isSolve = true;
    QChart *resultChartA;
    QChart *resultChartB;
    MyChartView *resultChartViewA;
    MyChartView *resultChartViewB;
    QVector<QLineSeries*>resultSeriesA;
    QVector<QLineSeries*>resultSeriesB;
    QLineSeries *autoSeriesA;//添加对比点
    QLineSeries *autoSeriesB;//添加对比点

    QValueAxis *resultAxisXA;
    QValueAxis *resultAxisYA;
    QValueAxis *resultAxisXB;
    QValueAxis *resultAxisYB;


    QLabel *ReLabel;                QLabel *MaLabel;

    QLabel *NcriLabel;              QLabel *AlphaStepLabel;
    QLabel *MinAlphaLabel;         QLabel *MaxAlphaLabel;
    QLabel *XtrTopLabel;           QLabel  *XtrBotLabel;

    QLineEdit *ReEdit;              QLineEdit *MaEdit;

    QLineEdit *NcriEdit;            QLineEdit *AlphaStepEdit;
    QLineEdit *MinAlphaEdit;        QLineEdit *MaxAlphaEdit;
    QLineEdit *XtrTopEdit;          QLineEdit *XtrBotEdit;

    QPushButton *saveDesignButton;  QPushButton *cancelDesignButton;





    xfoilSetting input;
    XFoil *modFoil;

    QVector<double> historySettingData;//导入历史设置
    double MAXAXIS[8];
    double MINAXIS[8];


    int airfoilIndex = 0;
    int choiceIndex = 0;

    QVector<QVector<QVector<double>>>historyAirfoilArray;
    QVector<QString>airfoilNameArray;
    QVector<airfoilDesign*>airfoilDesignModelArray; //每个翼型的设计类
    QVector<airfoilSolve*>airfoilSolveModelArray;
    QVector<XFoil*>airfoilXfoilArray;
    QVector<QVector<double>>cstArray;
    QVector<int>cstNumArray;
    QVector<QVector<double>>cstPointXArray;
    QVector<QVector<double>>cstPointYArray;
    QColorDialog *colorDialog;
    QVector<QColor>airfoilColorArray;
    QVector<int>resultPenArray;
    QVector<QVector<double>>resetAirfoilArray;//
    QString axisXName[10];
    QString axisYName[10];
    QString titleName[10];
    int resultAIndex = 1;
    int resultBIndex = 6;
    int activeAlphaIndex = 0;//迎角索引
    //第二种控制方式
    QDialog *airfoilModifyDialog;

    QDoubleSpinBox *thicknessSpin;
    QDoubleSpinBox *camberSpin;
    QDoubleSpinBox *thkPosSpin;
    QDoubleSpinBox *camPosSpin;
    QDoubleSpinBox *leRadiusSpin;
    QDoubleSpinBox *leBlendSpin;

    QPushButton *okBtn;
    QPushButton *cancelBtn;

    void initialModifyAirfoilDialog();











    void addNewAirfoil(const QVector<QVector<double>>&);
    void updateDesignMousePosition(const QPointF&);
    void updateDesignCSTPoint();
    void changeDesignCSTControlPoint(const double);
    bool returnDesignCSTNum(const QPointF);
    void updateAxes(QChart*, const QVector<QLineSeries*>&);

    void updateDesignTextView(airfoilDesign*);
    void updateDesignTextViewFormXfoil(XFoil*);
    void updateOptimizationTextView(const double);

    void modifyCamberAndThickness(double,double,QVector<QVector<double>>&);
    void modifyRadiusAndInfluence(double,double,QVector<QVector<double>>&);
    void modifyCamberAndThicknessLocation(double,double,QVector<QVector<double>>&);
    void initialXfoil(XFoil*,QVector<QVector<double>>);

private slots:
    void showModifyAirfoilDialog();
    void modifyCamber(double);
    void modifyThickness(double);
    void modifyCamberLocation(double);
    void modifyThicknessLocation(double);
    void resetAirfoil();

    void modifyRadius(double);
    void modifyRadiusInfluence(double);

private:


    //增加点
    void addDataPointA();
    void clearDataPointA();

    void addDataPointB();
    void clearDataPointB();
//singal
    void choiceAirfoilButton();
    void showColorDialog();
    void changeAirfoilColor(const QColor&);
    void changeDesignProgressBar(const int);
    void changeOptimizationProgressBar(const int);
    void changeAnalyseProgressBar(const int);
    void deleteAirfoilDesignButton();
    void changeAirfoilName(const QString);
    void changeResultPen(const int);
    void changeAlpha(const int);
    void changeResultCpx(const int);
    void changeTransitionLocation(const int);//改变转捩点
    void changeBlrSeries(const int);//更新翼型流线
    void changeDesignPoint(const int);//改变设计点
    int findLeadingEdgeIndex(int ,
                                   const QVector<QVector<QVector<double>>>&);

    void updateBlAreaSeries();
    void changeResultTypeA();
    void changeResultTypeB();
    void saveDesignSetting();
    void cancelDesignSetting();
    void changeDesignChartType();
//
    void drawDesignAirfoil(const int);

    void drawDesignResult(const int);
    void removeDesignAirfoil(const int);
    void setDesignButtonEnabled(const int);
    QPen getPen(const int);

    /***************************************右键菜单**************************************/
    void initialDesignChartMenu();
    QMenu *designChartMenu;
    //QAction* designActionArray[5];
    void toggleGridLines();//隐藏坐标系
    void adjustChartViewAspectRatio(QChartView *);//等比例显示
    QVector<QAction*>designActionArray;
    void showDesignChartMenu();
    int designChartIndex = 0;
    //
    QMenu *rChartAMenu; QMenu *rChartBMenu;
    QVector<QAction*>rActionArray;
    QVector<QAction*>rBctionArray;


    void initialRChartMenu();
    //solt
    void showRChartAMenu();
    void showRChartBMenu();
    void exportChartData(QChart *chart);

    /***************************************翼型优化********************************************/
    void initialAirfoilOptimizationWidget();

    QVBoxLayout *optimizationVLayout;
    QHBoxLayout *optimizationHLayoutA;
    QGridLayout *optimizationGLayoutA1;

    QHBoxLayout *optimizationHLayoutB;
    QGridLayout *optimizationGLayoutB1;
    QGridLayout *optimizationGLayoutB2;
    QVBoxLayout *optimizationVLayoutB1;
    QHBoxLayout *optimizationHLayoutB2;


    QGroupBox *optimizationBox;
    QGroupBox *optimizationToolBox;


    //翼型显示
    QChart *optimizationChartA;
    MyChartView *optimizationChartViewA;
    QLineSeries *optimizationSeriesA;
    QValueAxis *optimizationAxisXA;
    QValueAxis *optimizationAxisYA;
    // display  //迭代过程展示
    QValueAxis *iterateAxAxis;
    QValueAxis *iterateAyAxis;
    QLineSeries *iterateSeries;
    QChart *iterateChart;
    MyChartView *iterateView;


    // display  //压力分布曲线展示
    QValueAxis *cpxAxAxis;
    QValueAxis *cpxAyAxis;
    QLineSeries *oriCpxSeries;
    QLineSeries *optCpxSeries;
    QChart *cpxchart;
    QChartView *cpxchartView;
    QVector<double>oriCpx;
    QVector<double>optCpx;

    //参数设置
    QLabel *stepLabel;      QLabel *selectionLabel;
    QLabel *initialEliteNumLabel; QLabel *eliteNumLabel;
    QLabel *crossLabel;    QLabel *variationLabel;
    QLabel *valLabel;      QLabel *cstRadioLabel;
    QLabel *ThreadNumLabel;QLabel *iterLimLabel;
    QLabel *optReLabel;    QLabel *optMaLabel;





    QLineEdit *stepEdit;      QLineEdit *selectionEdit;
    QLineEdit *initialEliteNumEdit; QLineEdit *eliteNumEdit;
    QLineEdit *crossEdit;    QLineEdit *variationEdit;
    QLineEdit *valEdit;      QLineEdit *cstRadioEdit;
    QLineEdit *ThreadNumEdit;QLineEdit *iterLimEdit;
    QLineEdit *optReEdit;    QLineEdit *optMaEdit;




    //目标设置
    QLabel *designAlphaLabel; QLabel *designClLabel;
    QRadioButton *targetRadioButtonA; QRadioButton *targetRadioButtonB;

    QLabel *choseAirfoilLabel; QLineEdit *valueEdit;
    QLabel *thickWeightedLabel; QLabel *cmWeightedLabel;
    QLabel *thickWeightedValueLabel; QLabel *cmWeightedValueLabel;

    QSlider *thickWeightedSlider; QSlider *cmWeightedSlider;
    int thickWeighted = 0,cmWeighted = 0;//权重值
    QVector<double>thick;//存放厚度

    QLabel *targetLabel;




    QComboBox *choiceCombobox;

    //QLineSeries *optAirfoilSeries;

    int choiceOPTAirfoilIndex = 0; //进行优化的翼型ID


    QVector<fixClResult>historyData;              //历史迭代总结果
    QVector<double>resultK;                       //历史迭代目标结果
    double oriK;                                  //原始目标结果
    double optK;
    QVector<QVector<double>>optAirfoil;



    airfoilOptimization *exp;
    GaParameters GaSetting;
    int threadNum;
    int workerNum;
    int optStep;

    bool optCpxIsChange = false;
    //翼型显示


    void drawCpxData(const QVector<double>&,QLineSeries*);
    //slot

    void cancelOptimizationSetting();
    void updateThickWeightedValueLabel(const int);
    void updateCmWeightedValueLabel(const int);
    void drawOptIndexAirfoil(const int);
    void drawOptAirfoil(const QVector<QVector<double>>&);

/*****************************************************/
    airfoilLibary *airfoilList;
    void initialAirfoilLibaries();
    //slots
    void useAirfoilLibaries();

private slots:
    void saveAirfoilData();
    void resetAirfoilData();
public:








    //func

    void initialGa();
    void startOpt();
    void solveGa();
    void startXfoilInThread(int step,int n);
    void finishOpt();
    /******************************翼型分析*******************************/
    airfoilAnalyse *analyse;
    void initialAnalyse();
    void showAnalyseTable();
    void showAnalyseReTable();
    /*******************************翼型读取******************************/
    void readData();
    /******************************翼型输出********************************/
public:
    airfoilOutput *foilOutput;
    void initialAirfoilOutput();
    void showAirfoilOutputTable();

signals:
    void emitOptimizationUI(const int,const QVector<QVector<double>>&);
    void emitAirfoilArray(const QVector<QVector<QVector<double>>>&,const QVector<QString>&,const QVector<int>&);
    void emitAirfoilInterArray(const QVector<QVector<airfoilData>>&);//传递翼型插值数据
    void emitMyChoiceAirfoil(QVector<QVector<QVector<double>>>&,QVector<QString>&);
    void emitAirfoilProgressBarValue(const int);

public slots:
    void updateOptimizationUI(const int,const QVector<QVector<double>>&);
    /***************************************CST误差分析******************************************/
public:
    int cstNUM;   //cst总数
    QGridLayout *checkGridLayout;
    //QPushButton *checkCSTPushbutton;
    QDialog *checkCSTDialog;
    QSpinBox *CSTNumBox;



    QChart *chartF;
    QChartView *chartViewF;
    QChart *chartG;
    QChartView *chartViewG;

    QValueAxis *checkXAxis;
    QValueAxis *checkYAxis;
    QValueAxis *checkValueXAxis;
    QValueAxis *checkValueYAxis;

    QLineSeries *airfoilSeriesF;
    QLineSeries *airfoilSeriesG;

    QLineSeries *upperToleranceSeries;
    QLineSeries *lowerToleranceSeries;

    QVector<QVector<double>>checkAirfoil;
    QString checkName;




    void initialCheckCSTDialog();
    void showCheckAirfoilDialog();
    //螺旋桨调用
private:

/*************************机翼翼型修改**************************/
private:
    int cstnum = 0;
    bool isMousePressed = false;            // 鼠标左键判断
    QVector<double>cstPointY;
    QVector<double>cstPointX;

    QChart *chartA;
    MyChartView *chartViewA;
    QLineSeries *seriesA;
    QLineSeries *scatterSeries;
    QScatterSeries *seriesCstPointA;
    QScatterSeries *seriesCstPointB;

    QValueAxis *axisXA;
    QValueAxis *axisYA;
    QGridLayout *gridLayoutA;
    QSpinBox *spinBoxA;
    QComboBox *cstNumCombobox;
    QPushButton *saveAirfoilButton;
    QPushButton *restoreAirfoilButton;
    QPushButton *cancelAirfoilButton;

    airfoilDesign *designModel;

    void initialModifyAirfoilDialogCST();
    bool returnCstNum(const QPointF position);
    void changeCstControlPoint(double s);
    void drawAirfoil();


public slots:
    void updateMousePosition(const QPointF &);
    void getMousePressBeginPosition(const QPointF &);
    void getMouseReleaseEndPosition(const QPointF &);
    void updateResultAMousePosition(const QPointF &);
    void updateResultBMousePosition(const QPointF &);
    void drawCheckCSTAirfoil();

    void updateDesignTextItem(int,int);
    void updateOptimizationTextItem(int,int);
    //翼型融合
private:
    void initialAirfoilBlendingDialog();
    void updateBlendingAirfoil();
    QDialog *blendingDialog;
    QGridLayout *blendingGLayout;
    QChart *blendingChart;
    MyChartView *blendingView;
    QLineSeries *blendingSeriesA;
    QLineSeries *blendingSeriesB;
    QLineSeries *blendingSeriesC;
    QValueAxis *blendingAxisX;
    QValueAxis *blendingAxisY;

    QSlider *blendingSlider;
    QLabel *blendingTextLabelA;
    QLabel *blendingTextLabelB;
    QLabel *blendingTextLabelC;
    QLabel *blendingValueLabelA;
    QLabel *blendingValueLabelB;
    QCheckBox *blendingCheckA;
    QCheckBox *blendingCheckB;
    QCheckBox *blendingCheckC;
    QComboBox *blendingComboboxA;
    QComboBox *blendingComboboxB;
    QPushButton *blendingSaveButton;
    QPushButton *blendingCancelButton;
    bool blendingReadyA;
    bool blendingReadyB;
    QVector<QVector<double>>blendingAirfoilData;
private slots:
    void changeBlendingAirfoilA(const int);
    void changeBlendingAirfoilB(const int);
    void addBlendingAirfoil();
    void hideBlendingAirfoil();
    void showAirfoilBlendingDialog();



    //void drawBlendingAirfoil(const int,const QVector<QVector<double>>&);



//翼型数据插值
    /***********************阻力插值库****************************/
public:
    QWidget *dragInterWidget;
private:

    QGroupBox *dragSettingBox;
    QHBoxLayout *dragInterHLayout;
    QVBoxLayout *dragSettingVLayout;
    QVBoxLayout *dragViewVlayout;
    QGridLayout *dragSettingGLayout;

    QPushButton *solutionModelButtonA;
    QPushButton *solutionModelButtonB;
    QLabel *airfoilLabel; QComboBox *airfoilCombox;
    QLabel *minAlphaLabel;  QLineEdit *minAlphaEdit;
    QLabel *maxAlphaLabel;  QLineEdit *maxAlphaEdit;
    QLabel *stepAlphaLabel; QLineEdit *stepAlphaEdit;
    QLabel *minReLabel;  QLineEdit *minReEdit;
    QLabel *ncriLabel; QLineEdit *ncriEdit;
    QLabel *maxReLabel;  QLineEdit *maxReEdit;
    QLabel *stepReLabel; QLineEdit *stepReEdit;

    QLabel *maLabel; QLineEdit*maEdit;
    QLabel *iterlimLabel; QLineEdit *iterlimEdit;
    QLabel *threadNumLabel; QLineEdit *threadNumEdit;
    QLabel *interMethodLabel;QComboBox *interMethodCombox;
    QTextEdit *iterTextEdit;

    QChart *dragInterChartA;
    QChart *dragInterChartB;
    MyChartView *dragInterViewA;//result
    MyChartView *dragInterViewB;//foil
    QValueAxis *dragInterAxisXA;
    QValueAxis *dragInterAxisYA;
    QValueAxis *dragInterAxisXB;
    QValueAxis *dragInterAxisYB;
    QMenu *interChartMenu;
    QVector<QAction*>interActionArray;

    QVector<QLineSeries*>dragSeriesArray;
    QLineSeries *dragInterSeries;


    QVector<int>interReValueArray;
    bool isSettingOk = false;
    xfoilSetting interSetting;
    int interChoiceIndex = 0;
    int interActionIndex = 0;


    QVector<QVector<double>>interAirfoil;
    QVector<airfoilData>interResultArray;
    void initialInterDragWidget();
    void initialInterChartMenu();



    void startXfoilInThreadB(const int,const int);
    void saveInterText();
    void saveInterText(int);

    void startInterSolver();//
    void startInterChoiceSolver();//

    void drawInterClResult(const airfoilData&);
    void drawInterCdResult(const airfoilData&);
    void drawInterKResult(const airfoilData&);
    void drawInterPResult(const airfoilData&);
    void drawInterCmResult(const airfoilData&);
    void drawInterRResult(const airfoilData&);//极曲线






private slots:

    void updateAnalyseSetting();
    void updateInterButtonState();
    void drawInterAirfoil();
    void changeInterResultType();
    void showInterChartMenu();
public:
    void startInterAnalyse();

private:
    //DEBUG
    void startInterAllSolver();//Debug

private:
    airfoilExplorer *explorer;
    void initialExplorerWidget();

};

#endif // AIRFOILDISPLAY_H
