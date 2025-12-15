import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import AppContext 1.0
import EnterpriseSuite 1.0
import "../ui"
Page {
    id:page
    title:"Dashboard"

    // Column {
    //     anchors.centerIn: parent
    //     spacing: 12

    //     Rectangle {
    //         width: 220; height: 220; radius: 12
    //         color: AppContext.theme === "light" ? "#dddddd" : "#666666"
    //         border.color: "#bbbbbb"
    //         Text { anchors.centerIn: parent; text: "Idle"; font.pixelSize: 20 }
    //     }

    //     Row {
    //         spacing: 8
    //         Button { text: "Start"; onClicked: AppContext.showToast("Job started") }
    //         Button { text: "Stop";  onClicked: AppContext.showToast("Job stopped") }
    //         Button { text: "Error"; onClicked: AppContext.showToast("Job error!") }
    //     }
    // }

    // 模块 用户数   系统状态  当前时间  日志数量

    //自动刷新时间
    Timer{
        id:clock
        interval: 1000
        repeat: true
        running:true
        onTriggered: timeCard.content= Qt.formatTime(new Date(),"hh:mm:ss")
    }

    //主布局
    GridLayout{
        id: grid
        anchors.centerIn: parent
        columns:2
        rowSpacing:20
        columnSpacing:20

        //用户数量
        AppCard{
            title:"用户数"
            content: "1,240"
            backgroundColor: "#ffffff"
            titleColor:"#555"
            contentColor: "#222"
        }

        //系统状态
        AppCard{
            title:"系统状态"
            content: "在线"
            backgroundColor: "#ecf8f1"
            titleColor:"#27ae60"
            contentColor: "#2ecc71"
        }

        //当前时间
        AppCard{
            id:timeCard
            title:"当前时间"
            content: "--:--:--"
            backgroundColor: "#ecf8f1"
            titleColor:"#1e6be1"
            contentColor: "#1e6be1"
        }

        //日志数量 （目前是静态的）
        AppCard{
            title:"日志数量"
            content: "58"
            backgroundColor: "#fff8e1"
            titleColor:"#f39c12"
            contentColor: "#e67e22"
        }
    }
}
