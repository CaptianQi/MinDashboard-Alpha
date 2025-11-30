#ifndef LOGWORKER_H
#define LOGWORKER_H

#include <QObject>
#include<QFile>
#include<QTextStream>
#include<QQueue>
#include<QMutex>
#include<QWaitCondition>
#include<QDate>

/**
 * @brief The LogWorker class
 *      日志线程
 *   异步写入文件
 *   上锁共享资源
 */
class LogWorker : public QObject
{
    Q_OBJECT
public:
    explicit LogWorker(QObject *parent = nullptr);
    ~LogWorker();
public slots:
    void enqueueLine(const QString &line);

    void stopWork();

    void configure(const QString &directory,
                   const QString &filePrefix,
                   int maxDays,
                   bool rotateDaily,
                   qint64 maxFileSizeBytes);
private:
    void openNewLogFile(bool newDay);
    void cleanupOldFiles();
    QString logFilePathForDate(const QDate &date,int index)const;
    void initIndexForToday();

private:
    QQueue<QString> m_queue;
    QMutex m_mutex;
    bool m_running = false;

    QString m_directory;
    QString m_filePrefix;
    int m_maxDays = 7;
    bool m_rotateDaily = true;
    qint64 m_maxFileSizeBytes = 0;

    QFile m_file;
    QTextStream m_stream;
    QDate m_currentDate;
    int m_currentIndex = 0;  //当天第几个文件
};

#endif // LOGWORKER_H
