#include "saas/funClass/structcfddefinition.h"

#include <QList>
#include <QDebug>
structCFDDefinition::structCFDDefinition()
{

}
QStringList structCFDDefinition::getAllText()
{
    QStringList all;

    // 按模块顺序依次追加
    all << getRawGridText()
        << getForceMomentIntegPropertiesText()
        << getGoverningEquationsText()
        << getReferencePhtsicalPropertiesText()
        << getInviscidFluxMethodText()
        << getTurbulentDiffsionModelsText()
        << getSpalartText()
        << getCodeRunControlText()
        << getNonlinearSolverParametersText()
        << getLinearSolverParametersText()
        << getGlobalText()
        << getVolumeOutputText()
        << getBoundaryOutputText();

    return all;
}



QStringList structCFDDefinition::getRawGridText() {
    QStringList list;
    list << "&raw_grid";

    appendIfValid(list, "grid_format", rawGridData.gridFormat);
    appendIfValid(list, "data_format", rawGridData.dataFormat);
    appendIfValid(list, "twod_mode", rawGridData.twodMode);
    appendIfValid(list, "swap_yz_axes", rawGridData.swapYZAxes);
    appendIfValid(list, "fieldview_coordinate_precision", rawGridData.fieldviewPrecision);
    appendIfValid(list, "patch_lumping", rawGridData.patchLumping);
    appendIfValid(list, "ignore_euler_number", rawGridData.ignoreEulerNumber);

    list << "/";
    return list;
}



QStringList structCFDDefinition::getForceMomentIntegPropertiesText() {
    QStringList list;
    list << "&force_moment_integ_properties";

    appendIfValid(list, "area_reference", forceMomentData.areaReference);
    appendIfValid(list, "x_moment_length", forceMomentData.xMomentLength);
    appendIfValid(list, "y_moment_length", forceMomentData.yMomentLength);
    appendIfValid(list, "x_moment_center", forceMomentData.xMomentCenter);
    appendIfValid(list, "y_moment_center", forceMomentData.yMomentCenter);
    appendIfValid(list, "z_moment_center", forceMomentData.zMomentCenter);

    list << "/";
    return list;
}



QStringList structCFDDefinition::getGoverningEquationsText() {
    QStringList list;
    list << "&governing_equations";

    appendIfValid(list, "eqn_type", eqnData.eqnType);

    if (eqnData.eqnType == "incompressible") {
        appendIfValid(list, "artificial_compress", eqnData.artificialCompress);
    }
    else if (eqnData.eqnType == "generic") {
        appendIfValid(list, "chemical_kinetics", eqnData.chemicalKinetics);
        appendIfValid(list, "thermal_energy_model", eqnData.thermalEnergyModel);
        appendIfValid(list, "rad_use_impl_lines", eqnData.radUseImplLines);
        appendIfValid(list, "multi_component_diff", eqnData.multiComponentDiff);
    }

    appendIfValid(list, "viscous_terms", eqnData.viscousTerms);
    appendIfValid(list, "prandtlnumber_molecular", eqnData.prandtlNumberMolecular);
    //appendIfValid(list, "schmidt_number", eqnData.schmidtNumber);
    //appendIfValid(list, "gas_radiation", eqnData.gasRadiation);

    list << "/";
    return list;
}


QStringList structCFDDefinition::getReferencePhtsicalPropertiesText()
{
    QStringList list;
    list << "&reference_physical_properties";


    appendIfValid(list, "dim_input_type", formatQuoted(referenceData.dimType));
    appendIfValid(list, "gridlength_conversion", referenceData.gridlengthConversion);


    if(referenceData.dimType == "\"nondimensional\""){
        appendIfValid(list, "reynolds_number", referenceData.reynolds);
        appendIfValid(list, "mach_number", referenceData.mach);
    }else{
        appendIfValid(list, "velocity", referenceData.velocity);
        appendIfValid(list, "density", referenceData.density);
    }
    appendIfValid(list, "temperature", referenceData.temperature);
    appendIfValid(list, "temperature_units", formatQuoted(referenceData.tempUnit));
    appendIfValid(list, "angle_of_attack", referenceData.alpha);
    appendIfValid(list, "angle_of_yaw", referenceData.yaw);

    list << "/";
    return list;
}


QStringList structCFDDefinition::getInviscidFluxMethodText()
{
    QStringList list;
    list << "&inviscid_flux_method";

    appendIfValid(list, "flux_construction", formatQuoted(inviscidFluxData.fluxConstruction));
    appendIfValid(list, "flux_construction_lhs", formatQuoted(inviscidFluxData.fluxConstructionLHS));
    appendIfValid(list, "kappa_umuscl", inviscidFluxData.kappaUmuscl);
    appendIfValid(list, "flux_limiter", formatQuoted(inviscidFluxData.fluxLimiter));
    appendIfValid(list, "first_order_iterations", inviscidFluxData.firstOrderIterations);
    appendIfValid(list, "multidm_option", inviscidFluxData.multidmOption);
    appendIfValid(list, "fixed_direction", inviscidFluxData.fixedDirection);
    appendIfValid(list, "recalc_dir_freq", inviscidFluxData.recalcDirFreq);
    appendIfValid(list, "adptv_entropy_fix", inviscidFluxData.adaptiveEntropyFix);
    appendIfValid(list, "rhs_u_eigenvalue_coef", inviscidFluxData.rhsUEigenCoef);
    appendIfValid(list, "lhs_u_eigenvalue_coef", inviscidFluxData.lhsUEigenCoef);
    appendIfValid(list, "rhs_a_eigenvalue_coef", inviscidFluxData.rhsAEigenCoef);
    appendIfValid(list, "lhs_a_eigenvalue_coef", inviscidFluxData.lhsAEigenCoef);
    appendIfValid(list, "entropy_fix", inviscidFluxData.entropyFix);
    appendIfValid(list, "re_min_vswch", inviscidFluxData.reMinVswch);
    appendIfValid(list, "re_max_vswch", inviscidFluxData.reMaxVswch);

    list << "/";
    return list;
}


QStringList structCFDDefinition::getTurbulentDiffsionModelsText()
{
    QStringList list;
    list << "&turbulent_diffusion_models";

    appendIfValid(list, "turbulence_model", formatQuoted(turbulentDiffusionModelsData.turbulenceModel));
    appendIfValid(list, "turb_intensity", turbulentDiffusionModelsData.turbIntensity);
    appendIfValid(list, "turb_viscosity_ratio", turbulentDiffusionModelsData.turbMuRatio);
    appendIfValid(list, "turb_compress_model", formatQuoted(turbulentDiffusionModelsData.turbCompress));
    appendIfValid(list, "turb_conductivity_model", formatQuoted(turbulentDiffusionModelsData.turbCond));
    appendIfValid(list, "prandtlnumber_turbulent", turbulentDiffusionModelsData.pr);
    appendIfValid(list, "schmidtnumber_turbulent", turbulentDiffusionModelsData.sc);

    list << "/";
    return list;
}


QStringList structCFDDefinition::getSpalartText()
{
    QStringList list;

    QString model = turbulentDiffusionModelsData.turbulenceModel.toLower();
    bool useSa = (model.contains("spalart") || model.contains("sa-des"));

    if (!useSa)
        return list; // 非 Spalart 模型，直接返回空列表

    list << "&spalart";

    appendIfValid(list, "turbinf", spalartData.turbInf);
    appendIfValid(list, "dalcels_mariani", spalartData.daclesMariani);
    appendIfValid(list, "sarc", spalartData.sarc);
    appendIfValid(list, "ddes", spalartData.ddes);

    list << "/";
    return list;
}


QStringList structCFDDefinition::getCodeRunControlText()
{
    QStringList list;
    list << "&code_run_control";

    appendIfValid(list, "steps", codeRunData.steps);
    appendIfValid(list, "stopping_tolerance", codeRunData.stoppingTolerance);
    //appendIfValid(list, "duration_limit_in_minutes", codeRunData.durationLimit);

    appendIfValid(list, "no_restart", codeRunData.noRestart);
    appendIfValid(list, "restart_write_freq", codeRunData.restartWriteFreq);
    appendIfValid(list, "restart_read", codeRunData.restartRead);
    appendIfValid(list, "smart_jupdate", codeRunData.smartJupdate);
    appendIfValid(list, "jacobian_eval_freq", codeRunData.jacobianEvalFreq);
    appendIfValid(list, "jupdate_startup_steps", codeRunData.jupdateStartupSteps);
    appendIfValid(list, "dfduc3_jacobians", codeRunData.dfduc3Jacobians);
    if(codeRunData.alphaSweep == ".true."){
        appendIfValid(list, "alpha_sweep", codeRunData.alphaSweep);
        appendIfValid(list, "cycle_increment", codeRunData.cycleIncrement);
        appendIfValid(list, "alpha_increment", codeRunData.alphaIncrement);
        appendIfValid(list, "alpha_max", codeRunData.alphaMax);
        appendIfValid(list, "alpha_min", codeRunData.alphaMin);
        appendIfValid(list, "alpha_switchbacks", codeRunData.alphaSwitchbacks);
    }


    list << "/";
    return list;
}


QStringList structCFDDefinition::getNonlinearSolverParametersText()
{
    QStringList list;

    list << "&nonlinear_solver_parameters"
         << QString("  time_accuracy = %1").arg(nonlinearSolverData.timeAccuracy)
         << QString("  time_step_nondim = %1").arg(nonlinearSolverData.timeStepNondim)
         << QString("  subiterations = %1").arg(nonlinearSolverData.subiterations)
         //<< QString("  temporal_err_control = %1").arg(nonlinearSolverData.temporalErrControl)
         //<< QString("  temporal_err_floor = %1").arg(nonlinearSolverData.temporalErrFloor)
         << QString("  schedule_number = %1").arg(nonlinearSolverData.scheduleNumber)
         << QString("  schedule_iteration = %1, %2")
                .arg(nonlinearSolverData.scheduleIteration1)
                .arg(nonlinearSolverData.scheduleIteration2)
         << QString("  schedule_cfl = %1, %2")
                .arg(nonlinearSolverData.scheduleCfl1)
                .arg(nonlinearSolverData.scheduleCfl2)
         << QString("  schedule_cflturb = %1, %2")
                .arg(nonlinearSolverData.scheduleCflturb1)
                .arg(nonlinearSolverData.scheduleCflturb2)
         << QString("  f_allow_minimum_m = %1").arg(nonlinearSolverData.fAllowMinimumM)
         << QString("  invis_relax_factor = %1").arg(nonlinearSolverData.inviscRelaxFactor)
         << QString("  visc_relax_factor = %1").arg(nonlinearSolverData.viscRelaxFactor)
         << "/";

    return list;
}

QStringList structCFDDefinition::getLinearSolverParametersText()
{
    QStringList list;
    list << "&linear_solver_parameters";

    appendIfValid(list, "meanflow_sweeps", linearSolverData.meanflowSweeps);
    appendIfValid(list, "turbulence_sweeps", linearSolverData.turbulenceSweeps);
    appendIfValid(list, "linear_projection", linearSolverData.linearProjection);
    appendIfValid(list, "line_implicit", linearSolverData.lineImplicit);

    list << "/";
    return list;
}

QStringList structCFDDefinition::getGlobalText()
{
    QStringList list;
    list << "&global";

    appendIfValid(list, "moving_grid", globalData.movingGrid);
    appendIfValid(list, "grid_motion_only", globalData.gridMotionOnly);
    appendIfValid(list, "grid_motion_and_dci_only", globalData.gridMotionAndDCI);
    appendIfValid(list, "body_motion_only", globalData.bodyMotionOnly);
    appendIfValid(list, "timing", globalData.timing);
    appendIfValid(list, "time_moving_grid", globalData.timeMovingGrid);
    appendIfValid(list, "boundary_animation_freq", globalData.boundaryAnimFreq);
    appendIfValid(list, "volume_animation_freq", globalData.volumeAnimFreq);
    //appendIfValid(list, "slice_freq", globalData.sliceFreq);
    // appendIfValid(list, "record_command_lines", globalData.recordCmd);

    list << "/";
    return list;
}

QStringList structCFDDefinition::getVolumeOutputText()
{
    QStringList list;
    list << "&volume_output_variables";

    //appendIfValid(list, "export_to", formatQuoted(volumeOutputData.exportTo));
    appendIfValid(list, "x", volumeOutputData.x);
    appendIfValid(list, "y", volumeOutputData.y);
    appendIfValid(list, "z", volumeOutputData.z);
    appendIfValid(list, "primitive_variables", volumeOutputData.primitive);
    appendIfValid(list, "rho", volumeOutputData.rho);
    appendIfValid(list, "u", volumeOutputData.u);
    appendIfValid(list, "v", volumeOutputData.v);
    appendIfValid(list, "w", volumeOutputData.w);
    appendIfValid(list, "p", volumeOutputData.p);
    appendIfValid(list, "entropy", volumeOutputData.entropy);
    appendIfValid(list, "mach", volumeOutputData.mach);
    appendIfValid(list, "temperature", volumeOutputData.temperature);
    appendIfValid(list, "iblank", volumeOutputData.iblank);
    appendIfValid(list, "imesh", volumeOutputData.imesh);
    appendIfValid(list, "vort_mag", volumeOutputData.vortMag);
    appendIfValid(list, "vort_x", volumeOutputData.vortX);
    appendIfValid(list, "vort_y", volumeOutputData.vortY);
    appendIfValid(list, "vort_z", volumeOutputData.vortZ);
    appendIfValid(list, "q_criterion", volumeOutputData.qCriterion);
    appendIfValid(list, "div_vel", volumeOutputData.divVel);
    appendIfValid(list, "turbulent_fluctuations", volumeOutputData.turbFluct);
    appendIfValid(list, "uu_prime", volumeOutputData.uuPrime);
    appendIfValid(list, "vv_prime", volumeOutputData.vvPrime);
    appendIfValid(list, "ww_prime", volumeOutputData.wwPrime);

    list << "/";
    return list;
}




QStringList structCFDDefinition::getBoundaryOutputText()
{
    QStringList list;
    list << "&boundary_output_variables";

    if(boundaryOutputData.numberOfBoundaries > 0){
        appendIfValid(list, "number_of_boundaries", boundaryOutputData.numberOfBoundaries);
        appendIfValid(list, "boundary_list", formatQuoted(boundaryOutputData.boundaryList));
    }
    //appendIfValid(list, "export_to", formatQuoted(boundaryOutputData.exportTo));
    appendIfValid(list, "x", boundaryOutputData.x);
    appendIfValid(list, "y", boundaryOutputData.y);
    appendIfValid(list, "z", boundaryOutputData.z);
    appendIfValid(list, "primitive_variables", boundaryOutputData.primitive);
    appendIfValid(list, "rho", boundaryOutputData.rho);
    appendIfValid(list, "u", boundaryOutputData.u);
    appendIfValid(list, "v", boundaryOutputData.v);
    appendIfValid(list, "w", boundaryOutputData.w);
    appendIfValid(list, "p", boundaryOutputData.p);
    appendIfValid(list, "entropy", boundaryOutputData.entropy);
    appendIfValid(list, "mach", boundaryOutputData.mach);
    appendIfValid(list, "temperature", boundaryOutputData.temperature);
    appendIfValid(list, "iblank", boundaryOutputData.iblank);
    appendIfValid(list, "imesh", boundaryOutputData.imesh);
    appendIfValid(list, "vort_mag", boundaryOutputData.vortMag);
    appendIfValid(list, "vort_x", boundaryOutputData.vortX);
    appendIfValid(list, "vort_y", boundaryOutputData.vortY);
    appendIfValid(list, "vort_z", boundaryOutputData.vortZ);
    appendIfValid(list, "q_criterion", boundaryOutputData.qCriterion);
    appendIfValid(list, "div_vel", boundaryOutputData.divVel);
    appendIfValid(list, "turbulent_fluctuations", boundaryOutputData.turbFluct);
    appendIfValid(list, "uu_prime", boundaryOutputData.uuPrime);
    appendIfValid(list, "vv_prime", boundaryOutputData.vvPrime);
    appendIfValid(list, "ww_prime", boundaryOutputData.wwPrime);
    appendIfValid(list, "uv_prime", boundaryOutputData.uvPrime);
    appendIfValid(list, "uw_prime", boundaryOutputData.uwPrime);
    appendIfValid(list, "vw_prime", boundaryOutputData.vwPrime);
    appendIfValid(list, "cp", boundaryOutputData.cp);

    list << "/";
    return list;
}



QString structCFDDefinition::formatQuoted(const QString &s) {
    if (s.startsWith("\"") && s.endsWith("\""))
        return s;
    return "\"" + s + "\"";
}



inline void structCFDDefinition::appendIfValid(QStringList &list, const QString &label, const QVariant &value)
{
    // 多值（QList 或 QStringList）
    if (value.canConvert<QVariantList>()) {
        QVariantList arr = value.toList();
        if (arr.isEmpty()) return;

        QStringList strList;
        for (const auto &v : arr) {
            QString s = v.toString().trimmed();
            if (!s.isEmpty() && s.compare(".false.", Qt::CaseInsensitive) != 0)
                strList << s;
        }
        if (!strList.isEmpty())
            list << QString("  %1 = %2").arg(label).arg(strList.join(" "));
        return;
    }

    // 单值：字符串或数字
    QString s = value.toString().trimmed();
    if (s.isEmpty()) return;

    // 跳过 .false.
    if (s.compare(".false.", Qt::CaseInsensitive) == 0)
        return;

    list << QString("  %1 = %2").arg(label).arg(s);
}

