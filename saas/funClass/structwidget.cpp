#include "saas/funClass/structwidget.h"
#include <qDebug>
#if defined(_MSC_VER) && (_MSC_VER >= 1600)
# pragma execution_character_set("utf-8")
#endif
structWidget::structWidget(QWidget* parent)
    : QWidget(parent)
{
    mainLayout = new QVBoxLayout(this);





    initGlobalBox();
    initGlobalSettingsBox();
    initReferenceBox();
    initEquationBox();
    initTurbulentDiffusionBox();
    initSpalartBox();
    initForceMomentBox();
    initRawGridBox();
    initGridGenBox();
    initAirfoilInfoBox();
    initInviscidFluxBox();
    initCodeRunControlBox();
    initNonlinearSolverBox();
    initLinearSolverBox();
    initVolumeOutputBox();
    initBoundaryOutputBox();
    initCloudPlotBox();
    setWidgetObjectName();


    connectGroupBoxWidgets(rawGridBox,this);
    connectGroupBoxWidgets(forceMomentBox,this);
    connectGroupBoxWidgets(eqnBox,this);
    connectGroupBoxWidgets(refBox,this);
    connectGroupBoxWidgets(gridGenBox,this);
    connectGroupBoxWidgets(fluxBox,this);
    connectGroupBoxWidgets(turbulentDiffusionBox,this);
    connectGroupBoxWidgets(spalartBox,this);
    connectGroupBoxWidgets(codeRunBox,this);
    connectGroupBoxWidgets(nonlinearSolverBox,this);
    connectGroupBoxWidgets(linearSolverBox,this);
    connectGroupBoxWidgets(globalBox,this);
    //connectGroupBoxWidgets(globalSettingsBox,this);
    connectGroupBoxWidgets(volumeOutputBox,this);
    connectGroupBoxWidgets(boundaryOutputBox,this);
    connectGroupBoxWidgets(cloudPlotBox,this);
    boxes = {
        {globalSettingsBox, BOX_GlobalSettings},
        {globalBox,BOX_Global},
        {refBox, BOX_Ref},
        {gridGenBox,BOX_GridGen},
        {airfoilBox,BOX_Airfoil},
        {turbulentDiffusionBox, BOX_TurbulentDiffusion},
        {eqnBox, BOX_Eqn},
        {forceMomentBox, BOX_ForceMoment},
        {rawGridBox, BOX_RawGrid},
        {fluxBox, BOX_Flux},
        {codeRunBox, BOX_CodeRun},
        {nonlinearSolverBox, BOX_NonlinearSolver},
        {linearSolverBox, BOX_LinearSolver},
        {volumeOutputBox, BOX_VolumeOutput},
        {boundaryOutputBox, BOX_BoundaryOutput},
        {cloudPlotBox, BOX_CloudPlot}
    };



    mainLayout->addStretch();
    hideOptionalWidget();
    //globalBox->setEnabled(false);

}
void structWidget::initUISetup(const structCFDDefinition &a){


    updateTurbulentDiffusionBox(a);
    updateReferenceBox(a);
    updateEquationBox(a);
    updateForceMomentBox(a);
    updateRawGridBox(a);
    updateInviscidFluxBox(a);
    updateSpalartBox(a);
    updateCodeRunControlBox(a);
    updateNonlinearSolverBox(a);
    updateLinearSolverBox(a);
    updateGlobalBox(a);
    updateVolumeOutputBox(a);
    updateBoundaryOutputBox(a);



}

void structWidget::updateGlobalBox(const structCFDDefinition &def)
{

    const auto &data = def.globalData; // 假设 structDefinition 有这个成员


    // 阻断信号，防止触发互斥逻辑
    QSignalBlocker b1(globalUI.timingCheck);
    QSignalBlocker b2(globalUI.timeMovingGridCheck);
    QSignalBlocker b3(globalUI.movingGridCheck);
    QSignalBlocker b4(globalUI.gridMotionOnlyCheck);
    QSignalBlocker b5(globalUI.gridMotionAndDCICheck);
    QSignalBlocker b6(globalUI.bodyMotionOnlyCheck);
    QSignalBlocker b7(globalUI.recordCmdCheck);

    QSignalBlocker b8(globalUI.boundaryAnimSpin);
    QSignalBlocker b9(globalUI.volumeAnimSpin);
    QSignalBlocker b10(globalUI.sliceFreqSpin);





    // ===== 设置状态 =====
    globalUI.movingGridCheck->setChecked(data.movingGrid.toLower() == ".true." || data.movingGrid == "1");
    globalUI.gridMotionOnlyCheck->setChecked(data.gridMotionOnly.toLower() == ".true." || data.gridMotionOnly == "1");
    globalUI.gridMotionAndDCICheck->setChecked(data.gridMotionAndDCI.toLower() == ".true." || data.gridMotionAndDCI == "1");
    globalUI.bodyMotionOnlyCheck->setChecked(data.bodyMotionOnly.toLower() == ".true." || data.bodyMotionOnly == "1");

    globalUI.timingCheck->setChecked(data.timing.toLower() == ".true." || data.timing == "1");
    globalUI.timeMovingGridCheck->setChecked(data.timeMovingGrid.toLower() == ".true." || data.timeMovingGrid == "1");

    globalUI.boundaryAnimSpin->setValue(data.boundaryAnimFreq);
    globalUI.volumeAnimSpin->setValue(data.volumeAnimFreq);
    globalUI.sliceFreqSpin->setValue(data.sliceFreq);

    globalUI.recordCmdCheck->setChecked(data.recordCmd.toLower() == ".true." || data.recordCmd == "1");
}

void structWidget::updateTurbulentDiffusionBox(const structCFDDefinition &def)
{
    const auto &data = def.turbulentDiffusionModelsData; // 假设你在 structDefinition 里有这个成员

    // === 阻断信号，防止触发 UI 联动 ===
    QSignalBlocker b1(flowUI.turbIntensitySpin);
    QSignalBlocker b2(flowUI.turbMuRatioSpin);
    QSignalBlocker b3(flowUI.turbCompressCombo);
    QSignalBlocker b4(flowUI.turbCondCombo);
    QSignalBlocker b5(flowUI.prSpin);
    QSignalBlocker b6(flowUI.scSpin);

    // 暂时阻断所有湍流模型按钮
    for (auto *btn : flowUI.turbButtons)
        btn->blockSignals(true);

    // === 湍流模型选择 ===
    QString modelNorm = data.turbulenceModel.toLower();
    modelNorm.replace(QString::fromUtf8("–"), "-");
    modelNorm.remove(' ');

    for (QRadioButton *btn : flowUI.turbButtons) {
        QString text = btn->text().toLower();
        QString norm = text;
        norm.replace(QString::fromUtf8("–"), "-");
        norm.remove(' ');
        btn->setChecked(norm.contains(modelNorm));
    }

    // === 参数更新 ===
    flowUI.turbIntensitySpin->setValue(data.turbIntensity);
    flowUI.turbMuRatioSpin->setValue(data.turbMuRatio);
    flowUI.turbCompressCombo->setCurrentText(data.turbCompress);
    flowUI.turbCondCombo->setCurrentText(data.turbCond);
    flowUI.prSpin->setValue(data.pr);
    flowUI.scSpin->setValue(data.sc);

    // === 同步联动状态 ===
    bool compressible = (data.turbCompress.compare("on", Qt::CaseInsensitive) == 0);
    flowUI.prSpin->setEnabled(compressible);
    flowUI.scSpin->setEnabled(compressible);

    bool isSA = modelNorm.contains("spalart") ||
                 modelNorm.contains("sa-des") ||
                 modelNorm.contains("sades");
    spalartBox->setEnabled(isSA);

    // === 恢复信号 ===
    for (auto *btn : flowUI.turbButtons)
        btn->blockSignals(false);
}


void structWidget::updateEquationBox(const structCFDDefinition &def)
{
    const auto &data = def.eqnData; // 假设 structDefinition 里有 EqnData eqnData;

    // === 阻断信号 ===
    QSignalBlocker b1(eqnUI.eqnTypeCombo);
    QSignalBlocker b2(eqnUI.artificialCompressSpin);
    QSignalBlocker b3(eqnUI.viscousTermsCombo);
    QSignalBlocker b4(eqnUI.chemKineticsCombo);
    QSignalBlocker b5(eqnUI.thermalModelCombo);
    QSignalBlocker b6(eqnUI.prandtlSpin);
    QSignalBlocker b7(eqnUI.schmidtSpin);
    QSignalBlocker b8(eqnUI.gasRadiationCombo);
    QSignalBlocker b9(eqnUI.radUseImplCheck);
    QSignalBlocker b10(eqnUI.multiDiffCheck);
    //QSignalBlocker b11(eqnUI.cpivMinSpin);
    //QSignalBlocker b12(eqnUI.augmentKineticsCheck);
    //QSignalBlocker b13(eqnUI.implicitRateCheck);

    // === 同步 UI 值 ===
    eqnUI.eqnTypeCombo->setCurrentText(data.eqnType);
    eqnUI.artificialCompressSpin->setValue(data.artificialCompress);
    eqnUI.viscousTermsCombo->setCurrentText(data.viscousTerms);
    eqnUI.chemKineticsCombo->setCurrentText(data.chemicalKinetics);
    eqnUI.thermalModelCombo->setCurrentText(data.thermalEnergyModel);
    eqnUI.prandtlSpin->setValue(data.prandtlNumberMolecular);
    eqnUI.schmidtSpin->setValue(data.schmidtNumber);
    eqnUI.gasRadiationCombo->setCurrentText(data.gasRadiation);
    //eqnUI.cpivMinSpin->setValue(data.cpivMinFactor);

    eqnUI.radUseImplCheck->setChecked(data.radUseImplLines.compare(".true.", Qt::CaseInsensitive) == 0);
    eqnUI.multiDiffCheck->setChecked(data.multiComponentDiff.compare(".true.", Qt::CaseInsensitive) == 0);
    //eqnUI.augmentKineticsCheck->setChecked(data.augmentKineticsLimiting.compare(".true.", Qt::CaseInsensitive) == 0);
    //eqnUI.implicitRateCheck->setChecked(data.implicitRateLimiting.compare(".true.", Qt::CaseInsensitive) == 0);

    // === 手动刷新联动状态 ===
    bool isIncompressible = (data.eqnType == "incompressible");
    bool isGeneric = (data.eqnType == "cal_perf_compress");

    eqnUI.artificialCompressSpin->setEnabled(isIncompressible);
    eqnUI.chemKineticsCombo->setEnabled(isGeneric);
    eqnUI.thermalModelCombo->setEnabled(isGeneric);
    eqnUI.gasRadiationCombo->setEnabled(isGeneric);
    eqnUI.radUseImplCheck->setEnabled(isGeneric);
    eqnUI.multiDiffCheck->setEnabled(isGeneric);
    //eqnUI.cpivMinSpin->setEnabled(isGeneric);
    //eqnUI.augmentKineticsCheck->setEnabled(isGeneric);
    //eqnUI.implicitRateCheck->setEnabled(isGeneric);
}

void structWidget::updateForceMomentBox(const structCFDDefinition &def)
{
    // 假设 structDefinition 里有成员：ForceMomentData forceMomentData;
    const ForceMomentData &fm = def.forceMomentData;

    // 阻断信号，防止 setValue 触发外部槽函数（若你有信号联动）
    const bool oldState = this->blockSignals(true);

    // === 更新 UI 控件 ===
    fmUI.areaReferenceSpin->setValue(fm.areaReference);
    fmUI.xMomentLengthSpin->setValue(fm.xMomentLength);
    fmUI.yMomentLengthSpin->setValue(fm.yMomentLength);
    fmUI.xMomentCenterSpin->setValue(fm.xMomentCenter);
    fmUI.yMomentCenterSpin->setValue(fm.yMomentCenter);
    fmUI.zMomentCenterSpin->setValue(fm.zMomentCenter);

    // 恢复信号状态
    this->blockSignals(oldState);
}

void structWidget::updateRawGridBox(const structCFDDefinition &def)
{
    const RawGridData &raw = def.rawGridData;

    const bool oldState = this->blockSignals(true);

    // === 更新各控件值 ===
    rawUI.gridFormatCombo->setCurrentText(raw.gridFormat);
    rawUI.dataFormatCombo->setCurrentText(raw.dataFormat);
    rawUI.twodModeCheck->setChecked(raw.twodMode == ".true.");
    rawUI.swapYZAxesCheck->setChecked(raw.swapYZAxes == ".true.");
    rawUI.fieldviewPrecisionCombo->setCurrentText(raw.fieldviewPrecision);
    rawUI.patchLumpingCombo->setCurrentText(raw.patchLumping);
    rawUI.ignoreEulerCheck->setChecked(raw.ignoreEulerNumber == ".true.");

    this->blockSignals(oldState);
}

void structWidget::updateInviscidFluxBox(const structCFDDefinition &def)
{
    const InviscidFluxData &flux = def.inviscidFluxData;

    const bool oldState = this->blockSignals(true);

    // --- 更新基本通量设置 ---
    fluxUI.fluxConsCombo->setCurrentText(flux.fluxConstruction);
    fluxUI.fluxConsLhsCombo->setCurrentText(flux.fluxConstructionLHS);
    fluxUI.kappaSpin->setValue(flux.kappaUmuscl);
    fluxUI.fluxLimiterCombo->setCurrentText(flux.fluxLimiter);
    fluxUI.firstOrderSpin->setValue(flux.firstOrderIterations);

    // --- multidm 相关 ---
    fluxUI.multiOptionCombo->setCurrentText(QString::number(flux.multidmOption));
    fluxUI.fixedDirCheck->setChecked(flux.fixedDirection == ".true.");
    fluxUI.recalcSpin->setValue(flux.recalcDirFreq);

    // --- 熵修正 ---
    fluxUI.entropyFixCheck->setChecked(flux.entropyFix == ".true.");
    fluxUI.adaptEntropyCheck->setChecked(flux.adaptiveEntropyFix == ".true.");

    // --- 特征值系数 ---
    fluxUI.rhsUEigenSpin->setValue(flux.rhsUEigenCoef);
    fluxUI.lhsUEigenSpin->setValue(flux.lhsUEigenCoef);
    fluxUI.rhsAEigenSpin->setValue(flux.rhsAEigenCoef);
    fluxUI.lhsAEigenSpin->setValue(flux.lhsAEigenCoef);

    // --- Re 切换参数 ---
    fluxUI.reMinSpin->setValue(flux.reMinVswch);
    fluxUI.reMaxSpin->setValue(flux.reMaxVswch);

    this->blockSignals(oldState);
}

void structWidget::updateSpalartBox(const structCFDDefinition &def)
{

    const SpalartData &spalar = def.spalartData;
    QSignalBlocker b1(spalartUI.turbInfSpin);
    QSignalBlocker b2(spalartUI.daclesCheck);
    QSignalBlocker b3(spalartUI.sarcCheck);
    QSignalBlocker b4(spalartUI.ddesCheck);

    spalartUI.turbInfSpin->setValue(spalar.turbInf);
    spalartUI.daclesCheck->setChecked(spalar.daclesMariani == ".true.");
    spalartUI.sarcCheck->setChecked(spalar.sarc == ".true.");
    spalartUI.ddesCheck->setChecked(spalar.ddes == ".true.");
}

#include <vector>   // ✅ 记得加头文件

void structWidget::updateCodeRunControlBox(const structCFDDefinition &def)
{
    const CodeRunData &data = def.codeRunData;

    // ✅ 阻止所有信号发射（作用域内自动恢复）
    QList<QWidget*> widgets = codeRunBox->findChildren<QWidget*>();
    std::vector<QSignalBlocker> blockers;
    blockers.reserve(widgets.size());
    for (auto *w : widgets)
        blockers.emplace_back(w);  // ✅ 构造在容器内部，无拷贝行为

    // === 基本控制 ===
    codeRunUI.stepsSpin->setValue(data.steps);
    codeRunUI.stopTolEdit->setText(QString::number(data.stoppingTolerance));
    //codeRunUI.stopTolSpin->setValue(data.stoppingTolerance);
    codeRunUI.durationLimitSpin->setValue(data.durationLimit);

    // === 重启控制 ===

    codeRunUI.noRestartCheck->setChecked(data.noRestart == ".true.");
    codeRunUI.restartWriteFreqSpin->setValue(data.restartWriteFreq);
    //
    QString restartReadValue = cleanQuotedText(data.restartRead);  // 去掉首尾空格
    int idx = codeRunUI.restartReadCombo->findText(restartReadValue,Qt::MatchFixedString);
    codeRunUI.restartReadCombo->setCurrentIndex(idx >= 0 ? idx : 0);


    // === Jacobian 更新策略 ===
    codeRunUI.smartJupdateCheck->setChecked(data.smartJupdate == ".true.");
    codeRunUI.jacobianEvalFreqSpin->setValue(data.jacobianEvalFreq);
    codeRunUI.jupdateStartupStepsSpin->setValue(data.jupdateStartupSteps);
    codeRunUI.dfduc3Check->setChecked(data.dfduc3Jacobians == ".true.");

    // === 攻角扫描 ===
    codeRunUI.alphaSweepCheck->setChecked(data.alphaSweep == ".true.");
    codeRunUI.cycleIncrementSpin->setValue(data.cycleIncrement);
    codeRunUI.alphaIncrementSpin->setValue(data.alphaIncrement);
    codeRunUI.alphaMaxSpin->setValue(data.alphaMax);
    codeRunUI.alphaMinSpin->setValue(data.alphaMin);
    codeRunUI.alphaSwitchbacksSpin->setValue(data.alphaSwitchbacks);

    // === 互斥逻辑更新 ===
    codeRunUI.jacobianEvalFreqSpin->setEnabled(!codeRunUI.smartJupdateCheck->isChecked());

    bool alphaEnabled = codeRunUI.alphaSweepCheck->isChecked();
    codeRunUI.cycleIncrementSpin->setEnabled(alphaEnabled);
    codeRunUI.alphaIncrementSpin->setEnabled(alphaEnabled);
    codeRunUI.alphaMaxSpin->setEnabled(alphaEnabled);
    codeRunUI.alphaMinSpin->setEnabled(alphaEnabled);
    codeRunUI.alphaSwitchbacksSpin->setEnabled(alphaEnabled);
}




void structWidget::updateNonlinearSolverBox(const structCFDDefinition &def)
{
    const NonlinearSolverData &data = def.nonlinearSolverData;

    // ✅ 阻断信号期间更新
    QSignalBlocker block1(nonlinearUI.timeAccuracyCombo);
    QSignalBlocker block2(nonlinearUI.pseudoTimeSteppingCombo);
    QSignalBlocker block3(nonlinearUI.timeStepNondimSpin);
    QSignalBlocker block4(nonlinearUI.subiterationsSpin);
    QSignalBlocker block5(nonlinearUI.temporalErrControlCheck);
    QSignalBlocker block6(nonlinearUI.temporalErrFloorSpin);
    QSignalBlocker block7(nonlinearUI.scheduleNumberSpin);
    QSignalBlocker block8(nonlinearUI.scheduleIteration1Spin);
    QSignalBlocker block9(nonlinearUI.scheduleIteration2Spin);
    QSignalBlocker block10(nonlinearUI.scheduleCfl1Spin);
    QSignalBlocker block11(nonlinearUI.scheduleCfl2Spin);
    QSignalBlocker block12(nonlinearUI.scheduleCflturb1Spin);
    QSignalBlocker block13(nonlinearUI.scheduleCflturb2Spin);
    QSignalBlocker block14(nonlinearUI.fAllowMinimumMSpin);
    QSignalBlocker block15(nonlinearUI.inviscRelaxFactorSpin);
    QSignalBlocker block16(nonlinearUI.viscRelaxFactorSpin);

    // ===== time_accuracy =====
    QString timeAcc = cleanQuotedText(data.timeAccuracy);
    int idx = nonlinearUI.timeAccuracyCombo->findText(timeAcc, Qt::MatchFixedString);
    nonlinearUI.timeAccuracyCombo->setCurrentIndex(idx >= 0 ? idx : 0);

    // ===== pseudo_time_stepping =====
    QString pseudo = cleanQuotedText(data.pseudoTimeStepping);
    int pidx = nonlinearUI.pseudoTimeSteppingCombo->findText(pseudo, Qt::MatchFixedString);
    nonlinearUI.pseudoTimeSteppingCombo->setCurrentIndex(pidx >= 0 ? pidx : 0);

    // ===== 其它参数 =====
    nonlinearUI.timeStepNondimSpin->setValue(data.timeStepNondim);
    nonlinearUI.subiterationsSpin->setValue(data.subiterations);
    nonlinearUI.temporalErrControlCheck->setChecked(data.temporalErrControl == ".true.");
    nonlinearUI.temporalErrFloorSpin->setValue(data.temporalErrFloor);
    nonlinearUI.scheduleNumberSpin->setValue(data.scheduleNumber);
    nonlinearUI.scheduleIteration1Spin->setValue(data.scheduleIteration1);
    nonlinearUI.scheduleIteration2Spin->setValue(data.scheduleIteration2);
    nonlinearUI.scheduleCfl1Spin->setValue(data.scheduleCfl1);
    nonlinearUI.scheduleCfl2Spin->setValue(data.scheduleCfl2);
    nonlinearUI.scheduleCflturb1Spin->setValue(data.scheduleCflturb1);
    nonlinearUI.scheduleCflturb2Spin->setValue(data.scheduleCflturb2);
    nonlinearUI.fAllowMinimumMSpin->setValue(data.fAllowMinimumM);
    nonlinearUI.inviscRelaxFactorSpin->setValue(data.inviscRelaxFactor);
    nonlinearUI.viscRelaxFactorSpin->setValue(data.viscRelaxFactor);

    // ===== 启用逻辑 =====
    bool steady = (timeAcc.compare("steady", Qt::CaseInsensitive) == 0);
    nonlinearUI.timeStepNondimSpin->setEnabled(!steady);
    nonlinearUI.subiterationsSpin->setEnabled(!steady);
    nonlinearUI.temporalErrControlCheck->setEnabled(!steady);
    nonlinearUI.temporalErrFloorSpin->setEnabled(!steady && nonlinearUI.temporalErrControlCheck->isChecked());
}


void structWidget::updateLinearSolverBox(const structCFDDefinition &def)
{
    const LinearSolverData &data = def.linearSolverData;

    // === 主流方程松弛次数 ===
    linearSolverUI.meanflowSweepsSpin->setValue(data.meanflowSweeps);

    // === 湍流方程松弛次数 ===
    linearSolverUI.turbSweepsSpin->setValue(data.turbulenceSweeps);

    // === Krylov 投影 ===
    linearSolverUI.projectionCheck->setChecked(data.linearProjection == ".true.");

    // === 线隐式选项 ===
    QString lineImp = cleanQuotedText(data.lineImplicit);

    int idx = linearSolverUI.lineImplicitCombo->findText(lineImp, Qt::MatchFixedString);
    if (idx >= 0)
        linearSolverUI.lineImplicitCombo->setCurrentIndex(idx);
    else
        linearSolverUI.lineImplicitCombo->setCurrentIndex(0);
}

void structWidget::updateVolumeOutputBox(const structCFDDefinition &def)
{
    const VolumeOutputData &data = def.volumeOutputData;

    // 🟡 一次性屏蔽所有信号
    const bool oldState = blockSignals(true);

    // === 输出格式 export_to ===
    QString exportTo = cleanQuotedText(data.exportTo);
    //exportTo.remove("'");  // 移除引号
    int idx = volumeOutputUI.exportToCombo->findText(exportTo, Qt::MatchFixedString);
    if (idx >= 0)
        volumeOutputUI.exportToCombo->setCurrentIndex(idx);
    else
        volumeOutputUI.exportToCombo->setCurrentIndex(0);

    // === 坐标 ===
    volumeOutputUI.xCheck->setChecked(data.x == ".true.");
    volumeOutputUI.yCheck->setChecked(data.y == ".true.");
    volumeOutputUI.zCheck->setChecked(data.z == ".true.");

    // === 基础变量 ===
    volumeOutputUI.primitiveCheck->setChecked(data.primitive == ".true.");
    volumeOutputUI.rhoCheck->setChecked(data.rho == ".true.");
    volumeOutputUI.uCheck->setChecked(data.u == ".true.");
    volumeOutputUI.vCheck->setChecked(data.v == ".true.");
    volumeOutputUI.wCheck->setChecked(data.w == ".true.");
    volumeOutputUI.pCheck->setChecked(data.p == ".true.");
    volumeOutputUI.entropyCheck->setChecked(data.entropy == ".true.");
    volumeOutputUI.machCheck->setChecked(data.mach == ".true.");
    volumeOutputUI.temperatureCheck->setChecked(data.temperature == ".true.");
    volumeOutputUI.iblankCheck->setChecked(data.iblank == ".true.");
    volumeOutputUI.imeshCheck->setChecked(data.imesh == ".true.");

    // === 涡量/速度散度 ===
    volumeOutputUI.vortMagCheck->setChecked(data.vortMag == ".true.");
    volumeOutputUI.vortXCheck->setChecked(data.vortX == ".true.");
    volumeOutputUI.vortYCheck->setChecked(data.vortY == ".true.");
    volumeOutputUI.vortZCheck->setChecked(data.vortZ == ".true.");
    volumeOutputUI.qCriterionCheck->setChecked(data.qCriterion == ".true.");
    volumeOutputUI.divVelCheck->setChecked(data.divVel == ".true.");

    // === 湍流波动 ===
    volumeOutputUI.turbFluctCheck->setChecked(data.turbFluct == ".true.");
    volumeOutputUI.uuPrimeCheck->setChecked(data.uuPrime == ".true.");
    volumeOutputUI.vvPrimeCheck->setChecked(data.vvPrime == ".true.");
    volumeOutputUI.wwPrimeCheck->setChecked(data.wwPrime == ".true.");

    // 🟢 恢复信号状态
    blockSignals(oldState);
}


void structWidget::updateBoundaryOutputBox(const structCFDDefinition &def)
{
    const BoundaryOutputData &data = def.boundaryOutputData;

    // === 屏蔽信号（作用域自动恢复） ===
    QSignalBlocker b1(boundaryOutputUI.numberOfBoundaries);
    QSignalBlocker b2(boundaryOutputUI.boundaryListEdit);
    QSignalBlocker b3(boundaryOutputUI.exportToCombo);

    // === 基本参数 ===
    boundaryOutputUI.numberOfBoundaries->setValue(data.numberOfBoundaries);

    QString list = data.boundaryList;
    list.remove("'"); // 移除包裹的引号
    boundaryOutputUI.boundaryListEdit->setText(list);

    QString exportTo = cleanQuotedText(data.exportTo);
    //exportTo.remove("'");
    int idx = boundaryOutputUI.exportToCombo->findText(exportTo, Qt::MatchFixedString);
    if (idx < 0)
        idx = 0;
    boundaryOutputUI.exportToCombo->setCurrentIndex(idx);

    // === 坐标 ===
    boundaryOutputUI.xCheck->setChecked(data.x == ".true.");
    boundaryOutputUI.yCheck->setChecked(data.y == ".true.");
    boundaryOutputUI.zCheck->setChecked(data.z == ".true.");

    // === 基础变量 ===
    boundaryOutputUI.primitiveCheck->setChecked(data.primitive == ".true.");
    boundaryOutputUI.rhoCheck->setChecked(data.rho == ".true.");
    boundaryOutputUI.uCheck->setChecked(data.u == ".true.");
    boundaryOutputUI.vCheck->setChecked(data.v == ".true.");
    boundaryOutputUI.wCheck->setChecked(data.w == ".true.");
    boundaryOutputUI.pCheck->setChecked(data.p == ".true.");
    boundaryOutputUI.entropyCheck->setChecked(data.entropy == ".true.");
    boundaryOutputUI.machCheck->setChecked(data.mach == ".true.");
    boundaryOutputUI.temperatureCheck->setChecked(data.temperature == ".true.");
    boundaryOutputUI.iblankCheck->setChecked(data.iblank == ".true.");
    boundaryOutputUI.imeshCheck->setChecked(data.imesh == ".true.");

    // === 涡量/散度 ===
    boundaryOutputUI.vortMagCheck->setChecked(data.vortMag == ".true.");
    boundaryOutputUI.vortXCheck->setChecked(data.vortX == ".true.");
    boundaryOutputUI.vortYCheck->setChecked(data.vortY == ".true.");
    boundaryOutputUI.vortZCheck->setChecked(data.vortZ == ".true.");
    boundaryOutputUI.qCriterionCheck->setChecked(data.qCriterion == ".true.");
    boundaryOutputUI.divVelCheck->setChecked(data.divVel == ".true.");

    // === 湍流相关 ===
    boundaryOutputUI.turbFluctCheck->setChecked(data.turbFluct == ".true.");
    boundaryOutputUI.uuPrimeCheck->setChecked(data.uuPrime == ".true.");
    boundaryOutputUI.vvPrimeCheck->setChecked(data.vvPrime == ".true.");
    boundaryOutputUI.wwPrimeCheck->setChecked(data.wwPrime == ".true.");
    boundaryOutputUI.uvPrimeCheck->setChecked(data.uvPrime == ".true.");
    boundaryOutputUI.uwPrimeCheck->setChecked(data.uwPrime == ".true.");
    boundaryOutputUI.vwPrimeCheck->setChecked(data.vwPrime == ".true.");
    boundaryOutputUI.cpCheck->setChecked(data.cp == ".true.");
}


void structWidget::updateReferenceBox(const structCFDDefinition &def)
{
    const auto &data = def.referenceData;  // 假设 structDefinition 里有 ReferenceData referenceData;

    // === 阻断信号，防止联动干扰 ===
    QSignalBlocker b1(refUI.nondimBtn);
    QSignalBlocker b2(refUI.dimSIBtn);
    QSignalBlocker b3(refUI.gridLengthConversionSpin);
    QSignalBlocker b4(refUI.machSpin);
    QSignalBlocker b5(refUI.reynoldsSpin);
    QSignalBlocker b6(refUI.velocitySpin);
    QSignalBlocker b7(refUI.densitySpin);
    QSignalBlocker b8(refUI.tempSpin);
    QSignalBlocker b9(refUI.tempUnitCombo);
    QSignalBlocker b10(refUI.alphaSpin);
    QSignalBlocker b11(refUI.yawSpin);

    // === 基础值同步 ===
    refUI.gridLengthConversionSpin->setValue(data.gridlengthConversion);
    refUI.machSpin->setValue(data.mach);
    refUI.reynoldsSpin->setValue(data.reynolds);
    refUI.velocitySpin->setValue(data.velocity);
    refUI.densitySpin->setValue(data.density);
    refUI.tempSpin->setValue(data.temperature);
    refUI.tempUnitCombo->setCurrentText(data.tempUnit);
    refUI.alphaSpin->setValue(data.alpha);
    refUI.yawSpin->setValue(data.yaw);

    // === 维度类型 ===
    bool nondim = (cleanQuotedText(data.dimType) == "nondimensional");
    refUI.nondimBtn->setChecked(nondim);
    refUI.dimSIBtn->setChecked(!nondim);

    // === 根据类型启用/禁用控件 ===
    refUI.machSpin->setEnabled(nondim);
    refUI.reynoldsSpin->setEnabled(nondim);
    refUI.velocitySpin->setEnabled(!nondim);
    refUI.densitySpin->setEnabled(!nondim);
}


void structWidget::initGlobalSettingsBox() {
    // === 1️⃣ 全局设置区域 ===
    globalSettingsBox = new QGroupBox(tr("全局设置"));
    QVBoxLayout* vLayout = new QVBoxLayout(globalSettingsBox);

    // === 参数显示模式 ===
    QGroupBox* displayModelBox = new QGroupBox(tr("参数显示模式"));
    QHBoxLayout* modeLayout = new QHBoxLayout();
    commonBtn = new QRadioButton(tr("常用参数"));
    allBtn = new QRadioButton(tr("全部参数"));
    commonBtn->setChecked(true);
    modeLayout->addWidget(commonBtn);
    modeLayout->addWidget(allBtn);
    modeLayout->addStretch();
    displayModelBox->setLayout(modeLayout);
    vLayout->addWidget(displayModelBox);
    //vLayout->addWidget(globalBox);
    //vLayout->addStretch();

    mainLayout->addWidget(globalSettingsBox);

    // === 2️⃣ 单选逻辑连接 ===
    connect(commonBtn, &QRadioButton::toggled, this, [=](bool checked){
        if (checked)
            hideOptionalWidget(); // 常用参数 → 隐藏
    });

    connect(allBtn, &QRadioButton::toggled, this, [=](bool checked){
        if (checked)
            showOptionalWidget(); // 全部参数 → 显示
    });

    // 默认状态：常用参数 → 隐藏

}



/* ---------------------- Flow Model Box ---------------------- */
void structWidget::initTurbulentDiffusionBox()
{
    turbulentDiffusionBox = new QGroupBox(tr("湍流模型"));
    spalartBox = new QGroupBox(tr("Spalart–Allmaras Model"));
    QVBoxLayout* main = new QVBoxLayout(turbulentDiffusionBox);

    // === 湍流模型 ===

    QVBoxLayout* turbLayout = new QVBoxLayout();
    QStringList turbModels = {"sa", "des","menter-sst", "sst", "kw-des", "hrles"};

    flowUI.turbGroup = new QButtonGroup(this);
    flowUI.turbGroup->setExclusive(true);

    for (const QString &name : turbModels) {
        QRadioButton* btn = new QRadioButton(name);
        flowUI.turbGroup->addButton(btn);
        turbLayout->addWidget(btn);
        flowUI.turbButtons << btn;
    }

    // 默认选第一个（SA）
    if (!flowUI.turbButtons.isEmpty())
        flowUI.turbButtons.first()->setChecked(true);

    turbLayout->addStretch();
    main->addLayout(turbLayout);

    // === 湍流参数 ===
    QGroupBox* turbParamBox = new QGroupBox(tr("湍流参数"));
    QFormLayout* form = new QFormLayout(turbParamBox);

    flowUI.turbIntensitySpin = new QDoubleSpinBox();
    flowUI.turbIntensitySpin->setRange(0.0001, 0.05);
    flowUI.turbIntensitySpin->setValue(0.001);
    form->addRow(tr("湍流强度"), flowUI.turbIntensitySpin);

    flowUI.turbMuRatioSpin = new QDoubleSpinBox();
    flowUI.turbMuRatioSpin->setRange(0.001, 100.0);
    flowUI.turbMuRatioSpin->setValue(0.001);
    form->addRow(tr("湍流粘度比"), flowUI.turbMuRatioSpin);

    flowUI.turbCompressCombo = new QComboBox();
    flowUI.turbCompressCombo->addItems({"off", "on"});
    form->addRow(tr("可压缩修正"), flowUI.turbCompressCombo);

    flowUI.turbCondCombo = new QComboBox();
    flowUI.turbCondCombo->addItems({"off", "on"});
    form->addRow(tr("导热修正"), flowUI.turbCondCombo);

    flowUI.prSpin = new QDoubleSpinBox();
    flowUI.prSpin->setRange(0.1, 5.0);
    flowUI.prSpin->setValue(0.9);
    form->addRow(tr("普朗特数"), flowUI.prSpin);

    flowUI.scSpin = new QDoubleSpinBox();
    flowUI.scSpin->setRange(0.1, 5.0);
    flowUI.scSpin->setValue(1.0);
    form->addRow(tr("施密特数"), flowUI.scSpin);

    main->addWidget(turbParamBox);

    // ===== 联动 1️⃣：非湍流时禁用模型与参数区 =====
    connect(eqnUI.viscousTermsCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, [=](int index){

        // 判断当前选中项是否为湍流模式（根据实际选项设置条件）
        bool enableTurb = (index == 2); // 假设索引 1 是湍流选项

        turbulentDiffusionBox->setEnabled(enableTurb);
        if(enableTurb){
            flowUI.turbGroup->setExclusive(true);
            flowUI.turbButtons.first()->setChecked(true);
        }else{
            flowUI.turbGroup->setExclusive(false); // 允许不选中任何按钮
            for (QRadioButton* btn : flowUI.turbButtons) {
                btn->setChecked(false);
            }
        }
        //turbParamBox->setEnabled(enableTurb);
        //spalartBox->setEnabled(enableTurb); // 非湍流模式时禁用 SA 模型

    });

    // ===== 联动 2️⃣：turbulence 子模型变化时，控制 spalartBox =====
    for (QRadioButton* btn : qAsConst(flowUI.turbButtons)) {
        connect(btn, &QRadioButton::toggled, this, [this, btn](bool checked){
            if (!checked) return;

            QString model = btn->text().toLower();
            QString modelNorm = model;
            modelNorm.replace(QString::fromUtf8("–"), "-");
            modelNorm.remove(' ');

            bool isSA = modelNorm.contains("sa") ||
                        modelNorm.contains("sa-des") ||
                        modelNorm.contains("sades");

            spalartBox->setEnabled(isSA);
        });
    }

    // 初始化同步一次 spalartBox 状态
    {
        bool isSA = false;
        for (QRadioButton* btn : qAsConst(flowUI.turbButtons)) {
            if (btn->isChecked()) {
                QString model = btn->text().toLower();
                QString modelNorm = model;
                modelNorm.replace(QString::fromUtf8("–"), "-");
                modelNorm.remove(' ');
                isSA = modelNorm.contains("spalart") ||
                       modelNorm.contains("sa-des") ||
                       modelNorm.contains("sades");
                break;
            }
        }
        spalartBox->setEnabled(isSA);
    }

    // ===== 联动 3️⃣：可压缩修正时才启用 Pr/Sc =====
    connect(flowUI.turbCompressCombo, &QComboBox::currentTextChanged, this, [=](const QString &text){
        bool compressible = (text.compare("on", Qt::CaseInsensitive) == 0);
        flowUI.prSpin->setEnabled(compressible);
        flowUI.scSpin->setEnabled(compressible);
    });

    // 初始化 Pr/Sc 状态
    bool compressible = (flowUI.turbCompressCombo->currentText().compare("on", Qt::CaseInsensitive) == 0);
    flowUI.prSpin->setEnabled(compressible);
    flowUI.scSpin->setEnabled(compressible);

    // 初始状态（turbulent 被默认选中）
    //turbBox->setEnabled(true);
    turbParamBox->setEnabled(true);

    main->addWidget(spalartBox);
    mainLayout->addWidget(turbulentDiffusionBox);
}

void structWidget::initSpalartBox()
{
    QVBoxLayout *layout = new QVBoxLayout(spalartBox);

    // 🌊 turbinf
    QLabel *turbInfLabel = new QLabel("Freestream turbulence:");
    spalartUI.turbInfSpin = new QDoubleSpinBox();
    spalartUI.turbInfSpin->setRange(0.0, 100.0);
    spalartUI.turbInfSpin->setValue(3.0);
    spalartUI.turbInfSpin->setDecimals(3);
    QHBoxLayout *turbInfLayout = new QHBoxLayout();
    turbInfLayout->addWidget(turbInfLabel);
    turbInfLayout->addWidget(spalartUI.turbInfSpin);

    // 🌀 Dacles–Mariani
    spalartUI.daclesCheck = new QCheckBox("Dacles–Mariani correction");
    spalartUI.daclesCheck->setToolTip("Activates Dacles–Mariani rotation correction (SA-R)");

    // ⚙️ SA–RC correction
    spalartUI.sarcCheck = new QCheckBox("Rotation/curvature correction (sarc)");
    spalartUI.sarcCheck->setToolTip("Activates rotation/curvature correction (SA-RC)");

    // 🧩 Delayed DES
    spalartUI.ddesCheck = new QCheckBox("Delayed DES (ddes)");
    spalartUI.ddesCheck->setToolTip("Changes turbulence_model='des' into delayed DES (DDES)");

    layout->addLayout(turbInfLayout);
    layout->addWidget(spalartUI.daclesCheck);
    layout->addWidget(spalartUI.sarcCheck);
    layout->addWidget(spalartUI.ddesCheck);
}



/* ---------------------- Reference Box ---------------------- */
void structWidget::initReferenceBox()
{
    refBox = new QGroupBox(tr("参考物理量"));
    QVBoxLayout* vbox = new QVBoxLayout(refBox);

    // 输入类型
    QGroupBox* dimTypeBox = new QGroupBox(tr("输入类型"));
    QVBoxLayout* vLayout = new QVBoxLayout(dimTypeBox);
    QHBoxLayout* hLayout = new QHBoxLayout();
    QHBoxLayout* dimLayout = new QHBoxLayout();
    refUI.nondimBtn = new QRadioButton(tr("无量纲"));
    refUI.dimSIBtn  = new QRadioButton(tr("有量纲-SI"));
    refUI.gridLengthConversionSpin = new QDoubleSpinBox();
    QLabel* label = new QLabel(tr("缩放因子"));

    refUI.nondimBtn->setChecked(true);
    refUI.dimGroup = new QButtonGroup(this);
    refUI.gridLengthConversionSpin->setRange(0.0,1000);
    refUI.gridLengthConversionSpin->setValue(1.0);
    refUI.dimGroup->addButton(refUI.nondimBtn);
    refUI.dimGroup->addButton(refUI.dimSIBtn);
    dimLayout->addWidget(refUI.nondimBtn);
    dimLayout->addWidget(refUI.dimSIBtn);
    hLayout->addWidget(label);
    hLayout->addWidget(refUI.gridLengthConversionSpin);
    vLayout->addLayout(dimLayout);
    vLayout->addLayout(hLayout);

    vbox->addWidget(dimTypeBox);

    // 参数区
    QGroupBox* paramBox = new QGroupBox(tr("参考条件"));
    QFormLayout* form = new QFormLayout(paramBox);


    refUI.machSpin = new QDoubleSpinBox();
    refUI.machSpin->setRange(0.0, 50.0);
    refUI.machSpin->setValue(0.8);
    form->addRow(tr("马赫数"), refUI.machSpin);

    refUI.reynoldsSpin = new QDoubleSpinBox();
    refUI.reynoldsSpin->setRange(0.0, 1e9);
    refUI.reynoldsSpin->setValue(6e6);
    form->addRow(tr("雷诺数"), refUI.reynoldsSpin);

    refUI.velocitySpin = new QDoubleSpinBox();
    refUI.velocitySpin->setRange(0.0, 5000.0);
    refUI.velocitySpin->setValue(300);
    form->addRow(tr("速度 (m/s)"), refUI.velocitySpin);

    refUI.densitySpin = new QDoubleSpinBox();
    refUI.densitySpin->setRange(0.0, 10.0);
    refUI.densitySpin->setValue(1.225);
    refUI.densitySpin->setDecimals(3);
    form->addRow(tr("密度 (kg/m³)"), refUI.densitySpin);

    refUI.tempSpin = new QDoubleSpinBox();
    refUI.tempSpin->setRange(0.0, 5000.0);
    refUI.tempSpin->setValue(273.0);
    form->addRow(tr("温度"), refUI.tempSpin);

    refUI.tempUnitCombo = new QComboBox();
    refUI.tempUnitCombo->addItems({"Kelvin", "Rankine"});
    form->addRow(tr("温度单位"), refUI.tempUnitCombo);

    refUI.alphaSpin = new QDoubleSpinBox();
    refUI.alphaSpin->setRange(-180, 180);
    refUI.alphaSpin->setValue(6.0);
    form->addRow(tr("攻角 α (°)"), refUI.alphaSpin);

    refUI.yawSpin = new QDoubleSpinBox();
    refUI.yawSpin->setRange(-180, 180);
    refUI.yawSpin->setValue(0.0);
    form->addRow(tr("偏航角 β (°)"), refUI.yawSpin);

    vbox->addWidget(paramBox);

    connect(refUI.dimGroup, QOverload<QAbstractButton *>::of(&QButtonGroup::buttonClicked),
            this, [=](QAbstractButton* btn){
        bool nondim = (btn == refUI.nondimBtn);
        refUI.machSpin->setEnabled(nondim);
        refUI.reynoldsSpin->setEnabled(nondim);
        refUI.velocitySpin->setEnabled(!nondim);
        refUI.densitySpin->setEnabled(!nondim);
    });

    refUI.machSpin->setEnabled(true);
    refUI.reynoldsSpin->setEnabled(true);
    refUI.velocitySpin->setEnabled(false);
    refUI.densitySpin->setEnabled(false);

    mainLayout->addWidget(refBox);
}
// structWidget.cpp 中添加
void structWidget::initEquationBox()
{
    eqnBox = new QGroupBox(tr("控制方程设置"));
    QVBoxLayout* main = new QVBoxLayout(eqnBox);
    QFormLayout* form = new QFormLayout();

    // === 方程类型 ===
    QLabel* eqnTypeLabel = new QLabel(tr("方程类型"));
    eqnUI.eqnTypeCombo = new QComboBox();
    eqnUI.eqnTypeCombo->addItems({"compressible", "incompressible", "cal_perf_compress"});
    eqnUI.eqnTypeCombo->setCurrentText("compressible");
    form->addRow(eqnTypeLabel, eqnUI.eqnTypeCombo);

    // === 人工可压缩性 ===
    QLabel* artificialLabel = new QLabel(tr("人工可压缩性"));
    eqnUI.artificialCompressSpin = new QDoubleSpinBox();
    eqnUI.artificialCompressSpin->setRange(1.0, 100.0);
    eqnUI.artificialCompressSpin->setDecimals(2);
    eqnUI.artificialCompressSpin->setValue(15.0);
    form->addRow(artificialLabel, eqnUI.artificialCompressSpin);

    // === 黏性项 ===
    QLabel* viscousLabel = new QLabel(tr("黏性项"));
    eqnUI.viscousTermsCombo = new QComboBox();
    eqnUI.viscousTermsCombo->addItems({"inviscid", "laminar", "turbulent"});
    eqnUI.viscousTermsCombo->setCurrentText("turbulent");
    form->addRow(viscousLabel, eqnUI.viscousTermsCombo);

    // === 化学动力学 ===
    QLabel* chemLabel = new QLabel(tr("化学动力学"));
    eqnUI.chemKineticsCombo = new QComboBox();
    eqnUI.chemKineticsCombo->addItems({"frozen", "finite-rate"});
    form->addRow(chemLabel, eqnUI.chemKineticsCombo);
    optionalWidgetArray.append(chemLabel);
    optionalWidgetArray.append(eqnUI.chemKineticsCombo);

    // === 热能模型 ===
    QLabel* thermalLabel = new QLabel(tr("热能模型"));
    eqnUI.thermalModelCombo = new QComboBox();
    eqnUI.thermalModelCombo->addItems({"frozen", "non-equilib"});
    form->addRow(thermalLabel, eqnUI.thermalModelCombo);
    optionalWidgetArray.append(thermalLabel);
    optionalWidgetArray.append(eqnUI.thermalModelCombo);

    // === 分子普朗特数 ===
    QLabel* prandtlLabel = new QLabel(tr("分子普朗特数"));
    eqnUI.prandtlSpin = new QDoubleSpinBox();
    eqnUI.prandtlSpin->setRange(0.1, 5.0);
    eqnUI.prandtlSpin->setValue(0.72);
    form->addRow(prandtlLabel, eqnUI.prandtlSpin);

    // === 施密特数 ===
    QLabel* schmidtLabel = new QLabel(tr("施密特数"));
    eqnUI.schmidtSpin = new QDoubleSpinBox();
    eqnUI.schmidtSpin->setRange(-1.0, 5.0);
    eqnUI.schmidtSpin->setValue(-1.0);
    form->addRow(schmidtLabel, eqnUI.schmidtSpin);
    optionalWidgetArray.append(schmidtLabel);
    optionalWidgetArray.append(eqnUI.schmidtSpin);

    // === 辐射模型 ===
    QLabel* radiationLabel = new QLabel(tr("气体辐射"));
    eqnUI.gasRadiationCombo = new QComboBox();
    eqnUI.gasRadiationCombo->addItems({"off", "uncoupled", "coupled"});
    form->addRow(radiationLabel, eqnUI.gasRadiationCombo);
    optionalWidgetArray.append(radiationLabel);
    optionalWidgetArray.append(eqnUI.gasRadiationCombo);

    // === 辐射隐式线 ===
    eqnUI.radUseImplCheck = new QCheckBox(tr("使用隐式线"));
    form->addRow(QString(), eqnUI.radUseImplCheck);
    optionalWidgetArray.append(eqnUI.radUseImplCheck);

    // === 多组分扩散 ===
    eqnUI.multiDiffCheck = new QCheckBox(tr("多组分扩散"));
    form->addRow(QString(), eqnUI.multiDiffCheck);
    optionalWidgetArray.append(eqnUI.multiDiffCheck);

    // === Cv 最小因子 ===
    //QLabel* cpivLabel = new QLabel(tr("Cv 最小因子 (cpiv_min_factor)"));
    //eqnUI.cpivMinSpin = new QDoubleSpinBox();
    //eqnUI.cpivMinSpin->setRange(0.0, 0.1);
    //eqnUI.cpivMinSpin->setValue(0.0001);
    //form->addRow(cpivLabel, eqnUI.cpivMinSpin);
    //optionalWidgetArray.append(cpivLabel);
    //optionalWidgetArray.append(eqnUI.cpivMinSpin);

    // === 限制项 ===
    //eqnUI.augmentKineticsCheck = new QCheckBox(tr("增强化学源项限制 (augment_kinetics_limiting)"));
    //eqnUI.implicitRateCheck = new QCheckBox(tr("隐式速率限制 (implicit_rate_limiting)"));
    //form->addRow(QString(), eqnUI.augmentKineticsCheck);
    //form->addRow(QString(), eqnUI.implicitRateCheck);
    //optionalWidgetArray.append(eqnUI.augmentKineticsCheck);
    //optionalWidgetArray.append(eqnUI.implicitRateCheck);

    main->addLayout(form);

    // === 互斥逻辑 ===
    connect(eqnUI.eqnTypeCombo, &QComboBox::currentTextChanged, this, [=](const QString& type){
        bool isIncompressible = (type == "incompressible");
        bool isGeneric = (type == "cal_perf_compress");

        eqnUI.artificialCompressSpin->setEnabled(isIncompressible);

        eqnUI.chemKineticsCombo->setEnabled(isGeneric);
        eqnUI.thermalModelCombo->setEnabled(isGeneric);
        eqnUI.gasRadiationCombo->setEnabled(isGeneric);
        eqnUI.radUseImplCheck->setEnabled(isGeneric);
        eqnUI.multiDiffCheck->setEnabled(isGeneric);
        //eqnUI.cpivMinSpin->setEnabled(isGeneric);
        //eqnUI.augmentKineticsCheck->setEnabled(isGeneric);
        //eqnUI.implicitRateCheck->setEnabled(isGeneric);
    });

    emit eqnUI.eqnTypeCombo->currentTextChanged(eqnUI.eqnTypeCombo->currentText());

    mainLayout->addWidget(eqnBox);
}

void structWidget::initForceMomentBox()
{
    forceMomentBox = new QGroupBox(tr("气动力积分属性"));
    QFormLayout* form = new QFormLayout(forceMomentBox);

    // === 参考面积 ===
    fmUI.areaReferenceSpin = new QDoubleSpinBox();
    fmUI.areaReferenceSpin->setRange(0.0001, 1e6);
    fmUI.areaReferenceSpin->setDecimals(4);
    fmUI.areaReferenceSpin->setValue(1.0);
    fmUI.areaReferenceSpin->setToolTip(tr("用于力和力矩无量纲化的参考面积"));
    form->addRow(tr("参考面积"), fmUI.areaReferenceSpin);

    // === X方向力矩参考长度 ===
    fmUI.xMomentLengthSpin = new QDoubleSpinBox();
    fmUI.xMomentLengthSpin->setRange(0.0001, 1e6);
    fmUI.xMomentLengthSpin->setDecimals(4);
    fmUI.xMomentLengthSpin->setValue(1.0);
    fmUI.xMomentLengthSpin->setToolTip(tr("X方向的参考长度，用于无量纲化绕Y轴的俯仰力矩"));
    form->addRow(tr("X方向参考长度"), fmUI.xMomentLengthSpin);

    // === Y方向力矩参考长度 ===
    fmUI.yMomentLengthSpin = new QDoubleSpinBox();
    fmUI.yMomentLengthSpin->setRange(0.0001, 1e6);
    fmUI.yMomentLengthSpin->setDecimals(4);
    fmUI.yMomentLengthSpin->setValue(1.0);
    fmUI.yMomentLengthSpin->setToolTip(tr("Y方向的参考长度，用于无量纲化绕X和Z轴的滚转/偏航力矩"));
    form->addRow(tr("Y方向参考长度"), fmUI.yMomentLengthSpin);

    // === 力矩中心 ===
    fmUI.xMomentCenterSpin = new QDoubleSpinBox();
    fmUI.xMomentCenterSpin->setRange(-1e6, 1e6);
    fmUI.xMomentCenterSpin->setDecimals(4);
    fmUI.xMomentCenterSpin->setValue(0.0);
    fmUI.xMomentCenterSpin->setToolTip(tr("力矩中心的 X 坐标"));
    form->addRow(tr("X力矩中心"), fmUI.xMomentCenterSpin);

    fmUI.yMomentCenterSpin = new QDoubleSpinBox();
    fmUI.yMomentCenterSpin->setRange(-1e6, 1e6);
    fmUI.yMomentCenterSpin->setDecimals(4);
    fmUI.yMomentCenterSpin->setValue(0.0);
    fmUI.yMomentCenterSpin->setToolTip(tr("力矩中心的 Y 坐标"));
    form->addRow(tr("Y力矩中心"), fmUI.yMomentCenterSpin);

    fmUI.zMomentCenterSpin = new QDoubleSpinBox();
    fmUI.zMomentCenterSpin->setRange(-1e6, 1e6);
    fmUI.zMomentCenterSpin->setDecimals(4);
    fmUI.zMomentCenterSpin->setValue(0.0);
    fmUI.zMomentCenterSpin->setToolTip(tr("力矩中心的 Z 坐标"));
    form->addRow(tr("Z力矩中心"), fmUI.zMomentCenterSpin);

    // === 添加到主布局 ===
    mainLayout->addWidget(forceMomentBox);
}
void structWidget::initRawGridBox()
{
    rawGridBox = new QGroupBox(tr("网格格式设置"));
    QFormLayout* form = new QFormLayout(rawGridBox);

    // === 网格文件格式 ===
    rawUI.gridFormatCombo = new QComboBox();
    rawUI.gridFormatCombo->addItems({"fast", "vgrid", "fun2d", "aflr3", "felisa", "fieldview"});
    rawUI.gridFormatCombo->setCurrentText("aflr3");
    rawUI.gridFormatCombo->setToolTip(tr("指定网格文件格式"));
    form->addRow(tr("网格格式"), rawUI.gridFormatCombo);

    // === 数据编码格式 ===
    rawUI.dataFormatCombo = new QComboBox();
    rawUI.dataFormatCombo->addItems({"default", "ascii", "unformatted", "stream", "stream64"});
    rawUI.dataFormatCombo->setCurrentText("stream");
    rawUI.dataFormatCombo->setToolTip(tr("网格文件编码格式，与 grid_format 必须匹配"));
    form->addRow(tr("数据格式"), rawUI.dataFormatCombo);

    // === 二维模式 ===
    rawUI.twodModeCheck = new QCheckBox(tr("启用二维模式"));
    rawUI.twodModeCheck->setToolTip(tr("若为真，则使用二维模式 (单层六面体或棱柱网格)"));
    form->addRow(rawUI.twodModeCheck);

    // === 交换 YZ 轴 ===
    rawUI.swapYZAxesCheck = new QCheckBox(tr("交换 Y-Z 轴"));
    rawUI.swapYZAxesCheck->setToolTip(tr("将 Y 与 Z 轴互换以适配 Fun3D 的攻角坐标约定"));
    form->addRow(rawUI.swapYZAxesCheck);

    // === FieldView 精度 ===
    rawUI.fieldviewPrecisionCombo = new QComboBox();
    rawUI.fieldviewPrecisionCombo->addItems({"double", "single"});
    rawUI.fieldviewPrecisionCombo->setCurrentText("double");
    rawUI.fieldviewPrecisionCombo->setToolTip(tr("仅对 FieldView 网格有效，指定坐标浮点精度"));
    form->addRow(tr("FieldView 精度"), rawUI.fieldviewPrecisionCombo);

    // === Patch Lumping ===
    rawUI.patchLumpingCombo = new QComboBox();
    rawUI.patchLumpingCombo->addItems({"none", "bc", "family"});
    rawUI.patchLumpingCombo->setCurrentText("none");
    rawUI.patchLumpingCombo->setToolTip(tr("边界 Patch 合并模式"));
    form->addRow(tr("Patch 合并"), rawUI.patchLumpingCombo);

    // === 忽略欧拉数检查 ===
    rawUI.ignoreEulerCheck = new QCheckBox(tr("忽略欧拉数检查"));
    rawUI.ignoreEulerCheck->setToolTip(tr("若为真，将跳过欧拉数一致性检查"));
    form->addRow(rawUI.ignoreEulerCheck);

    //


    // === 加入主布局 ===
    mainLayout->addWidget(rawGridBox);
}

void structWidget::initGridGenBox()
{
    gridGenBox = new QGroupBox(tr("网格生成参数"));
    QFormLayout* form = new QFormLayout(gridGenBox);

    // === 网格类型选择（居中） ===
    QHBoxLayout *gridTypeLayout = new QHBoxLayout();

    gridUI.oGridRadio = new QRadioButton(tr("O型网格"));
    gridUI.cGridRadio = new QRadioButton(tr("C型网格"));
    gridUI.oGridRadio->setObjectName("oGrid");
    gridUI.cGridRadio->setObjectName("cGrid");
    gridUI.oGridRadio->setAttribute(Qt::WA_TransparentForMouseEvents, true);
    gridUI.cGridRadio->setAttribute(Qt::WA_TransparentForMouseEvents, true);


    gridUI.oGridRadio->setChecked(true);

    gridUI.oGridRadio->setToolTip(tr("适用于全包围网格，无尾迹展开"));
    gridUI.cGridRadio->setToolTip(tr("适用于带尾迹展开的网格（常用于机翼）"));

    // 左右加弹簧，实现居中
    gridTypeLayout->addStretch();
    gridTypeLayout->addWidget(gridUI.oGridRadio);
    gridTypeLayout->addSpacing(20);
    gridTypeLayout->addWidget(gridUI.cGridRadio);
    gridTypeLayout->addStretch();

    // ⭐ 关键：用一整行插入
    form->addRow(gridTypeLayout);


    // === 翼型表面点数 ===
    gridUI.nsrfSpin = new QSpinBox();
    gridUI.nsrfSpin->setRange(100, 400);
    gridUI.nsrfSpin->setValue(200);
    gridUI.nsrfSpin->setToolTip(tr("翼型表面离散点数（影响表面分辨率）"));
    form->addRow(tr("表面点数"), gridUI.nsrfSpin);
    gridUI.nsrfSpin->setObjectName("nsrf");

    // === 尾迹点数 ===
    gridUI.wakeSpin = new QSpinBox();
    gridUI.wakeSpin->setRange(100, 400);
    gridUI.wakeSpin->setValue(100);
    gridUI.wakeSpin->setToolTip(tr("尾迹方向网格点数（C型网格展开时使用）"));
    form->addRow(tr("尾迹点数"), gridUI.wakeSpin);
    gridUI.wakeSpin->setObjectName("wakeNum");

    // === 法向网格层数 ===
    gridUI.jMaxSpin = new QSpinBox();
    gridUI.jMaxSpin->setRange(80, 400);
    gridUI.jMaxSpin->setValue(100);
    gridUI.jMaxSpin->setToolTip(tr("垂直于翼型表面的网格层数"));
    form->addRow(tr("法向层数"), gridUI.jMaxSpin);
    gridUI.jMaxSpin->setObjectName("jMake");

    // === 前缘间距 ===
    gridUI.lespSpin = new QDoubleSpinBox();
    gridUI.lespSpin->setRange(0.0001, 10);
    gridUI.lespSpin->setDecimals(6);
    gridUI.lespSpin->setValue(0.001);
    gridUI.lespSpin->setToolTip(tr("翼型前缘局部加密控制（越小越密）"));
    form->addRow(tr("前缘密度"), gridUI.lespSpin);
    gridUI.lespSpin->setObjectName("lesp");

    // === 后缘间距 ===
    gridUI.tespSpin = new QDoubleSpinBox();
    gridUI.tespSpin->setRange(0.000001, 10);
    gridUI.tespSpin->setDecimals(6);
    gridUI.tespSpin->setValue(0.00002);
    gridUI.tespSpin->setToolTip(tr("翼型后缘局部加密控制"));
    form->addRow(tr("后缘密度"), gridUI.tespSpin);
    gridUI.tespSpin->setObjectName("tesp");

    // === 远场半径 ===
    gridUI.radiSpin = new QDoubleSpinBox();
    gridUI.radiSpin->setRange(15, 100.0);
    gridUI.radiSpin->setDecimals(2);
    gridUI.radiSpin->setValue(20.0);
    gridUI.radiSpin->setToolTip(tr("计算域远场半径（单位：弦长）"));
    form->addRow(tr("远场半径"), gridUI.radiSpin);
    gridUI.radiSpin->setObjectName("radi");

    // === 雷诺数 ===
    gridUI.reSpin = new QDoubleSpinBox();
    gridUI.reSpin->setRange(1e3, 1e9);
    gridUI.reSpin->setDecimals(0);
    gridUI.reSpin->setValue(1e6);
    gridUI.reSpin->setToolTip(tr("流动雷诺数（用于边界层估计）"));
    form->addRow(tr("雷诺数"), gridUI.reSpin);
    gridUI.reSpin->setObjectName("re");

    // === Y+ ===
    gridUI.yPlusSpin = new QDoubleSpinBox();
    gridUI.yPlusSpin->setRange(0.1, 50.0);
    gridUI.yPlusSpin->setDecimals(2);
    gridUI.yPlusSpin->setValue(1.0);
    gridUI.yPlusSpin->setToolTip(tr("目标壁面 Y+（控制第一层高度）"));
    form->addRow(tr("目标Y+"), gridUI.yPlusSpin);
    gridUI.yPlusSpin->setObjectName("yPlus");
    //
    gridUI.genMeshButton = new QPushButton();
    gridUI.genMeshButton->setText("生成网格");


    //
    gridUI.genTypeBox = new QComboBox();
    gridUI.genTypeBox->addItem("自动光顺");
    gridUI.genTypeBox->addItem("按节点分布");
    gridUI.genTypeBox->setCurrentIndex(0);
    form->addRow(tr("网格生成方式"),gridUI.genTypeBox);

    form->addRow(gridUI.genMeshButton);


    // 初始状态
    gridUI.wakeSpin->setEnabled(gridUI.cGridRadio->isChecked());

    // 联动
    connect(gridUI.oGridRadio, &QRadioButton::toggled, this, [=](bool checked){
        if (checked) {
            gridUI.wakeSpin->setEnabled(false);
        }
    });

    connect(gridUI.cGridRadio, &QRadioButton::toggled, this, [=](bool checked){
        if (checked) {
            gridUI.wakeSpin->setEnabled(true);
        }
    });


    // === 加入主布局 ===
    mainLayout->addWidget(gridGenBox);
}

void structWidget::initAirfoilInfoBox()
{
    airfoilBox = new QGroupBox(tr("翼型信息"));

    QVBoxLayout* mainLayoutBox = new QVBoxLayout(airfoilBox);

    // ===== 表单 =====
    QFormLayout* form = new QFormLayout();

    airfoilInfoUI.pointCountLabel = new QLabel("-");
    form->addRow(tr("坐标点数:"), airfoilInfoUI.pointCountLabel);

    airfoilInfoUI.trailingEdgeLabel = new QLabel("-");
    form->addRow(tr("尾缘状态:"), airfoilInfoUI.trailingEdgeLabel);

    mainLayoutBox->addLayout(form);

    // ===== 按钮 =====
    airfoilInfoUI.selectBtn = new QPushButton(tr("选择翼型"));

    QHBoxLayout* btnLayout = new QHBoxLayout();
    btnLayout->addStretch();
    btnLayout->addWidget(airfoilInfoUI.selectBtn);

    mainLayoutBox->addLayout(btnLayout);



    // ===== 加入主布局 =====
    mainLayout->addWidget(airfoilBox);
}

void structWidget::initInviscidFluxBox() {
    fluxBox = new QGroupBox(tr("无粘通量方法"));
    QVBoxLayout *layout = new QVBoxLayout(fluxBox);

    // 1️⃣ flux_construction
    QLabel *fluxConsLabel = new QLabel("Flux construction:");
    fluxUI.fluxConsCombo = new QComboBox();
    fluxUI.fluxConsCombo->addItems({
        "roe", "vanleer", "hllc", "aufs", "ldfss", "dldfss",
        "aldfss", "roe ec", "stvd", "stvd modified", "multidm"
    });
    QHBoxLayout *fluxConsLayout = new QHBoxLayout();
    fluxConsLayout->addWidget(fluxConsLabel);
    fluxConsLayout->addWidget(fluxUI.fluxConsCombo);
    layout->addLayout(fluxConsLayout);

    // 2️⃣ flux_construction_lhs
    QLabel *fluxLhsLabel = new QLabel("Flux construction:");
    fluxUI.fluxConsLhsCombo = new QComboBox();
    fluxUI.fluxConsLhsCombo->addItems({
        "consistent", "vanleer", "roe", "hllc", "aufs", "ldfss"
    });
    QHBoxLayout *fluxLhsLayout = new QHBoxLayout();
    fluxLhsLayout->addWidget(fluxLhsLabel);
    fluxLhsLayout->addWidget(fluxUI.fluxConsLhsCombo);
    layout->addLayout(fluxLhsLayout);

    // 3️⃣ kappa_umuscl
    QLabel *kappaLabel = new QLabel("Kappa umuscl:");
    fluxUI.kappaSpin = new QDoubleSpinBox();
    fluxUI.kappaSpin->setRange(-1.0, 1.0);
    fluxUI.kappaSpin->setSingleStep(0.1);
    fluxUI.kappaSpin->setValue(-1.0);
    QHBoxLayout *kappaLayout = new QHBoxLayout();
    kappaLayout->addWidget(kappaLabel);
    kappaLayout->addWidget(fluxUI.kappaSpin);
    layout->addLayout(kappaLayout);

    // 4️⃣ flux limiter
    QLabel *limiterLabel = new QLabel("Flux limiter:");
    fluxUI.fluxLimiterCombo = new QComboBox();
    fluxUI.fluxLimiterCombo->addItems({
        "none", "barth", "venkat", "hminmod", "hvanleer",
        "hvanalbada", "hvenkat", "minmod", "vanleer", "vanleer gg", "vanalbada"
    });
    QHBoxLayout *limiterLayout = new QHBoxLayout();
    limiterLayout->addWidget(limiterLabel);
    limiterLayout->addWidget(fluxUI.fluxLimiterCombo);
    layout->addLayout(limiterLayout);

    // 5️⃣ first order iterations
    QLabel *firstLabel = new QLabel("First order iterations:");
    fluxUI.firstOrderSpin = new QSpinBox();
    fluxUI.firstOrderSpin->setRange(0, 10000);
    QHBoxLayout *firstLayout = new QHBoxLayout();
    firstLayout->addWidget(firstLabel);
    firstLayout->addWidget(fluxUI.firstOrderSpin);
    layout->addLayout(firstLayout);

    // 6️⃣ multidm_option
    QLabel *multiLabel = new QLabel("Multidm option:");
    fluxUI.multiOptionCombo = new QComboBox();
    fluxUI.multiOptionCombo->addItems({"1", "2"});
    QHBoxLayout *multiLayout = new QHBoxLayout();
    multiLayout->addWidget(multiLabel);
    multiLayout->addWidget(fluxUI.multiOptionCombo);
    layout->addLayout(multiLayout);

    // 7️⃣ fixed direction
    fluxUI.fixedDirCheck = new QCheckBox("Fixed direction (.true.)");
    layout->addWidget(fluxUI.fixedDirCheck);

    // 8️⃣ recalc_dir_freq
    QLabel *recalcLabel = new QLabel("Recalc dir freq:");
    fluxUI.recalcSpin = new QSpinBox();
    fluxUI.recalcSpin->setRange(0, 100);
    fluxUI.recalcSpin->setValue(1);
    QHBoxLayout *recalcLayout = new QHBoxLayout();
    recalcLayout->addWidget(recalcLabel);
    recalcLayout->addWidget(fluxUI.recalcSpin);
    layout->addLayout(recalcLayout);

    // 9️⃣ entropy fix
    fluxUI.entropyFixCheck = new QCheckBox("Entropy fix (.false.)");
    layout->addWidget(fluxUI.entropyFixCheck);

    // 🔟 adaptive entropy fix
    fluxUI.adaptEntropyCheck = new QCheckBox("Adaptive entropy fix");
    layout->addWidget(fluxUI.adaptEntropyCheck);

    // 11️⃣ Eigenvalue coefficients
    QLabel *rhsULabel = new QLabel("rhs u eigenvalue coef:");
    QLabel *lhsULabel = new QLabel("lhs u eigenvalue coef:");
    QLabel *rhsALabel = new QLabel("rhs a eigenvalue coef:");
    QLabel *lhsALabel = new QLabel("lhs a eigenvalue coef:");
    fluxUI.rhsUEigenSpin = new QDoubleSpinBox();
    fluxUI.lhsUEigenSpin = new QDoubleSpinBox();
    fluxUI.rhsAEigenSpin = new QDoubleSpinBox();
    fluxUI.lhsAEigenSpin = new QDoubleSpinBox();
    for (auto *s : {fluxUI.rhsUEigenSpin, fluxUI.lhsUEigenSpin,
                    fluxUI.rhsAEigenSpin, fluxUI.lhsAEigenSpin}) {
        s->setRange(0, 10.0);
        s->setSingleStep(0.1);
        s->setValue(0.0);
    }
    QFormLayout *eigenLayout = new QFormLayout();
    eigenLayout->addRow(rhsULabel, fluxUI.rhsUEigenSpin);
    eigenLayout->addRow(lhsULabel, fluxUI.lhsUEigenSpin);
    eigenLayout->addRow(rhsALabel, fluxUI.rhsAEigenSpin);
    eigenLayout->addRow(lhsALabel, fluxUI.lhsAEigenSpin);
    layout->addLayout(eigenLayout);

    // 12️⃣ re_min_vswch / re_max_vswch
    QLabel *reMinLabel = new QLabel("Re min vswch:");
    QLabel *reMaxLabel = new QLabel("Re max vswch:");
    fluxUI.reMinSpin = new QDoubleSpinBox();
    fluxUI.reMaxSpin = new QDoubleSpinBox();
    fluxUI.reMinSpin->setRange(0, 1e6);
    fluxUI.reMaxSpin->setRange(0, 1e6);
    fluxUI.reMinSpin->setValue(50.0);
    fluxUI.reMaxSpin->setValue(500.0);
    QHBoxLayout *reVswchLayout = new QHBoxLayout();
    reVswchLayout->addWidget(reMinLabel);
    reVswchLayout->addWidget(fluxUI.reMinSpin);
    reVswchLayout->addWidget(reMaxLabel);
    reVswchLayout->addWidget(fluxUI.reMaxSpin);
    layout->addLayout(reVswchLayout);

    // ✅ 总布局加入主界面
    mainLayout->addWidget(fluxBox);

    // ✅ 所有可选项加入隐藏容器（label + 控件）
    optionalWidgetArray.append(fluxLhsLabel);
    optionalWidgetArray.append(fluxUI.fluxConsLhsCombo);
    optionalWidgetArray.append(kappaLabel);
    optionalWidgetArray.append(fluxUI.kappaSpin);
    optionalWidgetArray.append(multiLabel);
    optionalWidgetArray.append(fluxUI.multiOptionCombo);
    optionalWidgetArray.append(fluxUI.fixedDirCheck);
    optionalWidgetArray.append(recalcLabel);
    optionalWidgetArray.append(fluxUI.recalcSpin);
    optionalWidgetArray.append(fluxUI.entropyFixCheck);
    optionalWidgetArray.append(fluxUI.adaptEntropyCheck);
    optionalWidgetArray.append(rhsULabel);
    optionalWidgetArray.append(lhsULabel);
    optionalWidgetArray.append(rhsALabel);
    optionalWidgetArray.append(lhsALabel);
    optionalWidgetArray.append(fluxUI.rhsUEigenSpin);
    optionalWidgetArray.append(fluxUI.lhsUEigenSpin);
    optionalWidgetArray.append(fluxUI.rhsAEigenSpin);
    optionalWidgetArray.append(fluxUI.lhsAEigenSpin);
    optionalWidgetArray.append(reMinLabel);
    optionalWidgetArray.append(reMaxLabel);
    optionalWidgetArray.append(fluxUI.reMinSpin);
    optionalWidgetArray.append(fluxUI.reMaxSpin);
}


void structWidget::initCodeRunControlBox()
{
    codeRunBox = new QGroupBox(tr("运行控制"));
    QFormLayout* form = new QFormLayout(codeRunBox);

    // === 基本控制 ===
    codeRunUI.stepsSpin = new QSpinBox();
    codeRunUI.stepsSpin->setRange(1, 1000000);
    codeRunUI.stepsSpin->setValue(500);
    form->addRow(tr("步数"), codeRunUI.stepsSpin);

    codeRunUI.stopTolEdit = new QLineEdit();
    codeRunUI.stopTolEdit->setText("1e-15");
    form->addRow(tr("收敛容差"), codeRunUI.stopTolEdit);

    //codeRunUI.stopTolSpin = new QDoubleSpinBox();
    //codeRunUI.stopTolSpin->setDecimals(8);
    //codeRunUI.stopTolSpin->setRange(1e-20, 1e-3);
    //codeRunUI.stopTolSpin->setValue(1e-15);
    //form->addRow(tr("收敛容差"), codeRunUI.stopTolSpin);

    codeRunUI.durationLimitSpin = new QDoubleSpinBox();
    codeRunUI.durationLimitSpin->setRange(-1.0, 1e6);
    codeRunUI.durationLimitSpin->setValue(-1.0);
    form->addRow(tr("运行时限 (分钟)"), codeRunUI.durationLimitSpin);

    // === 重启控制 ===
    codeRunUI.noRestartCheck = new QCheckBox(tr("不保存重启文件"));
    form->addRow(codeRunUI.noRestartCheck);

    codeRunUI.restartWriteFreqSpin = new QSpinBox();
    codeRunUI.restartWriteFreqSpin->setRange(1, 100000);
    codeRunUI.restartWriteFreqSpin->setValue(250);
    form->addRow(tr("重启写出频率"), codeRunUI.restartWriteFreqSpin);

    codeRunUI.restartReadCombo = new QComboBox();
    codeRunUI.restartReadCombo->addItems({"on", "on nohistorykept", "off"});
    form->addRow(tr("读取重启文件"), codeRunUI.restartReadCombo);

    // === Jacobian 更新策略 ===
    codeRunUI.smartJupdateCheck = new QCheckBox(tr("智能 Jacobian 更新"));
    form->addRow(codeRunUI.smartJupdateCheck);

    codeRunUI.jacobianEvalFreqSpin = new QSpinBox();
    codeRunUI.jacobianEvalFreqSpin->setRange(0, 1000);
    codeRunUI.jacobianEvalFreqSpin->setValue(0);
    form->addRow(tr("Jacobian 评估频率"), codeRunUI.jacobianEvalFreqSpin);

    codeRunUI.jupdateStartupStepsSpin = new QSpinBox();
    codeRunUI.jupdateStartupStepsSpin->setRange(0, 1000);
    codeRunUI.jupdateStartupStepsSpin->setValue(10);
    form->addRow(tr("启动阶段评估步数"), codeRunUI.jupdateStartupStepsSpin);

    codeRunUI.dfduc3Check = new QCheckBox(tr("使用 df/duc³ Jacobian"));
    form->addRow(codeRunUI.dfduc3Check);

    // === 攻角扫描 ===
    codeRunUI.alphaSweepCheck = new QCheckBox(tr("启用攻角扫描"));
    form->addRow(codeRunUI.alphaSweepCheck);

    codeRunUI.cycleIncrementSpin = new QSpinBox();
    codeRunUI.cycleIncrementSpin->setRange(-10000, 10000);
    codeRunUI.cycleIncrementSpin->setValue(50);
    form->addRow(tr("步间攻角递增间隔"), codeRunUI.cycleIncrementSpin);

    codeRunUI.alphaIncrementSpin = new QDoubleSpinBox();
    codeRunUI.alphaIncrementSpin->setRange(-10.0, 10.0);
    codeRunUI.alphaIncrementSpin->setValue(0.25);
    form->addRow(tr("攻角增量"), codeRunUI.alphaIncrementSpin);

    codeRunUI.alphaMaxSpin = new QDoubleSpinBox();
    codeRunUI.alphaMaxSpin->setRange(-180.0, 180.0);
    codeRunUI.alphaMaxSpin->setValue(180.0);
    form->addRow(tr("攻角最大值"), codeRunUI.alphaMaxSpin);

    codeRunUI.alphaMinSpin = new QDoubleSpinBox();
    codeRunUI.alphaMinSpin->setRange(-180.0, 180.0);
    codeRunUI.alphaMinSpin->setValue(-180.0);
    form->addRow(tr("攻角最小值"), codeRunUI.alphaMinSpin);

    codeRunUI.alphaSwitchbacksSpin = new QSpinBox();
    codeRunUI.alphaSwitchbacksSpin->setRange(0, 10);
    codeRunUI.alphaSwitchbacksSpin->setValue(0);
    form->addRow(tr("攻角反转次数"), codeRunUI.alphaSwitchbacksSpin);

    startAnalyseButton = new QPushButton("Start");
    form->addRow(startAnalyseButton);


    mainLayout->addWidget(codeRunBox);

    // === 互斥逻辑 ===
    connect(codeRunUI.smartJupdateCheck, &QCheckBox::toggled, this, [=](bool enabled) {
        codeRunUI.jacobianEvalFreqSpin->setEnabled(!enabled);
    });

    connect(codeRunUI.alphaSweepCheck, &QCheckBox::toggled, this, [=](bool enabled) {
        codeRunUI.cycleIncrementSpin->setEnabled(enabled);
        codeRunUI.alphaIncrementSpin->setEnabled(enabled);
        codeRunUI.alphaMaxSpin->setEnabled(enabled);
        codeRunUI.alphaMinSpin->setEnabled(enabled);
        codeRunUI.alphaSwitchbacksSpin->setEnabled(enabled);
    });

    // 初始化默认禁用攻角扫描参数
    codeRunUI.alphaSweepCheck->setChecked(false);
    codeRunUI.cycleIncrementSpin->setEnabled(false);
    codeRunUI.alphaIncrementSpin->setEnabled(false);
    codeRunUI.alphaMaxSpin->setEnabled(false);
    codeRunUI.alphaMinSpin->setEnabled(false);
    codeRunUI.alphaSwitchbacksSpin->setEnabled(false);
}

void structWidget::initNonlinearSolverBox()
{
    nonlinearSolverBox = new QGroupBox(tr("非线性求解器参数"));
    QFormLayout* form = new QFormLayout(nonlinearSolverBox);

    // ===== time_accuracy =====
    QLabel* timeAccLabel = new QLabel(tr("时间精度"));
    nonlinearUI.timeAccuracyCombo = new QComboBox();
    nonlinearUI.timeAccuracyCombo->addItems({
        "steady", "1storder", "2ndorder", "2ndorderOPT",
        "3rdorder", "4thorderMEBDF4", "4thorderESDIRK4"
    });
    form->addRow(timeAccLabel, nonlinearUI.timeAccuracyCombo);

    // ===== pseudo_time_stepping =====
    QLabel* pseudoLabel = new QLabel(tr("伪时间步"));
    nonlinearUI.pseudoTimeSteppingCombo = new QComboBox();
    nonlinearUI.pseudoTimeSteppingCombo->addItems({"on", "off"});
    form->addRow(pseudoLabel, nonlinearUI.pseudoTimeSteppingCombo);

    // ===== time_step_nondim ===== (optional)
    QLabel* tstepLabel = new QLabel(tr("非维时间步"));
    nonlinearUI.timeStepNondimSpin = new QDoubleSpinBox();
    nonlinearUI.timeStepNondimSpin->setRange(0.0, 1e6);
    nonlinearUI.timeStepNondimSpin->setDecimals(6);
    nonlinearUI.timeStepNondimSpin->setValue(0.0);
    form->addRow(tstepLabel, nonlinearUI.timeStepNondimSpin);
    optionalWidgetArray.append(tstepLabel);
    optionalWidgetArray.append(nonlinearUI.timeStepNondimSpin);

    // ===== subiterations ===== (optional)
    QLabel* subitLabel = new QLabel(tr("子迭代次数"));
    nonlinearUI.subiterationsSpin = new QSpinBox();
    nonlinearUI.subiterationsSpin->setRange(0, 10000);
    nonlinearUI.subiterationsSpin->setValue(0);
    form->addRow(subitLabel, nonlinearUI.subiterationsSpin);
    optionalWidgetArray.append(subitLabel);
    optionalWidgetArray.append(nonlinearUI.subiterationsSpin);

    // ===== temporal_err_control ===== (optional, 单独一行)
    nonlinearUI.temporalErrControlCheck = new QCheckBox(tr("时间误差控制"));
    form->addRow(nonlinearUI.temporalErrControlCheck);
    optionalWidgetArray.append(nonlinearUI.temporalErrControlCheck);

    // ===== temporal_err_floor ===== (optional)
    QLabel* terrLabel = new QLabel(tr("时间误差容限"));
    QLabel* scheduleNameLabel = new QLabel(tr("schedule_number"));
    nonlinearUI.temporalErrFloorSpin = new QDoubleSpinBox();
    nonlinearUI.temporalErrFloorSpin->setRange(0.0, 10.0);
    nonlinearUI.temporalErrFloorSpin->setDecimals(6);
    nonlinearUI.temporalErrFloorSpin->setValue(0.1);
    form->addRow(terrLabel, nonlinearUI.temporalErrFloorSpin);
    optionalWidgetArray.append(terrLabel);
    optionalWidgetArray.append(nonlinearUI.temporalErrFloorSpin);

    nonlinearUI.scheduleNumberSpin = new QSpinBox();
    form->addRow(scheduleNameLabel,nonlinearUI.scheduleNumberSpin);
    optionalWidgetArray.append(scheduleNameLabel);
    optionalWidgetArray.append(nonlinearUI.scheduleNumberSpin);
    // ===== schedule_iteration(1:2) =====
    QLabel* label1 = new QLabel(" ～ ");
    QLabel* label2 = new QLabel(" ～ ");
    QLabel* label3 = new QLabel(" ～ ");
    label1->setAlignment(Qt::AlignCenter);
    label1->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
    label2->setAlignment(Qt::AlignCenter);
    label2->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
    label3->setAlignment(Qt::AlignCenter);
    label3->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
    QLabel* schedIterLabel = new QLabel(tr("CFL 调度起止迭代"));
    QHBoxLayout* schedIterLayout = new QHBoxLayout();
    nonlinearUI.scheduleIteration1Spin = new QSpinBox();
    nonlinearUI.scheduleIteration2Spin = new QSpinBox();
    schedIterLayout->setAlignment(Qt::AlignVCenter); // 保证整体垂直居中
    nonlinearUI.scheduleIteration1Spin->setRange(0,200);
    nonlinearUI.scheduleIteration2Spin->setRange(0,200);
    schedIterLayout->addWidget(nonlinearUI.scheduleIteration1Spin);
    schedIterLayout->addStretch();
    schedIterLayout->addWidget(label1);
    schedIterLayout->addStretch();
    schedIterLayout->addWidget(nonlinearUI.scheduleIteration2Spin);
    QWidget* schedIterWidget = new QWidget();
    schedIterWidget->setLayout(schedIterLayout);
    form->addRow(schedIterLabel);
    form->addRow(schedIterWidget);

    // ===== schedule_cfl =====
    QLabel* schedCflLabel = new QLabel(tr("CFL"));
    QHBoxLayout* schedCflLayout = new QHBoxLayout();
    nonlinearUI.scheduleCfl1Spin = new QDoubleSpinBox();
    nonlinearUI.scheduleCfl2Spin = new QDoubleSpinBox();
    schedCflLabel->setAlignment(Qt::AlignVCenter); // 保证整体垂直居中
    nonlinearUI.scheduleCfl1Spin->setRange(0.0,500);
    nonlinearUI.scheduleCfl2Spin->setRange(0.0,500);
    schedCflLayout->addWidget(nonlinearUI.scheduleCfl1Spin);
    schedCflLayout->addStretch();
    schedCflLayout->addWidget(label2);
    schedCflLayout->addStretch();
    schedCflLayout->addWidget(nonlinearUI.scheduleCfl2Spin);
    QWidget* schedCflWidget = new QWidget();
    schedCflWidget->setLayout(schedCflLayout);
    form->addRow(schedCflLabel);
    form->addRow(schedCflWidget);

    // ===== schedule_cflturb ===== (optional)
    QLabel* schedCflturbLabel = new QLabel(tr("湍流方程 CFL"));
    QHBoxLayout* schedCflturbLayout = new QHBoxLayout();
    nonlinearUI.scheduleCflturb1Spin = new QDoubleSpinBox();
    nonlinearUI.scheduleCflturb2Spin = new QDoubleSpinBox();
    schedCflturbLayout->setAlignment(Qt::AlignVCenter); // 保证整体垂直居中
    nonlinearUI.scheduleCflturb1Spin->setRange(0.0,500);
    nonlinearUI.scheduleCflturb2Spin->setRange(0.0,500);
    schedCflturbLayout->addWidget(nonlinearUI.scheduleCflturb1Spin);
    schedCflturbLayout->addStretch();
    schedCflturbLayout->addWidget(label3);
    schedCflturbLayout->addStretch();
    schedCflturbLayout->addWidget(nonlinearUI.scheduleCflturb2Spin);
    QWidget* schedCflturbWidget = new QWidget();
    schedCflturbWidget->setLayout(schedCflturbLayout);
    form->addRow(schedCflturbLabel);
    form->addRow(schedCflturbWidget);
    //optionalWidgetArray.append(schedCflturbLabel);
    //optionalWidgetArray.append(schedCflturbWidget);

    // ===== f_allow_minimum_m ===== (optional)
    QLabel* fminLabel = new QLabel(tr("允许最小M分数"));
    nonlinearUI.fAllowMinimumMSpin = new QDoubleSpinBox();
    nonlinearUI.fAllowMinimumMSpin->setRange(0.0, 1.0);
    nonlinearUI.fAllowMinimumMSpin->setValue(0.01);
    form->addRow(fminLabel, nonlinearUI.fAllowMinimumMSpin);
    optionalWidgetArray.append(fminLabel);
    optionalWidgetArray.append(nonlinearUI.fAllowMinimumMSpin);

    // ===== invisc_relax_factor ===== (optional)
    QLabel* inviscLabel = new QLabel(tr("Inviscid 松弛因子"));
    nonlinearUI.inviscRelaxFactorSpin = new QDoubleSpinBox();
    nonlinearUI.inviscRelaxFactorSpin->setRange(0.0, 10.0);
    nonlinearUI.inviscRelaxFactorSpin->setValue(1.0);
    form->addRow(inviscLabel, nonlinearUI.inviscRelaxFactorSpin);
    optionalWidgetArray.append(inviscLabel);
    optionalWidgetArray.append(nonlinearUI.inviscRelaxFactorSpin);

    // ===== visc_relax_factor ===== (optional)
    QLabel* viscLabel = new QLabel(tr("Viscous 松弛因子"));
    nonlinearUI.viscRelaxFactorSpin = new QDoubleSpinBox();
    nonlinearUI.viscRelaxFactorSpin->setRange(0.0, 10.0);
    nonlinearUI.viscRelaxFactorSpin->setValue(1.0);
    form->addRow(viscLabel, nonlinearUI.viscRelaxFactorSpin);
    optionalWidgetArray.append(viscLabel);
    optionalWidgetArray.append(nonlinearUI.viscRelaxFactorSpin);

    // ===== main layout =====
    mainLayout->addWidget(nonlinearSolverBox);

    // ===== 隐藏函数 =====
    connect(nonlinearUI.timeAccuracyCombo, &QComboBox::currentTextChanged, this, [=](const QString& mode){
        bool steady = (mode.compare("steady", Qt::CaseInsensitive) == 0);
        nonlinearUI.timeStepNondimSpin->setEnabled(!steady);
        nonlinearUI.subiterationsSpin->setEnabled(!steady);
        nonlinearUI.temporalErrControlCheck->setEnabled(!steady);
        nonlinearUI.temporalErrFloorSpin->setEnabled(!steady && nonlinearUI.temporalErrControlCheck->isChecked());
    });
    connect(nonlinearUI.temporalErrControlCheck, &QCheckBox::toggled, this, [=](bool on){
        nonlinearUI.temporalErrFloorSpin->setEnabled(on);
    });
}

void structWidget::initLinearSolverBox()
{
    linearSolverBox = new QGroupBox(tr("线性求解器参数"));
    //linearSolverBox->setStyleSheet("QGroupBox { font-weight: bold; color: #1E3A5F; }");

    // 表单布局
    QFormLayout* form = new QFormLayout();

    // 主流松弛次数
    QSpinBox* meanflowSpin = new QSpinBox();
    meanflowSpin->setRange(1, 100);
    meanflowSpin->setValue(15);

    // 湍流松弛次数
    QSpinBox* turbSpin = new QSpinBox();
    turbSpin->setRange(1, 100);
    turbSpin->setValue(10);

    // Krylov 投影
    QCheckBox* projectionCheck = new QCheckBox(tr("启用线性投影 (GCR)"));

    // 线隐式选项
    QComboBox* lineImplicitCombo = new QComboBox();
    lineImplicitCombo->addItems({ "off", "on" });

    // 添加到布局
    form->addRow(tr("主流方程松弛次数:"), meanflowSpin);
    form->addRow(tr("湍流方程松弛次数:"), turbSpin);
    form->addRow(tr("线性投影:"), projectionCheck);
    form->addRow(tr("线隐式:"), lineImplicitCombo);

    linearSolverBox->setLayout(form);

    // 保存到结构体
    linearSolverUI.meanflowSweepsSpin = meanflowSpin;
    linearSolverUI.turbSweepsSpin = turbSpin;
    linearSolverUI.projectionCheck = projectionCheck;
    linearSolverUI.lineImplicitCombo = lineImplicitCombo;

    mainLayout->addWidget(linearSolverBox);
}
void structWidget::initGlobalBox()
{
    // 主盒子
    globalBox = new QGroupBox(tr("Global Settings"));
    globalBox->setObjectName("globalBox");
    QVBoxLayout *globalLayout = new QVBoxLayout(globalBox);

    // ===== 移动网格相关 =====
    QLabel *movingGridLabel = new QLabel(tr("Moving Grid Options:"));
    movingGridLabel->setStyleSheet("font-weight:bold;");

    globalUI.movingGridCheck = new QCheckBox(tr("Enable Moving Grid"));
    globalUI.gridMotionOnlyCheck = new QCheckBox(tr("Grid Motion Only"));
    globalUI.gridMotionAndDCICheck = new QCheckBox(tr("Grid Motion + DCI Only"));
    globalUI.bodyMotionOnlyCheck = new QCheckBox(tr("Body Motion Only"));

    QVBoxLayout *gridLayout = new QVBoxLayout();
    gridLayout->addWidget(globalUI.movingGridCheck);
    gridLayout->addWidget(globalUI.gridMotionOnlyCheck);
    gridLayout->addWidget(globalUI.gridMotionAndDCICheck);
    gridLayout->addWidget(globalUI.bodyMotionOnlyCheck);

    // ===== 计时选项 =====
    QLabel *timingLabel = new QLabel(tr("Timing Options:"));
    timingLabel->setStyleSheet("font-weight:bold;");

    globalUI.timingCheck = new QCheckBox(tr("Enable Timing"));
    globalUI.timeMovingGridCheck = new QCheckBox(tr("Time Moving Grid"));

    QVBoxLayout *timingLayout = new QVBoxLayout();
    timingLayout->addWidget(globalUI.timingCheck);
    timingLayout->addWidget(globalUI.timeMovingGridCheck);

    // ===== 输出频率 =====
    QLabel *outputLabel = new QLabel(tr("Visualization Output Frequency:"));
    outputLabel->setStyleSheet("font-weight:bold;");

    globalUI.boundaryAnimSpin = new QSpinBox();
    globalUI.volumeAnimSpin = new QSpinBox();
    globalUI.sliceFreqSpin = new QSpinBox();

    globalUI.boundaryAnimSpin->setRange(-1, 100000);
    globalUI.volumeAnimSpin->setRange(-1, 100000);
    globalUI.sliceFreqSpin->setRange(-1, 100000);

    QLabel *boundaryLabel = new QLabel(tr("Boundary Animation Freq"));
    QLabel *volumeLabel = new QLabel(tr("Volume Animation Freq"));
    QLabel *sliceLabel = new QLabel(tr("Slice Output Freq"));

    globalUI.boundaryAnimSpin->setToolTip(tr("0 = no output, -1 = end of run, N = every N iterations"));
    globalUI.volumeAnimSpin->setToolTip(tr("0 = no output, -1 = end of run, N = every N iterations"));
    globalUI.sliceFreqSpin->setToolTip(tr("0 = no output, -1 = end of run, N = every N iterations"));

    QGridLayout *freqLayout = new QGridLayout();
    freqLayout->addWidget(boundaryLabel, 0, 0);
    freqLayout->addWidget(globalUI.boundaryAnimSpin, 0, 1);
    freqLayout->addWidget(volumeLabel, 1, 0);
    freqLayout->addWidget(globalUI.volumeAnimSpin, 1, 1);
    freqLayout->addWidget(sliceLabel, 2, 0);
    freqLayout->addWidget(globalUI.sliceFreqSpin, 2, 1);

    // ===== 记录命令行 =====
    QLabel *miscLabel = new QLabel(tr("Miscellaneous:"));
    miscLabel->setStyleSheet("font-weight:bold;");
    globalUI.recordCmdCheck = new QCheckBox(tr("Record Command Lines "));

    // ===== 组装布局 =====
    globalLayout->addWidget(movingGridLabel);
    globalLayout->addLayout(gridLayout);
    globalLayout->addSpacing(8);

    globalLayout->addWidget(timingLabel);
    globalLayout->addLayout(timingLayout);
    globalLayout->addSpacing(8);

    globalLayout->addWidget(outputLabel);
    globalLayout->addLayout(freqLayout);
    globalLayout->addSpacing(8);

    globalLayout->addWidget(miscLabel);
    globalLayout->addWidget(globalUI.recordCmdCheck);
    globalLayout->addStretch();

    // ===== 默认值 =====
    globalUI.movingGridCheck->setChecked(false);
    globalUI.gridMotionOnlyCheck->setChecked(false);
    globalUI.gridMotionAndDCICheck->setChecked(false);
    globalUI.bodyMotionOnlyCheck->setChecked(false);
    globalUI.timingCheck->setChecked(false);
    globalUI.timeMovingGridCheck->setChecked(false);
    globalUI.recordCmdCheck->setChecked(false);

    globalUI.boundaryAnimSpin->setValue(0);
    globalUI.volumeAnimSpin->setValue(0);
    globalUI.sliceFreqSpin->setValue(0);

    mainLayout->addWidget(globalBox);


    // ===== 信号槽（防止 timing 与 timeMovingGrid 同时启用） =====
    connect(globalUI.timingCheck, &QCheckBox::toggled, this, [=](bool checked){
        if (checked && globalUI.timeMovingGridCheck->isChecked()) {
            globalUI.timeMovingGridCheck->setChecked(false);
        }
    });
    connect(globalUI.timeMovingGridCheck, &QCheckBox::toggled, this, [=](bool checked){
        if (checked && globalUI.timingCheck->isChecked()) {
            globalUI.timingCheck->setChecked(false);
        }
    });

    // 如果你要把 globalBox 加到主布局里（假设 mainLayout 是你的窗口主布局）
    //mainLayout->addWidget(globalBox);
}
void structWidget::initVolumeOutputBox()
{
    volumeOutputBox = new QGroupBox(tr("体积输出变量"));


    QFormLayout* form = new QFormLayout();

    // === 输出格式 export_to ===
    volumeOutputUI.exportToCombo = new QComboBox();
    volumeOutputUI.exportToCombo->addItems({ "tecplot", "vtk", "hdf5" });
    volumeOutputUI.exportToCombo->setCurrentText("tecplot");

    // === 坐标 ===
    volumeOutputUI.xCheck = new QCheckBox(tr("X 坐标 (x)"));
    volumeOutputUI.yCheck = new QCheckBox(tr("Y 坐标 (y)"));
    volumeOutputUI.zCheck = new QCheckBox(tr("Z 坐标 (z)"));
    volumeOutputUI.xCheck->setChecked(true);
    volumeOutputUI.yCheck->setChecked(true);
    volumeOutputUI.zCheck->setChecked(true);

    // === 基础变量 ===
    volumeOutputUI.primitiveCheck   = new QCheckBox(tr("原始变量"));
    volumeOutputUI.primitiveCheck->setChecked(true);
    volumeOutputUI.rhoCheck         = new QCheckBox(tr("密度 (rho)"));
    volumeOutputUI.uCheck           = new QCheckBox(tr("速度 u"));
    volumeOutputUI.vCheck           = new QCheckBox(tr("速度 v"));
    volumeOutputUI.wCheck           = new QCheckBox(tr("速度 w"));
    volumeOutputUI.pCheck           = new QCheckBox(tr("压力 p"));
    volumeOutputUI.entropyCheck     = new QCheckBox(tr("熵 (entropy)"));
    volumeOutputUI.machCheck        = new QCheckBox(tr("马赫数 (mach)"));
    volumeOutputUI.temperatureCheck = new QCheckBox(tr("温度 (temperature)"));
    volumeOutputUI.iblankCheck      = new QCheckBox(tr("iblank"));
    volumeOutputUI.imeshCheck       = new QCheckBox(tr("imesh"));

    // === 涡量/速度散度 ===
    volumeOutputUI.vortMagCheck     = new QCheckBox(tr("涡量模 (vort_mag)"));
    volumeOutputUI.vortXCheck       = new QCheckBox(tr("vort_x"));
    volumeOutputUI.vortYCheck       = new QCheckBox(tr("vort_y"));
    volumeOutputUI.vortZCheck       = new QCheckBox(tr("vort_z"));
    volumeOutputUI.qCriterionCheck  = new QCheckBox(tr("Q 判据 (q_criterion)"));
    volumeOutputUI.divVelCheck      = new QCheckBox(tr("速度散度 (div_vel)"));

    // === 湍流波动 ===
    volumeOutputUI.turbFluctCheck   = new QCheckBox(tr("湍流脉动 (turbulent_fluctuations)"));
    volumeOutputUI.uuPrimeCheck     = new QCheckBox(tr("uu'"));
    volumeOutputUI.vvPrimeCheck     = new QCheckBox(tr("vv'"));
    volumeOutputUI.wwPrimeCheck     = new QCheckBox(tr("ww'"));

    // === 添加到布局 ===
    form->addRow(tr("输出格式 (export_to):"), volumeOutputUI.exportToCombo);
    form->addRow(volumeOutputUI.xCheck);
    form->addRow(volumeOutputUI.yCheck);
    form->addRow(volumeOutputUI.zCheck);
    form->addRow(volumeOutputUI.primitiveCheck);
    form->addRow(volumeOutputUI.rhoCheck);
    form->addRow(volumeOutputUI.uCheck);
    form->addRow(volumeOutputUI.vCheck);
    form->addRow(volumeOutputUI.wCheck);
    form->addRow(volumeOutputUI.pCheck);
    form->addRow(volumeOutputUI.entropyCheck);
    form->addRow(volumeOutputUI.machCheck);
    form->addRow(volumeOutputUI.temperatureCheck);
    form->addRow(volumeOutputUI.iblankCheck);
    form->addRow(volumeOutputUI.imeshCheck);
    form->addRow(volumeOutputUI.vortMagCheck);
    form->addRow(volumeOutputUI.vortXCheck);
    form->addRow(volumeOutputUI.vortYCheck);
    form->addRow(volumeOutputUI.vortZCheck);
    form->addRow(volumeOutputUI.qCriterionCheck);
    form->addRow(volumeOutputUI.divVelCheck);
    form->addRow(volumeOutputUI.turbFluctCheck);
    form->addRow(volumeOutputUI.uuPrimeCheck);
    form->addRow(volumeOutputUI.vvPrimeCheck);
    form->addRow(volumeOutputUI.wwPrimeCheck);

    volumeOutputBox->setLayout(form);
    mainLayout->addWidget(volumeOutputBox);
}

void structWidget::initBoundaryOutputBox()
{
    boundaryOutputBox = new QGroupBox(tr("边界输出变量"));


    QFormLayout* form = new QFormLayout();

    // === 基本参数 ===
    boundaryOutputUI.numberOfBoundaries = new QSpinBox();
    boundaryOutputUI.numberOfBoundaries->setMinimum(0);
    boundaryOutputUI.numberOfBoundaries->setValue(0);

    boundaryOutputUI.boundaryListEdit = new QLineEdit();
    boundaryOutputUI.boundaryListEdit->setPlaceholderText(tr("例如: '1 3 5' 或留空全部"));

    boundaryOutputUI.exportToCombo = new QComboBox();
    boundaryOutputUI.exportToCombo->addItems({"tecplot_dat", "tecplot_plt"});
    boundaryOutputUI.exportToCombo->setCurrentText("tecplot_plt");

    form->addRow(tr("边界数量:"), boundaryOutputUI.numberOfBoundaries);
    form->addRow(tr("边界列表:"), boundaryOutputUI.boundaryListEdit);
    form->addRow(tr("输出格式:"), boundaryOutputUI.exportToCombo);

    // === 坐标 ===
    boundaryOutputUI.xCheck = new QCheckBox(tr("x 坐标"));
    boundaryOutputUI.yCheck = new QCheckBox(tr("y 坐标"));
    boundaryOutputUI.zCheck = new QCheckBox(tr("z 坐标"));
    boundaryOutputUI.xCheck->setChecked(true);
    boundaryOutputUI.yCheck->setChecked(true);
    boundaryOutputUI.zCheck->setChecked(true);
    form->addRow(boundaryOutputUI.xCheck);
    form->addRow(boundaryOutputUI.yCheck);
    form->addRow(boundaryOutputUI.zCheck);

    // === 基础变量 ===
    boundaryOutputUI.primitiveCheck   = new QCheckBox(tr("原始变量"));
    boundaryOutputUI.primitiveCheck->setChecked(true);
    boundaryOutputUI.rhoCheck         = new QCheckBox(tr("密度 rho"));
    boundaryOutputUI.uCheck           = new QCheckBox(tr("速度 u"));
    boundaryOutputUI.vCheck           = new QCheckBox(tr("速度 v"));
    boundaryOutputUI.wCheck           = new QCheckBox(tr("速度 w"));
    boundaryOutputUI.pCheck           = new QCheckBox(tr("压力 p"));
    boundaryOutputUI.entropyCheck     = new QCheckBox(tr("熵 entropy"));
    boundaryOutputUI.machCheck        = new QCheckBox(tr("马赫数 mach"));
    boundaryOutputUI.temperatureCheck = new QCheckBox(tr("温度 temperature"));
    boundaryOutputUI.iblankCheck      = new QCheckBox(tr("iblank"));
    boundaryOutputUI.imeshCheck       = new QCheckBox(tr("imesh"));

    form->addRow(boundaryOutputUI.primitiveCheck);
    form->addRow(boundaryOutputUI.rhoCheck);
    form->addRow(boundaryOutputUI.uCheck);
    form->addRow(boundaryOutputUI.vCheck);
    form->addRow(boundaryOutputUI.wCheck);
    form->addRow(boundaryOutputUI.pCheck);
    form->addRow(boundaryOutputUI.entropyCheck);
    form->addRow(boundaryOutputUI.machCheck);
    form->addRow(boundaryOutputUI.temperatureCheck);
    form->addRow(boundaryOutputUI.iblankCheck);
    form->addRow(boundaryOutputUI.imeshCheck);

    // === 涡量/散度 ===
    boundaryOutputUI.vortMagCheck    = new QCheckBox(tr("涡量模 vort_mag"));
    boundaryOutputUI.vortXCheck      = new QCheckBox(tr("vort_x"));
    boundaryOutputUI.vortYCheck      = new QCheckBox(tr("vort_y"));
    boundaryOutputUI.vortZCheck      = new QCheckBox(tr("vort_z"));
    boundaryOutputUI.qCriterionCheck = new QCheckBox(tr("Q 判据 q_criterion"));
    boundaryOutputUI.divVelCheck     = new QCheckBox(tr("速度散度 div_vel"));
    form->addRow(boundaryOutputUI.vortMagCheck);
    form->addRow(boundaryOutputUI.vortXCheck);
    form->addRow(boundaryOutputUI.vortYCheck);
    form->addRow(boundaryOutputUI.vortZCheck);
    form->addRow(boundaryOutputUI.qCriterionCheck);
    form->addRow(boundaryOutputUI.divVelCheck);

    // === 湍流相关 ===
    boundaryOutputUI.turbFluctCheck = new QCheckBox(tr("湍流脉动"));
    boundaryOutputUI.uuPrimeCheck   = new QCheckBox(tr("uu'"));
    boundaryOutputUI.vvPrimeCheck   = new QCheckBox(tr("vv'"));
    boundaryOutputUI.wwPrimeCheck   = new QCheckBox(tr("ww'"));
    boundaryOutputUI.uvPrimeCheck   = new QCheckBox(tr("uv'"));
    boundaryOutputUI.uwPrimeCheck   = new QCheckBox(tr("uw'"));
    boundaryOutputUI.vwPrimeCheck   = new QCheckBox(tr("vw'"));
    boundaryOutputUI.cpCheck        = new QCheckBox(tr("压力系数 Cp"));

    form->addRow(boundaryOutputUI.turbFluctCheck);
    form->addRow(boundaryOutputUI.uuPrimeCheck);
    form->addRow(boundaryOutputUI.vvPrimeCheck);
    form->addRow(boundaryOutputUI.wwPrimeCheck);
    form->addRow(boundaryOutputUI.uvPrimeCheck);
    form->addRow(boundaryOutputUI.uwPrimeCheck);
    form->addRow(boundaryOutputUI.vwPrimeCheck);
    form->addRow(boundaryOutputUI.cpCheck);

    boundaryOutputBox->setLayout(form);
    mainLayout->addWidget(boundaryOutputBox);
}
void structWidget::initCloudPlotBox(){
    QStringList imagePaths = {
        ":/icons/colormaps/0.png",
        ":/icons/colormaps/1.png",
        ":/icons/colormaps/2.png",
        ":/icons/colormaps/3.png",
        ":/icons/colormaps/4.png"
    };

    cloudPlotBox = new QGroupBox(tr("Contour Plot"));
    QFormLayout* form = new QFormLayout();
    QHBoxLayout *layout = new QHBoxLayout();
    cloudPlotUI.variableCombo = new QComboBox();
    cloudPlotUI.minEdit = new QLineEdit();
    cloudPlotUI.maxEdit = new QLineEdit();
    cloudPlotUI.numsEdit = new QLineEdit();
    cloudPlotUI.colorCombo = new QComboBox();
    for (const QString& path : imagePaths) {
        QIcon icon(path);
        cloudPlotUI.colorCombo->addItem(icon, ""); // 第二个参数留空隐藏文字
    }
    cloudPlotUI.colorCombo->setEditable(true);
    cloudPlotUI.colorCombo->lineEdit()->setVisible(false); // 隐藏编辑框
    cloudPlotUI.colorCombo->setIconSize(QSize(250, 25));
    cloudPlotUI.colorCombo->setSizeAdjustPolicy(QComboBox::AdjustToContents);


    //cloudPlotUI.boundaryList = new QListWidget();
    cloudPlotUI.applyButton = new QPushButton(tr("显示"));
    cloudPlotUI.cancelButton = new QPushButton(tr("取消"));
    layout->addWidget(cloudPlotUI.applyButton);
    layout->addWidget(cloudPlotUI.cancelButton);
    form->addRow(tr("变量"),cloudPlotUI.variableCombo);
    form->addRow(new QLabel(tr("设置范围")));
    form->addRow(tr("最小值"),cloudPlotUI.minEdit);
    form->addRow(tr("最大值"),cloudPlotUI.maxEdit);
    form->addRow(tr("数量"),cloudPlotUI.numsEdit);
    form->addRow(tr("颜色"),cloudPlotUI.colorCombo);
    //form->addRow(new QLabel(tr("边界")));
    //form->addRow(cloudPlotUI.boundaryList);
    form->addRow(layout);
    cloudPlotBox->setLayout(form);
    mainLayout->addWidget(cloudPlotBox);

}
void structWidget::setCloudPlotEdits(double minVal, double maxVal, int numLabels)
{

    cloudPlotUI.minEdit->setText(QString::number(minVal, 'f', 3)); // 保留3位小数
    cloudPlotUI.maxEdit->setText(QString::number(maxVal, 'f', 3));
    cloudPlotUI.numsEdit->setText(QString::number(numLabels));
}




/* ---------------------- 数据导出 ---------------------- */
TurbulentDiffusionModelsData structWidget::getTurbulentDiffusionModelsData() const
{
    TurbulentDiffusionModelsData data;

    data.turbulenceModel = flowUI.turbGroup->checkedButton()
        ? normalizeQuoted(flowUI.turbGroup->checkedButton()->text())
        : "\"\"";

    data.turbIntensity = flowUI.turbIntensitySpin->value();
    data.turbMuRatio = flowUI.turbMuRatioSpin->value();


    data.turbCompress = normalizeQuoted(flowUI.turbCompressCombo->currentText());
    data.turbCond = normalizeQuoted(flowUI.turbCondCombo->currentText());

    data.pr = flowUI.prSpin->value();
    data.sc = flowUI.scSpin->value();

    return data;
}


RawGridData structWidget::getRawGridData() const {
    RawGridData data;

    // 所有字符串字段加上双引号
    data.gridFormat = normalizeQuoted(rawUI.gridFormatCombo->currentText());
    data.dataFormat = normalizeQuoted(rawUI.dataFormatCombo->currentText());

    // 布尔量仍使用 .true. / .false.
    data.twodMode = rawUI.twodModeCheck->isChecked() ? ".true." : ".false.";
    data.swapYZAxes = rawUI.swapYZAxesCheck->isChecked() ? ".true." : ".false.";

    data.fieldviewPrecision = normalizeQuoted(rawUI.fieldviewPrecisionCombo->currentText());
    data.patchLumping = normalizeQuoted(rawUI.patchLumpingCombo->currentText());
    data.ignoreEulerNumber = rawUI.ignoreEulerCheck->isChecked() ? ".true." : ".false.";

    return data;
}


ReferenceData structWidget::getReferenceData() const
{
    ReferenceData d;

    // 维度类型：nondimensional / dimensional-SI
    d.dimType = refUI.nondimBtn->isChecked()
        ? "\"nondimensional\""
        : "\"dimensional-SI\"";

    // 数值类型直接取浮点或整数值
    d.gridlengthConversion = refUI.gridLengthConversionSpin->value();
    d.mach = refUI.machSpin->value();
    d.reynolds = refUI.reynoldsSpin->value();
    d.velocity = refUI.velocitySpin->value();
    d.density = refUI.densitySpin->value();
    d.temperature = refUI.tempSpin->value();

    // 单位字符串也加双引号
    d.tempUnit = normalizeQuoted(refUI.tempUnitCombo->currentText());

    // 攻角、偏航角
    d.alpha = refUI.alphaSpin->value();
    d.yaw = refUI.yawSpin->value();

    return d;
}

EqnData structWidget::getEqnData() const
{
    EqnData data;

    // 所有 QString 类型字段都加上双引号
    data.eqnType = normalizeQuoted(eqnUI.eqnTypeCombo->currentText());
    data.artificialCompress = eqnUI.artificialCompressSpin->value();
    data.viscousTerms = normalizeQuoted(eqnUI.viscousTermsCombo->currentText());
    data.chemicalKinetics = normalizeQuoted(eqnUI.chemKineticsCombo->currentText());
    data.thermalEnergyModel = normalizeQuoted(eqnUI.thermalModelCombo->currentText());
    data.prandtlNumberMolecular = eqnUI.prandtlSpin->value();
    data.schmidtNumber = eqnUI.schmidtSpin->value();
    data.gasRadiation = normalizeQuoted(eqnUI.gasRadiationCombo->currentText());

    // bool 型逻辑字段仍然用 .true. / .false.
    data.radUseImplLines = eqnUI.radUseImplCheck->isChecked() ? ".true." : ".false.";
    data.multiComponentDiff = eqnUI.multiDiffCheck->isChecked() ? ".true." : ".false.";
    //data.cpivMinFactor = eqnUI.cpivMinSpin->value();
    //data.augmentKineticsLimiting = eqnUI.augmentKineticsCheck->isChecked() ? ".true." : ".false.";
    //data.implicitRateLimiting = eqnUI.implicitRateCheck->isChecked() ? ".true." : ".false.";

    return data;
}

ForceMomentData structWidget::getForceMomentData() const
{
    ForceMomentData data;
    data.areaReference   = fmUI.areaReferenceSpin->value();
    data.xMomentLength   = fmUI.xMomentLengthSpin->value();
    data.yMomentLength   = fmUI.yMomentLengthSpin->value();
    data.xMomentCenter   = fmUI.xMomentCenterSpin->value();
    data.yMomentCenter   = fmUI.yMomentCenterSpin->value();
    data.zMomentCenter   = fmUI.zMomentCenterSpin->value();
    return data;
}
InviscidFluxData structWidget::getInviscidFluxData() const {
    InviscidFluxData d;

    // === 字符串字段加双引号 ===
    d.fluxConstruction    = normalizeQuoted(fluxUI.fluxConsCombo->currentText());
    d.fluxConstructionLHS = normalizeQuoted(fluxUI.fluxConsLhsCombo->currentText());
    d.fluxLimiter         = normalizeQuoted(fluxUI.fluxLimiterCombo->currentText());

    // === 数值字段保持原样 ===
    d.kappaUmuscl         = fluxUI.kappaSpin->value();
    d.firstOrderIterations = fluxUI.firstOrderSpin->value();
    d.multidmOption       = fluxUI.multiOptionCombo->currentText().toInt();
    d.recalcDirFreq       = fluxUI.recalcSpin->value();
    d.rhsUEigenCoef       = fluxUI.rhsUEigenSpin->value();
    d.lhsUEigenCoef       = fluxUI.lhsUEigenSpin->value();
    d.rhsAEigenCoef       = fluxUI.rhsAEigenSpin->value();
    d.lhsAEigenCoef       = fluxUI.lhsAEigenSpin->value();
    d.reMinVswch          = fluxUI.reMinSpin->value();
    d.reMaxVswch          = fluxUI.reMaxSpin->value();

    // === 布尔字段保持 .true. / .false. 格式 ===
    d.fixedDirection      = fluxUI.fixedDirCheck->isChecked() ? ".true." : ".false.";
    d.adaptiveEntropyFix  = fluxUI.adaptEntropyCheck->isChecked() ? ".true." : ".false.";
    d.entropyFix          = fluxUI.entropyFixCheck->isChecked() ? ".true." : ".false.";

    return d;
}

GridGenData structWidget::getGridGenData() const
{
    GridGenData data;
    data.nsrf = gridUI.nsrfSpin->value();
    data.wake = gridUI.wakeSpin->value();
    data.jmax = gridUI.jMaxSpin->value();
    data.lesp = gridUI.lespSpin->value();
    data.tesp = gridUI.tespSpin->value();
    data.radi = gridUI.radiSpin->value();
    data.re = gridUI.reSpin->value();
    data.yPlus = gridUI.yPlusSpin->value();
    data.genType = gridUI.genTypeBox->currentIndex();

    return  data;
}


SpalartData structWidget::getSpalartData() const
{
    SpalartData data;

    // 数值字段
    data.turbInf = spalartUI.turbInfSpin->value();

    // 逻辑字段
    data.daclesMariani = spalartUI.daclesCheck->isChecked() ? ".true." : ".false.";
    data.sarc          = spalartUI.sarcCheck->isChecked() ? ".true." : ".false.";
    data.ddes          = spalartUI.ddesCheck->isChecked() ? ".true." : ".false.";

    return data;
}

CodeRunData structWidget::getCodeRunData() const {
    CodeRunData d;

    d.steps = codeRunUI.stepsSpin->value();
    d.stoppingTolerance = codeRunUI.stopTolEdit->text().toDouble();
    d.durationLimit = codeRunUI.durationLimitSpin->value();

    d.noRestart = codeRunUI.noRestartCheck->isChecked() ? ".true." : ".false.";
    d.restartWriteFreq = codeRunUI.restartWriteFreqSpin->value();

    // ✅ 字符串字段需要加引号
    d.restartRead = normalizeQuoted(codeRunUI.restartReadCombo->currentText());

    d.smartJupdate = codeRunUI.smartJupdateCheck->isChecked() ? ".true." : ".false.";
    d.jacobianEvalFreq = codeRunUI.jacobianEvalFreqSpin->value();
    d.jupdateStartupSteps = codeRunUI.jupdateStartupStepsSpin->value();
    d.dfduc3Jacobians = codeRunUI.dfduc3Check->isChecked() ? ".true." : ".false.";
    d.alphaSweep = codeRunUI.alphaSweepCheck->isChecked() ? ".true." : ".false.";

    d.cycleIncrement = codeRunUI.cycleIncrementSpin->value();
    d.alphaIncrement = codeRunUI.alphaIncrementSpin->value();
    d.alphaMax = codeRunUI.alphaMaxSpin->value();
    d.alphaMin = codeRunUI.alphaMinSpin->value();
    d.alphaSwitchbacks = codeRunUI.alphaSwitchbacksSpin->value();

    return d;
}

NonlinearSolverData structWidget::getNonlinearSolverData() const
{
    NonlinearSolverData data;

    // === 基本参数 ===
    data.timeAccuracy = normalizeQuoted(nonlinearUI.timeAccuracyCombo->currentText());
    data.pseudoTimeStepping = normalizeQuoted(nonlinearUI.pseudoTimeSteppingCombo->currentText());
    data.timeStepNondim = nonlinearUI.timeStepNondimSpin->value();
    data.subiterations = nonlinearUI.subiterationsSpin->value();
    data.temporalErrControl = nonlinearUI.temporalErrControlCheck->isChecked() ? ".true." : ".false.";
    data.temporalErrFloor = nonlinearUI.temporalErrFloorSpin->value();

    // === 调度参数 ===
    data.scheduleIteration1 = nonlinearUI.scheduleIteration1Spin->value();
    data.scheduleIteration2 = nonlinearUI.scheduleIteration2Spin->value();
    data.scheduleCfl1 = nonlinearUI.scheduleCfl1Spin->value();
    data.scheduleCfl2 = nonlinearUI.scheduleCfl2Spin->value();
    data.scheduleCflturb1 = nonlinearUI.scheduleCflturb1Spin->value();
    data.scheduleCflturb2 = nonlinearUI.scheduleCflturb2Spin->value();

    // === 松弛与限制参数 ===
    data.fAllowMinimumM = nonlinearUI.fAllowMinimumMSpin->value();
    data.inviscRelaxFactor = nonlinearUI.inviscRelaxFactorSpin->value();
    data.viscRelaxFactor = nonlinearUI.viscRelaxFactorSpin->value();

    return data;
}


LinearSolverData structWidget::getLinearSolverData() const
{
    LinearSolverData params;
    params.meanflowSweeps = linearSolverUI.meanflowSweepsSpin->value();
    params.turbulenceSweeps = linearSolverUI.turbSweepsSpin->value();
    params.linearProjection = linearSolverUI.projectionCheck->isChecked() ? ".true." : ".false.";
    params.lineImplicit = normalizeQuoted(linearSolverUI.lineImplicitCombo->currentText());
    return params;
}

GlobalData structWidget::getGlobalData() const
{
    GlobalData data;


    // 🟢 移动网格
    data.movingGrid = globalUI.movingGridCheck->isChecked() ? ".true." : ".false.";
    data.gridMotionOnly = globalUI.gridMotionOnlyCheck->isChecked() ? ".true." : ".false.";
    data.gridMotionAndDCI = globalUI.gridMotionAndDCICheck->isChecked() ? ".true." : ".false.";
    data.bodyMotionOnly = globalUI.bodyMotionOnlyCheck->isChecked() ? ".true." : ".false.";

    // 🟢 计时选项
    data.timing = globalUI.timingCheck->isChecked() ? ".true." : ".false.";
    data.timeMovingGrid = globalUI.timeMovingGridCheck->isChecked() ? ".true." : ".false.";

    // 🟢 输出频率
    data.boundaryAnimFreq = globalUI.boundaryAnimSpin->value();
    data.volumeAnimFreq = globalUI.volumeAnimSpin->value();
    data.sliceFreq = globalUI.sliceFreqSpin->value();

    // 🟢 其他
    data.recordCmd = globalUI.recordCmdCheck->isChecked() ? ".true." : ".false.";

    return data;
}
VolumeOutputData structWidget::getVolumeOutputData() const
{
    VolumeOutputData data;

    // === 输出格式 ===
    data.exportTo = normalizeQuoted(volumeOutputUI.exportToCombo->currentText());


    // === 坐标 ===
    data.x = volumeOutputUI.xCheck->isChecked() ? ".true." : ".false.";
    data.y = volumeOutputUI.yCheck->isChecked() ? ".true." : ".false.";
    data.z = volumeOutputUI.zCheck->isChecked() ? ".true." : ".false.";

    // === 基础变量 ===
    data.primitive   = volumeOutputUI.primitiveCheck->isChecked() ? ".true." : ".false.";
    data.rho         = volumeOutputUI.rhoCheck->isChecked() ? ".true." : ".false.";
    data.u           = volumeOutputUI.uCheck->isChecked() ? ".true." : ".false.";
    data.v           = volumeOutputUI.vCheck->isChecked() ? ".true." : ".false.";
    data.w           = volumeOutputUI.wCheck->isChecked() ? ".true." : ".false.";
    data.p           = volumeOutputUI.pCheck->isChecked() ? ".true." : ".false.";
    data.entropy     = volumeOutputUI.entropyCheck->isChecked() ? ".true." : ".false.";
    data.mach        = volumeOutputUI.machCheck->isChecked() ? ".true." : ".false.";
    data.temperature = volumeOutputUI.temperatureCheck->isChecked() ? ".true." : ".false.";
    data.iblank      = volumeOutputUI.iblankCheck->isChecked() ? ".true." : ".false.";
    data.imesh       = volumeOutputUI.imeshCheck->isChecked() ? ".true." : ".false.";

    // === 涡量 / 散度 ===
    data.vortMag     = volumeOutputUI.vortMagCheck->isChecked() ? ".true." : ".false.";
    data.vortX       = volumeOutputUI.vortXCheck->isChecked() ? ".true." : ".false.";
    data.vortY       = volumeOutputUI.vortYCheck->isChecked() ? ".true." : ".false.";
    data.vortZ       = volumeOutputUI.vortZCheck->isChecked() ? ".true." : ".false.";
    data.qCriterion  = volumeOutputUI.qCriterionCheck->isChecked() ? ".true." : ".false.";
    data.divVel      = volumeOutputUI.divVelCheck->isChecked() ? ".true." : ".false.";

    // === 湍流脉动 ===
    data.turbFluct   = volumeOutputUI.turbFluctCheck->isChecked() ? ".true." : ".false.";
    data.uuPrime     = volumeOutputUI.uuPrimeCheck->isChecked() ? ".true." : ".false.";
    data.vvPrime     = volumeOutputUI.vvPrimeCheck->isChecked() ? ".true." : ".false.";
    data.wwPrime     = volumeOutputUI.wwPrimeCheck->isChecked() ? ".true." : ".false.";

    return data;
}
BoundaryOutputData structWidget::getBoundaryOutputData() const
{
    BoundaryOutputData d;

    // 基本参数
    d.numberOfBoundaries = boundaryOutputUI.numberOfBoundaries->value();
    d.boundaryList = normalizeQuoted(boundaryOutputUI.boundaryListEdit->text());
    d.exportTo = normalizeQuoted(boundaryOutputUI.exportToCombo->currentText());


    // 坐标
    d.x = boundaryOutputUI.xCheck->isChecked() ? ".true." : ".false.";
    d.y = boundaryOutputUI.yCheck->isChecked() ? ".true." : ".false.";
    d.z = boundaryOutputUI.zCheck->isChecked() ? ".true." : ".false.";

    // 基础变量
    d.primitive   = boundaryOutputUI.primitiveCheck->isChecked() ? ".true." : ".false.";
    d.rho         = boundaryOutputUI.rhoCheck->isChecked() ? ".true." : ".false.";
    d.u           = boundaryOutputUI.uCheck->isChecked() ? ".true." : ".false.";
    d.v           = boundaryOutputUI.vCheck->isChecked() ? ".true." : ".false.";
    d.w           = boundaryOutputUI.wCheck->isChecked() ? ".true." : ".false.";
    d.p           = boundaryOutputUI.pCheck->isChecked() ? ".true." : ".false.";
    d.entropy     = boundaryOutputUI.entropyCheck->isChecked() ? ".true." : ".false.";
    d.mach        = boundaryOutputUI.machCheck->isChecked() ? ".true." : ".false.";
    d.temperature = boundaryOutputUI.temperatureCheck->isChecked() ? ".true." : ".false.";
    d.iblank      = boundaryOutputUI.iblankCheck->isChecked() ? ".true." : ".false.";
    d.imesh       = boundaryOutputUI.imeshCheck->isChecked() ? ".true." : ".false.";

    // 涡量/散度
    d.vortMag     = boundaryOutputUI.vortMagCheck->isChecked() ? ".true." : ".false.";
    d.vortX       = boundaryOutputUI.vortXCheck->isChecked() ? ".true." : ".false.";
    d.vortY       = boundaryOutputUI.vortYCheck->isChecked() ? ".true." : ".false.";
    d.vortZ       = boundaryOutputUI.vortZCheck->isChecked() ? ".true." : ".false.";
    d.qCriterion  = boundaryOutputUI.qCriterionCheck->isChecked() ? ".true." : ".false.";
    d.divVel      = boundaryOutputUI.divVelCheck->isChecked() ? ".true." : ".false.";

    // 湍流相关
    d.turbFluct   = boundaryOutputUI.turbFluctCheck->isChecked() ? ".true." : ".false.";
    d.uuPrime     = boundaryOutputUI.uuPrimeCheck->isChecked() ? ".true." : ".false.";
    d.vvPrime     = boundaryOutputUI.vvPrimeCheck->isChecked() ? ".true." : ".false.";
    d.wwPrime     = boundaryOutputUI.wwPrimeCheck->isChecked() ? ".true." : ".false.";
    d.uvPrime     = boundaryOutputUI.uvPrimeCheck->isChecked() ? ".true." : ".false.";
    d.uwPrime     = boundaryOutputUI.uwPrimeCheck->isChecked() ? ".true." : ".false.";
    d.vwPrime     = boundaryOutputUI.vwPrimeCheck->isChecked() ? ".true." : ".false.";
    d.cp          = boundaryOutputUI.cpCheck->isChecked() ? ".true." : ".false.";

    return d;
}



void structWidget::hideOptionalWidget()
{
    for (QWidget* widget : std::as_const(optionalWidgetArray)) {
        if (widget)
            widget->setVisible(false);

    }
}
void structWidget::showOptionalWidget()
{
    for (QWidget* widget : std::as_const(optionalWidgetArray)) {
        if (widget)
            widget->setVisible(true);

    }
}
void structWidget::setWidgetObjectName(){
    //RawGrid
    rawUI.gridFormatCombo->setObjectName("grid_format");
    rawUI.dataFormatCombo->setObjectName("data_format");
    rawUI.twodModeCheck->setObjectName("twod_mode");
    rawUI.swapYZAxesCheck->setObjectName("swap_yz_axes");
    rawUI.fieldviewPrecisionCombo->setObjectName("fieldview_coordinate_precision");
    rawUI.patchLumpingCombo->setObjectName("patch_lumping");
    rawUI.ignoreEulerCheck->setObjectName("ignore_euler_number");

    //force moment integ properties
    fmUI.areaReferenceSpin->setObjectName("area_reference");
    fmUI.xMomentLengthSpin->setObjectName("x_moment_length");
    fmUI.yMomentLengthSpin->setObjectName("y_moment_length");
    fmUI.xMomentCenterSpin->setObjectName("x_moment_center");
    fmUI.yMomentCenterSpin->setObjectName("y_moment_center");
    fmUI.zMomentCenterSpin->setObjectName("z_moment_center");

    //governing equations
    eqnUI.eqnTypeCombo->setObjectName("eqn_type");
    eqnUI.artificialCompressSpin->setObjectName("artificial_compress");
    eqnUI.viscousTermsCombo->setObjectName("viscous_terms");
    eqnUI.chemKineticsCombo->setObjectName("chemical_kinetics");
    eqnUI.thermalModelCombo->setObjectName("thermal_energy_model");
    eqnUI.prandtlSpin->setObjectName("prandtlnumber_molecular");
    eqnUI.schmidtSpin->setObjectName("schmidt_number");
    eqnUI.gasRadiationCombo->setObjectName("gas_radiation");
    eqnUI.radUseImplCheck->setObjectName("rad_use_impl_lines");
    eqnUI.multiDiffCheck->setObjectName("multi_component_diff");
    //eqnUI.cpivMinSpin->setObjectName("cpiv_min_factor");
    //eqnUI.augmentKineticsCheck->setObjectName("augment_kinetics_limiting");
    //eqnUI.implicitRateCheck->setObjectName("implicit_rate_limiting");

    //reference_physical_properties
    refUI.dimGroup->setObjectName("dim_input_type_group");  // 按钮组关联到dim_input_type参数
    refUI.nondimBtn->setObjectName("dim_input_type_nondimensional");  // 对应'dimensional'选项
    refUI.dimSIBtn->setObjectName("dim_input_type_dimensional");      // 对应'nondimensional'选项
    refUI.gridLengthConversionSpin->setObjectName("gridlength_conversion");
    refUI.machSpin->setObjectName("mach_number");
    refUI.reynoldsSpin->setObjectName("reynolds_number");
    refUI.velocitySpin->setObjectName("velocity");
    refUI.densitySpin->setObjectName("density");
    refUI.tempSpin->setObjectName("temperature");
    refUI.tempUnitCombo->setObjectName("temperature_units");
    refUI.alphaSpin->setObjectName("angle_of_attack");
    refUI.yawSpin->setObjectName("angle_of_yaw");

    //inviscid ﬂux method
    fluxUI.fluxConsCombo->setObjectName("flux_construction");
    fluxUI.fluxConsLhsCombo->setObjectName("flux_construction_lhs");
    fluxUI.kappaSpin->setObjectName("kappa_umuscl");
    fluxUI.fluxLimiterCombo->setObjectName("flux_limiter");
    fluxUI.firstOrderSpin->setObjectName("first_order_iterations");
    fluxUI.multiOptionCombo->setObjectName("multidm_option");
    fluxUI.fixedDirCheck->setObjectName("fixed_direction");
    fluxUI.recalcSpin->setObjectName("recalc_dir_freq");
    fluxUI.adaptEntropyCheck->setObjectName("adptv_entropy_fix");  // 对应adptv_entropy_fix参数
    fluxUI.entropyFixCheck->setObjectName("entropy_fix");          // 对应entropy_fix参数
    fluxUI.rhsUEigenSpin->setObjectName("rhs_u_eigenvalue_coef");
    fluxUI.lhsUEigenSpin->setObjectName("lhs_u_eigenvalue_coef");
    fluxUI.rhsAEigenSpin->setObjectName("rhs_a_eigenvalue_coef");
    fluxUI.lhsAEigenSpin->setObjectName("lhs_a_eigenvalue_coef");
    fluxUI.reMinSpin->setObjectName("re_min_vswch");
    fluxUI.reMaxSpin->setObjectName("re_max_vswch");

    //turbulent diﬀusion models

    flowUI.turbGroup->setObjectName("turbulence_model_group");  // 关联turbulence_model参数的按钮组
    flowUI.turbGroup->buttons().at(0)->setObjectName("sa");
    flowUI.turbGroup->buttons().at(1)->setObjectName("des");
    flowUI.turbGroup->buttons().at(2)->setObjectName("menter-sst");
    flowUI.turbGroup->buttons().at(3)->setObjectName("sst");
    flowUI.turbGroup->buttons().at(4)->setObjectName("kw-des");
    flowUI.turbGroup->buttons().at(5)->setObjectName("hrles");



    flowUI.turbIntensitySpin->setObjectName("turb_intensity");
    flowUI.turbMuRatioSpin->setObjectName("turb_viscosity_ratio");
    flowUI.turbCompressCombo->setObjectName("turb_compress_model");
    flowUI.turbCondCombo->setObjectName("turb_conductivity_model");
    flowUI.prSpin->setObjectName("prandtlnumber_turbulent");
    flowUI.scSpin->setObjectName("schmidtnumber_turbulent");

    //spalart
    spalartUI.turbInfSpin->setObjectName("turbinf");
    spalartUI.daclesCheck->setObjectName("dacles_mariani");
    spalartUI.sarcCheck->setObjectName("sarc");
    spalartUI.ddesCheck->setObjectName("ddes");

    //code run control
    codeRunUI.stepsSpin->setObjectName("steps");
    codeRunUI.stopTolEdit->setObjectName("stopping_tolerance");
    codeRunUI.durationLimitSpin->setObjectName("duration_limit_in_minutes");
    codeRunUI.noRestartCheck->setObjectName("no_restart");
    codeRunUI.restartWriteFreqSpin->setObjectName("restart_write_freq");
    codeRunUI.restartReadCombo->setObjectName("restart_read");
    codeRunUI.smartJupdateCheck->setObjectName("smart_jupdate");
    codeRunUI.jacobianEvalFreqSpin->setObjectName("jacobian_eval_freq");
    codeRunUI.jupdateStartupStepsSpin->setObjectName("jupdate_startup_steps");
    codeRunUI.dfduc3Check->setObjectName("dfduc3_jacobians");
    codeRunUI.alphaSweepCheck->setObjectName("alpha_sweep");
    codeRunUI.cycleIncrementSpin->setObjectName("cycle_increment");
    codeRunUI.alphaIncrementSpin->setObjectName("alpha_increment");
    codeRunUI.alphaMaxSpin->setObjectName("alpha_max");
    codeRunUI.alphaMinSpin->setObjectName("alpha_min");
    codeRunUI.alphaSwitchbacksSpin->setObjectName("alpha_switchbacks");

    //nonlinear solver parameters
    nonlinearUI.timeAccuracyCombo->setObjectName("time_accuracy");
    nonlinearUI.pseudoTimeSteppingCombo->setObjectName("pseudo_time_stepping");  // 补充参数名逻辑（原参数列表未明确，按控件功能推断）
    nonlinearUI.timeStepNondimSpin->setObjectName("time_step_nondim");
    nonlinearUI.subiterationsSpin->setObjectName("subiterations");
    nonlinearUI.temporalErrControlCheck->setObjectName("temporal_err_control");
    nonlinearUI.temporalErrFloorSpin->setObjectName("temporal_err_floor");
    nonlinearUI.scheduleNumberSpin->setObjectName("schedule_number");
    nonlinearUI.scheduleIteration1Spin->setObjectName("schedule_iteration_1");  // 对应数组第一个元素
    nonlinearUI.scheduleIteration2Spin->setObjectName("schedule_iteration_2");  // 对应数组第二个元素
    nonlinearUI.scheduleCfl1Spin->setObjectName("schedule_cfl_1");              // 对应数组第一个元素
    nonlinearUI.scheduleCfl2Spin->setObjectName("schedule_cfl_2");              // 对应数组第二个元素
    nonlinearUI.scheduleCflturb1Spin->setObjectName("schedule_cflturb_1");      // 对应数组第一个元素
    nonlinearUI.scheduleCflturb2Spin->setObjectName("schedule_cflturb_2");      // 对应数组第二个元素
    nonlinearUI.fAllowMinimumMSpin->setObjectName("f_allow_minimum_m");
    nonlinearUI.inviscRelaxFactorSpin->setObjectName("invis_relax_factor");
    nonlinearUI.viscRelaxFactorSpin->setObjectName("visc_relax_factor");

    //linear_solver_parameters
    linearSolverUI.meanflowSweepsSpin->setObjectName("meanflow_sweeps");
    linearSolverUI.turbSweepsSpin->setObjectName("turbulence_sweeps");
    linearSolverUI.projectionCheck->setObjectName("linear_projection");
    linearSolverUI.lineImplicitCombo->setObjectName("line_implicit");

    //global
    globalUI.movingGridCheck->setObjectName("moving_grid");
    globalUI.gridMotionOnlyCheck->setObjectName("grid_motion_only");
    globalUI.gridMotionAndDCICheck->setObjectName("grid_motion_and_dci_only");
    globalUI.bodyMotionOnlyCheck->setObjectName("body_motion_only");

    globalUI.timingCheck->setObjectName("timing");
    globalUI.timeMovingGridCheck->setObjectName("time_moving_grid");

    globalUI.boundaryAnimSpin->setObjectName("boundary_animation_freq");
    globalUI.volumeAnimSpin->setObjectName("volume_animation_freq");
    globalUI.sliceFreqSpin->setObjectName("slice_freq");

    globalUI.recordCmdCheck->setObjectName("record_command_lines");

    //volume output variables
    volumeOutputUI.exportToCombo->setObjectName("export_to");

    volumeOutputUI.xCheck->setObjectName("x");
    volumeOutputUI.yCheck->setObjectName("y");
    volumeOutputUI.zCheck->setObjectName("z");

    volumeOutputUI.primitiveCheck->setObjectName("primitive_variables");
    volumeOutputUI.rhoCheck->setObjectName("rho");
    volumeOutputUI.uCheck->setObjectName("u");
    volumeOutputUI.vCheck->setObjectName("v");
    volumeOutputUI.wCheck->setObjectName("w");
    volumeOutputUI.pCheck->setObjectName("p");
    volumeOutputUI.entropyCheck->setObjectName("entropy");
    volumeOutputUI.machCheck->setObjectName("mach");
    volumeOutputUI.temperatureCheck->setObjectName("temperature");
    volumeOutputUI.iblankCheck->setObjectName("iblank");
    volumeOutputUI.imeshCheck->setObjectName("imesh");

    volumeOutputUI.vortMagCheck->setObjectName("vort_mag");
    volumeOutputUI.vortXCheck->setObjectName("vort_x");
    volumeOutputUI.vortYCheck->setObjectName("vort_y");
    volumeOutputUI.vortZCheck->setObjectName("vort_z");
    volumeOutputUI.qCriterionCheck->setObjectName("q_criterion");
    volumeOutputUI.divVelCheck->setObjectName("div_vel");

    volumeOutputUI.turbFluctCheck->setObjectName("turbulent_fluctuations");
    volumeOutputUI.uuPrimeCheck->setObjectName("uuprime");
    volumeOutputUI.vvPrimeCheck->setObjectName("vvprime");
    volumeOutputUI.wwPrimeCheck->setObjectName("wwprime");

    //boundary output variables
    boundaryOutputUI.numberOfBoundaries->setObjectName("number_of_boundaries");
    boundaryOutputUI.boundaryListEdit->setObjectName("boundary_list");
    boundaryOutputUI.exportToCombo->setObjectName("export_to");

    boundaryOutputUI.xCheck->setObjectName("x");
    boundaryOutputUI.yCheck->setObjectName("y");
    boundaryOutputUI.zCheck->setObjectName("z");

    boundaryOutputUI.primitiveCheck->setObjectName("primitive_variables");
    boundaryOutputUI.rhoCheck->setObjectName("rho");
    boundaryOutputUI.uCheck->setObjectName("u");
    boundaryOutputUI.vCheck->setObjectName("v");
    boundaryOutputUI.wCheck->setObjectName("w");
    boundaryOutputUI.pCheck->setObjectName("p");
    boundaryOutputUI.entropyCheck->setObjectName("entropy");
    boundaryOutputUI.machCheck->setObjectName("mach");
    boundaryOutputUI.temperatureCheck->setObjectName("temperature");
    boundaryOutputUI.iblankCheck->setObjectName("iblank");
    boundaryOutputUI.imeshCheck->setObjectName("imesh");

    boundaryOutputUI.vortMagCheck->setObjectName("vort_mag");
    boundaryOutputUI.vortXCheck->setObjectName("vort_x");
    boundaryOutputUI.vortYCheck->setObjectName("vort_y");
    boundaryOutputUI.vortZCheck->setObjectName("vort_z");
    boundaryOutputUI.qCriterionCheck->setObjectName("q_criterion");
    boundaryOutputUI.divVelCheck->setObjectName("div_vel");

    boundaryOutputUI.turbFluctCheck->setObjectName("turbulent_fluctuations");
    boundaryOutputUI.uuPrimeCheck->setObjectName("uuprime");
    boundaryOutputUI.vvPrimeCheck->setObjectName("vvprime");
    boundaryOutputUI.wwPrimeCheck->setObjectName("wwprime");
    boundaryOutputUI.uvPrimeCheck->setObjectName("uvprime");
    boundaryOutputUI.uwPrimeCheck->setObjectName("uwprime");
    boundaryOutputUI.vwPrimeCheck->setObjectName("vwprime");

    boundaryOutputUI.cpCheck->setObjectName("cp");
}
void structWidget::onWidgetChanged()
{
    QObject* s = sender();
    if (!s) return;

    QString objName = s->objectName();
    QString valueStr;

    // 🟢 QLineEdit
    if (auto line = qobject_cast<QLineEdit*>(s)) {
        valueStr = line->text();
    }
    // 🟢 QSpinBox / QDoubleSpinBox
    else if (auto spin = qobject_cast<QSpinBox*>(s)) {
        valueStr = QString::number(spin->value());
    }
    else if (auto dspin = qobject_cast<QDoubleSpinBox*>(s)) {
        valueStr = QString::number(dspin->value(), 'f', 3);
    }
    // 🟢 QCheckBox / QRadioButton / QPushButton
    else if (auto check = qobject_cast<QAbstractButton*>(s)) {
        valueStr = check->isChecked() ? ".true." : ".false.";
    }
    // 🟢 QComboBox
    else if (auto combo = qobject_cast<QComboBox*>(s)) {
        valueStr = combo->currentText();
    }
    // 🟢 QButtonGroup（如果信号直接从 group 发出）
    else if (auto group = qobject_cast<QButtonGroup*>(s)) {
        QAbstractButton* btn = group->checkedButton();
        if (btn)
            valueStr = btn->objectName();
    }





    applyCurrentBoxData();

    if (!objName.isEmpty() && !valueStr.isEmpty()) {
        QString cmd = objName + " = " + valueStr;
        emit widgetTriggered(cmd);
    }
}



void structWidget::switchToBox(QGroupBox* targetBox)
{


    for (auto [box, type] : boxes) {
        bool visible = (box == targetBox);
        box->setVisible(visible);
        if (visible)

            currentBox = type;
    }
}

void structWidget::applyCurrentBoxData()
{
    switch (currentBox) {
    case BOX_RawGrid:
        {
            settingData.rawGridData = getRawGridData();


        }
        break;

    case BOX_TurbulentDiffusion:
        {
            settingData.turbulentDiffusionModelsData = getTurbulentDiffusionModelsData();
        }
        break;

    case BOX_Ref:
        {
            settingData.referenceData = getReferenceData();
        }
        break;

    case BOX_Eqn:
        {
            settingData.eqnData = getEqnData();
        }
        break;

    case BOX_ForceMoment:
        {
            settingData.forceMomentData = getForceMomentData();
        }
        break;

    case BOX_Flux:
        {
            settingData.inviscidFluxData = getInviscidFluxData();
        }
        break;

    case BOX_CodeRun:
        {
            settingData.codeRunData = getCodeRunData();
        }
        break;

    case BOX_NonlinearSolver:
        {
            settingData.nonlinearSolverData = getNonlinearSolverData();
        }
        break;

    case BOX_LinearSolver:
        {
            settingData.linearSolverData = getLinearSolverData();
        }
        break;

    case BOX_GlobalSettings:
        {
            settingData.globalData = getGlobalData();


        }
        break;

    case BOX_VolumeOutput:
        {
            settingData.volumeOutputData = getVolumeOutputData();
        }
        break;

    case BOX_BoundaryOutput:
        {
            settingData.boundaryOutputData = getBoundaryOutputData();
        }
        break;

    default:

        break;
    }
}
void structWidget::setCloudPlotBoxItems(const QStringList &list){
    cloudPlotUI.variableCombo->blockSignals(true);

    cloudPlotUI.variableCombo->clear();
    for (int i = 0; i < list.size(); ++i) {
        QString item = QString::number(i + 1) + ": " + list[i];
        cloudPlotUI.variableCombo->addItem(item);
    }

    if (!list.isEmpty())
        cloudPlotUI.variableCombo->setCurrentIndex(0);  // 保证 currentIndex 不为 -1

    cloudPlotUI.variableCombo->blockSignals(false);
}

QString structWidget::normalizeQuoted(const QString &text)
{
    QString t = text.trimmed();

    // 若本身已带双引号，则直接返回
    if (t.startsWith("\"") && t.endsWith("\""))
        return t;

    return "\"" + t + "\"";
}
QString structWidget::cleanQuotedText(const QString &text)
{
    QString result = text.trimmed();     // 去掉前后空白
    result.remove('\'');                 // 去掉单引号 '
    result.remove('\"');                 // 去掉双引号 "
    return result;
}
