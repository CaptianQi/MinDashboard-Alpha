#ifndef LOGPAGE_H
#define LOGPAGE_H

#include <QWidget>
#include <QStandardItemModel>
#include <QQuickWidget>
#include <QDateTime>
#include <QUrl>

#include "../../core/log/logger.h"

class LogPage : public QWidget
{
    Q_OBJECT
public:
    explicit LogPage(QWidget *parent = nullptr);

    // 这两个给 QML 调用（比如“清空”、“导出”），
    // 注意：要在 mainwindow 里用 QQuickWidget 的 rootContext 暴露 logPage 给 QML 才能用。
    Q_INVOKABLE void clearLogs();
    Q_INVOKABLE void exportLogs(const QUrl &fileUrl);

private slots:
    // 接收 Logger 的日志信号
    void onMessageLogged(Logger::Level level,
                         const QString &text,
                         const QDateTime &timestamp);

private:
    void setupUi();   // 初始化 QQuickWidget 和 model
    QString levelToString(Logger::Level level) const;

private:
    QQuickWidget       *m_quick = nullptr;   // 用来装 QML 的壳
    QStandardItemModel  m_model;             // Qt 自带的标准模型
    const int           m_maxRows = 300;     // 最多保存 300 条日志
};

#endif // LOGPAGE_H
