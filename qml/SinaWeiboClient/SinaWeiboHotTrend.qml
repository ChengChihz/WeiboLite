import QtQuick 1.1
import "AppTheme.js" as AppTheme
import com.nokia.meego 1.0
import "controls"
Page {
 orientationLock:PageOrientation.LockPortrait;

 id:hotPage;anchors.fill:parent;property bool ready:false;property string loadState:""
 function finished(code){loadState=code===-1?"正在加载…":code===200?"":"热搜暂时无法加载，请重试";}
 function selectWord(index){var word=hotTrendModel.getTrendName(index);if(funcContainer.showStatusOfTrend){funcContainer.searchContent=word;listPage.changePage(21);}else{funcContainer.insertTopic=word;insertTopicModel.setRecentTopicToModel(word);settingModel.saveRecentTopic(loginModel.userID,word);var targets=[11,9,8,25,45];listPage.jumpPageTo(targets[funcContainer.microBlogId]);hotTrendModel.selectTopicItemFinished();}}
 Component.onCompleted:{ready=true;hotTrendModel.getHotTrendResult.connect(finished);if(!hotTrendModel.hasData())reload.restart();}
 Component.onDestruction:hotTrendModel.getHotTrendResult.disconnect(finished)
 Timer {id:reload;interval:300;onTriggered:hotTrendModel.getSinaHotTrendFromModel(0)}
 Rectangle {anchors.fill:parent;color:AppTheme.color("white",settingModel.darkMode)}
 AccTitleBar {id:heading;width:parent.width;height:65;title:"热搜榜"
  Item {anchors.right:parent.right;width:64;height:65;Image {anchors.centerIn:parent;width:32;height:32;source:(settingModel.darkMode?"controls/icons/dark/refresh.svg":"controls/icons/refresh.svg")}MouseArea {anchors.fill:parent;onClicked:reload.restart()}}
 }
 ListView {id:words;anchors.top:heading.bottom;anchors.bottom:parent.bottom;width:parent.width;model:hotTrendModel;clip:true
  delegate:Rectangle {width:words.width;height:64;color:tap.pressed?AppTheme.color("#f3f6f8",settingModel.darkMode):AppTheme.color("white",settingModel.darkMode)
   Text {x:16;width:38;anchors.verticalCenter:parent.verticalCenter;text:index+1;font.pixelSize:22;color:index<3?AppTheme.color("#337ea9",settingModel.darkMode):AppTheme.color("#82919c",settingModel.darkMode)}
   Text {x:62;width:parent.width-78;anchors.verticalCenter:parent.verticalCenter;text:trendName;font.pixelSize:23;color:AppTheme.color("#202830",settingModel.darkMode);elide:Text.ElideRight}
   Rectangle {x:62;anchors.bottom:parent.bottom;width:parent.width-78;height:1;color:AppTheme.color("#edf0f2",settingModel.darkMode)}
   MouseArea {id:tap;anchors.fill:parent;onClicked:hotPage.selectWord(index)}
  }
  footer:Item {visible:hotPage.loadState!=="";width:words.width;height:visible?64:0;clip:true;Text {anchors.centerIn:parent;text:hotPage.loadState;font.pixelSize:20;color:AppTheme.color("#82919c",settingModel.darkMode)}}
 }
 tools:ToolBarLayout {visible:false;ToolIcon {iconId:(settingModel.darkMode?"icon-m-toolbar-back-white":"icon-m-toolbar-back");onClicked:listPage.viewBack()}}
}
