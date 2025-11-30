#ifndef DATARECORDER_H
#define DATARECORDER_H

#include <QObject>
#include<QVariantMap>
#include<QFile>
#include<QTextStream>
#include<QDateTime>
/**
 * @brief The DataRecorder class
 * 负责接收DataModel的实时数据并保存到CSV/JSON文件中
 * 支持实时写入 每帧一行
 * 支持停止/清空/重新开始记录
 * 可扩展导出格式
 */
class DataRecorder : public QObject
{
    Q_OBJECT
public:
    explicit DataRecorder(QObject *parent = nullptr);
    ~DataRecorder();

    //开始 新建文件
    // 参数 : 目录 和 格式名
    Q_INVOKABLE void setParams(const QString &directory,const QString &format = "csv");
    Q_INVOKABLE bool startRecording();
    //停止记录并关闭文件
    Q_INVOKABLE void stopRecording();
    Q_INVOKABLE bool isRecording()const {return m_isRecording;}
signals:
    void recording(bool recording);
public slots:
    //接收数据模型的帧信号
    void onFrameUpdated(const QVariantMap &frame);
private:
    bool m_isRecording = false;
    QString m_dir;
    QFile m_file;
    QTextStream m_stream;
    QString m_format;
};

#endif // DATARECORDER_H
