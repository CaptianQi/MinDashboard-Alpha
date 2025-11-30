#include "logger.h"

Q_GLOBAL_STATIC(Logger,ins)

#include<QFile>
#include<QDebug>
#include<QByteArray>
#include<QJsonDocument>
#include<QJsonObject>
#include<QJsonParseError>
#include<QFileInfo>
#include<QCoreApplication>
Logger::Logger(QObject *parent)
    : QObject{parent}
{

    // 创建并启动日志工作线程
    m_worker = new LogWorker();  //不指定父类，要交给线程
    m_worker->moveToThread(&m_workerThread);

    // 写日志请求 -> 工作线程入队
    connect(this, &Logger::writeRequested,
            m_worker, &LogWorker::enqueueLine);

    // 线程结束
    connect(&m_workerThread, &QThread::finished,
            m_worker, &LogWorker::stopWork);

    // 初始化配置（目录 / 前缀 / 保留天数 / 每日轮转 / 大小轮转）
    connect(this, &Logger::workerConfigure,
            m_worker, &LogWorker::configure);

    m_workerThread.start();
}


Logger *Logger::instance()
{
    return ins();
}

bool Logger::initFromJson(const QString &configPath)
{
    //读取配置
    QFile file(configPath);
    if(!file.open(QIODevice::ReadOnly|QIODevice::Text)){
        qDebug()<< "Logger: cannot open config.json";

        return false;
    }

    const QByteArray data = file.readAll();
    file.close();

    QJsonParseError err;
    const QJsonDocument doc = QJsonDocument::fromJson(data,&err);
    if(err.error!=QJsonParseError::NoError||!doc.isObject()){
        qDebug()<< "Logger: invalid log config:"<<err.errorString();
        return false;
    }

    const QJsonObject root = doc.object();
    const QJsonObject logging = root.value("logging").toObject();

    const QString levelStr = logging.value("level").toString();
    if(levelStr=="TRACE")
        m_minLevel = Trace;
    else if(levelStr=="DEBUG") m_minLevel = Debug;
    else if(levelStr=="INFO") m_minLevel = Info;
    else if(levelStr=="WARNING") m_minLevel = Warning;
    else if(levelStr=="ERROR") m_minLevel = Error;
    else if(levelStr=="FATAL") m_minLevel = Fatal;
    else    m_minLevel = Info;

    m_logToConsole = logging.value("logToConsole").toBool(true);

    QString directory = logging.value("directory").toString("logs");
    QString filePrefix = logging.value("filePrefix").toString("app");
    bool rotateDaily = logging.value("rotateDaily").toBool(true);
    int maxDays = logging.value("maxDays").toInt(7);
    double maxFileSizeMB = logging.value("maxFileSizeMB").toDouble(5.0);
    qint64 maxFileSizeBytes = static_cast<qint64>(maxFileSizeMB * 1024.0 * 1024.0);

    //设置路径
    QFileInfo dirInfo(directory);
    if(dirInfo.isRelative()){
        directory = QCoreApplication::applicationDirPath()+"/"+directory;
    }

    emit workerConfigure(directory,filePrefix,maxDays,rotateDaily,maxFileSizeBytes);

    return true;
}

void Logger::log(Level level, const QString &file, int line, const char *function, const QString &message)
{
    if(level<m_minLevel) return;

    const QDateTime now = QDateTime::currentDateTime();
    const QString timeStr = now.toString("yyyy-MM-dd hh:mm:ss.zzz");

    // 获取线程 ID
    quintptr threadId = reinterpret_cast<quintptr>(QThread::currentThreadId());

    QFileInfo fi(file);
    QString filePart = fi.fileName();


    const QString lineText =
        QStringLiteral("%1 [%2] [T%3] [%4] (%5:%6) %7")
        .arg(timeStr)
        .arg(levelToString(level))
        .arg(threadId,0,16)
        .arg(QString::fromUtf8(function))
        .arg(filePart)
        .arg(line)
        .arg(message);

    if(m_logToConsole){
        qDebug().noquote()<<lineText;
    }

    emit messageUi(level,message,now);

    emit writeRequested(lineText);



}


QString Logger::levelToString(Level level) const
{
    switch(level){
    case Trace: return "TRACE";
    case Debug: return "DEBUG";
    case Info:  return "INFO";
    case Warning:  return "WARN";
    case Error: return "ERROR";
    case Fatal: return "FATAL";
    }
    return "UNKNOWN";
}

Logger::~Logger()
{
    m_workerThread.quit();
    m_workerThread.wait();
}

