#ifndef STATUSLIGHTBUTTON_H
#define STATUSLIGHTBUTTON_H

#include <QPushButton>
#include <QPainter>

class StatusLightButton : public QPushButton
{
    Q_OBJECT

public:
    explicit StatusLightButton(QWidget *parent = nullptr)
        : QPushButton(parent), isReady(false)
    {
        setFixedSize(50, 50); // 设置按钮大小
        setCheckable(true);   // 按钮可以切换状态
        connect(this, &QPushButton::clicked, this, [this]() {
            isReady = this->isChecked(); // 更新状态
            update();                    // 重绘
        });
    }

    bool getStatus() const { return isReady; }

protected:
    void paintEvent(QPaintEvent *event) override
    {
        QPushButton::paintEvent(event);

        // 使用 QPainter 绘制状态指示灯
        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing); // 开启抗锯齿
        QBrush brush(isReady ? Qt::green : Qt::blue);  // 状态颜色
        painter.setBrush(brush);
        painter.setPen(Qt::NoPen);                     // 无边框
        int margin = 5;                                // 圆形内边距
        painter.drawEllipse(margin, margin, width() - 2 * margin, height() - 2 * margin);
    }

private:
    bool isReady; // 状态标志
};

#endif // STATUSLIGHTBUTTON_H

