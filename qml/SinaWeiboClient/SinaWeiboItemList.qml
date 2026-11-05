import QtQuick 1.1
import "AppTheme.js" as AppTheme
import com.nokia.meego 1.0
import "controls" as Controls
ListView {
 id:itemlist
 property int readMode:0
 property bool allowRefresh:false
 property bool needRefresh:model?!model.isReadingDB:true
 property bool refreshInProgress:false
 property bool interactionLocked:false
 property bool beganAtTop:false
 property bool pullDispatched:false
 property real gestureOriginY:0
 property real releasedPullDistance:0
 property real pullDistance:beganAtTop?Math.max(0,gestureOriginY-contentY):0
 signal itemlistscrolled(int type)
 signal itemSelected(int index)
 signal showStatusImage(int index)
 signal showForwardImage(int index)
 clip:true;focus:true;interactive:!interactionLocked;orientation:ListView.Vertical;flickDeceleration:1000
 Rectangle {parent:itemlist;width:itemlist.width;height:72;y:itemlist.pullDistance-72;visible:itemlist.needRefresh&&!itemlist.refreshInProgress&&itemlist.pullDistance>0;color:AppTheme.color("white",settingModel.darkMode);z:10
  Row {anchors.centerIn:parent;spacing:16
   BusyIndicator {width:28;height:28;running:itemlist.allowRefresh;visible:itemlist.allowRefresh}
   Text {visible:!itemlist.allowRefresh;text:"↓";font.pixelSize:26;color:AppTheme.color("#87929c",settingModel.darkMode)}
   Text {anchors.verticalCenter:parent.verticalCenter;text:itemlist.allowRefresh?"松开即可刷新":"下拉刷新";font.pixelSize:20;color:AppTheme.color("#75808a",settingModel.darkMode)}
  }
 }
 delegate:Controls.AccListSina {showPic:itemlist.readMode;onItemClicked:itemlist.itemSelected(index);onStatusImageClicked:itemlist.showStatusImage(index);onForwardImageClicked:itemlist.showForwardImage(index)}
 onMovementStarted:{beganAtTop=atYBeginning;gestureOriginY=contentY;allowRefresh=false;pullDispatched=false;releasedPullDistance=0;}
 onContentYChanged:{if(moving&&beganAtTop&&needRefresh&&!interactionLocked)allowRefresh=pullDistance>=72;}
 function releasePull(){if(visible&&allowRefresh&&needRefresh&&!interactionLocked){releasedPullDistance=pullDistance;allowRefresh=false;pullDispatched=true;itemlistscrolled(0);}}
 Connections {target:typeof _UT!=="undefined"?_UT:null;onPointerReleased:itemlist.releasePull()}
 onMovementEnded:{
  if(!visible)return;
  if(pullDispatched){pullDispatched=false;return;}
  if(interactionLocked)return;
  if(allowRefresh&&needRefresh){releasedPullDistance=pullDistance;allowRefresh=false;itemlistscrolled(0);}
  else if(atYEnd&&count>0)itemlistscrolled(2);
  else if(atYBeginning&&!needRefresh&&count>0)itemlistscrolled(1);
 }
}
