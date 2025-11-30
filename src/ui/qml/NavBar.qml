import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Rectangle {
    id: root
    width: 220
    color: "#1F2430"
    implicitWidth: 150
    implicitHeight: 480

    // 当前选中的页面索引（外部 C++ 也可以改）
    property int currentIndex: 0

    // 通知 C++（MainWindow）切换 QStackedWidget 页
    signal pageRequested(int index)
    //退出信号
    signal close()

    // ===== 导航数据模型 =====
    ListModel {
        id: navModel
        ListElement { title: qsTr("数据"); iconSource: "qrc:/icons/dashboard.svg" }
        ListElement { title: qsTr("日志");   iconSource: "qrc:/icons/log.svg" }
        ListElement { title: qsTr("设置");   iconSource: "qrc:/icons/settings.svg" }
    }

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 8
        spacing: 8

        // ===== 顶部标题区 =====
        Item {
            Layout.fillWidth: true
            Layout.preferredHeight: 56

            RowLayout {
                anchors.fill: parent
                anchors.margins: 4

                Rectangle {
                    Layout.preferredWidth: 32
                    Layout.preferredHeight: 32
                    radius: 8
                    color: "#2F80ED"
                    Layout.alignment: Qt.AlignVCenter

                    Text {
                        anchors.centerIn: parent
                        text: "M"
                        color: "white"
                        font.bold: true
                        font.pixelSize: 15
                    }
                }

                ColumnLayout {
                    Layout.fillWidth: true
                    spacing: 2

                    Text {
                        text: qsTr("MinDashboard")
                        color: "white"
                        font.pixelSize: 13
                        font.bold: true
                        elide: Text.ElideRight
                    }

                    Text {
                        text: qsTr("监控与日志面板")
                        color: "#9297A3"
                        font.pixelSize: 11
                        elide: Text.ElideRight
                    }
                }
            }
        }

        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: 1
            color: "#323949"
        }

        // ===== 中间：导航列表 =====
        ListView {
            id: navList
            Layout.fillWidth: true
            Layout.fillHeight: true

            model: navModel
            spacing: 2
            interactive: false
            clip: true


            delegate: Rectangle {
                id: navItem

                property bool hovered: false

                width: ListView.view.width
                implicitHeight: 40

                // 背景：hover 或当前选中（isCurrentItem）时高亮
                color: hovered || ListView.isCurrentItem ? "#283143" : "transparent"

                // 左侧选中指示条
                Rectangle {
                    width: ListView.isCurrentItem ? 3 : 0
                    color: "#2F80ED"
                    anchors.top: parent.top
                    anchors.bottom: parent.bottom
                    anchors.left: parent.left
                }

                MouseArea {
                    anchors.fill: parent
                    hoverEnabled: true
                    onClicked: {
                        currentIndex = index
                    }
                    onEntered: navItem.hovered = true
                    onExited: navItem.hovered = false
                }

                RowLayout {
                    anchors.fill: parent
                    anchors.leftMargin: 12
                    anchors.rightMargin: 8
                    spacing: 8

                    // 图标：直接用模型 role iconSource
                    // Image {
                    //     source: model.iconSource
                    //     Layout.preferredWidth: 18
                    //     Layout.preferredHeight: 18
                    //     fillMode: Image.PreserveAspectFit
                    //     smooth: true
                    //     visible: model.iconSource !== ""
                    //     Layout.alignment: Qt.AlignVCenter
                    // }

                    // 文本：直接用模型 role title
                    Text {
                        Layout.fillWidth: true
                        text: title
                        color: ListView.isCurrentItem ? "white" : "#D0D3DC"
                        font.pixelSize: 15
                        elide: Text.ElideRight
                    }
                }
            }



            // 用 ListView 的 currentIndex 驱动选中项
            currentIndex: root.currentIndex

            // ListView 的 currentIndex 改变时，同步到 root，并发信号给 C++
            onCurrentIndexChanged: {
                root.currentIndex = currentIndex
                root.pageRequested(currentIndex)
            }

        }

        // ===== 底部 About / Exit =====
        ColumnLayout {
            Layout.fillWidth: true
            Layout.margins: 4
            spacing: 4

            Rectangle {
                Layout.fillWidth: true
                Layout.preferredHeight: 1
                color: "#323949"
            }

            // 退出
            Rectangle {
                id: exitItem
                property bool hovered: false
                Layout.fillWidth: true
                implicitHeight: 36
                color: hovered ? "#283143" : "transparent"

                MouseArea {
                    anchors.fill: parent
                    hoverEnabled: true
                    onClicked: root.close()   //退出信号
                    onEntered: exitItem.hovered = true
                    onExited: exitItem.hovered = false
                }

                RowLayout {
                    anchors.fill: parent
                    anchors.leftMargin: 12
                    anchors.rightMargin: 8
                    spacing: 8

                    Text {
                        Layout.fillWidth: true
                        text: qsTr("退出")
                        color: "#D0D3DC"
                        font.pixelSize: 14
                        elide: Text.ElideRight
                    }
                }
            }
        }
    }
}
