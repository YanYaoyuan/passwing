#include "saas/publicClass/fun3drun.h"
#include <QDebug>
#include <QDir>
#include <QRegularExpression>
#ifdef Q_OS_WIN
#include <windows.h>
#endif
#if defined(_MSC_VER) && (_MSC_VER >= 1600)
# pragma execution_character_set("utf-8")
#endif
fun3DRun::fun3DRun(QObject *parent) : QObject(parent)
{
    process = new QProcess(this);

    // 连接信号
    connect(process, &QProcess::readyReadStandardOutput, this, &fun3DRun::onReadyReadStandardOutput);
    connect(process, &QProcess::readyReadStandardError, this, &fun3DRun::onReadyReadStandardError);
    connect(process, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            this, &fun3DRun::onProcessFinished);

}

void fun3DRun::startMPI(const QString &exeName, int processCount, const QString &workingDir)
{
    if (processCount <= 0 || exeName.trimmed().isEmpty() || !QDir(workingDir).exists()) {
        emit newError(tr("Invalid MPI launch configuration."));
        return;
    }

#ifdef Q_OS_WIN
    process->setCreateProcessArgumentsModifier([](QProcess::CreateProcessArguments *args) {
        args->flags |= CREATE_NO_WINDOW; // ✅ 隐藏 cmd 窗口
    });
#endif

    const QString program = QStringLiteral("mpiexec");
    const QStringList arguments{
        QStringLiteral("-n"),
        QString::number(processCount),
        QDir::toNativeSeparators(exeName),
    };

    currentBlock.clear();
    process->setWorkingDirectory(workingDir);
    process->start(program, arguments);
    isRun = process->waitForStarted(3000);
    if (!isRun) {
        emit newError(tr("Unable to start mpiexec: %1").arg(process->errorString()));
    }
}
void fun3DRun::stop()
{

    if (process->state() != QProcess::NotRunning) {
        process->kill();
        process->waitForFinished(2000);
    }
}

void fun3DRun::parseIterationBlock(const QStringList &lines)
{
    if (lines.isEmpty()) return;
    IterationData data;

    // 🔹 第1行：密度残差
    if (lines.size() >= 1) {
        QStringList parts = lines[0].simplified().split(QRegularExpression("\\s+"));
        if (parts.size() >= 6) {
            data.iter       = parts[0].toInt();
            data.densityRMS = parts[1].toDouble();
            data.densityMAX = parts[2].toDouble();
            data.densityX   = parts[3].toDouble();
            data.densityY   = parts[4].toDouble();
            data.densityZ   = parts[5].toDouble();
        }
    }

    // 🔹 第2行：湍流残差
    if (lines.size() >= 2) {
        QStringList parts = lines[1].simplified().split(QRegularExpression("\\s+"));
        if (parts.size() >= 5) {
            data.turbRMS = parts[0].toDouble();
            data.turbMAX = parts[1].toDouble();
            data.turbX   = parts[2].toDouble();
            data.turbY   = parts[3].toDouble();
            data.turbZ   = parts[4].toDouble();
        }
    }

    // 🔹 遍历所有行：查找 Lift / Drag
    QRegularExpression liftExp(R"(Lift\s+([-\d.E+]+))", QRegularExpression::CaseInsensitiveOption);
    QRegularExpression dragExp(R"(Drag\s+([-\d.E+]+))", QRegularExpression::CaseInsensitiveOption);

    for (const QString &line : lines) {
        if (line.contains("Lift", Qt::CaseInsensitive) || line.contains("Drag", Qt::CaseInsensitive)) {
            QRegularExpressionMatch m1 = liftExp.match(line);
            QRegularExpressionMatch m2 = dragExp.match(line);
            if (m1.hasMatch()) data.lift = m1.captured(1).toDouble();
            if (m2.hasMatch()) data.drag = m2.captured(1).toDouble();
            break; // ✅ 找到后立即退出
        }
    }


    // 🔒 写入历史数据（短时间锁定）
    {
        QMutexLocker locker(&historyMutex);
        historyData.append(data);
    }
    emit iterationParsed(data);  // ✅ 发送信号给显示模块
}


void fun3DRun::onReadyReadStandardOutput()
{
    QByteArray rawData = process->readAllStandardOutput();
    if (rawData.isEmpty()) return;

    QString text = QString::fromLocal8Bit(rawData);
    emit newOutput(text);

    QStringList lines = text.split('\n', Qt::SkipEmptyParts);
    for (QString line : lines) {
        line = line.trimmed();
        if (line.isEmpty()) continue;

        // 跳过标题行（只在最开始）
        if (line.startsWith("Iter", Qt::CaseInsensitive) ||
            line.startsWith("turb_RMS", Qt::CaseInsensitive))
            continue;

        // 🧩 组装迭代块
        currentBlock.append(line);

        // ✅ 一旦遇到 Drag，表示块结束
        if (line.contains("Drag", Qt::CaseInsensitive)) {
            if (currentBlock.size() >= 3) {

                parseIterationBlock(currentBlock);
            }
            currentBlock.clear(); // 重置收集器
        }
    }
}





void fun3DRun::onReadyReadStandardError()
{
    QString text = QString::fromLocal8Bit(process->readAllStandardError());
    if (!text.isEmpty())
        emit newError(text);  // 单纯传给 UI 日志窗口

}


void fun3DRun::onProcessFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
    isRun = false;
    emit processFinished(exitCode, exitStatus);
}


