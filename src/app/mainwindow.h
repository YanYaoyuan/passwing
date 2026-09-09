#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMenu>
#include <QMenuBar>
#include <QToolButton>
#include <QKeyEvent>
#include <QLabel>
#include "chat/chatwidget.h"
#include "wing/wingdisplay.h"
#include "aircraft/airplanedisplay.h"
#include "propeller/propellerdisplay.h"
#include "geometry/stlreader.h"
#include "geometry/airplanelibrary.h"
#include "widgets/imagebutton.h"
#include "common/myfile.h"
enum MODEL_TYPE{

    MODEL_AIRFOILDESIGN,
    MODEL_AIRFOILOPTIMIZATION,
    MODEL_AIRFOILINTER,
    MODEL_WINGDESIGN,
    MODEL_WINGOPTIMIZATION,
    MODEL_PROPDESIGN,
    MODEL_PROPANALYSE,
    MODEL_AIRPLANEDESIGN,
    MODEL_AIRPLANESTABILITY,
    MODEL_NOTHING

};
enum DATA_TYPE{
    DATA_AIRFOIL,
    DATA_PROJECT,
};
class mainWindow:public QMainWindow
{
    Q_OBJECT
public:
    mainWindow(QMainWindow *parent = nullptr);
    ~mainWindow();
protected:
    void closeEvent(QCloseEvent *event) override;
private:
    MODEL_TYPE ANALYSE_TYPE = MODEL_NOTHING;//
    DATA_TYPE  READ_TYPE;
    /**********************************************************************************/
    void loadIcon();
    QString homeViewIcon;
    QString airfoilViewIcon;
    QString airplaneViewIcon;
    QString powerViewIcon;
    QString optimizationViewIcon;
    QString saasViewIcon;
    QString saasViewIconA;
    QString startViewIcon;



    QString flyvisionIcon;
    QString airportIcon;
    QString helpIcon;
    QString theoryIcon;

    QString airfoilDesignIcon;
    QString airfoilChoiceIcon;
    QString airfoilInterIcon;

    QString wingDesignIcon;
    QString tailDesignIcon;
    QString airplaneDesignIcon;

    QString propAnalyseIcon;
    QString propDesignIcon;
    QString propSolveIcon;

    QString optimizationAirfoilIcon;
    QString optimizationWingIcon;
    /**************************************主菜单*************************************/

    void initialMenu();
    QMenuBar *menuBar;

    QMenu *fileMenu;
    QAction *importAirfoilActions;
    QAction *openActions;
    QAction *saveActions;

    //QMenuBar *settingMenu;
    QMenu *libaridesMenu;
    QAction *airfoilActions;
    QAction *propActions;

    QMenu *airfoilDesignMenu;
    QAction *airfoilDesignActions;
    QAction *airfoilInverseDesignActions;
    QAction *airfoilOptimizationActions;
    QAction *airfoilBlendingActions;

    QMenu *airfoilAnalysisMenu;
    QAction *simpleAnalysisActions;
    QAction *reynoldsAnalysisActions;

    QMenu *airplaneDesignMenu;
    QAction *wingDefineActions;
    QAction *tailDefineActions;
    QAction *airplaneDefineActions;


    QMenu *propellerDesignMenu;
    QAction *propellerDefineActions;

    QMenu *aboutMeMenu;
    QAction *aboutMeActions;

    QMenu *outputMenu;
    QAction *writeAirfoilDataActions;
    QAction *writeDXFDataActions;
    QAction *writeCATIAScriptActions;
    QAction *conversionDataActions;     //转换数据

    /************************************************************************/



    /*********************************左侧窗口********************************/
    void initialLeftWindow();
    QDockWidget *leftDock;
    QWidget *leftWidget;
    QVBoxLayout *productVLayout;

    //功能列表
    QToolButton *homeViewButton;
    QToolButton *airfoilViewButton;
    QToolButton *airplaneViewButton;
    QToolButton *powerViewButton;
    QToolButton *optimizationViewButton;
    QToolButton *saasViewButton;


    QToolButton *activeButton = nullptr;

    //开始运算
    QToolButton *startAnalyseButton;

    /******************************上方窗口***********************************/
    void initialTopWindow();
    QDockWidget *topDock;
    QWidget *topWidget;


    /******************************下方窗口***********************************/
    void initialBottomWindow();

    QDockWidget *bottomDock;
    QWidget *bottomWidget;
    QVBoxLayout *bottomVlayout;
    QProgressBar *timeBar;


    /********************************子界面*********************************/
    //主页
    QWidget *homeWidget;
    QGridLayout *homeGLayout;
    imageButton *visionButton;
    imageButton *airportButton;
    imageButton *helpButton;
    imageButton *theoryButton;
    void initialHomeWidget();
    //翼型设计
    QWidget *airfoilWidget;
    QGridLayout *airfoilGLayout;
    imageButton *airfoilDesignButton;
    imageButton *airfoilAntiDesignButton;
    imageButton *airfoilChoiceButton;
    imageButton *otherAirfoilButton;
    void initialAirfoilWidget();
    //飞机设计
    QWidget *airplaneWidget;
    QGridLayout *airplaneGLayout;
    imageButton *wingDesignButton;
    imageButton *tailDesignButton;
    imageButton *airplaneDesignButton;
    imageButton *airplaneOtherButtonA;
    void initialAirplaneWidget();

    //螺旋桨设计
    QWidget *powerWidget;
    QGridLayout *powerGLayout;
    imageButton *propellerAnalyseButton;
    imageButton *propellerDesignButton;
    imageButton *powerOtherButtonA;
    imageButton *powerOtherButtonB;
    void initialPowerWidget();


    //优化设计
    QWidget *optimizationWidget;
    QGridLayout *optimizationGLayout;
    imageButton *airfoilOptimizationButton;
    imageButton *wingOptimizationButton;
    imageButton *propellerOptimizationButton;
    imageButton *optimizationOtherButtonA;
    void initialOptimizationWidget();






private:
    chatWidget *chatWindow;
    wingDisplay *designWingWindow;
    airplaneDisplay *designAirplaneWindow;
    airfoilDisplay *designAirfoilWindow;
    propellerDisplay *designPropellerWindow;
    STLReader *displayAirCartWindow;
    airplaneLibrary *displayAirportWindow;
    myFile *projectFile;
    QLabel *saasPreviewWindow;





private:
    //home
    void changeWindowForHome();

    void changeWindowForAirport();
    //airfoil
    void changeWindowForAirfoil();

    void changeWindowForAirfoilDesign();

    void changeWindowForAirfoilInter();

    //airplane
    void changeWindowForAirplane();

    void changeWindowForWingDesign();
    void changeWindowForTailDesign();
    void changeWindowForAirplaneDesign();


    //propeller
    void changeWindowForPower();


    void changeWindowForPropResultAnalyse();
    void changeWindowForPropAnalyse();
    void changeWindowForPropDesign();
    //optimization
    void changeWindowForOptimization();

    void changeWindowForAirfoilOptimization();
    void changeWindowForWingOptimization();
    //
    void showAirfoilLibaries();
    //
    void showHelpHtml();
    void showTheoreticalFrameworkHtml();

    //信息传递
    //fun
    void replaceCentralWidget(QWidget*);

    //项目文件接口

    //LoginWidget login;

public slots:


    void readData();
    void saveData();//处理打开的数据;
    void startAnalyse();
    void updateState();
signals:
    void emitAirfoilArray(const QVector<QVector<QVector<double>>>&,const QVector<QString>&);
    void emitWingArray(QVector<wingDefinition>&);
    void emitTailArray(QVector<wingDefinition>&);
    void emitPropellerArray(QVector<wingDefinition>&);
    void emitAirplaneArray(QVector<airplaneDefinition>&);

public slots:

    void updateMyProgressBar(const int value){
        if(!timeBar->isVisible()){
            timeBar->show();
            startAnalyseButton->setEnabled(false);
        }
        timeBar->setValue(value);
        if(value >= 99){
            timeBar->hide();
            startAnalyseButton->setEnabled(true);
        }
    }

    void changeButtonState();
private:
    QDialog *aboutMeDialog;
    QLabel *titleAboutMeLabel;
    QLabel *versionAboutMeLabel;
    QLabel *authorAboutMeLabel;
    QLabel *infoAboutMeLabel;
    QPushButton *closeAboutMeButton;
    QVBoxLayout *vAboutMeLayout;
    void initialAboutMeDialog();
private slots:
    void onSaasViewToggled();



protected:
    // 重写 keyPressEvent 方法
    void keyPressEvent(QKeyEvent *event) override;
};



#endif // MAINWINDOW_H
