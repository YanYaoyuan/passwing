
//#include "UIdisplay.h"
#include "mainwindow.h"
#include <QApplication>
#include <QFile>
#include <QString>
#include <QTextStream>
#include <QSplashScreen>
#include <QPixmap>
#include <QScreen>
#include <QGuiApplication>


QString loadStyleSheet(const QString &fileName) {
    QFile file(fileName);
    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        return QString();
    }
    QTextStream stream(&file);
    return stream.readAll();
}

int main(int argc, char *argv[])
{



    QApplication a(argc, argv);
    //注册一些进行传递的数据类型
    qRegisterMetaType<std::vector<double>>("std::vector<double>");
    qRegisterMetaType<QVector<double>>("QVector<double>");
    qRegisterMetaType<QVector<QVector<double>>>("QVector<QVector<double>>");
    qRegisterMetaType<QVector<QVector<QVector<double>>>>("QVector<QVector<QVector<double>>>");



    // 获取主屏幕的物理分辨率（包含任务栏）
    QRect screenGeometry = QGuiApplication::primaryScreen()->geometry();
    //qDebug() << "主屏幕物理分辨率：" << screenGeometry.width() << "x" << screenGeometry.height();

    // 加载图像
    QPixmap originalPixmap(":/images/main/passwing.png");

    // 缩放图像以适应指定的尺寸
    QPixmap scaledPixmap = originalPixmap.scaled(600, 400, Qt::KeepAspectRatio, Qt::SmoothTransformation);

    // 创建并显示欢迎界面
    QSplashScreen splash(scaledPixmap);
    // 获取主屏幕的几何属性
    //QRect screenGeometry = QGuiApplication::primaryScreen()->geometry();
    int x = screenGeometry.width() / 2 - splash.width() / 2;
    int y = screenGeometry.height() / 2 - splash.height() / 2;
    splash.move(x, y);
    splash.show();


    QString styleSheet;
    // 加载多个QSS文件并合并
    styleSheet += loadStyleSheet(":/qss/iconButton.qss");
    styleSheet += loadStyleSheet(":/qss/colorButton.qss");
    styleSheet += loadStyleSheet(":/qss/groupBox.qss");
    styleSheet += loadStyleSheet(":/qss/progressBar.qss");
    styleSheet += loadStyleSheet(":/mainQss/leftViewButton.qss");
    styleSheet += loadStyleSheet(":/mainQss/mainWindow.qss");
    styleSheet += loadStyleSheet(":/mainQss/spinBox.qss");


    // 设置全局样式表
    a.setStyleSheet(styleSheet);





    mainWindow window;
    splash.finish(&window);
    window.show();
    return a.exec();
}
