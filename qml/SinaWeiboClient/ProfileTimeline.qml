import QtQuick 1.1
import "AppTheme.js" as AppTheme
import SinaWeiboTypeLib 1.0
import "controls"
Rectangle {
 id:home;color:AppTheme.color("white",settingModel.darkMode)
 property string userId:""
 property string initialName:""
 property bool own:userId!=="" && userId===String(loginModel.userID)
 property bool initialized:false
 property bool ready:false
 property bool busy:false
 property string statusText:""
 property bool allowNetwork:true
 function ensureLoaded(){if(!initialized && ready && visible && userId!=="")loadTimer.restart();}
 function fetchProfile(){if(userId===""||!allowNetwork){profileRefresh.finish(200);return;}var first=!initialized;initialized=true;busy=true;statusText="";person.getSinaWeiboProfile(userId,initialName);if(!own)person.getAttention(userId,initialName);timeline.connectModel();if(first)timeline.setUser(userId,initialName);timeline.getMyWeiBoFromModel(0);busy=false;if(first)topTimer.restart();}
 function refresh(){if(userId!==""&&allowNetwork)profileRefresh.start(0);}
 RefreshPulse {id:profileRefresh;objectName:"profileRefresh";anchors.top:heading.bottom;width:parent.width;list:posts;onRequested:home.fetchProfile()}
 function openCount(tab){funcContainer.myfansId=userId;funcContainer.senderScreenName=person.nick;funcContainer.listContainerTabIndex=tab;listPage.changePage(41);}
 onUserIdChanged:{initialized=false;ensureLoaded();}
 onVisibleChanged:ensureLoaded()
 Component.onCompleted:{ready=true;ensureLoaded();}
 Timer {id:topTimer;interval:1;onTriggered:posts.positionViewAtBeginning()}
 Timer {id:loadTimer;interval:1;onTriggered:home.refresh()}
 WeiboProfileModel {id:person;onReadyFinishProfile:{if(errCode!==200 && errCode!==-1)home.statusText="资料暂时无法加载";}onCreateOrDestroyAttention:{if(errCode!==200)funcContainer.showPopup("关注状态未能更新");}}
 MyWeiboListModel {id:timeline;onGetUserTimelineItemResult:{home.busy=errCode===-1;profileRefresh.finish(errCode);if(errCode!==200 && errCode!==-1)home.statusText="微博加载失败，请下拉重试";}}
 Rectangle {id:heading;width:parent.width;height:65;color:AppTheme.color("#f8fafb",settingModel.darkMode);z:2
 Text {x:16;width:parent.width-90;anchors.verticalCenter:parent.verticalCenter;text:home.own?"我的主页":(person.nick||home.initialName||"用户主页");font.pixelSize:28;color:AppTheme.color("#202830",settingModel.darkMode);elide:Text.ElideRight}
 Item {anchors.right:parent.right;width:64;height:65;Image{anchors.centerIn:parent;width:27;height:27;source:home.own?(settingModel.darkMode?"controls/icons/dark/more.svg":"controls/icons/more.svg"):(settingModel.darkMode?"controls/icons/dark/refresh.svg":"controls/icons/refresh.svg")}MouseArea{anchors.fill:parent;onClicked:{if(home.own)listPage.changePage(46);else home.refresh();}}}
 Rectangle {anchors.bottom:parent.bottom;width:parent.width;height:1;color:AppTheme.color("#e7ebee",settingModel.darkMode)}
 }
 SinaWeiboItemList {id:posts;objectName:"profilePosts";refreshInProgress:profileRefresh.busy;interactionLocked:profileRefresh.busy;anchors.topMargin:profileRefresh.offset;anchors.top:heading.bottom;anchors.bottom:parent.bottom;width:parent.width;clip:true;model:timeline
 header:Item {width:posts.width;height:summary.height+32
 Column {id:summary;objectName:"profileSummary";x:16;y:16;width:parent.width-32;spacing:16
 Item {width:parent.width;height:76
 Image {id:portrait;width:72;height:72;source:person.userIcon;fillMode:Image.PreserveAspectCrop;smooth:true;Image{anchors.fill:parent;source:(settingModel.darkMode?"controls/icons/dark/avatar-corners.svg":"controls/icons/avatar-corners.svg")}Rectangle{visible:person.isVip;width:20;height:20;radius:10;anchors.right:parent.right;anchors.bottom:parent.bottom;color:"#e6a037";Text{anchors.centerIn:parent;text:"V";color:"white";font.pixelSize:14}}}
 Text {x:88;y:6;width:parent.width-192;text:person.nick||home.initialName;font.pixelSize:26;color:AppTheme.color("#202830",settingModel.darkMode);elide:Text.ElideRight}
 Text {x:88;y:43;width:parent.width-192;text:person.address==="未提供"?"":person.address;font.pixelSize:17;color:AppTheme.color("#82919c",settingModel.darkMode);elide:Text.ElideRight}
 FlatActionButton {anchors.right:parent.right;anchors.verticalCenter:parent.verticalCenter;width:96;text:home.own?"编辑":person.isFollowing?"已关注":"关注";onClicked:{if(home.own){profile.getSinaWeiboProfile(home.userId,person.nick);listPage.changePage(4);}else if(person.isFollowing)person.destroyAttentionShip(home.userId,person.nick);else person.createAttentionShip(home.userId,person.nick);}}
 }
 Text {width:parent.width;text:person.account||"暂无简介";font.pixelSize:21;color:AppTheme.color("#5a6671",settingModel.darkMode);wrapMode:Text.WrapAtWordBoundaryOrAnywhere;textFormat:Text.PlainText}
 Rectangle {width:parent.width;height:1;color:AppTheme.color("#edf0f2",settingModel.darkMode)}
 Row {width:parent.width;spacing:0
 Repeater {model:["微博","关注","粉丝"]
 Item {width:summary.width/3;height:50
 Rectangle {visible:index>0;x:0;y:12;width:1;height:26;color:AppTheme.color("#e7ebee",settingModel.darkMode)}
 Text {anchors.centerIn:parent;text:(index===0?person.weibo:index===1?person.attention:person.funs)+" "+modelData;font.pixelSize:21;color:index===0?AppTheme.color("#202830",settingModel.darkMode):AppTheme.color("#337ea9",settingModel.darkMode)}
 MouseArea {anchors.fill:parent;onClicked:{if(index===0)posts.positionViewAtIndex(0,ListView.Beginning);else home.openCount(index===1?4:3);}}
 }} }
 Text {visible:posts.count>0&&!profileRefresh.busy&&text!=="";text:home.statusText;font.pixelSize:16;color:AppTheme.color("#82919c",settingModel.darkMode)}

 Rectangle {width:parent.width;height:1;color:AppTheme.color("#e7ebee",settingModel.darkMode)}
 }}
 delegate:AccListSina {width:posts.width;showPic:funcContainer.readMode;onItemClicked:{funcContainer.statusIDInDetailPage=timeline.getMyWeiBoStudusId(index);listPage.changePage(6);}onStatusImageClicked:funcContainer.showDetailImage(timeline.getImageAddress(0,index),timeline.getImageAddress(1,index));onForwardImageClicked:funcContainer.showDetailImage(timeline.getImageAddress(2,index),timeline.getImageAddress(3,index));}
 footer:Item {width:posts.width;visible:posts.count===0&&home.initialized&&!home.busy&&!profileRefresh.busy;clip:true;height:visible?72:0;Text{anchors.centerIn:parent;text:home.statusText||"暂无可显示的微博";font.pixelSize:20;color:AppTheme.color("#82919c",settingModel.darkMode)}}
 onItemlistscrolled:{if(type===0)profileRefresh.start(posts.releasedPullDistance);else timeline.getMyWeiBoFromModel(type);}

 }
}
