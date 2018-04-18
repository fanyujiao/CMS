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
import "../others" as Others
Item {
    id: screen; width: parent.width; height: 475
    Rectangle {
        id: setting_widget
        anchors.fill: parent
//        color: "white"

        //背景
        Image {
            source: "../../img/skin/bg-middle.png"
            anchors.fill: parent
        }
        //更新列表
        function updateList() {
            funcmodel.clear();//清空
            funcmodel.append({"icon": "../../img/icons/drivercenter_1.2.8.png", "name": qsTr("驱动中心"), "flag": "drivercenter"});//驱动中心
            funcmodel.append({"icon": "../../img/icons/applications-system.png", "name": qsTr("系统配置管理"), "flag": "applications-system"});//系统配置管理
            funcmodel.append({"icon": "../../img/icons/automanager_1.0.png", "name": qsTr("启动项管理"), "flag": "automanager"});//启动项管理
            funcmodel.append({"icon": "../../img/icons/checkscreen.png", "name": qsTr("Dead pixel test"), "flag": "CheckScreen"});//坏点检测
            funcmodel.append({"icon": "../../img/icons/bootanimation.png", "name": qsTr("Boot Animation"), "flag": "BootAnimation"});//开机动画
            funcmodel.append({"icon": "../../img/icons/fcitx.png", "name": qsTr("Fcitx Configure"), "flag": "FcitxConfigtool"});//输入法配置
            funcmodel.append({"icon": "../../img/icons/processmanager.png", "name": qsTr("Task Manager"), "flag": "ProcessManager"});//任务管理器
        }

        Item {
            id: views
            width: parent.width
            height: parent.height
            ListModel {
                id: funcmodel
                Component.onCompleted: {
                    setting_widget.updateList();
                }
            }


            Common.Label {
                id: label
                text: qsTr("Default toolkits")//默认工具箱
                font.bold: true
                font.pixelSize: 16
                color: "#383838"
                anchors {
                    top: parent.top
                    topMargin: 30
                    left: parent.left
                    leftMargin: 35
                }
            }

            Item {
                width: parent.width - 30*2
                anchors {
                    top: label.bottom
                    topMargin: 10
                    left: parent.left
                    leftMargin: 30
                }
                GridView {
                    id: gridView
                    anchors.fill: parent
                    model: funcmodel
                    delegate: Others.SettingsDelegate {}
                    cacheBuffer: 1000
                    //竖列
                    cellWidth: (parent.width-2)/6; cellHeight: cellWidth
                    //横列
    //                cellWidth: (parent.width-2)/6; cellHeight: cellWidth
                    highlight: Image {source: "../../img/toolWidget/box-hover.png"}
//                    highlight: Rectangle { color: "lightsteelblue"; radius: 5 }//kobe:设置选中项深色块
                    focus: true
                }
            }
            Item { id: foreground; anchors.fill: parent }
        }
    }
}
