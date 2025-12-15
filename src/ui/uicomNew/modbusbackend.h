#ifndef MODBUSBACKEND_H
#define MODBUSBACKEND_H

#include <QObject>

#include <QObject>
#include <QTimer>
#include <QVector>

#include <QtSerialBus/QModbusRtuSerialMaster>
#include <QtSerialBus/QModbusDataUnit>
#include"DataTypes.h"
class ModbusBackend : public QObject
{
    Q_OBJECT
public:
    explicit ModbusBackend(QObject *parent = nullptr);
    ~ModbusBackend();

    bool openSerial(const QString &portName, int baudRate);
    void startPolling();
    void stopPolling();

    // 用“别人给的寄存器文档”初始化 9 个从机
    void setupDevices();
private slots:
    void onPollTimeout();
    void onReadFinished();
signals:
    // 卡片在线状态灯：slaveId 对应第几个卡片
    void deviceOnlineChanged(int slaveId, bool online);

    // 模拟量更新：slaveId 第几个从机，index 是该从机里第几个点
    void analogUpdated(int slaveId, int index,
                       const QString &name, double value, bool valid);

    // 开关量更新
    void discreteUpdated(int slaveId, int index,
                         const QString &name, bool value, bool valid);
private:
    void pollAnalog(int devIndex);
    void pollDiscrete(int devIndex);

    void handleAnalogSuccess(int devIndex,
                             quint16 baseAddr,
                             const QModbusDataUnit &unit);
    void handleDiscreteSuccess(int devIndex,
                               quint16 baseAddr,
                               const QModbusDataUnit &unit);

    void handleReadError(int devIndex, const QString &errorText);

private:
    QModbusRtuSerialMaster *m_client = nullptr;
    QTimer m_pollTimer;

    QVector<DeviceState> m_devices;   // 9 个从机
    int m_currentDeviceIndex = 0;     // 当前轮询到第几个从机 (0~8)
    int m_currentPhase = 0;           // 0 先读模拟量，1 再读开关量

};

#endif // MODBUSBACKEND_H
