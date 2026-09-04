#ifndef CFDDISPLAY_H
#define CFDDISPLAY_H
#include <QWidget>
#include <QMenu>
#include <QMenuBar>
#include <QPushButton>
#include <QToolButton>
#include <QTreeWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QDialog>
#include <QDockWidget>
#include <QTextEdit>
#include <QLabel>
#include <QAction>
#include <QGroupBox>
#include <QProgressBar>
#include <QPlainTextEdit>
#include "saas/login/loginwidget.h"
#include "saas/publicClass/qcustomplot.h"
#include "saas/publicClass/fullwidgetdelegate.h"
#include "saas/funClass/structcfddefinition.h"
#include "saas/funClass/structwidget.h"
#include "saas/publicClass/fun3drun.h"
#include "saas/mesh/ugridviewer.h"
#include "saas/resultClass/flowviewer.h"

#include <QVTKOpenGLNativeWidget.h>


class cfdDisplay : public QWidget
{
    Q_OBJECT
public:
    cfdDisplay(QWidget *parent = nullptr);
    void applyQtPalette(int style);

    LoginWidget *login;
private:
    QString loadQssFile(const QString &);
    void applyGlobalStyle();

    //widget

    //void initialMenu();
    void initialCustomPlot();
    void initialStackWidget();
    void initialTreeWidget();
    void initialVTKWidget();//初始化VTK界面
    //void initialToolButton();
    void initialTreeAction();

    void initialCFDWidget();//初始化主界面

    void setTheme(int style);






    //Tree


    //
    void addProject(const QString,const QString);
    //
    void updateAddButtonState();

signals:
    // 当用户输入命令并按下回车时发出
    void commandEntered(const QString &command);


private:
    // 外部调用此函数输出文字到界面
    void appendOutput(const QString &text);
    int promptPosition; // 提示符后的位置（限制用户不能删除之前的内容）
    bool eventFilter(QObject *obj, QEvent *event);
    void handleCommand(const QString&);
private slots:
    void onAddProjectClicked();
    void onStopProjectClicked();
    void onTreeWidgetContextMenu(const QPoint &);


    void changeProjectDisplay(QTreeWidgetItem*,int);
    void showVolumeOutputSetting(QTreeWidgetItem*,int);
    void showBoundaryOutputSetting(QTreeWidgetItem*,int);
    void showFlowModelSetup(QTreeWidgetItem*,int);
    void showReferenceSetup(QTreeWidgetItem*,int);
    void showEquationSetup(QTreeWidgetItem*,int);
    void showForceMomentSetup(QTreeWidgetItem*,int);
    void showRawGridSetup(QTreeWidgetItem*,int);
    void showInviscidFluxSetup(QTreeWidgetItem*,int);
    //void showSpalartSetup(QTreeWidgetItem*,int);
    void showCodeRunControlSetup(QTreeWidgetItem*,int);
    void showNonlinearSolverSetup(QTreeWidgetItem*,int);
    void showLinearSolverSetup(QTreeWidgetItem*,int);
    void showCloudPlotSetup(QTreeWidgetItem*,int);
    void showAirfoil(QTreeWidgetItem*,int);
    void redrawFromSolver(int index);
    void handleIterationData(const IterationData &data);

    void selectAirfoil();

    void importPltData();//导入plt后处理数据
    void GenerateMesh();//网格生成
    //progressbar


    void startAnalyse();
    //外部UI调用
private slots:
    void setCloudPlotLevel();
    void updateCFDTree(const QVector<QTreeWidgetItem*> &,
                           const QVector<QString> &);
    void displayAirfoilMesh();

private:
    int getProgressValue(const double);
    void updateProgressBar(const int);

    void activateSolver(int);
    void connectSolverSignals(fun3DRun*);
    void calculationFinished();
    void clearAllPlots();
private:




    //布局
    QVBoxLayout* mainLayout;

    //QHBoxLayout *toolButtonLayout;


    //菜单
    QMenuBar* menuBar;
    QMenu* fileMenu;
    QMenu* settingMenu;
    QMenu* aboutMenu;
    //
    //QWidget *centralWidget;
    //按钮
    //QToolButton* importProjectToolButton;
    //QToolButton* addProjectToolButton;
    //QToolButton* saveProjectToolButton;
    //树目录
    QTreeWidget* cfdTreeWidget;
    QLineEdit* searchEdit;
    QPushButton* addProjectButton;
    //树动作
    QMenu *treeContextMenu;
    QAction *renameAction;
    QAction *deleteAction;
    QAction *copyAction;
    //
    QVector<QTreeWidgetItem*>proItemArray;
    //

    //右侧参数表
    QVBoxLayout* listLayout;



    structWidget* funSetupWidget;
    QWidget* listWidget;
    //QWidget* setupWidget;


    QProgressBar *progressBar;
    QPushButton *stopButton;

    //
    QIcon iconMeshA;
    QIcon iconMeshB;
    //VTK
    // --------------------------
    // 核心对象：作为类成员，生命周期与类实例绑定
    // --------------------------

    ugridViewer* viewer;
    flowViewer* resultViewer;
    QSplitter *viewSplitter;

    QStackedWidget* stackedPlot;
    QHBoxLayout *stackedLayout;
    // 曲线对象
    QCustomPlot *plotViewA;
    QCustomPlot *plotViewB;

    // A 残差曲线
    QCPGraph *densityRMSGraph = nullptr;
    QCPGraph *densityMAXGraph = nullptr;
    QCPGraph *turbRMSGraph = nullptr;
    QCPGraph *turbMAXGraph = nullptr;
    QCPGraph *densityXGraph = nullptr;
    QCPGraph *densityYGraph = nullptr;
    QCPGraph *densityZGraph = nullptr;
    QCPGraph *turbXGraph = nullptr;
    QCPGraph *turbYGraph = nullptr;
    QCPGraph *turbZGraph = nullptr;

    // B 升阻力曲线
    QCPGraph *liftGraph = nullptr;
    QCPGraph *dragGraph = nullptr;

    void setupLiftDragPlot(QCustomPlot *);
    void setupResidualPlot(QCustomPlot *);
       // 名称 -> 曲线

    QString getMeshPath();
    QStringList columnNames;             // 列名列表
    bool iterSectionStarted = false;

    //
    QWidget *plotContainer;
    QPlainTextEdit* interfaceEdit;
private:



    QDockWidget* toolbarDock;
    QWidget* toolbarContainer;

    QPushButton* btnLeft;
    QPushButton* btnTop;
    QPushButton* btnFront;
    QPushButton* btnBest;

    QPushButton* btnColorBar;
    QPushButton* btnAxes;

    QPushButton* btnMesh;
    QPushButton* btnSurface;

    QVector<QVector<QVector<double>>>airfoilArray;
    QVector<QString>AirfoilNameArray;
    QVector<int>cstNumArray;






    void initialFloatingToolbar();
private:
    int projectIndex = 0;
    int projectChoiceIndex = 0;
    QVector<int>airfoilChoiceArray;

    bool updatingPlots = true;
    QVector<structCFDDefinition>inputArray;
    QVector<QString>dirArray;//对应的路径
    QVector<fun3DRun*>solverArray;
    QVector<QString>nameArray;

    QVector<QTreeWidgetItem*> airfoilRoots;
    QVector<QTreeWidgetItem*>meshRoots;

public slots:
    void updateAirfoilArray(const QVector<QVector<QVector<double>>>&,const QVector<QString>&,const QVector<int>&);



};

#endif // CFDDISPLAY_H
