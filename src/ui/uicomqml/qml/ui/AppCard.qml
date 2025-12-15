import QtQuick
import QtQuick.Controls
import QtQuick.Effects

//通用组件  卡片组件 标题+内容区
Rectangle{
    id: card
    width:200; height:120
    radius: 8
    color: backgroundColor
    property alias title: titleLabel.text
    property alias content: contentLabel.text

    //=====自定义外观属性=======
    property color backgroundColor: "#ffffff"
    property color hoverColor: Qt.lighter(backgroundColor,1.1)
    property color borderColor: "#dcdcdc"
    property color titleColor: "#555"
    property color contentColor: "#222"
    property int shadowBlur:12

    //=====动画效果======
    scale: mouseArea.containsMouse ? 1.03:1.0
    Behavior on scale{ NumberAnimation{ duration:120; easing.type:Easing.OutQuad} }

    border.color:borderColor
    border.width:1

    //====阴影效果=======
    // layer.enabled:true
    // layer.effect: DropShaow{

    // }

    Column{
        anchors.centerIn: parent
        spacing:6

        Label{
            id: titleLabel
            text:"Title"
            font.pixelSize:14
            font.bold:true
            color: card.titleColor
        }
        Label{
            id: contentLabel
            text: "Content"
            font.pixelSize: 22
            color: card.contentColor
        }
    }

    MouseArea{
        id: mouseArea
        anchors.fill: parent
        hoverEnabled: true
        onEntered: card.color = card.hoverColor
        onExited: card.color = card.backgroundColor
    }
}
