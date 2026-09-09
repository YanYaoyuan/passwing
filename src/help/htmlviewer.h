#ifndef HTMLVIEWER_H
#define HTMLVIEWER_H

#include <QObject>
#include <QUrl>

class HTMLViewer : public QObject
{
    Q_OBJECT

public:
    explicit HTMLViewer(QObject *parent = nullptr);
    void openFile(const QString &filePath);
};

#endif // HTMLVIEWER_H
