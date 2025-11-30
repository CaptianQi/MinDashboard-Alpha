#include "logworker.h"


#include<QMutexLocker>
#include<QStandardPaths>
#include<QDir>
LogWorker::LogWorker(QObject *parent)
    : QObject{parent}
{}


void LogWorker::enqueueLine(const QString &line)
{
    if (line.isEmpty())
        return;

    QMutexLocker locker(&m_mutex);

    // 第一次写日志时，初始化当前文件
    if (!m_file.isOpen()) {
        initIndexForToday();      // 根据已有文件算出当前 index
        openNewLogFile(true);     // 创建 / 打开当天的日志文件
        cleanupOldFiles();        // 清理过期文件
    }

    // 1) 每日轮转：日期变了就开新文件
    const QDate today = QDate::currentDate();
    if (m_rotateDaily && (!m_currentDate.isValid() || today != m_currentDate)) {
        openNewLogFile(true);
        cleanupOldFiles();
    }

    // 2) 大小轮转：超过指定大小就开新文件
    if (m_maxFileSizeBytes > 0 && m_file.isOpen()) {
        if (m_file.size() >= m_maxFileSizeBytes) {
            openNewLogFile(false);  // 同一天 index++ 切下一文件
            cleanupOldFiles();
        }
    }

    // 3) 真正写入一行
    if (m_file.isOpen()) {
        m_stream << line << '\n';
        m_stream.flush();
    }
}



LogWorker::~LogWorker()
{
    stopWork();
}

void LogWorker::stopWork()
{
    QMutexLocker locker(&m_mutex);
    if (m_file.isOpen()) {
        m_stream.flush();
        m_file.close();
    }
}

void LogWorker::configure(const QString &directory, const QString &filePrefix, int maxDays, bool rotateDaily, qint64 maxFileSizeBytes)
{
    QMutexLocker locker(&m_mutex);
    m_directory = directory;
    m_filePrefix = filePrefix;
    m_maxDays = maxDays;
    m_rotateDaily = rotateDaily;
    m_maxFileSizeBytes = maxFileSizeBytes;

    // 配置改变时，可以选择关闭当前文件，让下一条日志重新打开
    if (m_file.isOpen()) {
        m_stream.flush();
        m_file.close();
        m_currentDate = QDate();   // 让下次写入时重新 init
        m_currentIndex = 0;
    }
}

void LogWorker::openNewLogFile(bool newDay)
{
    if(m_file.isOpen()){
        m_stream.flush();
        m_file.close();
    }

    if(newDay||m_currentDate.isNull()){
        m_currentDate = QDate::currentDate();
        m_currentIndex = 0;
    }else{
        m_currentIndex++;
    }

    const QString path = logFilePathForDate(m_currentDate,m_currentIndex);

    QFileInfo info(path);
    QDir dir = info.dir();
    if(!dir.exists()){
        dir.mkpath(".");
    }

    m_file.setFileName(path);
    if(!m_file.open(QIODevice::Append|QIODevice::Text)){
        qDebug()<<"LogWorker: cannot open log file:"<<path;
        return;
    }

#if QT_VERSION >=QT_VERSION_CHECK(5,15,0)
    m_stream.setEncoding(QStringConverter::Utf8);
#endif
    m_stream.setDevice(&m_file);

    qDebug()<<"LogWorker: using log file:"<<path;
}

void LogWorker::cleanupOldFiles()
{
    if(m_maxDays<=0){
        return;
    }

    QString dirPath = m_directory;
    if(dirPath.isEmpty()){
        const QString basePath =
            QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
        dirPath = basePath+"/logs";
    }

    QDir dir(dirPath);
    if(!dir.exists()){
        return;
    }

    const QFileInfoList files = dir.entryInfoList(
        QStringList()<<"*.log",
        QDir::Files |QDir::NoSymLinks,
        QDir::Time  //最近修改时间降序
        );

    const QDate today = QDate::currentDate();
    const QDate cutoff = today.addDays(-(m_maxDays-1));

    for(const QFileInfo &fi:files){
        const QDate modifiedDate = fi.lastModified().date();
        if(modifiedDate<cutoff){
            dir.remove(fi.fileName());
        }
    }
}

QString LogWorker::logFilePathForDate(const QDate &date, int index) const
{
    QString dirPath = m_directory;
    if(dirPath.isEmpty()){
        const QString basePath =
            QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
        dirPath = basePath +"/logs";
    }

    QDir dir(dirPath);
    QString prefix = m_filePrefix.isEmpty()?QStringLiteral("app"):m_filePrefix;

    QString fileName;
    if(m_rotateDaily){
        fileName = QString("%1-%2-%3.log")
            .arg(prefix,
                  date.toString("yyyy-MM-dd"),
        QString("%1").arg(index,3,10,QChar('0')));
    }else{
        fileName = QString("%1-%2.log")
            .arg(prefix,
          QString("%1").arg(index, 3, 10, QChar('0')));
    }

    return dir.filePath(fileName);
}

void LogWorker::initIndexForToday()
{
    QDate today = QDate::currentDate();

    // 1. 决定日志目录（和你其他地方保持一致）
    QString dirPath = m_directory;
    if (dirPath.isEmpty()) {
        const QString basePath =
            QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
        dirPath = basePath + "/logs";
    }

    QDir dir(dirPath);
    if (!dir.exists())
        return; // 没有目录 = 没有日志文件，直接让后面的逻辑从 0 开始就行

    QString prefix = m_filePrefix.isEmpty() ? QStringLiteral("app") : m_filePrefix;

    // 2. 根据 rotateDaily 决定匹配模式
    QString pattern;
    if (m_rotateDaily) {
        // 带日期：app-2025-11-21-000.log
        pattern = QString("%1-%2-*.log")
                      .arg(prefix,
                           today.toString("yyyy-MM-dd"));
    } else {
        // 只按索引轮转：app-000.log、app-001.log ...
        pattern = QString("%1-*.log").arg(prefix);
    }

    QFileInfoList list = dir.entryInfoList(
        QStringList() << pattern,
        QDir::Files | QDir::NoSymLinks,
        QDir::Name    // 按文件名排序：000,001,002...
        );

    if (list.isEmpty())
        return; // 今天还没有日志文件，从 0 开始

    // 3. 找出最大 index
    int maxIndex = -1;
    for (const QFileInfo &fi : list) {
        // 文件名：
        //  - rotateDaily=true:  app-2025-11-21-003.log → baseName: app-2025-11-21-003
        //  - rotateDaily=false: app-003.log            → baseName: app-003
        const QString base = fi.completeBaseName();
        const QStringList parts = base.split("-");

        if (parts.isEmpty())
            continue;

        // 取最后一段作为 index
        bool ok = false;
        int idx = parts.last().toInt(&ok);
        if (!ok)
            continue;

        if (idx > maxIndex)
            maxIndex = idx;
    }

    if (maxIndex < 0)
        return; // 匹配到文件但都没解析出 index，

    m_currentIndex = maxIndex;

    if (m_rotateDaily) {
        m_currentDate = today;
    }
}
