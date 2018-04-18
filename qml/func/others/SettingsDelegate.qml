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

import ProcessType 0.1//1101
import Apt 0.1
//import Signals 0.1

Rectangle {
    id: scaleMe

    scale: 1
    Behavior on scale { NumberAnimation { easing.type: Easing.InOutQuad} }
    //竖列高度和宽度
    width: 83//100
    height: 84//100
//    property int idata: 0
//    property string sname:""
//    property bool bname: false
    SystemPalette { id: myPalette; colorGroup: SystemPalette.Active }
    color: "transparent"


//    Apt{
//        id:apt8
//        onInstallProgress:{
//             scaleMe.sname = apt.getPagename(1)
//          //  console.debug(scaleMe.sname + "+++++++++++++++++++")
//            scaleMe.idata = apt.getProgress(0)
//            scaleMe.bname = true
//        }
//        onInstallPackageFinished: {
//          scaleMe.idata = 100
//            scaleMe.bname = false
//        }
//    }
//    DataShare{
//        id :data
//    }

    //竖列
    Column {
        anchors.centerIn: parent
        spacing: 10
        Image {
            id: seticon
            source: icon
            anchors.horizontalCenter: parent.horizontalCenter
        }
        Text {
            id: btnText
            anchors.horizontalCenter: parent.horizontalCenter
            color: "green"
            text: qsTr(name)//名字
        }
        //        Common.ProgressBar{
        //            id:progressbar

        //        }

    }

    Image {
        id: btnImg
        anchors.fill: parent
        source: ""
    }

    MouseArea {
        id: signaltest
        hoverEnabled: true
        anchors.fill: parent
        onEntered: btnImg.source = "../../img/toolWidget/menu_hover.png"
        onPressed: btnImg.source = "../../img/toolWidget/menu_press.png"
        //要判断松开是鼠标位置
        onReleased: btnImg.source = "../../img/toolWidget/menu_hover.png"
        onExited: btnImg.source = ""
        onClicked: {
            //系统配置管理
            if (flag == "applications-system")
            {
                sessiondispatcher.startApplicationsSystem(0,"cinnamon-settings");
            }
            //驱动中心
            else if (flag == "drivercenter")
            {
                //1)判断应用是否安装
                if(sessiondispatcher.checkInstalled("cdos-drivercenter") != ""){
                    //已安装
                    sessiondispatcher.startApplicationsSystem(1,"drivercenter");
                }else{
                    //2)未安装TODO
                    apt.initApt();
                    apt.installPackage("drivercenter")

                }
            }
            //启动项管理
            else if (flag == "automanager")
            {
                //1)判断应用是否安装
                if(sessiondispatcher.checkInstalled("cdos-autorunmanager") != ""){
                    //已安装
                    sessiondispatcher.startApplicationsSystem(1,"autorunmanager");
                }else{
                    //2)未安装
                    apt.initApt();
                    apt.installPackage("cdos-automanager");

                }
            }
            //坏点检测
            if (flag == "CheckScreen")
                sessiondispatcher.showCheckscreenDialog(mainwindow.pos.x, mainwindow.pos.y);
            else if (flag == "BootAnimation") {
                pageStack.push(bootanimationpage);
                //                var component_boot = Qt.createComponent("./settings/BootAnimation.qml");
                //                if (component_boot.status == Component.Ready) {
                //                    pageStack.push(component_boot);
                //                }
            }
            else if(flag == "FcitxConfigtool") {
                pageStack.push(fcitxConfigtoolpage);
            }
            else if(flag == "ProcessManager") {                
                pageStack.push(processmanagerpage);
            }
            //kobe:选中项深色块移动
            //scaleMe.GridView.view.currentIndex = index;
        }
    }
}
