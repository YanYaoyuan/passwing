#ifndef FUN3DWIDGET_H
#define FUN3DWIDGET_H

#include <QWidget>
#include <QLabel>
#include <QGroupBox>
#include <QFormLayout>
#include <QVBoxLayout>
#include <QScrollArea>
#include <QString>
#include "saas/funClass/structcfddefinition.h"

// 你原来的 structDefinition 应在可见头文件中包含或前置声明
// 假设 structDefinition 已在其他头文件定义并包含在工程中
// #include "structDefinition.h"

class Fun3DWidget : public QWidget
{
    Q_OBJECT
public:
    explicit Fun3DWidget(QWidget* parent = nullptr);

    // 传入你的结构体对象即可刷新界面（只更新右侧值）
    void updateUI(const structCFDDefinition& data);

private:
    // Helper to create a labeled value QLabel (returns pointer to value label)
    QLabel* makeValueLabel(const QString& initial = "-");

    // Group boxes
    QGroupBox* grpProject;
    QGroupBox* grpGoverning;
    QGroupBox* grpReference;
    QGroupBox* grpInviscid;
    QGroupBox* grpMolecular;
    QGroupBox* grpTurbulent;
    QGroupBox* grpGlobal;
    QGroupBox* grpBoundaryOutput;
    QGroupBox* grpVolumeOutput;
    QGroupBox* grpForceMoment;
    QGroupBox* grpNonlinear;
    QGroupBox* grpLinear;
    QGroupBox* grpRunControl;
    QGroupBox* grpRawGrid;

    // ---- labels: Project ----
    QLabel* lbl_project_rootname;

    // ---- labels: Governing ----
    QLabel* lbl_eqn_type;
    QLabel* lbl_viscous_terms;

    // ---- labels: Reference ----
    QLabel* lbl_dim_input_type;
    QLabel* lbl_temperature_units;
    QLabel* lbl_mach_number;
    QLabel* lbl_reynolds_number;
    QLabel* lbl_temperature;
    QLabel* lbl_angle_of_yaw;
    QLabel* lbl_angle_of_attack;

    // ---- labels: Inviscid Flux Method ----
    QLabel* lbl_flux_limiter;
    QLabel* lbl_first_order_iterations;
    QLabel* lbl_flux_construction;

    // ---- labels: Molecular ----
    QLabel* lbl_prandtl_number_molecular;

    // ---- labels: Turbulent diffusion ----
    QLabel* lbl_turbulence_model;

    // ---- labels: Global ----
    QLabel* lbl_boundary_animation_freq;
    QLabel* lbl_volume_animation_freq;

    // ---- labels: Boundary output variables (bools) ----
    QLabel* lbl_bnd_mach;
    QLabel* lbl_bnd_temperature;
    QLabel* lbl_bnd_cp;
    QLabel* lbl_bnd_cf_x;
    QLabel* lbl_bnd_cf_y;
    QLabel* lbl_bnd_cf_z;

    // ---- labels: Volume output variables ----
    QLabel* lbl_vol_mach;
    QLabel* lbl_vol_temperature;
    QLabel* lbl_vol_cp;

    // ---- labels: Force & Moment ----
    QLabel* lbl_area_reference;
    QLabel* lbl_x_moment_length;
    QLabel* lbl_y_moment_length;
    QLabel* lbl_x_moment_center;
    QLabel* lbl_y_moment_center;
    QLabel* lbl_z_moment_center;

    // ---- labels: Nonlinear solver ----
    QLabel* lbl_time_accuracy;
    QLabel* lbl_pseudo_time_stepping;
    QLabel* lbl_schedule_number;
    QLabel* lbl_schedule_iteration_start;
    QLabel* lbl_schedule_iteration_end;
    QLabel* lbl_schedule_cfl_start;
    QLabel* lbl_schedule_cfl_end;
    QLabel* lbl_schedule_cflturb_start;
    QLabel* lbl_schedule_cflturb_end;

    // ---- labels: Linear solver ----
    QLabel* lbl_meanflow_sweeps;
    QLabel* lbl_turbulence_sweeps;

    // ---- labels: Run control ----
    QLabel* lbl_steps;
    QLabel* lbl_stopping_tolerance;
    QLabel* lbl_restart_write_freq;
    QLabel* lbl_restart_read;

    // ---- labels: Raw grid ----
    QLabel* lbl_grid_format;
    QLabel* lbl_data_format;
    QLabel* lbl_patch_lumping;
    QLabel* lbl_ignore_euler_number;
};

#endif // FUN3DWIDGET_H

