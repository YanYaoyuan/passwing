#ifndef DATAPOINTDIALOG_H
#define DATAPOINTDIALOG_H

#include <QDialog>
#include <QVector>
#include <QPair>

class QLineEdit;
class QPushButton;
class QVBoxLayout;
class QHBoxLayout;

class DataPointDialog : public QDialog
{
    Q_OBJECT

public:
    explicit DataPointDialog(QWidget *parent = nullptr);
    ~DataPointDialog();

    QVector<QPair<double, double>> getPoints() const;

private slots:
    void addInputFields();

private:
    QVBoxLayout *mainLayout;
    QVBoxLayout *inputsLayout;
    QPushButton *addButton;
    QPushButton *okButton;
    QPushButton *cancelButton;

    int nextId;
};

#endif // DATAPOINTDIALOG_H

