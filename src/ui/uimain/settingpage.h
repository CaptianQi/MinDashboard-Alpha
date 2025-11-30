#ifndef SETTINGPAGE_H
#define SETTINGPAGE_H

#include <QObject>
#include <QWidget>
#include<QSettings>
#include<QString>
#include<QQuickWidget>
class SettingPage : public QWidget
{
    Q_OBJECT
public:
    explicit SettingPage(QWidget *parent = nullptr);
    void refreshFromSettings();
private slots:
    //对接 QML 信号
    void onBrowseSavePathRequested();
    void onSaveClicked();
    void onConnectState(bool);
signals:
    void requestDialog(const QString &title,
                       const QString &msg,
                       const QString &okText,
                       const QString &cancelText,
                       bool showCancel,
                       bool modal);
private:
    void setupUi();
    void loadSettings();
    void saveSettings();
    void loadToQml();
    void readFromQmlAndSave();
    void uiToBend();
private:
    QSettings m_settings;
    //========参数=========
    //TCP
    QString m_tcpHost;
    int m_tcpPort;
    //UDP
    int m_udpPort;   //本地监听端口
    //串口
    QString m_port;
    int m_baudRate;
    int m_dataBits;
    int m_stopBits;
    int m_parity;
    //模式
    QString m_mode;
    //存储路径
    QString m_savePath;
    //存储格式
    QString m_format;

    QQuickWidget *m_settingsQml = nullptr;  //qml页面
    QObject *m_settingsRoot = nullptr;   //qml 根对象

    bool m_connectState = false;

};

#endif // SETTINGPAGE_H
