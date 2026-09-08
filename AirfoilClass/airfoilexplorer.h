#ifndef AIRFOILEXPLORER_H
#define AIRFOILEXPLORER_H

#include <QWidget>
#include <QTableView>
#include <QTabWidget>
#include <QGroupBox>
#include <QDoubleSpinBox>
#include <QComboBox>
#include <QCheckBox>
#include <QPushButton>
#include <QSlider>
#include <QTextEdit>
#include <QtCharts/QChartView>
#include <QListWidget>
#include <QStringListModel>
#include <QPlainTextEdit>
#include <QProgressBar>
#include "PublicClass/qcustomplot.h"
#include "PublicClass/structDefinition.h"
#include "AirfoilClass/airfoilsolve.h"
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
QT_CHARTS_USE_NAMESPACE
#endif

struct AirfoilLib
{
    int id;                                      // 唯一ID
    QString name;                                // 可选库名
    const QVector<QVector<double>>* data;       // 指向翼型数组
    int count;                                   // 实际有效条目数
    double cl[6];
    double cm[6];
    double cd[6];
    bool badSolutions[6];

    QVector<double>alphaArray;
    QVector<double>clArray;
    QVector<double>cdArray;
    QVector<double>cmArray;
    QVector<double>kArray;
    QVector<double>pArray;
    double weight[6];
    int numDesignPoints;

    double maxThickness;
    double maxCamber;
    AirfoilLib() = default;
    AirfoilLib(int _id, const QString& _name,
                   const QVector<QVector<double>>* _data,
                   int _count,double _maxThickness)
        : id(_id), name(_name), data(_data), count(_count),maxThickness(_maxThickness) {}
};

struct StallDataPoint {
    double alpha;   // 攻角
    double Cl;      // 升力系数
};

struct StallCurve {
    QVector<StallDataPoint> points;
};



class airfoilExplorer : public QWidget
{
    Q_OBJECT

public:
    explicit airfoilExplorer(QWidget *parent = nullptr);

    void importAirfoilName(const QStringList&,const QStringList&,const QStringList&);
    void importAirfoilParameters(const QVector<AirfoilParameters>&,const QVector<AirfoilParameters>&,const QVector<AirfoilParameters>&);

    void addLibrary(int id, const QString& name,
                        const QVector<QVector<double>>* data, int count);

    void importAirfoilData(const QVector<QVector<double>> (&)[1000],
    const QVector<QVector<double>> (&)[1600],
    const QVector<QVector<double>> (&)[2000]);
    void initialAirfoilLib();

private:
    QStringList profiliName;
    QStringList UIUCName;
    QStringList windAIBenchName;

    const QVector<QVector<double>>* profiliAirfoil;
    const QVector<QVector<double>>* UIUCAirfoil;
    const QVector<QVector<double>>* windAIBenchAirfoil;
    QVector<AirfoilLib>airfoilLib;
    AirfoilLib designLib;

    void initialListView();

    void plotAirfoil(const AirfoilLib& lib,
                            const QColor& color);
    void plotResultCurve(QCPCurve*,const QVector<double>&xArray,
                         const QVector<double>&yArray);
    void updateResultPlot(const int);

    void drawThicknessAndFx();
    void drawCmAndFx();
    void drawClMaxAndFx();
    void drawClSlopeAndFx();
    QCPGraph*  drawPoint(QCustomPlot*,const QColor&,const QVector<double>&,const QVector<double>&);
    void syncSelection(int dataIndex);
    AirfoilLib getDesignPoints();
    void selectAirfoilRow(int row);
    void highlightCurve(int selectedIndex);
    void highlightGraphPoint(int dataIndex);
    void clearGraph(QCustomPlot* plot, QCPGraph*& graphPtr);



private:
    /* ===== UI 构建 ===== */
    void setupUI();

    QWidget* createCenterPanel();
    QWidget* createRightPanel();

    QGroupBox* createConditionGroup();
    QGroupBox* createAirfoilLibraryGroup();

    QGroupBox* createSoftConstraintGroup();
    QGroupBox* createDesignClGroup();
    QWidget* createFourPlotWidget();
    QWidget* createClItem(double clValue, double cmValue, double weight);
    void addDesignCl(double cl, double cm, double weight);

    void drawAirfoil(const QItemSelection&,const QItemSelection&);

    void setEqualAspectPlot(QCustomPlot* plot, double xMin, double xMax);
    void resizeEvent(QResizeEvent *event);

    //const AirfoilLibrary* findLibraryByID(int id) const;
    /* ===== Left panel ===== */
    QDoubleSpinBox *reSpin;
    QDoubleSpinBox *maSpin;
    QSpinBox* iterSpin;
    QDoubleSpinBox* stepSpin;
    QSpinBox* threadSpin;
    QCheckBox *xfoilSolver;
    QCheckBox *windAISolver;

    QCheckBox *profiliCheck;
    QCheckBox *uiucCheck;
    QCheckBox *windAICheck;






    QDoubleSpinBox *tcMinSpin;

    QDoubleSpinBox *tcMaxSpin;

    QDoubleSpinBox *camberMinSpin;
    QDoubleSpinBox *camberMaxSpin;

    QDoubleSpinBox *clMaxSpin;
    QCheckBox *enableStallSlopeCheck;
    QCheckBox *enableStallPlateauWidthCheck;
    QSlider *stallSlopeSlider;
    QSlider *stallPlateauWidthSlider;
    QLabel *stallSlopeLabel;
    QLabel *plateauWidthLabel;
    // 失速参数状态（长期存在）
    double postStallSlope = -0.10;     // 必须 < 0
    double stallPlateauWidth = 1.0;    // Δα, 只向后





    QPushButton *runButton;

    /* ===== Center panel ===== */
    QListView *airfoilList;
    QStringListModel *listModel;

    /* ===== Right panel ===== */
    QTabWidget *rightTabs;



    QCustomPlot *geometryPlot;
    QCustomPlot *stallPlot;

    QCustomPlot *resultPlot;
    QCPCurve *airfoilCurve;
    QVector<QCPCurve*>resultCurveArray;

    QWidget* plotWidget;
    QCustomPlot *plotTL;
    QCustomPlot *plotTR;
    QCustomPlot *plotBL;
    QCustomPlot *plotBR;
    QCPGraph *graphTL = nullptr;
    QCPGraph *graphTR = nullptr;
    QCPGraph *graphBL = nullptr;
    QCPGraph *graphBR = nullptr;


    QPlainTextEdit* logWidget;
    QProgressBar* gressBar;






    /* ===== Multi-Cl Design UI ===== */
    QGroupBox   *designClGroup;
    QListWidget *designClList;
    QPushButton *addClButton;
private:
    QVector<int>selectionArray;//选中的翼型ID
    int threadNum = 10;
    xfoilSetting setting[6];
    StallCurve currentCurve;
    int sumValue;
    int gressBarStep = 0;
    int currentSelectedAirfoil;
    bool m_syncingSelection = false;






private:
    void initialXfoilSetting();
    void checkThicknessConstraint();//厚度筛选
    void checkMomentConstraint();//力矩筛选
    void checkClMaxConstraint();//最大升力系数筛选
    void sortByWeightedScore();
    void startXfoilInThread(const int ,const int ,const int );
    void startXfoilInThread(const int ,const int);
    bool checkClArrayAgainstPostStallSlope(const QVector<double>&,const QVector<double>&);


    double computeWeightedInverseCdScore(const AirfoilLib);
    void sortScoresWithIds(QVector<double>& scoreArray, QVector<int>& selectionArray);
    double computeLiftSlope(
            const QVector<double>& alpha,
            const QVector<double>& cl);
private slots:


    void startSelection();

    void drawPostStallSlope(QCustomPlot *plot, const StallCurve &curve, double slope);
    void drawStallPlateau(QCustomPlot *plot, const StallCurve &curve, double plateauWidth);
    void drawStallCurve(QCustomPlot *plot, const StallCurve &curve);
    void updateStallMetric();
    void appendColoredText(const QString& text,const QColor& color,const int);
    void updateListView(const QVector<int>&);
    void showResultPlotContextMenu(const QPoint &pos);
    void onAnyPlotSelectionChanged();
signals:
    void requestAppendText(const QString& text, const QColor& color,const int);
    void requestSelectionArray(const QVector<int>&);
};

#endif // AIRFOILEXPLORER_H


