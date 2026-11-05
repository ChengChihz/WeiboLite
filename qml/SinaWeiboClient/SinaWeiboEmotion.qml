import QtQuick 1.1
import "AppTheme.js" as AppTheme
Rectangle {
 id:emotion;color:AppTheme.color("#f7f8fa",settingModel.darkMode)
 property string path:emotionlist.getPath()
 property string emotionContent:""
 property int itemcountperpage:funcContainer.inPortrait?24:30
 property alias currentPage:view.currentIndex
 signal itemClick(string emotionString)
 function selectEmotion(page,slot){var code=emotionlist.getEmotionCode(page,slot,itemcountperpage);if(code==="")return false;emotionContent=code;emotion.itemClick(code);return true;}
 onItemcountperpageChanged:view.currentIndex=0
 ListView {id:view;anchors.top:parent.top;anchors.bottom:indicator.top;width:parent.width;clip:true;model:emotionlist.getPageCount(itemcountperpage);orientation:ListView.Horizontal;snapMode:ListView.SnapOneItem;highlightRangeMode:ListView.StrictlyEnforceRange;preferredHighlightBegin:0;preferredHighlightEnd:0;flickDeceleration:2000
 delegate:Item {width:view.width;height:view.height;property int pageIndex:index
 GridView {anchors.fill:parent;model:emotion.itemcountperpage;interactive:false;cellWidth:view.width/(funcContainer.inPortrait?6:10);cellHeight:view.height/(funcContainer.inPortrait?4:3)
 delegate:Item {width:GridView.view.cellWidth;height:GridView.view.cellHeight;property string code:emotionlist.getEmotionCode(pageIndex,index,emotion.itemcountperpage)
 Rectangle {anchors.fill:parent;anchors.margins:3;radius:6;color:tap.pressed?AppTheme.color("#e4ebf1",settingModel.darkMode):"transparent"}
 Image {anchors.centerIn:parent;width:38;height:38;fillMode:Image.PreserveAspectFit;smooth:true;source:emotionlist.getIconPath(pageIndex,index,emotion.itemcountperpage)}
 MouseArea {id:tap;anchors.fill:parent;enabled:parent.code!=="";onClicked:emotion.selectEmotion(pageIndex,index)}
 }}}
 }
 Item {id:indicator;anchors.bottom:parent.bottom;width:parent.width;height:36
 Row {anchors.centerIn:parent;spacing:16;Repeater {model:emotionlist.getPageCount(itemcountperpage);Rectangle {width:8;height:8;radius:4;color:view.currentIndex===index?AppTheme.color("#5c7e91",settingModel.darkMode):AppTheme.color("#cbd3da",settingModel.darkMode);MouseArea{width:24;height:28;anchors.centerIn:parent;onClicked:view.currentIndex=index}}}}
 }
}
