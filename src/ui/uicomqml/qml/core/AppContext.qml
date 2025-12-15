pragma Singleton
import QtQml
import AppTheme 1.0

QtObject {
    id: appContext

    // ========= 页面导航 =========
    // 当前页面（dashboard / settings / logs）
    property string currentPageId: "dashboard"
    signal routeChanged(string pageId)

    function setPage(pageId) {
        if (currentPageId === pageId)
            return
        currentPageId = pageId
        routeChanged(pageId)   // 通知 Router
    }

    // ========= 对话框管理 =========
    property bool dialogVisible: false
    property string dialogId: ""

    function openDialog(id) {
        dialogId = id
        dialogVisible = true
    }

    function closeDialog() {
        dialogVisible = false
        dialogId = ""
    }

    // ========= Toast / 通知管理 =========
    property bool toastVisible: false
    property string toastMessage: ""
    property string toastLevel: "info"   // info / success / error
    signal toastRequested(string message, string level)

    function showToast(message, level) {
        toastMessage = message
        toastLevel = (level && level !== "") ? level : "info"
        toastVisible = true
        toastRequested(toastMessage, toastLevel)  // 通知 Notification 去播放动画 / 计时
    }

    function hideToast() {
        toastVisible = false
    }

    // ========= 主题管理（转发到 AppTheme） =========
    // 供 NavBar 读当前主题
    property string theme: AppTheme.themeName

    function toggleTheme() {
        AppTheme.toggleTheme()
        theme = AppTheme.themeName
    }
}
