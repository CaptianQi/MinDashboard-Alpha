#include "networkprovider.h"
#include"../log/logger.h"

#include<QString>
#include<QJsonValue>
#include<QJsonObject>
NetworkProvider::NetworkProvider(QObject *parent)
    : DataProvider{parent}
{}


NetworkProvider::~NetworkProvider()
{
    m_isRuning = false;
    m_tcp.disconnect();
    m_udp.disconnect();
}



void NetworkProvider::setTcpMode(const QString &host, quint16 port)
{
    m_useUdp = false;
    m_host = host;
    m_port = port;
}



void NetworkProvider::setUdpMode(quint16 port)
{
    m_useUdp = true;
    m_port = port;
}



bool NetworkProvider::start()
{
    //先判断是什么模式
    if(m_useUdp){
        //UDP模式
        if(m_udp.state()!=QAbstractSocket::UnconnectedState)
            m_udp.close();
                            //允许多个程序用一个端口， 允许端口快速重用
        bool ok = m_udp.bind(QHostAddress::Any,m_port,
                            QUdpSocket::ShareAddress|QUdpSocket::ReuseAddressHint);
        if(!ok){
            LOG_ERROR(QStringLiteral("UDP绑定失败：%1 错误：%2")
                          .arg(m_port)
                          .arg(m_udp.errorString()));
            m_udp.close();
            m_isRuning = false;
            //Todo: 抛出异常
            emit running(m_isRuning);
            return false;
        }
        connect(&m_udp,&QUdpSocket::readyRead,
                this,&NetworkProvider::onUdpReadyRead);

        LOG_INFO(QStringLiteral("UDP 监听启动：端口号 %1")
                 .arg(m_port));
        m_isRuning = true;  //更新状态
       // return true;
    }else{
        //TCP模式
        if(m_tcp.state()!=QAbstractSocket::UnconnectedState)
            m_tcp.disconnectFromHost();
        m_tcp.connectToHost(m_host,m_port);
        bool ok = m_tcp.waitForConnected(2000);
        if(!ok){
            LOG_ERROR(QStringLiteral("TCP连接失败：%1 %2,错误 %3")
                      .arg(m_host)
                      .arg(m_port)
                      .arg(m_tcp.errorString()));
            m_tcp.abort();  //强制断开
            m_isRuning =false;
            //Todo: 抛出异常
            emit running(m_isRuning);
            return false;
        }
        connect(&m_tcp,&QTcpSocket::readyRead,
                this,&NetworkProvider::onTcpReadyRead);
        LOG_INFO(QStringLiteral("TCP 连接成功：%1:%2")
                 .arg(m_host)
                 .arg(m_port));
        m_isRuning = true;
        //return true;
    }
    emit running(m_isRuning);
    return true;
}

void NetworkProvider::stop()
{
    if(m_useUdp){
        if(m_udp.state()!=QAbstractSocket::UnconnectedState){
            LOG_INFO(QStringLiteral("UDP 停止监听：端口%1")
                     .arg(m_port));
            m_udp.close();
            m_isRuning = false;
        }
    }else{
        if(m_tcp.state()!=QAbstractSocket::UnconnectedState){
            LOG_INFO(QStringLiteral("TCP 断开连接：%1:%2")
                     .arg(m_host)
                     .arg(m_port));
            m_tcp.disconnectFromHost();
            m_isRuning =false;
        }
    }
    emit running(m_isRuning);
}

bool NetworkProvider::setParams(const QJsonValue &p)
{
    QJsonObject params = p.toObject();
    QString mode = params["mode"].toString();
    if(mode==QString("UDP")){
        quint16 port = params["port"].toInt();
        setUdpMode(port);
        return true;

    }else if(mode==QString("TCP")){
        QString host = params["host"].toString();
        quint16 port = params["port"].toInt();
        setTcpMode(host, port);
        return true;
    }else{
        LOG_ERROR(QStringLiteral("参数错误：mode : %1")
                  .arg(mode));
        //Todo： 抛出异常
        return false;
    }
}

bool NetworkProvider::isRuning()
{
    if(m_isRuning){
        if(m_useUdp){
            if(m_udp.state()!=QAbstractSocket::UnconnectedState)
                return true;
            else{
                LOG_INFO("运行状态false");
                return false;
            }
        }else{
            if(m_tcp.state()!=QAbstractSocket::UnconnectedState)
                return true;
            else
                return false;
        }
    }else{
        return false;
    }
}



void NetworkProvider::onTcpReadyRead()
{
    //读取缓冲区所有数据据
    m_tcpBuffer.append(m_tcp.readAll());

    int newLineIndex = -1;

    while((newLineIndex = m_tcpBuffer.indexOf('\n'))>=0){
        QByteArray lineBytes = m_tcpBuffer.left(newLineIndex);
        m_tcpBuffer.remove(0,newLineIndex+1);
        lineBytes = lineBytes.trimmed();
        if(lineBytes.isEmpty()) continue;

        emit dataReceived(lineBytes);
    }
}



void NetworkProvider::onUdpReadyRead()
{
    while(m_udp.hasPendingDatagrams()){
        QByteArray datagram;
        datagram.resize(int(m_udp.pendingDatagramSize()));

        m_udp.readDatagram(datagram.data(),datagram.size());

        datagram = datagram.trimmed();
        if(datagram.isEmpty()) continue;

        emit dataReceived(datagram);
    }
}

