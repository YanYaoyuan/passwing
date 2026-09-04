#pragma once
#include <QObject>
#include <QVector>
#include <QIcon>
#include <QString>
#include <vtkSmartPointer.h>
#include <vtkLookupTable.h>

class ColorMapManager : public QObject
{
    Q_OBJECT
public:
    explicit ColorMapManager(QObject* parent = nullptr);

    int count() const { return presets.size(); }

    QString getName(int index) const;
    QIcon   getIcon(int index) const;
    vtkSmartPointer<vtkLookupTable> getLUT(int index) const;

    void generatePNGPreview(const QString& saveDir, int width = 256, int height = 20);

private:
    struct Preset
    {
        QString name;
        QIcon icon;
        vtkSmartPointer<vtkLookupTable> lut;
    };

    QVector<Preset> presets;

    // Internal LUT creators
    vtkSmartPointer<vtkLookupTable> makeJet();
    vtkSmartPointer<vtkLookupTable> makeRainbow();
    vtkSmartPointer<vtkLookupTable> makeHot();
    vtkSmartPointer<vtkLookupTable> makeCoolWarm();
    vtkSmartPointer<vtkLookupTable> makeGreys();
};
