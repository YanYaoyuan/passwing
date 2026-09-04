#ifndef LOGINWIDGET_H
#define LOGINWIDGET_H

#include <QDialog>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QUrl>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>
#include <QByteArray>
#include <QProgressDialog>
#include "saas/funClass/structcfddefinition.h"
#include "saas/manager/jobmanager.h"
class QLineEdit;
class QPushButton;

class LoginWidget : public QDialog
{
    Q_OBJECT

public:
    explicit LoginWidget(QDialog *parent = nullptr);
    bool isJoin = false;

private slots:
    void onLoginClicked();
    void onRegisterClicked();



    void receiveMessage(QString);
    void receiveMeshDownLoadFinish();


signals:
    void meshDownloadFinish();
    void jobLogStatus(const QString&);//任务状态

private:
    QLineEdit *userEdit;
    QLineEdit *passEdit;
    QPushButton *loginBtn;
    QPushButton *registerBtn;

    jobManager *manger2D;



private:
    QNetworkAccessManager *manager;
    QString token;
    QString jobId;
    QString user;
    QString meshName;
    QString getWindowsMachineId();

public:
    void submitJob(const QVector<QVector<double>>&,const GridGenData,QString);   //
    void submitFun3d(int coreNum = 4);  // ← 新增
};

#endif // LOGINWIDGET_H
