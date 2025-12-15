import QtQuick
import QtQuick.Controls
import AppContext 1.0
Page {
    id: logPage
    title: "Logs"

    //显示一组模拟日志； 每条日志包含时间+内容 ；
    //支持滚动浏览；  点击按钮可以插入新的日志（动态更新）
    Column{
        anchors.fill: parent
        anchors.margins: 20
        spacing:10

        //按钮栏
        Row{
            spacing: 10
            Button{
                text: "Add log"
                onClicked:{
                    const time = Qt.formatTime(new Date(),"hh:mm:ss")
                    logModel.append({
                        time: time,
                        content: "System heartbeat OK at "+time
                    })
                    AppContext.showToast("Log added")
                }
            }
            Button{
                text: "Clear Logs"
                onClicked: logModel.clear()
            }
        }

        //日志显示区域
        Rectangle{
            id: logArea
            color: AppContext.theme ==="light" ? "#f0f0f0" : "#2b2b2b"
            border.color: "#999"
            radius:6
            anchors.left: parent.left
            anchors.right:parent.right
            anchors.topMargin:10
            height: parent.height-120
            clip: true

            //动态列表
            ListView{
                id: logList
                anchors.fill:parent
                model: ListModel{
                    id: logModel
                    ListElement{ time:"12:00:01"; content:"System initialized" }
                    ListElement{ time:"12:00:03"; content:"Database connected" }
                    ListElement{ time:"12:00:05"; content:"App ready" }
                }

                delegate: Rectangle{
                    width: parent.width
                    height:36
                    color: index % 2 ===0
                           ?(AppContext.theme==="light"?"#ffffff":"#37424f")
                           :(AppContext.theme==="light"?"#f8f8f8":"#303a47")
                    border.color: "#ccc"
                    Row{
                        anchors.verticalCenter:parent.verticalCenter
                        anchors.left: parent.left
                        anchors.leftMargin:12
                        spacing:12
                        Label{ text: time; font.pixelSize:13; color: "#888"}
                        Label{ text:content; font.pixelSize:14; color:"#333" }
                    }
                }

                ScrollBar.vertical: ScrollBar{}
            }
        }
    }
}
