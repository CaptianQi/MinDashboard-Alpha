#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QtQml> // qmlRegisterSingletonType
#include<QDebug>
int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);
    QQmlApplicationEngine engine;

    // Register QML singleton from QML file (no qmldir / no pragma Singleton)
    //注册单例
    qmlRegisterSingletonType(
        QUrl(u"qrc:/EnterpriseSuite/qml/core/AppContext.qml"_qs),
        "AppContext", 1, 0, "AppContext");

    qmlRegisterSingletonType(
        QUrl(u"qrc:/EnterpriseSuite/qml/core/AppTheme.qml"_qs),
        "AppTheme", 1, 0, "AppTheme");

    const QUrl url(u"qrc:/EnterpriseSuite/qml/Main.qml"_qs);
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated, &app,
                     [url](QObject *obj, const QUrl &objUrl) {
                         if (!obj && url == objUrl) QCoreApplication::exit(-1);
                     }, Qt::QueuedConnection);
    engine.load(url);

    return app.exec();
}
