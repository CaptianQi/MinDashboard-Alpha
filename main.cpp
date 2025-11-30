#include "mainwindow.h"

#include <QApplication>
#include"src/core/log/logger.h"
#include"src/core/middle/StructType.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    //初始化日志系统
    Logger::instance()->initFromJson(":/src/config/config.json");
    //注册类型到Qt元对象系统
    qRegisterMetaType<FieldConfig>("FieldConfig");
    //QSettings 所有无参构造共享同一个文件  系统注册表单中
    // QCoreApplication::setOrganizationName("L");
    // QCoreApplication::setApplicationName("MinDashboard");

    MainWindow w;
    w.show();
    return a.exec();
}
