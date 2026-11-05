import QtQuick 1.1
import "AppTheme.js" as AppTheme
import com.nokia.meego 1.0
import "controls"
Page {
 orientationLock:PageOrientation.LockPortrait;

 id:commentsPage;anchors.fill:parent;property bool ready:false;property bool busy:false;property string loadError:""
 function refresh(){commentRefresh.start(0);}
 function finished(code){commentRefresh.finish(code);busy=code===-1;loadError=code!==-1&&code!==200?"评论暂时无法加载，请重试":"";}
 Component.onCompleted:{contentCommentModel.latest=funcContainer.commentOrderOverride<0?settingModel.latestComments:funcContainer.commentOrderOverride===1;funcContainer.commentOrderOverride=-1;ready=true;contentCommentModel.getContentCommentResult.connect(finished);reload.restart();}
 Component.onDestruction:contentCommentModel.getContentCommentResult.disconnect(finished)
 onStatusChanged:{if(ready&&status===PageStatus.Active)reload.restart();}
 Timer {id:reload;interval:300;onTriggered:commentsPage.refresh() }
 RefreshPulse {id:commentRefresh;z:2;anchors.top:message.bottom;width:parent.width;list:comments;onRequested:contentCommentModel.getContentCommentFromModel(funcContainer.commentStatusID,0)}
 Rectangle {anchors.fill:parent;color:AppTheme.color("white",settingModel.darkMode)}
 AccTitleBar {id:heading;width:parent.width;height:65;title:"微博评论"
  Item {anchors.right:parent.right;width:64;height:65;Image {anchors.centerIn:parent;width:34;height:34;source:(settingModel.darkMode?"controls/icons/dark/refresh.svg":"controls/icons/refresh.svg")}MouseArea {anchors.fill:parent;onClicked:commentsPage.refresh()}}
 }
 ButtonRow {id:order;enabled:!commentRefresh.busy;anchors.top:heading.bottom;anchors.topMargin:12;x:16;width:parent.width-32
  Button {text:"最新";checkable:true;checked:contentCommentModel.latest;onClicked:{contentCommentModel.latest=true;commentsPage.refresh();}}
  Button {text:"热门";checkable:true;checked:!contentCommentModel.latest;onClicked:{contentCommentModel.latest=false;commentsPage.refresh();}}
 }
 Text {id:message;anchors.top:order.bottom;anchors.topMargin:8;x:16;width:parent.width-32;height:text!==""?32:8;text:commentsPage.busy&&!commentRefresh.busy?"正在刷新…":commentsPage.loadError;font.pixelSize:18;color:AppTheme.color("#82919c",settingModel.darkMode)}
 SinaWeiboItemList {id:comments;anchors.top:message.bottom;anchors.topMargin:commentRefresh.offset;refreshInProgress:commentRefresh.busy;interactionLocked:commentRefresh.busy;anchors.bottom:parent.bottom;width:parent.width;model:contentCommentModel;readMode:funcContainer.readMode;clip:true;onItemlistscrolled:{if(type===0)commentRefresh.start(comments.releasedPullDistance);else contentCommentModel.getContentCommentFromModel(funcContainer.commentStatusID,type);}}
 tools:ToolBarLayout {visible:false
  ToolIcon {iconId:(settingModel.darkMode?"icon-m-toolbar-back-white":"icon-m-toolbar-back");onClicked:listPage.viewBack()}
  ToolIcon {transform:Translate {y:3} iconSource:(settingModel.darkMode?"image://nighticon/icon_comment.png":"images/icon_comment.png");onClicked:listPage.changePage(9)}
 }
}
