#ifndef AIRPLANEDISPLAY_H
#define AIRPLANEDISPLAY_H

#include <QWidget>
#include <QDialog>
#include <QGroupBox>
#include <QLayout>
#include <QLabel>
#include <QPushButton>
#include <QLineEdit>
#include <QListView>
#include <QStringListModel>
#include <QTreeWidget>
#include <QCheckBox>
#include <QComboBox>
#include <QChart>
#include <QLineSeries>
#include <QScatterSeries>
#include <QValueAxis>
#include <QMenu>
#include <QProgressBar>
#include "publicWidgetClass/callout.h"
#include "PublicClass/mychartview.h"
#include "PublicClass/hoverbutton.h"
#include "WingClass/wingdefinition.h"
#include "PublicClass/structDefinition.h"
#include "WingClass/wingvlm.h"
#include "publicWidgetClass/glwidget.h"
#include "AirPlaneClass/airplanevlm.h"
#include "AirPlaneClass/airplanestability.h"
enum airplaneViewType{
    AIRPLANE_DESIGN,
    AIRPLANE_RESULTA,
    AIRPLANE_RESULTB,
};


class airplaneDisplay : public QWidget
{
    Q_OBJECT
public:
    airplaneDisplay(QWidget *parent = nullptr);
    void showAirplaneDefineDialog();//机翼编辑窗口

    void startAnalyseAirplane();
    void startAnalyseAirplaneStability();

    void changeWindowForAirplaneDesign();

    QVector<airplaneDefinition>airplaneDataArray;



//数据
public slots:
    void updateList(const QVector<wingDefinition>&,const QVector<wingDefinition>&);
    void changeProgressUpdate(const int);
    void updateAirplaneList(QVector<airplaneDefinition>&);


/******************************飞机定义窗口******************************/
private:
    QStringList nameListArray[4];
    bool isModify = false;
private:
    void loadIcon();
    void initialSteup();
    QString redIcon;
    QString greenIcon;
private:

    //翼面数据库
    QVector<wingDefinition>wingDataArray;
    QVector<wingDefinition>tailDataArray;


    QVector<wingDefinition>tTailDataArray;
    QVector<wingDefinition>fuselageDataArray;
    //
    QVector<airplaneVLM*>airplaneSolverArray;
    QVector<airplaneStability*>airplaneStabilityArray;

    QVector<VLMSetting>airplaneSettingArray;

    int airplaneChoiceIndex = 0;
    int airplaneIndex = 0;

private:

    QDialog *listDialog;
    QListView *listView;
    QStringListModel *listModel;
    QVBoxLayout *listLayout;


    int modelIndex = 0;     //模式索引
    int listIndexArray[8];  //选择索引矩阵


    void initialListDialog();
    void initialAirplaneMessage();
    void updateAirplaneMessage();

private:
    //dialog
    QDialog *airplaneDefineDialog;
    QGroupBox *wingBox;
    QGroupBox *fuselageBox;
    QGroupBox *hTailBox;
    QGroupBox *vTailBox;
    QGroupBox *powerBox;
    QGroupBox *messageBox;
    QGridLayout *defineGridLayout;

    //QLabel *textLabel[4];
    //QPushButton *choiceButton[4];
    QGridLayout *gLayout[5];
    QPushButton *readyButton[5];
    QPushButton *choiceButton[5];

    QLabel *xLocationLabel[5];   QLineEdit *xLocationEdit[5];//x距离
    QLabel *zLocationLabel[5];   QLineEdit *zLocationEdit[5];//z距离
    QLabel *aAlphaLabel[5];   QLineEdit *aAlphaEdit[5];//安装角

    //message
    /*
     * 翼展
     * 机翼面积
     * 平尾面积
     * 垂尾面积
     * 平尾容量
     * 垂尾容量
     * 名称 --  XXX
     *
    */
    QLabel *labelArray[20];

    QLineEdit *nameEdit;


    void initialAirplaneDefineDialog();


    //
    void initialAirplaneSettingDialog();
    QDialog *airplaneSettingDialog;
    QGridLayout *airplaneSettingLayout;
    QLabel *airplaneVelocityLabel;QLineEdit *airplaneVelocityEdit;QLabel *airplaneVelocityUnitLabel;
    QLabel *airplaneHeightLabel;QLineEdit *airplaneHeightEdit;QLabel *airplaneHeightUnitLabel;
    QLabel *airplaneCgLocationLabel; QLineEdit *airplaneCgLocationEdit;QCheckBox *airplaneCgLocationCheckBox;
    QLabel *airplaneMinAlphaLabel;QLineEdit *airplaneMinAlphaEdit;QLabel *airplaneMaxAlphaLabel;QLineEdit *airplaneMaxAlphaEdit;QLabel *airplaneStepAlphaLabel;QLineEdit *airplaneStepAlphaEdit;
    QPushButton *saveAirplaneSettingButton; QPushButton *cancelAirplaneSettingButton;

    int getReadyButtonIndex(QPushButton*);
private slots:
    void updateListViewIndex(const QItemSelection &,const QItemSelection &);//更新列表索引
    void showListDialog();
    void setButtonStatus();



    void saveAirplaneData();
    void updateAirplaneDialog();
    void addAirplaneData(airplaneDefinition&);
    //
    void changeAirplaneDisplay(QTreeWidgetItem* item, int column);
    void showAirplaneSettingVinfDialog(QTreeWidgetItem* item, int column);
    void getAirplaneSetting();
    void onCheckboxStateChanged(int state);//重心状态

    void changeUIForAirplaneDesign();
    void changeUIForAirplaneResult();
    void changeUIForAirplaneSpanResult();

    void changeCgLocation();
    void changeSpanResultType();
    void exportChartData(QChart *chart);

    void updateGLTextA(QString);



    //


private:
    airplaneViewType modelType;

    QWidget *chartWWidget;

    //结果视图
    void initialChartWA();
    void initialChartWB();
    void initialChartWC();
    void initialChartWD();
    QPen getPen(const int);
    void drawResultChartA(const int);
    void drawResultChartB(const int);
    void drawResultChartC(const int);
    void drawResultChartD(const int);
    void drawSpanResult(const int);

    void addTreeNode(const QString name);
    void showResultNode(QTreeWidgetItem *, const QString &);
    void hideResultNode(QTreeWidgetItem *, const QString &);
    void closeOtherRoots(QTreeWidgetItem*, int);

    void updateAxes(QChart*,const QVector<QLineSeries*>&);



    Callout *toolTipA;
    Callout *toolTipB;
    Callout *toolTipC;
    Callout *toolTipD;

    QVector<QColor>colorArray;
    QVector<int>resultPenArray;

    //曲线右键菜单
    void removeSeriesLegendItem(QChart *chart, QLineSeries *series);
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

    QVBoxLayout *airplaneChartVLayout;
    QChart *chartWA;
    QChart *chartWB;
    QChart *chartWC;
    QChart *chartWD;
    MyChartView *chartViewWA;
    MyChartView *chartViewWB;
    MyChartView *chartViewWC;
    MyChartView *chartViewWD;
    QVector<QLineSeries*>airplaneResultSeriesA;
    QVector<QLineSeries*>airplaneResultSeriesB;
    QVector<QLineSeries*>airplaneResultSeriesC;
    QVector<QLineSeries*>airplaneResultSeriesD;
    //添加曲线
    QLineSeries *autoSeriesA;
    QLineSeries *autoSeriesB;
    QLineSeries *autoSeriesC;
    QLineSeries *autoSeriesD;

    QValueAxis *axisXWA;
    QValueAxis *axisYWA;
    QValueAxis *axisXWB;
    QValueAxis *axisYWB;
    QValueAxis *axisXWC;
    QValueAxis *axisYWC;
    QValueAxis *axisXWD;
    QValueAxis *axisYWD;



    QString curveIconA1,curveIconB1;
    QString curveIconA2,curveIconB2;
    QString curveIconA3,curveIconB3;
    hoverButton *changeModelViewButton;
    hoverButton *changeResultViewButton;
    hoverButton *changeSpanResultViewButton;
    QComboBox *airplaneAlphaCombox;
    QLabel *airplaneMessageTextLabel;
    QVBoxLayout *airplaneWidgetVLayout;//切换机翼编辑
    QHBoxLayout *viewChangeHLayout;//切换结果显示

    void showChartAMenu();
    void showChartBMenu();
    void showChartCMenu();
    void showChartDMenu();
    void showSpanChartMenu();
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
    //展向结果展示

    QChart *spanChart;
    MyChartView *spanChartView;
    QLineSeries *spanSeriesA;
    QLineSeries *spanSeriesB;
    QValueAxis *spanXAxis;
    QValueAxis *spanYAxis;
    int spanResultTypeIndex = 0;

    QMenu *spanRMenu;
    QVector<QAction*>spanActionArray;

    void initialSpanChart();
    void initialSpanRChartMenu();
    void initialRChartMenu();
    //void drawSpanResult(const int);

signals:
    void emitWingDefineDialog(int ,int);
    void emitAirplaneProgressBarValue(const int);
public slots:
    void updateAirplaneDefineWidget();//更新视图


/**************************飞机显示窗口****************************/

private:
    void initialAirplaneDesignWidget();
    void initialAirplaneShowModel();

    void addVTailToVTK();
    void addHTailToVTK();
    void addTTailToVTK();
    void addWingToVTK();
    void addFuselageToVTK();

    void updateAirplanePressureContourView();
    void updateStreamLineView();
    void changeAlphaView(const int);
    void updateWingPressureContour();
    void updateHTailPressureContour();
    void updateVTailPressureContour();
    void hideStreamLineView();

    void hidePressureContourView();



    void showAirplaneCompoentDefineWidget(QTreeWidgetItem*, int);//显示飞机部件编辑而界面
    void showAirplanePressureContour(QTreeWidgetItem*, int);//显示压力
    void showAirplaneStreamLineView(QTreeWidgetItem*, int);
    void setAirplaneXfoilDrag(QTreeWidgetItem*, int);
    void createTranslatedArray(vtkSmartPointer<vtkRenderer>,vtkSmartPointer<vtkPolyData>);
    void showAirplaneZeroLiftDragDialog(QTreeWidgetItem*,int);
    QTreeWidgetItem* findNodeByIdentifier(QTreeWidgetItem*, const QString&);
    QString airplaneDataToString(wingDefinition&);
    QWidget *airplaneDesignWidget;
    QTreeWidget *airplaneTreeWidget;
    QVector<QTreeWidgetItem*>airplaneTreeItemArray;

    void changeAirplaneData(QTreeWidgetItem*, int);

    //总布局
    QHBoxLayout *airplaneHLayout;
    QVBoxLayout *airplaneVLayout;

    vtkSmartPointer<vtkSphereSource> sphereSourceCg;
    vtkSmartPointer<vtkPolyDataMapper> mapperCg;
    vtkSmartPointer<vtkActor> actorCg;


    QVTKOpenGLNativeWidget *airplaneDisplayWidgetA;
    vtkSmartPointer<vtkRenderer> renderer;
    vtkSmartPointer<vtkGenericOpenGLRenderWindow> renwin;


    vtkSmartPointer<vtkPoints> pointsWing;
    vtkSmartPointer<vtkCellArray> linesWing;
    vtkSmartPointer<vtkPolyData> profilePolyDataWing;
    vtkSmartPointer<vtkAppendPolyData> appendFilterWing;
    vtkSmartPointer<vtkPolyDataMapper> mapperWing;
    vtkSmartPointer<vtkActor> actorWing;
    vtkSmartPointer<vtkUnsignedCharArray> colorWing;
    vtkSmartPointer<vtkTransform>transformWing;

    vtkSmartPointer<vtkPoints> pointsContourWing;
    vtkSmartPointer<vtkCellArray> linesContourWing;
    vtkSmartPointer<vtkPolyData> profilePolyDataContourWing;
    vtkSmartPointer<vtkPolyDataMapper> mapperContourWing;
    vtkSmartPointer<vtkActor> actorContourWing;
    vtkSmartPointer<vtkUnsignedCharArray> colorContourWing;


    vtkSmartPointer<vtkPoints> pointsVTail;
    vtkSmartPointer<vtkCellArray> linesVTail;
    vtkSmartPointer<vtkPolyData> profilePolyDataVTail;
    vtkSmartPointer<vtkAppendPolyData> appendFilterVTail;
    vtkSmartPointer<vtkPolyDataMapper> mapperVTail;
    vtkSmartPointer<vtkActor> actorVTail;
    vtkSmartPointer<vtkUnsignedCharArray> colorVTail;
    vtkSmartPointer<vtkTransform>transformVTail;

    vtkSmartPointer<vtkPoints> pointsContourVTail;
    vtkSmartPointer<vtkCellArray> linesContourVTail;
    vtkSmartPointer<vtkPolyData> profilePolyDataContourVTail;
    vtkSmartPointer<vtkPolyDataMapper> mapperContourVTail;
    vtkSmartPointer<vtkActor> actorContourVTail;
    vtkSmartPointer<vtkUnsignedCharArray> colorContourVTail;


    vtkSmartPointer<vtkPoints> pointsHTail;
    vtkSmartPointer<vtkCellArray> linesHTail;
    vtkSmartPointer<vtkPolyData> profilePolyDataHTail;
    vtkSmartPointer<vtkAppendPolyData> appendFilterHTail;
    vtkSmartPointer<vtkPolyDataMapper> mapperHTail;
    vtkSmartPointer<vtkActor> actorHTail;
    vtkSmartPointer<vtkUnsignedCharArray> colorHTail;
    vtkSmartPointer<vtkTransform>transformHTail;

    vtkSmartPointer<vtkPoints> pointsContourHTail;
    vtkSmartPointer<vtkCellArray> linesContourHTail;
    vtkSmartPointer<vtkPolyData> profilePolyDataContourHTail;
    vtkSmartPointer<vtkPolyDataMapper> mapperContourHTail;
    vtkSmartPointer<vtkActor> actorContourHTail;
    vtkSmartPointer<vtkUnsignedCharArray> colorContourHTail;


    vtkSmartPointer<vtkPoints> pointsTTail;
    vtkSmartPointer<vtkCellArray> linesTTail;
    vtkSmartPointer<vtkPolyData> profilePolyDataTTail;
    vtkSmartPointer<vtkAppendPolyData> appendFilterTTail;
    vtkSmartPointer<vtkPolyDataMapper> mapperTTail;
    vtkSmartPointer<vtkActor> actorTTail;
    vtkSmartPointer<vtkUnsignedCharArray> colorTTail;
    vtkSmartPointer<vtkTransform>transformTTail;

    vtkSmartPointer<vtkPoints> pointsFuselage;
    vtkSmartPointer<vtkCellArray> linesFuselage;
    vtkSmartPointer<vtkPolyData> profilePolyDataFuselage;
    vtkSmartPointer<vtkAppendPolyData> appendFilterFuselage;
    vtkSmartPointer<vtkPolyDataMapper> mapperFuselage;
    vtkSmartPointer<vtkActor> actorFuselage;
    vtkSmartPointer<vtkUnsignedCharArray> colorFuselage;
    vtkSmartPointer<vtkTransform>transformFuselage;
    //流线
    vtkSmartPointer<vtkPoints> pointsS;
    vtkSmartPointer<vtkCellArray> linesS;
    vtkSmartPointer<vtkPolyData> profilePolyDataS;
    vtkSmartPointer<vtkPolyDataMapper> mapperS;
    vtkSmartPointer<vtkActor> actorS;


    vtkSmartPointer<vtkLightKit> lightKit;
    vtkSmartPointer<vtkOrientationMarkerWidget>orientationMarkerA;


    vtkSmartPointer<vtkTextActor> textActorA;
    vtkSmartPointer<vtkTextProperty> textProperty;
    /*******************流线设置******************/

    QDialog *streamSettingDialog;//流线设置
    QDialog *dragChoiceDialog;//粘性阻力计算类型

    QLabel *streamLabelArray[4];
    QLineEdit *streamEditArray[4];
    void initialStreamDialog();
    void initialChoiceDragDialog();
    void setZeroLiftDrag();


    //粘性计算阻力选项
    QButtonGroup *dragBoxGroup;
    QCheckBox *dragBoxA;
    QCheckBox *dragBoxB;






    QDialog *progressDialog;
    QProgressBar *dialogBar;

    void initialAirplaneAddZeroLiftDragDialog();

    QDialog *zeroLiftDragChoiceDialog;//零升阻力设置
    QGridLayout *zeroLiftDragLayout;
    double zeroLiftDragValueArray[10];
    QVector<QPushButton*>dragButtonArray;
    QLineEdit *dragEdit;

    QPushButton *saveDragButton;           QPushButton *cancelDragButton;





    void initialProgressDialog();
    void startCalculation();
private slots:
    void showAirplaneStreamLine();
    void onCalculationFinished();
    void changeZeroLiftDragText();
    void updateProgress(int value);
    void cancelDragDialog();
    void saveDragDialog();
    void changeResultType();


};

#endif // AIRPLANEDISPLAY_H
