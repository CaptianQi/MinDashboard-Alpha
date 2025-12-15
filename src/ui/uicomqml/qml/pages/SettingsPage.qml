import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import AppContext 1.0
import EnterpriseSuite 1.0
import "../ui"
Page {
    title: "Settings"

    Column {
        anchors.margins: 20
        anchors.fill: parent
        spacing: 12

        Row {
            spacing: 12
            Label { text: "theme"
            }
            ComboBox {
                id: themeBox
                model: ["light", "dark"]
                currentIndex: AppContext.theme === "light" ? 0 : 1
                onActivated: (i)=> {AppContext.theme = (i===0 ? "light" : "dark");
                             }
            }
        }

        //Button { text: "Show Toast"; onClicked: AppContext.showToast("Settings saved") }
        //改用 自定义组件
        AppButton{
            text: "Show Toast"
            baseColor: "#2ecc71"
            onClicked: AppContext.showToast("Settings saved")
        }

        AppButton{
            text: "Dialog"
            onClicked:{
                //myDialog.visible = true
                myDialog.open("标题","内容")
            }
        }

        AppInput {
            id: usernameInput
            placeholder: "用户名"
           // iconSource: "qrc:/EnterpriseSuite/icons/user.svg"
            required: true
            minLength: 3
            maxLength: 12
        }

        AppInput {
            id: passwordInput
            placeholder: "密码"
            echoMode: TextInput.Password
         //   iconSource: "qrc:/EnterpriseSuite/icons/lock.svg"
            required: true
            minLength: 6
        }

    }

    AppDialog{
        id:myDialog
        onAccepted: console.log("点击了确定")
        onCanceled: console.log("点击了取消或背景")
    }
}
