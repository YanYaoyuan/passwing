#include "saas/publicClass/myfile.h"
#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <QDir>

bool myFile::writeTextFile(const QString &dirPath, const QStringList &lines,const QString& name)
{
    // 1️⃣ 检查并创建目录
    QDir dir(dirPath);
    if (!dir.exists()) {
        if (!dir.mkpath(".")) {
            qDebug() << "❌ 无法创建目录:" << dirPath;
            return false;
        }
    }

    // 2️⃣ 目标文件路径
    QString filePath = dir.filePath("fun3d.nml");

    // 3️⃣ 打开文件（覆盖写）
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text)) {
        qDebug() << "❌ 无法打开文件:" << filePath << "错误:" << file.errorString();
        return false;
    }

    // 4️⃣ 设置输出流
    QTextStream out(&file);
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    out.setCodec("UTF-8");              // ✅ 保证UTF-8
#endif
    out.setGenerateByteOrderMark(false); // 🚫 禁止 BOM
    out.setAutoDetectUnicode(false);
    out.setLocale(QLocale::C);          // ✅ 确保小数点格式标准
    out.setRealNumberNotation(QTextStream::FixedNotation);
    out.setRealNumberPrecision(6);

    // 5️⃣ 写入内容（统一控制换行）
    QStringList allLines;
    allLines << "&project"
             << "project_rootname = \"" + name + "\""
             << "/";

    allLines << lines;  // ✅ 追加剩余的所有块

    // ⚠️ 统一使用 "\n" 换行符，结尾不多加空行
    QString content = allLines.join("\n");
    if (!content.endsWith("/"))  // ✅ 确保最后一个 namelist 有结束符
        content.append("\n/");

    out << content;
    out.flush();
    file.close();

    return true;
}



