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
     id: toolbar
     property bool showok: true
     property bool showrestore: false
     signal quitBtnClicked
     signal okBtnClicked
     signal restoreBtnClicked

     BorderImage {
//         source: "../../img/icons/tab.png"
         width: parent.width; height: parent.height + 14; y: -7
     }

     Rectangle{id: splitbar; x:2; width:parent.width - 4 ; height:1; color:"#b9c5cc"}

     Row {
         spacing: 19
         height: 32
         anchors {
             right: parent.right
             rightMargin: 42
             top: splitbar.bottom
             topMargin: 10
         }

         Common.Button {
             id: okBtn
             visible: toolbar.showok
             hoverimage: "green.png"
             fontsize: 13
             text: qsTr("OK")//确定
             width: 94;height: 29
             onClicked: toolbar.okBtnClicked()
         }
         Common.Button {
             id: restoreBtn
             visible: toolbar.showrestore
             hoverimage: "blue.png"
             fontsize: 13
             text: qsTr("Restore")//恢复默认
             width: 94
             height: 29
             onClicked: toolbar.restoreBtnClicked()
         }
         Common.Button {
             id: quitBtn
             hoverimage: "gray.png"
             fontcolor:"#929292"
             fontsize: 13
             text: qsTr("Back")//返回
             width: 94; height: 29
             onClicked: toolbar.quitBtnClicked()
         }
     }
}
