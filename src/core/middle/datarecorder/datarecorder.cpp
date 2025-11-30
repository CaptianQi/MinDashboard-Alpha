#include "datarecorder.h"

#include<QVariantMap>
#include<QDir>
#include<QJsonDocument>
#include<QJsonObject>
#include"../../log/logger.h"

DataRecorder::DataRecorder(QObject *parent)
    : QObject{parent}
{}


DataRecorder::~DataRecorder()
{
    stopRecording();
}

void DataRecorder::setParams(const QString &directory, const QString &format)
{
    m_format =format.toLower();
    m_dir = directory;
}

bool DataRecorder::startRecording()
{
    //先关闭文件
    stopRecording();
    if(m_dir.isEmpty()){
        emit recording(false);
        LOG_WARN(QStringLiteral("未设置文件路径"));
        return false;
    }
    //目录
    QDir dir(m_dir);
    if(!dir.exists())
        dir.mkpath(m_dir);
    //时间戳
    QString timestamp =QDateTime::currentDateTime().toString("yyyy-MM-dd_hh-mm-ss");
    //文件名设置
    QString fileName = QString("%1/data_%2.%3")
        .arg(m_dir,timestamp,m_format);
    m_file.setFileName(fileName);
    if(!m_file.open(QIODevice::WriteOnly|QIODevice::Text)){
        LOG_WARN(QStringLiteral("无法打开记录文件：%1")
                 .arg(m_file.errorString()));
        emit recording(false);
        //Todo: 抛出异常
        return false;
    }

    m_stream.setDevice(&m_file);

    if(m_format=="csv"){
        m_stream<<timestamp;
    }
    m_isRecording = true;

    LOG_INFO(QStringLiteral("开始记录数据：%1")
             .arg(fileName));
    emit recording(true);
    return true;
}

void DataRecorder::stopRecording()
{
    if(m_isRecording){
        LOG_INFO(QStringLiteral("停止记录：%1")
                 .arg(m_file.fileName()));
        m_isRecording = false;
        m_stream.flush();
        m_file.close();
        emit recording(false);
    }
}

void DataRecorder::onFrameUpdated(const QVariantMap &frame)
{
    if(!m_isRecording) return;

    const QString timestamp = QDateTime::currentDateTime()
        .toString("hh:mm:ss.zzz");

    if(m_format=="csv"){
        //第一次写表头
        static bool headerWritten = false;
        if(!headerWritten){
            m_stream<<","<<frame.keys().join(",")<<"\n";
            headerWritten = true;
        }
        QStringList values;
        for(const QString &key: frame.keys()){
            //读取所有值
            values << frame.value(key).toString();
        }
        m_stream << timestamp <<","<<values.join(",")<<"\n";
    }else if(m_format=="json"){
        QJsonObject obj = QJsonObject::fromVariantMap(frame);
        obj.insert("timestamp",timestamp);
        QJsonDocument doc(obj);
        m_stream<<doc.toJson(QJsonDocument::Compact)<<"\n";
    }
    m_stream.flush();
}
