import QtQuick 1.1
import "AppTheme.js" as AppTheme
import com.nokia.meego 1.0
import karin.weibomm 1.0
Rectangle {
 id:root;color:AppTheme.color("white",settingModel.darkMode);clip:true;focus:true
 property bool largeView:true
 signal cancelled()
 Column {id:heading;width:parent.width;spacing:10
  Text {color:AppTheme.color("black",settingModel.darkMode);text:"离线触摸检查";font.pixelSize:28;anchors.horizontalCenter:parent.horizontalCenter}
  Row {anchors.horizontalCenter:parent.horizontalCenter;spacing:12
   Button {text:"返回登录";width:190;onClicked:root.cancelled()}
   Button {text:root.largeView?"原尺寸":"放大";width:190;onClicked:root.largeView=!root.largeView}
  }
 }
 KarinWebView {
  id:browser;objectName:"calibrationBrowser";focus:true
  anchors.top:heading.bottom;anchors.topMargin:12;anchors.bottom:parent.bottom;anchors.left:parent.left;anchors.right:parent.right
  preferredWidth:width;preferredHeight:height
  contentsScale:root.largeView?1.3:1.0
  onContentsScaleChanged:ConfigureVerificationViewport()
  Component.onCompleted:{ConfigureVerificationViewport();forceActiveFocus();}
  onWidthChanged:ConfigureVerificationViewport()
  onHeightChanged:ConfigureVerificationViewport()
  url:"file:///opt/weibomm/misc/touch-calibration.html"
 }
}
