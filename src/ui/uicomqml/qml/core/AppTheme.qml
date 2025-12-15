pragma Singleton
import QtQuick

QtObject {
    id: theme

    // 当前主题名（light / dark）
    property string themeName: "light"

    // 颜色配置
    property color primaryColor: themeName === "light" ? "#2196F3" : "#90CAF9"
    property color errorColor: "#F44336"
    property color successColor: "#4CAF50"
    property color backgroundColor: themeName === "light" ? "#ffffff" : "#2c2c2c"
    property color textColor: themeName === "light" ? "#333333" : "#f0f0f0"

    // 切换主题
    function toggleTheme() {
        themeName = (themeName === "light" ? "dark" : "light")
    }
}
