import QtQuick 2.15

Item {
    id: root
    width: 220
    height: 220

    // 对外暴露的属性
    property real minValue: 0
    property real maxValue: 100
    property real value: 37
    property string label: "温度"
    property string unit: "℃"

    // 刻度数量
    property int majorTickCount: 6       // 大刻度
    property int minorTickCount: 4       // 每两个大刻度之间的小刻度数

    // 仪表角度范围（不是整圆）
    property real startAngle: -210       // 左侧开始角度（度）
    property real endAngle: 30           // 右侧结束角度（度）

    // 内部映射：值 -> 角度
    function valueToAngle(v) {
        var ratio = (v - minValue) / (maxValue - minValue)
        if (ratio < 0) ratio = 0
        if (ratio > 1) ratio = 1
        return startAngle + ratio * (endAngle - startAngle)
    }

    Rectangle {
        anchors.fill: parent
        radius: width/2
        color: "#0B1120"            // 背景
        border.color: "#111827"
        border.width: 1
    }

    // 使用 Canvas 画背景弧和当前值弧
    Canvas {
        id: dialCanvas
        anchors.fill: parent
        onPaint: {
            var ctx = getContext("2d")
            var w = width
            var h = height
            var r = Math.min(w, h) * 0.42
            var cx = w / 2
            var cy = h / 2

            ctx.reset()
            ctx.translate(cx, cy)
            ctx.rotate(-Math.PI / 2) // 让 0 度在垂直方向

            var sa = (root.startAngle) * Math.PI / 180
            var ea = (root.endAngle) * Math.PI / 180
            var av = valueToAngle(root.value) * Math.PI / 180

            // 背景弧
            ctx.lineWidth = 10
            ctx.strokeStyle = "#1F2937"
            ctx.beginPath()
            ctx.arc(0, 0, r, sa, ea, false)
            ctx.stroke()

            // 当前值弧
            var grad = ctx.createLinearGradient(-r, 0, r, 0)
            grad.addColorStop(0, "#22C55E")
            grad.addColorStop(0.5, "#EAB308")
            grad.addColorStop(1, "#EF4444")

            ctx.strokeStyle = grad
            ctx.beginPath()
            ctx.arc(0, 0, r, sa, av, false)
            ctx.stroke()
        }

        Connections {
            target: root
            function onValueChanged() { dialCanvas.requestPaint() }
            function onMinValueChanged() { dialCanvas.requestPaint() }
            function onMaxValueChanged() { dialCanvas.requestPaint() }
            function onStartAngleChanged() { dialCanvas.requestPaint() }
            function onEndAngleChanged() { dialCanvas.requestPaint() }
        }
    }

    // 刻度（用 Repeater + 矩形）
    Item {
        id: tickLayer
        anchors.fill: parent

        Repeater {
            id: majorTicks
            model: root.majorTickCount
            delegate: Rectangle {
                width: 2
                height: 10
                radius: 1
                color: "#9CA3AF"
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.verticalCenter: parent.verticalCenter
                transform: [
                    Rotation {
                        origin.x: 0; origin.y: 0
                        angle: root.startAngle
                               + index * (root.endAngle - root.startAngle)
                                 / (root.majorTickCount - 1)
                    },
                    Translate {
                        x: 0
                        y: -root.height * 0.36
                    }
                ]
            }
        }

        Repeater {
            id: minorTicks
            model: (root.majorTickCount - 1) * root.minorTickCount
            delegate: Rectangle {
                width: 1
                height: 6
                radius: 0.5
                color: "#4B5563"
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.verticalCenter: parent.verticalCenter
                transform: [
                    Rotation {
                        origin.x: 0; origin.y: 0
                        angle: root.startAngle
                               + (index + 1) * (root.endAngle - root.startAngle)
                                 / ((root.majorTickCount - 1) * (root.minorTickCount + 1))
                    },
                    Translate {
                        x: 0
                        y: -root.height * 0.36
                    }
                ]
            }
        }
    }

    // 指针
    Rectangle {
        id: needle
        width: 4
        height: root.height * 0.32
        radius: 2
        color: "#E5E7EB"
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.verticalCenter: parent.verticalCenter
        antialiasing: true

        transform: Rotation {
            id: needleRotation
            origin.x: 0
            origin.y: needle.height * 0.75
            angle: root.valueToAngle(root.value)
        }

        Behavior on needleRotation.angle {
            NumberAnimation { duration: 150; easing.type: Easing.OutQuad }
        }
    }

    // 中心圆 & 边框
    Rectangle {
        width: 20
        height: 20
        radius: 10
        color: "#0F172A"
        border.color: "#6B7280"
        border.width: 2
        anchors.centerIn: parent
    }

    // 文本：当前值、单位、标签
    Column {
        spacing: 2
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 20

        Text {
            text: Number(root.value).toFixed(1) + " " + root.unit
            color: "white"
            font.pixelSize: 20
            font.bold: true
            horizontalAlignment: Text.AlignHCenter
            anchors.horizontalCenter: parent.horizontalCenter
        }

        Text {
            text: root.label
            color: "#9CA3AF"
            font.pixelSize: 12
            horizontalAlignment: Text.AlignHCenter
            anchors.horizontalCenter: parent.horizontalCenter
        }
    }
}
