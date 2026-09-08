#ifndef FLOWVIEWER_H
#define FLOWVIEWER_H

#include "saas/resultClass/vtkscalebar.h"
#include <QWidget>
#include <QColor>
#include <QVTKOpenGLNativeWidget.h>
#include <vtkSmartPointer.h>
#include <vtkMultiBlockDataSet.h>
#include <vtkUnstructuredGrid.h>
#include <vtkPointData.h>
#include <vtkDataArray.h>
#include <vtkDataSetMapper.h>
#include <vtkActor.h>
#include <vtkRenderer.h>
#include <vtkGenericOpenGLRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkLookupTable.h>
#include <vtkScalarBarActor.h>
#include <vtkCamera.h>
#include <vtkMatrix4x4.h>
#include <vtkOrientationMarkerWidget.h>
#include <vtkAxesActor.h>
#include <vtkPolyLine.h>
#include <vtkLight.h>
#include <vtkPolyDataMapper.h>
#include <vtkMultiBlockPLOT3DReader.h>
#include <vtkCompositeDataGeometryFilter.h>
#include <vtkStructuredGrid.h>
#include <vtkDataSetSurfaceFilter.h>
#include <map>
#include <string>



struct ValueRange{
    double min;
    double max;
    int nums = 10;
};
enum DisplayMode {
    ShowAirfoil,
    ShowMesh
};

class flowViewer : public QVTKOpenGLNativeWidget
{
    Q_OBJECT
public:
        ValueRange valueRange;
        ValueRange historyValueRange;
signals:
        void meshLogStatus(const QString&);

public:
    explicit flowViewer(QWidget *parent = nullptr);
    ~flowViewer() override;

    //contour
    void showCpCloud();
    void showScalarCloud(int);
    void setBackgroundStyle(int);
    void setMeshDisplayMode(int);
    //
    void setLightStrength(double ambient, double directional);
    void setDirectionalLightDirection(double x, double y, double z);
    void setLightColor(double r, double g, double b);

    void loadFlattenedGridAuto(const QString& xyzFile);






    // data
    void setInputData(vtkSmartPointer<vtkMultiBlockDataSet>inputData);

    // appearance
    void setBackgroundColor(const QColor &color);
    QColor getBackgroundColor() const;


public slots:
    void switchColorMap(int);
    void setCpRange(double,double,int);
    void toggleAxesWidget();
    void toggleScalarBar();


private:
    vtkScaleBar* scaleBar;
    // internal helpers
    vtkSmartPointer<vtkActor> createBasicActor(vtkDataSet* ds);

    void updateScaleBar();

    double pixelsToWorld(double px);
    double humanRound(double v);
    void setBackground( const std::string&,
                       const std::string&);

private:

    vtkSmartPointer<vtkLight> m_ambientLight;
    vtkSmartPointer<vtkLight> m_directionalLight;


    vtkSmartPointer<vtkMultiBlockDataSet> m_multiBlock;
    vtkSmartPointer<vtkUnstructuredGrid> m_unstructuredGrid;
    vtkSmartPointer<vtkPointData> m_pointData;
    vtkSmartPointer<vtkDataArray> m_scalars;

    vtkSmartPointer<vtkRenderer> m_renderer;
    vtkSmartPointer<vtkGenericOpenGLRenderWindow> m_renderWindow;
    vtkRenderWindowInteractor* m_interactor = nullptr;
    vtkSmartPointer<vtkActor> m_basicActor;

    vtkSmartPointer<vtkActor>m_wireActor;
    vtkSmartPointer<vtkActor>m_surfaceActor;




    // store actors with smart pointers
    std::map<std::string, vtkSmartPointer<vtkActor>> m_actors;
    std::map<std::string, bool> m_actorVisible;
    std::map<std::string, vtkSmartPointer<vtkLookupTable>> m_luts;
    std::map<std::string, vtkSmartPointer<vtkScalarBarActor>> m_bars;
    vtkSmartPointer<vtkDataSetMapper> m_cloudMapper;

    vtkSmartPointer<vtkAxesActor> m_axesActor;
    vtkSmartPointer<vtkOrientationMarkerWidget> m_axesWidget;


    int m_contourCount = 0;
    int m_glyphCount = 0;

    //cpScalarBar
    vtkSmartPointer<vtkLookupTable>m_lut;
    vtkSmartPointer<vtkActor> m_cpActor;
    vtkSmartPointer<vtkScalarBarActor> m_cpScalarBar;
private:
    // Internal LUT creators
    vtkSmartPointer<vtkLookupTable> makeJet();
    vtkSmartPointer<vtkLookupTable> makeRainbow();
    vtkSmartPointer<vtkLookupTable> makeHot();
    vtkSmartPointer<vtkLookupTable> makeCoolWarm();
    vtkSmartPointer<vtkLookupTable> makeGreys();

    void initFloatingToolbar();
    void initLighting();

    // 当前翼型
    DisplayMode currentMode;
    vtkSmartPointer<vtkActor> airfoilActor;
    vtkSmartPointer<vtkActor> meshActor;

    void updateAirfoilData(const QVector<QVector<double>>& airfoil);
    vtkSmartPointer<vtkActor> createAirfoilActor();

public:
    void setAirfoil(const QVector<QVector<double>>& airfoil);
private slots:


public slots:
    void leftView();
    void topView();
    void frontView();
    void bestView();
    void toggleScaleBar();
    void showAxes(bool visible);

    void switchToAirfoil();
    void switchToMesh();
};

#endif // FLOWVIEWER_H

