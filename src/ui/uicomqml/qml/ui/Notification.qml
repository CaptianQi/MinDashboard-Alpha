import QtQuick
import QtQuick.Controls
import AppContext 1.0
import AppTheme 1.0
import "../core"
Rectangle {
    id: toast
    height: visible ? 40 : 0
    color: AppContext.toastLevel === "error" ? AppTheme.errorColor
         : AppContext.toastLevel === "success" ? AppTheme.successColor
         : AppTheme.primaryColor
    visible: AppContext.toastVisible
    width: parent.width * 0.5
    radius: 6
    opacity: visible ? 1 : 0

    Behavior on opacity { NumberAnimation { duration: 150 } }

    Text {
        anchors.centerIn: parent
        text: AppContext.toastMessage
        color: "white"
        font.pixelSize: 14
    }

    Timer {
        id: hideTimer
        interval: 2000
        repeat: false
        onTriggered: AppContext.hideToast()
    }

    Connections {
        target: AppContext
        function onToastRequested(message, level) {
            hideTimer.restart()
        }
    }
}
