#include "logpage.h"

#include <QVBoxLayout>
#include <QQmlContext>
#include <QFile>
#include <QTextStream>

// 构造函数
LogPage::LogPage(QWidget *parent)
    : QWidget(parent)
{
    setupUi();

    connect(Logger::instance(), &Logger::messageUi,
            this, &LogPage::onMessageLogged);
}

// 初始化 UI：配置 model + 创建 QQuickWidget + 暴露给 QML
void LogPage::setupUi()
{
    auto *layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);

    m_model.setColumnCount(3);

    // 设定表头文字
    m_model.setHeaderData(0, Qt::Horizontal, QStringLiteral("时间"));
    m_model.setHeaderData(1, Qt::Horizontal, QStringLiteral("级别"));
    m_model.setHeaderData(2, Qt::Horizontal, QStringLiteral("内容"));

    // 创建 QQuickWidget 来加载 QML 页面
    m_quick = new QQuickWidget(this);
    m_quick->setResizeMode(QQuickWidget::SizeRootObjectToView);

    //  给QML 暴露：logModel 和 logPage
    m_quick->rootContext()->setContextProperty("logModel", &m_model);
    m_quick->rootContext()->setContextProperty("logPage",  this);

    //加载 QML 页面
    m_quick->setSource(QUrl(QStringLiteral("qrc:/src/ui/qml/LogPage.qml")));

    layout->addWidget(m_quick);
}

// 把 Logger 的 Level 转成字符串
QString LogPage::levelToString(Logger::Level level) const
{
    switch (level) {
    case Logger::Debug:   return QStringLiteral("DEBUG");
    case Logger::Info:    return QStringLiteral("INFO");
    case Logger::Warning: return QStringLiteral("WARN");
    case Logger::Error:   return QStringLiteral("ERROR");
    default:              return QStringLiteral("UNKNOWN");
    }
}

// 收到一条日志 → 往标准模型里追加一行
void LogPage::onMessageLogged(Logger::Level level,
                              const QString &text,
                              const QDateTime &timestamp)
{
    const QString timeStr  = timestamp.toString("yyyy-MM-dd-hh:mm:ss");
    const QString levelStr = levelToString(level);

    // 头插入一行
    const int row = 0;
    m_model.insertRow(row);

    //给这一行三列写数据
    m_model.setData(m_model.index(row, 0), timeStr,  Qt::DisplayRole);
    m_model.setData(m_model.index(row, 1), levelStr, Qt::DisplayRole);
    m_model.setData(m_model.index(row, 2), text,    Qt::DisplayRole);

    //自动只保留 m_maxRows 条：超过就删最旧的一行（最后一行行）
    if (m_model.rowCount() > m_maxRows) {
        m_model.removeRow(m_model.rowCount()-1); //限制行数
    }
}

// 清空所有日志（给 QML 调的）
void LogPage::clearLogs()
{
    if (m_model.rowCount() > 0)
        m_model.removeRows(0, m_model.rowCount());
}

// 导出日志到文件（给 QML 调的）
// QML 会传进一个 fileUrl，比如 "file:///C:/xxx/log.txt"
void LogPage::exportLogs(const QUrl &fileUrl)
{
    const QString path = fileUrl.toLocalFile();
    if (path.isEmpty())
        return;

    QFile file(path);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
        return;

    QTextStream out(&file);

    // 按行写出： [时间] [等级] 内容
    for (int row = 0; row < m_model.rowCount(); ++row) {
        const QString timeStr  = m_model.data(m_model.index(row, 0), Qt::DisplayRole).toString();
        const QString levelStr = m_model.data(m_model.index(row, 1), Qt::DisplayRole).toString();
        const QString textStr  = m_model.data(m_model.index(row, 2), Qt::DisplayRole).toString();

        out << "[" << timeStr << "] "
            << "[" << levelStr << "] "
            << textStr << "\n";
    }
}
