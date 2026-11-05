import QtQuick 1.1
import "AppTheme.js" as AppTheme
import com.nokia.meego 1.0
import "controls"
Page {
 orientationLock:PageOrientation.LockPortrait;

 id:searchPage;anchors.fill:parent;property bool embedded:false
 
 property bool searchUsers:false
 property bool ready:false
 property int hotCount:0
 property string hotState:""
 function activepage(){funcContainer.hometabindex=3;}
 function moreHot(){funcContainer.showStatusOfTrend=true;listPage.changePage(20);}
 function searchWord(word){word=String(word||"").replace(/^\s+|\s+$/g,"");if(!word){funcContainer.showPopup("请输入搜索条件");return;}searchInput.closeSoftwareInputPanel();funcContainer.searchContent=word;if(searchUsers){funcContainer.showUserInfo=true;listPage.changePage(22);}else listPage.changePage(21);}
 function hotFinished(code){hotState=code===-1?"正在加载…":code===200?"":"热搜暂时无法加载，点击重试";hotCount=code===200?Math.min(10,hotTrendModel.itemCount()):0;}
 Component.onCompleted:{ready=true;hotTrendModel.getHotTrendResult.connect(hotFinished);hotCount=Math.min(10,hotTrendModel.itemCount());if(visible&&!hotTrendModel.hasData())reload.restart();}
 Component.onDestruction:hotTrendModel.getHotTrendResult.disconnect(hotFinished)
 onVisibleChanged:{if(ready&&visible&&!hotTrendModel.hasData())reload.restart();}
 Timer {id:reload;interval:100;onTriggered:hotTrendModel.getSinaHotTrendFromModel(0)}
 Rectangle {anchors.fill:parent;color:AppTheme.color("white",settingModel.darkMode)}
 AccTitleBar {id:heading;width:parent.width;height:65;title:"搜索"}
 ButtonRow {id:types;anchors.top:heading.bottom;anchors.topMargin:16;x:16;width:parent.width-32
  Button {text:"搜微博";checkable:true;checked:!searchPage.searchUsers;onClicked:searchPage.searchUsers=false}
  Button {text:"搜人";checkable:true;checked:searchPage.searchUsers;onClicked:searchPage.searchUsers=true}
 }
 TextField {id:searchInput;anchors.top:types.bottom;anchors.topMargin:16;x:16;width:parent.width-32;placeholderText:searchPage.searchUsers?"输入昵称，寻找用户":"输入关键词，搜索微博";inputMethodHints:Qt.ImhNoPredictiveText;Keys.onReturnPressed:searchWord(text);platformStyle:AppInputStyle {paddingRight:52}
  Image {width:28;height:28;anchors.right:parent.right;anchors.rightMargin:14;anchors.verticalCenter:parent.verticalCenter;source:(settingModel.darkMode?"image://theme/icon-m-toolbar-search-white":"image://theme/icon-m-toolbar-search")}
  MouseArea {anchors.right:parent.right;width:52;height:parent.height;onClicked:searchPage.searchWord(searchInput.text)}
 }
 Item {id:hotHeading;anchors.top:searchInput.bottom;anchors.topMargin:16;width:parent.width;height:64
  Text {x:16;anchors.verticalCenter:parent.verticalCenter;text:"微博热搜";font.pixelSize:24;color:AppTheme.color("#202830",settingModel.darkMode)}
  Text {anchors.right:parent.right;anchors.rightMargin:16;anchors.verticalCenter:parent.verticalCenter;text:"更多 >";font.pixelSize:20;color:AppTheme.color("#337ea9",settingModel.darkMode)}
  MouseArea {anchors.right:parent.right;width:100;height:parent.height;onClicked:searchPage.moreHot()}
  Rectangle {x:16;anchors.bottom:parent.bottom;width:parent.width-32;height:1;color:AppTheme.color("#e7ebee",settingModel.darkMode)}
 }
 ListView {id:hotList;objectName:"searchHotList";anchors.top:hotHeading.bottom;anchors.bottom:parent.bottom;width:parent.width;clip:true;model:searchPage.hotCount
  delegate:Rectangle {width:hotList.width;height:62;color:tap.pressed?AppTheme.color("#f3f6f8",settingModel.darkMode):AppTheme.color("white",settingModel.darkMode)
   Text {x:16;width:34;anchors.verticalCenter:parent.verticalCenter;text:index+1;font.pixelSize:21;color:index<3?AppTheme.color("#337ea9",settingModel.darkMode):AppTheme.color("#82919c",settingModel.darkMode)}
   Text {x:58;width:parent.width-74;anchors.verticalCenter:parent.verticalCenter;text:hotTrendModel.getTrendName(index);font.pixelSize:22;color:AppTheme.color("#202830",settingModel.darkMode);elide:Text.ElideRight}
   Rectangle {x:58;anchors.bottom:parent.bottom;width:parent.width-74;height:1;color:AppTheme.color("#edf0f2",settingModel.darkMode)}
   MouseArea {id:tap;anchors.fill:parent;onClicked:{searchPage.searchUsers=false;searchPage.searchWord(hotTrendModel.getTrendName(index));}}
  }
  footer:Item {width:hotList.width;height:68
   Text {anchors.centerIn:parent;text:searchPage.hotState||"更多热搜 >";font.pixelSize:20;color:searchPage.hotState!==""?AppTheme.color("#82919c",settingModel.darkMode):AppTheme.color("#337ea9",settingModel.darkMode)}
   MouseArea {anchors.fill:parent;onClicked:{if(searchPage.hotState!=="")reload.restart();else searchPage.moreHot();}}
  }
 }
 tools:embedded?null:searchTools
 ToolBarLayout {id:searchTools;visible:!searchPage.embedded;ToolIcon {iconId:(settingModel.darkMode?"icon-m-toolbar-back-white":"icon-m-toolbar-back");onClicked:listPage.viewBack()}}
}
