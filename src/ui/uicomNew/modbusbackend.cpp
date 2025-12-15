#include "modbusbackend.h"
// modbusbackend.cpp
#include "datatypes.h"
#include <QtSerialPort/QSerialPort>
#include <QModbusReply>
#include <QDebug>
ModbusBackend::ModbusBackend(QObject *parent)
    : QObject{parent}
{
    m_client = new QModbusRtuSerialMaster(this);

    //巡回问
    connect(&m_pollTimer,&QTimer::timeout,
            this,&ModbusBackend::onPollTimeout);


    setupDevices();  // 用“文档" 初始化从机
}
ModbusBackend::~ModbusBackend()
{
    if (m_client) {
        //断开设备
        m_client->disconnectDevice();
    }
}

//设置点表
void ModbusBackend::setupDevices()
{
    m_devices.clear();

    // ===== 主控1：地址 3，AI 较多，DI 较少 =====
    {
        DeviceState dev;
        dev.slaveId = 3;   // 注意：不是必须 1，真实工程里经常 3、5、8 这样

        // ---- 模拟量点表（例子：不同板卡映射到不同地址段） ----
        // 文档示例：主控1
        // 40001~40010: AI板1 (温度/压力)
        // 40021~40030: AI板2 (电流/电压)

        {
            AnalogPoint p;
            p.name       = QStringLiteral("AI1_入口温度");
            p.docAddress = 40001;
            p.regAddress = 40001 - 40001; // =0
            p.scale      = 10.0;          // 原始值/10 = 真实温度
            dev.ai.append(p);
        }
        {
            AnalogPoint p;
            p.name       = QStringLiteral("AI1_出口温度");
            p.docAddress = 40002;
            p.regAddress = 40002 - 40001; // =1
            p.scale      = 10.0;
            dev.ai.append(p);
        }
        {
            AnalogPoint p;
            p.name       = QStringLiteral("AI2_A相电流");
            p.docAddress = 40021;
            p.regAddress = 40021 - 40001; // =20
            p.scale      = 100.0;         // /100 得到 A
            dev.ai.append(p);
        }
        {
            AnalogPoint p;
            p.name       = QStringLiteral("AI2_B相电流");
            p.docAddress = 40022;
            p.regAddress = 40022 - 40001; // =21
            p.scale      = 100.0;
            dev.ai.append(p);
        }

        // ---- 开关量点表：比如某块 DI 板在 00001~00016 ----
        {
            DiscretePoint p;
            p.name       = QStringLiteral("DI1_急停按钮");
            p.docAddress = 1;        // 文档 00001
            p.bitAddress = 1 - 1;    // =0
            dev.di.append(p);
        }
        {
            DiscretePoint p;
            p.name       = QStringLiteral("DI1_门禁开关");
            p.docAddress = 2;        // 文档 00002
            p.bitAddress = 2 - 1;    // =1
            dev.di.append(p);
        }

        m_devices.append(dev);
    }

    // ===== 主控2：地址 5，开关量一大堆，模拟量很少 =====
    {
        DeviceState dev;
        dev.slaveId = 5;

        // 模拟量：就两三个
        {
            AnalogPoint p;
            p.name       = QStringLiteral("环境温度");
            p.docAddress = 40101;
            p.regAddress = 40101 - 40101; // =0
            p.scale      = 10.0;
            dev.ai.append(p);
        }
        {
            AnalogPoint p;
            p.name       = QStringLiteral("机柜温度");
            p.docAddress = 40102;
            p.regAddress = 40102 - 40101; // =1
            p.scale      = 10.0;
            dev.ai.append(p);
        }

        // 开关量：很多 DO/DI 在线圈分布，比如 00001~00032
        {
            DiscretePoint p;
            p.name       = QStringLiteral("DO1_风机启动");
            p.docAddress = 1;
            p.bitAddress = 1 - 1; // 0
            dev.di.append(p);
        }
        {
            DiscretePoint p;
            p.name       = QStringLiteral("DO2_风机停止");
            p.docAddress = 2;
            p.bitAddress = 2 - 1; // 1
            dev.di.append(p);
        }
        {
            DiscretePoint p;
            p.name       = QStringLiteral("DI1_柜门开关");
            p.docAddress = 11;
            p.bitAddress = 11 - 1; // 10
            dev.di.append(p);
        }
        {
            DiscretePoint p;
            p.name       = QStringLiteral("DI2_烟雾报警");
            p.docAddress = 12;
            p.bitAddress = 12 - 1; // 11
            dev.di.append(p);
        }

        m_devices.append(dev);
    }

    // ===== 主控3：地址 8，只带少量板卡 =====
    {
        DeviceState dev;
        dev.slaveId = 8;

        // 只有一个 AI 板：文档 40051~40054
        {
            AnalogPoint p;
            p.name       = QStringLiteral("油温");
            p.docAddress = 40051;
            p.regAddress = 40051 - 40051; // =0
            p.scale      = 10.0;
            dev.ai.append(p);
        }
        {
            AnalogPoint p;
            p.name       = QStringLiteral("油压");
            p.docAddress = 40052;
            p.regAddress = 40052 - 40051; // =1
            p.scale      = 100.0;
            dev.ai.append(p);
        }

        // 开关量：只有几个状态
        {
            DiscretePoint p;
            p.name       = QStringLiteral("运行中");
            p.docAddress = 21;
            p.bitAddress = 21 - 1; // =20
            dev.di.append(p);
        }
        {
            DiscretePoint p;
            p.name       = QStringLiteral("故障灯");
            p.docAddress = 22;
            p.bitAddress = 22 - 1; // =21
            dev.di.append(p);
        }

        m_devices.append(dev);
    }

    // ===== 其它主控4 ~ 9 =====
    // 你以后就按上面这个模式，一个一个写：
    // DeviceState dev; dev.slaveId = X; dev.ai.append(...); dev.di.append(...); m_devices.append(dev);
}


// 打开串口
bool ModbusBackend::openSerial(const QString &portName, int baudRate)
{
    if (!m_client)
        return false;

    if (m_client->state() == QModbusDevice::ConnectedState)
        return true;

    //设置参数
    m_client->setConnectionParameter(QModbusDevice::SerialPortNameParameter, portName);
    m_client->setConnectionParameter(QModbusDevice::SerialBaudRateParameter, baudRate);
    m_client->setConnectionParameter(QModbusDevice::SerialDataBitsParameter, QSerialPort::Data8);
    m_client->setConnectionParameter(QModbusDevice::SerialParityParameter, QSerialPort::NoParity);
    m_client->setConnectionParameter(QModbusDevice::SerialStopBitsParameter, QSerialPort::OneStop);

    m_client->setTimeout(1000);
    m_client->setNumberOfRetries(1);

    return m_client->connectDevice();
}

void ModbusBackend::startPolling()
{
    if (!m_client || m_client->state() != QModbusDevice::ConnectedState)
        return;
    //当前从机地址
    m_currentDeviceIndex = 0;
    m_currentPhase = 0;
    //启动定时器
    m_pollTimer.start(200); // 每 200ms 轮询一个从机的模拟量 + 开关量
}

void ModbusBackend::stopPolling()
{
    //停止定时器
    m_pollTimer.stop();
}

//  槽函数：定时器槽 读设备
void ModbusBackend::onPollTimeout()
{
    //判断设备连接状态
    if (!m_client || m_client->state() != QModbusDevice::ConnectedState)
        return;
    if (m_devices.isEmpty())
        return;

    int devIndex = m_currentDeviceIndex;

    if (m_currentPhase == 0) {
        // 第一步：读模拟量（保持寄存器）
        pollAnalog(devIndex);  // 函数
        m_currentPhase = 1;
    } else {
        // 第二步：读开关量（线圈）
        pollDiscrete(devIndex);  //函数
        m_currentPhase = 0;

        // 这个从机模拟+开关都读完了 → 换下一个从机
        m_currentDeviceIndex++;
        if (m_currentDeviceIndex >= m_devices.size())
            m_currentDeviceIndex = 0;
    }
}
//读模拟量
void ModbusBackend::pollAnalog(int devIndex)
{
    //从机
    DeviceState &dev = m_devices[devIndex];
    //从机地址
    int slaveId = dev.slaveId;

    //模拟量为空则退出
    if (dev.ai.isEmpty())
        return;

    // 找出这个从机所有模拟量的最小/最大寄存器地址
    quint16 minAddr = dev.ai[0].regAddress;
    quint16 maxAddr = dev.ai[0].regAddress;
    for (const auto &p : dev.ai) {
        if (p.regAddress < minAddr) minAddr = p.regAddress;
        if (p.regAddress > maxAddr) maxAddr = p.regAddress;
    }
    quint16 count = maxAddr - minAddr + 1;

    // HoldingRegisters + 起始地址 + 数量
    QModbusDataUnit unit(QModbusDataUnit::HoldingRegisters,
                         minAddr,
                         count);

    if (QModbusReply *reply = m_client->sendReadRequest(unit, slaveId)) {
        if (!reply->isFinished()) {
            reply->setProperty("devIndex", devIndex);
            reply->setProperty("baseAddr", minAddr);
            reply->setProperty("phase", 0); // 0 = 模拟量
            connect(reply, &QModbusReply::finished,
                    this, &ModbusBackend::onReadFinished);
        } else {
            reply->deleteLater();
        }
    } else {
        handleReadError(devIndex, QStringLiteral("sendReadRequest analog failed"));
    }
}

//读开关量 Coils 功能码0x01
void ModbusBackend::pollDiscrete(int devIndex)
{
    //从机
    DeviceState &dev = m_devices[devIndex];
    //从机地址
    int slaveId = dev.slaveId;

    //开关量为空返回
    if (dev.di.isEmpty())
        return;

    quint16 minAddr = dev.di[0].bitAddress;
    quint16 maxAddr = dev.di[0].bitAddress;
    for (const auto &p : dev.di) {
        if (p.bitAddress < minAddr) minAddr = p.bitAddress;
        if (p.bitAddress > maxAddr) maxAddr = p.bitAddress;
    }
    quint16 count = maxAddr - minAddr + 1;

    //寄存器类型，起始地址，数量
    QModbusDataUnit unit(QModbusDataUnit::Coils,
                         minAddr,
                         count);

    if (QModbusReply *reply = m_client->sendReadRequest(unit, slaveId)) {
        if (!reply->isFinished()) {
            reply->setProperty("devIndex", devIndex);  //从机编号
            reply->setProperty("baseAddr", minAddr);  //起始地址
            reply->setProperty("phase", 1); // 1 = 开关量
            connect(reply, &QModbusReply::finished,
                    this, &ModbusBackend::onReadFinished);
        } else {
            reply->deleteLater();
        }
    } else {
        handleReadError(devIndex, QStringLiteral("sendReadRequest discrete failed"));
    }
}
// 回复 槽函数  统一处理回复：区分模拟量 / 开关量
void ModbusBackend::onReadFinished()
{
    //信号发送者
    auto *reply = qobject_cast<QModbusReply *>(sender());
    if (!reply)
        return;

    int devIndex   = reply->property("devIndex").toInt();
    quint16 baseAddr = static_cast<quint16>(reply->property("baseAddr").toInt());
    int phase      = reply->property("phase").toInt(); // 0=模拟量, 1=开关量

    //判断是否请求错误
    if (reply->error() == QModbusDevice::NoError) {
        //接收结果
        const QModbusDataUnit unit = reply->result();

        if (phase == 0) {
            handleAnalogSuccess(devIndex, baseAddr, unit);
        } else {
            handleDiscreteSuccess(devIndex, baseAddr, unit);
        }
    } else {
        handleReadError(devIndex, reply->errorString());
    }

    reply->deleteLater();
}

//成功接收  模拟量
void ModbusBackend::handleAnalogSuccess(int devIndex,
                                        quint16 baseAddr,
                                        const QModbusDataUnit &unit)
{
    if (devIndex < 0 || devIndex >= m_devices.size())
        return;

    DeviceState &dev = m_devices[devIndex];

    // 一旦有成功读，认为在线
    if (!dev.online) {
        dev.online = true;
        //发送从机在线信号
        emit deviceOnlineChanged(dev.slaveId, true);
    }
    dev.errorCount = 0;

    for (int i = 0; i < dev.ai.size(); ++i) {
        AnalogPoint &p = dev.ai[i];
        //寄存器地址值
        int offset = p.regAddress - baseAddr;
        if (offset < 0 || offset >= unit.valueCount()) {
            // 这次读不到这个点
            p.valid = false;
            emit analogUpdated(dev.slaveId, i, p.name, 0.0, false);
            continue;
        }

        quint16 raw = unit.value(offset);

        // 可选：约定 0xFFFF 为无效值
        if (raw == 0xFFFF) {
            p.valid = false;
            emit analogUpdated(dev.slaveId, i, p.name, 0.0, false);
            continue;
        }
        //值为人可读的值
        p.value = raw / p.scale;
        p.valid = true;

        emit analogUpdated(dev.slaveId, i, p.name, p.value, true);
    }
}


//读到开关量
void ModbusBackend::handleDiscreteSuccess(int devIndex,
                                          quint16 baseAddr,
                                          const QModbusDataUnit &unit)
{
    if (devIndex < 0 || devIndex >= m_devices.size())
        return;

    DeviceState &dev = m_devices[devIndex];

    dev.errorCount = 0;

    for (int i = 0; i < dev.di.size(); ++i) {
        DiscretePoint &p = dev.di[i];

        int offset = p.bitAddress - baseAddr;
        if (offset < 0 || offset >= unit.valueCount()) {
            p.valid = false;
            emit discreteUpdated(dev.slaveId, i, p.name, false, false);
            continue;
        }

        bool bit = unit.value(offset); // 0 or 1

        p.value = bit;
        p.valid = true;

        emit discreteUpdated(dev.slaveId, i, p.name, bit, true);
    }
}

//错误处理：连续错误 N 次认为“整台从机离线”
void ModbusBackend::handleReadError(int devIndex, const QString &errorText)
{
    if (devIndex < 0 || devIndex >= m_devices.size())
        return;

    DeviceState &dev = m_devices[devIndex];

    dev.errorCount++;
    if (dev.errorCount >= 3) { // 连续 3 次读失败 → 认为离线
        if (dev.online) {
            dev.online = false;
            emit deviceOnlineChanged(dev.slaveId, false);
        }

        // 所有数据点标记无效 → 前端可显示“报警/离线”
        for (int i = 0; i < dev.ai.size(); ++i) {
            AnalogPoint &p = dev.ai[i];
            p.valid = false;
            emit analogUpdated(dev.slaveId, i, p.name, 0.0, false);
        }
        for (int i = 0; i < dev.di.size(); ++i) {
            DiscretePoint &p = dev.di[i];
            p.valid = false;
            emit discreteUpdated(dev.slaveId, i, p.name, false, false);
        }
    }

    qDebug() << "Modbus read error: slave" << dev.slaveId << errorText;
}
