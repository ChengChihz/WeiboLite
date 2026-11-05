import QtQuick 1.1
import "controls"
import "AppTheme.js" as AppTheme
import com.nokia.meego 1.0
Page {
 orientationLock:PageOrientation.LockPortrait;

 id:loginPage
 property bool addingAccount:false
 property bool auxiliaryVisible:false
 property bool smsMode:true
 property int smsCountdown:0
 property bool verificationVisible:loginModel.verificationId!==""
 property bool sessionVisible:loginModel.sessionSyncUrl!==""
 property bool locked:verificationVisible||sessionVisible||auxiliaryVisible
 property bool inPortrait:funcContainer.inPortrait
 function closeSoftKeyboard(){accountInput.closeSoftwareInputPanel();passwordInput.closeSoftwareInputPanel();accountInput.focus=false;passwordInput.focus=false;}
 function showField(item){view.contentY=Math.max(0,form.y+item.y+item.height-view.height+24);}
 function login(){closeSoftKeyboard();if(!accountInput.text){funcContainer.showPopup(smsMode?"请输入手机号":"请输入账号");return;}if(!passwordInput.text){funcContainer.showPopup(smsMode?"请输入短信验证码":"请输入密码");return;}if(smsMode)loginModel.loginSms(accountInput.text,passwordInput.text);else loginModel.login(accountInput.text,passwordInput.text,"");}
 function loginFinished(code){
  if(!loginPage.visible)return;
  funcContainer.makeWaitingDialogVisible(code===-1);if(code===-1)return;
  if(sessionVisible||verificationVisible)return;
  if(code===201){smsCountdown=60;funcContainer.showPopup("短信已发送，请输入验证码");return;}
  if(code===200){funcContainer.loginTransitionBusy=true;funcContainer.logined=true;funcContainer.needRefresh=true;funcContainer.needRefreshMsg=true;finishTimer.restart();return;}
  funcContainer.showPopup(loginModel.loginData||"登录未完成，请稍后重试");
 }
 Timer {id:finishTimer;interval:120;onTriggered:{funcContainer.hometabindex=0;listPage.changePage(12);}}
 Timer {interval:1000;repeat:true;running:smsCountdown>0;onTriggered:smsCountdown--;}
 Component.onCompleted:loginModel.getloginResult.connect(loginFinished)
 Component.onDestruction:loginModel.getloginResult.disconnect(loginFinished)
 Connections {target:loginModel;onVerificationChanged:{if(loginPage.visible&&loginPage.verificationVisible){loginPage.closeSoftKeyboard();verificationFocus.restart();}}}
 Timer {id:verificationFocus;interval:250;onTriggered:{if(verificationLoader.item)verificationLoader.item.takeInputFocus();}}
 Rectangle {anchors.fill:parent;color:AppTheme.color("white",settingModel.darkMode)}
 Flickable {id:view;anchors.fill:parent;contentWidth:width;contentHeight:Math.max(height,form.y+form.height+32);clip:true
  Image {visible:!settingModel.darkMode;anchors.top:parent.top;anchors.topMargin:inPortrait?Math.round((form.y+64)/2-306):-25;anchors.horizontalCenter:parent.horizontalCenter;width:inPortrait?480:parent.width*0.62;height:inPortrait?854:width*480/854;source:inPortrait?"images/blog_loading.png":"images/blog_loading_landscape.png";smooth:true}
  Column {visible:settingModel.darkMode;y:145;anchors.horizontalCenter:parent.horizontalCenter;spacing:18;Image {anchors.horizontalCenter:parent.horizontalCenter;width:96;height:96;source:"images/app_icon.png";smooth:true} Text {anchors.horizontalCenter:parent.horizontalCenter;text:"微博Lite";font.pixelSize:32;color:"#edf2f7"} Text {text:"Original app © Sina Weibo\nWeibo++ by Karin · Modified by Chih";horizontalAlignment:Text.AlignHCenter;font.pixelSize:16;color:"#94a3b2"}}
  Column {id:form;x:20;y:inPortrait?Math.min(485,Math.max(440,view.height-height-24)):185;width:parent.width-40;spacing:24
   ButtonRow {width:parent.width
    Button {text:"短信登录";checkable:true;checked:loginPage.smsMode;enabled:!loginPage.locked;onClicked:{loginPage.smsMode=true;passwordInput.text="";}}
    Button {text:"密码登录";checkable:true;checked:!loginPage.smsMode;enabled:!loginPage.locked;onClicked:{loginPage.smsMode=false;passwordInput.text="";}}
   }
   TextField {platformStyle:AppInputStyle {}
id:accountInput;width:parent.width;enabled:!loginPage.locked;placeholderText:smsMode?"手机号（其他地区加国家码）":"账号";inputMethodHints:smsMode?Qt.ImhDialableCharactersOnly:Qt.ImhNoPredictiveText;onActiveFocusChanged:{if(activeFocus)loginPage.showField(accountInput);}Keys.onReturnPressed:passwordInput.forceActiveFocus()}
   Item {width:parent.width;height:passwordInput.height
    TextField {platformStyle:AppInputStyle {}
id:passwordInput;objectName:"smsCodeField";width:smsMode?parent.width-170:parent.width;enabled:!loginPage.locked;placeholderText:smsMode?"短信验证码":"密码";echoMode:smsMode?TextInput.Normal:TextInput.Password;inputMethodHints:smsMode?Qt.ImhDigitsOnly:Qt.ImhNoPredictiveText;onActiveFocusChanged:{if(activeFocus)view.contentY=Math.max(0,form.y+parent.y+height-view.height+24);}Keys.onReturnPressed:loginPage.login()}
    Button {objectName:"smsSendButton";anchors.right:parent.right;width:160;anchors.verticalCenter:parent.verticalCenter;visible:smsMode;enabled:smsCountdown===0&&!loginPage.locked;text:smsCountdown>0?smsCountdown+" 秒":"获取验证码";onClicked:{loginPage.closeSoftKeyboard();loginModel.sendLoginSms(accountInput.text);}}
   }
   Button {anchors.horizontalCenter:parent.horizontalCenter;width:210;enabled:!loginPage.locked;text:"登录";onClicked:loginPage.login()}
  }
 }
 Button {text:"辅助登录";width:150;anchors.right:parent.right;anchors.rightMargin:20;y:12;enabled:!locked;onClicked:{loginPage.closeSoftKeyboard();loginPage.auxiliaryVisible=true;}}
 Loader {id:auxiliaryLoader;anchors.fill:parent;z:1500;source:auxiliaryVisible?"AuxiliaryLoginView.qml":""}
 Connections {target:auxiliaryLoader.item;onCancelled:loginPage.auxiliaryVisible=false;onImported:{loginPage.auxiliaryVisible=false;loginPage.loginFinished(200);}}

 Loader {id:sessionLoader;anchors.fill:parent;z:1001;focus:sessionVisible;source:sessionVisible&&loginPage.visible?"SessionSyncView.qml":"";onLoaded:loginPage.closeSoftKeyboard()}
 Connections {target:sessionLoader.item;onCancelled:loginModel.cancelVerification()}
 Loader {id:verificationLoader;anchors.fill:parent;z:1000;focus:verificationVisible;source:verificationVisible&&loginPage.visible?"VerificationView.qml":"";onLoaded:{item.challengeId=loginModel.verificationId;loginPage.closeSoftKeyboard();verificationFocus.restart();}}
 Connections {target:verificationLoader.item;onVerified:loginModel.completeVerification();onCancelled:loginModel.cancelVerification()}
 tools:addingAccount?backTools:null
 ToolBarLayout {id:backTools;visible:false;ToolIcon {iconId:(settingModel.darkMode?"icon-m-toolbar-back-white":"icon-m-toolbar-back");onClicked:{loginPage.closeSoftKeyboard();listPage.viewBack();}}}
}
