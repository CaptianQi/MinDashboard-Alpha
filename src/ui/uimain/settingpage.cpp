#include "settingpage.h"
#include"../../core/man/gateway.h"
#include"../../config/apis.h"
#include"../../core/middle/signals/datasignals.h"
#include<QUrl>
#include<QVBoxLayout>
#include<QFileDialog>
#include<QQuickItem>
#include<QJsonObject>
#include<QJsonValue>
#include<QDebug>
SettingPage::SettingPage(QWidget *parent)
    : QWidget(parent),m_settings("settingsconfig.ini",QSettings::IniFormat)
{
    setupUi();
    loadSettings();
    loadToQml();
    uiToBend();
    connect(DataSignals::instance(),&DataSignals::providerState,this,&SettingPage::onConnectState);
}

//切换界面刷新
void SettingPage::refreshFromSettings()
{
    loadToQml();
}
//浏览本地目录
void SettingPage::onBrowseSavePathRequested()
{
    QString dir = QFileDialog::getExistingDirectory(
        this, tr("选择数据保存目录"), QString());

    if (dir.isEmpty())
        return;

    // 更新 QML 的编辑值
    if (m_settingsRoot){
        m_savePath = dir;
        m_settingsRoot->setProperty("savePath",dir);
        saveSettings();
        uiToBend();
    }
}

void SettingPage::onSaveClicked()
{
    if(m_connectState){
        emit requestDialog(
            "错误",
            "请先断开通信连接",
            QStringLiteral("确定"),
            QStringLiteral("取消"),
            false,
            true
            );
        return;
    }
    readFromQmlAndSave();
    uiToBend();
}

void SettingPage::onConnectState(bool running)
{
    m_connectState = running;
}

void SettingPage::setupUi()
{
    //qml
    m_settingsQml = new QQuickWidget(this);
    m_settingsQml->setResizeMode(QQuickWidget::SizeRootObjectToView);
    m_settingsQml->setSource(
        QUrl(QStringLiteral("qrc:/src/ui/qml/SettingsPage.qml")));
    //布局
    auto *layout = new QVBoxLayout(this);
    layout->setContentsMargins(0,0,0,0);
    layout->addWidget(m_settingsQml);
    //qml根对象
    m_settingsRoot = m_settingsQml->rootObject();

    //连接QML信号
    connect(m_settingsRoot,SIGNAL(browseSavePathRequested()),
            this,SLOT(onBrowseSavePathRequested()),
            Qt::UniqueConnection);  //只连接一次
    connect(m_settingsRoot,SIGNAL(saveClicked()),
            this,SLOT(onSaveClicked()),
            Qt::UniqueConnection);
}
//读取配置
void SettingPage::loadSettings()
{
    // settings.value(key,default)  第二参数是默认值
    //恢复通信模式
    m_mode = m_settings.value("mode","TCP").toString();

    //恢复串口配置
    m_port = m_settings.value("serial/port","COM1").toString();
    m_baudRate = m_settings.value("serial/baud",115200).toInt();
    m_dataBits = m_settings.value("serial/databits",8).toInt();
    m_stopBits = m_settings.value("serial/stopbits",1).toInt();
    m_parity = m_settings.value("serial/parity",0).toInt();

    //恢复TCP配置
    m_tcpHost = m_settings.value("tcp/host","127.0.0.1").toString();
    m_tcpPort = m_settings.value("tcp/port",9000).toInt();

    //恢复UDP配置
    m_udpPort = m_settings.value("udp/port",9001).toInt();

    //恢复存储路径
    m_savePath = m_settings.value("recorder/path","").toString();
    //恢复存储格式
    m_format = m_settings.value("recorder/format","CSV").toString();
}
//保存配置
void SettingPage::saveSettings()
{
    m_settings.setValue("mode",m_mode);
    //保存串口设置
    m_settings.setValue("serial/port",m_port);
    m_settings.setValue("serial/baud",m_baudRate);
    m_settings.setValue("serial/databits",m_dataBits);
    m_settings.setValue("serial/stopbits",m_stopBits);
    m_settings.setValue("serial/parity",m_parity);
    //保存TCP设置
    m_settings.setValue("tcp/host",m_tcpHost);
    m_settings.setValue("tcp/port",m_tcpPort);
    //保存UDP设置
    m_settings.setValue("udp/port",m_udpPort);
    //保存存储路径
    m_settings.setValue("recorder/path",m_savePath);
    //保存存储格式
    m_settings.setValue("recorder/format",m_format);


}

// C++ -> QML
void SettingPage::loadToQml()
{
    if(!m_settingsRoot) return;

    m_settingsRoot->setProperty("mode",m_mode);
    m_settingsRoot->setProperty("tcpHost",m_tcpHost);
    m_settingsRoot->setProperty("tcpPort",m_tcpPort);
    m_settingsRoot->setProperty("udpPort",m_udpPort);
    m_settingsRoot->setProperty("serialName",m_port);
    m_settingsRoot->setProperty("baudBits", m_baudRate);
    m_settingsRoot->setProperty("dataBits", m_dataBits);
    m_settingsRoot->setProperty("stopBits", m_stopBits);
    m_settingsRoot->setProperty("parity", m_parity);
    m_settingsRoot->setProperty("savePath", m_savePath);
    m_settingsRoot->setProperty("format",m_format);
}
// QML -> C++
void SettingPage::readFromQmlAndSave()
{
    if(!m_settingsRoot) return;
    //读取
    m_mode = m_settingsRoot->property("mode").toString();
    m_tcpHost = m_settingsRoot->property("tcpHost").toString();
    m_tcpPort = m_settingsRoot->property("tcpPort").toInt();
    m_udpPort = m_settingsRoot->property("udpPort").toInt();
    m_port = m_settingsRoot->property("serialName").toString();
    m_baudRate = m_settingsRoot->property("baudBits").toInt();
    m_dataBits = m_settingsRoot->property("dataBits").toInt();
    m_stopBits = m_settingsRoot->property("stopBits").toInt();
    m_parity = m_settingsRoot->property("parity").toInt();
    m_savePath = m_settingsRoot->property("savePath").toString();
    m_format = m_settingsRoot->property("format").toString();
    //保存
    saveSettings();

}

void SettingPage::uiToBend()
{
    QJsonObject params;
    QJsonObject path;
    if(m_mode =="TCP"){
        params["mode"] = QString("TCP");
        params["host"] = m_tcpHost;
        params["port"] = m_tcpPort;
    }else if(m_mode =="UDP"){
        params["mode"] = QString("UDP");
        params["port"] = m_udpPort;
    }else{
        params["mode"] = QString("SerialPort");
        params["port"] = m_port;
        params["baudRate"] = m_baudRate;
        params["dataBits"] = m_dataBits;
        params["stopBits"] = m_stopBits;
        params["parity"] = m_parity;
    }
    path["directory"] = m_savePath;
    path["format"] = m_format;
    qDebug()<< m_mode;
    GateWay::instance()->send(API::PROVIDER::SETP,params);
    GateWay::instance()->send(API::RECORDER::SET,path);
}
