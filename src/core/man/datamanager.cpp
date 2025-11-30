#include "datamanager.h"
Q_GLOBAL_STATIC(DataManager,ins)

#include<QThread>
#include<QMetaObject>
#include<QDebug>
#include<QJsonObject>
#include "../log/logger.h"
#include"../helper/framparser.h"
#include"../middle/model/datamodel.h"
#include"../dao/dataprovider.h"
#include"../dao/networkprovider.h"
#include"../dao/serialprovider.h"
#include"../middle/datarecorder/datarecorder.h"
#include"../middle/readproject/readprojectconfig.h"
#include"../middle/signals/datasignals.h"
DataManager::DataManager(QObject *parent)
    : QObject{parent}
{
    m_parser = new FramParser(this);
    m_model = new DataModel(this);
    m_rpc = new ReadProjectConfig(this);
    m_dataRecorder = new DataRecorder;
    //  FramParser --> DataModel
    connect(m_parser,&FramParser::frameParsed,
            m_model,&DataModel::updateFrame);
    //  DataModel -->  DataManager   整帧数据
    connect(m_model,&DataModel::frameUpdated,
            DataSignals::instance(),&DataSignals::dataUpdated);
    connect(m_model,&DataModel::valueChanged,
            DataSignals::instance(),&DataSignals::valueUpdated);

    m_diskThread = new QThread(this);
    m_ioThread =new QThread(this);

    m_dataRecorder->moveToThread(m_diskThread);
    connect(m_diskThread, &QThread::finished,
            m_dataRecorder, &QObject::deleteLater);
    connect(m_dataRecorder,&DataRecorder::recording,
            DataSignals::instance(),&DataSignals::recorderState);
    m_ioThread->start();
    m_diskThread->start();
}


DataManager::~DataManager()
{
    if(m_provider) {
        QMetaObject::invokeMethod(m_provider, "stop", Qt::QueuedConnection);
        m_provider->deleteLater();
    }

    if(m_ioThread) {
        m_ioThread->quit();
        m_ioThread->wait();
    }

    if(m_diskThread) {
        m_diskThread->quit();
        m_diskThread->wait();
    }
}

DataManager *DataManager::instance()
{
    return ins();
}

//设置用
void DataManager::setProvider(const QJsonValue &params)
{
    //先销毁旧连接
    if(m_provider){
        QMetaObject::invokeMethod(m_provider,"stop",
                                Qt::QueuedConnection);

        m_provider->disconnect();
        //延时销毁，等事件循环空闲后销毁
        m_provider->deleteLater();
        m_provider=nullptr;
    }
    const QString mode = params["mode"].toString();
    m_mode = mode;
    if(mode==QString("SerialPort")){
        m_provider = new SerialProvider;
    }else if(mode == QString("TCP")||mode==QString("UDP")){
        m_provider = new NetworkProvider;
    }else{
        LOG_ERROR(QStringLiteral("通信参数错误：mode : %1")
                      .arg(mode));
        qDebug()<< mode;
        //Todo: 抛出异常
        return;
    }
    m_provider->moveToThread(m_ioThread);
    //连接数据解析   DataProvider  ->  FramParser
    connect(m_provider,&DataProvider::dataReceived,
                m_parser,&FramParser::parseFrame,
            Qt::QueuedConnection);
    connect(m_provider,&DataProvider::running,
                DataSignals::instance(),&DataSignals::providerState);


    bool ok = QMetaObject::invokeMethod(m_provider,"setParams",
                            Qt::QueuedConnection,
                            Q_ARG(QJsonValue,params));
    if(!ok){
        //Todo: 异常
        return;
    }

}

void DataManager::startProvider()
{
    if(!m_provider){
        LOG_ERROR(QStringLiteral("未设置 provider"));
        return;
    }
    bool invokeOk = QMetaObject::invokeMethod(m_provider,"start",
                    Qt::QueuedConnection);
    if(!invokeOk){
        LOG_ERROR(QStringLiteral("启动是失败：invokeMethod 调用失败"));
        return;
    }
}

void DataManager::closeProvider()
{
    if(!m_provider){
        LOG_ERROR(QStringLiteral("未设置 provider"));
        return;
    }
    QMetaObject::invokeMethod(m_provider, "stop",
                              Qt::QueuedConnection);
    LOG_INFO(QStringLiteral("关闭连接"));

}

void DataManager::providerIsRunning()
{
    bool running  = false;
    if(!m_provider){
        running =false;
    }else{
        running = QMetaObject::invokeMethod(m_provider,"isRuning",
                        Qt::QueuedConnection);
    }
    LOG_INFO(QStringLiteral("运行状态：%1").arg(running));
    emit DataSignals::instance()->providerState(running);
}

void DataManager::setReadProject(const QJsonValue &params)
{
    QString filePath = params["filePath"].toString().trimmed();
    if(m_rpc){
        bool ok = m_rpc->loadFromFile(filePath);
        if(!ok){
            //TODo: 抛出异常
            LOG_ERROR(QStringLiteral("打开项目文件异常"));
        }
        LOG_INFO(QStringLiteral("已经打开项目文件"));
    }
}

void DataManager::getProject()
{
    QList<FieldConfig> fields = m_rpc->fields();
    emit projectData(fields);

}
// 目录和格式
void DataManager::setRecorderParams(const QJsonValue &params)
{
    const QString dir =params["directory"].toString().trimmed();
    const QString format = params["format"].toString("csv").trimmed();   //默认CSV
    QMetaObject::invokeMethod(
        m_dataRecorder, "setParams",
        Qt::QueuedConnection,
        Q_ARG(QString, dir),
        Q_ARG(QString, format));
}

void DataManager::startRecorder()
{
    connect(m_model,&DataModel::frameUpdated,
            m_dataRecorder,&DataRecorder::onFrameUpdated,
            Qt::QueuedConnection);
    bool ok = QMetaObject::invokeMethod(
                    m_dataRecorder, "startRecording",
                    Qt::QueuedConnection);
    if(!ok){
        //TODo: 抛出异常
        LOG_ERROR(QStringLiteral("启动是失败：invokeMethod 调用失败"));
        //todo:emit
        return;
    }
    //todo:emit
}

void DataManager::stopRecorder()
{
    if (!m_dataRecorder){
        LOG_ERROR(QStringLiteral("数据记录对象不存在"));
        return;
    }
    QMetaObject::invokeMethod(m_dataRecorder, "stopRecording",
                            Qt::QueuedConnection
                            );
    disconnect(m_model,&DataModel::frameUpdated,
            m_dataRecorder,&DataRecorder::onFrameUpdated);
    LOG_INFO(QStringLiteral("停止记录"));
    //todo:emit
}

void DataManager::recorderIsRunning()
{
    bool running  = false;
    if(!m_dataRecorder){
        running =false;
    }else{
        running = QMetaObject::invokeMethod(m_dataRecorder,"isRecording",
                                            Qt::QueuedConnection);
    }
    emit DataSignals::instance()->recorderState(running);
}

