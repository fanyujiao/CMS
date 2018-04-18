import QtQuick 1.1
import "../bars" as Bars

//import Apt 0.1
//import Signals 0.1


Rectangle {
    id : spro
    //    width: 100
    //    height: 15
    property int r: 0
    property string protext : ""
    property string  textinfor: ""
    property string mycolor: "transparent"
   color: mycolor

    Row{
        anchors.centerIn: parent
        spacing: 10
        Bars.Progress{
            id : pro
           provalue: spro.protext
         //   property int r : Math.floor(Math.random() * 5000 + 1000);
           property int rr : spro.r
            property int widthvalue: 850
            width : widthvalue

           // NumberAnimation on value { duration: pro.rr; from: 0; to: 100; loops: Animation.Infinite }
            //ColorAnimation on color { duration: pro.rr; from: "transparent"; to: "lightgreen"; loops: Animation.Infinite }
            //ColorAnimation on secondColor { duration: pro.rr; from: "transparent"; to: "green"; loops: Animation.Infinite }
        }
//            Text{
//                text:textinfor
//            }
    }
}
