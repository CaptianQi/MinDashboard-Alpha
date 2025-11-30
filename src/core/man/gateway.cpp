#include "gateway.h"
Q_GLOBAL_STATIC(GateWay,ins)

#include"../../config/apis.h"
#include"datamanager.h"
GateWay::GateWay(QObject *parent)
    : QObject{parent}
{}

GateWay *GateWay::instance()
{
    return ins();
}

void GateWay::send(int api, const QJsonValue &value)
{
    //todo: 可以做异常捕获
    this->dispach(api,value);
}

void GateWay::dispach(int api, const QJsonValue &value)
{
    switch(api){
    case API::PROVIDER::SETP:
        apiSetProvider(value);
        break;
    case API::PROVIDER::STARTP:
        apiStartProvider();
        break;
    case API::PROVIDER::CLOSEP:
        apiCloseProvider();
        break;

    case API::RECORDER::SET:
        apiSetRecorder(value);
        break;
    case API::RECORDER::START:
        apiStartRecorder();
        break;
    case API::RECORDER::STOPR:
        apiStopRecorder();
        break;
    }
}

void GateWay::apiSetProvider(const QJsonValue &params)
{
    DataManager::instance()->setProvider(params);
}

void GateWay::apiStartProvider()
{
    DataManager::instance()->startProvider();
}

void GateWay::apiCloseProvider()
{
    DataManager::instance()->closeProvider();
}

void GateWay::apiSetReadProject(const QJsonValue &params)
{

}

void GateWay::apiGetProject()
{

}

void GateWay::apiSetRecorder(const QJsonValue &params)
{
    DataManager::instance()->setRecorderParams(params);
}

void GateWay::apiStartRecorder()
{
    DataManager::instance()->startRecorder();
}

void GateWay::apiStopRecorder()
{
    DataManager::instance()->stopRecorder();
}
