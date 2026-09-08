#ifndef WINGDISPLAY_H
#define WINGDISPLAY_H

#include <QWidget>
#include <QGroupBox>
#include <QComboBox>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QLayout>
#include <QDialog>
#include <QTreeWidget>
#include "publicWidgetClass/glwidget.h"
#include "WingClass/wingdefinition.h"
#include "WingClass/wingvlm.h"
#include "AirfoilClass/airfoildisplay.h"
#include "WingClass/wingoptimization.h"
#include "publicWidgetClass/rangeslider.h"
#include "publicWidgetClass/switchbutton.h"

#include "PublicClass/mymath.h"
#include "PublicClass/airfoilplot.h"

enum viewType{//当前界面
    WING_DESIGN,
    WING_RESULTA,
    WING_RESULTB,
    };

enum wingType{//当前翼类型
    WING_WING,
    WING_HTAIL,
    WING_VTAIL,
};
/*2025/1/6增加弹性铰链功能
 *
 *
 * */
class wingDisplay : public QWidget
{
    Q_OBJECT


public:
    wingDisplay(QWidget *parent = nullptr);
    void changeWindowForWingDesign();
    void changeWindowForTailDesign();
    void changeWindowForWingOptimization();

signals:
    void emitWingProgressBarValue(const int);
    void emitList(const QVector<wingDefinition>&,const QVector<wingDefinition>&);
    void emitWingDefineFinish();
private:

    //通过飞机设计窗口修改翼面
    bool isAirplaneSingal = false;


    void addWingData(wingDefinition&);
    void addTailData(wingDefinition&);
    void updateWingData();
    void updateView(const wingDefinition&);
    void updateStreamLineView(const wingVLM*);
    void updatePressureContourView(const wingVLM*);
    void hidePressureContourView();
    void hideStreamLineView();
    void updateWingEdit(const int);
    int getAirfoilIndex(const QString);
    //
    void updateWingTip(int);
    //
    void updateListData();//刷新飞机设计数据


    //void removeDesignWing(const int);
public slots:
    void updateWingList(QVector<wingDefinition>&);
    void updateTailList(QVector<wingDefinition>&);
    void showWingDefineDialog(int,int);
public:
    void startOptimizationWing();
    void startAnalyseWing();
    void printfWingData(const wingDefinition&);

/**********************************************机翼***************************************************/

    double SpanB;
    int SpanNumB;
    double AreaB;
    double AspectRatioB;
    QDialog *wingDefineDialogB;
    QGridLayout *wingDefineLayoutB;
    QLabel *SpanLabelB;    QLineEdit *SpanEditB;
    QLabel *SpanNumLabelB; QLineEdit *SpanNumEditB;
    QLabel *AreaLabelB;    QLineEdit *AreaEditB;
    QLabel *AspectRatioLabelB;QLineEdit *AspectRatioEditB;
    QPushButton *saveButtonB; QPushButton *cancelButtonB;
    void initialWingDefineDialog();
    QVector<double>simpleCST;

    QVector<QString>AirfoilNameArray;
    QVector<QVector<QVector<double>>>airfoilArray;
    QVector<QVector<airfoilData>>interDataArray;//翼型插值数据

    QVector<int>cstNumArray;
    wingDefinition getWingDefine();





/*******************************************机翼定义窗口************************************************/

    QWidget *wingDefineWidget;//主窗口
    QHBoxLayout *wingHLayout;
    QVBoxLayout *wingVLayout;
    QComboBox *wingTypeCombobox;
    QTreeWidget *wingTreeWidget;
    QVector<QTreeWidgetItem*>wingTreeItemArray;

    //trapezium


    QLabel *spanLabel; QLabel *chordLenthgLabel; QLabel *twistAngleLabel; QLabel *dihedralAngleLabel;QLabel *offsetLengthLabel;QLabel *nameLabel;
    QLabel *xGridNumLabel;QLabel *yGridNumLabel;

    QGroupBox *wingDefineBox;
    QGridLayout *wingDefineLayout;
    QVector<QLineEdit*>profileSpanEdit;
    QVector<QLabel*>profileLabel; QVector<QLineEdit*>profilechordLengthEdit;
    QVector<QLineEdit*>profileOffsetLengthEdit;QVector<QComboBox*>profileAirfoilChoiceCombobox;
    QVector<QLineEdit*>profileTwistAngleEdit; QVector<QLineEdit*>profileDihedralAngleEdit;
    QVector<QLineEdit*>profileXGridEdit;QVector<QLineEdit*>profileYGridEdit;

/*********************************************定义机翼显示*************************************************/
    QVTKOpenGLNativeWidget *wingDisplayWidgetA;
    vtkSmartPointer<vtkRenderer> renderer;
    vtkSmartPointer<vtkGenericOpenGLRenderWindow> renwin;


    vtkSmartPointer<vtkPoints> points;
    vtkSmartPointer<vtkCellArray> lines;
    vtkSmartPointer<vtkPolyData> profilePolyData;
    vtkSmartPointer<vtkAppendPolyData> appendFilter;
    vtkSmartPointer<vtkPolyDataMapper> mapper;
    vtkSmartPointer<vtkActor> actor;
    vtkSmartPointer<vtkLightKit> lightKit;
    vtkSmartPointer<vtkOrientationMarkerWidget>orientationMarkerA;
    void showWingDefineWidget();
/*****************************************机翼参数信息显示******************************************/
    QGroupBox *wingMessageBox;
    QGridLayout *wingMessageLayout;

    QLabel *areaLabel;       QLabel *areaTextLabel;
    QLabel *realSpanLabel;   QLabel *realSpanTextLabel;
    QLabel *aspectRatioLabel;QLabel *aspectRatioTextLabel;
    QLabel *realChordLabel;  QLabel *realChordTextLabel;
    QLabel *tipRatioLabel;   QLabel *tipRatioTextLabel;
    QLabel *meshNumLabel;    QLabel *meshNumTextLabel;
    QSpacerItem *verticalSpacerB;

    QLabel *meshVTypeLabel;  QLabel *meshUTypeLabel;
    QComboBox *meshVTypeCombobox;QComboBox *meshUTypeCombobox;
    QLabel *yteTypeLabel; QLabel *yteValueLabel;
    QComboBox *yteTypeCombobox; QLineEdit *yteValueEdit;

    QLabel *wingNameLabel;  QLineEdit *wingNameEdit;
    QPushButton *saveWingButton; QPushButton *outputWingButton;






    void initialWingMessageBox();
public slots:
    void showTailDefineDialog();
private:
    QDialog *tailDefineDialog;
    QTreeWidget *tailTreeWidget;//尾翼树目录
    QVector<QTreeWidgetItem*>tailTreeItemArray;

    QVBoxLayout *tailDefineVLayout;
    QGroupBox *tailDefineBox;
    QGridLayout *tailDefineGLayout;



    QLabel *hTailLabel;  QPushButton *hTailButton;
    QLabel *vTailLabel;  QPushButton *vTailButton;
    QLabel *tTailLabel;  QPushButton *tTailButton;//尾翼模板

    QDialog *tailTemplateDialog;

    vtkSmartPointer<vtkPoints> pointsH;
    vtkSmartPointer<vtkCellArray> linesH;
    vtkSmartPointer<vtkPolyData> profilePolyDataH;
    vtkSmartPointer<vtkAppendPolyData> appendFilterH;
    vtkSmartPointer<vtkPolyDataMapper> mapperH;
    vtkSmartPointer<vtkActor> actorH;
    vtkSmartPointer<vtkUnsignedCharArray> colorH;

    vtkSmartPointer<vtkPoints> pointsV;
    vtkSmartPointer<vtkCellArray> linesV;
    vtkSmartPointer<vtkPolyData> profilePolyDataV;
    vtkSmartPointer<vtkAppendPolyData> appendFilterV;
    vtkSmartPointer<vtkPolyDataMapper> mapperV;
    vtkSmartPointer<vtkActor> actorV;
    vtkSmartPointer<vtkUnsignedCharArray> colorV;

    void initialTailDefineDialog();
    void initialTailTemplateDialog();

    //func

    //void showTailTemplateDialog();


    void saveTailData();

private slots:
    void showTailDefineWidget();
    void saveData();//保存数据
    //void generateTTail();//打开尾翼模板界面
public:
    QVector<wingDefinition>wingDataArray;
    QVector<wingDefinition>tailDataArray;


private:
    wingType saveType;
/*******************************************设计窗口显示*****************************************/
    //PUBLIC
    QComboBox *wingListCombox;
    QGridLayout *analyseLayout;//求解器设置
    QGroupBox *analyseBox;
    QLabel *wingSolverLabel;
    QLabel *wingListLabel;
    QComboBox *wingSolverCombox;
    QLabel *wingVelocitySetLabel;
    QLineEdit *wingVelocitySetEdit;
    QLabel *wingHeightSetLabel;
    QLineEdit *wingHeightSetEdit;
    QLabel *wingLiftSetLabel;
    QLineEdit *wingLiftSetEdit;


 /********************************/

    QWidget *chartWWidget;
    QDialog *wingSettingDialog;//机翼求解参数设置
    QDialog *zeroLiftDragChoiceDialog;//零升阻力设置




    airfoilDisplay *defineAirfoil;





    QGridLayout *zeroLiftDragLayout;

    QVBoxLayout *designMainVLayout;
    QHBoxLayout *listHboxLayout;
    QVBoxLayout *listVboxLayout;


    QGridLayout *wingSettingLayout;
    QLabel *wingVelocityLabel;QLineEdit *wingVelocityEdit;QLabel *wingVelocityUnitLabel;
    QLabel *wingHeightLabel;QLineEdit *wingHeightEdit;QLabel *wingHeightUnitLabel;
    QLabel *wingCgLocationLabel; QLineEdit *wingCgLocationEdit;QCheckBox *wingCgLocationCheckBox;

    QLabel *wingMinAlphaLabel;QLineEdit *wingMinAlphaEdit;QLabel *wingMaxAlphaLabel;QLineEdit *wingMaxAlphaEdit;QLabel *wingStepAlphaLabel;QLineEdit *wingStepAlphaEdit;
    QPushButton *saveWingSettingButton; QPushButton *cancelWingSettingButton;



    QVector<QPushButton*>dragButtonArray;
    QLineEdit *dragEdit;

    QPushButton *saveDragButton;           QPushButton *cancelDragButton;
 /*******************流线设置******************/

    QDialog *streamSettingDialog;//流线设置
    QDialog *dragChoiceDialog;//粘性阻力计算类型
    double zeroLiftDragValueArray[10];
    QLabel *streamLabelArray[4];
    QLineEdit *streamEditArray[4];
    //粘性计算阻力选项
    QButtonGroup *dragBoxGroup;
    QCheckBox *dragBoxA;
    QCheckBox *dragBoxB;

    void initialStreamDialog();
    void initialChoiceDragDialog();
    void setZeroLiftDrag();

    QDialog *progressDialog;
    QProgressBar *dialogBar;





    void initialProgressDialog();
    void startCalculation();

private slots:
    void showWingStreamLine();
    void changeZeroLiftDragText();
    void onCalculationFinished();
    void updateProgress(int value);
    void cancelDragDialog();
    void saveDragDialog();
private:


    QVector<VLMSetting>wingSettingArray;
    //QGridLayout *wingChartLayout;
    QVBoxLayout *wingChartVLayout;
    PlotWidget *chartWA;
    PlotWidget *chartWB;
    PlotWidget *chartWC;
    PlotWidget *chartWD;
    PlotWidget *chartViewWA;
    PlotWidget *chartViewWB;
    PlotWidget *chartViewWC;
    PlotWidget *chartViewWD;
    QVector<PlotSeries*>wingResultSeriesA;
    QVector<PlotSeries*>wingResultSeriesB;
    QVector<PlotSeries*>wingResultSeriesC;
    QVector<PlotSeries*>wingResultSeriesD;
    //添加曲线
    PlotSeries *autoSeriesA;
    PlotSeries *autoSeriesB;
    PlotSeries *autoSeriesC;
    PlotSeries *autoSeriesD;

    PlotAxis *axisXWA;
    PlotAxis *axisYWA;
    PlotAxis *axisXWB;
    PlotAxis *axisYWB;
    PlotAxis *axisXWC;
    PlotAxis *axisYWC;
    PlotAxis *axisXWD;
    PlotAxis *axisYWD;
    QVector<wingVLM*>VLMSolverArray;


    int wingIndex = 0;      //第N个机翼
    int wingChoiceIndex = 0;//选中机翼的索引
    int tailIndex = 0;      //第N个尾翼
    int tailChoiceIndex = 0;//选中尾翼的索引

    bool isModify = false;
    myMath functionMath;
    QVector<QColor>colorArray;
    QVector<int>resultPenArray;
    //翼型修改
    int cstNum = 6;
    int airfoilIndex = 0;
    QVector<double>cstPointX;
    QVector<double>cstPointY;
private slots:
    void changeAirfoilData(const QVector<QVector<double>>&,const QVector<double>&);
    void updateIterText(const std::vector<double>&);
    void resetAirfoilData();
    //

    void addDataPoint();
    void clearDataPoint();

    void addDataPointA();
    void addDataPointB();
    void addDataPointC();
    void addDataPointD();
    void clearDataPointA();
    void clearDataPointB();
    void clearDataPointC();
    void clearDataPointD();

private:
    //切换左侧视图
    viewType modelType;//当前界面类型
    QString curveIconA1,curveIconB1;
    QString curveIconA2,curveIconB2;
    QString curveIconA3,curveIconB3;
    hoverButton *changeModelViewButton;
    hoverButton *changeResultViewButton;
    hoverButton *changeSpanResultViewButton;
    QComboBox *wingAlphaCombox;
    QLabel *wingMessageTextLabel;
    QVBoxLayout *wingWidgetVLayout;//切换机翼编辑
    QHBoxLayout *viewChangeHLayout;//切换结果显示
    void removeSeriesLegendItem(PlotWidget *chart, PlotSeries *series);
    //曲线类型更改
    QDialog *seriesStyleDialog;
    QColorDialog *colorDialog;
    QPushButton *colorButton;
    //QGroupBox *seriesTypeBox;
    //曲线右键菜单
    QMenu *rChartMenu;
    QVector<QAction*>actionArray;
    int chartIndex = 0;
    int chartTypeIndexArray[4];
    int axisIndex[15][2];
    QString axisXName[15];
    QString axisYName[15];
    QString titleName[15];
    int xAxisIndexA;int yAxisIndexA;
    int xAxisIndexB;int yAxisIndexB;
    int xAxisIndexC;int yAxisIndexC;
    int xAxisIndexD;int yAxisIndexD;
    QVector<QVector<QVector<double>>>resultArray;//当前机翼结果数据
private:
    //展向结果展示
    PlotWidget *spanChart;
    PlotWidget *spanChartView;
    PlotSeries *spanSeriesA;
    PlotSeries *spanSeriesB;
    PlotAxis *spanXAxis;
    PlotAxis *spanYAxis;
    int spanResultTypeIndex = 0;

    QMenu *spanRMenu;
    QVector<QAction*>spanActionArray;

    void initialSpanChart();
    void drawSpanResult(const int);

private:

    void initialWingDesignWidget();
    void initialWingSettingDialog();
    void initialWingAddZeroLiftDragDialog();


    void initialChartWA();
    void initialChartWB();
    void initialChartWC();
    void initialChartWD();

    void initialSeriesStyleDialog();


    void initialRChartMenu();
    void initialSpanRChartMenu();
    void initialSteup();





    QPen getPen(const int);

    void getWingSetting();


    void saveWingData();
    void deleteWingData();


    void changeWingDisplay(QTreeWidgetItem*, int);           //切换视图
    void changeWingData(QTreeWidgetItem*, int);              //更改模型
    void changeTailData(QTreeWidgetItem*, int);
    void showWingSettingVinfDialog(QTreeWidgetItem*, int);   //打开入口设置
    void showWingZeroLiftDragDialog(QTreeWidgetItem*, int);  //打开零升阻力系数设置
    void showAirfoilDesignDialog(QTreeWidgetItem*, int);     //打开翼型修改栏
    void showWingStreamLineView(QTreeWidgetItem*, int);
    void showWingPrssureContourView(QTreeWidgetItem*, int);
    void setWingXfoilDrag(QTreeWidgetItem*, int);
    void showSeriesTypeDialog(QTreeWidgetItem*, int);
    void closeOtherRoots(QTreeWidgetItem*, int);
    QTreeWidgetItem* findNodeByIdentifier(QTreeWidgetItem*, const QString&);
    void uncheckNodeByIdentifier(QTreeWidget*, const QString&);

    void changeTailDisplay(QTreeWidgetItem*, int);

    void changeUIForWingDesign();
    void changeUIForWingResult();
    void changeUIForWingSpanResult();
    void changeUIForWingIter();

    void showResultNode(QTreeWidgetItem *, const QString &);
    void hideResultNode(QTreeWidgetItem *, const QString &);

    void drawResultChartA(const int);
    void drawResultChartB(const int);
    void drawResultChartC(const int);
    void drawResultChartD(const int);

    void exportChartData(PlotWidget *chart);

    void updateAxes(PlotWidget *, const QVector<PlotSeries*> &);












    QVTKOpenGLNativeWidget *wingDisplayWidgetB;
    vtkSmartPointer<vtkRenderer> rendererB;
    vtkSmartPointer<vtkGenericOpenGLRenderWindow> renwinB;

    vtkSmartPointer<vtkSphereSource> sphereSourceCg;
    vtkSmartPointer<vtkPolyDataMapper> mapperCg;
    vtkSmartPointer<vtkActor> actorCg;


    vtkSmartPointer<vtkPoints> pointsB;
    vtkSmartPointer<vtkCellArray> linesB;
    vtkSmartPointer<vtkPolyData> profilePolyDataB;
    vtkSmartPointer<vtkAppendPolyData> appendFilterB;
    vtkSmartPointer<vtkPolyDataMapper> mapperB;
    vtkSmartPointer<vtkActor> actorB;
    vtkSmartPointer<vtkUnsignedCharArray> colorB;


    vtkSmartPointer<vtkPoints> pointsS;
    vtkSmartPointer<vtkCellArray> linesS;
    vtkSmartPointer<vtkPolyData> profilePolyDataS;
    vtkSmartPointer<vtkPolyDataMapper> mapperS;
    vtkSmartPointer<vtkActor> actorS;

    vtkSmartPointer<vtkPoints> pointsC;
    vtkSmartPointer<vtkCellArray> linesC;
    vtkSmartPointer<vtkPolyData> profilePolyDataC;

    vtkSmartPointer<vtkPolyDataMapper> mapperC;
    vtkSmartPointer<vtkActor> actorC;
    vtkSmartPointer<vtkUnsignedCharArray> colorC;

    //vtkSmartPointer<vtkLightKit> lightKitB;

    vtkSmartPointer<vtkOrientationMarkerWidget>orientationMarkerB;

    vtkSmartPointer<vtkTextActor> textActorA;
    vtkSmartPointer<vtkTextProperty> textProperty;




    //平移阵列
    void CreateTranslatedArray(vtkSmartPointer<vtkRenderer>,vtkSmartPointer<vtkPolyData>);
    void updateWingDefineWidget(const wingDefinition&);
public slots:
    void updateAirfoilArray(const QVector<QVector<QVector<double>>>&,const QVector<QString>&,const QVector<int>&);
    void updateAirfoilInterArray(const QVector<QVector<airfoilData>>&);
    void showColorDialog();
    void hideSeriesStyleDialog();
    void changeSeriesStyle(const int);
    void changeSeriesColor(const QColor&);

    void showChartAMenu();
    void showChartBMenu();
    void showChartCMenu();
    void showChartDMenu();
    void showSpanChartMenu();
    void changeResultType();
    void changeSpanResultType();

    void onCheckboxStateChanged(int state);//重心状态
    void changeCgLocation();






/*****************************************机翼输出参数设置****************************************/
/***********************************************涡格******************************************************/

    //wingVLM vlmModel;


private:
    int wingTypeIndex;

    void initialWingDesignerModel();
    void initialWingShowModel();


    void removeWidget();
    void generateMultiWing();
    void generateSimpleWing();
    void updateWingDesignerModel();
    void updateComboBox(QComboBox*, const QStringList&);//更新Qcombobox
    void initialWingDefineWidget();



private slots:
    void defineWingTypes();
    void generateWing();
    void generateEllipticalWing();
    void changeProgressUpdate(const int);


/*******************************************机翼优化窗口*****************************************/
private:

    wingDefinition bestWingData;
    myMath mathSolver;
    void initialWingOptimizationWidget();
    QGroupBox *valueSettingOptBox;
    QGroupBox *geometrySettingOptBox;
    QGridLayout *valueSettingOptGLayout;
    QGridLayout *geometrySettingOptGLayout;
    //模型参数
    QSpinBox *numsSpinBox;
    QLabel *optLabel[5];
    QLabel *optValLabel[5];
    QComboBox *optCombox[5];
    SwitchButton *optSwithButton[5];
    RangeSlider *optSlider[5];
    QLabel *optMinValLabel[5];
    QLabel *optMaxValLabel[5];
    int comboxMaxVal[5][3];
    int comboxMinVal[3];
    double minVal[5];
    QString unitVal[5];
    //算法参数
    QLabel *optSettingLabel[8];
    QLineEdit *optSettingEdit[8];
    double optMinVal[8];
    double optMaxVal[8];
    QString warningEditStyle;
    QString nothingEditStyle;
    //模型参数
    QVector<WingGaParameters>optSettingArray;
    QVector<WingGaParameters>optRealSettingArray;
    QVector<wingDefinition>optWingDataArray;



    wingOptimization *optModel = nullptr;
    WingGaParameters optSetting;

    int wingWorkNum;
    int wingThreadNum = 10;
    //result
    QVector<double>clArray;
    QVector<double>cdArray;
    QVector<double>cmArray;
    QVector<QVector<point3d>>xABArray;
    QVector<QVector<point3d>>xBCArray;
    QVector<QVector<point3d>>xCDArray;
    QVector<QVector<point3d>>xDAArray;
    QVector<QVector<double>>colorContourArray;
    QVector<QVector<double>>xSpan;
    QVector<QVector<double>>xSpanLiftA;
    QVector<QVector<double>>xSpanLiftB;
    int meshNum;
    bool isChangeRealVal;



private:
    void finishWingOptimization();
    //slots
    void optMinLabelChanged(int);
    void optMaxLabelChanged(int);
    void optChangeVal(int);
    void optRangeChange(const int);
    void updateOptSetting();
    void updateGeometryOptSetting();
    void setIsChange(bool);
    //
    void changeSliderType(const int,const int);
    //
    void showAllWidgetsInBox(QGroupBox *);
    void hideAllWidgetsInBox(QGroupBox *);
    //func
    void startVLMInThread(int,int);
    void updatePressureContourView(QVector<point3d>,QVector<point3d>,
                                   QVector<point3d>,QVector<point3d>,const QVector<double>&,const int);

    bool checkOptSetting();
    int getValueIndex(const double);
    void saveGeometryOptSetting(const int);//

    void changeAlphaView(const int);
    void updateOptUI(wingDefinition&);
    //改变GL内文本
    void updateGLTextA(QString);
    QString wingDataToString(wingDefinition&);
    QString resultDataToString(wingDefinition&,const wingVLM&);

    //阻力插值

    // void interDrag();
    // QVector<QVector<double>>dragDataArray;
    // QVector<int>reArray;
    // QVector<double>liftArray;

    // xfoilSetting dragSetting;
    // void startXfoil(const int,const int);

    //nothing
    glWidget *nothingWidget = nullptr;

/*******************************机翼优化迭代窗口*********************************/
private:


    QVBoxLayout *iterVLayout;

    PlotWidget *iterChartA;
    PlotWidget *iterChartB;
    PlotWidget *iterViewA;
    PlotWidget *iterViewB;
    PlotAxis *iterXAxisA;
    PlotAxis *iterYAxisA;
    PlotAxis *iterXAxisB;
    PlotAxis *iterYAxisB;
    PlotSeries *iterSeriesA;
    PlotSeries *iterSeriesB;
    PlotSeries *iterSeriesC;
    QVector<QString>iterTextArray;




    void initialWingIterWidget();
    void drawOptIterSeries(const int ,const double);
    void drawOptSpanLiftSeries(const QVector<double>&,const QVector<double>&,const QVector<double>&);
    void updateOptIterText(const double);
protected:


    //输出
    void outputWingData();
public slots:
    void updateOptimizationTextItem(const int x,const int y);
    void resizeEvent(QResizeEvent *event) override;

};

#endif // WINGDISPLAY_H
