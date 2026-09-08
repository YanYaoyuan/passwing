#ifndef AIRFOILANALYSE_H
#define AIRFOILANALYSE_H

#include <QWidget>
#include <QGridLayout>
#include <QTableWidget>
#include <QLabel>
#include <QLineEdit>
#include <QCombobox>
class airfoilAnalyse : public QWidget
{
public:
    airfoilAnalyse();
    /*********************************compute**************************************/
public:
    QWidget *moreAirfoilComputeWidget;
    QGridLayout *gridlayoutA;
    QTableWidget *airfoilTableList;







    /*QListView *listView*/         QLabel *ReLabel;                    QLineEdit *ReEdit;                   QLabel *MaLabel;          QLineEdit *MaEdit;
                                    QLabel *locationThicknessLabel;     QLineEdit *locationThicknessEdit;    QLabel *flapAngleLabel;   QLineEdit *flapAngleEdit;
                                    QLabel *radiusLabel;                QLineEdit *radiusEdit;               QLabel *NriclLabel;       QComboBox *NriclCombobox;
                                    QLabel *xtrTopLabel;                QLineEdit *xtrTopEdit;               QLabel *xtrBotLabel;      QLineEdit *xtrBotEdit;
                                    QLabel *alphaLabel;                 QLabel *alphaCrLabel;                QLineEdit *minAlphaEdit;  QLineEdit *maxAlphaEdit;

};

#endif // AIRFOILANALYSE_H
