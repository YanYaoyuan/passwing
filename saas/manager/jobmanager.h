#ifndef JOBMANAGER_H
#define JOBMANAGER_H

#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QUrl>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>
#include <QByteArray>
#include <QObject>
#include <QTimer>
#include "saas/funClass/structcfddefinition.h"
class jobManager : public QObject
{
    Q_OBJECT
public:
    jobManager();
    void setUserMessage(QString,QString,QString);
private slots:
    void onFun3dSubmitted(QNetworkReply *reply);   // ← 新增
    void pollFun3dStatus();
signals:
    void meshDownloadFinish();
    void jobLogStatus(const QString&);//任务状态
    void jobFailed(QString state,QString err);
private:
    bool m_isDownloading = false;
    bool m_isRequesting = false;
    bool m_isQuerying = false;

    QNetworkAccessManager *manager;
    QString token;
    QString jobId;
    QString user;
    QString meshName;

    // loginwidget.h 中
    QTimer* m_timer = nullptr;        // 网格下载用（保持原样）
    QTimer* m_fun3dTimer = nullptr;   // FUN3D 轮询专用


    int m_elapsed = 0;
    int m_tryCount = 0;

    QString m_fun3dTaskId;       // ← 新增
    int     m_pollCount = 0;     // ← 新增


    QString getWindowsMachineId();
    void startDownloadTry();

    void queryJobLog();

    QNetworkReply *m_sseReply = nullptr;   // SSE 长连接
    QByteArray     m_sseBuf;               // 行缓冲（SSE 按行解析）
    //
    void startFun3dSSE(const QString &jobId);
    void stopFun3dSSE();
    void onSseReadyRead();
    void onSseLine(const QString &line);    // 解析单条 SSE 行

public:
    void submitJob(const QVector<QVector<double>>&,const GridGenData,QString);   //
    void downloadMesh();
    void writeFun3dInput(const QString);
    void submitFun3d(int coreNum = 4);  // ← 新增

public slots:







};

#endif // JOBMANAGER_H
