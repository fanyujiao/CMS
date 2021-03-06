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

Item {
    id:root
    width: parent.width
    height: 435
    property string title: qsTr("Cleanup browser Cookies information, to protect your privacy")//清理浏览器 Cookies 信息，保护个人隐私
    property string description: qsTr("Clean up user login information, support Firefox and Chromium browser")//清理用户登陆网站信息, 支持 Firefox 和 Chromium 浏览器
    property bool firefoxResultFlag: false//判断扫描后的实际内容是否为空，为空时为false，有内容时为true
    property bool chromiumResultFlag: false//判断扫描后的实际内容是否为空，为空时为false，有内容时为true
    property int firefoxNum: 0//扫描后得到的apt的项目总数
    property int chromiumNum: 0//扫描后得到的apt的项目总数
    property bool splitFlag: true//传递给ClearDelegate.qml,为true时切割字符串，为false时不切割字符串
    property bool flag: false//记录是清理后重新获取数据（true），还是点击开始扫描后获取数据（false）
    property int spaceValue: 20
    property int firefox_arrow_show: 0//传递给CookiesDelegate.qml是否显示伸缩图标，为1时显示，为0时隐藏
    property int chromium_arrow_show: 0//传递给CookiesDelegate.qml是否显示伸缩图标，为1时显示，为0时隐藏
    property bool firefox_expanded: false//传递给CookiesDelegate.qml,觉得伸缩图标是扩展还是收缩
    property bool chromium_expanded: false//传递给CookiesDelegate.qml,觉得伸缩图标是扩展还是收缩
    property bool firefox_maincheck: true
    property bool chromium_maincheck: true
    property bool firefox_showNum: false//决定firefox的扫描结果数是否显示
    property bool chromium_showNum: false//决定chromium的扫描结果数是否显示
    ListModel { id: firefoxmainModel }
    ListModel { id: firefoxsubModel }
    ListModel { id: chromiummainModel }
    ListModel { id: chromiumsubModel }
    property string firefox_btn_text: qsTr("Start scanning")//开始扫描
    property string chromium_btn_text: qsTr("Start scanning")//开始扫描
    property bool firefox_reset: false//firefox重置按钮默认隐藏
    property bool chromium_reset: false//chromium重置按钮默认隐藏
    property string firefox_btn_flag: "cookies_scan"//扫描或者清理的标记
    property string chromium_btn_flag: "cookies_scanc"//扫描或者清理的标记
    property int item_height: 30

    Connections
    {
        target: sessiondispatcher
        onAppendCookiesContentToModel: {
            //QString flag, QString domain, QString num
            if(flag == "firefox") {
                firefoxsubModel.append({"itemTitle": domain, "desc": "", "number": num, "index": root.firefoxNum});
                root.firefoxNum += 1;
            }
            else if(flag == "chromium") {
                chromiumsubModel.append({"itemTitle": domain, "desc": "", "number": num, "index": root.chromiumNum});
                root.chromiumNum += 1;
            }
        }
        onTellQMLCookiesOver: {
            if (cookiesFlag == "funinstall") {
                //友情提示    没有安装 Firefox！
                sessiondispatcher.showWarningDialog(qsTr("Tips:"), qsTr("Firefox is not installed!"), mainwindow.pos.x, mainwindow.pos.y);
            }
            else if (cookiesFlag == "cuninstall") {
                //友情提示        没有安装 Chromium！
                sessiondispatcher.showWarningDialog(qsTr("Tips:"), qsTr("Chromium is not installed!"), mainwindow.pos.x, mainwindow.pos.y);
            }
            else {
                if (cookiesFlag == "firefox") {
                    if(root.firefoxNum != 0) {
                        root.firefoxResultFlag = true;//扫描的实际有效内容存在
                        firefoxmainModel.clear();
                        //清理 Firefox 保存的 Cookies                清理 Firefox 浏览器自动保存的登录信息 (Cookies)
                        firefoxmainModel.append({
                                         "itemTitle": qsTr("Cleanup the Cookies saving in Firefox"),
                                         "picture": "../../img/toolWidget/firefox.png"})
//                                       "detailstr": qsTr("Clean up automatically saved logon information by Firefox browser(Cookies)")})
                    }
                    else {
                        root.firefoxResultFlag = false;//扫描的实际有效内容不存在
                    }
                    if(root.firefoxResultFlag == false) {
                        root.firefox_expanded = false;//伸缩箭头不扩展
                        root.firefox_arrow_show = 0;//伸缩箭头不显示
                        root.firefox_showNum = false;
                        if(root.flag == false) {//点击扫描时的获取数据，此时显示该对话框
                            //友情提示：      扫描内容为空，无需清理！
                            sessiondispatcher.showWarningDialog(qsTr("Tips:"), qsTr("The scan results are empty, no need to clean up !"), mainwindow.pos.x, mainwindow.pos.y);
                        }
                        else {//清理apt后的重新获取数据，此时不需要显示对话框
                            root.flag = false;
                        }
                        root.firefox_btn_flag = "cookies_scan";//1206
                        root.firefox_btn_text = qsTr("Start scanning")//开始扫描//1206
                        root.firefox_reset = false;//1206
                        if(root.chromium_expanded) {
                            scrollItem.height = root.item_height + (root.chromiumNum + 1) * root.item_height + root.spaceValue*4;
                        }
                        else {
                            scrollItem.height = 2 * root.item_height + root.spaceValue*4;
                        }
                    }
                    else if(root.firefoxResultFlag == true) {
                        root.firefox_expanded = true;//伸缩箭头扩展
                        root.firefox_arrow_show = 1;//伸缩箭头显示
                        root.firefox_showNum = true;
                        if(root.flag == false) {//点击扫描时的获取数据，此时显示该对话框
                            toolkits.alertMSG(qsTr("Scan completed!"), mainwindow.pos.x, mainwindow.pos.y);//扫描完成！
                        }
                        else {//清理software后的重新获取数据，此时不需要显示对话框
                            root.flag = false;
                        }

                        //当真正扫描到内容时：按钮状态改变、显示文字改变、重置按钮显示
                        root.firefox_btn_flag = "cookies_work";//1206
                        root.firefox_btn_text = qsTr("All cleanup");//全部清理//1206
                        root.firefox_reset = true;//1206
                        if(root.chromium_expanded) {
                            scrollItem.height = (root.firefoxNum + 1) * root.item_height + (root.chromiumNum + 1) * root.item_height + root.spaceValue*4;
                        }
                        else {
                            scrollItem.height = (root.firefoxNum + 1) * root.item_height + root.item_height + root.spaceValue*4;
                        }
                    }
                }
                else if (cookiesFlag == "chromium") {
                    if(root.chromiumNum != 0) {
                        root.chromiumResultFlag = true;//扫描的实际有效内容存在
                        chromiummainModel.clear();
                        //清理 Chromium 保存的 Cookies             清理 Chromium 浏览器自动保存的登录信息 (Cookies)
                        chromiummainModel.append({
                                         "itemTitle": qsTr("Cleanup the Cookies saving in Chromium"),
                                         "picture": "../../img/toolWidget/chromium.png"})
//                                       "detailstr": qsTr("Clean up automatically saved logon information by Chromium browser(Cookies)")})
                    }
                    else {
                        root.chromiumResultFlag = false;//扫描的实际有效内容不存在
                    }
                    if(root.chromiumResultFlag == false) {
                        root.chromium_expanded = false;//伸缩箭头不扩展
                        root.chromium_arrow_show = 0;//伸缩箭头不显示
                        root.chromium_showNum = false;

                        if(root.flag == false) {//点击扫描时的获取数据，此时显示该对话框
                            //友情提示：      扫描内容为空，无需清理！
                            sessiondispatcher.showWarningDialog(qsTr("Tips:"), qsTr("The scan results are empty, no need to clean up !"), mainwindow.pos.x, mainwindow.pos.y);
                        }
                        else {//清理apt后的重新获取数据，此时不需要显示对话框
                            root.flag = false;
                        }

                        root.chromium_btn_flag = "cookies_scanc";//1206
                        root.chromium_btn_text = qsTr("Start scanning")//开始扫描
                        root.chromium_reset = false;//1206
                        if(root.firefox_expanded) {
                            scrollItem.height = root.item_height + (root.firefoxNum + 1) * root.item_height + root.spaceValue*4;
                        }
                        else {
                            scrollItem.height = 2 * root.item_height + root.spaceValue*4;
                        }
                    }
                    else if(root.chromiumResultFlag == true) {
                        root.chromium_expanded = true;//伸缩箭头扩展
                        root.chromium_arrow_show = 1;//伸缩箭头显示
                        root.chromium_showNum = true;

                        if(root.flag == false) {//点击扫描时的获取数据，此时显示该对话框
                            toolkits.alertMSG(qsTr("Scan completed!"), mainwindow.pos.x, mainwindow.pos.y);//扫描完成！
                        }
                        else {//清理software后的重新获取数据，此时不需要显示对话框
                            root.flag = false;
                        }
                        //当真正扫描到内容时：按钮状态改变、显示文字改变、重置按钮显示
                        root.chromium_btn_flag = "cookies_workc";//1206
                        root.chromium_btn_text = qsTr("All cleanup");//全部清理
                        root.chromium_reset = true;//1206
                        if(root.firefox_expanded) {
                            scrollItem.height = (root.firefoxNum + 1) * root.item_height + (root.chromiumNum + 1) * root.item_height + root.spaceValue*4;
                        }
                        else {
                            scrollItem.height = (root.chromiumNum + 1) * root.item_height + root.item_height + root.spaceValue*4;
                        }
                    }
                }
            }
        }
    }


    Component.onCompleted: {
        //清理 Firefox 保存的 Cookies                清理 Firefox 浏览器自动保存的登录信息 (Cookies)
        firefoxmainModel.append({
                         "itemTitle": qsTr("Cleanup the Cookies saving in Firefox"),
                         "picture": "../../img/toolWidget/firefox.png"})
//                         "detailstr": qsTr("Clean up automatically saved logon information by Firefox browser(Cookies)")})
        //清理 Chromium 保存的 Cookies             清理 Chromium 浏览器自动保存的登录信息 (Cookies)
        chromiummainModel.append({
                         "itemTitle": qsTr("Cleanup the Cookies saving in Chromium"),
                         "picture": "../../img/toolWidget/chromium.png"})
//                         "detailstr": qsTr("Clean up automatically saved logon information by Chromium browser(Cookies)")})
    }

    //信号绑定，绑定qt的信号finishCleanWork，该信号emit时触发onFinishCleanWork
    Connections
    {
        target: systemdispatcher
        onQuitCleanWork: {//用户在policykit验证时直接关闭验证或者点击取消
            if (msg == "firefox") {
                toolkits.alertMSG(qsTr("Cleanup interrupted!"), mainwindow.pos.x, mainwindow.pos.y);//清理中断！
            }
            else if (msg == "chromium") {
                toolkits.alertMSG(qsTr("Cleanup interrupted!"), mainwindow.pos.x, mainwindow.pos.y);//清理中断！
            }
        }

        onFinishCleanWorkError: {//清理过程中出错
            if (msg == "firefox") {
                if (root.firefox_btn_flag == "cookies_work") {
                    toolkits.alertMSG(qsTr("Cleanup abnormal!"), mainwindow.pos.x, mainwindow.pos.y);//清理出现异常！
                }
            }
            else if (msg == "chromium") {
                if (root.chromium_btn_flag == "cookies_workc") {
                    toolkits.alertMSG(qsTr("Cleanup abnormal!"), mainwindow.pos.x, mainwindow.pos.y);//清理出现异常！
                }
            }
        }

        onFinishCleanWork: {//清理正常完成
            if (msg == "firefox") {
                if (root.firefox_btn_flag == "cookies_work") {
//                    systemdispatcher.clear_cookies_args();
                    firefoxsubModel.clear();//内容清空
                    firefoxmainModel.clear();
                    //清理 Firefox 保存的 Cookies                清理 Firefox 浏览器自动保存的登录信息 (Cookies)
                    firefoxmainModel.append({
                                     "itemTitle": qsTr("Cleanup the Cookies saving in Firefox"),
                                     "picture": "../../img/toolWidget/firefox.png",
                                     "detailstr": qsTr("Clean up automatically saved logon information by Firefox browser(Cookies)")})
                    root.firefox_expanded = false;//伸缩箭头不扩展
                    root.firefox_arrow_show = 0;//伸缩箭头不显示
                    root.firefox_showNum = false;
                    root.firefox_btn_flag = "cookies_scan";//1206
                    root.firefox_btn_text = qsTr("Start scanning")//开始扫描//1206
                    root.firefox_reset = false;//1206
                    root.firefoxNum = 0;//隐藏滑动条
                    if(root.chromium_expanded) {
                        scrollItem.height = root.item_height + (root.chromiumNum + 1) * root.item_height + root.spaceValue*4;
                    }
                    else {
                        scrollItem.height = 2 * root.item_height + root.spaceValue*4;
                    }
                    toolkits.alertMSG(qsTr("Cleared"), mainwindow.pos.x, mainwindow.pos.y);//清理完毕！
                }
            }
            else if (msg == "chromium") {
                if (root.chromium_btn_flag == "cookies_workc") {
                    chromiumsubModel.clear();//内容清空
                    chromiummainModel.clear();
                    //清理 Chromium 保存的 Cookies             清理 Chromium 浏览器自动保存的登录信息 (Cookies)
                    chromiummainModel.append({
                                     "itemTitle": qsTr("Cleanup the Cookies saving in Chromium"),
                                     "picture": "../../img/toolWidget/chromium.png"})
//                                     "detailstr": qsTr("Clean up automatically saved logon information by Chromium browser(Cookies)")})
                    root.chromium_expanded = false;//伸缩箭头不扩展
                    root.chromium_arrow_show = 0;//伸缩箭头不显示
                    root.chromium_showNum = false;
                    root.chromium_btn_flag = "cookies_scanc";//1206
                    root.chromium_btn_text = qsTr("Start scanning")//开始扫描//1206
                    root.chromium_reset = false;//1206
                    root.chromiumNum = 0;//隐藏滑动条
                    if(root.firefox_expanded) {
                        scrollItem.height = root.item_height + (root.firefoxNum + 1) * root.item_height + root.spaceValue*4;
                    }
                    else {
                        scrollItem.height = 2 * root.item_height + root.spaceValue*4;
                    }
                    toolkits.alertMSG(qsTr("Cleared"), mainwindow.pos.x, mainwindow.pos.y);//清理完毕！
                }
            }
        }
    }

    //背景
    Image {
        source: "../../img/skin/bg-middle.png"
        anchors.fill: parent
    }

    //titlebar
    Row {
        id: titlebar
        spacing: 20
        width: parent.width
        anchors { top: parent.top; topMargin: 20; left: parent.left; leftMargin: 27 }
        Image {
            id: apt_refreshArrow
            source: "../../img/toolWidget/cookies-bg.png"
            Behavior on rotation { NumberAnimation { duration: 200 } }
        }
        Column {
            anchors.verticalCenter: parent.verticalCenter
            spacing: 10
            Text {
                width: 500
                text: root.title
                wrapMode: Text.WordWrap
                font.bold: true
                font.pixelSize: 14
                color: "#383838"
            }
            Text {
                width: 500
                text: root.description
                wrapMode: Text.WordWrap
                font.pixelSize: 12
                color: "#7a7a7a"
            }
        }
    }

    //分割条
    Common.Separator {
        id: splitbar
        anchors {
            top: titlebar.bottom
            topMargin: 18
            left: parent.left
            leftMargin: 2
        }
        width: parent.width - 4
    }

    Common.ScrollArea {
        frame:false
        anchors.top: titlebar.bottom
        anchors.topMargin: 30
        anchors.left:parent.left
//        anchors.leftMargin: 27
        height: root.height -titlebar.height - 47
        width: parent.width -2//parent.width - 27 -2
        Item {
            id: scrollItem
            width: parent.width
            height: root.item_height*2 + root.spaceValue*2*2
            Column {
                spacing: root.spaceValue*2
                //垃圾清理显示内容
                ListView {
                    id: aptListView
                    width: parent.width
                    height: root.firefox_expanded ? (root.firefoxNum + 1) * root.item_height : root.item_height
                    model: firefoxmainModel
                    delegate: CookiesDelegate{
                        sub_num: root.firefoxNum
                        sub_model: firefoxsubModel
                        btn_flag: root.firefox_btn_flag
                        flag: "firefox"
                        actionTitle: root.firefox_btn_text
                        resetStatus: root.firefox_reset
                        showNum: root.firefox_showNum
                        arrow_display: root.firefox_arrow_show//为0时隐藏伸缩图标，为1时显示伸缩图标
                        expanded: root.firefox_expanded//firefox_expanded为true时，箭头向下，内容展开;firefox_expanded为false时，箭头向上，内容收缩
                        delegate_flag: root.splitFlag
                        onSendBrowserType: {
                            if(browserFlag == "firefox") {
                                if(status == "reset") {//点击重置按钮，清空数据
                                    firefoxsubModel.clear();
                                    root.firefoxNum = 0;
                                    root.firefox_btn_flag = "cookies_scan";//1206
                                    root.firefox_btn_text = qsTr("Start scanning")//开始扫描//1206
                                    root.firefox_reset = false;//1206
                                    if(root.firefox_expanded == true) {
                                        root.firefox_expanded = false;//1、先传递给CookiesDelegate.qml的伸缩值设为默认的false
                                    }
                                    root.firefox_arrow_show = 0;//2、然后传递给CookiesDelegate.qml去隐藏伸展按钮
                                    root.firefox_showNum = false;//隐藏扫描的数目
                                    if(root.chromium_expanded) {
                                        scrollItem.height = root.item_height + (root.chromiumNum + 1) * root.item_height + root.spaceValue*4;
                                    }
                                    else {
                                        scrollItem.height = 2 * root.item_height + root.spaceValue*4;
                                    }
                                }
                                else if(status == "rescan") {//点击重新扫描按钮
                                    firefoxsubModel.clear();
                                    root.firefoxNum = 0;
                                    root.firefox_btn_flag = "cookies_scan";//1206
                                    root.firefox_btn_text = qsTr("Start scanning")//开始扫描//1206
                                    root.firefox_reset = false;//1206
                                    if(root.firefox_expanded == true) {
                                        root.firefox_expanded = false;//1、先传递给CookiesDelegate.qml的伸缩值设为默认的false
                                    }
                                    root.firefox_arrow_show = 0;//2、然后传递给CookiesDelegate.qml去隐藏伸展按钮
                                    root.firefox_showNum = false;//隐藏扫描的数目
                                    root.flag = false;
                                    sessiondispatcher.cookies_scan_function_qt("firefox");
//                                    root.getDataOfFirefox();
                                }
                                else if(status == "refresh") {//清理完某个子项后自动刷新列表
                                    console.log("---*****2222------");
                                    firefoxsubModel.clear();
                                    root.firefoxNum = 0;
                                    root.firefox_btn_flag = "cookies_scan";//1206
                                    root.firefox_btn_text = qsTr("Start scanning")//开始扫描//1206
                                    root.firefox_reset = false;//1206
                                    if(root.firefox_expanded == true) {
                                        root.firefox_expanded = false;//1、先传递给CookiesDelegate.qml的伸缩值设为默认的false
                                    }
                                    root.firefox_arrow_show = 0;//2、然后传递给CookiesDelegate.qml去隐藏伸展按钮
                                    root.firefox_showNum = false;//隐藏扫描的数目
                                    root.flag = true;
                                    sessiondispatcher.cookies_scan_function_qt("firefox");
//                                    root.getDataOfFirefox();
                                }
                                else {
                                    root.firefox_btn_flag = status;
                                    if (root.firefox_btn_flag == "cookies_scan") {
//                                        console.log("scan---f......");
                                        //开始扫描时获取cookies
                                        root.firefoxNum = 0;
                                        root.flag = false;
                                        sessiondispatcher.cookies_scan_function_qt("firefox");
//                                        root.getDataOfFirefox();
                                    }
                                    else if (root.firefox_btn_flag == "cookies_work") {
//                                        if(root.firefox_check_flag) {
                                            console.log("clean---f......");
                                            //开始清理cookies
                                            systemdispatcher.set_user_homedir_qt();
                                            systemdispatcher.cookies_clean_records_function_qt("firefox");
//                                        }
//                                        else {
//                                            sessiondispatcher.showWarningDialog(qsTr("Tips:"), qsTr("Sorry, you have no choice to clean up the items, please confirm!"), mainwindow.pos.x, mainwindow.pos.y);
//                                        }
                                    }
                                }
                            }
                        }
                        onBrowserArrowClicked: {
                            if(browserFlag == "firefox") {
                                if(expand_flag == true) {
                                    root.firefox_expanded = true;
                                    if(root.chromium_expanded == true) {
                                        scrollItem.height = (root.firefoxNum + 1) * root.item_height + (root.chromiumNum + 1) * root.item_height + root.spaceValue*4;
                                    }
                                    else {
                                        scrollItem.height = (root.firefoxNum + 2) * root.item_height + root.spaceValue*4;
                                    }
                                }
                                else {
                                    root.firefox_expanded = false;
                                    if(root.chromium_expanded == true) {
                                        scrollItem.height = (root.chromiumNum + 2) * root.item_height + root.spaceValue*4;
                                    }
                                    else {
                                        scrollItem.height = 2* root.item_height + root.spaceValue*4;
                                    }
                                }
                            }
                        }
                    }
                    cacheBuffer: 1000
                    opacity: 1
//                    spacing: 10
                    snapMode: ListView.NoSnap
                    boundsBehavior: Flickable.DragOverBounds
                    currentIndex: 0
                    preferredHighlightBegin: 0
                    preferredHighlightEnd: preferredHighlightBegin
                    highlightRangeMode: ListView.StrictlyEnforceRange
                }
                //垃圾清理显示内容
                ListView {
                    id: softListView
                    width: parent.width
                    height: root.chromium_expanded ? (root.chromiumNum + 1) * root.item_height : root.item_height
                    model: chromiummainModel
                    delegate: CookiesDelegate{
                        sub_num: root.chromiumNum
                        sub_model: chromiumsubModel
                        btn_flag: root.chromium_btn_flag
                        showNum: root.chromium_showNum
                        flag: "chromium"
                        actionTitle: root.chromium_btn_text
                        resetStatus: root.chromium_reset
                        arrow_display: root.chromium_arrow_show//为0时隐藏伸缩图标，为1时显示伸缩图标
                        expanded: root.chromium_expanded//firefox_expanded为true时，箭头向下，内容展开;firefox_expanded为false时，箭头向上，内容收缩
                        delegate_flag: root.splitFlag
                        onSendBrowserType: {
                            if(browserFlag == "chromium") {
                                if(status == "reset") {//点击重置按钮，清空数据
                                    chromiumsubModel.clear();
                                    root.chromiumNum = 0;
                                    root.chromium_btn_flag = "cookies_scanc";//1206
                                    root.chromium_btn_text = qsTr("Start scanning")//开始扫描//1206
                                    root.chromium_reset = false;//1206
                                    if(root.chromium_expanded == true) {
                                        root.chromium_expanded = false;//1、先传递给CookiesDelegate.qml的伸缩值设为默认的false
                                    }
                                    root.chromium_arrow_show = 0;//2、然后传递给CookiesDelegate.qml去隐藏伸展按钮
                                    root.chromium_showNum = false;//隐藏扫描的数目
                                    if(root.firefox_expanded) {
                                        scrollItem.height = root.item_height + (root.firefoxNum + 1) * root.item_height + root.spaceValue*4;
                                    }
                                    else {
                                        scrollItem.height = 2 * root.item_height + root.spaceValue*4;
                                    }
                                }
                                else if(status == "rescan") {
                                    chromiumsubModel.clear();
                                    root.chromiumNum = 0;
                                    root.chromium_btn_flag = "cookies_scanc";//1206
                                    root.chromium_btn_text = qsTr("Start scanning")//开始扫描//1206
                                    root.chromium_reset = false;//1206
                                    if(root.chromium_expanded == true) {
                                        root.chromium_expanded = false;//1、先传递给CookiesDelegate.qml的伸缩值设为默认的false
                                    }
                                    root.chromium_arrow_show = 0;//2、然后传递给CookiesDelegate.qml去隐藏伸展按钮
                                    root.chromium_showNum = false;//隐藏扫描的数目
                                    root.flag = false;
                                    sessiondispatcher.cookies_scan_function_qt("chromium");
//                                    root.getDataOfChromium();
                                }
                                else if(status == "refresh") {//清理完某个子项后自动刷新列表
                                    chromiumsubModel.clear();
                                    root.chromiumNum = 0;
                                    root.chromium_btn_flag = "cookies_scanc";//1206
                                    root.chromium_btn_text = qsTr("Start scanning")//开始扫描//1206
                                    root.chromium_reset = false;//1206
                                    if(root.chromium_expanded == true) {
                                        root.chromium_expanded = false;//1、先传递给CookiesDelegate.qml的伸缩值设为默认的false
                                    }
                                    root.chromium_arrow_show = 0;//2、然后传递给CookiesDelegate.qml去隐藏伸展按钮
                                    root.chromium_showNum = false;//隐藏扫描的数目
                                    root.flag = true;
                                    sessiondispatcher.cookies_scan_function_qt("chromium");
//                                    root.getDataOfChromium();
                                }
                                else {
                                    root.chromium_btn_flag = status;
                                    if (root.chromium_btn_flag == "cookies_scanc") {
//                                        console.log("scan---c......");
                                        //开始扫描时获取cookies
                                        root.flag = false;
                                        root.chromiumNum = 0;
                                        sessiondispatcher.cookies_scan_function_qt("chromium");
//                                        root.getDataOfChromium();
                                    }
                                    else if (root.chromium_btn_flag == "cookies_workc") {
//                                        if(root.chromium_check_flag)
//                                        {
                                            console.log("clean---c......");
                                            //开始清理cookies
                                            systemdispatcher.set_user_homedir_qt();
                                            systemdispatcher.cookies_clean_records_function_qt("chromium");
//                                        }
//                                        else {
//                                            sessiondispatcher.showWarningDialog(qsTr("Tips:"), qsTr("Sorry, you have no choice to clean up the items, please confirm!"), mainwindow.pos.x, mainwindow.pos.y);
//                                        }
                                    }
                                }
                            }
                        }
                        onBrowserArrowClicked: {
                            if(browserFlag == "chromium") {//1212
                                if(expand_flag == true) {
                                    root.chromium_expanded = true;
                                    if(root.firefox_expanded == true) {
                                        scrollItem.height = (root.firefoxNum + 1) * root.item_height + (root.chromiumNum + 1) * root.item_height + root.spaceValue*4;
                                    }
                                    else {
                                        scrollItem.height = (root.chromiumNum + 2) * root.item_height + root.spaceValue*4;
                                    }
                                }
                                else {
                                    root.chromium_expanded = false;
                                    if(root.firefox_expanded == true) {
                                        scrollItem.height = (root.firefoxNum + 2) * root.item_height + root.spaceValue*4;
                                    }
                                    else {
                                        scrollItem.height = 2* root.item_height + root.spaceValue*4;
                                    }
                                }
                            }
                        }
                    }
                    cacheBuffer: 1000
                    opacity: 1
//                    spacing: 10
                    snapMode: ListView.NoSnap
                    boundsBehavior: Flickable.DragOverBounds
                    currentIndex: 0
                    preferredHighlightBegin: 0
                    preferredHighlightEnd: preferredHighlightBegin
                    highlightRangeMode: ListView.StrictlyEnforceRange
                }
            }
        }
    }
}
