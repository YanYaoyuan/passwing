#ifndef PROPELLERLIBARY_H
#define PROPELLERLIBARY_H

#include <QWidget>
#include <QListView>
#include <QChart>
#include <QChartView>
#include <QComboBox>
#include <QPushButton>
#include <QLineEdit>
#include <QLabel>
#include <QStringListModel>
#include <QGridLayout>


class propellerLibary:public QWidget
{
    Q_OBJECT
    //螺旋桨模型库
public:
    QVector<QString>propGeoNameArray;//模型名称
    QVector<QVector<QVector<double>>>propGeoArray;//模型数据
    QVector<QStringList>propAirfoilNameArray;//截面翼型名称

    double threePointInterpolation(const QVector<QVector<double>>& data, double target_x, int column);
private:

    void readPropGeometey();
    double lagrangeInterpolation(const QVector<QVector<double>>& nearestPoints, double target_x, int column);
    void findNearestPoints(const QVector<QVector<double>>& data, double target_x, QVector<QVector<double>>& nearestPoints);
    //螺旋桨性能库
public:
    propellerLibary();
    void initialPropLibaries();
    void initialPropModel();
    QVector<QString>propNameA;
    QVector<QString>propNameCopy;
    QVector<QString>propNameB;
    QWidget *propLibaryWidget;
    QPushButton *addPropButton;
    QPushButton *cancelPropButton;
    int choiceIndex = 0;
    QString choicePropName;

private:
    QWidget *propDataWidget;




private:


    QGridLayout *libaryGLayout;
    QHBoxLayout *libaryHLayout;
    QComboBox *libaryTypeCombobox;
    QLabel *propDisplayLabel;
    QLabel *propDiameterLabel;
    QLabel *propPitchLabel;
    QLineEdit *propDiameterEdit;
    QLineEdit *propPitchEdit;
    QPushButton *searchPropButton;


    //QWidget libaryWidget;
    QListView *listView;
    //QGraphicsSimpleTextItem *mouseLocitionLibary;//
    QStringListModel *StringListModelA;
    QStringListModel *StringListModelB;
    QVector<QString>propPictureNameA;
    QVector<QString>propPictureNameB;
    bool searchType = false;

    void readPropAName();
    void readPropBName();
    void readPropAPicture();
    //slots
    void changePropPicture(const QItemSelection &,const QItemSelection &);
    void searchProp();

    //func
    int findPictureId(const int);

};

#endif // PROPELLERLIBARY_H
