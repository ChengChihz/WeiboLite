import QtQuick 1.1
import com.nokia.meego 1.0
Page {
 orientationLock:PageOrientation.LockPortrait;
id:userPage;property string targetId:"";property string targetName:"";Component.onCompleted:{targetId=String(funcContainer.userIDInMyProfilePage);targetName=funcContainer.userNameInProfilePage;}
 ProfileTimeline {anchors.fill:parent;userId:userPage.targetId;initialName:userPage.targetName}
 tools:ToolBarLayout {ToolIcon {iconId:theme.inverted?"icon-m-toolbar-back-white":"icon-m-toolbar-back";onClicked:listPage.viewBack()}}
}
