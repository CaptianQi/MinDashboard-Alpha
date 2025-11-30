#ifndef DATAMANAGER_H
#define DATAMANAGER_H

#include <QObject>
#include<QVariantMap>
#include<QJsonValue>
#include<QList>
#include"../middle/StructType.h"
class DataModel;
class FramParser;
class DataProvider;
class ReadProjectConfig;
class DataRecorder;
class QThread;
/**
 * @brief The DataManager class
 * 全局管理单例
 * 统一分发数据
 */
class DataManager : public QObject
{
    Q_OBJECT
public:
    explicit DataManager(QObject *parent = nullptr);
    ~DataManager();

    static DataManager* instance();
    //设置当前数据源
    void setProvider(const QJsonValue &params);
    //连接
    void startProvider();
    //断开数据源
    void closeProvider();
    //连接状态
    void providerIsRunning();

    //读取项目
    void setReadProject(const QJsonValue &params);
    //发送项目文件记录
    void getProject();

    //记录开关
    void setRecorderParams(const QJsonValue &params);
    void startRecorder();
    //停止记录
    void stopRecorder();
    //录制状态
    void recorderIsRunning();


    //获取数据模型
    DataModel* model() const {return m_model;}

signals:
    //前端数据接口
    void dataUpdated(const QVariantMap &data);  //整帧数据
    void valueUpdated(const QString &key,const QVariant &value);//单个数据
    void projectData(const QList<FieldConfig> &fields); //项目文件配置
private:
    DataProvider *m_provider = nullptr;  //数据接口
    FramParser *m_parser = nullptr;  //数据解析
    DataModel *m_model = nullptr;  //数据模型
    ReadProjectConfig *m_rpc=nullptr;   //读取项目
    DataRecorder *m_dataRecorder = nullptr; //记录
    QThread *m_diskThread= nullptr; //记录线程
    QThread *m_ioThread = nullptr;  //数据源线程
    QString m_mode;//模式
};

#endif // DATAMANAGER_H
