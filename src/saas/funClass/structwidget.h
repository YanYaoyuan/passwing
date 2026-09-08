#ifndef STRUCTWIDGET_H
#define STRUCTWIDGET_H

#include <QWidget>
#include <QGroupBox>
#include <QVBoxLayout>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QButtonGroup>
#include <QRadioButton>
#include <QDoubleSpinBox>
#include <QComboBox>
#include <QLabel>
#include <QCheckBox>
#include <QLineEdit>
#include <QPushButton>
#include <QListWidget>
#include "saas/funClass/structcfddefinition.h"

/* ============================================================
 * 数据层：存值的结构体
 * ============================================================ */

enum BoxType {
    BOX_GlobalSettings,
    BOX_Global,
    BOX_Ref,
    BOX_TurbulentDiffusion,
    BOX_Eqn,
    BOX_ForceMoment,
    BOX_RawGrid,
    BOX_GridGen,
    BOX_Airfoil,
    BOX_Flux,
    BOX_CodeRun,
    BOX_NonlinearSolver,
    BOX_LinearSolver,
    BOX_VolumeOutput,
    BOX_BoundaryOutput,
    BOX_CloudPlot,
};



enum class Fun3DDisplayMode {
    CommonOnly,
    All
};

class structWidget : public QWidget
{
    Q_OBJECT


public:
    explicit structWidget(QWidget* parent = nullptr);

    void initUISetup(const structCFDDefinition&);
    void switchToBox(QGroupBox* targetBox);
    void hideOptionalWidget();
    void showOptionalWidget();
    void setCloudPlotEdits(double,double,int);

    //设置云图显示变量
    void setCloudPlotBoxItems(const QStringList&);
    structCFDDefinition settingData;
    QPushButton *startAnalyseButton;


    QGroupBox* globalSettingsBox;

    QGroupBox* rawGridBox;
    QGroupBox* gridGenBox;
    QGroupBox* airfoilBox;
    QGroupBox* forceMomentBox;
    QGroupBox* eqnBox;
    QGroupBox* refBox;
    QGroupBox *fluxBox;
    QGroupBox* turbulentDiffusionBox;
    QGroupBox *spalartBox;
    QGroupBox *codeRunBox;
    QGroupBox *nonlinearSolverBox;
    QGroupBox *linearSolverBox;
    QGroupBox *globalBox;
    QGroupBox *volumeOutputBox;
    QGroupBox* boundaryOutputBox;
    QGroupBox* cloudPlotBox;

    /* ============================================================
     * UI 层：存控件指针
     * ============================================================ */
    struct RawGridUI {
        QComboBox* gridFormatCombo;
        QComboBox* dataFormatCombo;
        QCheckBox* twodModeCheck;
        QCheckBox* swapYZAxesCheck;
        QComboBox* fieldviewPrecisionCombo;
        QComboBox* patchLumpingCombo;
        QCheckBox* ignoreEulerCheck;



    }rawUI;
    struct GridGenUI{


        QRadioButton *oGridRadio;
        QRadioButton *cGridRadio;

        QSpinBox *nsrfSpin;//翼型表面网格点数
        QSpinBox *wakeSpin;//尾迹区域节点，如果是C型网格展开
        QSpinBox *jMaxSpin;//垂直翼型表面网格



        QDoubleSpinBox *lespSpin;//翼型前缘间距
        QDoubleSpinBox *tespSpin;//翼型后缘间距
        QDoubleSpinBox *radiSpin;//远场半径

        QDoubleSpinBox *reSpin;//
        QDoubleSpinBox *yPlusSpin;//Y+

        QComboBox *genTypeBox;

        QPushButton *genMeshButton;


    }gridUI;

    struct AirfoilInfoUI {

        QLabel* pointCountLabel;
        QLabel* trailingEdgeLabel;

        QPushButton* selectBtn;
    } airfoilInfoUI;

    struct FlowModelUI {
        QButtonGroup* turbGroup;
        QList<QRadioButton*> turbButtons;
        QDoubleSpinBox* turbIntensitySpin;
        QDoubleSpinBox* turbMuRatioSpin;
        QComboBox* turbCompressCombo;
        QComboBox* turbCondCombo;
        QDoubleSpinBox* prSpin;
        QDoubleSpinBox* scSpin;
    } flowUI;

    struct ReferenceUI {
        QButtonGroup* dimGroup;
        QRadioButton* nondimBtn;
        QRadioButton* dimSIBtn;
        QDoubleSpinBox* gridLengthConversionSpin;
        QDoubleSpinBox* machSpin;
        QDoubleSpinBox* reynoldsSpin;
        QDoubleSpinBox* velocitySpin;
        QDoubleSpinBox* densitySpin;
        QDoubleSpinBox* tempSpin;
        QComboBox* tempUnitCombo;
        QDoubleSpinBox* alphaSpin;
        QDoubleSpinBox* yawSpin;
    } refUI;



    struct EqnUI {
        QComboBox* eqnTypeCombo;
        QDoubleSpinBox* artificialCompressSpin;
        QComboBox* viscousTermsCombo;
        QComboBox* chemKineticsCombo;
        QComboBox* thermalModelCombo;
        QDoubleSpinBox* prandtlSpin;
        QDoubleSpinBox* schmidtSpin;
        QComboBox* gasRadiationCombo;
        QCheckBox* radUseImplCheck;
        QCheckBox* multiDiffCheck;
        //QDoubleSpinBox* cpivMinSpin;
        //QCheckBox* augmentKineticsCheck;
        //QCheckBox* implicitRateCheck;
    }eqnUI;


    struct ForceMomentUI {
        QDoubleSpinBox* areaReferenceSpin;
        QDoubleSpinBox* xMomentLengthSpin;
        QDoubleSpinBox* yMomentLengthSpin;
        QDoubleSpinBox* xMomentCenterSpin;
        QDoubleSpinBox* yMomentCenterSpin;
        QDoubleSpinBox* zMomentCenterSpin;
    }fmUI;



    struct InviscidFluxUI {
        QComboBox *fluxConsCombo;
        QComboBox *fluxConsLhsCombo;
        QDoubleSpinBox *kappaSpin;
        QComboBox *fluxLimiterCombo;
        QSpinBox *firstOrderSpin;
        QComboBox *multiOptionCombo;
        QCheckBox *fixedDirCheck;
        QSpinBox *recalcSpin;
        QCheckBox *entropyFixCheck;
        QCheckBox *adaptEntropyCheck;
        QDoubleSpinBox *rhsUEigenSpin;
        QDoubleSpinBox *lhsUEigenSpin;
        QDoubleSpinBox *rhsAEigenSpin;
        QDoubleSpinBox *lhsAEigenSpin;
        QDoubleSpinBox *reMinSpin;
        QDoubleSpinBox *reMaxSpin;
    } fluxUI;

    struct SpalartUI {
        QDoubleSpinBox *turbInfSpin;
        QCheckBox *daclesCheck;
        QCheckBox *sarcCheck;
        QCheckBox *ddesCheck;
    } spalartUI;

    struct CodeRunControlUI {
        QSpinBox* stepsSpin;                // steps
        QLineEdit *stopTolEdit;
        QDoubleSpinBox* stopTolSpin;        // stopping_tolerance
        QDoubleSpinBox* durationLimitSpin;  // duration_limit_in_minutes
        QCheckBox* noRestartCheck;          // no_restart
        QSpinBox* restartWriteFreqSpin;     // restart_write_freq
        QComboBox* restartReadCombo;        // restart_read
        QCheckBox* smartJupdateCheck;       // smart_jupdate
        QSpinBox* jacobianEvalFreqSpin;     // jacobian_eval_freq
        QSpinBox* jupdateStartupStepsSpin;  // jupdate_startup_steps
        QCheckBox* dfduc3Check;             // dfduc3_jacobians
        QCheckBox* alphaSweepCheck;         // alpha_sweep
        QSpinBox* cycleIncrementSpin;       // cycle_increment
        QDoubleSpinBox* alphaIncrementSpin; // alpha_increment
        QDoubleSpinBox* alphaMaxSpin;       // alpha_max
        QDoubleSpinBox* alphaMinSpin;       // alpha_min
        QSpinBox* alphaSwitchbacksSpin;     // alpha_switchbacks
    }codeRunUI;

    struct NonlinearSolverUI {
        QComboBox* timeAccuracyCombo;       // time_accuracy
        QComboBox* pseudoTimeSteppingCombo;
        QDoubleSpinBox* timeStepNondimSpin; // time_step_nondim
        QSpinBox* subiterationsSpin;        // subiterations
        QCheckBox* temporalErrControlCheck; // temporal_err_control
        QDoubleSpinBox* temporalErrFloorSpin; // temporal_err_floor
        QSpinBox* scheduleNumberSpin;
        QSpinBox* scheduleIteration1Spin;   // schedule_iteration(1)
        QSpinBox* scheduleIteration2Spin;   // schedule_iteration(2)
        QDoubleSpinBox* scheduleCfl1Spin;   // schedule_cfl(1)
        QDoubleSpinBox* scheduleCfl2Spin;   // schedule_cfl(2)
        QDoubleSpinBox* scheduleCflturb1Spin; // schedule_cflturb(1)
        QDoubleSpinBox* scheduleCflturb2Spin; // schedule_cflturb(2)

        QDoubleSpinBox* fAllowMinimumMSpin; // f_allow_minimum_m
        QDoubleSpinBox* inviscRelaxFactorSpin; // invisc_relax_factor
        QDoubleSpinBox* viscRelaxFactorSpin;   // visc_relax_factor
    }nonlinearUI;

    struct LinearSolverUI {
        QSpinBox* meanflowSweepsSpin;
        QSpinBox* turbSweepsSpin;
        QCheckBox* projectionCheck;
        QComboBox* lineImplicitCombo;
    } linearSolverUI;

    // UI container for output settings



    struct GlobalUI
    {
        // === 移动网格相关 ===
        QCheckBox *movingGridCheck;
        QCheckBox *gridMotionOnlyCheck;
        QCheckBox *gridMotionAndDCICheck;
        QCheckBox *bodyMotionOnlyCheck;

        // === 计时选项 ===
        QCheckBox *timingCheck;
        QCheckBox *timeMovingGridCheck;

        // === 输出频率 ===
        QSpinBox *boundaryAnimSpin;
        QSpinBox *volumeAnimSpin;
        QSpinBox *sliceFreqSpin;

        // === 其他 ===
        QCheckBox *recordCmdCheck;
    }globalUI;

    struct VolumeOutputUI {
        QComboBox* exportToCombo;

        QCheckBox* xCheck;
        QCheckBox* yCheck;
        QCheckBox* zCheck;

        QCheckBox* primitiveCheck;
        QCheckBox* rhoCheck;
        QCheckBox* uCheck;
        QCheckBox* vCheck;
        QCheckBox* wCheck;
        QCheckBox* pCheck;
        QCheckBox* entropyCheck;
        QCheckBox* machCheck;
        QCheckBox* temperatureCheck;
        QCheckBox* iblankCheck;
        QCheckBox* imeshCheck;

        QCheckBox* vortMagCheck;
        QCheckBox* vortXCheck;
        QCheckBox* vortYCheck;
        QCheckBox* vortZCheck;
        QCheckBox* qCriterionCheck;
        QCheckBox* divVelCheck;

        QCheckBox* turbFluctCheck;
        QCheckBox* uuPrimeCheck;
        QCheckBox* vvPrimeCheck;
        QCheckBox* wwPrimeCheck;
    }volumeOutputUI;

    struct BoundaryOutputUI {


        QSpinBox* numberOfBoundaries;
        QLineEdit* boundaryListEdit;
        QComboBox* exportToCombo;

        QCheckBox* xCheck;
        QCheckBox* yCheck;
        QCheckBox* zCheck;

        QCheckBox* primitiveCheck;
        QCheckBox* rhoCheck;
        QCheckBox* uCheck;
        QCheckBox* vCheck;
        QCheckBox* wCheck;
        QCheckBox* pCheck;
        QCheckBox* entropyCheck;
        QCheckBox* machCheck;
        QCheckBox* temperatureCheck;
        QCheckBox* iblankCheck;
        QCheckBox* imeshCheck;

        QCheckBox* vortMagCheck;
        QCheckBox* vortXCheck;
        QCheckBox* vortYCheck;
        QCheckBox* vortZCheck;
        QCheckBox* qCriterionCheck;
        QCheckBox* divVelCheck;

        QCheckBox* turbFluctCheck;
        QCheckBox* uuPrimeCheck;
        QCheckBox* vvPrimeCheck;
        QCheckBox* wwPrimeCheck;
        QCheckBox* uvPrimeCheck;
        QCheckBox* uwPrimeCheck;
        QCheckBox* vwPrimeCheck;

        QCheckBox* cpCheck;
    }boundaryOutputUI;


    struct CloudPlotUI {


        // 选择变量
        QComboBox* variableCombo;
        //取值范围
        QLineEdit* minEdit;
        QLineEdit* maxEdit;
        QLineEdit* numsEdit;
        //
        QComboBox* colorCombo;
        // 边界列表
        QListWidget* boundaryList;
        //
        QPushButton* applyButton;
        QPushButton* cancelButton;
        // 选择

    }cloudPlotUI;














private:


    QVBoxLayout* mainLayout;
    QVector<QWidget*>optionalWidgetArray;
    BoxType currentBox = BOX_GlobalSettings;
    QList<QPair<QGroupBox*, BoxType>> boxes;
    void initGlobalSettingsBox();
    void initTurbulentDiffusionBox();
    void initReferenceBox();
    void initEquationBox();
    void initForceMomentBox();
    void initRawGridBox();
    void initGridGenBox();
    void initAirfoilInfoBox();

    void initInviscidFluxBox();
    void initSpalartBox();
    void initCodeRunControlBox();
    void initNonlinearSolverBox();
    void initLinearSolverBox(); 
    void initGlobalBox();
    void initVolumeOutputBox();
    void initBoundaryOutputBox();
    void initCloudPlotBox();


    void updateTurbulentDiffusionBox(const structCFDDefinition&);
    void updateReferenceBox(const structCFDDefinition&);
    void updateEquationBox(const structCFDDefinition&);
    void updateForceMomentBox(const structCFDDefinition&);
    void updateRawGridBox(const structCFDDefinition&);
    void updateInviscidFluxBox(const structCFDDefinition&);
    void updateSpalartBox(const structCFDDefinition&);
    void updateCodeRunControlBox(const structCFDDefinition&);
    void updateNonlinearSolverBox(const structCFDDefinition&);
    void updateLinearSolverBox(const structCFDDefinition&);
    void updateGlobalBox(const structCFDDefinition&);
    void updateVolumeOutputBox(const structCFDDefinition&);
    void updateBoundaryOutputBox(const structCFDDefinition&);
    void updateCloudPlotBox(const structCFDDefinition&);



    void applyCurrentBoxData();
    void setWidgetObjectName();

    void updateDisplayMode(bool all); // 切换显示模式

    // ✅ 导出用户输入值
    RawGridData getRawGridData() const;
    TurbulentDiffusionModelsData getTurbulentDiffusionModelsData() const;
    ReferenceData getReferenceData() const;
    EqnData getEqnData() const;
    ForceMomentData getForceMomentData() const;

    InviscidFluxData getInviscidFluxData() const;
    SpalartData getSpalartData() const;
    CodeRunData getCodeRunData() const;
    NonlinearSolverData getNonlinearSolverData() const;
    LinearSolverData getLinearSolverData() const;
    GlobalData getGlobalData()const;
    VolumeOutputData getVolumeOutputData()const;
    BoundaryOutputData getBoundaryOutputData() const;
    cloudPlotData getCloudPlotData() const;










    QRadioButton* commonBtn;
    QRadioButton* allBtn;

private slots:
    void onWidgetChanged();
    //
signals:
    void widgetTriggered(const QString&);
public:
    GridGenData getGridGenData() const;
private:
    static QString normalizeQuoted(const QString &text);
    QString cleanQuotedText(const QString &text);
    template<typename T>
    void connectGroupBoxWidgets(QGroupBox* groupBox, T* receiver)
    {
        const QList<QWidget*> widgets = groupBox->findChildren<QWidget*>();
        const QList<QButtonGroup*> groups = groupBox->findChildren<QButtonGroup*>();

        // === 普通控件 ===
        for (QWidget* w : widgets)
        {
            if (qobject_cast<QLabel*>(w))
                continue;

            if (auto edit = qobject_cast<QLineEdit*>(w)) {
                QObject::connect(edit, &QLineEdit::editingFinished,
                                 receiver, [receiver]() { receiver->onWidgetChanged(); });
            }
            else if (auto combo = qobject_cast<QComboBox*>(w)) {
                QObject::connect(combo, &QComboBox::currentTextChanged,
                                 receiver, [receiver]() { receiver->onWidgetChanged(); });
            }
            else if (auto spin = qobject_cast<QSpinBox*>(w)) {
                QObject::connect(spin, QOverload<int>::of(&QSpinBox::valueChanged),
                                 receiver, [receiver]() { receiver->onWidgetChanged(); });
            }
            else if (auto dspin = qobject_cast<QDoubleSpinBox*>(w)) {
                QObject::connect(dspin, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
                                 receiver, [receiver]() { receiver->onWidgetChanged(); });
            }
            else if (auto chk = qobject_cast<QCheckBox*>(w)) {
                QObject::connect(chk, &QCheckBox::stateChanged,
                                 receiver, [receiver]() { receiver->onWidgetChanged(); });
            }
            else if (auto radio = qobject_cast<QRadioButton*>(w)) {
                QObject::connect(radio, &QRadioButton::toggled,
                                 receiver, [receiver]() { receiver->onWidgetChanged(); });
            }
        }

        // === 按钮组 ===
        for (QButtonGroup* group : groups)
        {
            QObject::connect(group, QOverload<QAbstractButton*>::of(&QButtonGroup::buttonClicked),
                             receiver, [receiver]() { receiver->onWidgetChanged(); });
        }
    }


};

#endif // STRUCTCFDWIDGET_H


