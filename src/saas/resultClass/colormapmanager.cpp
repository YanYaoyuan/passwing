#include "saas/resultClass/colormapmanager.h"
#include <QImage>
#include <QPainter>
#include <QDebug>

ColorMapManager::ColorMapManager(QObject* parent)
    : QObject(parent)
{
    presets.clear();

    presets.push_back({ "Tecplot Jet",      QIcon(":/colormap/0.png"), makeJet() });
    presets.push_back({ "Tecplot Rainbow",  QIcon(":/colormap/1.png"), makeRainbow() });
    presets.push_back({ "Hot",              QIcon(":/colormap/2.png"), makeHot() });
    presets.push_back({ "CoolWarm",         QIcon(":/colormap/3.png"), makeCoolWarm() });
    presets.push_back({ "Greys",            QIcon(":/colormap/4.png"), makeGreys() });
}

QString ColorMapManager::getName(int index) const
{
    return presets[index].name;
}

QIcon ColorMapManager::getIcon(int index) const
{
    return presets[index].icon;
}

vtkSmartPointer<vtkLookupTable> ColorMapManager::getLUT(int index) const
{
    return presets[index].lut;
}
vtkSmartPointer<vtkLookupTable> ColorMapManager::makeJet()
{
    auto lut = vtkSmartPointer<vtkLookupTable>::New();
    lut->SetNumberOfTableValues(256);
    lut->SetHueRange(0.667, 0.0);  // Tecplot-like
    lut->SetSaturationRange(1.0, 1.0);
    lut->SetValueRange(1.0, 1.0);
    lut->Build();
    return lut;
}
vtkSmartPointer<vtkLookupTable> ColorMapManager::makeRainbow()
{
    auto lut = vtkSmartPointer<vtkLookupTable>::New();
    lut->SetNumberOfTableValues(256);
    lut->SetHueRange(0.8, -0.2);
    lut->SetSaturationRange(1.0, 1.0);
    lut->SetValueRange(1.0, 1.0);
    lut->Build();
    return lut;
}
vtkSmartPointer<vtkLookupTable> ColorMapManager::makeHot()
{
    auto lut = vtkSmartPointer<vtkLookupTable>::New();
    lut->SetNumberOfTableValues(256);

    for (int i = 0; i < 256; i++)
    {
        double t = i / 255.0;
        double r = std::min(1.0, 3 * t);
        double g = std::min(1.0, 3 * (t - 1.0/3.0));
        double b = std::min(1.0, 3 * (t - 2.0/3.0));

        lut->SetTableValue(i, r, std::max(0.0, g), std::max(0.0, b), 1.0);
    }
    lut->Build();
    return lut;
}
vtkSmartPointer<vtkLookupTable> ColorMapManager::makeCoolWarm()
{
    auto lut = vtkSmartPointer<vtkLookupTable>::New();
    lut->SetNumberOfTableValues(256);

    for (int i = 0; i < 256; i++)
    {
        double t = i / 255.0;
        lut->SetTableValue(i, t, t, 1.0 - t, 1.0);
    }
    lut->Build();
    return lut;
}
vtkSmartPointer<vtkLookupTable> ColorMapManager::makeGreys()
{
    auto lut = vtkSmartPointer<vtkLookupTable>::New();
    lut->SetNumberOfTableValues(256);

    for (int i = 0; i < 256; i++)
    {
        double t = i / 255.0;
        lut->SetTableValue(i, t, t, t, 1.0);
    }
    lut->Build();
    return lut;
}
void ColorMapManager::generatePNGPreview(const QString& dir, int w, int h)
{
    for (int i = 0; i < presets.size(); i++)
    {
        QImage img(w, h, QImage::Format_RGB32);

        for (int x = 0; x < w; x++)
        {
            double t = x / double(w - 1);
            double rgb[3];

            presets[i].lut->GetColor(t, rgb);
            QColor c(rgb[0] * 255, rgb[1] * 255, rgb[2] * 255);

            for (int y = 0; y < h; y++)
                img.setPixelColor(x, y, c);
        }

        QString filename = dir + QString("/%1.png").arg(i);
        img.save(filename);
    }
}
