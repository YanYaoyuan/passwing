#include "htmlviewer.h"
#include <QDesktopServices>
#include <QFile>
#include <QMessageBox>

HTMLViewer::HTMLViewer(QObject *parent) : QObject(parent)
{
}

void HTMLViewer::openFile(const QString &filePath)
{
    QFile file(filePath);
    if (!file.exists()) {
        QMessageBox::critical(nullptr, "Error", "File not found!");
        return;
    }
    QDesktopServices::openUrl(QUrl::fromLocalFile(filePath));
}




