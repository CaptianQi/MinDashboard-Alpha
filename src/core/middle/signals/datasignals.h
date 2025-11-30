#ifndef DATASIGNALS_H
#define DATASIGNALS_H

#include <QObject>
#include<QVariantMap>

class QString;
class QVariant;
class DataSignals : public QObject
{
    Q_OBJECT
public:
    explicit DataSignals(QObject *parent = nullptr);

    static DataSignals *instance();

signals:
    void dataUpdated(const QVariantMap &data);  //整帧数据
    void valueUpdated(const QString &key,const QVariant &value);//单个数据
    void providerState(bool running);  //通信连接状态
    void recorderState(bool recording);  //录制状态
};

#endif // DATASIGNALS_H
