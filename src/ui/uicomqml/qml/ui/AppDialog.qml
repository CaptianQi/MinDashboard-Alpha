import QtQuick
import QtQuick.Controls

// 弹窗组件： 包括遮罩层 + 中间面板
Rectangle{
    id: dialog
    anchors.fill:parent
    color:"#80000000"   //半透明黑色遮罩 50%左右透明
    visible: false  //默认不显示
    opacity: 0  //控制淡入淡出
    z:999  //在最上层

    //记录确定还是取消
    property bool _lastResult: false

    property alias title: titleText.text
    property alias message: messageText.text

    signal accepted()
    signal canceled()

    //中间白色对话款面板
    Rectangle{
        id: panel
        width:360
        height:200
        radius:10
        color: "white"
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.verticalCenter: parent.verticalCenter

        scale: 0.8   //初始缩小一点，动画时放大
        opacity: 0

        //点击面板外则退出的方法1  防止点击穿透，拦截事件
        // MouseArea{
        //     anchors.fill:parent
        //     acceptedButtons:Qt.LeftButton
        // }

        //  标题   消息
        Column{
            anchors.fill:panel
            anchors.margins:16
            spacing:16

            //标题
            Label{
                id:titleText
                text:"提示"
                font.pixelSize:18
                font.bold: true
            }

            //内容文本
            Label{
                id: messageText
                text: "对话框内容"
                wrapMode: Text.Wrap
                font.pixelSize:14
            }
        }
        //按钮行， 右下角对齐
        Row{
            id: buttonRow
            spacing:8
            anchors.right:parent.right
            anchors.bottom:parent.bottom
            anchors.margins:16

            Button{
                text:"确定"
                onClicked:{
                    // dialog.visible = false
                    // dialog.accepted()
                    dialog.close(true)
                }
            }

            Button{
                text:"取消"
                onClicked:{
                    // dialog.visible = false
                    // dialog.canceled()
                    dialog.close(true)
                }
            }
        }

    }

    //背景空白区域点击：关闭对话框 （可选）
    //方法1
    // MouseArea{
    //     anchors.fill: parent
    //     //只在点击 panel 外部时 触发
    //     z:-1
    //     onClicked:{
    //         //如果点击在面板外部才关闭
    //             dialog.visible = false
    //             dialog.canceled()
    //     }
    // }
    //方法2
    MouseArea{
        anchors.fill: parent
        z:-1   //背景的MouseArea放最前面，或者手动z:-1
        onClicked:(mouse)=>{            //把MouseArea的坐标，映射到panel
                    const localPoint = panel.mapFromItem(dialog,mouse.x,mouse.y)
                    //判断是否在panel内部
                    if(!panel.contains(localPoint)){
                        // dialog.visible = false
                        dialog.close(false)
                    }
                }
    }

    //打开动画
    SequentialAnimation{
        id:openAnim
        running:false

        ParallelAnimation{
            NumberAnimation{
                target: dialog
                property: "opacity"
                from: 0 ; to: 1
                duration: 200
            }
            NumberAnimation{
                target: panel
                property: "opacity"
                from: 0; to: 1
                duration: 250
            }
            NumberAnimation{
                target: panel
                property: "scale"
                from: 0.8; to:1
                duration: 250
                easing.type: Easing.OutBack
            }
        }
    }

    //关闭动画
    SequentialAnimation{
        id:closeAnim
        running:false

        ParallelAnimation{
            NumberAnimation{
                target:dialog
                property: "opacity"
                from:1 ; to: 0
                duration: 180
            }
            NumberAnimation{
                target: panel
                property: "opacity"
                from:1 ; to: 0
                duration: 180
            }
            NumberAnimation{
                target:panel
                property: "scale"
                from: 1; to: 0,8;
                duration: 180
                easing.type: Easing.InBack
            }

            //动画结束后隐藏 并发信号
            ScriptAction{
                script:{
                    dialog.visible = false
                    //重置到初始状态
                    dialog.opacity = 0
                    panel.opacity =0
                    panel.scale =0.8

                    if(dialog._lastResult)
                        dialog.accepted()
                    else
                        dialog.canceled()
                }
            }
        }
    }


    // 函数： 打开/关闭
    function open(t,msg){
        if(t!==undefined) title=t
        if(msg!==undefined) message =msg

        dialog.visible = true
        //每次打开前重置初始状态
        dialog.opacity = 0
        panel.opacity =0
        panel.scale =0.8

        openAnim.start()
    }

    function close(result){
        dialog._lastResult = (result ===true)
        closeAnim.start()
    }
}
