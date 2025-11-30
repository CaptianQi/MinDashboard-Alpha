#ifndef NETWORKPROVIDER_H
#define NETWORKPROVIDER_H

#include <QObject>
#include"dataprovider.h"
#include<QTcpSocket>
#include<QUdpSocket>
#include<QByteArray>
class NetworkProvider : public DataProvider
{
    Q_OBJECT
public:
    explicit NetworkProvider(QObject *parent = nullptr);
    ~NetworkProvider();

    Q_INVOKABLE bool start() override;
    Q_INVOKABLE void stop() override;
    Q_INVOKABLE bool setParams(const QJsonValue &params) override;
    Q_INVOKABLE bool isRuning() override;
private:
    void setTcpMode(const QString &host,quint16 port);
    void setUdpMode(quint16 port);
private slots:
    void onTcpReadyRead();
    void onUdpReadyRead();

private:
    bool m_useUdp = false;
    QTcpSocket m_tcp;
    QUdpSocket m_udp;
    QString m_host;
    quint16 m_port=0;
    QByteArray m_tcpBuffer;
    bool m_isRuning = false;

};

#endif // NETWORKPROVIDER_H
