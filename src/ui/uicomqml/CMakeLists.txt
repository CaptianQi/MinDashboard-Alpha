cmake_minimum_required(VERSION 3.16)
project(EnterpriseSuite VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOMOC ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Quick)

qt_add_executable(appEnterpriseSuite
    main.cpp
)
# 会生成一个虚拟的.qrc文件  还会生成一个隐藏的qmldir/资源索引文件
qt_add_qml_module(appEnterpriseSuite
    URI EnterpriseSuite
    VERSION 1.0
    QML_FILES
        qml/Main.qml
        qml/ui/NavBar.qml
        qml/ui/Router.qml
        qml/ui/Notification.qml
        qml/pages/DashboardPage.qml
        qml/pages/SettingsPage.qml
        qml/pages/LogViewerPage.qml
        qml/ui/AppButton.qml
        qml/core/AppContext.qml
        qml/ui/AppCard.qml
        qml/ui/AppDialog.qml
        qml/ui/AppInput.qml
        qml/core/AppTheme.qml
)

# qt_add_resources(appEnterpriseSuite "extra_resources"
#     PREFIX "/EnterpriseSuite"
#     FILES
#         singletons/AppContext.qml
# )

target_link_libraries(appEnterpriseSuite PRIVATE Qt6::Quick)

set_target_properties(appEnterpriseSuite PROPERTIES
    WIN32_EXECUTABLE TRUE
    MACOSX_BUNDLE TRUE
)

install(TARGETS appEnterpriseSuite
    BUNDLE DESTINATION .
    RUNTIME DESTINATION .
)
