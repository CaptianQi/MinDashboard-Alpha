#ifndef DASHBOARDPAGE_H
#define DASHBOARDPAGE_H

#include <QObject>
#include <QWidget>
class ArcGaugeWidget;
class RealtimeAreaChartWidget;
class StatusIndicatorWidget;
class QQuickWidget;
class DashboardPage : public QWidget
{
    Q_OBJECT
public:
    explicit DashboardPage(QWidget *parent = nullptr);
private:
    void setupUi();
private slots:
    void onValue(const QString &key,const QVariant &value);
    void onIsRunning(bool running);
    void connectProvider(bool running);
    //recorder
    void startRecorder(bool recording);
    void onIsRecording(bool recording);
private:
    ArcGaugeWidget *m_gauge1 = nullptr;
    ArcGaugeWidget *m_gauge2 = nullptr;
    RealtimeAreaChartWidget *m_areaChart = nullptr;
    StatusIndicatorWidget *m_indicatorComm = nullptr;
    StatusIndicatorWidget *m_indicatorRun = nullptr;
    StatusIndicatorWidget *m_indicatorRecord = nullptr;
    QQuickWidget *m_connectQml = nullptr;
    QQuickWidget *m_recorderQml = nullptr;
    QObject *m_connectRoot = nullptr;
    QObject *m_recorderRoot = nullptr;
    bool m_desiredRunning = false;
    bool m_desiredRecorder = false;
    bool m_connectState = false;
signals:
    void requestDialog(const QString &title,
                       const QString &msg,
                       const QString &okText,
                       const QString &cancelText,
                       bool showCancel,
                       bool modal);
};

#endif // DASHBOARDPAGE_H
