import QtQuick 1.1
import "../AppTheme.js" as AppTheme
Rectangle {
 id:tabbar;color:"transparent"
 property int count:0
 property int commentCount:0
 property int mentionCount:0
 property int privateMsgCount:0
 signal tabChanged
 Row {anchors.fill:parent
 Repeater {model:["@我","评论","私信"]
 Item {width:tabbar.width/3;height:tabbar.height
 Text {anchors.centerIn:parent;text:modelData; font.pixelSize:22;color:tabbar.count===index?AppTheme.color("#337ea9",settingModel.darkMode):AppTheme.color("#79858f",settingModel.darkMode)}
 Rectangle {anchors.bottom:parent.bottom;anchors.horizontalCenter:parent.horizontalCenter;width:36;height:3;radius:1;color:AppTheme.color("#337ea9",settingModel.darkMode);visible:tabbar.count===index}
 Rectangle {property int unread:index===0?mentionCount:index===1?commentCount:privateMsgCount;visible:unread>0;width:unread>99?27:22;height:20;radius:10;color:AppTheme.color("#CDE4FF",settingModel.darkMode);anchors.right:parent.right;anchors.top:parent.top;Text{anchors.centerIn:parent;text:parent.unread>99?"99+":parent.unread;font.pixelSize:12;color:AppTheme.color("#26384a",settingModel.darkMode)}}
 MouseArea {anchors.fill:parent;onClicked:{tabbar.count=index;tabbar.tabChanged();}}
 }}}
}