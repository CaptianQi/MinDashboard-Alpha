#ifndef SERIALPROVIDER_H
#define SERIALPROVIDER_H

#include <QObject>
#include"dataprovider.h"
#include<QSerialPort>
class SerialProvider : public DataProvider
{
    Q_OBJECT
public:
    explicit SerialProvider(QObject *parent = nullptr);


    Q_INVOKABLE bool start() override;
    Q_INVOKABLE void stop() override;
    Q_INVOKABLE bool setParams(const QJsonValue &params) override;
    Q_INVOKABLE bool isRuning() override;
private:
    //串口配置
    void setPortName(const QString &name);
    void setBaudRate(int baud);
    void setDataBits(int bits);
    void setStopBits(int bits);
    void setParity(int parity);
private slots:
    void onReayRead(); //有数据触发
private:
    QSerialPort m_serial;
    QByteArray m_buffer;
    QString m_port;
    int m_baudRate;
    int m_dataBits;
    int m_stopBits;
    int m_parity;
    bool m_isRuning = false;
};

#endif // SERIALPROVIDER_H
