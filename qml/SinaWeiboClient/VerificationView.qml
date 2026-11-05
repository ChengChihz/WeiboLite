import QtQuick 1.1
import "AppTheme.js" as AppTheme
import com.nokia.meego 1.0
import karin.weibomm 1.0

Rectangle {
 id: root
 color: AppTheme.color("white",settingModel.darkMode)
 focus:true
 clip:true
 function takeInputFocus(){browser.forceActiveFocus();browser.ConfigureVerificationViewport();}
 property bool largeView:true
 property string lastDiagnosticState:""
 property string challengeId: loginModel.verificationId
 signal verified()
 signal cancelled()
 Column {
  id: heading
  width: parent.width
  spacing: 12
  Text {text: "微博安全验证";font.pixelSize:28;color:AppTheme.color("#222",settingModel.darkMode);anchors.horizontalCenter:parent.horizontalCenter}
  Text {width:parent.width-32;anchors.horizontalCenter:parent.horizontalCenter;text:"正在加载安全验证组件。由于设备性能限制，N9 加载时间较长，请耐心等待。如多次验证未能通过，请尝试辅助登录。";font.pixelSize:18;color:AppTheme.color("#75808a",settingModel.darkMode);wrapMode:Text.Wrap;horizontalAlignment:Text.AlignHCenter;visible:root.lastDiagnosticState===""||root.lastDiagnosticState==="sdk_loading"||root.lastDiagnosticState==="provider_loading"||root.lastDiagnosticState==="initializing"}
  Row {anchors.horizontalCenter:parent.horizontalCenter;spacing:12
   Button {text:"重新加载";width:145;onClicked:browser.reload.trigger()}
   Button {text:root.largeView?"原尺寸":"放大";width:145;onClicked:root.largeView=!root.largeView}
   Button {text:"返回";width:145;onClicked:root.cancelled()}
  }
 }
 KarinWebView {
  id: browser
  objectName:"verificationBrowser"
  focus:true
  contentsScale:root.largeView?1.3:1.0
  onLoadStarted:root.lastDiagnosticState="sdk_loading"
  onContentsScaleChanged:ConfigureVerificationViewport()
  Component.onCompleted:root.takeInputFocus()
  onWidthChanged:ConfigureVerificationViewport()
  onHeightChanged:ConfigureVerificationViewport()
  anchors.top:heading.bottom;anchors.topMargin:12
  anchors.bottom:parent.bottom;anchors.left:parent.left;anchors.right:parent.right
  preferredWidth:width;preferredHeight:height
  url:root.challengeId ? "file:///opt/weibomm/misc/verification.html#"+encodeURIComponent(root.challengeId) : "about:blank"
 }
 Timer {
  interval:800;repeat:true;running:root.visible && root.challengeId!==""
  onTriggered: {
   var state=browser.evaluateJavaScript("window.n9VerificationState || ''");
   if(state!==root.lastDiagnosticState){root.lastDiagnosticState=state;
    loginModel.reportVerificationDiagnostic(state,Number(browser.evaluateJavaScript("window.n9VerificationErrorCode || 0")),Number(browser.evaluateJavaScript("window.n9VerificationElapsed || 0")));}
   if(state==="verified"){stop();root.verified();}
  }
 }
}
