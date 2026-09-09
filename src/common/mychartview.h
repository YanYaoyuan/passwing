#ifndef MYCHARTVIEW_H
#define MYCHARTVIEW_H
#include <QChartView>
#include <QGraphicsTextItem>


#include <QtCharts/QChart>
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
QT_CHARTS_USE_NAMESPACE
#endif

class MyChartView:public QChartView
{
    Q_OBJECT
public:

    MyChartView(QChart *chart,QWidget *parent= nullptr)
        :QChartView(chart,parent),textItem(new QGraphicsTextItem("")) {
        textItem->setFont(QFont("Arial", 10));
        textItem->setDefaultTextColor(Qt::black);
        chart->scene()->addItem(textItem);
    }
    void mouseMoveEvent(QMouseEvent *event)override{
        QPoint mousePoint = event->pos(); // 获取鼠标在视图中的位置

        QPointF chartPoint = chart()->mapToValue(mousePoint);

        emit mousePositionChanged(chartPoint);

        QChartView::mouseMoveEvent(event);

    }
    void mousePressEvent(QMouseEvent *event)override{


        if(event->button() == Qt::LeftButton){
            QPointF beginPoint = event->pos();

            // emit mousePressIs(leftButtonIs);
            emit mousePressBegin(beginPoint);
        }
        if(event->button() == Qt::RightButton){

            QPoint point = event->pos();
            emit mouseRightLeftPress(point);

        }

        QChartView::mousePressEvent(event);


        }

    void mouseReleaseEvent(QMouseEvent *event)override{


        if(event->button() == Qt::LeftButton){
            QPointF endPoint = event->pos();


            emit mouseReleaseEnd(endPoint);
        }




        QChartView::mouseReleaseEvent(event);
    }

    void resizeEvent(QResizeEvent *event)override{
        QChartView::resizeEvent(event);
        realSize = event->size();
        emit chartResized(event->size().width(), event->size().height());
    }




    QGraphicsTextItem *textItem;

    QSize getRealSize(){
        return realSize;
    }
private:
    QSize realSize;

signals:

    void mousePositionChanged(const QPointF &localPoint);
    void mousePressBegin(const QPointF &beginPoint);
    void mouseReleaseEnd(const QPointF &endPoint);
    void mouseRightLeftPress(const QPoint& point);
    void chartResized(int width, int height);


};

#endif // MYCHARTVIEW_H
