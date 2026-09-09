#ifndef FULLWIDGETDELEGATE_H
#define FULLWIDGETDELEGATE_H
#include <QStyledItemDelegate>
#include <QPainter>
#include <QTreeView>
#include <QApplication>
#include <QLineEdit>

/**
 * @brief 一个定制的委托：
 *  - 让编辑框宽度与整列保持一致；
 *  - 第一行（例如搜索框）不显示选中或悬停背景；
 *  - 其他行仍保留正常的 hover / selected 效果。
 */
class fullWidgetDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    explicit fullWidgetDelegate(QObject *parent = nullptr)
        : QStyledItemDelegate(parent)
    {}

    // 绘制每个 item
    void paint(QPainter *painter,
               const QStyleOptionViewItem &option,
               const QModelIndex &index) const override
    {
        // ✅ 如果是第一个顶层节点（也就是放 QLineEdit 的）
        if (index.row() == 0 && !index.parent().isValid()) {
            return; // 完全不绘制，保持背景透明
        }

        // 🧩 其他节点正常绘制，让 QSS 生效
        QStyledItemDelegate::paint(painter, option, index);
    }

    // 调整编辑器几何尺寸（宽度撑满列）
    void updateEditorGeometry(QWidget * /*editor*/,
                              const QStyleOptionViewItem & /*option*/,
                              const QModelIndex & /*index*/) const override
    {

    }

    /*
    QWidget *createEditor(QWidget *parent,
                          const QStyleOptionViewItem &,
                          const QModelIndex &) const override {
        QLineEdit *edit = new QLineEdit(parent);
        edit->setMinimumHeight(58);     // ✅ 建议用这个
        edit->setStyleSheet("padding: 4px;");  // 调整视觉高度

        return edit;
    }
    */
};

#endif // FULLWIDGETDELEGATE_H
