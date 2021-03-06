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

Rectangle {
    id: screen; width: parent.width; height: 475

    function openFile(file) {
         var component = Qt.createComponent(file)
         if (component.status == Component.Ready)
             pageStack.push(component);
         else
             console.log("Error loading component:", component.errorString());
     }

    Rectangle {
        id: tools_widget
        anchors {
            fill: parent
            left: parent.right
            leftMargin: 2
            right: parent.left
            rightMargin: 2
        }

        Item {
            id: views
            width: parent.width
            height: parent.height

            //左右布局
            //坐边栏
            Others.LeftPage {
                id: leftbar
                anchors.left: parent.left
                anchors.leftMargin: -2
                width: screen.width; height: screen.height
            }
            Item { id: foreground; anchors.fill: parent }


        }
    }
}
