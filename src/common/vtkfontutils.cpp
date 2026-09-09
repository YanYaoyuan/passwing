#include "common/vtkfontutils.h"

#include <QDebug>
#include <QDir>
#include <QFile>
#include <QSaveFile>
#include <QStandardPaths>

#include <vtkTextProperty.h>

namespace {

QString materializeChineseFont()
{
    static const QString extractedFontPath = []() -> QString {
        QFile resourceFile(QStringLiteral(":/language/chinese.ttf"));
        if (!resourceFile.open(QIODevice::ReadOnly)) {
            qWarning() << "Unable to open embedded Chinese font:"
                       << resourceFile.errorString();
            return {};
        }

        const QByteArray fontData = resourceFile.readAll();
        if (fontData.isEmpty()) {
            qWarning() << "Embedded Chinese font is empty";
            return {};
        }

        QString cacheRoot =
            QStandardPaths::writableLocation(QStandardPaths::CacheLocation);
        if (cacheRoot.isEmpty())
            cacheRoot =
                QDir(QDir::tempPath()).filePath(QStringLiteral("PassWing"));

        QDir cacheDirectory(cacheRoot);
        if (!cacheDirectory.mkpath(QStringLiteral("fonts"))) {
            qWarning() << "Unable to create the font cache directory:"
                       << cacheDirectory.filePath(QStringLiteral("fonts"));
            return {};
        }

        const QString targetPath =
            cacheDirectory.filePath(QStringLiteral("fonts/chinese.ttf"));

        QFile existingFont(targetPath);
        if (existingFont.open(QIODevice::ReadOnly) &&
            existingFont.readAll() == fontData) {
            return QDir::toNativeSeparators(targetPath);
        }

        QSaveFile outputFile(targetPath);
        if (!outputFile.open(QIODevice::WriteOnly) ||
            outputFile.write(fontData) != fontData.size() ||
            !outputFile.commit()) {
            qWarning() << "Unable to extract the embedded Chinese font to:"
                       << targetPath << outputFile.errorString();
            return {};
        }

        return QDir::toNativeSeparators(targetPath);
    }();

    return extractedFontPath;
}

} // namespace

namespace passwing {

bool configureVtkChineseFont(vtkTextProperty *textProperty)
{
    if (!textProperty)
        return false;

    const QString fontPath = materializeChineseFont();
    if (fontPath.isEmpty())
        return false;

    const QByteArray encodedPath = fontPath.toUtf8();
    textProperty->SetFontFamily(VTK_FONT_FILE);
    textProperty->SetFontFile(encodedPath.constData());
    return true;
}

} // namespace passwing
