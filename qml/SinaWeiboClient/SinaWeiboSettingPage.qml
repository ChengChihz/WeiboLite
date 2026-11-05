import QtQuick 1.1
import "AppTheme.js" as AppTheme
import com.nokia.meego 1.0
import "controls"
Page {
 orientationLock:PageOrientation.LockPortrait
 Rectangle {anchors.fill:parent;color:AppTheme.color("#f7f8fa",settingModel.darkMode)}
 AccTitleBar {id:heading;width:parent.width;height:65;title:"设置"}
 Flickable {anchors.top:heading.bottom;anchors.bottom:parent.bottom;width:parent.width;contentWidth:width;contentHeight:options.height+32;clip:true
 Column {id:options;y:16;x:16;width:parent.width-32;spacing:16
 Rectangle {width:parent.width;height:80;color:AppTheme.color("white",settingModel.darkMode);Text{x:20;anchors.verticalCenter:parent.verticalCenter;text:"提示声音";font.pixelSize:24;color:AppTheme.color("#202830",settingModel.darkMode)} ThemeSwitch {anchors.right:parent.right;anchors.rightMargin:16;anchors.verticalCenter:parent.verticalCenter;checked:settingModel.readAudioMode();onCheckedChanged:settingModel.saveAudioMode(checked)}}
 Text {text:"字号";font.pixelSize:22;color:AppTheme.color("#52606c",settingModel.darkMode)}
 ButtonRow {width:parent.width;Button {text:"较小";checkable:true;checked:settingModel.fontSizeMode===0;onClicked:settingModel.fontSizeMode=0} Button {text:"标准";checkable:true;checked:settingModel.fontSizeMode===1;onClicked:settingModel.fontSizeMode=1} Button {text:"较大";checkable:true;checked:settingModel.fontSizeMode===2;onClicked:settingModel.fontSizeMode=2}}
 Text {text:"时间流图片";font.pixelSize:22;color:AppTheme.color("#52606c",settingModel.darkMode)}
 ButtonRow {width:parent.width;Button {text:"自动加载";font.pixelSize:20;checkable:true;checked:settingModel.imageMode===0;onClicked:settingModel.imageMode=0} Button {text:"点击后加载";font.pixelSize:20;checkable:true;checked:settingModel.imageMode===1;onClicked:settingModel.imageMode=1} Button {text:"无图模式";font.pixelSize:20;checkable:true;checked:settingModel.imageMode===2;onClicked:settingModel.imageMode=2}}
 Text {text:"评论默认排序";font.pixelSize:22;color:AppTheme.color("#52606c",settingModel.darkMode)}
 ButtonRow {width:parent.width;Button {text:"最新";checkable:true;checked:settingModel.latestComments;onClicked:settingModel.latestComments=true} Button {text:"热门";checkable:true;checked:!settingModel.latestComments;onClicked:settingModel.latestComments=false}}
 Text {width:parent.width;text:"设置保存在本机，不修改微博账号设置。";font.pixelSize:18;color:AppTheme.color("#87929c",settingModel.darkMode);wrapMode:Text.Wrap}
 }}
 tools:ToolBarLayout {ToolIcon {iconId:(settingModel.darkMode?"icon-m-toolbar-back-white":"icon-m-toolbar-back");onClicked:listPage.viewBack()}}
}
