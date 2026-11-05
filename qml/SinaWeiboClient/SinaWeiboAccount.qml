import QtQuick 1.0
import "AppTheme.js" as AppTheme
import com.nokia.meego 1.0
import "controls"

Page {
 orientationLock:PageOrientation.LockPortrait;

    id: accountPage
    anchors.fill:parent
    property bool deleteMode: false
    property int delindex: 0

    Rectangle{
        anchors.fill: parent
        color: AppTheme.color("#f7f8fa",settingModel.darkMode)
    }

    AccTitleBar{
        id: accountPagePageTitle
        width: parent.width
        height: 65
        title:"账号管理"
        anchors.top: parent.top
        bgsource: "../images/restitlebg_green.png"
        z: flickable.z + 1

        SheetButton {
            visible:!accountPage.deleteMode;anchors.verticalCenter:parent.verticalCenter;anchors.right:parent.right;anchors.rightMargin:16;width:81;text:"管理";onClicked:accountPage.deleteMode=true;
        }

        SheetButton {
            id: finishBtn
            width: 81
            visible: accountPage.deleteMode
            anchors.right: parent.right
            anchors.rightMargin: 20
            anchors.verticalCenter: parent.verticalCenter
            text: "完成"
            onClicked: {
                accountPage.deleteMode = false;
            }
        }
    }

    Flickable {
        id: flickable
        anchors.fill: parent
        anchors.topMargin: 85
        contentWidth: accountList.width
        contentHeight: accountList.height
        flickableDirection: Flickable.VerticalFlick

        Column {
            id: accountList
            spacing: 15
            width:  flickable.width

            Component.onCompleted: {
                var count = children.length;
                for (var i = 0; i < count; i++) {
                    var item = children[i];
                    item.anchors.horizontalCenter = item.parent.horizontalCenter;
                }
            }

            Repeater {
                 model: accountModel

                 Rectangle{
                     width: flickable.width
                     height: 60
                     color: "transparent"
                     Button {
											 property bool isCurrent: loginModel.userID == accountModel.getAccountId(index); //k
                         anchors.left: parent.left;
                         anchors.leftMargin: 20
                         width: deleteMode ? parent.width - 150 : parent.width - 40
                         height: 60
                         text: userName
                         enabled: deleteMode ? false : true
                         iconSource: if(isCurrent && funcContainer.logined){"images/select-key_choose.png"}else{"images/select-key_unselected.png"}
                         onClicked: {
                             funcContainer.activateAccount(accountModel.getAccountToken(index),accountModel.getAccountSecret(index));
                         }

                         Image {
                             anchors.verticalCenter: parent.verticalCenter;
                             anchors.right: parent.right;
                             anchors.rightMargin: 20
                             smooth: true
                             opacity: deleteMode ? 0 : 1
                             source: "images/icon_arrow_gray.png"

                             Behavior on opacity{
                                 NumberAnimation{duration: 200}
                             }
                         }

                         Behavior on width{
                             NumberAnimation{duration: 200}
                         }
                     }

                     Button {
                         anchors.verticalCenter: parent.verticalCenter;
                         anchors.right: parent.right;
                         anchors.rightMargin: 20

                         width: deleteMode ? 90 : 0
                         height: 60
                         text: "删除"

                         onClicked: {
                             accountPage.delindex = index;
                             delquery.open();
                         }

                         Behavior on width{
                             NumberAnimation{duration: 200}
                         }
                     }
                 }
            }

            Button {
                id: btn2
                width: parent.width - 40
                height: 60
                text: "添加账号"
                onClicked: {
                    listPage.changePage(35);
                }
            }
        }
    }
    onVisibleChanged: {

        if(visible) {
            accountModel.getAccountList();
            console.log("onVisibleChanged");
        }
    }

    tools: ToolBarLayout {
        ToolIcon {
            visible: funcContainer.logined
            iconId: theme.inverted ? "icon-m-toolbar-back-white" : "icon-m-toolbar-back"; onClicked: listPage.viewBack();
        }
    }

    QueryDialog {
        id: delquery
        message: "确认是否删除？"
        acceptButtonText: "是"
        rejectButtonText: "否"
        property bool currentAccount: false
        onAccepted: {
            if(accountModel.getAccountId(accountPage.delindex) == loginModel.userID) {
                currentAccount = true;
            }

            console.log("currentAccount", currentAccount);

            accountModel.deleteAccount(accountModel.getAccountId(accountPage.delindex));
            accountModel.getAccountList();

            if(accountModel.getAccountCount() == 0) {
                newMsgNotification.stop();
//                funcContainer.needRelogin = true;
                funcContainer.hometabindex = 0;
                funcContainer.needRefresh = true;

                funcContainer.logined = false;
                firstPageModel.clearData();
                loginModel.clearData();

                listPage.changePage(1);
            }
            else {
                if(currentAccount){
                    funcContainer.logined = false;
                    funcContainer.needRefresh = true;
                    newMsgNotification.reset();
                    accountModel.login();
                }
            }
        }
    }
}
