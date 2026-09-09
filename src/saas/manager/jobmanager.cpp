#include "jobmanager.h"
#include <QSettings>
#include <QFile>
#include <QDir>
#include <QTimer>

#if defined(_MSC_VER) && (_MSC_VER >= 1600)
# pragma execution_character_set("utf-8")
#endif
jobManager::jobManager()
{

    manager = new QNetworkAccessManager(this);

}

void jobManager::setUserMessage(QString tokenTmp, QString jobIdTmp, QString userTmp){
    token = tokenTmp;
    jobId = jobIdTmp;
    user = userTmp;
}
void jobManager::submitJob(const QVector<QVector<double>>& airfoil, const GridGenData par,QString name)
{
    if (token.isEmpty()) {
        jobFailed("Error","请先登录！");
        //QMessageBox::warning(this, "Error", "请先登录！");
        return;
    }
    meshName = name;
    QUrl url("http://112.46.144.60:8000/submit_job");
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setRawHeader("Authorization", ("Bearer " + token).toUtf8());
    // ================= airfoil =================
    QJsonArray airfoil_data;
    for (const auto& point : airfoil) { QJsonArray p;
        p.append(point[0]);
        p.append(point[1]);
        airfoil_data.append(p);
    }

    QJsonObject json;
    json["airfoil"] = airfoil_data;
    json["grid_dim"] = 3;
    json["grid_nsrf"] = par.nsrf;
    json["grid_lesp"] = par.lesp;
    json["grid_tesp"] = par.tesp;
    json["grid_radi"] = par.radi;
    json["grid_nwke"] = par.wake;
    json["grid_jmax"] = par.jmax;
    json["grid_yplus"] = par.yPlus;
    json["grid_recd"] = par.re;

    if(!par.genType)
        json["gen_type"] = "SMTH";
    else
        json["gen_type"] = "BUFF";


    QByteArray data = QJsonDocument(json).toJson();
    QNetworkReply *reply = manager->post(request, data);

    connect(reply, &QNetworkReply::finished, this, [=]() {
        if (reply->error() != QNetworkReply::NoError) {
            jobLogStatus("HTTP ERROR:" + reply->errorString());
            reply->deleteLater();
            return;
        }

        QByteArray response = reply->readAll();
        reply->deleteLater();
        //qDebug() << "submit response:" << response;
        QJsonDocument doc = QJsonDocument::fromJson(response);
        QJsonObject obj = doc.object();


        if (obj.contains("job_id")) {
            this->jobId = obj["job_id"].toString();
            startDownloadTry();
            writeFun3dInput(meshName);

        }else{
            jobFailed("Error",obj["detail"].toString());
            //QMessageBox::warning(this, "Error", obj["detail"].toString());
        }
    });

}
void jobManager::writeFun3dInput(const QString name){
    if (token.isEmpty()) {
        //QMessageBox::warning(this, "Error", "请先登录！");
        jobFailed("Error","请先登录");
        return;
    }

    meshName = name;
    QUrl url("http://112.46.144.60:8000/create_case");
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setRawHeader("Authorization", ("Bearer " + token).toUtf8());

    QJsonObject json;
    json["user"] = user;
    json["job_id"] = jobId;

    QByteArray data = QJsonDocument(json).toJson();
    QNetworkReply *reply = manager->post(request, data);

    connect(reply, &QNetworkReply::finished, this, [=]() {
        if (reply->error() != QNetworkReply::NoError) {
            jobLogStatus("HTTP ERROR:" + reply->errorString());
            reply->deleteLater();
            return;
        }

        QByteArray response = reply->readAll();
        reply->deleteLater();
        //qDebug() << "submit response:" << response;
        QJsonDocument doc = QJsonDocument::fromJson(response);
        QJsonObject obj = doc.object();

        if (obj.contains("job_id")) {
            this->jobId = obj["job_id"].toString();

        }else{
            jobFailed("Error",obj["detail"].toString());
            //QMessageBox::warning(this, "Error", obj["detail"].toString());
        }
    });
}
void jobManager::downloadMesh()
{
    // ⭐ 防止并发请求
    if (m_isRequesting) return;
    m_isRequesting = true;



    QString url = QString("http://112.46.144.60:8000/download_mesh/%1/%2")
                    .arg(user)
                    .arg(jobId);

    QNetworkRequest request = QNetworkRequest(QUrl(url));
    QNetworkReply *reply = manager->get(request);

    connect(reply, &QNetworkReply::finished, this, [=]() {

        int status = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();

        // ⭐ 轮询阶段失败是正常的，不提示
        if (reply->error() != QNetworkReply::NoError || status != 200) {
            reply->deleteLater();
            m_isRequesting = false;
            return;
        }

        QByteArray data = reply->readAll();

        if (data.isEmpty()) {
            reply->deleteLater();
            m_isRequesting = false;
            return;
        }

        QString dirPath = QDir::currentPath() + "/mesh";

        QDir dir(dirPath);
        if (!dir.exists()) {
            dir.mkpath(".");
        }

        QString filePath = dirPath + QString("/%1.p3d").arg(meshName);

        // 删除旧文件
        if (QFile::exists(filePath)) {
            QFile::remove(filePath);
        }

        QFile file(filePath);
        if (file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {

            file.write(data);
            file.close();

            jobLogStatus("\nFile saved: " + filePath);

            emit meshDownloadFinish();

            // ⭐ 成功 → 停 timer
            if (m_timer) {
                m_timer->stop();
                m_timer->deleteLater();
                m_timer = nullptr;
            }


            m_isDownloading = false;
        }
        else {
            jobLogStatus("File open failed");
        }

        reply->deleteLater();
        m_isRequesting = false;
    });
}

QString jobManager::getWindowsMachineId()
{

    QSettings settings(
        "HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\Cryptography",
        QSettings::NativeFormat
    );

    return settings.value("MachineGuid").toString();

}


void jobManager::startDownloadTry()
{
    if (m_isDownloading) return;
    m_isDownloading = true;
    m_isQuerying = false;  // ← 加这行
    m_tryCount = 0;


    // ⭐ 如果旧 timer 存在，先清掉
    if (m_timer) {
        m_timer->stop();
        m_timer->deleteLater();
        m_timer = nullptr;
    }

    m_timer = new QTimer(this);

    connect(m_timer, &QTimer::timeout, this, [=]() {

        m_tryCount++;

        jobLogStatus(QString("Try download %1/6").arg(m_tryCount));


        if (m_tryCount >= 6) {
            m_timer->stop();
            m_timer->deleteLater();
            m_timer = nullptr;

            jobLogStatus("Download timeout");
            m_isDownloading = false;
            return;
        }

        downloadMesh();
        queryJobLog();


    });

    m_timer->start(1000);
}

void jobManager::queryJobLog()
{

    if (m_isQuerying) return;   // 上一次还没回来，跳过
    m_isQuerying = true;

    QUrl url("http://112.46.144.60:8000/task/" + jobId);

    QNetworkRequest request(url);
    request.setRawHeader("Authorization", ("Bearer " + token).toUtf8());

    QNetworkReply *reply = manager->get(request);

    connect(reply, &QNetworkReply::finished, this, [=]() {

        m_isQuerying = false;

        QByteArray response = reply->readAll();
        reply->deleteLater();

        //qDebug() << "RAW RESPONSE:" << response;

        QJsonDocument doc = QJsonDocument::fromJson(response);
        if (!doc.isObject()) {
            jobLogStatus("Invalid JSON");
            return;
        }

        QJsonObject obj = doc.object();
        QString state = obj["state"].toString();

        // ===== 状态处理 =====
        if (state == "PENDING" || state == "STARTED") {
            jobLogStatus("Running...");
            return;
        }

        qDebug()<<state;

        if (state == "FAILURE") {
            QJsonObject result = obj["result"].toObject();
            QString err = result["error"].toString();
            jobLogStatus("Job Failed:\n" + err);
            m_tryCount = 6;
            m_timer->stop();
            m_timer->deleteLater();
            m_timer = nullptr;
            m_isDownloading = false;
            return;
        }

        if (state != "SUCCESS") {
            jobLogStatus("Unknown state: " + state);
            return;
        }

        // ===== SUCCESS =====
        QJsonObject result = obj["result"].toObject();

        if (!result.contains("logs")) {
            jobLogStatus("No logs found");
            return;
        }

        QJsonObject logs = result["logs"].toObject();

        QString constructLog = logs["construct_stdout"].toString();
        QString convertLog   = logs["convert_stdout"].toString();

        jobLogStatus(
            "=== Construct ===\n" + constructLog
        );
    });
}
// ─────────────────────────────────────────────
// 提交 FUN3D 求解任务
// ─────────────────────────────────────────────
void jobManager::submitFun3d(int coreNum)
{
    if (token.isEmpty()) {
        jobFailed("未登录","请先登录后再提交 FUN3D 任务");
        //QMessageBox::warning(this, "未登录", "请先登录后再提交 FUN3D 任务");
        return;
    }

    QUrl url("http://112.46.144.60:8000/submit_fun3d");   // ← 替换为你的服务地址
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setRawHeader("Authorization", QString("Bearer %1").arg(token).toUtf8());

    QJsonObject body;
    body["job_id"]   = jobId;
    body["core_num"] = coreNum;

    QNetworkReply *reply = manager->post(request, QJsonDocument(body).toJson());
    connect(reply, &QNetworkReply::finished,
            this,  [this, reply]() { onFun3dSubmitted(reply); });
}

// ─────────────────────────────────────────────
// 提交响应：拿到 task_id，启动轮询定时器
// ─────────────────────────────────────────────
void jobManager::onFun3dSubmitted(QNetworkReply *reply)
{
    // 断开旧 SSE
    if (m_sseReply) {
        m_sseReply->abort();
        m_sseReply->deleteLater();
        m_sseReply = nullptr;
    }

    if (reply->error() != QNetworkReply::NoError) {
        QString err = reply->errorString();
        reply->deleteLater();
        emit jobLogStatus(QString("FUN3D 提交失败：%1").arg(err));
        jobFailed("提交失败", err);
        return;
    }

    QByteArray response = reply->readAll();
    reply->deleteLater();

    QJsonObject resp  = QJsonDocument::fromJson(response).object();
    m_fun3dTaskId     = resp.value("task_id").toString();
    QString caseJobId = resp.value("case_id").toString();

    if (m_fun3dTaskId.isEmpty()) {
        jobFailed("提交失败", "服务端未返回 task_id");
        return;
    }

    m_pollCount = 0;
    emit jobLogStatus(QString("FUN3D 任务已提交，task_id=%1").arg(m_fun3dTaskId));

    // ── 1. 重置轮询 timer ────────────────────────
    if (m_fun3dTimer) {
        m_fun3dTimer->stop();
        m_fun3dTimer->deleteLater();
        m_fun3dTimer = nullptr;  // ⭐ 必须置空
    }
    m_fun3dTimer = new QTimer(this);
    m_fun3dTimer->setInterval(3000);
    connect(m_fun3dTimer, &QTimer::timeout,
            this, &jobManager::pollFun3dStatus);
    m_fun3dTimer->start();

    // ── 2. 启动 SSE ──────────────────────────────
    startFun3dSSE(caseJobId);
}

// 建立 SSE 长连接
// ─────────────────────────────────────────────────────────────────
void jobManager::startFun3dSSE(const QString &requestedJobId)
{
    stopFun3dSSE();   // 先关闭旧连接

    QUrl url(QString("http://112.46.144.60:8000/fun3d/stream/%1").arg(requestedJobId));
    QNetworkRequest request(url);
    request.setRawHeader("Authorization",
                         QString("Bearer %1").arg(token).toUtf8());
    request.setRawHeader("Accept", "text/event-stream");
    // 禁止 Qt 把响应缓存起来，必须流式读取
    request.setAttribute(QNetworkRequest::CacheLoadControlAttribute,
                         QNetworkRequest::AlwaysNetwork);

    m_sseBuf.clear();
    m_sseReply = manager->get(request);

    connect(m_sseReply, &QNetworkReply::readyRead,
            this, &jobManager::onSseReadyRead);

    connect(m_sseReply, &QNetworkReply::finished, this, [this]() {
        // 服务端主动关闭（done/error 事件后服务端断开）
        emit jobLogStatus("SSE 连接已关闭");
        m_sseReply->deleteLater();
        m_sseReply = nullptr;
    });
}


void jobManager::stopFun3dSSE()
{
    if (m_sseReply) {
        m_sseReply->abort();
        m_sseReply->deleteLater();
        m_sseReply = nullptr;
    }
    m_sseBuf.clear();
}


// ─────────────────────────────────────────────────────────────────
// readyRead：把新数据追加到行缓冲，按行切分
// SSE 格式：
//   event: iteration\n
//   data: {"iter":1,"density_rms":...}\n
//   \n                          ← 空行 = 消息结束
// ─────────────────────────────────────────────────────────────────
void jobManager::onSseReadyRead()
{
    m_sseBuf += m_sseReply->readAll();

    while (true) {
        int idx = m_sseBuf.indexOf('\n');
        if (idx < 0) break;                         // 没有完整行，等下次
        QString line = QString::fromUtf8(m_sseBuf.left(idx)).trimmed();
        m_sseBuf.remove(0, idx + 1);
        onSseLine(line);
    }
}


// ─────────────────────────────────────────────────────────────────
// 解析单条 SSE 行
// ─────────────────────────────────────────────────────────────────
void jobManager::onSseLine(const QString &line)
{
    // SSE 用成对的 event:/data: 行表示一条消息
    // 用静态变量暂存 event 类型
    static QString s_event;

    if (line.startsWith("event:")) {
        s_event = line.mid(6).trimmed();   // "iteration" / "done" / "error" / "started"
        return;
    }

    if (!line.startsWith("data:")) return;

    QString     rawData = line.mid(5).trimmed();
    QJsonObject data    = QJsonDocument::fromJson(rawData.toUtf8()).object();

    if (s_event == "iteration") {
        // ── 解析迭代数据 ──────────────────────────────────────────
        int    iter       = data.value("iter").toInt();
        double densityRms = data.value("density_rms").toDouble();
        double turbRms    = data.value("turb_rms").toDouble();
        double lift       = data.value("lift").toDouble();   // null → 0.0
        double drag       = data.value("drag").toDouble();

        // 打印到日志
        emit jobLogStatus(
            QString("iter=%1  density_RMS=%2  turb_RMS=%3  Lift=%4  Drag=%5")
                .arg(iter)
                .arg(densityRms, 0, 'e', 3)
                .arg(turbRms,    0, 'e', 3)
                .arg(lift,       0, 'f', 6)
                .arg(drag,       0, 'f', 6)
        );

        // 如果有残差曲线图，在这里更新：
        // emit iterationReceived(iter, densityRms, turbRms, lift, drag);

    } else if (s_event == "started") {
        emit jobLogStatus(
            QString("FUN3D 开始求解，核数=%1").arg(data.value("np").toInt())
        );

    } else if (s_event == "done") {
        int totalIters = data.value("total_iters").toInt();
        emit jobLogStatus(
            QString("FUN3D 求解完成，共 %1 个迭代").arg(totalIters)
        );
        stopFun3dSSE();

    } else if (s_event == "error") {
        emit jobLogStatus(
            QString("FUN3D 错误：%1").arg(data.value("msg").toString())
        );
        stopFun3dSSE();
    }

    s_event.clear();
}


// ─────────────────────────────────────────────────────────────────
// 轮询 /task/{task_id}（原逻辑不变，仅在完成/失败时顺便关闭 SSE）
// ─────────────────────────────────────────────────────────────────
void jobManager::pollFun3dStatus()
{
    const int MAX_POLLS = 600;
    if (++m_pollCount > MAX_POLLS) {
        m_fun3dTimer->stop();
        stopFun3dSSE();
        emit jobLogStatus("FUN3D 轮询超时，请手动检查服务端");
        //QMessageBox::warning(this, "超时", "FUN3D 任务轮询超时");
        return;
    }

    QUrl url(QString("http://112.46.144.60:8000/task/%1").arg(m_fun3dTaskId));
    QNetworkRequest request(url);
    request.setRawHeader("Authorization",
                         QString("Bearer %1").arg(token).toUtf8());

    QNetworkReply *reply = manager->get(request);
    connect(reply, &QNetworkReply::finished, this, [this, reply]() {

        if (reply->error() != QNetworkReply::NoError) {
            QString err = reply->errorString();
            reply->deleteLater();
            emit jobLogStatus(QString("轮询网络异常（将重试）：%1").arg(err));
            return;
        }

        QByteArray  raw   = reply->readAll();
        reply->deleteLater();

        QJsonObject data  = QJsonDocument::fromJson(raw).object();
        QString     state = data.value("state").toString();

        if (state == "PENDING" || state == "STARTED") {
            emit jobLogStatus(
                QString("FUN3D 求解中… 已等待 %1s").arg(m_pollCount * 3)
            );

        } else if (state == "SUCCESS") {
            m_fun3dTimer->stop();
            stopFun3dSSE();   // SSE 通常已自动关闭，这里兜底

            QJsonObject result = data.value("result").toObject();
            QString msg = QString("FUN3D 求解完成 ✅\n"
                                  "task_id: %1\n"
                                  "job_dir: %2\n"
                                  "耗时: %3 秒")
                            .arg(m_fun3dTaskId)
                            .arg(result.value("job_dir").toString("N/A"))
                            .arg(result.value("elapsed").toDouble(), 0, 'f', 1);
            emit jobLogStatus(msg);
            QMetaObject::invokeMethod(this, [this, msg]() {
                //QMessageBox::information(this, "求解完成", msg);
                jobFailed("求解完成", msg);
            }, Qt::QueuedConnection);

        } else if (state == "FAILURE") {
            m_fun3dTimer->stop();
            stopFun3dSSE();
            QString err = data.value("error").toString();
            emit jobLogStatus(QString("FUN3D 求解失败：%1").arg(err));
            QMetaObject::invokeMethod(this, [this, err]() {
                //QMessageBox::critical(
                    //this, "求解失败",
                    //QString("task_id: %1\n错误: %2").arg(m_fun3dTaskId, err)
                //);
                jobFailed("Error",QString("task_id: %1\n错误: %2").arg(m_fun3dTaskId, err));
            }, Qt::QueuedConnection);

        } else {
            m_fun3dTimer->stop();
            stopFun3dSSE();
            emit jobLogStatus(QString("未知状态：%1").arg(state));
            QMetaObject::invokeMethod(this, [this, state]() {
                //QMessageBox::critical(this, "未知错误",
                                      //QString("state=%1").arg(state));
                jobFailed("未知错误",QString("state=%1").arg(state));
            }, Qt::QueuedConnection);
        }
    });
}
