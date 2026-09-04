#include <QApplication>
#include <QPushButton>
#include <QPixmap>
#include <QPainter>
#include <QEnterEvent>
#include <QPainterPath>
class imageButton : public QPushButton {
public:
    imageButton(const QString &imagePath, QWidget *parent = nullptr)
        : QPushButton(parent), pixmap(imagePath), hovered(false) {
        setMouseTracking(true);
        setStyleSheet("QPushButton { border: none; border-radius: 15px; }"); // 设置圆角半径
    }

protected:
    void paintEvent(QPaintEvent *event) override {
        QPushButton::paintEvent(event);

        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing); // 开启反锯齿

        // 创建一个圆角矩形路径
        QPainterPath path;
        path.addRoundedRect(rect(), 15, 15); // 圆角半径为15
        painter.setClipPath(path);

        if (!pixmap.isNull()) {
            QPixmap scaledPixmap = pixmap.scaled(this->size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
            painter.drawPixmap(0, 0, scaledPixmap);
        }

        // 绘制圆角背景和边框
        if (hovered) {
            painter.setBrush(QColor(0, 0, 0, 50)); // 半透明黑色背景
            painter.setPen(Qt::NoPen);
            painter.drawRoundedRect(rect(), 15, 15); // 绘制圆角背景

            // 绘制圆角边框
            painter.setBrush(Qt::NoBrush);
            painter.setPen(QPen(QColor(0, 0, 0, 150), 2)); // 半透明黑色边框，线宽为2
            painter.drawRoundedRect(rect(), 15, 15); // 绘制圆角边框
        } else {
            // 确保正常状态下也绘制圆角边框
            painter.setBrush(Qt::NoBrush);
            painter.setPen(QPen(QColor(0, 0, 0, 150), 2)); // 半透明黑色边框，线宽为2
            painter.drawRoundedRect(rect(), 15, 15); // 绘制圆角边框
        }
    }

    void enterEvent(QEvent *event)override {
        hovered = true;
        update(); // 重新绘制
        QPushButton::enterEvent(event);
    }

    void leaveEvent(QEvent *event) override {
        hovered = false;
        update(); // 重新绘制
        QPushButton::leaveEvent(event);
    }

private:
    QPixmap pixmap;
    bool hovered;
};
