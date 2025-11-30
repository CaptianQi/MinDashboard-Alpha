#ifndef GATEWAY_H
#define GATEWAY_H

#include <QObject>
#include<QJsonValue>
class GateWay : public QObject
{
    Q_OBJECT
public:
    explicit GateWay(QObject *parent = nullptr);

    static GateWay* instance();

    void send(int api,const QJsonValue& value=QJsonValue());

private:
    //转发
    void dispach(int api,const QJsonValue& value);

    //接口
    void apiSetProvider(const QJsonValue& params);
    void apiStartProvider();
    void apiCloseProvider();

    void apiSetReadProject(const QJsonValue &params);
    //发送项目文件记录
    void apiGetProject();

    //记录开关
    void apiSetRecorder(const QJsonValue &params);
    void apiStartRecorder();
    //停止记录
    void apiStopRecorder();
};

#endif // GATEWAY_H
