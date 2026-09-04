#ifndef CUSTOMSCROLLAREA_H
#define CUSTOMSCROLLAREA_H

#include <QScrollArea>
#include <QWheelEvent>
#include <QEvent>
#include <QScrollBar>
#include <QDebug>  // 添加调试信息

class CustomScrollArea : public QScrollArea
{
    Q_OBJECT

public:
    explicit CustomScrollArea(QWidget *parent = nullptr)
        : QScrollArea(parent)
    {
    }

protected:
    void wheelEvent(QWheelEvent *event) override
    {
        // 获取滚动条区域的位置和大小
        QRect horizontalScrollBarRect = horizontalScrollBar()->geometry();
        QRect verticalScrollBarRect = verticalScrollBar()->geometry();

        // 获取滚动条的全局位置
        QPoint horizontalScrollBarPos = mapToGlobal(horizontalScrollBarRect.topLeft());
        QPoint verticalScrollBarPos = mapToGlobal(verticalScrollBarRect.topLeft());

        // 获取鼠标的全局位置
        QPoint mousePos = QCursor::pos();

        // 判断鼠标是否在水平滚动条区域内
        bool inHorizontalScrollBar = mousePos.x() >= horizontalScrollBarPos.x() &&
                                     mousePos.x() <= (horizontalScrollBarPos.x() + horizontalScrollBarRect.width()) &&
                                     mousePos.y() >= horizontalScrollBarPos.y() &&
                                     mousePos.y() <= (horizontalScrollBarPos.y() + horizontalScrollBarRect.height());

        // 判断鼠标是否在垂直滚动条区域内
        bool inVerticalScrollBar = mousePos.x() >= verticalScrollBarPos.x() &&
                                   mousePos.x() <= (verticalScrollBarPos.x() + verticalScrollBarRect.width()) &&
                                   mousePos.y() >= verticalScrollBarPos.y() &&
                                   mousePos.y() <= (verticalScrollBarPos.y() + verticalScrollBarRect.height());

        // 仅在鼠标在滚动条区域内时处理滚轮事件
        if (inHorizontalScrollBar || inVerticalScrollBar)
        {

            QScrollArea::wheelEvent(event); // 调用基类的处理方法
        }
        else
        {

            event->ignore(); // Ignore the event if not in scroll bar area
        }
    }
};

#endif // CUSTOMSCROLLAREA_H

