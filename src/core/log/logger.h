#ifndef LOGGER_H
#define LOGGER_H

#include <QObject>
#include<QDateTime>
#include<QThread>
#include<QString>
#include"logworker.h"
#define LOG_INTERNAL(level,msg)  \
        Logger::instance()->log(level,QString::fromUtf8(__FILE__),__LINE__,Q_FUNC_INFO,(msg))

#define LOG_TRACE(msg)  LOG_INTERNAL(Logger::Trace,msg)
#define LOG_DEBUG(msg)  LOG_INTERNAL(Logger::Debug,msg)
#define LOG_INFO(msg)    LOG_INTERNAL(Logger::Info,msg)
#define LOG_WARN(msg)    LOG_INTERNAL(Logger::Warning,msg)
#define LOG_ERROR(msg)   LOG_INTERNAL(Logger::Error,msg)
#define LOG_FATAL(msg)   LOG_INTERNAL(Logger::Fatal,msg)


/**
 * @brief The Logger class
 *      日志系统
 *  根据配置文件，进行日志记录
 *  通过 LogWorker异步写文件
 *  有日志等级过滤，日志文件路径，日志轮转等控制
 *  全局单例
 */
class Logger : public QObject
{
    Q_OBJECT
public:
    explicit Logger(QObject *parent = nullptr);
    ~Logger();

    static Logger* instance();

    enum Level{
        Trace = 0,
        Debug,
        Info,
        Warning,
        Error,
        Fatal
    };
    Q_ENUM(Level)

    bool initFromJson(const QString &configPath);

    void setMinLevel(Level level){ m_minLevel = level;}
    Level getMinLevel()const{return m_minLevel;}

    void setLogToConsole(bool on){m_logToConsole = on;}
    bool getLogToConsole()const{return m_logToConsole;}

    void log(Level level,
             const QString &file,
             int line,
             const char *function,
             const QString &message);


    // void logTrace(const QString &msg);
    // void logDebug(const QString &msg);
    // void logInfo(const QString &msg);
    // void logWarning(const QString &msg);
    // void logError(const QString &msg);
    // void logFatal(const QString &msg);


private:
    QString levelToString(Level level) const;

private:
    Level m_minLevel = Info;  //日志等级
    bool  m_logToConsole = true;    //输出到控制台

    QThread   m_workerThread;
    LogWorker *m_worker = nullptr;


signals:
    void messageUi(Logger::Level level,
                   const QString &text,
                   const QDateTime &timestamp);  //发送给UI
    void writeRequested(const QString &line);
    void workerConfigure(const QString &directory,
                         const QString &filePrefix,
                         int maxDays,
                         bool rotateDaily,
                         qint64 maxFileSizeBytes);
};

#endif // LOGGER_H
