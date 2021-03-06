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
    id: desktopiconsetpage
    width: parent.width
    height: 475

    property int current_index//当前主题的索引
    property int default_index//系统默认主题的索引
    property string actiontitle: qsTr("Desktop Icons")//桌面图标设置
    property string actiontext: qsTr("Set the desktop icon theme and the visibility of desktop icons.")//设置桌面图标主题和桌面图标的可见性

    ListModel { id: choices }

    //背景
    Image {
        source: "../../img/skin/bg-middle.png"
        anchors.fill: parent
    }

    //使用云配置后，控件状态根据配置发生相应的变化
    Connections
    {
        target: sessiondispatcher
        onTellDownloadCloudConfToQML: {
            if(download == "icon_theme") {
                var iconlist = sessiondispatcher.get_icon_themes_qt();
                var current_icon_theme = sessiondispatcher.get_icon_theme_qt();
                for(var i=0; i < iconlist.length; i++) {
                    if (iconlist[i] == current_icon_theme) {
                        desktopiconsetpage.current_index = i;
                        break;
                    }
                }
                iconcombo.selectedIndex = desktopiconsetpage.current_index;
            }
            else if(download == "show_desktop_icons") {
                if (sessiondispatcher.get_show_desktop_icons_qt()) {
                    iconswitcher.switchedOn = true;
                }
                else {
                    iconswitcher.switchedOn = false;
                }
            }
            else if(download == "show_homefolder") {
                if (sessiondispatcher.get_show_homefolder_qt()) {
                    folderswitcher.switchedOn = true;
                }
                else {
                    folderswitcher.switchedOn = false;
                }
            }
            else if(download == "show_network") {
                if (sessiondispatcher.get_show_network_qt()) {
                    networkswitcher.switchedOn = true;
                }
                else {
                    networkswitcher.switchedOn = false;
                }
            }
            else if(download == "show_trash") {
                if (sessiondispatcher.get_show_trash_qt()) {
                    trashswitcher.switchedOn = true;
                }
                else {
                    trashswitcher.switchedOn = false;
                }
            }
            else if(download == "show_devices") {
                if (sessiondispatcher.get_show_devices_qt()) {
                    deviceswitcher.switchedOn = true;
                }
                else {
                    deviceswitcher.switchedOn = false;
                }
            }
        }
    }

    Component.onCompleted: {
        var iconlist = sessiondispatcher.get_icon_themes_qt();
        var current_icon_theme = sessiondispatcher.get_icon_theme_qt();
        var default_theme = sessiondispatcher.get_default_theme_sring_qt("icontheme");
        choices.clear();
        if(current_icon_theme == default_theme) {
            for(var i=0; i < iconlist.length; i++) {
                choices.append({"text": iconlist[i]});
                if (iconlist[i] == current_icon_theme) {
                    desktopiconsetpage.current_index = i;
                    desktopiconsetpage.default_index = i;
                }
            }
        }
        else {
            for(var j=0; j < iconlist.length; j++) {
                choices.append({"text": iconlist[j]});
                if (iconlist[j] == current_icon_theme) {
                    desktopiconsetpage.current_index = j;
                }
                else if (iconlist[j] == default_theme) {
                    desktopiconsetpage.default_index = j;
                }
            }
        }
        iconcombo.selectedIndex = desktopiconsetpage.current_index;


        if (sessiondispatcher.get_show_desktop_icons_qt()) {
            iconswitcher.switchedOn = true;
        }
        else {
            iconswitcher.switchedOn = false;
        }

        if (sessiondispatcher.get_show_homefolder_qt()) {
            folderswitcher.switchedOn = true;
        }
        else {
            folderswitcher.switchedOn = false;
        }

        if (sessiondispatcher.get_show_network_qt()) {
            networkswitcher.switchedOn = true;
        }
        else {
            networkswitcher.switchedOn = false;
        }

        if (sessiondispatcher.get_show_trash_qt()) {
            trashswitcher.switchedOn = true;
        }
        else {
            trashswitcher.switchedOn = false;
        }

        if (sessiondispatcher.get_show_devices_qt()) {
            deviceswitcher.switchedOn = true;
        }
        else {
            deviceswitcher.switchedOn = false;
        }
    }

    Column {
        spacing: 10
        anchors.top: parent.top
        anchors.topMargin: 44
        anchors.left: parent.left
        anchors.leftMargin: 80
        Text {
            text: desktopiconsetpage.actiontitle
            font.bold: true
            font.pixelSize: 14
            color: "#383838"
        }
        Text {
            text: desktopiconsetpage.actiontext
            font.pixelSize: 12
            color: "#7a7a7a"
        }
    }

    Row {
        id: settitle
        anchors{
            left: parent.left
            leftMargin: 40
            top: parent.top
            topMargin: 120
        }
        Text{
            id: themetitle
            text: qsTr("Icon theme settings")//图标主题设置
            font.bold: true
            font.pixelSize: 12
            color: "#383838"
        }
        //横线
        Common.Separator {
            anchors.verticalCenter: parent.verticalCenter
            width: desktopiconsetpage.width - themetitle.width - 40 * 2
        }
    }

    Row {
        id: themeline
        anchors{
            left: parent.left
            leftMargin: 60
            top: settitle.bottom
            topMargin: 10
        }
        spacing: 245 - 16 - 20
        z: 11
        Row {
            spacing: 20
            Common.TipLabel {
                anchors.verticalCenter: parent.verticalCenter
                kflag: "yes"
                showImage: "../../img/icons/cloud-light.png"
            }
            Text {
                id: iconthemelabel
                width: 170
                text: qsTr("Icon theme:")//图标主题：
                font.pixelSize: 12
                color: "#7a7a7a"
                anchors.verticalCenter: parent.verticalCenter
            }
            Common.ComboBox {
                id: iconcombo
                model: choices
                width: 220
                onSelectedTextChanged: {
                    sessiondispatcher.set_icon_theme_qt(iconcombo.selectedText);
                }
            }
        }

        Common.Button {
            hoverimage: "blue.png"
            text: qsTr("Restore")//恢复默认
            width: 94
            height: 29
            fontsize: 13
            onClicked: {
                sessiondispatcher.set_default_theme_qt("icontheme");
                iconcombo.selectedIndex = desktopiconsetpage.default_index;
            }
        }
    }

    Row {
        id: icontitle
        anchors{
            left: parent.left
            leftMargin: 40
            top: themeline.bottom
            topMargin: 30
        }
        Text{
            id: showtitle
            text: qsTr("Desktop icon display control")//桌面图标显示控制
            font.bold: true
            font.pixelSize: 12
            color: "#383838"
        }
        //横线
        Common.Separator {
            anchors.verticalCenter: parent.verticalCenter
            width: desktopiconsetpage.width - showtitle.width - 40 * 2
        }
    }

    Column {
        anchors{
            left: parent.left
            leftMargin: 60
            top: icontitle.bottom
            topMargin: 10
        }
        spacing: 10
        z: 11
        Row {
            spacing: 294 - 16 - 20
            Row {
                spacing: 20
                Common.TipLabel {
                    anchors.verticalCenter: parent.verticalCenter
                    kflag: "yes"
                    showImage: "../../img/icons/cloud-light.png"
                }
                Common.Label {
                    id: desktopiconlabel
                    width: 170
                    text: qsTr("Show Desktop Icons: ")//显示桌面图标：
                    font.pixelSize: 12
                    color: "#383838"
                    anchors.verticalCenter: parent.verticalCenter
                }
                Common.Switch {
                    id: iconswitcher
                    width: desktopiconlabel.width
                    onSwitched: {
                        if (iconswitcher.switchedOn) {
                            sessiondispatcher.set_show_desktop_icons_qt(true);
                        }
                        else if(!iconswitcher.switchedOn) {
                            sessiondispatcher.set_show_desktop_icons_qt(false);
                        }
                    }
                }
            }

            Common.Button {
                hoverimage: "blue.png"
                text: qsTr("Restore")//恢复默认
                width: 94
                height: 29
                fontsize: 13
                onClicked: {
                    sessiondispatcher.set_default_desktop_qt("showdesktopicons");
                    if (sessiondispatcher.get_show_desktop_icons_qt()) {
                        iconswitcher.switchedOn = true;
                    }
                    else {
                        iconswitcher.switchedOn = false;
                    }
                }
            }
        }

        Row {
            spacing: 294 - 16 - 20
            Row {
                spacing: 20
                Common.TipLabel {
                    anchors.verticalCenter: parent.verticalCenter
                    kflag: "yes"
                    showImage: "../../img/icons/cloud-light.png"
                }
                Common.Label {
                    id: homefolderlabel
                    width: 170
                    text: qsTr("Home Folder: ")//主文件夹：
                    font.pixelSize: 12
                    color: "#383838"
                    anchors.verticalCenter: parent.verticalCenter
                }
                Common.Switch {
                    id: folderswitcher
                    width: homefolderlabel.width
                    onSwitched: {
                        if (folderswitcher.switchedOn) {
                            sessiondispatcher.set_show_homefolder_qt(true);
                        }
                        else if(!folderswitcher.switchedOn) {
                            sessiondispatcher.set_show_homefolder_qt(false);
                        }
                    }
                }
            }

            Common.Button {
                hoverimage: "blue.png"
                text: qsTr("Restore")//恢复默认
                width: 94
                height: 29
                fontsize: 13
                onClicked: {
                    sessiondispatcher.set_default_desktop_qt("homeiconvisible");
                    if (sessiondispatcher.get_show_homefolder_qt()) {
                        folderswitcher.switchedOn = true;
                    }
                    else {
                        folderswitcher.switchedOn = false;
                    }
                }
            }
        }

        Row {
            spacing: 294 - 16 - 20
            Row {
                spacing: 20
                Common.TipLabel {
                    anchors.verticalCenter: parent.verticalCenter
                    kflag: "yes"
                    showImage: "../../img/icons/cloud-light.png"
                }
                Common.Label {
                    id: networklabel
                    width: 170
                    text: qsTr("Network: ")//网络：
                    font.pixelSize: 12
                    color: "#383838"
                    anchors.verticalCenter: parent.verticalCenter
                }
                Common.Switch {
                    id: networkswitcher
                    width: networklabel.width
                    onSwitched: {
                        if (networkswitcher.switchedOn) {
                            sessiondispatcher.set_show_network_qt(true);
                        }
                        else if(!networkswitcher.switchedOn) {
                            sessiondispatcher.set_show_network_qt(false);
                        }
                    }
                }
            }

            Common.Button {
                hoverimage: "blue.png"
                text: qsTr("Restore")//恢复默认
                width: 94
                height: 29
                fontsize: 13
                onClicked: {
                    sessiondispatcher.set_default_desktop_qt("networkiconvisible");
                    if (sessiondispatcher.get_show_network_qt()) {
                        networkswitcher.switchedOn = true;
                    }
                    else {
                        networkswitcher.switchedOn = false;
                    }
                }
            }
        }

        Row {
            spacing: 294 - 16 - 20
            Row {
                spacing: 20
                Common.TipLabel {
                    anchors.verticalCenter: parent.verticalCenter
                    kflag: "yes"
                    showImage: "../../img/icons/cloud-light.png"
                }
                Common.Label {
                    id: trashlabel
                    width: 170
                    text: qsTr("Trash : ")//回收站：
                    font.pixelSize: 12
                    color: "#383838"
                    anchors.verticalCenter: parent.verticalCenter
                }
                Common.Switch {
                    id: trashswitcher
                    width: trashlabel.width
                    onSwitched: {
                        if (trashswitcher.switchedOn) {
                            sessiondispatcher.set_show_trash_qt(true);
                        }
                        else if(!trashswitcher.switchedOn) {
                            sessiondispatcher.set_show_trash_qt(false);
                        }
                    }
                }
            }

            Common.Button {
                hoverimage: "blue.png"
                text: qsTr("Restore")//恢复默认
                width: 94
                height: 29
                fontsize: 13
                onClicked: {
                    sessiondispatcher.set_default_desktop_qt("trashiconvisible");
                    if (sessiondispatcher.get_show_trash_qt()) {
                        trashswitcher.switchedOn = true;
                    }
                    else {
                        trashswitcher.switchedOn = false;
                    }
                }
            }
        }

        Row {
            spacing: 294 - 16 - 20
            Row {
                spacing: 20
                Common.TipLabel {
                    anchors.verticalCenter: parent.verticalCenter
                    kflag: "yes"
                    showImage: "../../img/icons/cloud-light.png"
                }
                Common.Label {
                    id: devicelabel
                    width: 170
                    text: qsTr("Mounted Volumes: ")//已经挂载卷标：
                    font.pixelSize: 12
                    color: "#383838"
                    anchors.verticalCenter: parent.verticalCenter
                }
                Common.Switch {
                    id: deviceswitcher
                    width: devicelabel.width
                    onSwitched: {
                        if (deviceswitcher.switchedOn) {
                            sessiondispatcher.set_show_devices_qt(true);
                        }
                        else if(!deviceswitcher.switchedOn) {
                            sessiondispatcher.set_show_devices_qt(false);
                        }
                    }
                }
            }

            Common.Button {
                hoverimage: "blue.png"
                text: qsTr("Restore")//恢复默认
                width: 94
                height: 29
                fontsize: 13
                onClicked: {
                    sessiondispatcher.set_default_desktop_qt("volumesvisible");
                    if (sessiondispatcher.get_show_devices_qt()) {
                        deviceswitcher.switchedOn = true;
                    }
                    else {
                        deviceswitcher.switchedOn = false;
                    }
                }
            }

        }
    }//Column

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
        showok: false
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
    }
}
