#ifndef PROPELLERDISPLAY_H
#define PROPELLERDISPLAY_H


#include <QObject>
#include <QWidget>
#include <QDockWidget>
#include <QChart>
#include <QValueAxis>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLineSeries>
#include <QScatterSeries>
#include <QColorDialog>
#include <QGroupBox>
#include <QMenu>
#include <QTextEdit>
#include <QSpinBox>
#include <QLegendMarker>
#include <QTableWidget>
#include <QTreeWidget>
#include <QCheckBox>
#include <QProgressDialog>
#include <QTimer>
#include "AirfoilClass/airfoildesign.h"   //插值APC模板翼型用
#include "PublicClass/mychartview.h"
#include "PublicClass/structDefinition.h"
#include "PublicClass/mymath.h"
#include "PublicClass/hoverbutton.h"
#include "propellersClass/propellerlibary.h"
#include "propellersClass/propellerdesign.h"
#include "propellersClass/propellervlm.h"
#include "propellersClass/propellerbemt.h"
#include "publicWidgetClass/glwidget.h"
#include "publicWidgetClass/callout.h"
#include "publicWidgetClass/datapointdialog.h"
enum resultChartType {
    PROPVT,//推力曲线A
    PROPRT,//推力曲线B
    PROPJN,//效率曲线A
    PROPVN,//效率曲线B
    PROPJCT,//推力系数曲线A
    PROPVCT,//推力系数曲线B
    PROPVW,//功率曲线
    PROPVM,//力矩曲线
};
struct resultWidget
{
    resultWidget() {}
};
struct propRPMData;
class propellerDisplay:public QWidget
{
    Q_OBJECT
public:
    propellerDisplay(QWidget *parent = nullptr);
    ~propellerDisplay();


    void initialText(const QVector<QString>&);
    void showPropLibary();

    void startAnalyseProp();
    QWidget *propResultDisplayWidget;
    QWidget *propAnalyseWidget;

    QWidget *propDesignWidget;
    void startSolve();
    QVector<wingDefinition>propArray;


private:
    void initialSteup();
    /******************************加载语言********************************/
    QString axisXName[30];
    QString axisYName[30];
    QString titleName[30];
    /*********************************风格********************************/
    void loadStyleSheet();

    QString buttonStyle;
    QString progressBarStyle;
    QString groupBoxStyle;
    /*******************************加载纹理******************************/
    void loadIcon();
    QString checkIcon;
    QString removeIcon;



    QColorDialog *colorDialog;
    QVector<QColor>propColorArray;
    /*******************************螺旋桨定义*******************************/


private:
    //定义默认数据
    double propRh = 0.01;
    int SpanNumB = 2;
    int propNumB = 2;
    int xGridNum = 10;
    int yGridNum = 10;
    double propDiameter = 0.6;
    double propPitch = 10;
    //



    void initialPropDesignerModel();
    void initialPropShowModel();
    void buildPropDefineDialog();
    //OpenGL
    QWidget *propDefineWidget;
    QGroupBox *propDefineBox;
    QScrollArea *scrolArea;

    QHBoxLayout *propDefineHLayout;
    QVBoxLayout *propDefineVLayout;
    QGridLayout *propDefineGLayout;
    QLabel *spanLabel; QLabel *chordLengthLabel; QLabel *twistAngleLabel;
    QLabel *offsetLengthLabel;QLabel *xGridNumLabel;QLabel *yGridNumLabel;
    QLabel *nameLabel;
    QVector<QString>propAirfoilNameArray;
    //propeller Design
    QVector<QLineEdit*>profileSpanEdit;
    QVector<QLabel*>profileLabel; QVector<QLineEdit*>profilechordLengthEdit;
    QVector<QLineEdit*>profileOffsetLengthEdit;QVector<QComboBox*>profileAirfoilChoiceCombobox;
    QVector<QLineEdit*>profileTwistAngleEdit;
    QVector<QLineEdit*>profileXGridEdit;QVector<QLineEdit*>profileYGridEdit;
    //propellerDefine
    QDialog *propDefineDialog;
    QGridLayout *propDefineGLayoutB;
    QLabel *propDiameterLabel;
    QLabel *propDiameterLabelA;QLineEdit *propDiameterEditA;
    QLabel *propDiameterLabelB;QLineEdit *propDiameterEditB;
    QLabel *propPitchLabel;
    QLabel *propPitchLabelA;QLineEdit *propPitchEditA;
    QLabel *propPitchLabelB;QLineEdit *propPitchEditB;

    QLabel *XGridNumLabel; QLineEdit *XGridNumEdit;
    QLabel *YGridNumLabel; QLineEdit *YGridNumEdit;
    QLabel *propRhLabel; QLineEdit *propRhEdit;//桨毂
    QLabel *spanNumLabelB; QLineEdit *spanNumEditB;
    QLabel *propInputModelLabel;QCheckBox *propInputModelCheckbox;
    QComboBox *propListCombox;
    QPushButton *saveButton; QPushButton *cancelButton;
    //propellerMessage
    QGroupBox *propMessageBox;
    QGridLayout *propMessageLayout;
    QLabel *propRealDiameterLabel; QLabel *propRealDiameterTextLabel;
    QLabel *propRealMeshNumLabel; QLabel *propRealMeshNumTextLabel;
    QLabel *propSoildityLabel; QLabel *propSoildityTextLabel;
    QLabel *areaLabel; QLabel *areaTextLabel;

    QSpacerItem *verticaSpacer;//拉伸因子
    QLabel *propNumLabel; QLineEdit *propNumEdit;
    QLabel *propMeshVTypeLabel; QComboBox *propMeshVTypeCombobox;
    QLabel *propMeshUTypeLabel; QComboBox *propMeshUTypeCombobox;
    QLabel *propYteTypeLabel; QLabel *propYteValueLabel;
    QComboBox *propYteTypeCombobox; QLineEdit *propYteValueEdit;
    QLabel *propNameLabel; QLineEdit *propNameEdit;
    QPushButton *propSaveButton; QPushButton *propOutputButton;
    //处理从螺旋桨模型库导入
    bool isFormAPC = false;
    bool isAPCOK = true;
    QVector<QVector<QVector<double>>>APCAirfoilArray;
    //

    void removeWidget();
    //void generateProp();
    void updatePropDesignerModel();
    void buildPropDesignerModel();//创建螺旋桨定义列表
    void initialPropMessage();

    void generatePropModelA();
    void generatePropModelB();
    void updateDefineView(const wingDefinition&);
    wingDefinition getPropDefine();

//func
    QVector<int> findMatchingIds(const QVector<QString>&, const QStringList&) ;//找到对应的翼型
signals:
private slots:
    void showPropDefineWidget();
    void updatePropData();
    void updateInput();//刷新输入参数
    void updateInputState();//刷新输入状态
    void outputAnalysePropData();

public slots:
    void showPropDefineDialog();
    void updatePropList(QVector<wingDefinition>&);



/*****************************************螺旋桨气动力计算与可视化******************************************/
private:





    QVector<propellerVLM*>VLMSolverArray;
    QVector<QVector<QVector<double>>>propResultArray;
    QVector<bool>propResultTypeArray;//求解器类型
    QVector<QStringList>propAirfoilListArray;
    propellerBemt *bemtSolver;

    QVector<propVLMSetting>VLMSettingArray;
    QStringList airfoilList;

    propVLMSetting initialVLMSetting;

    bool isModify = false;
    int propellerIndex = 0;
    int propChoiceIndex = 0;
    int propStateIndex = 0;//状态索引

    //求解参数设置
    QDialog *propSettingDialog;
    QVBoxLayout *propSettingVLayout;
    QGridLayout *propSettingLayout;
    QHBoxLayout *propSettingHLayoutA;
    QLabel *heightLabel;
    QLineEdit *heightEdit;
    QLabel *wakeStepLabel;
    QLineEdit *wakeStepEdit;
    QLabel *matrixLabel;
    QLabel *symbolLabel;
    QLineEdit *rowsEdit;
    QLineEdit *columnsEdit;
    QPushButton *updateSettingButton;

    QLabel *propSolverModelLabel;
    QComboBox *propSolverModelCombox;
    QLabel *propTipLabel;
    QCheckBox *propTipCheckbox;//翼尖修正

    QVector<QLabel*>propStateLabelArray;//状态点显示矩阵
    QVector<QLabel*>velocityLabelArray;
    QVector<QLabel*>rpmLabelArray;
    QVector<QLineEdit*>velocityEditArray;
    QVector<QLineEdit*>rpmEditArray;
    QPushButton *saveSettingButton;
    QPushButton *cancelSettingButton;
    QHBoxLayout *propSettingHLayoutB;
    //
    QTreeWidget *propTreeWidget;
    QVector<QTreeWidgetItem*>propTreeItemArray;
    QVBoxLayout *analyseMainVLayout;
    QHBoxLayout *listHboxLayout;
    QVBoxLayout *listVboxLayout;
    //切换左侧视图
    QPushButton *changeModelViewButton;
    QPushButton *changeResultViewButton;
    QLabel *propMessageTextLabel;
    QVBoxLayout *propWidgetVLayout;//切换机翼编辑
    QHBoxLayout *viewChangeHLayout;//切换结果显示

    QMenu *propChartMenu;
    QVector<QAction*>propActionArray;
    int propChartTypeIndex = 0;



    void initialPropAnalyseWidget();
    void initialPropDefineWidget();
    void initialPropSettingDialog();

    void updatePropDefineWidget(const wingDefinition&);

    void updatePropEdit(const int);
    void updateStreamLineView(const propellerVLM*);

    QString resultToString();
    //fun
    int getAirfoilIndex(const QString);
    void hideStreamLineView();
    void hidePressureContourView();
    void updatePressureContourView(const propellerVLM*);
    void updateGLTextA(QString);

    void hidePropActor();
    void showPropActor();
    //qtreewidget


    void addItemsToResultB();
    void addCheckboxItems(QTreeWidgetItem*, const QStringList &);
    void removeCheckboxItems(QTreeWidgetItem*, const QStringList&);
    void removeItemsToResultB();


    void initialPropChartMenu();

    //void exportChartData(QChart *chart);

    void addPropellerData(wingDefinition&);
private slots:
    void savePropellerData();
    void updatePropSettingState();
    void updateButtonState();
    void updateLabelState();
    void updateViewText();
    void getPropSetting();//求解设置保存
    void updateVTipCheckbox();


    void changePropDisplay(QTreeWidgetItem*, int);
    void changePropData(QTreeWidgetItem*, int);
    void showPropSettingVinfDialog(QTreeWidgetItem*, int);
    void showPropStreamLineView(QTreeWidgetItem*, int);
    void showPropPressureContourView(QTreeWidgetItem*, int);
    void showResultNode(QTreeWidgetItem *, const QString &);
    void hideResultNode(QTreeWidgetItem *, const QString &);
    void changeUIForPropDesign();//切换窗口至模型界面
    void changeUIForPropResult();
    void showPropStreamLine();

    void changePropResultType();
    void addPropDataPoint();
    void clearPropDataPoint();
    void showChartMenu(const QPoint&);


//


private:
    //opengl
    QVTKOpenGLNativeWidget *propDisplayWidgetE;
    vtkSmartPointer<vtkRenderer> rendererE;
    vtkSmartPointer<vtkGenericOpenGLRenderWindow> renwinE;

    vtkSmartPointer<vtkSphereSource> sphereSource;
    vtkSmartPointer<vtkPolyDataMapper> mapperD;
    vtkSmartPointer<vtkActor> actorD;


    vtkSmartPointer<vtkPoints> pointsE;
    vtkSmartPointer<vtkCellArray> linesE;
    vtkSmartPointer<vtkPolyData> profilePolyDataE;
    vtkSmartPointer<vtkAppendPolyData> appendFilterE;
    vtkSmartPointer<vtkPolyDataMapper> mapperE;
    vtkSmartPointer<vtkActor> actorE;
    vtkSmartPointer<vtkUnsignedCharArray> colorE;


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
    vtkSmartPointer<vtkUnsignedCharArray> colors;

    //vtkSmartPointer<vtkLightKit> lightKitB;

    vtkSmartPointer<vtkOrientationMarkerWidget>orientationMarkerE;

    vtkSmartPointer<vtkTextActor> textActorA;
    vtkSmartPointer<vtkTextProperty> textProperty;
/************************************************结果查看***************************************************/
    //QWidget *propResultWidget;
    QComboBox *propStateCombox;
    QComboBox *propResultTypeCombox;
    QChart *resultChart;
    MyChartView *resultView;

    QVector<QLineSeries*> resultSeriesArray;
    QValueAxis *resultAxisX;
    QValueAxis *resultAxisY;
    QLineSeries *autoSeries;

    QDialog *propResultDialog;
    QVector<QCheckBox*>propResultCheckBoxArray;
    QVector<QPushButton*>propResultColorButtonArray;




    void removeSeriesLegendItem(QChart *, QLineSeries *);
    void clearChartView(QChart*, QVector<QLineSeries*>&);//清空历史曲线

/*******************计算log*******************/
    QDialog *propLogDialog;
    QVBoxLayout *propLogVLayout;
    QTextEdit *propLogTextEdit;
    QProgressBar *propLogBar;

    void initialAnalyseLogDialog();


private slots:
    void updateLog(const QString);
    void updateBar(const int);
    void drawPropResult(int);//绘制曲线


    void onCalculationFinished();
    void updateProgress(int value);



private:

/*******************流线设置******************/
    QDialog *streamSettingDialog;//流线设置
    QLabel *streamLabelArray[4];
    QLineEdit *streamEditArray[4];



    void initialStreamDialog();

    QDialog *progressDialog;
    QProgressBar *dialogBar;





    void initialProgressDialog();
    void startCalculation();

/*********************************************定义螺旋桨显示*************************************************/




    QVTKOpenGLNativeWidget *propDisplayWidgetB;
    vtkSmartPointer<vtkRenderer> rendererB;
    vtkSmartPointer<vtkGenericOpenGLRenderWindow> renwinB;


    vtkSmartPointer<vtkPoints> pointsB;
    vtkSmartPointer<vtkCellArray> linesB;
    vtkSmartPointer<vtkPolyData> profilePolyDataB;
    vtkSmartPointer<vtkAppendPolyData> appendFilterB;
    vtkSmartPointer<vtkPolyDataMapper> mapperB;
    vtkSmartPointer<vtkActor> actorB;
    vtkSmartPointer<vtkLightKit> lightKitB;
    vtkSmartPointer<vtkOrientationMarkerWidget>orientationMarkerB;



    /*******************************螺旋桨*******************************/


    /*******************************螺旋桨设计*******************************/

private:
    propellerDesign *propDesign;
    //main
    QHBoxLayout *propDesignHLayout;
    //childA
    QVBoxLayout *propDesignVLayoutA;
    QGridLayout *propDesignGMessageLayout;
    QGridLayout *propDesignGLayout;
    //childB
    QVBoxLayout *propDesignVLayoutB;

    //设置
    QGroupBox *propDesignSettingBox;
    QGroupBox *propDesignResultBox;
    QLabel *propDesignLabelArray[12];
    QLineEdit *propDesignEditArray[11];

    QLabel *yteTypeLabel; QLabel *yteValueLabel;
    QComboBox *yteTypeCombobox; QLineEdit *yteValueEdit;
    //airfoil
    QComboBox *propDesignAirfoilCombox;
    //result
    QTextEdit *propDesignMessageTextEdit;
    //opengl
    QVTKOpenGLNativeWidget *propDisplayWidget;
    vtkSmartPointer<vtkRenderer> renderer;
    vtkSmartPointer<vtkGenericOpenGLRenderWindow> renwin;

    vtkSmartPointer<vtkPoints> points;
    vtkSmartPointer<vtkCellArray> lines;
    vtkSmartPointer<vtkPolyData> profilePolyData;
    vtkSmartPointer<vtkAppendPolyData> appendFilter;
    vtkSmartPointer<vtkPolyDataMapper> mapper;
    vtkSmartPointer<vtkUnsignedCharArray> color;



    vtkSmartPointer<vtkActor> actor;
    vtkSmartPointer<vtkLightKit> lightKit;
    vtkSmartPointer<vtkOrientationMarkerWidget> orientationMarker;

    //旋转阵列
    void CreateRotatedArray(vtkSmartPointer<vtkRenderer>,vtkSmartPointer<vtkPolyData>, double);

    //view setting
    hoverButton *twistControlButton;
    hoverButton *chordControlButton;
    hoverButton *savePropDataButton;
    hoverButton *changeViewButtonA;
    hoverButton *changeViewButtonB;
    hoverButton *changeViewButtonC;
    //dialog chart
    void initialDialogA();
    void initialDialogB();
    void initialTwistTable();
    void initialChordTable();
    void changeTwistTableValue(const int,const QVector<double>&);
    void changeChordTableValue(const int,const QVector<double>&);



    QTableWidget *twistTable;
    QTableWidget *chordTable;
    QDialog *chartADialog;
    QDialog *chartBDialog;
    QChart *chartA;
    QChart *chartB;
    QChart *chartC;
    MyChartView *chartViewA;
    MyChartView *chartViewB;
    MyChartView *chartViewC;
    QLineSeries *chordSeries;
    QLineSeries *optChordSeries;

    QLineSeries *xAtaqueSeries;
    QLineSeries *xSalidaSeries;
    QScatterSeries *xAtaqueScatterSeries;
    QScatterSeries *xSalidaScatterSeries;
    QScatterSeries *xAtaqueScatterSeriesB;
    QScatterSeries *xSalidaScatterSeriesB;









    QLineSeries *twistSeries;
    QScatterSeries *optTwistScatterSeries;
    QScatterSeries *optTwistScatterSeriesB;
    QLineSeries *optTwistSeries;
    QValueAxis *chordAxisX;
    QValueAxis *chordAxisY;
    QValueAxis *twistAxisX;
    QValueAxis *twistAxisY;
    QValueAxis *shapeAxisX;
    QValueAxis *shapeAxisY;

    QSpinBox *chordSpinBox;
    QSpinBox *twistSpinBox;

    bool isPropMousePressed = false;
    int controlTwistIndex = 0;//扭转角控制点的索引
    int controlChordIndex = 0;//弦长控制点的索引

    QVector<QString>nameArray;
    QVector<QVector<QVector<double>>>airfoilArray;
    QVector<int>cstNumArray;




    void initialPropDesignWidget();
    void initialView();
    void drawPropTwistOnce();
    void drawPropChordOnce();





signals:
    void emitPropellerProgressBarValue(const int);
public slots:

    void updateAirfoilArray(const QVector<QVector<QVector<double>>>&,const QVector<QString>&,const QVector<int>&);
    void updateMessage(const QString);
    void changeProgressValue(const int);
    //更新opengl视图
    void updateView();
    void drawPropTwist();
    void drawPropChord();
    void drawOldPropTwist();
    void drawOldPropChord();

    void updateMousePositionTwist(const QPointF&);
    void updateMousePositionChord(const QPointF&);

    void getMousePressBeginPosition(const QPointF&);
    void getMouseReleaseEndPosition(const QPointF&);

    void showPropTwistDialog();
    void showPropChordDialog();

    void changePropXMesh();

    void onTwistItemChanged(QTableWidgetItem *);
    void onChordItemChanged(QTableWidgetItem *);

    void exportToTwistTXT();
    void exportToChordTXT();



private:
    //fun
    bool returnPropNumTwist(const QPointF&);
    void changePropTwist(const double s);

    bool returnPropNumChord(const QPointF&);
    void changePropChord(const double s);

    void resetPropChord();
    void resetPropTwist();

    void switchTopView();
    void switchMainView();
    void switchLeftView();



    /********************************************************************/
public:


    propellerLibary *propLibary;

    QVector<QVector<propRPMData>>propDataArray;
    void initialPropLibaries();
private:
    double axisRange[15];
    myMath functionMath;


    //螺旋桨结果显示
    resultChartType chartTypeModel;

    int axisIndex[10][2];



    QHBoxLayout *resultHLayout;
    QVBoxLayout *resultVlayout;
    QVBoxLayout *propListVLayout;
    QChart *resultChartA;
    QChart *resultChartB;
    QChart *resultChartC;
    QChart *resultChartD;
    MyChartView *resultChartViewA;
    MyChartView *resultChartViewB;
    MyChartView *resultChartViewC;
    MyChartView *resultChartViewD;
    QValueAxis *resultAxisXA,*resultAxisYA;
    QValueAxis *resultAxisXB,*resultAxisYB;
    QValueAxis *resultAxisXC,*resultAxisYC;
    QValueAxis *resultAxisXD,*resultAxisYD;
    QVector<QLineSeries*>seriesA;
    QVector<QLineSeries*>seriesB;
    QVector<QLineSeries*>seriesC;
    QVector<QLineSeries*>seriesD;


    //添加曲线
    QLineSeries *autoSeriesA;
    QLineSeries *autoSeriesB;
    QLineSeries *autoSeriesC;
    QLineSeries *autoSeriesD;

    void addDataPointA();
    void addDataPointB();
    void addDataPointC();
    void addDataPointD();
    void clearDataPointA();
    void clearDataPointB();
    void clearDataPointC();
    void clearDataPointD();
    void addDataPoint();
    void clearDataPoint();
    void updateAxes(QChart *, const QVector<QLineSeries*> &);


    unsigned int xAxisIndexA;unsigned int xAxisIndexB;unsigned int xAxisIndexC;unsigned int xAxisIndexD;
    unsigned int yAxisIndexA;unsigned int yAxisIndexB;unsigned int yAxisIndexC;unsigned int yAxisIndexD;

    int chartTypeIndexArray[4];

    //右键菜单
    QMenu *rChartMenu;
    QVector<QAction*>actionArray;
    //鼠标位置


    Callout *toolTipA;
    Callout *toolTipB;
    Callout *toolTipC;
    Callout *toolTipD;









    QGroupBox *propListBox;
    QGridLayout *propListGLayout;
    QVector<QPushButton*>buttonArray;
    QVector<hoverButton*>colorButtonArray;
    QVector<hoverButton*>removeButtonArray;
    QVector<QLabel*>labelArrayA;
    QVector<QLabel*>labelArrayB;
    QVector<QLabel*>labelArrayC;
    QVector<QLabel*>labelArrayD;
    QVector<QLabel*>labelArrayE;
    QVector<QLabel*>labelArrayF;
    QVector<QLineEdit*>lineEditArray;
    QVector<QSpinBox*>spinBoxArray;
    QVector<QFrame*>lineArray;
    QVector<QHBoxLayout*>hLayoutArray;
    QSpacerItem *itemSpacer;
    int propIndex = 0;
    int choiceIndex;


    QVector<unsigned int>rpmArray;

    unsigned int chartIndex = 0;
    bool isMousePressed = false;
    bool bothChart = true;
    double lastYA = 0;
    double lastYB = 0;
    double lastYC = 0;
    double lastYD = 0;



    void initialAxis();
    void initialPropResultWidget();
    void initialResultChart();
    void initialRChartMenu();
    void exportChartData(QChart *chart);


    //func
    QVector<QVector<double>> getInterpResult(const int,const int,const int,const int);
    QVector<QVector<double>> getResult(const int,const int,const int,const int);
    void readPropData();
    void addPropData();

    void updateChartType();
    void setPorpButtonEnabled(const int);
    void changePropColor(const QColor&);
    void removeProp(const int);
    void drawResultChartA();
    void drawResultChartB();
    void drawResultChartC();
    void drawResultChartD();

    double getFixVelocityTmp(const double,const QVector<double>&,const QVector<double>&);







    //
private slots:
    void updateMousePositionRA(const QPointF &);
    void updateMousePositionRB(const QPointF &);
    void updateMousePositionRC(const QPointF &);
    void updateMousePositionRD(const QPointF &);

    void showChartCoordA(const QPointF&,bool);
    void showChartCoordB(const QPointF&,bool);
    void showChartCoordC(const QPointF&,bool);
    void showChartCoordD(const QPointF&,bool);


    void getMousePressBeginPositionR(const QPointF &);
    void getMouseReleaseEndPositionR(const QPointF &);
    void changeResultType();
    void choicePropButton();
    void showColorDialog();
    void deleteProp();
    void showChartAMenu(const QPoint&);
    void showChartBMenu(const QPoint&);
    void showChartCMenu(const QPoint&);
    void showChartDMenu(const QPoint&);

    void changeRpm(const int);
    void outputPropData();



private:
    //nothing
    glWidget *nothingWidget = nullptr;
};

#endif // PROPELLERDISPLAY_H
