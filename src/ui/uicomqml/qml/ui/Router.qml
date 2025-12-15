import QtQuick
import QtQuick.Controls
import AppContext 1.0
import EnterpriseSuite 1.0   // 里面注册了 pages/*.qml
import "../pages"
import "../core"
Item {
    id: router

    // 预创建三个页面组件
    Component { id: dashboardPage; DashboardPage {} }
    Component { id: settingsPage; SettingsPage {} }
    Component { id: logsPage; LogViewerPage {} }

    StackView {
        id: stack
        anchors.fill: parent
        initialItem: dashboardPage
    }

    // 监听 AppContext 的路由信号
    Connections {
        target: AppContext
        function onRouteChanged(pageId) {
            if (pageId === "dashboard") {
                stack.replace(dashboardPage)
            } else if (pageId === "settings") {
                stack.replace(settingsPage)
            } else if (pageId === "logs") {
                stack.replace(logsPage)
            }
        }
    }
}
