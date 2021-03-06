/*
 * Copyright (C) 2013 ~ 2014 National University of Defense Technology(NUDT) & cdos Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 3.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

import QtQuick 1.1
import "../common" as Common
import "../bars" as Bars
Rectangle {
    id: bootimagepage
    width: parent.width
    height: 475

    property int scrollbar_z:0
    property int lisv_height: 250
    property string image_path: ""
    property string actiontitle: qsTr("Boot animation settings")//开机动画设置
    property string actiontext: qsTr("Click the ' Custom Image ' button to select the picture you want to add, select the picture you want to set and click ' OK ' button to complete the setup.")//单击＂自定义图片＂按钮选择需要添加的图片，选中列表中要设置的图片名称，单击＂确定＂按钮完成设置。
    property int num: 0
    property string selectedimage: ""

    ListModel { id: mainModel }

    //背景
    Image {
        source: "../../img/skin/bg-middle.png"//bg-bottom-tab
        anchors.fill: parent
    }
    Component.onCompleted: {
        systemdispatcher.plymouth_init_check_qt();
        var plymouth_list = systemdispatcher.get_existing_plymouth_list_qt();
        bootimagepage.num = plymouth_list.length;
        mainModel.clear();
        for(var i=0; i < plymouth_list.length; i++) {
            if(systemdispatcher.get_image_path_qt(plymouth_list[i]) != "False"){
                mainModel.append({"itemTitle": plymouth_list[i]});
            }
        }
        if(30*plymouth_list.length<=lisv_height) {
            scrollbar_z = -1
        }
        else {
            scrollbar_z = 1;
        }

    }
    //信号绑定，绑定qt的信号finishCleanWork，该信号emit时触发onFinishCleanWork
    Connections
    {
        target: systemdispatcher
        onFinishAddBootImage: {
            systemdispatcher.plymouth_init_check_qt();
            var plymouth_list = systemdispatcher.get_existing_plymouth_list_qt();
            bootimagepage.num = plymouth_list.length;
            mainModel.clear();
            for(var i=0; i < plymouth_list.length; i++) {
                if(systemdispatcher.get_image_path_qt(plymouth_list[i]) != "False"){
                    mainModel.append({"itemTitle": plymouth_list[i]});
                }
            }
            bootimagepage.selectedimage = plymouth_list[0];
        }
    }

    Column {
        spacing: 10
        anchors.top: parent.top
        anchors.topMargin: 44
        anchors.left: parent.left
        anchors.leftMargin: 80
        Row {
            spacing: 50
            Text {
                 text: bootimagepage.actiontitle
                 font.bold: true
                 font.pixelSize: 14
                 color: "#383838"
             }
            //status picture
            Common.StatusImage {
                id: statusImage
                visible: false
                iconName: "green.png"
                text: qsTr("Completed!")//已完成！
                anchors.verticalCenter: parent.verticalCenter
            }
        }
        Text {
            width: bootimagepage.width - 80 - 20
            text: bootimagepage.actiontext
            wrapMode: Text.WordWrap
            font.pixelSize: 12
            color: "#7a7a7a"
        }
    }

    Column {
        anchors {
            top: parent.top
            topMargin: 120
            left: parent.left
            leftMargin: 60
        }
        spacing: 30
        Image {
            id: previewzone
            source: "../../img/icons/previewzone.png"
        }

        Common.Button {
            width: 94
            height: 29
            fontsize: 13
            hoverimage: "blue.png"
            text: qsTr("Custom picture")//自定义图片
            anchors.horizontalCenter: parent.horizontalCenter
            onClicked: {
                bootimagepage.image_path = systemdispatcher.showSelectFileDialog("bootanimation");
                bootimage.source = bootimagepage.image_path;
                var imagename = bootimagepage.image_path;
                imagename = imagename.substr(imagename.lastIndexOf("/") + 1, imagename.length - imagename.lastIndexOf("/"));
                systemdispatcher.add_new_plymouth_qt(bootimagepage.image_path, imagename);
                systemdispatcher.readyAddBootImageToList();
            }
        }
    }

    Image {
        id: bootimage
        width: 165
        height: 114
        anchors {
            top: parent.top
            topMargin: 140
            left: parent.left
            leftMargin: 80
        }
//        fillMode: Image.TileHorizontally
        source: ""
        smooth: true
    }

    Text {
        id: chooselabel
        anchors {
            top: parent.top
            topMargin: 120
            left: bootimage.right
            leftMargin: 100
        }
        text: qsTr("Optional BootAnimation")//可选的开机动画
        font.bold: true
        font.pixelSize: 14
        color: "#383838"
    }

    Rectangle{
        border.color: "#b9c5cc"
        width: 460; height: 250
        clip:true
        anchors {
            top: chooselabel.bottom
            topMargin: 5
            left: bootimage.right
            leftMargin: 100
        }
        Component{
            id:cdelegat
            Item{
                id:wrapper
                width: 440; height: 30
                Text{
                    id:listtext
                    anchors {
                        left: parent.left
                        leftMargin: 10
                        verticalCenter: parent.verticalCenter
                    }
                    font.pixelSize: 12
                    color: "#7a7a7a"
                    text:itemTitle
                }
                Image {
                    id: btnImg
                    anchors.fill: parent
                    source: ""
                }
                MouseArea{
                    anchors.fill:parent
                    hoverEnabled: true
                    onClicked: {
                        wrapper.ListView.view.currentIndex = index;
                        if(systemdispatcher.get_image_path_qt(itemTitle) != "False"){
                            bootimagepage.selectedimage = itemTitle;
                            bootimage.source = systemdispatcher.get_image_path_qt(itemTitle);
                        }
                        else{
                            //待添加原图片路径不存在时的说明图
                        }

                    }
                }
                Rectangle{
                    width: revoke.width;height: revoke.height; color: "transparent"
                    anchors{
                        verticalCenter: parent.verticalCenter
                        right: parent.right
                        rightMargin: 20
                    }
                    opacity: wrapper.ListView.isCurrentItem? 1:0
                    Image {id:revoke;source: "../../img/icons/revoke.png"}
                    MouseArea{
                        anchors.fill:parent
                        enabled:wrapper.ListView.isCurrentItem? true:false
                        onClicked: {
                            systemdispatcher.delete_plymouth_qt(itemTitle);
                            systemdispatcher.readyAddBootImageToList();
                        }
                    }
                }

            }
        }
        Common.ScrollArea {
            frame:false
            anchors{
                top:parent.top
                topMargin: 1
                left:parent.left
                leftMargin: 1
            }
            height: parent.height-1
            width: parent.width-1
            Item {
                width: parent.width
                height: bootimagepage.num * 30 //列表长度
                //垃圾清理显示内容
                ListView{
                    id:lisv
                    anchors.fill: parent
                    model: mainModel
                    delegate: cdelegat
                    highlight: Rectangle{width: 440;height: 30 ; color: "lightsteelblue"}
                    focus:true
                }
            }//Item
        }//ScrollArea
    }

    //顶层工具栏
    Bars.TopBar {
        id: topBar
        width: 28
        height: 26
        anchors.top: parent.top
        anchors.topMargin: 40
        anchors.left: parent.left
        anchors.leftMargin: 40
        opacity: 0.9
        onButtonClicked: {
            var num = sessiondispatcher.get_page_num();
            if (num == 0) {
                pageStack.push(homepage);
            }
            else if (num == 3) {
                pageStack.push(systemset);
            }
            else if (num == 4) {
                pageStack.push(functioncollection);
            }
        }
    }
    //底层工具栏
    Bars.ToolBar {
        id: toolBar
        height: 50; anchors.bottom: parent.bottom; width: parent.width; opacity: 0.9
        onQuitBtnClicked: {
            var num = sessiondispatcher.get_page_num();
            if (num == 0) {
                pageStack.push(homepage);
            }
            else if (num == 3) {
                pageStack.push(systemset);
            }
            else if (num == 4) {
                pageStack.push(functioncollection);
            }
        }
        onOkBtnClicked: {
            systemdispatcher.custom_plymouth_bg_qt(bootimagepage.selectedimage);
            statusImage.visible = true;
        }
        Timer {
            interval: 5000; running: true; repeat: true
            onTriggered: statusImage.visible = false
        }
    }
}
