import QtQuick 6.5
import QtQuick.Controls 6.5
import QtQuick.Layouts 6.5
import QtQuick.Dialogs 6.5

Item {
    id: root
    anchors.fill: parent

    // 导出文件对话框（系统风格）
    FileDialog {
        id: saveDialog
        fileMode: FileDialog.SaveFile
        nameFilters: ["Text Files (*.txt)", "All Files (*)"]
        defaultSuffix: "txt"
        onAccepted: {
            if (selectedFile) {
                // 调 C++ 的 logPage.exportLogs(url)
                logPage.exportLogs(selectedFile);
            }
        }
    }

    // 整体浅色背景
    Rectangle {
        anchors.fill: parent
        color: "#F3F5F9"
    }

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 16
        spacing: 12

        // 顶部标题 + 操作区
        RowLayout {
            Layout.fillWidth: true
            spacing: 12

            ColumnLayout {
                spacing: 2
                Label {
                    text: qsTr("日志中心")
                    font.pixelSize: 20
                    font.bold: true
                    color: "#263238"
                }
                Label {
                    text: qsTr("查看运行时日志，清空与导出")
                    font.pixelSize: 12
                    color: "#78909C"
                }
            }

            Item { Layout.fillWidth: true }

            // 清空按钮
            Rectangle {
                id: btnCancel
                radius: 16
                border.color: "#CFD8DC"
                border.width: 1
                color:  "transparent"
                implicitWidth: 40
                implicitHeight: 25
                Text {
                    anchors.centerIn: parent
                    text: qsTr("清空")
                    color: "#546E7A"
                    font.pixelSize: 14
                }
                MouseArea {
                    anchors.fill: parent
                    hoverEnabled: true
                    onEntered: parent.scale = 0.95
                    onExited: parent.scale = 1.0
                    onClicked: logPage.clearLogs()
                }
            }
            // 确定按钮
            Rectangle {
                id: btnOk
                radius: 16
                Layout.rightMargin:10
                border.color: "#CFD8DC"
                border.width: 1
                color:  "#1976D2"
                implicitWidth: 40
                implicitHeight: 25
                Text {
                    anchors.centerIn: parent
                    text: qsTr("导出")
                    color: "white"
                    font.pixelSize: 14
                }
                MouseArea {
                    anchors.fill: parent
                    hoverEnabled: true
                    onEntered: parent.scale = 0.95
                    onExited: parent.scale = 1.0
                    onClicked: saveDialog.open()
                }
            }
        }

        //  TableView
        Rectangle {
            Layout.fillWidth: true
            Layout.fillHeight: true
            radius: 14
            color: "#FFFFFF"
            border.color: "#E0E6EE"

            layer.enabled: true  //开启图层渲染
            layer.samples: 4    //抗锯齿

            ColumnLayout {
                anchors.fill: parent
                anchors.margins: 12
                spacing: 8

                // 表头（自定义一行，而不是用原始 Header）
                RowLayout {
                    Layout.fillWidth: true
                    Layout.maximumHeight:10
                    spacing: 0

                    Rectangle {
                        Layout.preferredWidth: 170
                        Layout.fillHeight: true
                        color: "transparent"
                        Label {
                            anchors.verticalCenter: parent.verticalCenter
                            anchors.horizontalCenter:parent.horizontalCenter
                           // anchors.left: parent.left
                            //anchors.leftMargin: 8
                            text: qsTr("时间")
                            font.pixelSize: 12
                            color: "#90A4AE"
                        }
                    }

                    Rectangle {
                        Layout.preferredWidth: 100
                        Layout.fillHeight: true
                        color: "transparent"
                        Label {
                            anchors.verticalCenter: parent.verticalCenter
                            anchors.horizontalCenter:parent.horizontalCenter
                            // anchors.left: parent.left
                            // anchors.leftMargin: 8
                            text: qsTr("级别")
                            font.pixelSize: 12
                            color: "#90A4AE"
                        }
                    }

                    Rectangle {
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        color: "transparent"
                        Label {
                            anchors.horizontalCenter:parent.horizontalCenter
                            anchors.verticalCenter: parent.verticalCenter
                           // anchors.left: parent.left
                          //  anchors.leftMargin: 8
                            text: qsTr("消息")
                            font.pixelSize: 12
                            color: "#90A4AE"
                        }
                    }
                }

                Rectangle {
                    Layout.fillWidth: true
                    Layout.preferredHeight: 1
                    color: "#ECEFF1"
                }


                TableView {
                    id: table
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    // 建议先把 Layout.rightMargin / topMargin 这些挪到外层卡片上去
                    // 这里就让表格自己占满

                    clip: true
                    model: logModel   // C++

                    // 正常滚动：允许拖动和滚轮，但不要橡皮筋
                    interactive: true
                    boundsBehavior: Flickable.StopAtBounds

                    columnSpacing: 0
                    rowSpacing: 0

                    // 每列宽度
                    columnWidthProvider: function(col) {
                        if (col === 0)      return 170;   // 时间
                        else if (col === 1) return 100;   // 级别
                        else                return Math.max(width - 170 - 100, 200);  // 内容列
                    }

                    // 右侧整体边界线：跟随内容宽度
                    Rectangle {
                        anchors.top: parent.top
                        anchors.bottom: parent.bottom
                        width: 1
                        color: "#E0E0E0"
                        x: table.contentWidth - width
                    }

                    // 单元格 delegate
                    delegate: Rectangle {
                        implicitHeight: 30

                        // 交替行背景（淡淡的条纹）
                        color: row % 2 === 0 ? "#FFFFFF" : "#F7F9FC"

                        // 横向线：只画底部
                        border.width: 1
                        border.color: "#E0E0E0"

                        // 竖线：每一列右侧都画
                        Rectangle {
                            anchors.top: parent.top
                            anchors.bottom: parent.bottom
                            anchors.right: parent.right
                            width: 1
                            color: "#E0E0E0"
                        }

                        // ================= 级别列（column == 1）彩色 Tag =================
                        Rectangle {
                            anchors.fill: parent
                            anchors.margins: column === 1 ? 6 : 0
                            radius: column === 1 ? height / 2 : 0
                            visible: column === 1

                            // 根据等级文字 display 决定颜色
                            color: {
                                if (display === "ERROR") return "#FDE8E8";
                                if (display === "WARN")  return "#FFF4D5";
                                if (display === "INFO")  return "#E3F2FD";
                                if (display === "DEBUG") return "#ECEFF1";
                                return "transparent";
                            }
                            border.width: column === 1 ? 1 : 0
                            border.color: {
                                if (display === "ERROR") return "#F44336";
                                if (display === "WARN")  return "#FFB300";
                                if (display === "INFO")  return "#2196F3";
                                if (display === "DEBUG") return "#90A4AE";
                                return "transparent";
                            }

                            Text {
                                anchors.centerIn: parent
                                text: display
                                font.pixelSize: 11
                                color: {
                                    if (display === "ERROR") return "#C62828";
                                    if (display === "WARN")  return "#E65100";
                                    if (display === "INFO")  return "#1565C0";
                                    if (display === "DEBUG") return "#546E7A";
                                    return "#607D8B";
                                }
                            }
                        }

                        // ================= 时间列（column == 0）普通 Text =================
                        Text {
                            visible: column === 0
                            anchors.verticalCenter: parent.verticalCenter
                            anchors.left: parent.left
                            anchors.leftMargin: 8
                            anchors.right: parent.right
                            anchors.rightMargin: 8

                            text: display          // timeStr
                            font.pixelSize: 13
                            color: "#37474F"
                            horizontalAlignment: Text.AlignHCenter
                            elide: Text.ElideRight
                        }

                        // ================= 内容列（column == 2）→ 可选中、可横向拖拽 =================
                        TextInput {
                            visible: column === 2
                            anchors.fill: parent
                            anchors.leftMargin: 8
                            anchors.rightMargin: 8
                            anchors.topMargin: 6
                            anchors.bottomMargin: 6

                            text: display                 // 日志内容
                            font.pixelSize: 13
                            color: "#37474F"

                            readOnly: true                // 只读，不允许修改
                            selectByMouse: true           // 鼠标拖选
                            wrapMode: TextInput.NoWrap    // 不换行，超出就水平滚动
                            clip: true                    // 超出单元格部分裁掉

                            cursorVisible: false          // 不想看到光标就 false；想看就 true
                            activeFocusOnPress: true      // 点击时获取焦点，方便用左右键移动
                            inputMethodHints: Qt.ImhNoPredictiveText | Qt.ImhNoAutoUppercase
                        }
                    }

                    // 不需要滚动条的话就直接隐藏
                    ScrollBar.vertical: ScrollBar {
                        visible: false
                    }
                }
            }
        }
    }
}
