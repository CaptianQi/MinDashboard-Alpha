import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import AppContext 1.0
import AppTheme 1.0
import "../core"
Rectangle {
    id: navBar
    height: 48
    width: parent.width
    color: AppContext.theme === "light" ? "#2c3e50" : "#1f2a35"

    Row {
        anchors.fill: parent
        anchors.margins: 8
        spacing: 12

        AppButton {
            text: "Dashboard"
            onClicked: AppContext.setPage("dashboard")
        }

        AppButton {
            text: "Settings"
            onClicked: AppContext.setPage("settings")
        }

        AppButton {
            text: "Logs"
            onClicked: AppContext.setPage("logs")
        }

        Item { Layout.fillWidth: true }

        AppButton {
            text: AppContext.theme === "light" ? "dark" : "light"
            onClicked: AppContext.toggleTheme()
        }
    }
}
