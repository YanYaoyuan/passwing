#include "saas/login/loginwidget.h"

#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QMessageBox>
#include <QSettings>
#include <QFile>
#include <QDir>
#include <QTimer>
#if defined(_MSC_VER) && (_MSC_VER >= 1600)
# pragma execution_character_set("utf-8")
#endif

LoginWidget::LoginWidget(QDialog *parent)
    : QDialog(parent)
{
    manger2D = new jobManager();//
    setWindowTitle("Login");
    setFixedSize(320, 200);
    manager = new QNetworkAccessManager(this);



    // ===== 输入框 =====
    userEdit = new QLineEdit(this);
    userEdit->setPlaceholderText("账号");

    passEdit = new QLineEdit(this);
    passEdit->setPlaceholderText("密码");
    passEdit->setEchoMode(QLineEdit::Password);

    // ===== 按钮 =====
    loginBtn = new QPushButton("登录", this);
    registerBtn = new QPushButton("注册", this);

    // ===== 布局 =====
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(userEdit);
    layout->addWidget(passEdit);
    //



    // 按钮横排
    QHBoxLayout *btnLayout = new QHBoxLayout();
    btnLayout->addWidget(loginBtn);
    btnLayout->addWidget(registerBtn);

    layout->addLayout(btnLayout);

    // ===== 信号槽 =====
    connect(loginBtn, &QPushButton::clicked,
            this, &LoginWidget::onLoginClicked);

    connect(registerBtn, &QPushButton::clicked,
            this, &LoginWidget::onRegisterClicked);

   connect(manger2D,&jobManager::meshDownloadFinish,this,&LoginWidget::receiveMeshDownLoadFinish);
   connect(manger2D,&jobManager::jobLogStatus,this,&LoginWidget::receiveMessage);

}
void LoginWidget::receiveMessage(QString text){
    emit jobLogStatus(text);
}
void LoginWidget::receiveMeshDownLoadFinish(){
    emit meshDownloadFinish();
}
void LoginWidget::onLoginClicked()
{
    user = userEdit->text();
    QString pass = passEdit->text();

    // 👉 你电脑的 device_id（先写死测试，后面再自动生成）
    QString deviceId = getWindowsMachineId();

    QUrl url("http://112.46.144.60:8000/login");
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    QJsonObject json;
    json["username"] = user;
    json["password"] = pass;
    json["device_id"] = deviceId;

    QJsonDocument doc(json);
    const QByteArray requestBody = doc.toJson();

    QNetworkReply *reply = manager->post(request, requestBody);

    connect(reply, &QNetworkReply::finished, this, [=]() {
        QByteArray response = reply->readAll();

        QJsonDocument respDoc = QJsonDocument::fromJson(response);
        QJsonObject obj = respDoc.object();

        reply->deleteLater();

        if (obj.contains("token")) {
            QString token = obj["token"].toString();

            QMessageBox::information(this, "OK", "登录成功");

            isJoin = true;


            // 👉 保存 token（后面 submit_job 要用）
            this->token = token;
            manger2D->setUserMessage(token,jobId,user);
            this->close();

        } else {
            isJoin = false;
            QString err = obj["detail"].toString();
            QMessageBox::warning(this, "Error", err);
        }
    });

}
void LoginWidget::onRegisterClicked()
{
    user = userEdit->text();
    QString pass = passEdit->text();

    QUrl url("http://112.46.144.60:8000/register");
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    QJsonObject json;
    json["username"] = user;
    json["password"] = pass;

    QNetworkReply* reply = manager->post(request, QJsonDocument(json).toJson());

    connect(reply, &QNetworkReply::finished, this, [reply]() {

        if (reply->error() != QNetworkReply::NoError) {
            //qDebug() << "Network error:" << reply->errorString();
            reply->deleteLater();
            return;
        }

        QByteArray res = reply->readAll();
        //qDebug() << "RAW RESPONSE:" << res;

        QJsonDocument doc = QJsonDocument::fromJson(res);
        if (doc.isNull() || !doc.isObject()) {
            //qDebug() << "JSON parse failed";
            reply->deleteLater();
            return;
        }

        QJsonObject obj = doc.object();

        QString status = obj.value("status").toString();
        QString msg = obj.value("msg").toString();

        //qDebug() << "STATUS:" << status;
        //qDebug() << "MSG:" << msg;

        if (status == "pending") {
            QMessageBox::information(nullptr, "register",
                "申请成功，等待管理员审批");
        }
        else if (status == "error") {
            QMessageBox::warning(nullptr, "register",
                msg.isEmpty() ? "注册失败" : msg);
        }
        else {
            QMessageBox::warning(nullptr, "register",
                "未知返回状态");
        }

        reply->deleteLater();
    });
}
void LoginWidget::submitJob(const QVector<QVector<double>>& airfoil, const GridGenData par,QString name)
{
    manger2D->submitJob(airfoil,par,name);

}

QString LoginWidget::getWindowsMachineId()
{

    QSettings settings(
        "HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\Cryptography",
        QSettings::NativeFormat
    );

    return settings.value("MachineGuid").toString();

}





// ─────────────────────────────────────────────
// 提交 FUN3D 求解任务
// ─────────────────────────────────────────────
void LoginWidget::submitFun3d(int coreNum)
{
    manger2D->submitFun3d(coreNum);
}

