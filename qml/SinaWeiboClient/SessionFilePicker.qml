import QtQuick 1.1
import "AppTheme.js" as AppTheme
import com.nokia.meego 1.0
import "controls"
Rectangle {
 id:picker;color:AppTheme.color("white",settingModel.darkMode);property string folder:"";property string parentFolder:"";property variant entries:[];property string errorText:""
 signal cancelled()
 signal selected(string path)
 function load(path){var result=loginModel.browserSessionFiles(path);if(!result.folder){errorText="无法打开文档目录，请退出 USB 存储模式后重试。";return;}folder=result.folder;parentFolder=result.parent||"";entries=result.entries||[];errorText="";}
 Component.onCompleted:load("")
 AccTitleBar {id:heading;width:parent.width;height:65;title:"选择 TXT 文件"}
 Item {id:location;anchors.top:heading.bottom;width:parent.width;height:76
  Button {id:up;x:16;anchors.verticalCenter:parent.verticalCenter;width:140;text:"上一级";enabled:picker.parentFolder!=="";onClicked:picker.load(picker.parentFolder)}
  Text {anchors.left:up.right;anchors.leftMargin:16;anchors.right:parent.right;anchors.rightMargin:16;anchors.verticalCenter:parent.verticalCenter;text:picker.folder.replace("/home/user/MyDocs","文档");font.pixelSize:18;color:AppTheme.color("#75808a",settingModel.darkMode);elide:Text.ElideLeft}
 }
 ListView {anchors.top:location.bottom;anchors.bottom:backArea.top;width:parent.width;clip:true;model:picker.entries
  delegate:Rectangle {width:parent.width;height:68;color:tap.pressed?AppTheme.color("#edf3f6",settingModel.darkMode):AppTheme.color("white",settingModel.darkMode)
   Text {anchors.left:parent.left;anchors.leftMargin:20;anchors.right:parent.right;anchors.rightMargin:20;anchors.verticalCenter:parent.verticalCenter;text:(modelData.directory?"›  ":"")+modelData.name;font.pixelSize:22;color:modelData.directory?AppTheme.color("#337ea9",settingModel.darkMode):AppTheme.color("#202830",settingModel.darkMode);elide:Text.ElideRight}
   Rectangle {x:20;anchors.bottom:parent.bottom;width:parent.width-40;height:1;color:AppTheme.color("#edf0f2",settingModel.darkMode)}
   MouseArea {id:tap;anchors.fill:parent;onClicked:{if(modelData.directory)picker.load(modelData.path);else picker.selected(modelData.path);}}
  }
  footer:Item {width:parent.width;height:picker.entries.length===0?100:0;visible:height>0;clip:true;Text {anchors.centerIn:parent;width:parent.width-40;text:picker.errorText||"此目录没有 TXT 文件，可进入其他文件夹查找。";font.pixelSize:20;color:AppTheme.color("#87929c",settingModel.darkMode);wrapMode:Text.Wrap;horizontalAlignment:Text.AlignHCenter}}
 }
 Rectangle {id:backArea;anchors.bottom:parent.bottom;width:parent.width;height:80;color:AppTheme.color("#f7f8fa",settingModel.darkMode);Button {anchors.centerIn:parent;text:"返回";width:180;onClicked:picker.cancelled()}}
}
