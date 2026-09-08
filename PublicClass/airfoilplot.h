#ifndef AIRFOILPLOT_H
#define AIRFOILPLOT_H

#include "PublicClass/qcustomplot.h"

#include <QLabel>
#include <QPointF>
#include <QVector>

class AirfoilPlot;

class AirfoilPlotText : public QLabel
{
public:
    explicit AirfoilPlotText(QWidget *parent = nullptr);

    void setPlainText(const QString &text) { setText(text); adjustSize(); }
    QRectF boundingRect() const { return QRectF(QPointF(0, 0), sizeHint()); }
    void setPos(qreal x, qreal y) { move(qRound(x), qRound(y)); raise(); }
};

class AirfoilPlotAxis
{
public:
    AirfoilPlotAxis() = default;

    void setRange(double lower, double upper);
    void setMin(double lower) { setRange(lower, max()); }
    void setMax(double upper) { setRange(min(), upper); }
    double min() const { return m_axis ? m_axis->range().lower : m_lower; }
    double max() const { return m_axis ? m_axis->range().upper : m_upper; }

    void setVisible(bool visible);
    bool isVisible() const { return m_visible; }
    void setGridLineVisible(bool visible);
    bool isGridLineVisible() const { return m_gridVisible; }
    void setReverse(bool reversed);
    void setTitleText(const QString &title);
    QString titleText() const { return m_title; }
    void setTickCount(int count);
    void setLinePenColor(const QColor &color);
    Qt::Orientation orientation() const { return m_orientation; }

private:
    friend class AirfoilPlot;
    void bind(QCPAxis *axis, AirfoilPlot *plot, Qt::Orientation orientation);
    void refresh();

    QCPAxis *m_axis = nullptr;
    AirfoilPlot *m_plot = nullptr;
    Qt::Orientation m_orientation = Qt::Horizontal;
    double m_lower = 0.0;
    double m_upper = 5.0;
    int m_tickCount = 5;
    bool m_hasRange = false;
    bool m_visible = true;
    bool m_gridVisible = true;
    bool m_reversed = false;
    QString m_title;
};

class AirfoilPlotSeries
{
public:
    explicit AirfoilPlotSeries(bool scatterOnly = false);
    ~AirfoilPlotSeries();

    void append(double x, double y);
    void append(const QPointF &point) { append(point.x(), point.y()); }
    void clear();
    int count() const { return m_points.size(); }
    const QVector<QPointF> &points() const { return m_points; }

    void setPen(const QPen &pen);
    void setColor(const QColor &color);
    void setName(const QString &name);
    QString name() const { return m_name; }
    void setVisible(bool visible);
    bool isVisible() const { return m_visible; }
    void setMarkerSize(qreal size);
    void setLegendVisible(bool visible);
    void attachAxis(AirfoilPlotAxis *) {}

private:
    friend class AirfoilPlot;
    void attach(AirfoilPlot *plot);
    void applyAppearance();

    AirfoilPlot *m_plot = nullptr;
    QCPCurve *m_curve = nullptr;
    QVector<QPointF> m_points;
    QPen m_pen = QPen(QColor(31, 119, 180), 2.0);
    QString m_name;
    qreal m_markerSize = 7.0;
    bool m_scatterOnly = false;
    bool m_visible = true;
    bool m_legendVisible = true;
};

class AirfoilPlotArea
{
public:
    AirfoilPlotArea(AirfoilPlotSeries *upper, AirfoilPlotSeries *lower);
    ~AirfoilPlotArea();

    void setBrush(const QBrush &brush);
    void setPen(const QPen &pen);
    void setVisible(bool visible);
    bool isVisible() const { return m_visible; }
    void attachAxis(AirfoilPlotAxis *) {}

private:
    friend class AirfoilPlot;
    void attach(AirfoilPlot *plot);

    AirfoilPlot *m_plot = nullptr;
    AirfoilPlotSeries *m_upper = nullptr;
    AirfoilPlotSeries *m_lower = nullptr;
    QCPGraph *m_upperGraph = nullptr;
    QCPGraph *m_lowerGraph = nullptr;
    QBrush m_brush;
    QPen m_pen = Qt::NoPen;
    bool m_visible = true;
};

class AirfoilPlot : public QCustomPlot
{
    Q_OBJECT

public:
    explicit AirfoilPlot(QWidget *parent = nullptr);
    ~AirfoilPlot() override;

    void addAxis(AirfoilPlotAxis *axis, Qt::Alignment alignment);
    QVector<AirfoilPlotAxis *> axes(Qt::Orientation orientation) const;
    QVector<AirfoilPlotAxis *> axes() const;
    AirfoilPlotAxis *horizontalAxis() const { return m_horizontalAxis; }
    AirfoilPlotAxis *verticalAxis() const { return m_verticalAxis; }

    void addSeries(AirfoilPlotSeries *series);
    void addSeries(AirfoilPlotArea *area);
    void removeSeries(AirfoilPlotSeries *series);
    void removeSeries(AirfoilPlotArea *area);
    const QVector<AirfoilPlotSeries *> &series() const { return m_series; }

    AirfoilPlot *chart() { return this; }
    const AirfoilPlot *chart() const { return this; }
    QCPLegend *legend() const { return QCustomPlot::legend; }
    void setTitle(const QString &title);
    void setBackgroundBrush(const QBrush &brush) { setBackground(brush); }
    void setRenderHint(QPainter::RenderHint, bool = true) { setAntialiasedElements(QCP::aeAll); }
    void setRangeDragEnabled(bool enabled);
    QSize getRealSize() const { return size(); }
    void queueReplot();

    AirfoilPlotText *textItem = nullptr;

signals:
    void mousePositionChanged(const QPointF &localPoint);
    void mousePressBegin(const QPointF &beginPoint);
    void mouseReleaseEnd(const QPointF &endPoint);
    void mouseRightLeftPress(const QPoint &point);
    void chartResized(int width, int height);

protected:
    void mouseMoveEvent(QMouseEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

private:
    friend class AirfoilPlotSeries;
    friend class AirfoilPlotArea;
    QPointF coordinatesAt(const QPoint &position) const;

    QVector<AirfoilPlotAxis *> m_axes;
    QVector<AirfoilPlotSeries *> m_series;
    QVector<AirfoilPlotArea *> m_areas;
    AirfoilPlotAxis *m_horizontalAxis = nullptr;
    AirfoilPlotAxis *m_verticalAxis = nullptr;
    QCPTextElement *m_titleElement = nullptr;
    bool m_destroying = false;
};

using PlotWidget = AirfoilPlot;
using PlotAxis = AirfoilPlotAxis;
using PlotSeries = AirfoilPlotSeries;
using PlotArea = AirfoilPlotArea;

#endif // AIRFOILPLOT_H
