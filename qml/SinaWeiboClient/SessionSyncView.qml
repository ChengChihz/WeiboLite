import QtQuick 1.1
import "AppTheme.js" as AppTheme
import com.nokia.meego 1.0
import karin.weibomm 1.0

Rectangle {
 id:root
 color:AppTheme.color("white",settingModel.darkMode);focus:true;clip:true
 property string startUrl:loginModel.sessionSyncUrl
 property int checks:0
 property bool navigationBlocked:false
 property bool navigationFailed:false
 signal cancelled()
 Column {
  id:heading;width:parent.width;spacing:10
  Text {id:statusText;text:"正在同步微博登录状态…";font.pixelSize:24;color:AppTheme.color("#222",settingModel.darkMode);anchors.horizontalCenter:parent.horizontalCenter}
  Row {anchors.horizontalCenter:parent.horizontalCenter;spacing:12
   Button {text:"检查状态";width:145;onClicked:loginModel.checkSessionSync()}
   Button {text:"继续登录";width:145;onClicked:{if(loginModel.resumePendingLogin()){root.navigationFailed=false;root.navigationBlocked=false;root.checks=0;statusText.text="正在继续登录…";browser.StartSessionSync(loginModel.sessionSyncUrl);poll.restart();}else statusText.text="登录进度已过期，请返回。";}}
   Button {text:"返回";width:145;onClicked:root.cancelled()}
  }
 }
 KarinWebView {
  id:browser;objectName:"sessionSyncBrowser";focus:true;dontDelegateLinks:true
  anchors.top:heading.bottom;anchors.topMargin:12
  anchors.bottom:parent.bottom;anchors.left:parent.left;anchors.right:parent.right
  preferredWidth:width;preferredHeight:height
  Component.onCompleted:{ConfigureSessionSync();forceActiveFocus();if(root.startUrl)StartSessionSync(root.startUrl);}
  onWidthChanged:ConfigureVerificationViewport()
  onHeightChanged:ConfigureVerificationViewport()
  url:"about:blank"
  onLoadFailed:{root.navigationFailed=true;statusText.text="新浪登录页面加载失败，正在确认状态…";}
  onAlert: {
   if(message==="N9_SSO_NAVIGATION_BLOCKED"){
    root.navigationBlocked=true;statusText.text="正在确认登录状态…";
   }
  }
 }
 Timer {
  id:poll;interval:3000;repeat:true;running:root.visible && root.startUrl!==""
  onTriggered:{
   if(!loginModel.checkSessionSync()){
    root.checks++;
    if(root.navigationFailed){stop();statusText.text="新浪登录跳转失败，请返回登录。";return;}
    if(root.navigationBlocked){stop();statusText.text="登录页面要求另行确认，请返回。";return;}
    if(root.checks>=10){stop();statusText.text="尚未完成登录，请检查页面或返回。";}
   }
  }
 }
}
