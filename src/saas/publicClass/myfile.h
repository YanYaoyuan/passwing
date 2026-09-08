#ifndef MYFILE_H
#define MYFILE_H
#include <QString>
#include <QStringList>

class myFile
{
public:
    myFile() = default;
    // 写 QStringList 到文件
    static bool writeTextFile(const QString &filePath, const QStringList &lines,const QString&);


};

#endif // MYFILE_H
