#include "airplanelibrary.h"
#include <QDir>

#include <QPushButton>
#include <QTextBrowser>
#include <QUrl>

#ifdef PASSWING_HAS_WEBENGINE
#include <QtWebEngineWidgets/QWebEngineView>
#endif
#if defined(_MSC_VER) && (_MSC_VER >= 1600)
# pragma execution_character_set("utf-8")
#endif
airplaneLibrary::airplaneLibrary(QWidget *parent)
    :QWidget(parent)
{
    // 创建 QScrollArea
    scrollArea = new CustomScrollArea(this);

    // 创建内容部件，这个部件将被设置为 QScrollArea 的部件
    contentWidget = new QWidget();

    contentLayout = new QVBoxLayout(contentWidget);

    // 创建 modelWidget 和 webView
    modelWidget = new STLReader(contentWidget);
#ifdef PASSWING_HAS_WEBENGINE
    webView = new QWebEngineView(contentWidget);
#else
    auto *browser = new QTextBrowser(contentWidget);
    browser->setOpenExternalLinks(true);
    webView = browser;
#endif





    // 设置本地文件路径
    QString localFilePathA = QDir::currentPath() + "/libaries/airplane/airPort/uas/penguin.stl";
    QString localFilePathB = QDir::currentPath() + "/libaries/airplane/airPort/uas/penguin.html";
    modelWidget->setModelName(localFilePathA);
    loadHtmlFile(localFilePathB);




    // 设置 modelWidget 的大小策略
    modelWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

    // 设置 webView 的最小高度，确保它在视口底部出现
    modelWidget->setMinimumHeight(1000);
    webView->setMinimumHeight(1000); // 根据需要调整
    contentWidget->setMinimumWidth(scrollArea->viewport()->width());

    // 将 modelWidget 和 webView 添加到布局中
    contentLayout->addWidget(modelWidget);
    contentLayout->addWidget(webView);
    contentWidget->setLayout(contentLayout);

    // 设置 QScrollArea 的部件
    scrollArea->setWidget(contentWidget);
    scrollArea->setWidgetResizable(true); // 使得内容在滚动区域中可调整大小

    // 设置主窗口的布局，并添加 QScrollArea
    mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(scrollArea);
    setLayout(mainLayout);







    readAllFiles();
    initialAirplaneLibrary();
    initialAirplaneList();



}
void airplaneLibrary::initialAirplaneLibrary(){
    libraryDialog = new QDialog();
    QVBoxLayout *vboxLayout = new QVBoxLayout(libraryDialog);
    QHBoxLayout *hLayoutA = new QHBoxLayout();
    QHBoxLayout *hLayoutB = new QHBoxLayout();
    //viewWidget = new QWidget();
    airplaneList = new QTreeWidget(libraryDialog);
    stlWidget = new STLReader(libraryDialog);

    //创建按钮
    QPushButton *button1 = new QPushButton(contentWidget);
    QPushButton *button2 = new QPushButton(contentWidget);

    hLayoutA->addWidget(airplaneList);
    hLayoutA->addWidget(stlWidget);
    hLayoutB->addWidget(button1);
    hLayoutB->addWidget(button2);

    airplaneList->setColumnCount(1); // 设置列
    airplaneList->setHeaderLabel(QObject::tr("无人机库")); // 设置标题


    button1->setText("确认");
    button2->setText("取消");

    libraryDialog->setFixedSize(900,650);
    airplaneList->setMaximumWidth(300);
    stlWidget->setMinimumWidth(600);
    vboxLayout->addLayout(hLayoutA);
    vboxLayout->addLayout(hLayoutB);
    libraryDialog->setLayout(vboxLayout);
    connect(airplaneList,&QTreeWidget::itemClicked,this,&airplaneLibrary::changeModel);
    connect(button1,&QPushButton::clicked,this,&airplaneLibrary::changeView);
    connect(button2,&QPushButton::clicked,libraryDialog,&QDialog::hide);


}

void airplaneLibrary::readAllFiles(){
    QString path = QDir::currentPath() + "/libaries/airplane/airPort";;

    QDir directory(path);
    QStringList entries = directory.entryList(QDir::AllEntries|QDir::NoDotAndDotDot);


    for(const QString& entry :entries)
       projectFileNameArray.append(entry);


    for(int i = 0;i<projectFileNameArray.length();i++){
        QVector<QString>tmp;
        QString FileName = path + "/" + projectFileNameArray[i];
        QDir directoryTmp(FileName);
        QStringList entriesTmp = directoryTmp.entryList(QDir::AllEntries|QDir::NoDotAndDotDot);
        for(const QString& entry :entriesTmp){
            if(entry.right(3) == "stl")
                tmp.append(entry.left(entry.length() - 4));
        }

        modelNameArray.append(tmp);
    }

}
void airplaneLibrary::showAirportDialog(){
    libraryDialog->show();
}
void airplaneLibrary::changeLibrariesView(const QString &fileName){
    stlWidget->setModelName(fileName + ".stl");

}
void airplaneLibrary::loadHtmlFile(const QString &fileName){
    const QUrl url = QUrl::fromLocalFile(fileName);
#ifdef PASSWING_HAS_WEBENGINE
    static_cast<QWebEngineView *>(webView)->setUrl(url);
#else
    static_cast<QTextBrowser *>(webView)->setSource(url);
#endif
}
void airplaneLibrary::stopHtmlLoading(){
#ifdef PASSWING_HAS_WEBENGINE
    static_cast<QWebEngineView *>(webView)->stop();
#endif
}
void airplaneLibrary::changeView(){
    libraryDialog->hide();
    modelWidget->setModelName(choiceFileName + ".stl");
    loadHtmlFile(choiceFileName + ".html");


}
void airplaneLibrary::initialAirplaneList(){
    for(int i = 0;i<projectFileNameArray.length();i++){
        QTreeWidgetItem *airplaneClass = new QTreeWidgetItem(airplaneList,QStringList(QString(projectFileNameArray[i])));
        airplaneClass->setData(0,Qt::UserRole, QVariant(QString::number(i)));
        for(int j = 0;j<modelNameArray[i].length();j++){
            QTreeWidgetItem *model = new QTreeWidgetItem(airplaneClass,QStringList(QString(modelNameArray[i][j])));
            model->setData(0,Qt::UserRole,QVariant(projectFileNameArray[i] + QString::number(i)));
        }
    }
}

void airplaneLibrary::changeModel(QTreeWidgetItem* item, int column){
    if(item->parent()){
        qDebug()<<item->parent()->text(0);
        QString parentName = item->parent()->text(0);
        QString childName = item->text(0);

        choiceFileName = QDir::currentPath() + "/libaries/airplane/airPort/" + parentName + "/" + childName;
        changeLibrariesView(choiceFileName);
    }

}
airplaneLibrary::~airplaneLibrary()
{

}
