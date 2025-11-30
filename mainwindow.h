#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include<QObject>
#include <QMainWindow>
#include<QQuickWidget>
#include<QStackedWidget>
#include<QWidget>
QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE
class DashboardPage;
class LogPage;
class SettingPage;
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
private:
    void setupMenuBar();
    void setupMain();
private slots:
    void showAppDialog(const QString &title,
                       const QString &msg,
                       const QString &okText,
                       const QString &cancelText,
                       bool showCancel,
                       bool modal);
    void onAppDialogAccepted();
    void onAppDialogRejected();
private:
    Ui::MainWindow *ui;

    QQuickWidget *m_navQml = nullptr;
    DashboardPage *m_dashboardPage = nullptr;
    LogPage *m_logPage=nullptr;
    SettingPage *m_settingsPage = nullptr;
    QStackedWidget *m_stack = nullptr;
    QQuickWidget *m_appDialogWidget = nullptr; // 弹窗
    QObject  *m_appDialogRoot   = nullptr;
};
#endif // MAINWINDOW_H
