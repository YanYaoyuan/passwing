#ifndef FUN3DRUN_H
#define FUN3DRUN_H
#pragma once
#include <QObject>
#include <QProcess>
#include <QMutex>
struct IterationData {
    int iter = 0;
    double lift = 0.0;
    double drag = 0.0;
    double densityRMS = 0.0;
    double densityMAX = 0.0;
    double densityX = 0.0;
    double densityY = 0.0;
    double densityZ = 0.0;
    double turbRMS = 0.0;
    double turbMAX = 0.0;
    double turbX = 0.0;
    double turbY = 0.0;
    double turbZ = 0.0;
};


class fun3DRun : public QObject {
    Q_OBJECT
public:
    explicit fun3DRun(QObject *parent = nullptr);
    void startMPI(const QString &exeName, int processCount, const QString &workingDir);

    void stop();
    bool isRun = false;

    QMutex historyMutex;
    QVector<IterationData>historyData;
    QMetaObject::Connection outputConn;
    QMetaObject::Connection errorConn;
    QMetaObject::Connection iterConn;


signals:
    // 🟢 发给 UI 的信号
    void newOutput(const QString &text);
    void newError(const QString &text);
    void processFinished(int exitCode, QProcess::ExitStatus exitStatus);
    void iterationSectionStarted(const QString &headerLine);
    void iterationParsed(const IterationData &data);  // ✅ 每解析完一块就发信号

private slots:
    void onReadyReadStandardOutput();
    void onReadyReadStandardError();
    void onProcessFinished(int exitCode, QProcess::ExitStatus exitStatus);

private:
    QProcess *process = nullptr;
    void parseIterationBlock(const QStringList &);
    bool blockStarted = false;
    QStringList currentBlock;      // 缓冲当前 block
    QString outputBuffer;



};

#endif // FUN3DRUN_H
