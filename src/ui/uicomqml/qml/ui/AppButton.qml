import QtQuick
import QtQuick.Controls

//通用组件 ------自定义按钮组件
Rectangle{
    id: root
    property alias text: label.text                 //外部可设置文字 将内部组件属性变为 “公有” 让外部可以访问
    property color baseColor: "#3498db"             // 主色
    property color hoverColor: Qt.lighter(baseColor,1.2)
    property color pressedColor: Qt.darker(baseColor,1.2)
    property color textColor: "white"
    property int cornerRadius: 6
    property int padding: 10

    signal clicked()

    implicitWidth: label.width+ padding*2
    implicitHeight: label.height+ padding

    color:baseColor
    radius:cornerRadius

    Text{
        id: label
        anchors.centerIn: parent
        font.pixelSize:14
        color:root.textColor
    }

    MouseArea{
        id: area
        anchors.fill: parent
        hoverEnabled:true  //检测鼠标悬停
        onPressed: root.color = pressedColor
        onReleased: root.color = baseColor
        onClicked: root.clicked()

        onEntered: root.color = hoverColor
        onExited: root.color= baseColor
    }

    //动态缩放反馈
    scale: area.pressed ? 0.95 : 1.0
    //缩放过程中的动画
    Behavior on scale{ NumberAnimation{ duration:120; easing.type:Easing.OutQuad} }

    //阴影
    // layer.enabled: true
    // layer.effect: DropShadow{

    // }
}
