import QtQuick 1.1
import "AppTheme.js" as AppTheme
import com.nokia.meego 1.0
import "controls"
Rectangle {
 id:root;color:AppTheme.color("#f7f8fa",settingModel.darkMode);property bool busy:false;property string feedback:"";property string selectedFile:"";property bool filePickerVisible:false
 signal cancelled()
 signal imported()
 AccTitleBar {id:heading;width:parent.width;height:65;title:"辅助登录"}
 Flickable {id:instructions;anchors.top:heading.bottom;anchors.bottom:actions.top;width:parent.width;contentWidth:width;contentHeight:body.height+32;clip:true
  Column {id:body;x:20;y:16;width:parent.width-40;spacing:20
   Text {width:parent.width;text:"通过电脑登录微博后，把登录文件（Cookie）导入 N9 ，导入后微博 MeeGo 版可保持登录状态独立使用，微博有一定可能要求重新验证。";font.pixelSize:22;color:AppTheme.color("#40505f",settingModel.darkMode);wrapMode:Text.Wrap}
   Text {width:parent.width;text:"Cookie 是登录凭证，请不要分享给他人。";font.pixelSize:19;color:AppTheme.color("#75808a",settingModel.darkMode);wrapMode:Text.Wrap}
   Button {text:"选择 TXT 文件";width:240;enabled:!root.busy;onClicked:{sessionInput.closeSoftwareInputPanel();sessionInput.focus=false;root.filePickerVisible=true;}}
   Text {width:parent.width;visible:root.selectedFile!=="";text:"已选择："+root.selectedFile.split("/").pop();font.pixelSize:19;color:AppTheme.color("#52606c",settingModel.darkMode);wrapMode:Text.Wrap}
   TextField {platformStyle:AppInputStyle {}
id:sessionInput;width:parent.width;maximumLength:32767;enabled:!root.busy;placeholderText:"或在此粘贴 Cookie 字段";onTextChanged:{if(text!=="")root.selectedFile="";} echoMode:TextInput.Password;inputMethodHints:Qt.ImhHiddenText|Qt.ImhNoPredictiveText;onActiveFocusChanged:{if(activeFocus)instructions.contentY=Math.max(0,body.y+y+height-instructions.height+24);}}
   Text {width:parent.width;text:root.feedback;visible:text!=="";font.pixelSize:19;color:AppTheme.color("#52606c",settingModel.darkMode);wrapMode:Text.Wrap}
   Text {text:"电脑端准备步骤";font.pixelSize:22;color:AppTheme.color("#40505f",settingModel.darkMode)}
   Text {width:parent.width;text:"1. 电脑端登录\n在电脑浏览器打开 m.weibo.cn，完成登录。\n\n2. 找到并复制 Cookie\n① 按 F12 打开开发者工具。Mac 可按 Option + Command + I\n② 点击 Network（网络）标签，然后刷新一次网页\n③ 在搜索输入 config。列表可能有多个 config，在 Chrome 中它前面带有小齿轮图标\n④ 点击 config，打开 Headers（标头）标签\n⑤ 向下找到 Request Headers（请求标头），展开后找到 Cookie\n⑥ 在 Cookie 的值上点右键，选择 Copy value（复制值），此处不要复制整个请求，也不要选 Response Headers（响应标头）。若没有 config，可刷新页面，或查找 getIndex 请求，需确认域名是 m.weibo.cn\n\n3. 导入到 N9\n把复制的内容存为 UTF-8 编码的 .txt 文件，通过 USB 或蓝牙传到 N9 的目录中，在本页选择 TXT 文件，再点“验证并登录”";font.pixelSize:20;color:AppTheme.color("#52606c",settingModel.darkMode);wrapMode:Text.Wrap}
   Text {width:parent.width;text:"验证成功后 Cookie 将保存在微博 MeeGo 版中，不会上传云端或对外分享，若导入失败不影响电脑端的登录状态。";font.pixelSize:18;color:AppTheme.color("#87929c",settingModel.darkMode);wrapMode:Text.Wrap}
  }
 }
 ScrollDecorator {flickableItem:instructions}
 Rectangle {id:actions;anchors.bottom:parent.bottom;width:parent.width;height:80;color:AppTheme.color("#f7f8fa",settingModel.darkMode)
  Row {anchors.centerIn:parent;spacing:16
   Button {text:"返回";width:150;enabled:!root.busy;onClicked:{sessionInput.text="";sessionInput.closeSoftwareInputPanel();root.cancelled();}}
   Button {text:root.busy?"正在检查…":"验证并登录";width:230;enabled:!root.busy;onClicked:{var value=sessionInput.text;var file=root.selectedFile;sessionInput.text="";root.selectedFile="";sessionInput.closeSoftwareInputPanel();root.busy=true;if(file!=="")loginModel.importBrowserSessionFile(file);else loginModel.importBrowserSession(value);value="";}}
  }
 }
 Connections {target:loginModel;onBrowserSessionImported:{root.busy=code===-1;root.feedback=message;if(code===200)root.imported();}}
 Loader {id:filePicker;anchors.fill:parent;z:10;source:root.filePickerVisible?"SessionFilePicker.qml":""}
 Connections {target:filePicker.item;onCancelled:root.filePickerVisible=false;onSelected:{root.selectedFile=path;sessionInput.text="";root.filePickerVisible=false;}}
 Component.onDestruction:sessionInput.text=""
}
