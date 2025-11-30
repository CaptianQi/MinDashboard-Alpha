import QtQuick 6.5
import QtQuick.Controls 6.5
import QtQuick.Layouts 6.5

Item {
    id: root
    anchors.fill: parent

    // ======= 对外属性（当前配置） =======
    property string mode: "TCP"          // "TCP" / "UDP" / "串口"
    property string tcpHost: ""
    property int    tcpPort: 9000
    property int    udpPort: 9001
    property string serialName: "COM1"
    property int    baudBits: 115200
    property int    dataBits: 8
    property int    stopBits: 1
    property int    parity: 0

    property string savePath: ""        // 数据保存路径
    property string format: "CSV"        // 保存格式: "CSV" 或 "JSON"

    // ======= 对外信号（交给 C++ 处理） =======
    signal browseSavePathRequested()
    signal saveClicked()

    // ======= 整体卡片背景 =======
    Rectangle {
        anchors.fill: parent
        anchors.margins: 12
        radius: 14
        color: "#FAFBFD"
        border.color: "#E0E6EE"

        ColumnLayout {
            anchors.fill: parent
            anchors.margins: 16
            spacing: 18

            // ==================== 通信模式设置 ====================
            ColumnLayout {
                Layout.fillWidth: true
                Layout.bottomMargin:4
                spacing: 8

                Text {
                    text: qsTr("通信模式设置")
                    font.pixelSize: 14
                    font.bold: true
                    color: "#263238"
                }

                Rectangle {
                    Layout.fillWidth: true
                    Layout.preferredHeight: 215
                    radius: 10
                    color: "#FFFFFF"
                    border.color: "#E0E6EE"
                    border.width: 1

                    ColumnLayout {
                        anchors.fill: parent
                        anchors.margins: 14
                        spacing: 12

                        // 1 行：模式 + 下拉框
                        RowLayout {
                            Layout.fillWidth: true
                            spacing: 8

                            Label {
                                text: qsTr("模式：")
                                font.pixelSize: 13
                                color: "#37474F"
                            }

                            ComboBox {
                                id: modeCombo
                                Layout.preferredWidth: 80
                                model: [ "TCP", "UDP", "串口" ]
                                currentIndex: {
                                    if (root.mode === "TCP") return 0;
                                    if (root.mode === "UDP") return 1;
                                    return 2;
                                }

                                onCurrentTextChanged: {
                                    root.mode = currentText
                                }

                                contentItem: Text {
                                    text: modeCombo.currentText
                                    verticalAlignment: Text.AlignVCenter
                                    leftPadding: 10
                                    font.pixelSize: 13
                                    color: "#37474F"
                                }
                                background: Rectangle {
                                    radius: 6
                                    border.width: 1
                                    border.color: modeCombo.activeFocus ? "#1976D2" : "#CFD8DC"
                                    color: "#FFFFFF"
                                }
                            }

                            Item { Layout.fillWidth: true } // 占位
                        }

                        Rectangle {
                            Layout.fillWidth: true
                            Layout.preferredHeight: 1
                            color: "#E0E6EE"
                        }

                        // 2 行：根据模式切换的具体参数
                        StackLayout {
                            id: stack
                            Layout.fillWidth: true
                            Layout.fillHeight: true
                            currentIndex: modeCombo.currentIndex

                            // -------- TCP 页面 --------
                            Item {
                                Layout.fillWidth: true
                                Layout.fillHeight: true

                                GridLayout {
                                    anchors.fill: parent
                                    columns: 2
                                    columnSpacing: 12
                                    rowSpacing: 8

                                    Label {
                                        text: qsTr("IP 地址")
                                        font.pixelSize: 13
                                        color: "#455A64"
                                    }
                                    TextField {
                                        id: tcpIpInput
                                        Layout.fillWidth: true
                                        text: root.tcpHost
                                        placeholderText: qsTr("例如 192.168.1.100")
                                        font.pixelSize: 13

                                        onEditingFinished: {
                                            root.tcpHost = text.trim()
                                        }
                                    }

                                    Label {
                                        text: qsTr("端口号")
                                        font.pixelSize: 13
                                        color: "#455A64"
                                    }
                                    TextField {
                                        id: tcpPortInput
                                        Layout.fillWidth: true
                                        inputMethodHints: Qt.ImhDigitsOnly
                                        text: root.tcpPort.toString()
                                        placeholderText: "9000"
                                        font.pixelSize: 13

                                        onEditingFinished: {
                                            var p = parseInt(text)
                                            if (isNaN(p) || p <= 0) {
                                                p = root.tcpPort
                                                text = p.toString()
                                            }
                                            root.tcpPort = p
                                        }
                                    }
                                }
                            }

                            // -------- UDP 页面 --------
                            Item {
                                Layout.fillWidth: true
                                Layout.fillHeight: true

                                GridLayout {
                                    anchors.fill: parent
                                    columns: 2
                                    columnSpacing: 12
                                    rowSpacing: 8

                                    Label {
                                        text: qsTr("端口号")
                                        font.pixelSize: 13
                                        color: "#455A64"
                                    }
                                    TextField {
                                        id: udpPortInput
                                        Layout.fillWidth: true
                                        inputMethodHints: Qt.ImhDigitsOnly
                                        text: root.udpPort.toString()
                                        placeholderText: "9001"
                                        font.pixelSize: 13

                                        onEditingFinished: {
                                            var p = parseInt(text)
                                            if (isNaN(p) || p <= 0) {
                                                p = root.udpPort
                                                text = p.toString()
                                            }
                                            root.udpPort = p
                                        }
                                    }
                                }
                            }

                            // -------- 串口 页面 --------
                            Item {
                                Layout.fillWidth: true
                                Layout.fillHeight: true

                                GridLayout {
                                    anchors.fill: parent
                                    columns: 2
                                    columnSpacing: 12
                                    rowSpacing: 8

                                    Label {
                                        text: qsTr("串口号")
                                        font.pixelSize: 13
                                        color: "#455A64"
                                    }
                                    TextField {
                                        id: serialNameInput
                                        Layout.fillWidth: true
                                        text: root.serialName
                                        placeholderText: qsTr("例如 COM1")
                                        font.pixelSize: 13
                                        onEditingFinished: {
                                            root.serialName = text.trim()
                                        }
                                    }

                                    Label { text: qsTr("波特率"); font.pixelSize: 13; color: "#455A64" }
                                    ComboBox {
                                        id: baudCombo
                                        Layout.fillWidth: true
                                        model: [9600, 19200, 38400, 57600, 115200]
                                        currentIndex: Math.max(0, model.indexOf(root.baudBits))
                                        onCurrentTextChanged: {
                                            root.baudBits = parseInt(currentText)
                                        }
                                    }

                                    Label { text: qsTr("数据位"); font.pixelSize: 13; color: "#455A64" }
                                    ComboBox {
                                        id: dataBitsCombo
                                        Layout.fillWidth: true
                                        model: [7, 8]
                                        currentIndex: Math.max(0, model.indexOf(root.dataBits))
                                        onCurrentTextChanged: {
                                            root.dataBits = parseInt(currentText)
                                        }
                                    }

                                    Label { text: qsTr("停止位"); font.pixelSize: 13; color: "#455A64" }
                                    ComboBox {
                                        id: stopBitsCombo
                                        Layout.fillWidth: true
                                        model: [1, 2]
                                        currentIndex: Math.max(0, model.indexOf(root.stopBits))
                                        onCurrentTextChanged: {
                                            root.stopBits = parseInt(currentText)

                                        }
                                    }

                                    Label { text: qsTr("校验位"); font.pixelSize: 13; color: "#455A64" }
                                    ComboBox {
                                        id: parityCombo
                                        Layout.fillWidth: true
                                        // 0:无,1:奇,2:偶  只是示例
                                        model: [ qsTr("无"), qsTr("奇校验"), qsTr("偶校验") ]
                                        currentIndex: root.parity
                                        onCurrentIndexChanged: {
                                            root.parity = currentIndex

                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }

            // ==================== 数据保存地址 ====================
            ColumnLayout {
                Layout.fillWidth: true
                spacing: 8

                Text {
                    text: qsTr("数据保存地址")
                    font.pixelSize: 14
                    font.bold: true
                    color: "#263238"
                }

                Rectangle {
                    Layout.fillWidth: true
                    Layout.preferredHeight: 110
                    radius: 10
                    color: "#FFFFFF"
                    border.color: "#E0E6EE"
                    border.width: 1

                    ColumnLayout {
                        anchors.fill: parent
                        anchors.margins: 14
                        spacing: 8

                        // 第一行：路径 + 浏览
                        RowLayout {
                            Layout.fillWidth: true
                            spacing: 8

                            Label {
                                text: qsTr("路径")
                                font.pixelSize: 13
                                color: "#455A64"
                            }

                            TextField {
                                id: pathInput
                                Layout.fillWidth: true
                                text: root.savePath
                                placeholderText: qsTr("选择数据保存目录")
                                font.pixelSize: 13
                                onEditingFinished: {
                                    root.savePath = text.trim()
                                }
                            }

                            Button {
                                text: qsTr("浏览")
                                Layout.preferredWidth: 72
                                onClicked: root.browseSavePathRequested()
                            }
                        }

                        // 第二行：格式下拉框
                        RowLayout {
                            Layout.fillWidth: true
                            spacing: 8

                            Label {
                                text: qsTr("格式")
                                font.pixelSize: 13
                                color: "#455A64"
                            }

                            ComboBox {
                                id: formatCombo
                                Layout.preferredWidth: 80
                                model: [ "CSV", "JSON" ]

                                // 根据 root.format 决定当前项
                                currentIndex: root.format === "JSON" ? 1 : 0

                                onCurrentTextChanged: {
                                    root.format = currentText
                                }

                                contentItem: Text {
                                    text: formatCombo.displayText
                                    verticalAlignment: Text.AlignVCenter
                                    leftPadding: 10
                                    font.pixelSize: 13
                                    color: "#37474F"
                                }
                                background: Rectangle {
                                    radius: 6
                                    border.width: 1
                                    border.color: formatCombo.activeFocus ? "#1976D2" : "#CFD8DC"
                                    color: "#FFFFFF"
                                }
                            }

                            Item { Layout.fillWidth: true } // 拉开右侧空白
                        }
                    }
                }

            }

            // ==================== 右下角 保存按钮 ====================
            Item {
                Layout.fillWidth: true
                Layout.preferredHeight: 40
                Rectangle {
                    id: saveButton
                    anchors.right: parent.right
                    anchors.verticalCenter: parent.verticalCenter
                    focus:true  //可获得检点
                    radius: 16
                    border.color: "#CFD8DC"
                    border.width: 1
                    color:  "#1976D2"
                    implicitWidth: 40
                    implicitHeight: 25
                    Text {
                        anchors.centerIn: parent
                        text: qsTr("保存")
                        color: "white"
                        font.pixelSize: 14
                    }
                    MouseArea {
                        anchors.fill: parent
                        hoverEnabled: true
                        onEntered: parent.scale = 0.95
                        onExited: parent.scale = 1.0
                        onClicked: {
                            saveButton.forceActiveFocus();
                            root.saveClicked();
                        }
                    }
                }
            }
        }
    }
}
