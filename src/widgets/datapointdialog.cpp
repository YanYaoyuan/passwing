#include "datapointdialog.h"
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QDoubleValidator>

DataPointDialog::DataPointDialog(QWidget *parent) :
    QDialog(parent),
    mainLayout(new QVBoxLayout(this)),
    inputsLayout(new QVBoxLayout),
    addButton(new QPushButton("+", this)),
    okButton(new QPushButton("OK", this)),
    cancelButton(new QPushButton("Cancel", this)),
    nextId(0)
{
    setWindowTitle("Add Data Points");

    // Set up main layout
    mainLayout->addLayout(inputsLayout);

    // Add initial input fields
    addInputFields();

    // Set up buttons layout
    QHBoxLayout *buttonLayout = new QHBoxLayout;
    buttonLayout->addWidget(addButton);
    buttonLayout->addWidget(okButton);
    buttonLayout->addWidget(cancelButton);
    mainLayout->addLayout(buttonLayout);

    // Connect signals
    connect(addButton, &QPushButton::clicked, this, &DataPointDialog::addInputFields);
    connect(okButton, &QPushButton::clicked, this, &DataPointDialog::accept);
    connect(cancelButton, &QPushButton::clicked, this, &DataPointDialog::reject);
}

DataPointDialog::~DataPointDialog()
{
    // Cleanup
    delete addButton;
    delete okButton;
    delete cancelButton;
    delete inputsLayout;
    delete mainLayout;
}

QVector<QPair<double, double>> DataPointDialog::getPoints() const
{
    QVector<QPair<double, double>> points;
    for (int i = 0; i < inputsLayout->count(); ++i) {
        QLayoutItem *item = inputsLayout->itemAt(i);
        if (item->widget()) {
            QLineEdit *xLineEdit = item->widget()->findChild<QLineEdit*>("xLineEdit");
            QLineEdit *yLineEdit = item->widget()->findChild<QLineEdit*>("yLineEdit");
            if (xLineEdit && yLineEdit) {
                bool xOk, yOk;
                double x = xLineEdit->text().toDouble(&xOk);
                double y = yLineEdit->text().toDouble(&yOk);
                if (xOk && yOk) {
                    points.append(QPair<double, double>(x, y));
                }
            }
        }
    }
    return points;
}

void DataPointDialog::addInputFields()
{
    // Create a new layout for the new input fields
    QWidget *newFields = new QWidget(this);
    QHBoxLayout *layout = new QHBoxLayout(newFields);

    // Create X and Y line edits
    QLineEdit *xLineEdit = new QLineEdit(newFields);
    xLineEdit->setObjectName("xLineEdit");
    xLineEdit->setPlaceholderText("X coordinate");
    layout->addWidget(xLineEdit);

    QLineEdit *yLineEdit = new QLineEdit(newFields);
    yLineEdit->setObjectName("yLineEdit");
    yLineEdit->setPlaceholderText("Y coordinate");
    layout->addWidget(yLineEdit);

    // Add the new fields to the layout
    inputsLayout->addWidget(newFields);
}

