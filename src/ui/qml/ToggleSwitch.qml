import QtQuick
import QtQuick.Controls

Rectangle {
    id: root
    width: 90
    height: 28
    radius: height / 2

    // 对外属性
    property bool checked: false
    property string textOn:  "ON"
    property string textOff: "OFF"
    signal toggled(bool checked)

    // 背景颜色
    color: checked ? "#4CAF50" : "#ECEFF1"       // ON 绿色, OFF 淡灰
    border.width: 1
    border.color: checked ? "#43A047" : "#CFD8DC"

    // 背景颜色动画
    Behavior on color {
        ColorAnimation { duration: 150; easing.type: Easing.OutCubic }
    }

    // 中间的文字 ON / OFF
    Text {
        id: label
        x: root.checked ? parent.width*0.1: (parent.width*0.9-label.width)
        y: (root.height/2)-label.height/2
        text: root.checked ? root.textOn : root.textOff
        font.pixelSize: 12
        font.bold: true
        color: root.checked ? "#FFFFFF" : "#9E9E9E"
    }

    // 圆形滑块
    Rectangle {
        id: knob
        width: root.height - 6
        height: width
        radius: width / 2
        y: (root.height - height) / 2
        x: root.checked ? root.width - width - 3 : 3
        color: "#FFFFFF"
        border.width: 1
        border.color: "#B0BEC5"

        Behavior on x {
            NumberAnimation { duration: 150; easing.type: Easing.OutCubic }
        }
    }

    // 点击切换
    MouseArea {
        anchors.fill: parent
        hoverEnabled: true

        onClicked: {
            root.checked = !root.checked
            root.toggled(root.checked)
        }
        onPressed: root.scale = 0.97
        onReleased: root.scale = 1.0
        onCanceled: root.scale = 1.0
    }
}

