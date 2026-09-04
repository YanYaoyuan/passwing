#ifndef AIRPLANELIBRARY_H
#define AIRPLANELIBRARY_H
#include <QHBoxLayout>
#include <QTreeWidget>
#include <QDialog>
#include "publicWidgetClass/customscrollarea.h"
#include <QtWebEngineWidgets/QWebEngineView>

#include "geometryClass/stlreader.h"

class airplaneLibrary : public QWidget
{
    Q_OBJECT
public:
    airplaneLibrary(QWidget* parent = nullptr);
    ~airplaneLibrary();
    QWebEngineView *webView;

    void showAirportDialog();
private:
    QDialog *libraryDialog;

    QTreeWidget *airplaneList;
    QVector<QString>projectFileNameArray;
    QVector<QVector<QString>>modelNameArray;

    STLReader *stlWidget;


    void initialAirplaneLibrary();
    void initialAirplaneList();


    QWidget *contentWidget;
    QVBoxLayout *contentLayout;
    QVBoxLayout *mainLayout;
    QScrollArea *scrolArea;
    QVBoxLayout *vLayout;
    STLReader *modelWidget;
    CustomScrollArea *scrollArea;


    void initialScrolArea();
    void changeLibrariesView(const QString);


    void readAllFiles();

    QString choiceFileName;


private slots:
    void changeModel(QTreeWidgetItem*, int);
    void changeView();





};

#endif // AIRPLANELIBRARY_H
