#include "saas/publicClass/Fun3DWidget.h"
#include <QHBoxLayout>
#include <QSpacerItem>
#include <QFont>


// 假设 structDefinition.h 已包含并在项目中可访问
// #include "structDefinition.h"

Fun3DWidget::Fun3DWidget(QWidget* parent)
    : QWidget(parent)
{




    QVBoxLayout* mainLayout = new QVBoxLayout;

    // Scroll area in case many fields exceed window height
    QScrollArea* sa = new QScrollArea;
    sa->setWidgetResizable(true);
    QWidget* content = new QWidget;
    QVBoxLayout* contentLayout = new QVBoxLayout;

    auto createGroupWithForm = [&](const QString& title)->QGroupBox* {
        QGroupBox* g = new QGroupBox(title);
        QFormLayout* f = new QFormLayout;
        g->setLayout(f);
        return g;
    };

    auto addRow = [&](QGroupBox* g, const QString& name, QLabel* valueLabel){
        QFormLayout* f = qobject_cast<QFormLayout*>(g->layout());
        f->addRow(new QLabel(name), valueLabel);
    };

    // ========== Project ==========
    grpProject = createGroupWithForm("Project Settings");
    lbl_project_rootname = makeValueLabel();
    addRow(grpProject, "project_rootname:", lbl_project_rootname);
    contentLayout->addWidget(grpProject);

    // ========== Governing ==========
    grpGoverning = createGroupWithForm("Governing Equations");
    lbl_eqn_type = makeValueLabel();
    lbl_viscous_terms = makeValueLabel();
    addRow(grpGoverning, "eqn_type:", lbl_eqn_type);
    addRow(grpGoverning, "viscous_terms:", lbl_viscous_terms);
    contentLayout->addWidget(grpGoverning);

    // ========== Reference ==========
    grpReference = createGroupWithForm("Reference Physical Properties");
    lbl_dim_input_type = makeValueLabel();
    lbl_temperature_units = makeValueLabel();
    lbl_mach_number = makeValueLabel();
    lbl_reynolds_number = makeValueLabel();
    lbl_temperature = makeValueLabel();
    lbl_angle_of_yaw = makeValueLabel();
    lbl_angle_of_attack = makeValueLabel();
    addRow(grpReference, "dim_input_type:", lbl_dim_input_type);
    addRow(grpReference, "temperature_units:", lbl_temperature_units);
    addRow(grpReference, "mach_number:", lbl_mach_number);
    addRow(grpReference, "reynolds_number:", lbl_reynolds_number);
    addRow(grpReference, "temperature:", lbl_temperature);
    addRow(grpReference, "angle_of_yaw:", lbl_angle_of_yaw);
    addRow(grpReference, "angle_of_attack:", lbl_angle_of_attack);
    contentLayout->addWidget(grpReference);

    // ========== Inviscid Flux ==========
    grpInviscid = createGroupWithForm("Inviscid Flux Method");
    lbl_flux_limiter = makeValueLabel();
    lbl_first_order_iterations = makeValueLabel();
    lbl_flux_construction = makeValueLabel();
    addRow(grpInviscid, "flux_limiter:", lbl_flux_limiter);
    addRow(grpInviscid, "first_order_iterations:", lbl_first_order_iterations);
    addRow(grpInviscid, "flux_construction:", lbl_flux_construction);
    contentLayout->addWidget(grpInviscid);

    // ========== Molecular ==========
    grpMolecular = createGroupWithForm("Molecular Viscous Models");
    lbl_prandtl_number_molecular = makeValueLabel();
    addRow(grpMolecular, "prandtl_number_molecular:", lbl_prandtl_number_molecular);
    contentLayout->addWidget(grpMolecular);

    // ========== Turbulent ==========
    grpTurbulent = createGroupWithForm("Turbulent Diffusion Models");
    lbl_turbulence_model = makeValueLabel();
    addRow(grpTurbulent, "turbulence_model:", lbl_turbulence_model);
    contentLayout->addWidget(grpTurbulent);

    // ========== Global ==========
    grpGlobal = createGroupWithForm("Global Settings");
    lbl_boundary_animation_freq = makeValueLabel();
    lbl_volume_animation_freq = makeValueLabel();
    addRow(grpGlobal, "boundary_animation_freq:", lbl_boundary_animation_freq);
    addRow(grpGlobal, "volume_animation_freq:", lbl_volume_animation_freq);
    contentLayout->addWidget(grpGlobal);

    // ========== Boundary Output ==========
    grpBoundaryOutput = createGroupWithForm("Boundary Output Variables");
    lbl_bnd_mach = makeValueLabel();
    lbl_bnd_temperature = makeValueLabel();
    lbl_bnd_cp = makeValueLabel();
    lbl_bnd_cf_x = makeValueLabel();
    lbl_bnd_cf_y = makeValueLabel();
    lbl_bnd_cf_z = makeValueLabel();
    addRow(grpBoundaryOutput, "mach:", lbl_bnd_mach);
    addRow(grpBoundaryOutput, "temperature:", lbl_bnd_temperature);
    addRow(grpBoundaryOutput, "cp:", lbl_bnd_cp);
    addRow(grpBoundaryOutput, "cf_x:", lbl_bnd_cf_x);
    addRow(grpBoundaryOutput, "cf_y:", lbl_bnd_cf_y);
    addRow(grpBoundaryOutput, "cf_z:", lbl_bnd_cf_z);
    contentLayout->addWidget(grpBoundaryOutput);

    // ========== Volume Output ==========
    grpVolumeOutput = createGroupWithForm("Volume Output Variables");
    lbl_vol_mach = makeValueLabel();
    lbl_vol_temperature = makeValueLabel();
    lbl_vol_cp = makeValueLabel();
    addRow(grpVolumeOutput, "mach:", lbl_vol_mach);
    addRow(grpVolumeOutput, "temperature:", lbl_vol_temperature);
    addRow(grpVolumeOutput, "cp:", lbl_vol_cp);
    contentLayout->addWidget(grpVolumeOutput);

    // ========== Force & Moment ==========
    grpForceMoment = createGroupWithForm("Force & Moment Integration Properties");
    lbl_area_reference = makeValueLabel();
    lbl_x_moment_length = makeValueLabel();
    lbl_y_moment_length = makeValueLabel();
    lbl_x_moment_center = makeValueLabel();
    lbl_y_moment_center = makeValueLabel();
    lbl_z_moment_center = makeValueLabel();
    addRow(grpForceMoment, "area_reference:", lbl_area_reference);
    addRow(grpForceMoment, "x_moment_length:", lbl_x_moment_length);
    addRow(grpForceMoment, "y_moment_length:", lbl_y_moment_length);
    addRow(grpForceMoment, "x_moment_center:", lbl_x_moment_center);
    addRow(grpForceMoment, "y_moment_center:", lbl_y_moment_center);
    addRow(grpForceMoment, "z_moment_center:", lbl_z_moment_center);
    contentLayout->addWidget(grpForceMoment);

    // ========== Nonlinear Solver ==========
    grpNonlinear = createGroupWithForm("Nonlinear Solver Parameters");
    lbl_time_accuracy = makeValueLabel();
    lbl_pseudo_time_stepping = makeValueLabel();
    lbl_schedule_number = makeValueLabel();
    lbl_schedule_iteration_start = makeValueLabel();
    lbl_schedule_iteration_end = makeValueLabel();
    lbl_schedule_cfl_start = makeValueLabel();
    lbl_schedule_cfl_end = makeValueLabel();
    lbl_schedule_cflturb_start = makeValueLabel();
    lbl_schedule_cflturb_end = makeValueLabel();
    addRow(grpNonlinear, "time_accuracy:", lbl_time_accuracy);
    addRow(grpNonlinear, "pseudo_time_stepping:", lbl_pseudo_time_stepping);
    addRow(grpNonlinear, "schedule_number:", lbl_schedule_number);
    addRow(grpNonlinear, "schedule_iteration_start:", lbl_schedule_iteration_start);
    addRow(grpNonlinear, "schedule_iteration_end:", lbl_schedule_iteration_end);
    addRow(grpNonlinear, "schedule_cfl_start:", lbl_schedule_cfl_start);
    addRow(grpNonlinear, "schedule_cfl_end:", lbl_schedule_cfl_end);
    addRow(grpNonlinear, "schedule_cflturb_start:", lbl_schedule_cflturb_start);
    addRow(grpNonlinear, "schedule_cflturb_end:", lbl_schedule_cflturb_end);
    contentLayout->addWidget(grpNonlinear);

    // ========== Linear Solver ==========
    grpLinear = createGroupWithForm("Linear Solver Parameters");
    lbl_meanflow_sweeps = makeValueLabel();
    lbl_turbulence_sweeps = makeValueLabel();
    addRow(grpLinear, "meanflow_sweeps:", lbl_meanflow_sweeps);
    addRow(grpLinear, "turbulence_sweeps:", lbl_turbulence_sweeps);
    contentLayout->addWidget(grpLinear);

    // ========== Run Control ==========
    grpRunControl = createGroupWithForm("Code Run Control");
    lbl_steps = makeValueLabel();
    lbl_stopping_tolerance = makeValueLabel();
    lbl_restart_write_freq = makeValueLabel();
    lbl_restart_read = makeValueLabel();
    addRow(grpRunControl, "steps:", lbl_steps);
    addRow(grpRunControl, "stopping_tolerance:", lbl_stopping_tolerance);
    addRow(grpRunControl, "restart_write_freq:", lbl_restart_write_freq);
    addRow(grpRunControl, "restart_read:", lbl_restart_read);
    contentLayout->addWidget(grpRunControl);

    // ========== Raw Grid ==========
    grpRawGrid = createGroupWithForm("Raw Grid");
    lbl_grid_format = makeValueLabel();
    lbl_data_format = makeValueLabel();
    lbl_patch_lumping = makeValueLabel();
    lbl_ignore_euler_number = makeValueLabel();
    addRow(grpRawGrid, "grid_format:", lbl_grid_format);
    addRow(grpRawGrid, "data_format:", lbl_data_format);
    addRow(grpRawGrid, "patch_lumping:", lbl_patch_lumping);
    addRow(grpRawGrid, "ignore_euler_number:", lbl_ignore_euler_number);
    contentLayout->addWidget(grpRawGrid);

    contentLayout->addStretch();
    content->setLayout(contentLayout);
    sa->setWidget(content);

    mainLayout->addWidget(sa);
    setLayout(mainLayout);
}

QLabel* Fun3DWidget::makeValueLabel(const QString& initial)
{
    QLabel* l = new QLabel(initial);
    l->setTextInteractionFlags(Qt::TextSelectableByMouse);
    return l;
}

void Fun3DWidget::updateUI(const structCFDDefinition& data)
{
    // Project
    lbl_project_rootname->setText(data.project.project_rootname);

    // Governing
    lbl_eqn_type->setText(data.governing.eqn_type);
    lbl_viscous_terms->setText(data.governing.viscous_terms);

    // Reference
    lbl_dim_input_type->setText(data.reference.dim_input_type);
    lbl_temperature_units->setText(data.reference.temperature_units);
    lbl_mach_number->setText(QString::number(data.reference.mach_number, 'f', 6));
    lbl_reynolds_number->setText(QString::number(data.reference.reynolds_number, 'f', 0));
    lbl_temperature->setText(QString::number(data.reference.temperature, 'f', 6));
    lbl_angle_of_yaw->setText(QString::number(data.reference.angle_of_yaw, 'f', 6));
    lbl_angle_of_attack->setText(QString::number(data.reference.angle_of_attack, 'f', 6));

    // Inviscid
    lbl_flux_limiter->setText(data.inviscid.flux_limiter);
    lbl_first_order_iterations->setText(QString::number(data.inviscid.first_order_iterations));
    lbl_flux_construction->setText(data.inviscid.flux_construction);

    // Molecular
    lbl_prandtl_number_molecular->setText(QString::number(data.molecular.prandtl_number_molecular, 'f', 6));

    // Turbulent
    lbl_turbulence_model->setText(data.turbulent.turbulence_model);

    // Global
    lbl_boundary_animation_freq->setText(QString::number(data.global.boundary_animation_freq));
    lbl_volume_animation_freq->setText(QString::number(data.global.volume_animation_freq));

    // Boundary output variables (bool)
    lbl_bnd_mach->setText(data.boundary.mach ? "true" : "false");
    lbl_bnd_temperature->setText(data.boundary.temperature ? "true" : "false");
    lbl_bnd_cp->setText(data.boundary.cp ? "true" : "false");
    lbl_bnd_cf_x->setText(data.boundary.cf_x ? "true" : "false");
    lbl_bnd_cf_y->setText(data.boundary.cf_y ? "true" : "false");
    lbl_bnd_cf_z->setText(data.boundary.cf_z ? "true" : "false");

    // Volume output variables
    lbl_vol_mach->setText(data.volume.mach ? "true" : "false");
    lbl_vol_temperature->setText(data.volume.temperature ? "true" : "false");
    lbl_vol_cp->setText(data.volume.cp ? "true" : "false");

    // Force & moment
    lbl_area_reference->setText(QString::number(data.forceMoment.area_reference, 'f', 6));
    lbl_x_moment_length->setText(QString::number(data.forceMoment.x_moment_length, 'f', 6));
    lbl_y_moment_length->setText(QString::number(data.forceMoment.y_moment_length, 'f', 6));
    lbl_x_moment_center->setText(QString::number(data.forceMoment.x_moment_center, 'f', 6));
    lbl_y_moment_center->setText(QString::number(data.forceMoment.y_moment_center, 'f', 6));
    lbl_z_moment_center->setText(QString::number(data.forceMoment.z_moment_center, 'f', 6));

    // Nonlinear solver
    lbl_time_accuracy->setText(data.nonlinear.time_accuracy);
    lbl_pseudo_time_stepping->setText(data.nonlinear.pseudo_time_stepping);
    lbl_schedule_number->setText(QString::number(data.nonlinear.schedule_number));
    lbl_schedule_iteration_start->setText(QString::number(data.nonlinear.schedule_iteration_start));
    lbl_schedule_iteration_end->setText(QString::number(data.nonlinear.schedule_iteration_end));
    lbl_schedule_cfl_start->setText(QString::number(data.nonlinear.schedule_cfl_start, 'f', 6));
    lbl_schedule_cfl_end->setText(QString::number(data.nonlinear.schedule_cfl_end, 'f', 6));
    lbl_schedule_cflturb_start->setText(QString::number(data.nonlinear.schedule_cflturb_start, 'f', 6));
    lbl_schedule_cflturb_end->setText(QString::number(data.nonlinear.schedule_cflturb_end, 'f', 6));

    // Linear solver
    lbl_meanflow_sweeps->setText(QString::number(data.linear.meanflow_sweeps));
    lbl_turbulence_sweeps->setText(QString::number(data.linear.turbulence_sweeps));

    // Run control
    lbl_steps->setText(QString::number(data.run.steps));
    lbl_stopping_tolerance->setText(QString::number(data.run.stopping_tolerance, 'g', 12));
    lbl_restart_write_freq->setText(QString::number(data.run.restart_write_freq));
    lbl_restart_read->setText(data.run.restart_read);

    // Raw grid
    lbl_grid_format->setText(data.grid.grid_format);
    lbl_data_format->setText(data.grid.data_format);
    lbl_patch_lumping->setText(data.grid.patch_lumping);
    lbl_ignore_euler_number->setText(data.grid.ignore_euler_number ? "true" : "false");
}
