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
import "./func/common" as Common
import "./func/others" as Others
import Apt 0.1
//import Signals 0.1

Rectangle {
    id: root
    width: parent.width
    height: 30
    color: "transparent"
    property string version: ""//版本号
    property bool visiblevalue: false
    property int prodata: 0
    property string  probar_text: ""
    //    Timer {
    //        id:protimer_t
    //        interval: 50;
    //        running: true;
    //        repeat: true
    //        onTriggered: {
    ////            spro_c.visiblevalue=datashare.install_status
    ////            spro_c.prodata  = datashare.progressdata
    ////            spro_c.probar_text =  datashare.install_pagagename + "  installing  " + datashare.progressdata + "%"


    //         //   state:"spro_s"
    //        }
    //    }

    Connections
    {
        target: sessiondispatcher
        onTellDownloadCloudConfToQML: {
            if(download == "download_norun") {
                //                root.downcloud = false;
                downloaddynamic.paused = true;
                downloaddynamic.playing = false;
                sessiondispatcher.showWarningDialog(qsTr("Tips:"), qsTr("The kuaipan4uk is not running!"), mainwindow.pos.x, mainwindow.pos.y);
            }
            else if(download == "download_notconf") {
                //                root.downcloud = false;
                downloaddynamic.paused = true;
                downloaddynamic.playing = false;
                sessiondispatcher.showWarningDialog(qsTr("Tips:"), qsTr("Not found the cloud configuration!"), mainwindow.pos.x, mainwindow.pos.y);
            }
            else if(download == "download_ok") {
                //                root.downcloud = false;
                downloaddynamic.paused = true;
                downloaddynamic.playing = false;
                toolkits.alertMSG(qsTr("Download OK!"), mainwindow.pos.x, mainwindow.pos.y);//使用完毕！
            }
        }
        onTellUploadCloudConfToQML: {
            if(upload == "upload_norun") {
                //                root.upcloud = false;
                uploaddynamic.paused = true;
                uploaddynamic.playing = false;
                sessiondispatcher.showWarningDialog(qsTr("Tips:"), qsTr("The kuaipan4uk is not running!"), mainwindow.pos.x, mainwindow.pos.y);
            }
            else if(upload == "upload_ok") {
                //                root.upcloud = false;
                uploaddynamic.paused = true;
                uploaddynamic.playing = false;
                toolkits.alertMSG(qsTr("Upload OK!"), mainwindow.pos.x, mainwindow.pos.y);//上传完成！
            }
        }
    }

    Component.onCompleted: {
    }
    //    Others.DataShare{
    //        id : datashare
    //    }
    Row {
        anchors {
            left: parent.left
            leftMargin: 10
            verticalCenter: parent.verticalCenter
        }
        spacing: 5
        Image {
            source: "./img/icons/arrowhead.png"
        }
        Text {
            color: "white"
            font.pixelSize: 12
            text: qsTr("main version:") + root.version//主版本：
        }
    }

    Connections
    {
        target: apt
        onInstallProgress:{
            root.probar_text = apt.getPagename(1) + " 安装中. . .  " + apt.getProgress(0) + "%"
            //  console.debug(scaleMe.sname + "+++++++++++++++++++")
            root.prodata = apt.getProgress(0)
            root.visiblevalue = true
        }
        onInstallPackageFinished: {
            root.prodata = 100

            root.probar_text = "安装完成！" + "100%"
            if (apt.getStatus() == "exit-failed")
            {
                root.probar_text = "安装错误！                                                                                                                                "
            }
            prot.start();
        }
    }
    Timer{
        id:prot
        interval: 3000;
        running: true;
        repeat: false
        onTriggered: {
            root.visiblevalue = false
        }
    }
    Rectangle{
        id :progressbar
        width: root.prodata * 8.5
        height: 32
        color: "lightgreen"
        radius: 3
        visible: root.visiblevalue
        Text{
            anchors.right: parent.right
            anchors.verticalCenter: parent.verticalCenter
            text:root.probar_text
        }
    }

    //    Others.SignalProgressBar{
    //        id : spro_c
    //        width: parent.width
    //        height: parent.height

    //        //   visible: datashare.install_status
    //        visible: root.visiblevalue
    //        anchors {
    //            left:parent.left
    //            verticalCenter: parent.verticalCenter
    //        }

    //        r:root.prodata
    //        protext: root.probar_text



    //    }



    //        states: [
    //            State {
    //                name: "spro_s"
    //                when:datashare.install_status == true
    //                PropertyChanges { target: spro_c ;visible : true }

    //            }
    //        ]

    //    MouseArea {
    //        id: mouseRegion
    //        anchors.fill: parent
    //        property variant clickPos: "1,1"
    //        onPressed: {
    //            clickPos  = Qt.point(mouse.x,mouse.y)
    //        }
    //        onPositionChanged: {
    //            var delta = Qt.point(mouse.x-clickPos.x, mouse.y-clickPos.y)
    //            mainwindow.pos = Qt.point(mainwindow.pos.x+delta.x, mainwindow.pos.y+delta.y)
    //        }
    //    }
    //    Image {
    //        id: downloadImage
    //        anchors {
    //            right: parent.right
    //            rightMargin: 5
    //            verticalCenter: parent.verticalCenter

    //        }
    //        width: 16
    //        height: 16
    //        source: "./img/icons/move.png"
    //        MouseArea {
    //            anchors.fill: downloadImage
    //            onClicked: {
    //                console.log("1111111111");
    //                sessiondispatcher.showSkinCenter(/*mainwindow.pos.x, mainwindow.pos.y*/);
    //            }
    //        }
    //    }


    Row {
        anchors {
            right: parent.right
            rightMargin: 3
            verticalCenter: parent.verticalCenter
        }
        spacing: 10
        Text {
            id: b1
            text: qsTr("Use Cloud")//使用云配置
            visible: false
        }
        Text {
            id: b2
            text: qsTr("Save Cloud")//保存云配置
            visible: false
        }
    }

    //    Row {
    //        anchors {
    //            right: parent.right
    //            rightMargin: 5
    //            verticalCenter: parent.verticalCenter
    //        }
    //        spacing: 10
    //        Row {
    //            Text {
    //                id: downBtn
    //                anchors.verticalCenter: parent.verticalCenter
    //                color: "white"
    //                font.pixelSize: 12
    //                text: qsTr("Use Cloud Conf")//使用云配置
    //                MouseArea {
    //                    anchors.fill: downBtn
    //                    onClicked: {
    ////                        root.downcloud = !root.downcloud;
    ////                        console.debug("00000");
    ////                        if(root.downcloud) {
    ////                            console.debug("1111");
    ////                            downloaddynamic.playing = true;
    ////                            downloaddynamic.paused = false;
    ////                        }
    ////                        else {
    ////                            console.debug("2222");
    ////                            downloaddynamic.paused = true;
    ////                            downloaddynamic.playing = false;
    ////                        }
    //                        downloaddynamic.playing = true;
    //                        downloaddynamic.paused = false;
    //                        sessiondispatcher.download_kysoft_cloud_conf_qt();
    //                    }
    //                }
    //            }
    //            AnimatedImage {//动态图片
    //                id: downloaddynamic
    //                playing: false
    //                paused: true
    //                width: 25
    //                height: 25
    //                source: "./img/icons/download.gif"
    //            }
    //        }
    //        Row {
    //            Text {
    //                id: upBtn
    //                anchors.verticalCenter: parent.verticalCenter
    //                color: "white"
    //                font.pixelSize: 12
    //                text: qsTr("Upload Cloud Conf")//上传云配置
    //                MouseArea {
    //                    anchors.fill: upBtn
    //                    onClicked: {
    ////                        root.upcloud = !root.upcloud;
    ////                        console.debug("00000");
    ////                        if(root.upcloud) {
    ////                            console.debug("1111");
    ////                            uploaddynamic.playing = true;
    ////                            uploaddynamic.paused = false;
    ////                        }
    ////                        else {
    ////                            console.debug("2222");
    ////                            uploaddynamic.paused = true;
    ////                            uploaddynamic.playing = false;
    ////                        }
    //                        uploaddynamic.playing = true;
    //                        uploaddynamic.paused = false;
    //                        sessiondispatcher.upload_kysoft_cloud_conf_qt();
    //                    }
    //                }
    //            }
    //            AnimatedImage {//动态图片
    //                id: uploaddynamic
    ////                visible: root.upcloud ? true : false
    //                width: 25
    //                height: 25
    //                playing: false
    //                paused: true
    //                source: "./img/icons/upload.gif"
    //            }
    //        }
    //    }


    //    Row {
    //        anchors {
    //            right: parent.right
    //            rightMargin: 5
    //            verticalCenter: parent.verticalCenter
    //        }
    //        spacing: 10
    //        Row {
    //            Text {
    //                id: downBtn
    //                anchors.verticalCenter: parent.verticalCenter
    //                color: "white"
    //                font.pixelSize: 12
    //                text: qsTr("Use Cloud Conf")//使用云配置
    //                MouseArea {
    //                    anchors.fill: downBtn
    //                    onClicked: {
    //                        root.downcloud = true;
    //                        sessiondispatcher.download_kysoft_cloud_conf_qt();
    //                    }
    //                }
    //            }
    //            AnimatedImage {//动态图片
    //                id: downloaddynamic
    //                visible: root.downcloud ? false : true
    //                width: 25
    //                height: 25
    //                source: "./img/icons/download.png"
    //            }
    ////            Image {
    ////                id: downloadImage
    ////                visible: root.downcloud ? false : true
    ////                width: 25
    ////                height: 25
    ////                source: "./img/icons/download.png"
    ////            }
    ////            AnimatedImage {//动态图片
    ////                id: downloaddynamic
    ////                visible: root.downcloud ? true : false
    ////                width: 16
    ////                height: 16
    ////                source: "./img/icons/move.gif"
    ////            }
    //        }
    //        Row {
    //            Text {
    //                id: upBtn
    //                anchors.verticalCenter: parent.verticalCenter
    //                color: "white"
    //                font.pixelSize: 12
    //                text: qsTr("Upload Cloud Conf")//上传云配置
    //                MouseArea {
    //                    anchors.fill: upBtn
    //                    onClicked: {
    //                        root.upcloud = true;
    //                        sessiondispatcher.upload_kysoft_cloud_conf_qt();
    //                    }
    //                }
    //            }
    //            Image {
    //                id: uploadImage
    //                visible: root.upcloud ? false : true
    //                width: 25
    //                height: 25
    //                source: "./img/icons/upload.png"
    //            }
    //            AnimatedImage {//动态图片
    //                id: uploaddynamic
    //                visible: root.upcloud ? true : false
    //                width: 16
    //                height: 16
    //                source: "./img/icons/move.gif"
    //            }
    //        }
    //    }
}
