#include "serialprovider.h"
#include"../log/logger.h"
#include<QString>
#include<QJsonObject>
SerialProvider::SerialProvider(QObject *parent)
    : DataProvider{parent}
{

}


void SerialProvider::setPortName(const QString &name)
{
    m_serial.setPortName(name);
}

void SerialProvider::setBaudRate(int baud)
{
    m_serial.setBaudRate(baud);
}

void SerialProvider::setDataBits(int bits)
{
    m_serial.setDataBits(static_cast<QSerialPort::DataBits>(bits));
}

void SerialProvider::setStopBits(int bits)
{
    m_serial.setStopBits(static_cast<QSerialPort::StopBits>(bits));
}

void SerialProvider::setParity(int parity)
{
    m_serial.setParity(static_cast<QSerialPort::Parity>(parity));
}

bool SerialProvider::start()
{
    if(!m_serial.open(QIODevice::ReadOnly)){
        LOG_ERROR(QStringLiteral("串口打开失败：%1").
                  arg(m_serial.errorString()));
        m_isRuning = false;
        emit running(m_isRuning);
        //Todo: 抛出异常
        return false;
    }
    //连接信号
    connect(&m_serial,&QSerialPort::readyRead,this,&SerialProvider::onReayRead);
    LOG_INFO(QStringLiteral("串口已打开：%1 @ %2 @ %3 @ %4 @ %5")
             .arg(m_serial.portName())
             .arg(m_serial.baudRate())
             .arg(m_serial.dataBits())
             .arg(m_serial.stopBits())
             .arg(m_serial.parity()));

    m_isRuning =true;
    emit running(m_isRuning);
    return true;
}

void SerialProvider::stop()
{
    if(m_serial.isOpen()){
        LOG_INFO(QStringLiteral("串口已关闭：%1")
                 .arg(m_serial.portName()));
        m_serial.disconnect();
        m_serial.close();
        m_isRuning = false;
    }
    emit running(m_isRuning);
}

bool SerialProvider::setParams(const QJsonValue &p)
{
    QJsonObject params = p.toObject();
    QString mode = params["mode"].toString();
    if(mode == QString("SerialPort")){
        QString m_port = params["port"].toString();
        int m_baudRate = params["baudRate"].toInt();
        int m_dataBits = params["dataBits"].toInt();
        int m_stopBits = params["stopBits"].toInt();
        int m_parity = params["parity"].toInt();
        setPortName(m_port);
        setBaudRate(m_baudRate);
        setDataBits(m_dataBits);
        setStopBits(m_stopBits);
        setParity(m_parity);
        return true;
    }else{
        LOG_ERROR(QStringLiteral("参数错误：mode : %1")
                      .arg(mode));
        //Todo: 抛出异常
        return false;
    }
}

bool SerialProvider::isRuning()
{
    if(m_isRuning){
        if(m_serial.isOpen())
            return true;
        else
            return false;
    }
    return false;
}

void SerialProvider::onReayRead()
{
    QByteArray chunk = m_serial.readAll();
    m_buffer.append(chunk);

    int newlineIndex = -1;

    //约定以\n为结束
    while((newlineIndex=m_buffer.indexOf('\n'))>=0){
        //取数据 不包含 \n
        QByteArray lineBytes = m_buffer.left(newlineIndex);
        //从缓冲区删除一行和\n
        m_buffer.remove(0,newlineIndex+1);
        //去掉前后空白
        lineBytes = lineBytes.trimmed();
        if(lineBytes.isEmpty())  continue;  //空行忽略

        emit dataReceived(lineBytes);
    }
}

