#ifndef DATAPROVIDER_H
#define DATAPROVIDER_H

#include <QObject>
#include<QByteArray>
#include<QJsonValue>
class DataProvider : public QObject
{
    Q_OBJECT
public:
    explicit DataProvider(QObject *parent = nullptr): QObject(parent){}

    Q_INVOKABLE virtual bool start()=0;
    Q_INVOKABLE virtual void stop()=0;

    Q_INVOKABLE virtual bool setParams(const QJsonValue &params)=0;
    Q_INVOKABLE virtual bool isRuning()=0;

signals:
    void dataReceived(const QByteArray &frame);
    void running(bool running);
};

#endif // DATAPROVIDER_H
