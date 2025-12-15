import QtQuick
import QtQuick.Controls
import EnterpriseSuite 1.0
import AppContext 1.0
import AppTheme 1.0
import "core"
import "ui"
ApplicationWindow {
    id: app
    width: 900
    height: 640
    visible: true
    title: "Enterprise Suite"
    color: AppTheme.backgroundColor   // 使用主题背景色

    NavBar {
        id: nav
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right
    }

    Router {
        id: router
        anchors.top: nav.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: toast.bottom
    }

    Notification {
        id: toast
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom
    }

    Component.onCompleted: {
        AppContext.showToast("App started successfully!", "info")
    }
}
