import QtQuick 1.1
import "../AppTheme.js" as AppTheme
import com.nokia.meego 1.0
Rectangle {
 id:pulse;color:AppTheme.color("white",settingModel.darkMode);height:offset;clip:true;visible:height>0
 property variant list:null
 property real offset:0
 property bool busy:false
 signal requested()
 function start(distance){if(busy||!list)return;busy=true;var held=typeof distance==="number"&&distance>=72?Math.min(distance,Math.max(72,parent.height-150)):0;offset=held;list.positionViewAtBeginning();opening.from=held;opening.to=72;opening.start();dispatch.restart();}
 function finish(code){if(code!==-1&&busy)hold.restart();}
 NumberAnimation {id:opening;target:pulse;property:"offset";duration:180;easing.type:Easing.OutCubic}
 NumberAnimation {id:closing;target:pulse;property:"offset";to:0;duration:180;easing.type:Easing.OutCubic;onRunningChanged:{if(!running&&pulse.offset<1)pulse.busy=false;}}
 Timer {id:dispatch;interval:200;onTriggered:pulse.requested()}
 Timer {id:hold;interval:350;onTriggered:{closing.from=pulse.offset;closing.start();}}
 Row {anchors.centerIn:parent;spacing:16;BusyIndicator {width:28;height:28;running:pulse.busy} Text {anchors.verticalCenter:parent.verticalCenter;text:"正在刷新…";font.pixelSize:20;color:AppTheme.color("#75808a",settingModel.darkMode)}}
}
