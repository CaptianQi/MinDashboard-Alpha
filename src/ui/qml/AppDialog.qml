import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Rectangle {
    id: root
    width: 360
    height: 200
    radius: 12
    color: "#F9FAFB"
    border.color: "#D0D5DD"
    border.width: 1

    // ====== 对外属性（由 C++ 设置）======
    // 标题（上面一行）
    property string title: "提示"
    // 中间内容文本
    property string message: ""
    // 确定按钮文字
    property string okText: "确定"
    // 取消按钮文字
    property string cancelText: "取消"
    // 是否显示【取消】按钮
    property bool showCancel: true

    // ====== 对外信号（C++ 来连）======
    signal accepted()   // 点击「确定」
    signal rejected()   // 点击「取消」

    // 1. 标题
    Text {
        x: parent.width/2 - width
        text: root.title
        font.pixelSize: 16
        font.bold: true
        color: "#263238"
        elide: Text.ElideRight
    }
    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 20
        spacing: 12
        // 分割线
        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: 1
            color: "#ECEFF1"
        }


        Rectangle{
            Layout.fillWidth:true
            Layout.fillHeight:true
            color: "#F9FAFB"
            // 2. 内容文本
            Text {
                id: msgText
                anchors.centerIn:parent
                text: root.message
                font.pixelSize: 13
                color: "#455A64"
                wrapMode: Text.WordWrap
            }
        }

        // 3. 底部按钮行
        RowLayout {
            Layout.fillWidth: true
            Layout.preferredHeight: 36
            Layout.alignment: Qt.AlignRight
            spacing: 10
            // 确定按钮
            Rectangle {
                id: btnOk
                Layout.alignment: Qt.AlignRight
                radius: 16
                border.color: "#CFD8DC"
                border.width: 1
                color:  "#1976D2"
                implicitWidth: 40
                implicitHeight: 25
                Text {
                    anchors.centerIn: parent
                    text: root.okText
                    color: "white"
                    font.pixelSize: 14
                }
                MouseArea {
                    anchors.fill: parent
                    hoverEnabled: true
                    onEntered: parent.scale = 0.95
                    onExited: parent.scale = 1.0
                    onClicked: root.accepted()
                }
            }
            //取消按钮 可选
            Rectangle {
                id: btnCancel
                visible: root.showCancel
                Layout.alignment: Qt.AlignRight
                radius: 16
                border.color: "#CFD8DC"
                border.width: 1
                color:  "transparent"
                implicitWidth: 40
                implicitHeight: 25
                Text {
                    anchors.centerIn: parent
                    text: root.cancelText
                    color: "#546E7A"
                    font.pixelSize: 14
                }
                MouseArea {
                    anchors.fill: parent
                    hoverEnabled: true
                    onEntered: parent.scale = 0.95
                    onExited: parent.scale = 1.0
                    onClicked: root.rejected()
                }
            }
        }
    }
}
