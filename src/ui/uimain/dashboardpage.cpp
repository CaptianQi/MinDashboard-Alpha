#include "dashboardpage.h"
#include"../../core/log/logger.h"
#include"../uicom/gaugewidget.h"
#include"../uicom/arcgaugewidget.h"
#include"../uicom/realtimeareachartwidget.h"
#include"../uicom/statusindicatorwidget.h"
#include"../uicom/cardwidget.h"
#include"../../core/log/logger.h"
#include"../../core/middle/signals/datasignals.h"
#include"../../core/man/gateway.h"
#include"../../config/apis.h"

#include<QVBoxLayout>
#include<QLabel>
#include<QQuickItem>
#include<QQuickWidget>
#include<QUrl>
DashboardPage::DashboardPage(QWidget *parent)
    : QWidget{parent}
{
    setupUi();


    connect(DataSignals::instance(),&DataSignals::valueUpdated,
            this,&DashboardPage::onValue);
    connect(DataSignals::instance(),&DataSignals::providerState,
            this,&DashboardPage::onIsRunning);
    connect(DataSignals::instance(),&DataSignals::recorderState,
            this,&DashboardPage::onIsRecording);
    connect(m_connectRoot,SIGNAL(toggled(bool)),
            this,SLOT(connectProvider(bool)));
    connect(m_recorderRoot,SIGNAL(toggled(bool)),
            this,SLOT(startRecorder(bool)));
}


void DashboardPage::setupUi()
{
    // 整个仪表盘页面：垂直布局
    auto *pageLayout = new QVBoxLayout(this);
    pageLayout->setContentsMargins(12, 10, 12, 10);
    pageLayout->setSpacing(12);

    // ================================
    // 顶部仪表行（Two Gauges + Summary）
    auto *topRow = new QHBoxLayout;
    topRow->setSpacing(12);
    pageLayout->addLayout(topRow);

    // ------ Gauge 1 ------
    auto *gaugeCard1 = new CardWidget(this);
    gaugeCard1->setFooterText(QStringLiteral("转速 / Speed (rpm)"));
    gaugeCard1->setMinimumHeight(220);

    auto *gaugeLayout1 = new QVBoxLayout(gaugeCard1);
    gaugeLayout1->setContentsMargins(12, 12, 12, 12);

    m_gauge1 = new ArcGaugeWidget(gaugeCard1);
    m_gauge1->setRange(0, 5000);
    m_gauge1->setUnit(QStringLiteral("rpm"));
    m_gauge1->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    gaugeLayout1->addWidget(m_gauge1);

    topRow->addWidget(gaugeCard1, 2);


    // ------ Gauge 2 ------
    auto *gaugeCard2 = new CardWidget(this);
    gaugeCard2->setFooterText(QStringLiteral("冷却水温度 / Temperature (°C)"));
    gaugeCard2->setMinimumHeight(220);

    auto *gaugeLayout2 = new QVBoxLayout(gaugeCard2);
    gaugeLayout2->setContentsMargins(12, 12, 12, 12);

    m_gauge2 = new ArcGaugeWidget(gaugeCard2);
    m_gauge2->setRange(0, 100);
    m_gauge2->setUnit(QStringLiteral("°C"));
    m_gauge2->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    gaugeLayout2->addWidget(m_gauge2);

    topRow->addWidget(gaugeCard2, 2);


    auto *controlCard = new CardWidget(this);
    controlCard->setMinimumWidth(160);
    controlCard->setMaximumWidth(220);
    controlCard->setFooterText(QStringLiteral("控制面板 / ControlPanel"));

    // 主垂直布局
    auto *controlLayout = new QVBoxLayout(controlCard);
    controlLayout->setContentsMargins(12, 16, 12, 12);
    controlLayout->setSpacing(12);

    // 第一行：标题
    QLabel *titleLabel = new QLabel("通信与录制控制");
    titleLabel->setStyleSheet("font-weight: bold; font-size: 15px;");
    controlLayout->addWidget(titleLabel);

    // 第二行：连接：QML
    {
        auto *row = new QHBoxLayout();
        row->setSpacing(8);
        row->setContentsMargins(0,20,0,4);
        QLabel *lblConnect = new QLabel("通信连接：");
        row->addWidget(lblConnect);

        m_connectQml = new QQuickWidget(controlCard);
        m_connectQml->setResizeMode(QQuickWidget::SizeRootObjectToView);
        m_connectQml->setSource(QUrl("qrc:/src/ui/qml/ToggleSwitch.qml"));
        m_connectRoot = m_connectQml->rootObject();
      //  m_connectQml->setFixedSize(60, 30);
        row->addWidget(m_connectQml);

        row->addStretch();   // 右侧留余量
        controlLayout->addLayout(row);
    }

    // 第三行：录制：QML
    {
        auto *row = new QHBoxLayout();
        row->setContentsMargins(0,5,0,4);
        row->setSpacing(8);

        QLabel *lblRecord = new QLabel("数据录制：");
        row->addWidget(lblRecord);

        m_recorderQml = new QQuickWidget(controlCard);
        m_recorderQml->setResizeMode(QQuickWidget::SizeRootObjectToView);
        m_recorderQml->setSource(QUrl("qrc:/src/ui/qml/ToggleSwitch.qml"));
        m_recorderRoot = m_recorderQml->rootObject();
     //   m_recorderQml->setFixedSize(60, 30);
        row->addWidget(m_recorderQml);

        row->addStretch();
        controlLayout->addLayout(row);
    }

    controlLayout->addStretch();
    topRow->addWidget(controlCard, 1);  // stretch=1


    // 设置 topRow 的整体高度拉伸系数
    pageLayout->setStretch(0, 3);  // topRow 占整体 3 份


    // ================================
    // 中间实时曲线图
    auto *chartCard = new CardWidget(this);
    chartCard->setFooterText(QStringLiteral("实时趋势 / Realtime Trend"));

    auto *chartLayout = new QVBoxLayout(chartCard);
    chartLayout->setContentsMargins(12, 12, 14, 28);  //
    chartLayout->setSpacing(8);

    m_areaChart = new RealtimeAreaChartWidget(chartCard);
    m_areaChart->setMaxPoints(7);
    m_areaChart->setSampleIntervalMs(1000);
    m_areaChart->setFixedYRange(3000, 6000);

    m_areaChart->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    chartLayout->addWidget(m_areaChart);

    pageLayout->addWidget(chartCard);
    pageLayout->setStretch(1, 4);   // 中间曲线图占最大高度


    // ================================
    // 底部状态行
    auto *statusCard = new CardWidget(this);
    statusCard->setFixedHeight(40);


    auto *statusLayout = new QHBoxLayout(statusCard);
    statusLayout->setContentsMargins(8, 4, 8, 4);
    statusLayout->setSpacing(16);

    m_indicatorComm = new StatusIndicatorWidget(statusCard);
    m_indicatorComm->setText(QStringLiteral("通讯连接"));
    m_indicatorComm->setStatus(StatusIndicatorWidget::Off);
    statusLayout->addWidget(m_indicatorComm);

    m_indicatorRun = new StatusIndicatorWidget(statusCard);
    m_indicatorRun->setText(QStringLiteral("设备运行"));
    m_indicatorRun->setStatus(StatusIndicatorWidget::Warning);
    statusLayout->addWidget(m_indicatorRun);

    m_indicatorRecord = new StatusIndicatorWidget(statusCard);
    m_indicatorRecord->setText(QStringLiteral("录制状态"));
    m_indicatorRecord->setStatus(StatusIndicatorWidget::Off);
    statusLayout->addWidget(m_indicatorRecord);

    statusLayout->addStretch(1);
    pageLayout->addWidget(statusCard);
    pageLayout->setStretch(2, 0);   // 状态行占最小
}

void DashboardPage::onValue(const QString &key, const QVariant &value)
{
    const double v = value.toDouble();
    if(key==QString("gauge1")){
        m_gauge1->setValue(v);
        return;
    }else if(key==QString("gauge2")){
        m_gauge2->setValue(v);
        return;
    }else if(key==QString("areaChart")){
        m_areaChart->pushExternalValue(v); // 高频喂最新值
        return;
    }else{
        LOG_WARN(QStringLiteral("未监控的数据 key: %1")
                     .arg(v));
        return;
    }
}

void DashboardPage::onIsRunning(bool running)
{
    //更新UI
    if (running) {
        m_indicatorComm->setStatus(StatusIndicatorWidget::Ok);
    } else {
        m_indicatorComm->setStatus(StatusIndicatorWidget::Off);
    }
    m_connectRoot->setProperty("checked", running);

    if (!running && m_desiredRunning) {
        emit requestDialog(
            "提示",
            "连接失败，请检查通信设置",
            QStringLiteral("确定"),
            QStringLiteral("取消"),
            false,
            true
            );
        m_desiredRunning = false;
    }
    m_connectState = running;
}

void DashboardPage::connectProvider(bool running)
{
    m_desiredRunning = running;
    if(running){
        GateWay::instance()->send(API::PROVIDER::STARTP);
    }else{
        GateWay::instance()->send(API::PROVIDER::CLOSEP);
    }
}

void DashboardPage::startRecorder(bool recording)
{
    if(!m_connectState){
        emit requestDialog("提示",
                           "通信未连接，请先启动通信",
                           QStringLiteral("确定"),
                           QStringLiteral("取消"),
                           false, true);
        m_recorderRoot->setProperty("checked",false);
        return;
    }
    m_desiredRecorder = recording;
    if(recording){
        GateWay::instance()->send(API::RECORDER::START);
    }else{
        GateWay::instance()->send(API::RECORDER::STOPR);
    }
}

void DashboardPage::onIsRecording(bool recording)
{
    if(recording){
        m_indicatorRecord->setStatus(StatusIndicatorWidget::Ok);
        m_recorderRoot->setProperty("checked",recording);
    }else{
        m_indicatorRecord->setStatus(StatusIndicatorWidget::Off);
        m_recorderRoot->setProperty("checked",recording);
    }

    if (!recording && m_desiredRecorder) {
        emit requestDialog("提示",
                           "数据记录失败，请检查文件路径或读写权限",
                           QStringLiteral("确定"),
                           QStringLiteral("取消"),
                           false, true);
        m_desiredRecorder = false;
    }
}
