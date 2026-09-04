#include "languageapp.h"
#include <QObject>
#include <QMap>
static bool g_isEnglish = false;  // 全局标志：false=中文，true=英文

languageApp::languageApp() {}

void languageApp::setLanguage(bool english)
{
    g_isEnglish = english;
}

QString languageApp::trText(const QString &key)
{
    static QMap<QString, QPair<QString, QString>> map = {
        {"Setting",             {QObject::tr("设置"), "Setting"}},
        {"Grid Properties",     {QObject::tr("网格属性"), "Grid Properties"}},
        {"Reference Values",    {QObject::tr("参考值"), "Reference Values"}},
        {"Volume Output",       {QObject::tr("体积输出变量"), "Volume Output"}},
        {"Boundary Output",     {QObject::tr("边界输出变量"), "Boundary Output"}},
        {"Solution Settings",   {QObject::tr("求解设置"), "Solution Settings"}},
        {"Equations",           {QObject::tr("控制方程"), "Equations"}},
        {"Reference Physics",   {QObject::tr("参考物理属性"), "Reference Physics"}},
        {"Flux Method",         {QObject::tr("无粘通量方法"), "Flux Method"}},
        {"Turbulent Diffusion", {QObject::tr("湍流扩散模型"), "Turbulent Diffusion"}},
        {"Nonlinear Solver",    {QObject::tr("非线性求解器参数"), "Nonlinear Solver"}},
        {"Linear Solver",       {QObject::tr("线性求解器参数"), "Linear Solver"}},
        {"Run Simulation",      {QObject::tr("开始计算"), "Run Simulation"}},
        {"Results",             {QObject::tr("结果与后处理"), "Results"}}
    };

    if (!map.contains(key))
        return key;

    return g_isEnglish ? map[key].second : map[key].first;
}

