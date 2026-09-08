#include "PublicClass/airfoilplot.h"

#include <QMouseEvent>
#include <QResizeEvent>

namespace {

void splitPoints(const QVector<QPointF> &points, QVector<double> &keys, QVector<double> &values)
{
    keys.reserve(points.size());
    values.reserve(points.size());
    for (const QPointF &point : points) {
        keys.append(point.x());
        values.append(point.y());
    }
}

} // namespace

AirfoilPlotText::AirfoilPlotText(QWidget *parent)
    : QLabel(parent)
{
    setAttribute(Qt::WA_TransparentForMouseEvents);
    setStyleSheet("QLabel { background: rgba(255, 255, 255, 210); padding: 2px 5px; }");
    setFont(QFont("Arial", 10));
    adjustSize();
}

void AirfoilPlotAxis::bind(QCPAxis *axis, AirfoilPlot *plot, Qt::Orientation orientation)
{
    m_axis = axis;
    m_plot = plot;
    m_orientation = orientation;
    refresh();
}

void AirfoilPlotAxis::refresh()
{
    if (!m_axis)
        return;

    if (m_hasRange)
        m_axis->setRange(m_lower, m_upper);
    m_axis->setVisible(m_visible);
    m_axis->grid()->setVisible(m_gridVisible);
    m_axis->setRangeReversed(m_reversed);
    m_axis->setLabel(m_title);
    m_axis->ticker()->setTickCount(m_tickCount);
    if (m_plot)
        m_plot->queueReplot();
}

void AirfoilPlotAxis::setRange(double lower, double upper)
{
    if (qFuzzyCompare(lower, upper)) {
        const double padding = qFuzzyIsNull(lower) ? 1.0 : qAbs(lower) * 0.05;
        lower -= padding;
        upper += padding;
    }
    m_lower = lower;
    m_upper = upper;
    m_hasRange = true;
    if (m_axis)
        m_axis->setRange(lower, upper);
    if (m_plot)
        m_plot->queueReplot();
}

void AirfoilPlotAxis::setVisible(bool visible)
{
    m_visible = visible;
    if (m_axis)
        m_axis->setVisible(visible);
    if (m_plot)
        m_plot->queueReplot();
}

void AirfoilPlotAxis::setGridLineVisible(bool visible)
{
    m_gridVisible = visible;
    if (m_axis)
        m_axis->grid()->setVisible(visible);
    if (m_plot)
        m_plot->queueReplot();
}

void AirfoilPlotAxis::setReverse(bool reversed)
{
    m_reversed = reversed;
    if (m_axis)
        m_axis->setRangeReversed(reversed);
    if (m_plot)
        m_plot->queueReplot();
}

void AirfoilPlotAxis::setTitleText(const QString &title)
{
    m_title = title;
    if (m_axis)
        m_axis->setLabel(title);
    if (m_plot)
        m_plot->queueReplot();
}

void AirfoilPlotAxis::setTickCount(int count)
{
    m_tickCount = qMax(2, count);
    if (m_axis)
        m_axis->ticker()->setTickCount(m_tickCount);
    if (m_plot)
        m_plot->queueReplot();
}

void AirfoilPlotAxis::setLinePenColor(const QColor &color)
{
    if (m_axis) {
        m_axis->setBasePen(QPen(color));
        m_axis->setTickPen(QPen(color));
        m_axis->setSubTickPen(QPen(color));
    }
    if (m_plot)
        m_plot->queueReplot();
}

AirfoilPlotSeries::AirfoilPlotSeries(bool scatterOnly)
    : m_scatterOnly(scatterOnly)
{
}

AirfoilPlotSeries::~AirfoilPlotSeries()
{
    if (m_plot)
        m_plot->removeSeries(this);
}

void AirfoilPlotSeries::attach(AirfoilPlot *plot)
{
    if (m_plot == plot)
        return;
    if (m_plot)
        m_plot->removeSeries(this);

    m_plot = plot;
    m_curve = new QCPCurve(plot->xAxis, plot->yAxis);
    QVector<double> order;
    QVector<double> keys;
    QVector<double> values;
    order.reserve(m_points.size());
    splitPoints(m_points, keys, values);
    for (int i = 0; i < m_points.size(); ++i)
        order.append(i);
    m_curve->setData(order, keys, values, true);
    applyAppearance();
}

void AirfoilPlotSeries::applyAppearance()
{
    if (!m_curve)
        return;
    m_curve->setPen(m_pen);
    m_curve->setName(m_name);
    m_curve->setVisible(m_visible);
    m_curve->setLineStyle(m_scatterOnly ? QCPCurve::lsNone : QCPCurve::lsLine);
    if (m_scatterOnly) {
        m_curve->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssDisc,
                                                  m_pen,
                                                  QBrush(m_pen.color()),
                                                  m_markerSize));
    } else {
        m_curve->setScatterStyle(QCPScatterStyle::ssNone);
    }
    if (!m_legendVisible)
        m_curve->removeFromLegend();
}

void AirfoilPlotSeries::append(double x, double y)
{
    m_points.append(QPointF(x, y));
    if (m_curve)
        m_curve->addData(m_points.size() - 1, x, y);
    if (m_plot)
        m_plot->queueReplot();
}

void AirfoilPlotSeries::clear()
{
    m_points.clear();
    if (m_curve)
        m_curve->data()->clear();
    if (m_plot)
        m_plot->queueReplot();
}

void AirfoilPlotSeries::setPen(const QPen &pen)
{
    m_pen = pen;
    applyAppearance();
    if (m_plot)
        m_plot->queueReplot();
}

void AirfoilPlotSeries::setColor(const QColor &color)
{
    m_pen.setColor(color);
    applyAppearance();
    if (m_plot)
        m_plot->queueReplot();
}

void AirfoilPlotSeries::setName(const QString &name)
{
    m_name = name;
    if (m_curve)
        m_curve->setName(name);
    if (m_plot)
        m_plot->queueReplot();
}

void AirfoilPlotSeries::setVisible(bool visible)
{
    m_visible = visible;
    if (m_curve)
        m_curve->setVisible(visible);
    if (m_plot)
        m_plot->queueReplot();
}

void AirfoilPlotSeries::setMarkerSize(qreal size)
{
    m_markerSize = size;
    applyAppearance();
    if (m_plot)
        m_plot->queueReplot();
}

void AirfoilPlotSeries::setLegendVisible(bool visible)
{
    m_legendVisible = visible;
    if (m_curve) {
        if (visible)
            m_curve->addToLegend();
        else
            m_curve->removeFromLegend();
    }
    if (m_plot)
        m_plot->queueReplot();
}

AirfoilPlotArea::AirfoilPlotArea(AirfoilPlotSeries *upper, AirfoilPlotSeries *lower)
    : m_upper(upper), m_lower(lower)
{
}

AirfoilPlotArea::~AirfoilPlotArea()
{
    if (m_plot)
        m_plot->removeSeries(this);
}

void AirfoilPlotArea::attach(AirfoilPlot *plot)
{
    if (m_plot == plot)
        return;
    if (m_plot)
        m_plot->removeSeries(this);

    m_plot = plot;
    m_upperGraph = plot->addGraph();
    m_lowerGraph = plot->addGraph();

    QVector<double> upperKeys;
    QVector<double> upperValues;
    QVector<double> lowerKeys;
    QVector<double> lowerValues;
    splitPoints(m_upper->points(), upperKeys, upperValues);
    splitPoints(m_lower->points(), lowerKeys, lowerValues);
    m_upperGraph->setData(upperKeys, upperValues);
    m_lowerGraph->setData(lowerKeys, lowerValues);
    m_upperGraph->setChannelFillGraph(m_lowerGraph);
    m_upperGraph->setBrush(m_brush);
    m_upperGraph->setPen(m_pen);
    m_lowerGraph->setPen(m_pen);
    m_upperGraph->setVisible(m_visible);
    m_lowerGraph->setVisible(m_visible);
    plot->queueReplot();
}

void AirfoilPlotArea::setBrush(const QBrush &brush)
{
    m_brush = brush;
    if (m_upperGraph)
        m_upperGraph->setBrush(brush);
    if (m_plot)
        m_plot->queueReplot();
}

void AirfoilPlotArea::setPen(const QPen &pen)
{
    m_pen = pen;
    if (m_upperGraph)
        m_upperGraph->setPen(pen);
    if (m_lowerGraph)
        m_lowerGraph->setPen(pen);
    if (m_plot)
        m_plot->queueReplot();
}

void AirfoilPlotArea::setVisible(bool visible)
{
    m_visible = visible;
    if (m_upperGraph)
        m_upperGraph->setVisible(visible);
    if (m_lowerGraph)
        m_lowerGraph->setVisible(visible);
    if (m_plot)
        m_plot->queueReplot();
}

AirfoilPlot::AirfoilPlot(QWidget *parent)
    : QCustomPlot(parent), textItem(new AirfoilPlotText(this))
{
    setMouseTracking(true);
    setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);
    axisRect()->setRangeDrag(Qt::Horizontal | Qt::Vertical);
    axisRect()->setRangeZoom(Qt::Horizontal | Qt::Vertical);
    QCustomPlot::legend->setVisible(false);
    setNoAntialiasingOnDrag(true);
}

AirfoilPlot::~AirfoilPlot()
{
    m_destroying = true;
    while (!m_areas.isEmpty())
        delete m_areas.last();
    while (!m_series.isEmpty())
        delete m_series.last();
    qDeleteAll(m_axes);
    m_axes.clear();
}

void AirfoilPlot::addAxis(AirfoilPlotAxis *axis, Qt::Alignment alignment)
{
    if (!axis || m_axes.contains(axis))
        return;

    const bool horizontal = alignment.testFlag(Qt::AlignBottom) || alignment.testFlag(Qt::AlignTop);
    axis->bind(horizontal ? xAxis : yAxis,
               this,
               horizontal ? Qt::Horizontal : Qt::Vertical);
    m_axes.append(axis);
    if (horizontal)
        m_horizontalAxis = axis;
    else
        m_verticalAxis = axis;
}

QVector<AirfoilPlotAxis *> AirfoilPlot::axes(Qt::Orientation orientation) const
{
    QVector<AirfoilPlotAxis *> result;
    for (AirfoilPlotAxis *axis : m_axes) {
        if (axis->orientation() == orientation)
            result.append(axis);
    }
    return result;
}

QVector<AirfoilPlotAxis *> AirfoilPlot::axes() const
{
    return m_axes;
}

void AirfoilPlot::addSeries(AirfoilPlotSeries *series)
{
    if (!series || m_series.contains(series))
        return;
    m_series.append(series);
    series->attach(this);
    queueReplot();
}

void AirfoilPlot::addSeries(AirfoilPlotArea *area)
{
    if (!area || m_areas.contains(area))
        return;
    m_areas.append(area);
    area->attach(this);
    queueReplot();
}

void AirfoilPlot::removeSeries(AirfoilPlotSeries *series)
{
    if (!series)
        return;
    m_series.removeOne(series);
    if (series->m_curve) {
        removePlottable(series->m_curve);
        series->m_curve = nullptr;
    }
    series->m_plot = nullptr;
    queueReplot();
}

void AirfoilPlot::removeSeries(AirfoilPlotArea *area)
{
    if (!area)
        return;
    m_areas.removeOne(area);
    if (area->m_upperGraph) {
        removeGraph(area->m_upperGraph);
        area->m_upperGraph = nullptr;
    }
    if (area->m_lowerGraph) {
        removeGraph(area->m_lowerGraph);
        area->m_lowerGraph = nullptr;
    }
    area->m_plot = nullptr;
    queueReplot();
}

void AirfoilPlot::setTitle(const QString &title)
{
    if (!m_titleElement) {
        plotLayout()->insertRow(0);
        m_titleElement = new QCPTextElement(this, title, QFont("Arial", 11, QFont::Bold));
        plotLayout()->addElement(0, 0, m_titleElement);
    } else {
        m_titleElement->setText(title);
    }
    queueReplot();
}

void AirfoilPlot::setRangeDragEnabled(bool enabled)
{
    QCP::Interactions interactions = QCP::iRangeZoom | QCP::iSelectPlottables;
    if (enabled)
        interactions |= QCP::iRangeDrag;
    setInteractions(interactions);
}

void AirfoilPlot::queueReplot()
{
    if (!m_destroying)
        replot(QCustomPlot::rpQueuedReplot);
}

QPointF AirfoilPlot::coordinatesAt(const QPoint &position) const
{
    return QPointF(xAxis->pixelToCoord(position.x()), yAxis->pixelToCoord(position.y()));
}

void AirfoilPlot::mouseMoveEvent(QMouseEvent *event)
{
    QCustomPlot::mouseMoveEvent(event);
    emit mousePositionChanged(coordinatesAt(event->pos()));
}

void AirfoilPlot::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
        emit mousePressBegin(event->pos());
    else if (event->button() == Qt::RightButton)
        emit mouseRightLeftPress(event->pos());
    QCustomPlot::mousePressEvent(event);
}

void AirfoilPlot::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
        emit mouseReleaseEnd(event->pos());
    QCustomPlot::mouseReleaseEvent(event);
}

void AirfoilPlot::resizeEvent(QResizeEvent *event)
{
    QCustomPlot::resizeEvent(event);
    emit chartResized(event->size().width(), event->size().height());
}
