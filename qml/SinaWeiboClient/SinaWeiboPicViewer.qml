import QtQuick 1.1
import "AppTheme.js" as AppTheme
import com.nokia.meego 1.0
import "controls"
Page {
 orientationLock:PageOrientation.LockPortrait;

 anchors.fill:parent
 Rectangle {anchors.fill:parent;color:AppTheme.color("#f0f1f2",settingModel.darkMode)}
 ZoomPhoto {id:photo;anchors.fill:parent;source:funcContainer.picSource;onTapped:funcContainer.showToolBar=!funcContainer.showToolBar;onZoomChanged:{if(slider&&Math.abs(slider.value-zoom*100)>0.1)slider.value=zoom*100;}}
 BusyIndicator {anchors.centerIn:parent;width:48;height:48;visible:photo.loading;running:photo.loading}
 Text {anchors.centerIn:parent;visible:photo.failed;text:"图片暂时无法加载";font.pixelSize:22;color:AppTheme.color("#75808a",settingModel.darkMode)}
 tools:ToolBarLayout {visible:false
  ToolIcon {iconId:(settingModel.darkMode?"icon-m-toolbar-back-white":"icon-m-toolbar-back");onClicked:{funcContainer.showStatusBar=true;listPage.viewBack();}}
  Slider {id:slider;width:parent.width*0.65;minimumValue:100;maximumValue:400;stepSize:1;value:100;valueIndicatorVisible:true;valueIndicatorText:Math.round(value)+"%";onValueChanged:{if(photo)photo.zoomTo(value/100)}}
  ToolIcon {iconSource:(settingModel.darkMode?"image://nighticon/icon_cancel.png":"images/icon_cancel.png");visible:funcContainer.isEditable;onClicked:{funcContainer.isPicDeleted=true;funcContainer.showStatusBar=true;listPage.viewBack();}}
 }
 Component.onCompleted:{funcContainer.showStatusBar=true;funcContainer.makeWaitingDialogVisible(false);}
 Component.onDestruction:funcContainer.showStatusBar=true
}
