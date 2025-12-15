import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Item {
    id: root
    width: 260
    height: 64   // 整体高度固定，包含输入框 + 错误提示

    // ==== 对外属性 ====
    property alias text: inputField.text
    property alias placeholder: inputField.placeholderText
    property alias echoMode: inputField.echoMode
    property url iconSource: ""
    property bool required: false
    property int minLength: 0
    property int maxLength: 9999
    property string errorText: ""
    property bool valid: true
    signal textChange(string value)

    // ==== 视觉样式 ====
    Rectangle {
        id: frame
        width: parent.width
        height: 40
        radius: 6
        color: "#ffffff"
        border.width: 1
        border.color: !root.valid ? "#F44336" : (inputField.activeFocus ? "#2196F3" : "#cccccc")
        Behavior on border.color { ColorAnimation { duration: 120 } }

        Row {
            id: contentRow
            anchors.fill: parent
            anchors.margins: 8
            spacing: icon.visible ? 6 : 0

            // 图标（可选）
            Image {
                id: icon
                source: root.iconSource
                visible: source !== ""
                width: 20; height: 20
                fillMode: Image.PreserveAspectFit
                anchors.verticalCenter: parent.verticalCenter
            }

            // 输入框
            TextField {
                id: inputField
                anchors.verticalCenter: parent.verticalCenter
                width: parent.width - (icon.visible ? icon.width + contentRow.spacing : 0)
                height: 26
                placeholderText: "请输入内容"
                background: null
                font.pixelSize: 14
                color: "#333"

                onTextChanged: {
                    root.textChange(text)
                    root.validate()
                }
            }
        }
    }

    // ==== 错误提示 ====
    Text {
        id: errorLabel
        text: root.errorText
        color: "#F44336"
        font.pixelSize: 12
        visible: !root.valid
        anchors.top: frame.bottom
        anchors.left: frame.left
        anchors.topMargin: 4
    }

    // ==== 验证逻辑 ====
    function validate() {
        const t = inputField.text.trim()
        if (required && t.length === 0) {
            errorText = "此项不能为空"
            valid = false
            return false
        }
        if (t.length < minLength) {
            errorText = "最少输入 " + minLength + " 个字符"
            valid = false
            return false
        }
        if (t.length > maxLength) {
            errorText = "最多输入 " + maxLength + " 个字符"
            valid = false
            return false
        }
        errorText = ""
        valid = true
        return true
    }
}
