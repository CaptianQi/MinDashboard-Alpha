#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include"src/core/log/logger.h"
#include"src/ui/uimain/dashboardpage.h"
#include"src/ui/uimain/logpage.h"
#include"src/ui/uimain/settingpage.h"

#include<QMenu>
#include<QAction>
#include<QString>
#include<QMessageBox>
#include<QHBoxLayout>
#include<QQmlContext>
#include<QQuickItem>
#include<QDebug>
#include<QMetaObject>
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowTitle("MinDashboard");
   // setupMenuBar();
    setupMain();
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::setupMenuBar()
{
    QMenu *file = new QMenu(QString::fromUtf8("文件"));
    QMenu *view = new QMenu(QString::fromUtf8("视图"));
    QMenu *item = new QMenu(QString::fromUtf8("项目"));
    QMenu *help = new QMenu(QString::fromUtf8("帮助"));

    ui->menubar->addMenu(file);
    ui->menubar->addMenu(view);
    ui->menubar->addMenu(item);
    ui->menubar->addMenu(help);

    //file 菜单
    QAction *fileExit = new QAction(QString::fromUtf8("退出(&X)"));
    file->addAction(fileExit);
    connect(fileExit,&QAction::triggered,this,&MainWindow::close);

    //help 菜单
    QAction *helpAbout = new QAction(QString::fromUtf8("关于"));
    help->addAction(helpAbout);

}

void MainWindow::setupMain()
{
    QWidget *central = ui->centralwidget;
    //设置布局
    auto *rootLayout = new QHBoxLayout(central);
    rootLayout->setContentsMargins(0,0,0,0);
    rootLayout->setSpacing(0);

    //左侧导航栏 QML
    m_navQml = new QQuickWidget(central);
    m_navQml->setResizeMode(QQuickWidget::SizeRootObjectToView);
    m_navQml->setSource(QUrl(QStringLiteral("qrc:/src/ui/qml/NavBar.qml")));
    m_navQml->setMinimumWidth(120);
    m_navQml->setMaximumWidth(160);

    //右侧堆栈页面
    m_stack = new QStackedWidget(central);
    //dashboard页面
    m_dashboardPage = new DashboardPage(m_stack);
    m_dashboardPage->setObjectName("DashboardPage");
    //日志页面
    m_logPage  = new LogPage(m_stack);
    m_logPage->setObjectName("LogPage");
    //设置页面
    m_settingsPage = new SettingPage(m_stack);
    m_settingsPage->setObjectName("SettingsPage");
    //加入到stack
    m_stack->addWidget(m_dashboardPage);
    m_stack->addWidget(m_logPage);
    m_stack->addWidget(m_settingsPage);

    //加入到布局
    rootLayout->addWidget(m_navQml);
    rootLayout->addWidget(m_stack,1);

    //QML信号 连接到 StackWidget
    QObject *navRoot = m_navQml->rootObject();  //QML 根对象Rectangle
    if(navRoot){
        connect(navRoot,SIGNAL(pageRequested(int)),
                         m_stack,SLOT(setCurrentIndex(int)));
        connect(m_stack,&QStackedWidget::currentChanged,
                this,[=](int index){
            if(index==2)
                m_settingsPage->refreshFromSettings();
        });
        connect(navRoot,SIGNAL(close()),this,SLOT(close()),
                    Qt::UniqueConnection);
    }

    //默认当前页面
    m_stack->setCurrentIndex(0);

    // ===== 创建弹窗 QQuickWidget =====
    m_appDialogWidget = new QQuickWidget(this);
    m_appDialogWidget->setResizeMode(QQuickWidget::SizeRootObjectToView);
    m_appDialogWidget->setSource(QUrl(QStringLiteral("qrc:/src/ui/qml/AppDialog.qml")));

    m_appDialogWidget->setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint);
    m_appDialogWidget->setAttribute(Qt::WA_TranslucentBackground);
    m_appDialogWidget->setClearColor(Qt::transparent);

    m_appDialogWidget->resize(360, 220);
    m_appDialogWidget->hide();

    m_appDialogRoot = m_appDialogWidget->rootObject();
    if (m_appDialogRoot) {
        connect(m_appDialogRoot, SIGNAL(accepted()),
                this, SLOT(onAppDialogAccepted()));
        connect(m_appDialogRoot, SIGNAL(rejected()),
                this, SLOT(onAppDialogRejected()));
    }
    //弹窗信号
    connect(m_dashboardPage, &DashboardPage::requestDialog,
            this, &MainWindow::showAppDialog);
    connect(m_settingsPage,&SettingPage::requestDialog,
            this,&MainWindow::showAppDialog);

    LOG_DEBUG("测试");
}

void MainWindow::showAppDialog(const QString &title, const QString &msg,const QString &okText,
                               const QString &cancelText,bool showCancel,bool modal)
{
    if (!m_appDialogWidget || !m_appDialogRoot)
        return;

    // 设置 QML 属性
    m_appDialogRoot->setProperty("title",      title);
    m_appDialogRoot->setProperty("message",    msg);
    m_appDialogRoot->setProperty("okText",     okText);
    m_appDialogRoot->setProperty("cancelText", cancelText);
    m_appDialogRoot->setProperty("showCancel", showCancel);

    // 由 QWidget 控制模态 / 非模态
    m_appDialogWidget->setWindowModality(
        modal ? Qt::ApplicationModal : Qt::NonModal
        );

    // 定位到 MainWindow 正中间
    const int dlgW = m_appDialogWidget->width();
    const int dlgH = m_appDialogWidget->height();
    const QRect mwRect = this->geometry();
    int x = mwRect.center().x() - dlgW / 2;
    int y = mwRect.center().y() - dlgH / 2;
    m_appDialogWidget->move(x, y);

    m_appDialogWidget->show();
    m_appDialogWidget->raise();
}
void MainWindow::onAppDialogAccepted()
{
    if (m_appDialogWidget)
        m_appDialogWidget->hide();

}

void MainWindow::onAppDialogRejected()
{
    if (m_appDialogWidget)
        m_appDialogWidget->hide();

}
