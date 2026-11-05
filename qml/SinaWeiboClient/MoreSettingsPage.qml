import QtQuick 1.1
import com.nokia.meego 1.0
Page {
 orientationLock:PageOrientation.LockPortrait;
 SinaWeiboMorePage {anchors.fill:parent} tools:ToolBarLayout{ToolIcon{iconId:theme.inverted?"icon-m-toolbar-back-white":"icon-m-toolbar-back";onClicked:listPage.viewBack()}} }