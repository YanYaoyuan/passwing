#ifndef AIRPLANELIBRARY_H
#define AIRPLANELIBRARY_H
#include <QHBoxLayout>
#include <QTreeWidget>
#include <QDialog>
#include "widgets/customscrollarea.h"
#include "geometry/stlreader.h"

class airplaneLibrary : public QWidget
{
    Q_OBJECT
public:
    airplaneLibrary(QWidget* parent = nullptr);
    ~airplaneLibrary();
    QWidget *webView = nullptr;

    void showAirportDialog();
    void stopHtmlLoading();
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
    void changeLibrariesView(const QString &fileName);
    void loadHtmlFile(const QString &fileName);


    void readAllFiles();

    QString choiceFileName;


private slots:
    void changeModel(QTreeWidgetItem*, int);
    void changeView();





};

#endif // AIRPLANELIBRARY_H
