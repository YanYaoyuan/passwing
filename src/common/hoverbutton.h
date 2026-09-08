#ifndef HOVERBUTTON_H
#define HOVERBUTTON_H

#include <QPushButton>
#include <QPropertyAnimation>
#include <QEvent>
#include <QEnterEvent>
#include <QIcon>

class hoverButton : public QPushButton {
    Q_OBJECT

public:
    hoverButton(QWidget *parent = nullptr) :
        QPushButton(parent), defaultSize(40, 40), iconSize(40, 40) {
        setFixedSize(defaultSize);
        setMouseTracking(true); // 启用鼠标跟踪

        sizeAnimation = new QPropertyAnimation(this, "geometry");
        sizeAnimation->setDuration(200); // 设置动画持续时间
    }

    void setButtonIcon(const QString &iconPath, const QSize &size) {
        buttonIcon = QIcon(iconPath);
        iconSize = size;
        setIcon(buttonIcon);
        setIconSize(iconSize);
    }

protected:
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    void enterEvent(QEnterEvent *event) override {
#else
    void enterEvent(QEvent *event) override {
#endif
        if (event->type() == QEvent::Enter) {
            animateSize(defaultSize + QSize(10, 10)); // 放大按钮
            updateIconSize(iconSize + QSize(10, 10)); // 放大图标
        }
        QPushButton::enterEvent(event); // 调用基类的 enterEvent 处理
    }

    void leaveEvent(QEvent *event) override {
        if (event->type() == QEvent::Leave) {
            animateSize(defaultSize); // 恢复原始大小
            updateIconSize(iconSize); // 恢复图标大小
        }
        QPushButton::leaveEvent(event);
    }




private:
    QSize defaultSize;
    QPropertyAnimation *sizeAnimation;
    QIcon buttonIcon;
    QSize iconSize;

    void animateSize(const QSize &targetSize) {
        sizeAnimation->stop();
        sizeAnimation->setStartValue(geometry());
        sizeAnimation->setEndValue(QRect(geometry().x(), geometry().y(), targetSize.width(), targetSize.height()));
        sizeAnimation->start();
    }

    void updateIconSize(const QSize &size) {
        setIconSize(size);
        update();
    }
};

#endif // HOVERBUTTON_H
