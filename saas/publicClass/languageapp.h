#ifndef LANGUAGEAPP_H
#define LANGUAGEAPP_H

#include <QString>

class languageApp
{
public:
    languageApp();

    // 设置语言（true=英文, false=中文）
    static void setLanguage(bool english);

    // 获取翻译
    static QString trText(const QString &key);
};

#endif // LANGUAGEAPP_H


